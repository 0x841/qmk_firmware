#include "planck.h"
#include "action_layer.h"
#include "keymap_jp.h"

#ifdef AUDIO_ENABLE
#define TRUE_NOTE  E__NOTE(_A5),E__NOTE(_A6),
#define FALSE_NOTE E__NOTE(_A6),E__NOTE(_A5),
#define TRUE_SONG  SONG(TRUE_NOTE)
#define FALSE_SONG SONG(FALSE_NOTE)
#endif

#define NUMERIC TG(_NUMERIC)

#define IS_RAISE_ON (IS_LAYER_ON(_RAISE_US) || IS_LAYER_ON(_RAISE_JIS))
#define IS_RAISE_OFF (IS_LAYER_OFF(_RAISE_US) && IS_LAYER_OFF(_RAISE_JIS))
#define IS_LOWER_ON IS_LAYER_ON(_LOWER)
#define IS_LOWER_OFF IS_LAYER_OFF(_LOWER)
#define HAS_SFT (keyboard_report->mods & (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT)))
#define HAS_CTL (keyboard_report->mods & (MOD_BIT(KC_LCTL) | MOD_BIT(KC_RCTL)))
#define HAS_GUI (keyboard_report->mods & (MOD_BIT(KC_LGUI) | MOD_BIT(KC_RGUI)))
#define HAS_ALT (keyboard_report->mods & (MOD_BIT(KC_LALT) | MOD_BIT(KC_RALT)))
#define HAS_LSFT (keyboard_report->mods & MOD_BIT(KC_LSFT))
#define HAS_LCTL (keyboard_report->mods & MOD_BIT(KC_LCTL))
#define HAS_LGUI (keyboard_report->mods & MOD_BIT(KC_LGUI))
#define HAS_LALT (keyboard_report->mods & MOD_BIT(KC_LALT))
#define HAS_RSFT (keyboard_report->mods & MOD_BIT(KC_RSFT))
#define HAS_RCTL (keyboard_report->mods & MOD_BIT(KC_RCTL))
#define HAS_RGUI (keyboard_report->mods & MOD_BIT(KC_RGUI))
#define HAS_RALT (keyboard_report->mods & MOD_BIT(KC_RALT))

extern keymap_config_t keymap_config;

bool is_orig_layout_enabled = true;
bool is_rmods_enabled = false;
bool is_zhtg_replaced = false;
bool is_capslock_swapped = false;
bool is_mac = false;
bool is_jis = false;

bool is_capslock_enable;
bool is_lower_held;
bool is_lower_pressed;

#ifdef AUDIO_ENABLE
float true_song[][2]  = TRUE_SONG;
float false_song[][2] = FALSE_SONG;
#endif

enum layers {
    _QWERTY,
    _ORIGINAL,
    _MOD_SC,
    _MOD_MAC,
    _MOD_SCM,
    _RAISE_US,
    _RAISE_JIS,
    _LOWER,
    _EXTRA,
    _RMODS,
    _NUMERIC
};

