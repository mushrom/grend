#include <grend-config.h>

#include <grend/gameMain.hpp>
#include <grend/timers.hpp>

#include <string.h> // memset

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#if defined(PHYSICS_BULLET)
#include <grend/bulletPhysics.hpp>
#elif defined(PHYSICS_IMP)
#include <grend/impPhysics.hpp>
#endif

using namespace grendx;

// non-pure virtual destructors for rtti
gameMain::~gameMain() {};

gameMain::gameMain(std::string name)
	: ctx(name.c_str())
{
	initializeOpengl();

#if defined(PHYSICS_BULLET)
	phys   = std::dynamic_pointer_cast<physics>(std::make_shared<bulletPhysics>());
#elif defined(PHYSICS_IMP)
	phys   = std::dynamic_pointer_cast<physics>(std::make_shared<impPhysics>());
#else
#error "No physics implementation defined!"
#endif

	state  = std::make_shared<gameState>();
	rend   = std::make_shared<renderContext>(ctx);
	audio  = std::make_shared<audioMixer>(ctx);
	jobs   = std::make_shared<jobQueue>();

	entities  = std::make_shared<ecs::entityManager>(this);
	factories = std::make_shared<ecs::factories>();
}

void gameMain::clearMetrics(void) {
	metrics.drawnMeshes = 0;
}

int gameMain::step(void) {
	// TODO: kinda disperate set of metrics, should unify/clean this up
	frame_timer.stop();
	frame_timer.start();
	clearMetrics();
	profile::newFrame();
	handleInput();

	if (running) {
		// XXX: remove
		Uint32 cur_ticks = SDL_GetTicks();
		Uint32 ticks_delta = cur_ticks - last_frame_time;
		float fticks = ticks_delta / 1000.0f;
		last_frame_time = cur_ticks;

		if (view == nullptr) {
			SDL_Log(
				"ERROR: no view defined, you must set a view controller "
				"with gameMain::setView()"
			);
			running = false;
		}

		profile::startGroup("View");
		view->logic(this, fticks);
		profile::endGroup();

		profile::startGroup("Syncronous jobs");
		if (jobs != nullptr) {
			// spread long-running syncronous job batches across multiple frames
			// TODO: more fine-grained than SDL_GetTicks()
			//       could use std::chrono high res clock
			uint32_t start = SDL_GetTicks();
			uint32_t ticks = start;
			// TODO: configurable max time
			uint32_t maxTime = 2 /*ms*/;

			bool running = true;

			while (running && ticks - start < maxTime) {
				running = jobs->runSingleDeferred();
				ticks = SDL_GetTicks();
			}

			if (ticks - start >= maxTime) {
				SDL_Log("Exceeded time limit for deferred job!");
			}

			// left here for debugging, just in case
			//jobs->runDeferred();
		}
		profile::endGroup();

		profile::startGroup("Render");
		setDefaultGlFlags();
		rend->framebuffer->clear();
		view->render(this);

		if (phys) {
			phys->drawDebug(view->cam->viewProjTransform());
		}

		SDL_GL_SwapWindow(ctx.window);
		profile::endGroup();
	}

	profile::endFrame();
	return running;
}

#ifdef __EMSCRIPTEN__
// non-member step function for emscripten
static int render_step(double time, void *data) {
	gameMain *game = static_cast<gameMain*>(data);
	return game->step();
}
#endif

int gameMain::run(void) {
	running = true;

#ifdef __EMSCRIPTEN__
	emscripten_request_animation_frame_loop(&render_step, this);

#else
	while (running) {
		step();
	}
#endif

	return false;
}

void gameMain::setView(std::shared_ptr<gameView> nview) {
	view = nview;

	if (audio != nullptr && view != nullptr && view->cam != nullptr) {
		audio->setCamera(view->cam);
	}
}

void gameMain::handleInput(void) {
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT) {
			running = false;
			return;
		}

		if (view != nullptr) {
			view->handleInput(this, ev);
		}
	}
}

void grendx::renderWorld(gameMain *game, camera::ptr cam, renderFlags& flags) {
	renderQueue que(cam);
	renderWorld(game, cam, que, flags);
}

void grendx::renderWorld(gameMain *game,
                         camera::ptr cam,
                         renderQueue& base,
                         renderFlags& flags)
{
	profile::startGroup("renderWorld()");
	assert(game->rend && game->rend->framebuffer);

	int winsize_x, winsize_y;
	SDL_GetWindowSize(game->ctx.window, &winsize_x, &winsize_y);

	if (game->state->rootnode) {
		renderQueue que(base);
		que.setCamera(cam);
		float fticks = SDL_GetTicks() / 1000.0f;

		DO_ERROR_CHECK();

		profile::startGroup("Build queue");
		que.add(game->state->rootnode, fticks);
		que.add(game->state->physObjects);

		profile::startGroup("Update lights/shadowmaps");
		que.updateLights(game->rend);
		que.updateReflections(game->rend);
		profile::endGroup();
		profile::endGroup();
		DO_ERROR_CHECK();

		game->rend->framebuffer->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		DO_ERROR_CHECK();

		flags.mainShader->bind();
		flags.mainShader->set("time_ms", SDL_GetTicks() * 1.f);
		DO_ERROR_CHECK();

		profile::startGroup("Cull");
		que.cull(game->rend->framebuffer->width,
		         game->rend->framebuffer->height,
		         game->rend->lightThreshold);
		profile::endGroup();

		profile::startGroup("Sort");
		que.sort();
		profile::endGroup();

		//profile::startGroup("Batch");
		//que.batch();
		//profile::endGroup();

		profile::startGroup("Build light tilemap");
		buildTilemap(que, game->rend);
		profile::endGroup();

		profile::startGroup("Draw");
		que.updateReflectionProbe(game->rend);
		game->metrics.drawnMeshes +=
			que.flush(game->rend->framebuffer, game->rend, flags);
		DO_ERROR_CHECK();
		game->rend->defaultSkybox.draw(cam, game->rend->framebuffer);
		DO_ERROR_CHECK();
		profile::endGroup();
	}

	profile::endGroup();
}
