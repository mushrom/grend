#pragma once

#include <grend/glm-includes.hpp>
#include <vector>
#include <string>

namespace grendx {

class scene {
	public:
		struct node {
			std::string name;
			glm::mat4 transform;
		};

		std::vector<struct node> nodes;
};

// namespace grendx
}