#include QMK_KEYBOARD_H
#include "action_layer.h"
#include "keymap_jp.h"
#include "myutil.h"

#ifdef AUDIO_ENABLE
#include "audio_user.h"
#endif

#ifdef BACKLIGHT_ENABLE
#include "backlight_user.h"
#endif

#define IS_RAISE_ON (IS_LAYER_ON(_RAISE_US) || IS_LAYER_ON(_RAISE_JIS))
#define IS_RAISE_OFF (IS_LAYER_OFF(_RAISE_US) && IS_LAYER_OFF(_RAISE_JIS))
#define IS_LOWER_ON IS_LAYER_ON(_LOWER)
#define IS_LOWER_OFF IS_LAYER_OFF(_LOWER)
#define IS_NUMERIC_ON IS_LAYER_ON(_NUMERIC)
#define IS_NUMERIC_OFF IS_LAYER_OFF(_NUMERIC)

extern keymap_config_t keymap_config;

typedef union {
    uint32_t raw;
    struct {
        bool is_orig_layout_enabled : 1;
        bool is_rmods_enabled       : 1;
        bool is_zhtg_replaced       : 1;
        bool is_capslock_swapped    : 1;
        bool is_mac                 : 1;
        bool is_jis                 : 1;
    };
} user_config_t;
user_config_t config;

bool is_lower_held;
bool is_lower_pressed;

enum layers {
    _QWERTY,
    _ORIGINAL,
    _MOD_SC,
    _MOD_MAC,
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
    NUMERIC,
    HLD_LOW,
    LAYO_TG,
    RMOD_TG,
    ZHTG_TG,
    JIS_TG,
    SWP_CAP,
    OS_TG,
    FLS_ROM
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Qwerty
     * ,-----------------------------------------------------------------------------------.
     * |   (  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |   =  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Ctrl |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |RAISE |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Shift |   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |   -  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Caps | NUM  | GUI  | Alt  |LOWER |Space | Bksp |Enter | Left | Down |  Up  |Right |
     * `-----------------------------------------------------------------------------------'
     */

    [_QWERTY] = {
        {KC_LPRN, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_EQL},
        {KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, RAISE},
        {KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_MINS},
        {KC_CAPS, NUMERIC, KC_LGUI, KC_LALT, LOWER,   KC_SPC,  KC_BSPC, KC_ENT,  KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT}
    },

    /* Original
     * ,-----------------------------------------------------------------------------------.
     * |      |   Y  |   W  |   E  |   R  |   F  |   Q  |   U  |   I  |   O  |   P  |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |   A  |   S  |   D  |   T  |   G  |   H  |   J  |   K  |   L  |   ;  |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |   Z  |   X  |   V  |   C  |   B  |   N  |   M  |   ,  |   .  |   /  |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |      |      |      |      |      |      |      |
     * `-----------------------------------------------------------------------------------'
     */

    [_ORIGINAL] = {
        {_______, KC_Y,    KC_W,    KC_E,    KC_R,    KC_F,    KC_Q,    KC_U,    KC_I,    KC_O,    KC_P,    _______},
        {_______, KC_A,    KC_S,    KC_D,    KC_T,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, _______},
        {_______, KC_Z,    KC_X,    KC_V,    KC_C,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, _______},
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______}
    },

    [_MOD_SC] = {
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {KC_CAPS, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {KC_LCTL, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______}
    },

    [_MOD_MAC] = {
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______},
        {_______, _______, KC_LALT, KC_LGUI, _______, _______, _______, _______, _______, _______, _______, _______}
    },

    /* Raise
     * ,-----------------------------------------------------------------------------------.
     * | Tab  |   !  |   @  |   #  |   $  |   %  |      |   ^  |   &  |   *  |  Del | Esc  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Ctrl |   "  |   '  |   {  |   [  |   }  |   ]  |   |  |   \  |   ~  |   `  |RAISE |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Shift |      |      |      |      |      |      |      |      |      |      |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      | GUI  | Alt  |LOWER |Space | Bksp |Enter |      |      |      |      |
     * `-----------------------------------------------------------------------------------'
     */

