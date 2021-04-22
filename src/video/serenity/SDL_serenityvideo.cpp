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
extern "C" {

#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_SERENITY

#    include "../../events/SDL_events_c.h"
#    include "../SDL_pixels_c.h"
#    include "../SDL_sysvideo.h"
#    include "SDL_mouse.h"
#    include "SDL_video.h"
#    include "../../../include/SDL_scancode.h"
}

#    include "SDL_serenityevents_c.h"
#    include "SDL_serenityvideo.h"

#    include <LibCore/EventLoop.h>
#    include <LibGUI/Application.h>
#    include <LibGUI/Painter.h>
#    include <LibGUI/Widget.h>
#    include <LibGUI/Window.h>
#    include <LibGfx/Bitmap.h>

static int conversion_map[] = {
    SDLK_UNKNOWN,
    SDLK_ESCAPE,
    SDLK_TAB,
    SDLK_BACKSPACE,
    SDLK_RETURN,
    SDLK_INSERT,
    SDLK_DELETE,
    SDLK_PRINTSCREEN,
    SDLK_SYSREQ,
    SDLK_HOME,
    SDLK_END,
    SDLK_LEFT,
    SDLK_UP,
    SDLK_RIGHT,
    SDLK_DOWN,
    SDLK_PAGEUP,
    SDLK_PAGEDOWN,
    SDLK_LSHIFT,
    SDLK_RSHIFT,
    SDLK_LCTRL,
    SDLK_LALT,
    SDLK_CAPSLOCK,
    SDLK_NUMLOCKCLEAR,
    SDLK_SCROLLLOCK,
    SDLK_F1,
    SDLK_F2,
    SDLK_F3,
    SDLK_F4,
    SDLK_F5,
    SDLK_F6,
    SDLK_F7,
    SDLK_F8,
    SDLK_F9,
    SDLK_F10,
    SDLK_F11,
    SDLK_F12,
    SDLK_SPACE,
    SDLK_EXCLAIM,
    SDLK_QUOTEDBL,
    SDLK_HASH,
    SDLK_DOLLAR,
    SDLK_PERCENT,
    SDLK_AMPERSAND,
    SDLK_QUOTE,
    SDLK_LEFTPAREN,
    SDLK_RIGHTPAREN,
    SDLK_ASTERISK,
    SDLK_PLUS,
    SDLK_COMMA,
    SDLK_MINUS,
    SDLK_PERIOD,
    SDLK_SLASH,
    SDLK_0,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_5,
    SDLK_6,
    SDLK_7,
    SDLK_8,
    SDLK_9,
    SDLK_COLON,
    SDLK_SEMICOLON,
    SDLK_LESS,
    SDLK_EQUALS,
    SDLK_GREATER,
    SDLK_QUESTION,
    SDLK_AT,
    SDLK_a,
    SDLK_b,
    SDLK_c,
    SDLK_d,
    SDLK_e,
    SDLK_f,
    SDLK_g,
    SDLK_h,
    SDLK_i,
    SDLK_j,
    SDLK_k,
    SDLK_l,
    SDLK_m,
    SDLK_n,
    SDLK_o,
    SDLK_p,
    SDLK_q,
    SDLK_r,
    SDLK_s,
    SDLK_t,
    SDLK_u,
    SDLK_v,
    SDLK_w,
    SDLK_x,
    SDLK_y,
    SDLK_z,
    SDLK_LEFTBRACKET,
    SDLK_RIGHTBRACKET,
    SDLK_BACKSLASH,
    SDLK_UNKNOWN,
    SDLK_UNDERSCORE,
    SDLK_KP_LEFTBRACE,
    SDLK_KP_RIGHTBRACE,
    SDLK_KP_VERTICALBAR,
    SDLK_BACKQUOTE,
    SDLK_BACKQUOTE,
    SDLK_UNKNOWN,
};

