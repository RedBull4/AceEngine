/**
 * @author Alexandr
 * @email alexandralibekov@yahoo.com
 * @create date 2020-10-28 14:48:48
 * @modify date 2020-10-31 23:05:24
 * @version 0.03
 */

#include "../header_files/AceEngine.hpp"
#include <stdexcept>
using namespace ACE;

bool CAN_DRAW_OBJECTS = false;

window::window()
{
    SDL_Init(SDL_INIT_VIDEO);
}

void window::init_gl()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    _window_data.gl_context = SDL_GL_CreateContext(_window);

    SDL_GL_SetSwapInterval(SDL_FALSE);
}

void window::create(ACE_STRING title, vector4<int> bounds)
{
    _window = SDL_CreateWindow(title, bounds.x,
                               bounds.y, bounds.w,
                               bounds.h, SDL_WINDOW_OPENGL);

    init_gl();

    _window_data.bounds = bounds;
}

void window::create(ACE_STRING title, uint16_t position, vector2<int> size)
{
    switch (position)
    {
    case ACE_WINDOW_POS_CENTERED:
        _window_data.bounds.x = screen::get_size().x / 2 - size.x / 2;
        _window_data.bounds.y = screen::get_size().y / 2 - size.y / 2;
        break;
    case ACE_WINDOW_POS_TOP_LEFT:
        _window_data.bounds.x = 0;
        _window_data.bounds.y = 0;
        break;
    case ACE_WINDOW_POS_TOP_RIGHT:
        _window_data.bounds.x = screen::get_size().x - size.x;
        _window_data.bounds.y = 0;
        break;
    case ACE_WINDOW_POS_BOTTOM_LEFT:
        _window_data.bounds.x = 0;
        _window_data.bounds.y = screen::get_size().y - size.y;
        break;
    case ACE_WINDOW_POS_BOTTOM_RIGHT:
        _window_data.bounds.x = screen::get_size().x - size.x;
        _window_data.bounds.y = screen::get_size().y - size.y;
        break;
    }

    _window = SDL_CreateWindow(title, _window_data.bounds.x,
                               _window_data.bounds.y, size.x,
                               size.y, SDL_WINDOW_OPENGL);

    init_gl();

    _window_data.bounds.w = size.x;
    _window_data.bounds.h = size.y;
}

void window::set_flags(ACE_FLAGS window_flags)
{
    switch (window_flags)
    {
    case ACE_WINDOW_NO_FLAGS:
        SDL_SetWindowFullscreen(_window, SDL_FALSE);
        SDL_SetWindowResizable(_window, SDL_FALSE);
        break;
    case ACE_WINDOW_FULLSCREEN:
        SDL_SetWindowSize(_window, screen::get_size().x,
                          screen::get_size().y);
        SDL_SetWindowFullscreen(_window, SDL_TRUE);
        break;
    case ACE_WINDOW_RESIZABLE:
        SDL_SetWindowResizable(_window, SDL_TRUE);
        break;
    }
}

void window::set_fill_color(rgba_color fill_color)
{
    _window_data.fill_color = fill_color;
}

void window::close()
{
    SDL_DestroyWindow(_window);
    _window = NULL;
}

window &window::operator=(SDL_Window *_window)
{
    this->_window = _window;

    return *this;
}

bool window::operator==(SDL_Window *_window)
{
    return this->_window == _window;
}

bool window::operator!=(SDL_Window *_window)
{
    return this->_window != _window;
}

bool window::quit(SDL_Event event)
{
    if (event.type == SDL_WINDOWEVENT &&
        event.window.event == SDL_WINDOWEVENT_CLOSE)
    {
        if (_window != NULL &&
            SDL_GetWindowFromID(event.window.windowID) == _window)
            return true;
        else
            return false;
    }

    return false;
}

bool window::is_open()
{
    return _window != NULL;
}

