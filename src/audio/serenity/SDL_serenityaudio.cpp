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

#if SDL_AUDIO_DRIVER_SERENITY
#    define AK_DONT_REPLACE_STD

extern "C" {

#    include <errno.h>
#    include <fcntl.h>
#    include <stdio.h>
#    include <string.h>
#    include <sys/stat.h>
#    include <sys/time.h>
#    include <unistd.h>

#    include "SDL_audio.h"

#    include "../SDL_audio_c.h"
#    include "../SDL_audiodev_c.h"
#    include "SDL_timer.h"
}

#    include "SDL_serenityaudio.h"

static void SERENITYAUDIO_DetectDevices()
{
}

static void SERENITYAUDIO_CloseDevice(_THIS)
{
    if (that->hidden->audio_fd != -1)
        close(that->hidden->audio_fd);
    SDL_free(that->hidden->mixbuf);
    SDL_free(that->hidden);
}

static int SERENITYAUDIO_OpenDevice(_THIS, void*, const char*, int iscapture)
{
    /* Initialize all variables that we clean on shutdown */
    that->hidden = (struct SDL_PrivateAudioData*)SDL_malloc((sizeof *that->hidden));
    if (!that->hidden)
        return SDL_OutOfMemory();
    SDL_zerop(that->hidden);

    int fd = open("/dev/audio/0", O_WRONLY);
    if (fd < 0)
        return SDL_SetError("Unable to open /dev/audio/0");
    that->hidden->audio_fd = fd;

    that->spec.freq = 44100;
    that->spec.format = AUDIO_S16LSB;
    that->spec.channels = 2;
    that->spec.samples = 1024;

    /* Calculate the final parameters for this audio specification */
    SDL_CalculateAudioSpec(&that->spec);

    /* Allocate mixing buffer */
    if (!iscapture) {
        that->hidden->mixlen = that->spec.size;
        that->hidden->mixbuf = (Uint8*)SDL_malloc(that->hidden->mixlen);
        if (!that->hidden->mixbuf)
            return SDL_OutOfMemory();
        SDL_memset(that->hidden->mixbuf, that->spec.silence, that->spec.size);
    }

    /* We're ready to rock and roll. :-) */
    return 0;
}

static void SERENITYAUDIO_PlayDevice(_THIS)
{
    struct SDL_PrivateAudioData* h = that->hidden;
    write(h->audio_fd, h->mixbuf, h->mixlen);
#    ifdef DEBUG_AUDIO
    fprintf(stderr, "Wrote %d bytes of audio data\n", h->mixlen);
#    endif
}

static Uint8* SERENITYAUDIO_GetDeviceBuf(_THIS)
{
    return that->hidden->mixbuf;
}

static int SERENITYAUDIO_CaptureFromDevice(_THIS, void* buffer, int buflen)
{
    return -EIO;
}

static void SERENITYAUDIO_FlushCapture(_THIS)
{
}

static int SERENITYAUDIO_Init(SDL_AudioDriverImpl* impl)
{
    /* Set the function pointers */
    impl->DetectDevices = SERENITYAUDIO_DetectDevices;
    impl->OpenDevice = SERENITYAUDIO_OpenDevice;
    impl->PlayDevice = SERENITYAUDIO_PlayDevice;
    impl->GetDeviceBuf = SERENITYAUDIO_GetDeviceBuf;
    impl->CloseDevice = SERENITYAUDIO_CloseDevice;
    impl->CaptureFromDevice = SERENITYAUDIO_CaptureFromDevice;
    impl->FlushCapture = SERENITYAUDIO_FlushCapture;

    impl->AllowsArbitraryDeviceNames = 1;
    impl->HasCaptureSupport = SDL_TRUE;

    return 1; /* this audio target is available. */
}

AudioBootStrap SERENITYAUDIO_bootstrap = {
    "serenity", "Serenity using /dev/audio/0", SERENITYAUDIO_Init, 0
};

#endif
