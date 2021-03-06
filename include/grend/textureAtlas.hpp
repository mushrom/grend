#pragma once

#include <grend/quadtree.hpp>
#include <grend/glmIncludes.hpp>
#include <grend/openglIncludes.hpp>
#include <grend/glManager.hpp>
#include <memory>

namespace grendx {

class atlas {
	public:
		typedef std::shared_ptr<atlas> ptr;
		typedef std::weak_ptr<atlas> weakptr;

		enum mode {
			Color,
			Depth,
		};

		atlas(size_t dimension, enum mode m = mode::Color);
		bool bind_atlas_fb(quadtree::node_id id);
		glm::mat3 tex_matrix(quadtree::node_id id);
		glm::vec3 tex_vector(quadtree::node_id id);

		quadtree tree;
		Texture::ptr color_tex;
		Texture::ptr depth_tex;

		// framebuffer with texture as a backing
		Framebuffer::ptr framebuffer;
};

// namespace grendx
}