static SDL_Scancode scancode_map[] = {
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_ESCAPE,
    SDL_SCANCODE_TAB,
    SDL_SCANCODE_BACKSPACE,
    SDL_SCANCODE_RETURN,
    SDL_SCANCODE_INSERT,
    SDL_SCANCODE_DELETE,
    SDL_SCANCODE_PRINTSCREEN,
    SDL_SCANCODE_SYSREQ,
    SDL_SCANCODE_HOME,
    SDL_SCANCODE_END,
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_PAGEUP,
    SDL_SCANCODE_PAGEDOWN,
    SDL_SCANCODE_LSHIFT,
    SDL_SCANCODE_RSHIFT,
    SDL_SCANCODE_LCTRL,
    SDL_SCANCODE_LALT,
    SDL_SCANCODE_CAPSLOCK,
    SDL_SCANCODE_NUMLOCKCLEAR,
    SDL_SCANCODE_SCROLLLOCK,
    SDL_SCANCODE_F1,
    SDL_SCANCODE_F2,
    SDL_SCANCODE_F3,
    SDL_SCANCODE_F4,
    SDL_SCANCODE_F5,
    SDL_SCANCODE_F6,
    SDL_SCANCODE_F7,
    SDL_SCANCODE_F8,
    SDL_SCANCODE_F9,
    SDL_SCANCODE_F10,
    SDL_SCANCODE_F11,
    SDL_SCANCODE_F12,
    SDL_SCANCODE_SPACE,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_APOSTROPHE,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_COMMA,
    SDL_SCANCODE_MINUS,
    SDL_SCANCODE_PERIOD,
    SDL_SCANCODE_SLASH,
    SDL_SCANCODE_0,
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_4,
    SDL_SCANCODE_5,
    SDL_SCANCODE_6,
    SDL_SCANCODE_7,
    SDL_SCANCODE_8,
    SDL_SCANCODE_9,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_SEMICOLON,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_EQUALS,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_A,
    SDL_SCANCODE_B,
    SDL_SCANCODE_C,
    SDL_SCANCODE_D,
    SDL_SCANCODE_E,
    SDL_SCANCODE_F,
    SDL_SCANCODE_G,
    SDL_SCANCODE_H,
    SDL_SCANCODE_I,
    SDL_SCANCODE_J,
    SDL_SCANCODE_K,
    SDL_SCANCODE_L,
    SDL_SCANCODE_M,
    SDL_SCANCODE_N,
    SDL_SCANCODE_O,
    SDL_SCANCODE_P,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_R,
    SDL_SCANCODE_S,
    SDL_SCANCODE_T,
    SDL_SCANCODE_U,
    SDL_SCANCODE_V,
    SDL_SCANCODE_W,
    SDL_SCANCODE_X,
    SDL_SCANCODE_Y,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_LEFTBRACKET,
    SDL_SCANCODE_RIGHTBRACKET,
    SDL_SCANCODE_BACKSLASH,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_KP_LEFTBRACE,
    SDL_SCANCODE_KP_RIGHTBRACE,
    SDL_SCANCODE_KP_VERTICALBAR,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_GRAVE,
    SDL_SCANCODE_UNKNOWN,

};

/* Initialization/Query functions */
static int SERENITY_VideoInit(_THIS);
static int SERENITY_SetDisplayMode(_THIS, SDL_VideoDisplay* display,
    SDL_DisplayMode* mode);
static void SERENITY_VideoQuit(_THIS);

/* SERENITY driver bootstrap functions */

static int SERENITY_Available(void) { return (1); }

static void SERENITY_DeleteDevice(SDL_VideoDevice* device)
{
    dbgln("SERENITY_DeleteDevice");
    SDL_free(device);
}

extern int Serenity_CreateWindow(_THIS, SDL_Window* window);
extern void Serenity_ShowWindow(_THIS, SDL_Window* window);
extern void Serenity_HideWindow(_THIS, SDL_Window* window);
extern void Serenity_SetWindowTitle(_THIS, SDL_Window* window);
extern void Serenity_SetWindowSize(_THIS, SDL_Window* window);
extern void Serenity_SetWindowFullscreen(_THIS, SDL_Window* window,
    SDL_VideoDisplay* display,
    SDL_bool fullscreen);
extern void Serenity_DestroyWindow(_THIS, SDL_Window* window);
extern int Serenity_CreateWindowFramebuffer(_THIS, SDL_Window* window,
    Uint32* format, void** pixels,
    int* pitch);
extern int Serenity_UpdateWindowFramebuffer(_THIS, SDL_Window* window,
    const SDL_Rect* rects,
    int numrects);
extern void Serenity_DestroyWindowFramebuffer(_THIS, SDL_Window* window);

static SDL_VideoDevice* SERENITY_CreateDevice(int devindex)
{
    SDL_VideoDevice* device;

    dbgln("SERENITY_CreateDevice {}", devindex);
    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice*)SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        return (0);
    }
    device->is_dummy = SDL_FALSE;

    /* Set the function pointers */
    device->VideoInit = SERENITY_VideoInit;
    device->VideoQuit = SERENITY_VideoQuit;
    device->SetDisplayMode = SERENITY_SetDisplayMode;
    device->PumpEvents = SERENITY_PumpEvents;

    device->CreateWindowFramebuffer = Serenity_CreateWindowFramebuffer;
    device->UpdateWindowFramebuffer = Serenity_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = Serenity_DestroyWindowFramebuffer;
    device->CreateSDLWindow = Serenity_CreateWindow;
    device->ShowWindow = Serenity_ShowWindow;
    device->HideWindow = Serenity_HideWindow;
    device->SetWindowTitle = Serenity_SetWindowTitle;
    device->SetWindowSize = Serenity_SetWindowSize;
    device->SetWindowFullscreen = Serenity_SetWindowFullscreen;
    device->DestroyWindow = Serenity_DestroyWindow;

    device->free = SERENITY_DeleteDevice;

    return device;
}

