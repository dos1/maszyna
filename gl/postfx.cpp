#include "stdafx.h"

#include "postfx.h"

std::shared_ptr<gl::shader> gl::postfx::vertex;
std::shared_ptr<gl::vao> gl::postfx::vao;

gl::postfx::postfx(const std::string &s) : postfx(shader("postfx_" + s + ".frag"))
{
}

gl::postfx::postfx(const shader &s)
{
    if (!vertex)
        vertex = std::make_shared<gl::shader>("quad.vert");
    if (!vao)
        vao = std::make_shared<gl::vao>();

    program.attach(*vertex);
    program.attach(s);
    program.link();
}

void gl::postfx::apply(opengl_texture &src, framebuffer *dst)
{
    apply({&src}, dst);
}

void gl::postfx::apply(std::vector<opengl_texture *> src, framebuffer *dst)
{
    if (dst)
    {
        dst->clear(GL_COLOR_BUFFER_BIT);
        dst->bind();
    }
    else
        framebuffer::unbind();


    glClear(GL_COLOR_BUFFER_BIT);
    program.bind();
    vao->bind();

    size_t unit = 0;
    for (opengl_texture *tex : src)
        tex->bind(unit++);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gl::postfx::apply_blend(opengl_texture &src, framebuffer *dst)
{
    if (dst)
        dst->bind();
    else
        framebuffer::unbind();

    program.bind();
    vao->bind();

    src.bind(0);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisable(GL_BLEND);
}
