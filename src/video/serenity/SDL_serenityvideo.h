/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#ifndef SDL_serenityvideo_h_
#define SDL_serenityvideo_h_

#include "../SDL_sysvideo.h"

#include <LibGUI/Widget.h>
#include <LibGUI/Window.h>

class SerenitySDLWidget final : public GUI::Widget {
    C_OBJECT(SerenitySDLWidget)
public:
    explicit SerenitySDLWidget(SDL_Window*);
    RefPtr<Gfx::Bitmap> m_buffer;

protected:
    void paint_event(GUI::PaintEvent&) override;
    void resize_event(GUI::ResizeEvent&) override;
    void show_event(GUI::ShowEvent&) override;
    void hide_event(GUI::HideEvent&) override;

    void mousedown_event(GUI::MouseEvent&) override;
    void mousemove_event(GUI::MouseEvent&) override;
    void mouseup_event(GUI::MouseEvent&) override;

    void keydown_event(GUI::KeyEvent& event) override;
    void keyup_event(GUI::KeyEvent& event) override;

    void enter_event(Core::Event&) override;
    void leave_event(Core::Event&) override;

private:
    SDL_Window* m_sdl_window = nullptr;
};

class SerenityPlatformWindow final {
public:
    SerenityPlatformWindow(SDL_Window* sdl_window)
        : m_window(GUI::Window::construct())
        , m_widget(SerenitySDLWidget::construct(sdl_window))
    {
        m_window->resize(sdl_window->w, sdl_window->h);
        m_window->set_resizable(false);
    }

    static SerenityPlatformWindow* from_sdl_window(SDL_Window* window)
    {
        return static_cast<SerenityPlatformWindow*>(window->driverdata);
    }

    NonnullRefPtr<GUI::Window> window() { return m_window; }
    NonnullRefPtr<SerenitySDLWidget> widget() { return m_widget; }

private:
    NonnullRefPtr<GUI::Window> m_window;
    NonnullRefPtr<SerenitySDLWidget> m_widget;
};

#endif /* SDL_serenityvideo_h_ */

/* vi: set ts=4 sw=4 expandtab: */
