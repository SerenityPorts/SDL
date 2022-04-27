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

#    include "SDL_audio.h"

#    include "../SDL_audio_c.h"
#    include "../SDL_audiodev_c.h"
}

#    include <AK/Time.h>
#    include <LibAudio/Buffer.h>
#    include <LibAudio/ConnectionFromClient.h>
#    include <LibAudio/SampleFormats.h>
#    include <time.h>
#    include "SDL_serenityaudio.h"

static Array<Audio::Sample, Audio::AUDIO_BUFFER_SIZE> output_buffer {};
static size_t output_buffer_samples_remaining { 0 };

static void SERENITYAUDIO_CloseDevice(_THIS)
{
    SDL_free(that->hidden->mixbuf);
    SDL_free(that->hidden);
}

static int SERENITYAUDIO_OpenDevice(_THIS, void*, char const*, int iscapture)
{
    /* Initialize all variables that we clean on shutdown */
    that->hidden = static_cast<struct SDL_PrivateAudioData*>(SDL_malloc(sizeof *that->hidden));
    if (!that->hidden)
        return SDL_OutOfMemory();
    SDL_zerop(that->hidden);

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

    // We need to create our audio connection and event loop here, in order to register them with SDL's audio thread
    if (!h->event_loop)
        h->event_loop = make<Core::EventLoop>(Core::EventLoop::MakeInspectable::No);
    if (!h->client)
        h->client = MUST(Audio::ConnectionFromClient::try_create());

    h->client->async_start_playback();

    auto convert_i16_to_double = [](i16 input) -> auto {
        return (static_cast<double>(input) - NumericLimits<i16>::min()) / NumericLimits<u16>::max() * 2. - 1.;
    };

    auto const sleep_spec = Time::from_nanoseconds(100).to_timespec();
    auto input_buffer = reinterpret_cast<i16*>(h->mixbuf);
    auto input_samples = h->mixlen / that->spec.channels / sizeof(i16);
    size_t input_position = 0;

    while (input_samples > 0) {
        // Fill up the output buffer
        auto const input_samples_to_process = min(input_samples, Audio::AUDIO_BUFFER_SIZE - output_buffer_samples_remaining);
        for (size_t i = 0; i < input_samples_to_process; ++i) {
            auto left = convert_i16_to_double(input_buffer[input_position]);
            auto right = convert_i16_to_double(input_buffer[input_position + 1]);
            output_buffer[output_buffer_samples_remaining + i] = Audio::Sample(left, right);
            input_position += 2;
        }
        output_buffer_samples_remaining += input_samples_to_process;
        input_samples -= input_samples_to_process;

        // Stop if we don't have enough samples to fill a buffer
        if (output_buffer_samples_remaining < Audio::AUDIO_BUFFER_SIZE)
            break;

        // Try to enqueue our output buffer
        for (;;) {
            auto enqueue_result = h->client->realtime_enqueue(output_buffer);
            if (!enqueue_result.is_error())
                break;
            if (enqueue_result.error() != Audio::AudioQueue::QueueStatus::Full)
                return;

            nanosleep(&sleep_spec, nullptr);
        }
        output_buffer_samples_remaining = 0;
    }

    // Pump our event loop - should just be the IPC call to start playback
    for (;;) {
        auto number_of_events_pumped = h->event_loop->pump(Core::EventLoop::WaitMode::PollForEvents);
        if (number_of_events_pumped == 0)
            break;
    }
}

static Uint8* SERENITYAUDIO_GetDeviceBuf(_THIS)
{
    return that->hidden->mixbuf;
}

static int SERENITYAUDIO_Init(SDL_AudioDriverImpl* impl)
{
    /* Set the function pointers */
    impl->OpenDevice = SERENITYAUDIO_OpenDevice;
    impl->PlayDevice = SERENITYAUDIO_PlayDevice;
    impl->GetDeviceBuf = SERENITYAUDIO_GetDeviceBuf;
    impl->CloseDevice = SERENITYAUDIO_CloseDevice;

    impl->AllowsArbitraryDeviceNames = 1;
    impl->HasCaptureSupport = SDL_FALSE;

    return 1; // this audio target is available.
}

AudioBootStrap SERENITYAUDIO_bootstrap = {
    "serenity", "Serenity using AudioServer", SERENITYAUDIO_Init, 0
};

#endif