    [_RAISE_US] = {
        {KC_TAB,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, XXXXXXX, KC_CIRC, KC_AMPR, KC_ASTR, KC_DEL,  KC_ESC},
        {_______, KC_DQT,  KC_QUOT, KC_LCBR, KC_LBRC, KC_RCBR, KC_RBRC, KC_PIPE, KC_BSLS, KC_TILD, KC_GRV,  _______},
        {_______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX},
        {XXXXXXX, XXXXXXX, _______, _______, _______, _______, _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX}
    },

    [_RAISE_JIS] = {
        {KC_TAB,  KC_EXLM, JP_AT,   KC_HASH, KC_DLR,  KC_PERC, XXXXXXX, JP_CIRC, JP_AMPR, JP_ASTR, KC_DEL,  KC_ESC},
        {_______, JP_DQT,  JP_QUOT, JP_LCBR, JP_LBRC, JP_RCBR, JP_RBRC, JP_PIPE, JP_BSLS, JP_TILD, JP_GRV,  _______},
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
     * | Ctrl | App  |      |FLSROM|      |      |      |JIS_TG|      |OS_TGL|Insert|RAISE |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Shift |ZHTGTG|      |SWPCAP|LAYOTG|      |NumLc |RMODTG| PtSc | ScLc |Pause |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Reset |      | GUI  | Alt  |LOWER |Space | Bksp |Enter |      |      |      |      |
     * `-----------------------------------------------------------------------------------'
     */

    [_EXTRA] = {
        {KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12},
        {_______, KC_APP,  XXXXXXX, FLS_ROM, XXXXXXX, XXXXXXX, XXXXXXX, JIS_TG,  XXXXXXX, OS_TG,   KC_INS,  _______},
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

void tap_key(uint16_t kc) {
    register_code(kc);
    unregister_code(kc);
}

void alert_extra(bool b) {
    #ifdef AUDIO_ENABLE
    beep_bool(b);
    #endif
    #ifdef BACKLIGHT_ENABLE
    backlight_extra_bool(b);
    #endif
}

void set_layer(uint8_t layer, bool is_enable) {
    if (is_enable) {
        layer_on(layer);
    }
    else {
        layer_off(layer);
    }
}

void switch_extra(void) {
    if (IS_RAISE_ON && IS_LOWER_ON) {
        layer_on(_EXTRA);
        is_lower_held = false;
    }
    else {
        layer_off(_EXTRA);
        #ifdef BACKLIGHT_ENABLE
        backlight_extra_off();
        #endif
    }
}

void matrix_init_user(void) {
    is_lower_held    = false;
    is_lower_pressed = false;

    config.raw = eeconfig_read_user();
    set_single_persistent_default_layer(_QWERTY);
    set_layer(_ORIGINAL, config.is_orig_layout_enabled);
    set_layer(_MOD_SC, config.is_capslock_swapped);
    set_layer(_MOD_MAC, config.is_mac);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool is_left_pressed, is_right_pressed;
    switch (keycode) {
        // layer
        case RAISE:
            if (record->event.pressed) {
                layer_on(config.is_jis ? _RAISE_JIS : _RAISE_US);
            }
            else {
                layer_off(_RAISE_US);
                layer_off(_RAISE_JIS);
            }
            switch_extra();
            return false;
            break;
        case LOWER:
            if (record->event.pressed) {
                layer_on(_LOWER);
            }
            else if (!is_lower_held) {
                layer_off(_LOWER);
            }
            switch_extra();
            is_lower_pressed = record->event.pressed;
            return false;
            break;
        case NUMERIC:
            if (record->event.pressed) {
                set_layer(_NUMERIC, !IS_NUMERIC_ON);
                #ifdef BACKLIGHT_ENABLE
                backlight_numeric(IS_NUMERIC_ON);
                #endif
            }
            return false;
            break;

        case HLD_LOW:
            if (record->event.pressed && !is_lower_pressed) {
                layer_off(_LOWER);
            }
            else {
                is_lower_held = record->event.pressed;
            }
            return false;
            break;

        // Toggle settings
        case LAYO_TG:
            if (record->event.pressed) {
                config.is_orig_layout_enabled ^= 1;
                set_layer(_ORIGINAL, config.is_orig_layout_enabled);
                alert_extra(config.is_orig_layout_enabled);
            }
            return false;
            break;
        case RMOD_TG:
            if (record->event.pressed) {
                config.is_rmods_enabled ^= 1;
                set_layer(_RMODS, config.is_rmods_enabled);
                alert_extra(config.is_rmods_enabled);
            }
            return false;
            break;
        case ZHTG_TG:
            if (record->event.pressed) {
                config.is_zhtg_replaced ^= 1;
                alert_extra(config.is_zhtg_replaced);
            }
            return false;
            break;
        case JIS_TG:
            if (record->event.pressed) {
                config.is_jis ^= 1;
                set_layer(_RAISE_US, !config.is_jis);
                set_layer(_RAISE_JIS, config.is_jis);
                alert_extra(config.is_jis);
            }
            return false;
            break;
        case SWP_CAP:
            if (record->event.pressed) {
                config.is_capslock_swapped ^= 1;
                set_layer(_MOD_SC, config.is_capslock_swapped);
                alert_extra(config.is_capslock_swapped);
            }
            return false;
            break;
        case OS_TG:
            if (record->event.pressed) {
                config.is_mac ^= 1;
                set_layer(_MOD_MAC, config.is_mac);
                alert_extra(config.is_mac);
            }
            return false;
            break;

        // "=" for jis
        case KC_EQL:
            if (record->event.pressed && config.is_jis && IS_RAISE_OFF) {
                if (HAS_SFT) {
                    tap_key(KC_SCLN);
                }
                else {
                    add_weak_mods(MOD_LSFT);
                    tap_key(JP_MINS);
                    del_weak_mods(MOD_LSFT);
                }
                return false;
            }
            return true;
            break;

        // ":" and for jis and Ctrl-; send Ctrl-[
        case KC_SCLN:
            if (record->event.pressed) {
                if (HAS_CTL) {
                    tap_key(KC_LBRC);
                    return false;
                }
                else if (config.is_jis && HAS_SFT && IS_RAISE_OFF) {
                    is_left_pressed  = HAS_LSFT;
                    is_right_pressed = HAS_RSFT;
                    if (is_left_pressed)  unregister_code(KC_LSFT);
                    if (is_right_pressed) unregister_code(KC_RSFT);

                    tap_key(JP_COLN);

                    if (is_left_pressed)  register_code(KC_LSFT);
                    if (is_right_pressed) register_code(KC_RSFT);
                    return false;
                }
            }
            return true;
            break;

        // "_" for jis and capslock
        case KC_MINS:
            if (record->event.pressed && ((config.is_jis && HAS_SFT) || IS_CAPSLOCK_ENABLE) && IS_RAISE_OFF) {
                if (IS_CAPSLOCK_ENABLE && HAS_SFT) {
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
                    tap_key(config.is_jis ? JP_BSLS : KC_MINS);
                    del_weak_mods(MOD_LSFT);
                }
                return false;
            }
            return true;
            break;

        // replace "(" with  ")"  when pressed Shift
        // replace "(" with "Tab" when pressed Ctrl or Alt
        case KC_LPRN:
            if (record->event.pressed) {
                if (HAS_CTL || HAS_ALT || HAS_GUI) {
                    tap_key(KC_TAB);
                    return false;
                }
                else if (HAS_SFT) {
                    tap_key(config.is_jis ? KC_9 : KC_0);
                    return false;
                }
                else if (config.is_jis) {
                    add_weak_mods(MOD_LSFT);
                    tap_key(KC_8);
                    clear_weak_mods();
                    return false;
                }
            }
            return true;
            break;

        // replace "Ctrl-Space" when is_zhtg_replaced is true
        case KC_SPC:
            if (record->event.pressed && config.is_zhtg_replaced && HAS_CTL
                    && IS_RAISE_OFF && IS_LOWER_OFF && !HAS_SFT && !HAS_GUI && !HAS_ALT) {
                is_left_pressed  = HAS_LCTL;
                is_right_pressed = HAS_RCTL;
                if (is_left_pressed)  unregister_code(KC_LCTL);
                if (is_right_pressed) unregister_code(KC_RCTL);

                if (config.is_jis) {
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

        case FLS_ROM:
            if (record->event.pressed) {
                eeconfig_update_user(config.raw);
                alert_extra(true);
            }
            return false;
            break;

#ifdef BACKLIGHT_ENABLE
        case RESET:
            if (record->event.pressed) {
                backlight_strong();
            }
            return true;
            break;
#endif
    }
    return true;
}
