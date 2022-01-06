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

#include "../../events/SDL_events_c.h"

#include "SDL_serenityvideo.h"
#include "SDL_serenityevents_c.h"
#include "SDL_timer.h"

#include <LibCore/EventLoop.h>

void
SERENITY_PumpEvents(_THIS)
{
    auto& loop = Core::EventLoop::current();
    if (loop.was_exit_requested())
        exit(0);

    auto const event_loop_timeout_ms = SDL_GetTicks() + 100;
    while (loop.pump(Core::EventLoop::WaitMode::PollForEvents) > 0) {
        if (SDL_TICKS_PASSED(SDL_GetTicks(), event_loop_timeout_ms))
            break;
    }

}


#endif /* SDL_VIDEO_DRIVER_SERENITY */

/* vi: set ts=4 sw=4 expandtab: */
