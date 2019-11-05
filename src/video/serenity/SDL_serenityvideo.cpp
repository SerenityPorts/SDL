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

#include "../../events/SDL_events_c.h"
#include "../SDL_pixels_c.h"
#include "../SDL_sysvideo.h"
#include "SDL_mouse.h"
#include "SDL_video.h"
}

#include "SDL_serenityevents_c.h"
#include "SDL_serenityvideo.h"

#include <LibCore/CEventLoop.h>
#include <LibGUI/GApplication.h>
#include <LibGUI/GPainter.h>
#include <LibGUI/GWidget.h>
#include <LibGUI/GWindow.h>

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

/* Initialization/Query functions */
static int SERENITY_VideoInit(_THIS);
static int SERENITY_SetDisplayMode(_THIS, SDL_VideoDisplay *display,
                                   SDL_DisplayMode *mode);
static void SERENITY_VideoQuit(_THIS);

/* SERENITY driver bootstrap functions */

static int SERENITY_Available(void) { return (1); }

static void SERENITY_DeleteDevice(SDL_VideoDevice *device) {
  dbgprintf("SERENITY_DeleteDevice\n");
  SDL_free(device);
}

extern int Serenity_CreateWindow(_THIS, SDL_Window *window);
extern void Serenity_ShowWindow(_THIS, SDL_Window *window);
extern void Serenity_HideWindow(_THIS, SDL_Window *window);
extern void Serenity_SetWindowTitle(_THIS, SDL_Window *window);
extern void Serenity_SetWindowSize(_THIS, SDL_Window *window);
extern void Serenity_SetWindowFullscreen(_THIS, SDL_Window *window,
                                         SDL_VideoDisplay *display,
                                         SDL_bool fullscreen);
extern void Serenity_DestroyWindow(_THIS, SDL_Window *window);
extern int Serenity_CreateWindowFramebuffer(_THIS, SDL_Window *window,
                                            Uint32 *format, void **pixels,
                                            int *pitch);
extern int Serenity_UpdateWindowFramebuffer(_THIS, SDL_Window *window,
                                            const SDL_Rect *rects,
                                            int numrects);
extern void Serenity_DestroyWindowFramebuffer(_THIS, SDL_Window *window);

