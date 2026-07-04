#include "stdafx.h"
#include "ubo.h"

gl::ubo::ubo(size_t size, int idx, GLenum hint)
{
    m_hint = hint;
    allocate(buffer::UNIFORM_BUFFER, size, hint);
    index = idx;
    bind_uniform();
}

void gl::ubo::bind_uniform()
{
    bind_base(buffer::UNIFORM_BUFFER, index);
}

void gl::ubo::bind_uniform_range(GLintptr offset, GLsizeiptr size)
{
    bind_base_range(buffer::UNIFORM_BUFFER, index, offset, size);
}

void gl::ubo::update(const uint8_t *data, int offset, GLsizeiptr size)
{
    upload(buffer::UNIFORM_BUFFER, data, offset, size);
}

void gl::ubo::init_ring(size_t element_size, size_t slot_count)
{
    GLint alignment = 256;
    ::glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    if( alignment < 1 ) { alignment = 1; }

    m_slot_stride = ( ( static_cast<GLintptr>( element_size ) + alignment - 1 ) / alignment ) * alignment;
    m_slot_count = std::max<size_t>( 1, slot_count );
    m_cursor = 0;
    m_lap = 0;
    m_wrap_fences.clear();
    m_wrap_fences.resize( 2 );

    allocate( buffer::UNIFORM_BUFFER, m_slot_stride * static_cast<GLsizeiptr>( m_slot_count ), m_hint );
}

void gl::ubo::grow_ring(size_t new_slot_count)
{
    m_slot_count = std::max( new_slot_count, m_slot_count + 1 );
    m_cursor = 0;
    m_lap = 0;
    for( auto &f : m_wrap_fences ) { f.reset(); }

    allocate( buffer::UNIFORM_BUFFER, m_slot_stride * static_cast<GLsizeiptr>( m_slot_count ), m_hint );
}

void gl::ubo::update_ring_bytes(const uint8_t *data, size_t size)
{
    if( m_slot_count <= 1 ) {
        update( data, 0, static_cast<GLsizeiptr>( size ) );
        bind_uniform();
        return;
    }

    if( m_cursor == 0 ) {
        auto &guard = m_wrap_fences[ m_lap % m_wrap_fences.size() ];
        if( guard && !guard->is_signalled() ) {
            grow_ring( m_slot_count * 2 );
        }
        else {
            guard = std::make_unique<fence>();
            ++m_lap;
        }
    }

    GLintptr const offset = static_cast<GLintptr>( m_cursor ) * m_slot_stride;
    upload( buffer::UNIFORM_BUFFER, data, static_cast<int>( offset ), static_cast<GLsizeiptr>( size ) );
    bind_uniform_range( offset, static_cast<GLsizeiptr>( size ) );

    m_cursor = ( m_cursor + 1 ) % m_slot_count;
}
