#include <grend/renderFramebuffer.hpp>

using namespace grendx;

renderFramebuffer::renderFramebuffer(int Width, int Height)
	: width(Width), height(Height)
{
	framebuffer = gen_framebuffer();
	framebuffer->bind();
	color = framebuffer->attach(GL_COLOR_ATTACHMENT0,
	            gen_texture_color(width, height, rgbaf_if_supported()));
	depth = framebuffer->attach(GL_DEPTH_STENCIL_ATTACHMENT,
	            gen_texture_depth_stencil(width, height));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		SDL_Die("incomplete!");
	}
}

renderFramebuffer::renderFramebuffer(Framebuffer::ptr fb, int Width, int Height)
	: width(Width), height(Height)
{
	framebuffer = fb;
}

void renderFramebuffer::clear(void) {
	drawn_meshes.clear();
	framebuffer->bind();
	glClearStencil(0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DO_ERROR_CHECK();
}

gameMesh::ptr renderFramebuffer::index(float x, float y) {
	// TODO: use these at some point
	//GLbyte color[4];
	//GLfloat depth;
	GLubyte idx;

	// adjust coordinates for resolution scaling
	unsigned adx = width * x * scale.x;
	unsigned ady = (height - height*y - 1) * scale.y;

	framebuffer->bind();
	//glReadPixels(adx, ady, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
	//glReadPixels(adx, ady, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	glReadPixels(adx, ady, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &idx);
	DO_ERROR_CHECK();

	return index(idx);
}

gameMesh::ptr renderFramebuffer::index(unsigned idx) {
	if (idx > 0 && idx <= drawn_meshes.size()) {
		puts("got here");
		return drawn_meshes[idx - 1];
	}

	return nullptr;
}