static SDL_VideoDevice *SERENITY_CreateDevice(int devindex) {
  SDL_VideoDevice *device;

  dbgprintf("SERENITY_CreateDevice %d\n", devindex);
  /* Initialize all variables that we clean on shutdown */
  device = (SDL_VideoDevice *)SDL_calloc(1, sizeof(SDL_VideoDevice));
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

VideoBootStrap SERENITY_bootstrap = {"serenity", "SDL serenity video driver",
                                     SERENITY_Available, SERENITY_CreateDevice};

static GApplication *gapp;

// TODO: Ask kling about being able to query this from GWindow!
struct ScreenMode {
  int width;
  int height;
};

static ScreenMode modes[] = {
    {640, 480}, {800, 600}, {1024, 768}, {1280, 1024}, {1920, 1080}};

int SERENITY_VideoInit(_THIS) {
  ASSERT(!gapp);
  gapp = new GApplication(0, nullptr);
  SDL_DisplayMode mode;

  dbgprintf("SDL2: Initialising SDL application\n");

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

static int SERENITY_SetDisplayMode(_THIS, SDL_VideoDisplay *display,
                                   SDL_DisplayMode *mode) {
  dbgprintf("SERENITY_SetDisplayMode\n");
  return 0;
}

void SERENITY_VideoQuit(_THIS) {
  dbgprintf("SERENITY_VideoQuit\n");
  delete gapp;
}

class SerenitySDLWidget final : public GWidget {
  C_OBJECT(SerenitySDLWidget)
public:
  SerenitySDLWidget(SDL_Window *window, GWidget *parent = nullptr);
  RefPtr<GraphicsBitmap> m_buffer;

protected:
  void paint_event(GPaintEvent &) override;
  void resize_event(GResizeEvent &) override;
  void show_event(GShowEvent &) override;
  void hide_event(GHideEvent &) override;

  void mousedown_event(GMouseEvent &) override;
  void mousemove_event(GMouseEvent &) override;
  void mouseup_event(GMouseEvent &) override;

  void keydown_event(GKeyEvent &event) override;
  void keyup_event(GKeyEvent &event) override;

  void enter_event(CEvent &) override;
  void leave_event(CEvent &) override;

private:
  SDL_Window *m_sdl_window = nullptr;
};

SerenitySDLWidget::SerenitySDLWidget(SDL_Window *window, GWidget *parent)
    : GWidget(parent), m_sdl_window(window) {
  update();
}

void SerenitySDLWidget::paint_event(GPaintEvent &event) {
  ASSERT(size() == m_buffer->size());
  if (size() != m_buffer->size()) {
    return; // can't paint this
  }
  // dbgprintf("SerenitySDLWidget::paint_event %dx%d, %dx%d\n",
  // event.rect().x(), event.rect().y(), event.rect().width(),
  // event.rect().height());
  GPainter painter(*this);
  painter.add_clip_rect(event.rect());
  painter.blit(Point(0, 0), *m_buffer, event.rect());
}
void SerenitySDLWidget::resize_event(GResizeEvent &) {
  dbgprintf("SerenitySDLWidget::resize_event\n");
  SDL_SendWindowEvent(m_sdl_window, SDL_WINDOWEVENT_RESIZED, width(), height());
}
void SerenitySDLWidget::show_event(GShowEvent &) {
  dbgprintf("SerenitySDLWidget::show_event\n");
  SDL_SendWindowEvent(m_sdl_window, SDL_WINDOWEVENT_SHOWN, 0, 0);
}
void SerenitySDLWidget::hide_event(GHideEvent &) {
  dbgprintf("SerenitySDLWidget::hide_event\n");
  SDL_SendWindowEvent(m_sdl_window, SDL_WINDOWEVENT_HIDDEN, 0, 0);
}

static int mapButton(GMouseButton button) {
  switch (button) {
  case GMouseButton::None:
    ASSERT(false);
    break;
  case GMouseButton::Left:
    return SDL_BUTTON_LEFT;
  case GMouseButton::Middle:
    return SDL_BUTTON_MIDDLE;
  case GMouseButton::Right:
    return SDL_BUTTON_RIGHT;
  }

  ASSERT_NOT_REACHED();
  return 0;
}

void SerenitySDLWidget::mousedown_event(GMouseEvent &event) {
  SDL_SendMouseMotion(m_sdl_window, 0, 0, event.x(), event.y());
  SDL_SendMouseButton(m_sdl_window, 0, SDL_PRESSED, mapButton(event.button()));
}

void SerenitySDLWidget::mousemove_event(GMouseEvent &event) {
  SDL_SendMouseMotion(m_sdl_window, 0, 0, event.x(), event.y());
}

void SerenitySDLWidget::mouseup_event(GMouseEvent &event) {
  SDL_SendMouseMotion(m_sdl_window, 0, 0, event.x(), event.y());
  SDL_SendMouseButton(m_sdl_window, 0, SDL_RELEASED, mapButton(event.button()));
}

void SerenitySDLWidget::keydown_event(GKeyEvent &event) {
  SDL_Event sdl_event;

  sdl_event.type = SDL_KEYDOWN;
  sdl_event.key.state = SDL_PRESSED;
  sdl_event.key.keysym.sym = conversion_map[event.key()];
  SDL_PushEvent(&sdl_event);
}

void SerenitySDLWidget::keyup_event(GKeyEvent &event) {
  SDL_Event sdl_event;

  sdl_event.type = SDL_KEYUP;
  sdl_event.key.state = SDL_RELEASED;
  sdl_event.key.keysym.sym = conversion_map[event.key()];
  SDL_PushEvent(&sdl_event);
}

void SerenitySDLWidget::enter_event(CEvent &) {
  SDL_SetMouseFocus(m_sdl_window);
}

void SerenitySDLWidget::leave_event(CEvent &) { SDL_SetMouseFocus(nullptr); }

struct SerenityPlatformWindow final {
  SerenityPlatformWindow(SDL_Window *sdl_window)
      : m_window(GWindow::construct()),
        m_widget(SerenitySDLWidget::construct(sdl_window, nullptr)) {
    m_window->resize(sdl_window->w, sdl_window->h);
    m_window->set_resizable(false);
  }

  RefPtr<GWindow> m_window;
  NonnullRefPtr<SerenitySDLWidget> m_widget;
};

int Serenity_CreateWindow(_THIS, SDL_Window *window) {
  dbgprintf("SDL2: Creating new window of size %dx%d\n", window->w, window->h);
  auto w = new SerenityPlatformWindow(window);
  window->driverdata = w;
  w->m_window->set_double_buffering_enabled(false);
  w->m_widget->set_fill_with_background_color(false);
  w->m_window->set_main_widget(w->m_widget);
  dbgprintf("w->m_window size = %dx%d\n", w->m_window->width(),
            w->m_window->height());
  SERENITY_PumpEvents(_this);

  return 0;
}

void Serenity_ShowWindow(_THIS, SDL_Window *window) {
  static_cast<SerenityPlatformWindow *>(window->driverdata)->m_window->show();
}

void Serenity_HideWindow(_THIS, SDL_Window *window) {
  static_cast<SerenityPlatformWindow *>(window->driverdata)->m_window->hide();
}

void Serenity_SetWindowTitle(_THIS, SDL_Window *window) {
  auto win = static_cast<SerenityPlatformWindow *>(window->driverdata);
  win->m_window->set_title(window->title);
}

void Serenity_SetWindowSize(_THIS, SDL_Window *window) {
  auto win = static_cast<SerenityPlatformWindow *>(window->driverdata);
  win->m_window->resize(window->w, window->h);
}

void Serenity_SetWindowFullscreen(_THIS, SDL_Window *window,
                                  SDL_VideoDisplay *display,
                                  SDL_bool fullscreen) {
  dbgprintf("Attempting to set SDL Window fullscreen to %d\n", fullscreen);
  auto win = static_cast<SerenityPlatformWindow *>(window->driverdata);
  win->m_window->set_fullscreen(fullscreen);
}

void Serenity_DestroyWindow(_THIS, SDL_Window *window) {
  delete static_cast<SerenityPlatformWindow *>(window->driverdata);
  window->driverdata = nullptr;
}

int Serenity_CreateWindowFramebuffer(_THIS, SDL_Window *window, Uint32 *format,
                                     void **pixels, int *pitch) {
  dbgprintf("SDL2: Creating a new framebuffer of size %dx%d\n", window->w,
            window->h);
  auto win = static_cast<SerenityPlatformWindow *>(window->driverdata);
  *format = SDL_PIXELFORMAT_RGB888;
  win->m_widget->m_buffer = GraphicsBitmap::create(
      GraphicsBitmap::Format::RGB32,
      Size(win->m_widget->width(), win->m_widget->height()));
  *pitch = win->m_widget->m_buffer->pitch();
  *pixels = win->m_widget->m_buffer->bits(0);
  dbgprintf("Created framebuffer %dx%d\n", win->m_widget->width(),
            win->m_widget->height());
  return 0;
}

int Serenity_UpdateWindowFramebuffer(_THIS, SDL_Window *window,
                                     const SDL_Rect *rects, int numrects) {
  auto win = static_cast<SerenityPlatformWindow *>(window->driverdata);
  for (int i = 0; i < numrects; i++) {
    win->m_widget->update(Rect(rects[i].x, rects[i].y, rects[i].w, rects[i].h));
  }
  SERENITY_PumpEvents(_this);

  return 0;
}

void Serenity_DestroyWindowFramebuffer(_THIS, SDL_Window *window) {
  auto win = static_cast<SerenityPlatformWindow *>(window->driverdata);
  dbgprintf("DESTROY framebuffer %dx%d\n", win->m_widget->width(),
            win->m_widget->height());
}

#endif /* SDL_VIDEO_DRIVER_SERENITY */

/* vi: set ts=4 sw=4 expandtab: */
