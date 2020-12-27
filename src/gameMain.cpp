#include <grend/gameMain.hpp>
#include <string.h> // memset

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

using namespace grendx;

void gameMain::clearMetrics(void) {
	metrics.drawnMeshes = 0;
}

int gameMain::step(void) {
	frame_timer.stop();
	frame_timer.start();
	clearMetrics();
	handleInput();

	if (running) {
		// XXX: remove
		Uint32 cur_ticks = SDL_GetTicks();
		Uint32 ticks_delta = cur_ticks - last_frame_time;
		float fticks = ticks_delta / 1000.0f;
		last_frame_time = cur_ticks;

		if (view == nullptr) {
			std::cerr <<
				"ERROR: no view defined, you must set a view controller "
				"with gameMain::setView()"
				<< std::endl;
			running = false;
		}

		view->logic(this, fticks);
		if (jobs != nullptr) {
			jobs->runDeferred();
		}

		setDefaultGlFlags();
		rend->framebuffer->clear();
		view->render(this);

		SDL_GL_SwapWindow(ctx.window);
	}

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

void gameMain::step_physics(void) {
	float dt = 1.0/frame_timer.last();
	phys->stepSimulation(dt);
}

void gameMain::logic(void) {

}

void gameMain::setView(std::shared_ptr<gameView> nview) {
	view = nview;

	if (audio != nullptr && view != nullptr && view->cam != nullptr) {
		audio->setCamera(view->cam);
	}
}

void grendx::renderWorld(gameMain *game, camera::ptr cam, renderFlags& flags) {
	assert(game->rend && game->rend->framebuffer);

	int winsize_x, winsize_y;
	SDL_GetWindowSize(game->ctx.window, &winsize_x, &winsize_y);

	if (game->state->rootnode) {
		renderQueue que(cam);
		float fticks = SDL_GetTicks() / 1000.0f;

		que.add(game->state->rootnode, fticks);
		que.add(game->state->physObjects);
		que.updateLights(game->rend);
		que.updateReflections(game->rend);
		DO_ERROR_CHECK();

		game->rend->framebuffer->framebuffer->bind();
		DO_ERROR_CHECK();

		// TODO: constants for texture bindings, no magic numbers floating around
		flags.mainShader->bind();
		flags.mainShader->set("time_ms", SDL_GetTicks() * 1.f);
		DO_ERROR_CHECK();

		que.cull(game->rend->framebuffer->width,
		         game->rend->framebuffer->height,
		         game->rend->lightThreshold);
		que.sort();
		buildTilemap(que, game->rend);
		que.updateReflectionProbe(game->rend);
		game->metrics.drawnMeshes +=
			que.flush(game->rend->framebuffer, game->rend, flags);
		game->rend->defaultSkybox.draw(cam, game->rend->framebuffer);
	}
}
