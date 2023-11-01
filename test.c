#include <stdio.h>
#include <assert.h>

#define SDL_MAIN_HANDLED

#include "SDL.h"
#include "SDL_mixer.h"

//gcc test.c -Iinclude -L. -lsdl2 -lsdl2_mixer

int main(int argc, char * argv []) {
    assert(SDL_Init(SDL_INIT_AUDIO) == 0);
    Mix_Init(0);

    Mix_OpenAudio(48000, AUDIO_S32SYS, 2, 30000);

    const SDL_version * ver = Mix_Linked_Version();
    printf("Mixer %i.%i.%i\n", ver->major, ver->minor, ver->patch);
    printf(argv[ 0 ]);
    Mix_Music* music = Mix_LoadMUS(argv[ 0 ]);

    if (music == NULL) {
        printf("SDL_GetError=%s\n", SDL_GetError());
        exit(0);
    }

    Mix_PlayMusic(music, 0);

    SDL_Delay(5000);

    Mix_FreeMusic(music);
    SDL_Quit();
}