void update_window_data(SDL_Window *_window, window_data &_window_data)
{
    SDL_GetWindowSize(_window, &_window_data.bounds.w, &_window_data.bounds.h);
    SDL_GetWindowPosition(_window, &_window_data.bounds.x, &_window_data.bounds.y);
}

void window::clear()
{
    update_window_data(_window, _window_data);

    glClearColor(_window_data.fill_color.r != 0 ? _window_data.fill_color.r / 255 : 0,
                 _window_data.fill_color.g != 0 ? _window_data.fill_color.g / 255 : 0,
                 _window_data.fill_color.b != 0 ? _window_data.fill_color.b / 255 : 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, _window_data.bounds.w, _window_data.bounds.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, _window_data.bounds.w, _window_data.bounds.h, 0, -10, 10);

    CAN_DRAW_OBJECTS = true;
}

void window::display()
{
    CAN_DRAW_OBJECTS = false;

    SDL_GL_SwapWindow(_window);
}

SDL_Window *window::translate_to_sdl()
{
    return _window;
}

SDL_GLContext window::get_gl_context()
{
    return _window_data.gl_context;
}

vector4<int> window::get_bounds()
{
    return _window_data.bounds;
}

void polygon::set_point_count(uint64_t count)
{
    for (int i = 0; i < count; i++)
        _polygon_data.point_position.push_back(vector2<float>(0, 0));
}

void polygon::set_point_position(uint64_t id, vector2<float> position)
{
    _polygon_data.point_position.at(id) = position;

    _polygon_data.bounds.w = std::max(position.x, _polygon_data.bounds.w);
    _polygon_data.bounds.h = std::max(position.y, _polygon_data.bounds.h);
}

void polygon::set_fill_color(rgba_color fill_color)
{
    _polygon_data.fill_color = fill_color;
}

void polygon::begin()
{
    if (CAN_DRAW_OBJECTS)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glTranslatef(_polygon_data.bounds.x + _polygon_data.center.x,
                     _polygon_data.bounds.y + _polygon_data.center.y, 0);
        glRotatef(_polygon_data.angle, 0, 0, 1);
        glTranslatef(-(_polygon_data.bounds.x + _polygon_data.center.x),
                     -(_polygon_data.bounds.y + _polygon_data.center.y), 0);

        glTranslatef(_polygon_data.bounds.x,
                     _polygon_data.bounds.y, 0);
        glBegin(GL_POLYGON);
    }
}

void polygon::end()
{
    if (CAN_DRAW_OBJECTS)
    {
        glEnd();

        glDisable(GL_BLEND);
    }
}

void polygon::set_position(vector2<float> position)
{
    _polygon_data.bounds.x = position.x;
    _polygon_data.bounds.y = position.y;
}

void polygon::translate_point_to_vertex(uint64_t id)
{
    _polygon_data.point_position.at(id).translate_to_vertex();
}

void polygon::show_unfilled()
{
    glPushMatrix();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    begin();
    {
        glColor4ub(_polygon_data.fill_color.r,
                   _polygon_data.fill_color.g,
                   _polygon_data.fill_color.b,
                   _polygon_data.fill_color.a);
        for (int i = 0; i < _polygon_data.point_position.size(); i++)
            translate_point_to_vertex(i);
    }
    end();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPopMatrix();
}

void polygon::show_filled()
{
    glPushMatrix();

    begin();
    {
        glColor4ub(_polygon_data.fill_color.r,
                   _polygon_data.fill_color.g,
                   _polygon_data.fill_color.b,
                   _polygon_data.fill_color.a);
        for (int i = 0; i < _polygon_data.point_position.size(); i++)
            translate_point_to_vertex(i);
    }
    end();

    glPopMatrix();
}

void polygon::set_rotation(float angle)
{
    _polygon_data.angle = angle;
}

void polygon::rotate(float angle)
{
    _polygon_data.angle += angle;
}

void polygon::set_center(vector2<float> center)
{
    _polygon_data.center = center;
}

vector4<float> polygon::get_bounds()
{
    return _polygon_data.bounds;
}