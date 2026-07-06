#pragma once

#include "shader.h"
#include "vao.h"
#include "framebuffer.h"
#include "model/Texture.h"

namespace gl
{
    class postfx
    {
    private:
        gl::program program;
        static std::shared_ptr<gl::shader> vertex;
        static std::shared_ptr<gl::vao> vao;

    public:
        postfx(const std::string &s);
        postfx(const shader &s);

        void attach();
        void apply(opengl_texture &src, framebuffer *dst);
        void apply(std::vector<opengl_texture*> src, framebuffer *dst);
        // like apply(), but draws the quad blended (straight alpha) over whatever
        // is already in dst instead of replacing it. intended for overlay/HUD style
        // compositing rather than full-frame post-processing effects.
        void apply_blend(opengl_texture &src, framebuffer *dst);
    };
}
