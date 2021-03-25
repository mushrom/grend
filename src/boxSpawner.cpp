#include "boxSpawner.hpp"
#include <grend/geometryGeneration.hpp>
#include <grend/audioMixer.hpp>
#include <grend/gameEditor.hpp>

static channelBuffers_ptr sfx = nullptr;

using namespace grendx;

boxBullet::boxBullet(entityManager *manager, gameMain *game, glm::vec3 position)
	: projectile(manager, game, position)
{
	static gameObject::ptr model = nullptr;

	manager->registerComponent(this, "boxBullet", this);

	if (!model) {
		// TODO: assets
		// TODO: resource manager
		//model = loadSceneCompiled("test-assets/obj/smoothcube.glb");
		gameModel::ptr mod = generate_cuboid(0.3, 0.3, 0.3);
		model = std::make_shared<gameObject>();
		compileModel("boxProjectile", mod);
		setNode("model", model, mod);
	}

	setNode("model", node, model);
}

void boxSpawner::handleInput(entityManager *manager, entity *ent, inputEvent& ev)
{
	if (ev.active && ev.type == inputEvent::types::primaryAction) {
		std::cerr << "boxSpawner::handleInput(): got here" << std::endl;
		glm::mat3 noderot = glm::mat3_cast(ent->node->transform.rotation);
		glm::vec3 playerrot = noderot*glm::vec3(0, 0, 1);

		//auto box = new boxBullet(manager, manager->engine, ent->node->transform.position + 3.f*ev.data);
		for (unsigned i = 0; i < 10; i++) {
			auto box = new boxBullet(manager, manager->engine, ent->node->transform.position + (2.f + i)*playerrot);

			rigidBody *body;
			castEntityComponent(body, manager, box, "rigidBody");

			if (body) {
				body->phys->setVelocity((30.f + 5*i) * playerrot);
				manager->add(box);
			}
		}

		//auto ch = std::make_shared<stereoAudioChannel>(sfx);
		//ch->worldPosition = position;
		//manager->engine->audio->add(ch);
	}
}

nlohmann::json boxSpawner::serialize(entityManager *manager) {
	// TODO: actually serialize
	return defaultProperties();
}
