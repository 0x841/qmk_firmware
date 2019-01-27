#ifdef AUDIO_ENABLE
#include "audio_user.h"

#include QMK_KEYBOARD_H
#include "action_layer.h"

float true_song[][2]  = SONG(E__NOTE(_A5), E__NOTE(_A6));
float false_song[][2] = SONG(E__NOTE(_A6), E__NOTE(_A5));

void beep_bool(bool b) {
    if (b) {
        PLAY_SONG(true_song);
    }
    else {
        PLAY_SONG(false_song);
    }
}
#endif