VideoBootStrap SERENITYVIDEO_bootstrap = { "serenity", "SDL serenity video driver", SERENITY_CreateDevice };

static RefPtr<GUI::Application> g_app;

// TODO: Ask kling about being able to query this from GWindow!
struct ScreenMode {
    int width;
    int height;
};

static ScreenMode modes[] = {
    { 640, 480 }, { 800, 600 }, { 1024, 768 }, { 1280, 1024 }, { 1920, 1080 }
};

int SERENITY_VideoInit(_THIS)
{
    VERIFY(!g_app);
    g_app = GUI::Application::construct(0, nullptr);
    SDL_DisplayMode mode;

    dbgln("SDL2: Initialising SDL application");

    for (int i = 0; i < 5; i++) {
        SDL_DisplayMode mode;
        mode.format = SDL_PIXELFORMAT_RGB888;
        mode.w = modes[i].width;
        mode.h = modes[i].height;
        mode.refresh_rate = 60;
        mode.driverdata = nullptr;

        if (SDL_AddBasicVideoDisplay(&mode) < 0) {
            return -1;
        }

        SDL_AddDisplayMode(&_this->displays[0], &mode);
    }

    /* We're done! */
    return 0;
}

static int SERENITY_SetDisplayMode(_THIS, SDL_VideoDisplay* display,
    SDL_DisplayMode* mode)
{
    dbgln("SERENITY_SetDisplayMode");
    return 0;
}

void SERENITY_VideoQuit(_THIS)
{
    dbgln("SERENITY_VideoQuit");
    g_app->quit();
}

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

SerenitySDLWidget::SerenitySDLWidget(SDL_Window* window)
    : m_sdl_window(window)
{
    SDL_Keycode keymap[SDL_NUM_SCANCODES];
    memset(keymap, 0, sizeof(keymap));

    for (int i = 0; i < (sizeof(scancode_map) / sizeof(SDL_Keycode)); ++i) {
        if (scancode_map[i] != SDL_SCANCODE_UNKNOWN)
            keymap[scancode_map[i]] = conversion_map[i];
    }
    SDL_SetKeymap(0, keymap, SDL_NUM_SCANCODES);

    update();
}

void SerenitySDLWidget::paint_event(GUI::PaintEvent& event)
{
    VERIFY(size() == m_buffer->size());
    if (size() != m_buffer->size()) {
        return; // can't paint this
    }
    GUI::Painter painter(*this);
    painter.add_clip_rect(event.rect());
    painter.blit(event.rect().location(), *m_buffer, event.rect());
}
void SerenitySDLWidget::resize_event(GUI::ResizeEvent&)
{
    SDL_SendWindowEvent(m_sdl_window, SDL_WINDOWEVENT_RESIZED, width(), height());
}
void SerenitySDLWidget::show_event(GUI::ShowEvent&)
{
    SDL_SendWindowEvent(m_sdl_window, SDL_WINDOWEVENT_SHOWN, 0, 0);
}
void SerenitySDLWidget::hide_event(GUI::HideEvent&)
{
    SDL_SendWindowEvent(m_sdl_window, SDL_WINDOWEVENT_HIDDEN, 0, 0);
}

static int map_button(GUI::MouseButton button)
{
    switch (button) {
    case GUI::MouseButton::None:
        VERIFY(false);
        break;
    case GUI::MouseButton::Left:
        return SDL_BUTTON_LEFT;
    case GUI::MouseButton::Middle:
        return SDL_BUTTON_MIDDLE;
    case GUI::MouseButton::Right:
        return SDL_BUTTON_RIGHT;
    }

    VERIFY_NOT_REACHED();
    return 0;
}

void SerenitySDLWidget::mousedown_event(GUI::MouseEvent& event)
{
    SDL_SendMouseMotion(m_sdl_window, 0, 0, event.x(), event.y());
    SDL_SendMouseButton(m_sdl_window, 0, SDL_PRESSED, map_button(event.button()));
}

void SerenitySDLWidget::mousemove_event(GUI::MouseEvent& event)
{
    SDL_SendMouseMotion(m_sdl_window, 0, 0, event.x(), event.y());
}

void SerenitySDLWidget::mouseup_event(GUI::MouseEvent& event)
{
    SDL_SendMouseMotion(m_sdl_window, 0, 0, event.x(), event.y());
    SDL_SendMouseButton(m_sdl_window, 0, SDL_RELEASED, map_button(event.button()));
}

