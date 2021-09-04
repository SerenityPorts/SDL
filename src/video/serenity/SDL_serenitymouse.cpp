/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2021 Sam Lantinga <slouken@libsdl.org>

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

#if SDL_VIDEO_DRIVER_SERENITY
#define AK_DONT_REPLACE_STD

/*
 * SDL includes:
 */
extern "C" {
#include "../../events/SDL_mouse_c.h"
#include "../../events/SDL_touch_c.h"
#include "../SDL_sysvideo.h"
#include "SDL_events.h"
}

#include "SDL_serenitymouse.h"
#include "SDL_serenityvideo.h"

#include <LibGfx/StandardCursor.h>

struct SerenityCursorData final {
    Gfx::StandardCursor cursor_type;
};

static SDL_Cursor*
SERENITY_CreateSystemCursor(SDL_SystemCursor id)
{
    auto cursor = static_cast<SDL_Cursor*>(SDL_calloc(1, sizeof(SDL_Cursor)));
    if (!cursor)
        SDL_OutOfMemory();

    auto cursor_data = static_cast<SerenityCursorData*>(SDL_calloc(1, sizeof(SerenityCursorData)));
    if (!cursor_data)
        SDL_OutOfMemory();
    cursor->driverdata = static_cast<void*>(cursor_data);

    switch(id)
    {
        case SDL_SYSTEM_CURSOR_ARROW:     cursor_data->cursor_type = Gfx::StandardCursor::Arrow; break;
        case SDL_SYSTEM_CURSOR_IBEAM:     cursor_data->cursor_type = Gfx::StandardCursor::IBeam; break;
        case SDL_SYSTEM_CURSOR_WAIT:      cursor_data->cursor_type = Gfx::StandardCursor::Wait; break;
        case SDL_SYSTEM_CURSOR_CROSSHAIR: cursor_data->cursor_type = Gfx::StandardCursor::Crosshair; break;
        case SDL_SYSTEM_CURSOR_WAITARROW: cursor_data->cursor_type = Gfx::StandardCursor::Wait; break;
        case SDL_SYSTEM_CURSOR_SIZENWSE:  cursor_data->cursor_type = Gfx::StandardCursor::ResizeDiagonalTLBR; break;
        case SDL_SYSTEM_CURSOR_SIZENESW:  cursor_data->cursor_type = Gfx::StandardCursor::ResizeDiagonalBLTR; break;
        case SDL_SYSTEM_CURSOR_SIZEWE:    cursor_data->cursor_type = Gfx::StandardCursor::ResizeHorizontal; break;
        case SDL_SYSTEM_CURSOR_SIZENS:    cursor_data->cursor_type = Gfx::StandardCursor::ResizeVertical; break;
        case SDL_SYSTEM_CURSOR_SIZEALL:   cursor_data->cursor_type = Gfx::StandardCursor::Move; break;
        case SDL_SYSTEM_CURSOR_NO:        cursor_data->cursor_type = Gfx::StandardCursor::Disallowed; break;
        case SDL_SYSTEM_CURSOR_HAND:      cursor_data->cursor_type = Gfx::StandardCursor::Hand; break;
        default:
            SDL_assert(0);
            return nullptr;
    }
    return cursor;
}

static SDL_Cursor*
SERENITY_CreateDefaultCursor()
{
    return SERENITY_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
}

static void
SERENITY_FreeCursor(SDL_Cursor* cursor)
{
    if (cursor->driverdata)
        SDL_free(cursor->driverdata);
    SDL_free(cursor);
}

static int
SERENITY_ShowCursor(SDL_Cursor* cursor)
{
    auto focused_window = SDL_GetMouseFocus();
    if (!focused_window)
        return -1;

    auto platform_window = SerenityPlatformWindow::from_sdl_window(focused_window);

    // Hide the cursor
    if (cursor == nullptr) {
        platform_window->window()->set_cursor(Gfx::StandardCursor::Hidden);
        return 0;
    }

    auto cursor_data = static_cast<SerenityCursorData*>(cursor->driverdata);
    if (!cursor_data)
        return -1;

    platform_window->window()->set_cursor(cursor_data->cursor_type);
    return 0;
}

void
SERENITY_InitMouse(_THIS)
{
    dbgln("SERENITY_InitMouse");

    auto mouse = SDL_GetMouse();

    mouse->CreateSystemCursor = SERENITY_CreateSystemCursor;
    mouse->ShowCursor = SERENITY_ShowCursor;
    mouse->FreeCursor = SERENITY_FreeCursor;

    // FIXME: implement below methods
    //mouse->CreateCursor = ...;
    //mouse->WarpMouse = ...;
    //mouse->SetRelativeMouseMode = ...;

    SDL_SetDefaultCursor(SERENITY_CreateDefaultCursor());
}

void
SERENITY_QuitMouse(_THIS)
{
    dbgln("SERENITY_QuitMouse");
}

#endif /* SDL_VIDEO_DRIVER_SERENITY */

/* vi: set ts=4 sw=4 expandtab: */