enum additional_keycodes {
    RAISE = SAFE_RANGE,
    LOWER,
    HLD_LOW,
    LAYO_TG,
    RMOD_TG,
    ZHTG_TG,
    JIS_TG,
    SWP_CAP,
    OS_TG
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Qwerty
     * ,-----------------------------------------------------------------------------------.
     * |   (  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |   [  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Ctrl |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |RAISE |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Shift |   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |   -  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Caps | NUM  | GUI  | Alt  |LOWER |Space | Bksp |Enter | Left | Down |  Up  |Right |
     * `-----------------------------------------------------------------------------------'
     */

    [_QWERTY] = {
        {KC_LPRN, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC},
        {KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, RAISE},
        {KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_MINS},
        {KC_CAPS, NUMERIC, KC_LGUI, KC_LALT, LOWER,   KC_SPC,  KC_BSPC, KC_ENT,  KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT}
    },

    /* Original
     * ,-----------------------------------------------------------------------------------.
     * |   (  |   Y  |   W  |   E  |   R  |   T  |   Q  |   U  |   I  |   O  |   P  |   [  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Ctrl |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |RAISE |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Shift |   Z  |   X  |   V  |   C  |   B  |   N  |   M  |   ,  |   .  |   /  |   -  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Caps | NUM  | GUI  | Alt  |LOWER |Space | Bksp |Enter | Left | Down |  Up  |Right |
     * `-----------------------------------------------------------------------------------'
     */

    [_ORIGINAL] = {
        {KC_LPRN, KC_Y,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Q,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC},
        {KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, RAISE},
        {KC_LSFT, KC_Z,    KC_X,    KC_V,    KC_C,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_MINS},
        {KC_CAPS, NUMERIC, KC_LGUI, KC_LALT, LOWER,   KC_SPC,  KC_BSPC, KC_ENT,  KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT}
    },

    /*          SwapCaps  Mac   SwapCaps & Mac
     * Ctrl ->    Caps    GUI        GUI
     * Caps ->    Ctrl               Ctrl
     * GUI  ->            Alt        Alt
     * Alt  ->            Ctrl       Caps
     */

    [_MOD_SC] = {
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {KC_CAPS, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {KC_LCTL, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______}
    },

    [_MOD_MAC] = {
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {KC_LGUI, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, KC_LALT, KC_LCTL, _______, _______, _______, _______, _______, _______, _______, _______}
    },

    [_MOD_SCM] = {
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {KC_LGUI, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {KC_LCTL, _______, KC_LALT, KC_CAPS, _______, _______, _______, _______, _______, _______, _______, _______}
    },

    /* Raise
     * ,-----------------------------------------------------------------------------------.
     * | Tab  |   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   \  |   |  | Esc  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Ctrl |   =  |   +  |   '  |   "  |   ]  |   }  |   `  |   ~  |      | Del  |RAISE |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Shift |      |      |      |      |      |      |      |      |      |      |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      | GUI  | Alt  |LOWER |Space | Bksp |Enter |      |      |      |      |
     * `-----------------------------------------------------------------------------------'
     */

    [_RAISE_US] = {
        {KC_TAB,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_BSLS, KC_PIPE, KC_ESC},
        {_______, KC_EQL,  KC_PLUS, KC_QUOT, KC_DQT,  KC_RBRC, KC_RCBR, KC_GRV,  KC_TILD, XXXXXXX, KC_DEL,  _______},
        {_______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX},
        {XXXXXXX, XXXXXXX, _______, _______, _______, _______, _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX}
    },

    [_RAISE_JIS] = {
        {KC_TAB,  KC_EXLM, JP_AT,   KC_HASH, KC_DLR,  KC_PERC, JP_CIRC, JP_AMPR, JP_ASTR, JP_BSLS, JP_PIPE, KC_ESC},
        {_______, JP_EQL,  JP_PLUS, JP_QUOT, JP_DQT,  JP_RBRC, JP_RCBR, JP_GRV,  JP_TILD, XXXXXXX, KC_DEL,  _______},
        {_______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX},
        {XXXXXXX, XXXXXXX, _______, _______, _______, _______, _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX}
    },

    /* Lower
     * ,-----------------------------------------------------------------------------------.
     * | Tab  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Esc  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Ctrl | Home | PgDn | PgUp | End  |      | Left | Down |  Up  |Right | Del  |RAISE |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Shift |      |      |      |      |      |      |      |   ,  |   .  |HLDLOW|      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      | GUI  | Alt  |LOWER |Space | Bksp |Enter |      |      |      |      |
     * `-----------------------------------------------------------------------------------'
     */

    [_LOWER] = {
        {KC_TAB,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_ESC},
        {_______, KC_HOME, KC_PGDN, KC_PGUP, KC_END,  XXXXXXX, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_DEL,  _______},
        {_______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_COMM, KC_DOT,  HLD_LOW, XXXXXXX},
        {XXXXXXX, XXXXXXX, _______, _______, _______, _______, _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX}
    },

    /* Extra
     * ,-----------------------------------------------------------------------------------.
     * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F7  |  F8  |  F9  | F10  | F11  | F12  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Ctrl | App  |      |      |      |      |      |JIS_TG|      |OS_TGL|Insert|RAISE |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Shift |ZHTGTG|      |SWPCAP|LAYOTG|      |NumLc |RMODTG| PtSc | ScLc |Pause |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Reset |      | GUI  | Alt  |LOWER |Space | Bksp |Enter |      |      |      |      |
     * `-----------------------------------------------------------------------------------'
     */

    [_EXTRA] = {
        {KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12},
        {_______, KC_APP,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, JIS_TG,  XXXXXXX, OS_TG,   KC_INS,  _______},
        {_______, ZHTG_TG, XXXXXXX, SWP_CAP, LAYO_TG, XXXXXXX, KC_NLCK, RMOD_TG, KC_PSCR, KC_SLCK, KC_PAUS, XXXXXXX},
        {RESET,   XXXXXXX, _______, _______, _______, _______, _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX}
    },

    [_RMODS] = {
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, _______, _______, _______, _______, _______, _______, KC_RALT, KC_RGUI, KC_RCTL, KC_RSFT}
    },

    /* Numeric
     * ,-----------------------------------------------------------------------------------.
     * |      |      |      |  Up  |      |      |   7  |   8  |   9  |   -  |   /  | Bksp |
     * |------+------+------+------+------+------+-------------+------+------+------+------|
     * |      |      | Left | Down |Right |      |   4  |   5  |   6  |   +  |   *  | Del  |
     * |------+------+------+------+------+------+------|------+------+------+------+------|
     * |      |      |      |      |      |      |   1  |   2  |   3  |Enter |      |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      | NUM  |      |      |      |Space |      |   0  |   .  |      |      |      |
     * `-----------------------------------------------------------------------------------'
     */

    [_NUMERIC] = {
        {XXXXXXX, XXXXXXX, XXXXXXX, KC_UP,   XXXXXXX, XXXXXXX, KC_P7,   KC_P8,   KC_P9,   KC_PMNS, KC_PSLS, KC_BSPC},
        {XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX, KC_P4,   KC_P5,   KC_P6,   KC_PPLS, KC_PAST, KC_DEL},
        {XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_P1,   KC_P2,   KC_P3,   KC_PENT, XXXXXXX, XXXXXXX},
        {XXXXXXX, _______, XXXXXXX, XXXXXXX, XXXXXXX, KC_SPC,  XXXXXXX, KC_P0,   KC_PDOT, XXXXXXX, XXXXXXX, XXXXXXX}
    }
};

void beep_bool(bool b) {
    #ifdef AUDIO_ENABLE
    if (b)
        PLAY_SONG(true_song);
    else
        PLAY_SONG(false_song);
    #endif
}

void tap_key(uint16_t kc) {
    register_code(kc);
    unregister_code(kc);
}

void set_base_layer(void) {
    set_single_persistent_default_layer(is_orig_layout_enabled ? _ORIGINAL : _QWERTY);
}

void set_mod_layer(void) {
    layer_off(_MOD_SC);
    layer_off(_MOD_MAC);
    layer_off(_MOD_SCM);
    if (is_capslock_swapped) {
        if (is_mac)
            layer_on(_MOD_SCM);
        else
            layer_on(_MOD_SC);
    }
    else if (is_mac) {
        layer_on(_MOD_MAC);
    }
}

void switch_extra(void) {
    if (IS_RAISE_ON && IS_LOWER_ON) {
        layer_on(_EXTRA);
        is_lower_held = false;
    }
    else {
        layer_off(_EXTRA);
    }
}

void matrix_init_user(void) {
    is_lower_held = false;
    is_lower_pressed = false;
    set_base_layer();
    set_mod_layer();
}

void led_set_user(uint8_t usb_led) {
    is_capslock_enable = usb_led & (1 << USB_LED_CAPS_LOCK);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool is_left_pressed, is_right_pressed;
    switch (keycode) {
        // layer
        case RAISE:
            if (record->event.pressed) {
                layer_on(is_jis ? _RAISE_JIS : _RAISE_US);
            }
            else {
                layer_off(_RAISE_US);
                layer_off(_RAISE_JIS);
            }
            switch_extra();
            return false;
            break;
        case LOWER:
            if (record->event.pressed)
                layer_on(_LOWER);
            else if (!is_lower_held)
                layer_off(_LOWER);
            switch_extra();
            is_lower_pressed = record->event.pressed;
            return false;
            break;

        case HLD_LOW:
            if (record->event.pressed && !is_lower_pressed)
                layer_off(_LOWER);
            else
                is_lower_held = record->event.pressed;
            return false;
            break;

        // Toggle settings
        case LAYO_TG:
            if (record->event.pressed) {
                is_orig_layout_enabled = !is_orig_layout_enabled;
                set_base_layer();
                beep_bool(is_orig_layout_enabled);
            }
            return false;
            break;
        case RMOD_TG:
            if (record->event.pressed) {
                is_rmods_enabled = !is_rmods_enabled;
                if (is_rmods_enabled)
                    layer_on(_RMODS);
                else
                    layer_off(_RMODS);
                beep_bool(is_rmods_enabled);
            }
            return false;
            break;
        case ZHTG_TG:
            if (record->event.pressed) {
                is_zhtg_replaced = !is_zhtg_replaced;
                beep_bool(is_zhtg_replaced);
            }
            return false;
            break;
        case JIS_TG:
            if (record->event.pressed) {
                layer_off(is_jis ? _RAISE_JIS : _RAISE_US);
                is_jis = !is_jis;
                layer_on(is_jis ? _RAISE_JIS : _RAISE_US);
                beep_bool(is_jis);
            }
            return false;
            break;
        case SWP_CAP:
            if (record->event.pressed) {
                is_capslock_swapped = !is_capslock_swapped;
                set_mod_layer();
                layer_on(is_jis ? _RAISE_JIS : _RAISE_US);
                layer_on(_LOWER);
                layer_on(_EXTRA);
                beep_bool(is_capslock_swapped);
            }
            return false;
            break;
        case OS_TG:
            if (record->event.pressed) {
                is_mac = !is_mac;
                set_mod_layer();
                beep_bool(is_mac);
            }
            return false;
            break;

        // replace "(" with "Tab" when pressed Ctrl or Alt
        // replace "(" with  ")"  when pressed Shift
        case KC_LPRN:
            if (record->event.pressed) {
                if (HAS_CTL || HAS_ALT) {
                    tap_key(KC_TAB);
                    return false;
                }
                else if (HAS_SFT) {
                    tap_key(is_jis ? KC_9 : KC_0);
                    return false;
                }
                else if (is_jis) {
                    add_weak_mods(MOD_LSFT);
                    tap_key(KC_8);
                    del_weak_mods(MOD_LSFT);
                    return false;
                }
            }
            return true;
            break;

        // "_" for jis and capslock
        case KC_MINS:
            if (record->event.pressed && ((is_jis && HAS_SFT) || is_capslock_enable) && IS_RAISE_OFF) {
                if (is_capslock_enable && HAS_SFT) {
                    is_left_pressed  = HAS_LSFT;
                    is_right_pressed = HAS_RSFT;
                    if (is_left_pressed)  unregister_code(KC_LSFT);
                    if (is_right_pressed) unregister_code(KC_RSFT);

                    tap_key(KC_MINS);

                    if (is_left_pressed)  register_code(KC_LSFT);
                    if (is_right_pressed) register_code(KC_RSFT);
                }
                else {
                    add_weak_mods(MOD_LSFT);
                    tap_key(is_jis ? JP_BSLS : KC_MINS);
                    del_weak_mods(MOD_LSFT);
                }
                return false;
            }
            return true;
            break;

        // "[" and ":" for jis
        case KC_LBRC:
            if (record->event.pressed && is_jis && IS_RAISE_OFF) {
                tap_key(JP_LBRC);
                return false;
            }
            return true;
            break;
        case KC_SCLN:
            if (record->event.pressed && is_jis && HAS_SFT && IS_RAISE_OFF) {
                is_left_pressed  = HAS_LSFT;
                is_right_pressed = HAS_RSFT;
                if (is_left_pressed)  unregister_code(KC_LSFT);
                if (is_right_pressed) unregister_code(KC_RSFT);

                tap_key(JP_COLN);

                if (is_left_pressed)  register_code(KC_LSFT);
                if (is_right_pressed) register_code(KC_RSFT);
                return false;
            }
            return true;
            break;

        // replace "Ctrl-Space" when is_zhtg_replaced is true
        case KC_SPC:
            if (record->event.pressed && is_zhtg_replaced && HAS_CTL
                    && IS_RAISE_OFF && IS_LOWER_OFF && !HAS_SFT && !HAS_GUI && !HAS_ALT) {
                is_left_pressed  = HAS_LCTL;
                is_right_pressed = HAS_RCTL;
                if (is_left_pressed)  unregister_code(KC_LCTL);
                if (is_right_pressed) unregister_code(KC_RCTL);

                if (is_jis) {
                    tap_key(JP_ZHTG);
                }
                else {
                    add_weak_mods(MOD_LALT);
                    tap_key(KC_GRV);
                    del_weak_mods(MOD_LALT);
                }

                if (is_left_pressed)  register_code(KC_LCTL);
                if (is_right_pressed) register_code(KC_RCTL);
                return false;
            }
            return true;
            break;
    }
    return true;
}
