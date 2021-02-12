#include <grend/geometryGeneration.hpp>
#include <grend/gameEditor.hpp>
#include <stdlib.h>

#include "killedParticles.hpp"
#include "boxSpawner.hpp"
#include "player.hpp"
#include "timedLifetime.hpp"

// destructor for rtti
killedParticles::~killedParticles() {};

void killedParticles::onEvent(entityManager *manager, entity *ent, float delta) {
	//auto box = new player(manager, manager->engine, ent->node->transform.position + glm::vec3(0, 0, 1));
	auto box = new boxParticles(manager, ent->node->transform.position);

	manager->add(box);
}

boxParticles::boxParticles(entityManager *manager, glm::vec3 pos)
	: entity(manager)
{
	static gameObject::ptr model = nullptr;

	manager->registerComponent(this, "boxParticles", this);
	new timedLifetime(manager, this, 7.f);
	parts = std::make_shared<gameParticles>(32);

	for (unsigned i = 0; i < 32; i++) {
		velocities[i] = float(rand()) / RAND_MAX * 15.f;
		offsets[i]    = float(rand()) / RAND_MAX * 10.f;

		parts->positions[i] = (glm::mat4(1));
	}

	asdf = pos;
	parts->activeInstances = 32;
	parts->radius = 50.f;
	parts->update();

	if (!model) {
		/*
		gameModel::ptr mod = generate_cuboid(0.5, 0.5, 0.5);
		model = std::make_shared<gameObject>();
		compileModel("boxProjectile", mod);
		setNode("model", model, mod);
		*/
		model = loadScene("assets/obj/emissive-cube.glb");

		/*
		auto mesh = std::dynamic_pointer_cast<gameMesh>(mod->getNode("mesh"));

		if (mesh) {
			SDL_Log("adding a material to the farticles");
			mesh->meshMaterial = std::make_shared<material>();
			//mesh->meshMaterial->factors.emissive = {0.3, 0.5, 0.8, 1.0};
			mesh->meshMaterial->factors.emissive = {1, 1, 1, 1};
		}
		*/
	}

	setNode("model", parts, model);
	setNode("parts", node, parts);
};

void boxParticles::update(entityManager *manager, float delta) {
	time += delta;

	for (unsigned i = 0; i < 32; i++) {
		glm::vec3 pos =
			glm::vec3(sin(offsets[i] * time),
		              velocities[i] * time,
		              cos(offsets[i]*time))
			+ asdf;

		parts->positions[i] =
			glm::translate(pos)
			* glm::mat4_cast(glm::quat(glm::vec3(0, time, 0)))
			* glm::scale(glm::vec3(sin(time + offsets[i])));
	}

	parts->update();
}