void SerenitySDLWidget::keydown_event(GUI::KeyEvent& event)
{
    SDL_SendKeyboardKey(SDL_PRESSED, scancode_map[event.key()]);
    auto text = event.text();
    if (!text.is_empty())
        SDL_SendKeyboardText(text.characters());
}

void SerenitySDLWidget::keyup_event(GUI::KeyEvent& event)
{
    SDL_SendKeyboardKey(SDL_RELEASED, scancode_map[event.key()]);
}

void SerenitySDLWidget::enter_event(Core::Event&)
{
    SDL_SetMouseFocus(m_sdl_window);
}

void SerenitySDLWidget::leave_event(Core::Event&) { SDL_SetMouseFocus(nullptr); }

struct SerenityPlatformWindow final {
    SerenityPlatformWindow(SDL_Window* sdl_window)
        : m_window(GUI::Window::construct())
        , m_widget(SerenitySDLWidget::construct(sdl_window))
    {
        m_window->resize(sdl_window->w, sdl_window->h);
        m_window->set_resizable(false);
    }

    NonnullRefPtr<GUI::Window> m_window;
    NonnullRefPtr<SerenitySDLWidget> m_widget;
};

int Serenity_CreateWindow(_THIS, SDL_Window* window)
{
    dbgln("SDL2: Creating new window of size {}x{}", window->w, window->h);
    auto w = new SerenityPlatformWindow(window);
    window->driverdata = w;
    w->m_window->set_double_buffering_enabled(false);
    w->m_widget->set_fill_with_background_color(false);
    w->m_window->set_main_widget(w->m_widget);
    w->m_window->on_close_request = [] {
        if (SDL_SendQuit())
            return GUI::Window::CloseRequestDecision::Close;
        return GUI::Window::CloseRequestDecision::StayOpen;
    };
    SERENITY_PumpEvents(_this);

    return 0;
}

void Serenity_ShowWindow(_THIS, SDL_Window* window)
{
    static_cast<SerenityPlatformWindow*>(window->driverdata)->m_window->show();
}

void Serenity_HideWindow(_THIS, SDL_Window* window)
{
    static_cast<SerenityPlatformWindow*>(window->driverdata)->m_window->hide();
}

void Serenity_SetWindowTitle(_THIS, SDL_Window* window)
{
    auto win = static_cast<SerenityPlatformWindow*>(window->driverdata);
    win->m_window->set_title(window->title);
}

void Serenity_SetWindowSize(_THIS, SDL_Window* window)
{
    auto win = static_cast<SerenityPlatformWindow*>(window->driverdata);
    win->m_window->resize(window->w, window->h);
}

void Serenity_SetWindowFullscreen(_THIS, SDL_Window* window,
    SDL_VideoDisplay* display,
    SDL_bool fullscreen)
{
    dbgln("Attempting to set SDL Window fullscreen to {}", (bool)fullscreen);
    auto win = static_cast<SerenityPlatformWindow*>(window->driverdata);
    win->m_window->set_fullscreen(fullscreen);
}

void Serenity_DestroyWindow(_THIS, SDL_Window* window)
{
    delete static_cast<SerenityPlatformWindow*>(window->driverdata);
    window->driverdata = nullptr;
}

int Serenity_CreateWindowFramebuffer(_THIS, SDL_Window* window, Uint32* format,
    void** pixels, int* pitch)
{
    dbgln("SDL2: Creating a new framebuffer of size {}x{}", window->w, window->h);
    auto win = static_cast<SerenityPlatformWindow*>(window->driverdata);
    *format = SDL_PIXELFORMAT_RGB888;
    win->m_widget->m_buffer = Gfx::Bitmap::create(
        Gfx::BitmapFormat::BGRx8888,
        Gfx::IntSize(window->w, window->h));
    *pitch = win->m_widget->m_buffer->pitch();
    *pixels = win->m_widget->m_buffer->scanline(0);
    dbgln("Created framebuffer {}x{}", win->m_widget->width(), win->m_widget->height());
    return 0;
}

int Serenity_UpdateWindowFramebuffer(_THIS, SDL_Window* window,
    const SDL_Rect* rects, int numrects)
{
    auto win = static_cast<SerenityPlatformWindow*>(window->driverdata);
    for (int i = 0; i < numrects; i++) {
        win->m_widget->update(Gfx::IntRect(rects[i].x, rects[i].y, rects[i].w, rects[i].h));
    }
    SERENITY_PumpEvents(_this);

    return 0;
}

void Serenity_DestroyWindowFramebuffer(_THIS, SDL_Window* window)
{
    auto win = static_cast<SerenityPlatformWindow*>(window->driverdata);
    dbgln("DESTROY framebuffer {}x{}", win->m_widget->width(), win->m_widget->height());
}

#endif /* SDL_VIDEO_DRIVER_SERENITY */

/* vi: set ts=4 sw=4 expandtab: */
