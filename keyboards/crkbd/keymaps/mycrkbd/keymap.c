#include QMK_KEYBOARD_H
#include "bootloader.h"
#include "keymap_jp.h"
#include "myutil.h"

#ifdef PROTOCOL_LUFA
#include "lufa.h"
#include "split_util.h"
#endif

#ifdef SSD1306OLED
#include "ssd1306.h"
#endif

#define OSM_SFT OSM(MOD_LSFT)
#define NUMERIC TG(_NUMERIC)

#define IS_RAISE_ON  (IS_LAYER_ON(_RAISE_US)  || IS_LAYER_ON(_RAISE_JIS))
#define IS_RAISE_OFF (IS_LAYER_OFF(_RAISE_US) && IS_LAYER_OFF(_RAISE_JIS))
#define IS_LOWER_ON    IS_LAYER_ON(_LOWER)
#define IS_LOWER_OFF   IS_LAYER_OFF(_LOWER)
#define IS_EXTRA_ON    IS_LAYER_ON(_EXTRA)
#define IS_EXTRA_OFF   IS_LAYER_OFF(_EXTRA)
#define IS_NUMERIC_ON  IS_LAYER_ON(_NUMERIC)
#define IS_NUMERIC_OFF IS_LAYER_OFF(_NUMERIC)

extern keymap_config_t keymap_config;
extern uint8_t is_master;

typedef union {
    uint32_t raw;
    struct {
        bool is_oled_on             : 1;
        bool is_orig_layout_enabled : 1;
        bool is_zhtg_replaced       : 1;
        bool is_mac                 : 1;
        bool is_jis                 : 1;
    };
} user_config_t;
user_config_t config;

bool is_oled_extra_on;
bool is_lower_held;
bool is_lower_pressed;
bool is_reset_pressed;
bool is_mods_swapped;
bool flashed_eeprom;

enum layers {
    _QWERTY,
    _ORIGINAL,
    _MOD_MAC,
    _SWAP_MOD,
    _SWAP_MAC_MOD,
    _RAISE_US,
    _RAISE_JIS,
    _LOWER,
    _EXTRA,
    _NUMERIC
};

enum custom_keycodes {
    RAISE = SAFE_RANGE,
    LOWER,
    HLD_LOW,
    LAYO_TG,
    ZHTG_TG,
    JIS_TG,
    SWP_CAP,
    OS_TG,
    MOD_TG,
    FLS_ROM,
    OLED_TG
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Qwerty
     * ,-----------------------------------------.                ,-----------------------------------------.
     * |   (  |   Q  |   W  |   E  |   R  |   T  |                |   Y  |   U  |   I  |   O  |   P  |   =  |
     * |------+------+------+------+------+------|                |------+------+------+------+------+------|
     * | Ctrl |   A  |   S  |   D  |   F  |   G  |                |   H  |   J  |   K  |   L  |   ;  |RAISE |
     * |------+------+------+------+------+------|                |------+------+------+------+------+------|
     * |Shift |   Z  |   X  |   C  |   V  |   B  |                |   N  |   M  |   ,  |   .  |   /  |   -  |
     * `---------------------------+------+------+------.  ,------+------+------+---------------------------'
     *                             |LOWER |Space | Alt  |  | Bksp |Enter | GUI  |
     *                             `--------------------'  `--------------------'
     */

    [_QWERTY] = LAYOUT( \
        KC_LPRN, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_EQL,  \
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                      KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, RAISE,   \
        OSM_SFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                      KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_MINS, \
                                            LOWER,   KC_SPC,  KC_LALT, KC_BSPC, KC_ENT,  KC_LGUI \
    ),

    /* Orignal
     * ,-----------------------------------------.                ,-----------------------------------------.
     * |      |   Y  |   W  |   E  |   R  |   F  |                |   Q  |   U  |   I  |   O  |   P  |      |
     * |------+------+------+------+------+------|                |------+------+------+------+------+------|
     * |      |   A  |   S  |   D  |   T  |   G  |                |   H  |   J  |   K  |   L  |   ;  |      |
     * |------+------+------+------+------+------|                |------+------+------+------+------+------|
     * |      |   Z  |   X  |   V  |   C  |   B  |                |   N  |   M  |   ,  |   .  |   /  |      |
     * `---------------------------+------+------+------.  ,------+------+------+---------------------------'
     *                             |      |      |      |  |      |      |      |
     *                             `--------------------'  `--------------------'
     */

    [_ORIGINAL] = LAYOUT( \
        _______, KC_Y,    KC_W,    KC_E,    KC_R,    KC_F,                      KC_Q,    KC_U,    KC_I,    KC_O,    KC_P,    _______, \
        _______, KC_A,    KC_S,    KC_D,    KC_T,    KC_G,                      KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, _______, \
        _______, KC_Z,    KC_X,    KC_V,    KC_C,    KC_B,                      KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, _______, \
                                            _______, _______, _______, _______, _______, _______ \
    ),

    [_MOD_MAC] = LAYOUT( \
        _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
                                            _______, _______, KC_LGUI, _______, _______, KC_LALT \
    ),

    [_SWAP_MOD] = LAYOUT( \
        _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
        KC_RCTL, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
        KC_RSFT, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
                                            _______, _______, KC_RALT, _______, _______, KC_RGUI \
    ),

    [_SWAP_MAC_MOD] = LAYOUT( \
        _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
        KC_RCTL, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
        KC_RSFT, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
                                            _______, _______, KC_RGUI, _______, _______, KC_RALT \
    ),

    /* Raise
     * ,-----------------------------------------.                ,-----------------------------------------.
     * | Tab  |   !  |   @  |   #  |   $  |   %  |                |      |   ^  |   &  |   *  | Del  | Esc  |
     * |------+------+------+------+------+------|                |------+------+------+------+------+------|
     * | Ctrl |   "  |   '  |   {  |   [  |   }  |                |   ]  |   |  |   \  |   ~  |   `  |RAISE |
     * |------+------+------+------+------+------|                |------+------+------+------+------+------|
     * |Shift |      |      |      |      | Caps |                |      |      |      |      |      |      |
     * `---------------------------+------+------+------.  ,------+------+------+---------------------------'
     *                             |LOWER |Space | Alt  |  | Bksp |Enter | GUI  |
     *                             `--------------------'  `--------------------'
     */

    [_RAISE_US] = LAYOUT( \
        KC_TAB,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                   XXXXXXX, KC_CIRC, KC_AMPR, KC_ASTR, KC_DEL,  KC_ESC,  \
        _______, KC_DQT,  KC_QUOT, KC_LCBR, KC_LBRC, KC_RCBR,                   KC_RBRC, KC_PIPE, KC_BSLS, KC_TILD, KC_GRV,  _______, \
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_CAPS,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
                                            _______, _______, _______, _______, _______, _______ \
    ),

    [_RAISE_JIS] = LAYOUT( \
        KC_TAB,  KC_EXLM, JP_AT,   KC_HASH, KC_DLR,  KC_PERC,                   XXXXXXX, JP_CIRC, JP_AMPR, JP_ASTR, KC_DEL,  KC_ESC,  \
        _______, JP_DQT,  JP_QUOT, JP_LCBR, JP_LBRC, JP_RCBR,                   JP_RBRC, JP_PIPE, JP_BSLS, JP_TILD, JP_GRV,  _______, \
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_CAPS,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
                                            _______, _______, _______, _______, _______, _______ \
    ),

    /* Lower
     * ,-----------------------------------------.                ,-----------------------------------------.
     * | Tab  |   1  |   2  |   3  |   4  |   5  |                |   6  |   7  |   8  |   9  |   0  | Esc  |
     * |------+------+------+------+------+------|                |------+------+------+------+------+------|
     * | Ctrl | Home | PgDn | PgUp | End  |      |                | Left | Down |  Up  |Right | Del  |RAISE |
     * |------+------+------+------+------+------|                |------+------+------+------+------+------|
     * |Shift | NUM  |      |      |HLDLOW|OLEDTG|                |      |      |   ,  |   .  |HLDLOW|      |
     * `---------------------------+------+------+------.  ,------+------+------+---------------------------'
     *                             |LOWER |Space | Alt  |  | Bksp |Enter | GUI  |
     *                             `--------------------'  `--------------------'
     */

    [_LOWER] = LAYOUT( \
        KC_TAB,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_ESC,  \
        _______, KC_HOME, KC_PGDN, KC_PGUP, KC_END,  XXXXXXX,                   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_DEL,  _______, \
        _______, NUMERIC, XXXXXXX, XXXXXXX, HLD_LOW, OLED_TG,                   XXXXXXX, XXXXXXX, KC_COMM, KC_DOT,  HLD_LOW, XXXXXXX, \
                                            _______, _______, _______, _______, _______, _______ \
    ),

    /* Extra
     * ,-----------------------------------------.                ,-----------------------------------------.
     * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |                |  F7  |  F8  |  F9  | F10  | F11  | F12  |
     * |------+------+------+------+------+------|                |------+------+------+------+------+------|
     * | Ctrl | App  |      |UPDROM|      |Reset |                |      |JIS_TG|      |OS_TGL|Insert|RAISE |
     * |------+------+------+------+------+------|                |------+------+------+------+------+------|
     * |Shift |ZHTGTG|      |      |LAYOTG|      |                |NumLc |SWPMOD| PtSc | ScLc |Pause |      |
     * `---------------------------+------+------+------.  ,------+------+------+---------------------------'
     *                             |LOWER |Space | Alt  |  | Bksp |Enter | GUI  |
     *                             `--------------------'  `--------------------'
     */

    [_EXTRA] = LAYOUT( \
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  \
        _______, KC_APP,  XXXXXXX, FLS_ROM, XXXXXXX, RESET,                     XXXXXXX, JIS_TG,  XXXXXXX, OS_TG,   KC_INS,  _______, \
        _______, ZHTG_TG, XXXXXXX, SWP_CAP, LAYO_TG, XXXXXXX,                   KC_NLCK, MOD_TG,  KC_PSCR, KC_SLCK, KC_PAUS, XXXXXXX, \
                                            _______, _______, _______, _______, _______, _______ \
    ),

    /* Numeric
     * ,-----------------------------------------.                ,-----------------------------------------.
     * | NUM  |      |      |  Up  |      |      |                |   7  |   8  |   9  |   -  |   /  | Bksp |
     * |------+------+------+------+------+------|                |-------------+------+------+------+------|
     * |      |      | Left | Down |Right |      |                |   4  |   5  |   6  |   +  |   *  | Del  |
     * |------+------+------+------+------+------|                |------|------+------+------+------+------|
     * |      |      |      |      |      |      |                |   1  |   2  |   3  |Enter |      |      |
     * `---------------------------+------+------+------.  ,------+------+------+---------------------------'
     *                             |      |      |      |  |Space |   0  |   .  |
     *                             `--------------------'  `--------------------'
     */

    [_NUMERIC] = LAYOUT( \
        NUMERIC, XXXXXXX, XXXXXXX, KC_UP,   XXXXXXX, XXXXXXX,                   KC_P7,   KC_P8,   KC_P9,   KC_PMNS, KC_PSLS, KC_BSPC, \
        XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX,                   KC_P4,   KC_P5,   KC_P6,   KC_PPLS, KC_PAST, KC_DEL,  \
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   KC_P1,   KC_P2,   KC_P3,   KC_PENT, XXXXXXX, XXXXXXX, \
                                            _______, _______, _______, KC_SPC,  KC_P0,   KC_PDOT \
    )
};

void tap_key(uint16_t kc) {
    register_code(kc);
    unregister_code(kc);
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
        is_oled_extra_on = false;
        flashed_eeprom = false;
    }
}

void set_mod_layer(void) {
    set_layer(_MOD_MAC,      config.is_mac && !is_mods_swapped);
    set_layer(_SWAP_MOD,    !config.is_mac &&  is_mods_swapped);
    set_layer(_SWAP_MAC_MOD, config.is_mac &&  is_mods_swapped);
}

void matrix_init_user(void) {
    is_lower_pressed = false;
    is_lower_held    = false;
    is_oled_extra_on = false;
    is_mods_swapped  = false;
    is_reset_pressed = false;
    flashed_eeprom   = false;

    config.raw = eeconfig_read_user();
    set_single_persistent_default_layer(_QWERTY);
    set_layer(_ORIGINAL, config.is_orig_layout_enabled);
    set_layer(_MOD_MAC, config.is_mac);

    #ifdef SSD1306OLED
        #ifdef MASTER_RIGHT
        iota_gfx_init(has_usb());
        #else
        iota_gfx_init(!has_usb());
        #endif
    #endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool is_left_pressed, is_right_pressed;
    switch (keycode) {
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
                if (IS_NUMERIC_ON) {
                    layer_off(_NUMERIC);
                }
                else {
                    layer_off(_LOWER);
                    is_lower_pressed = false;
                    layer_on(_NUMERIC);
                }
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

        case LAYO_TG:
            if (record->event.pressed) {
                config.is_orig_layout_enabled ^= 1;
                set_layer(_ORIGINAL, config.is_orig_layout_enabled);
                is_oled_extra_on = true;
            }
            return false;
            break;
        case ZHTG_TG:
            if (record->event.pressed) {
                config.is_zhtg_replaced ^= 1;
                is_oled_extra_on = true;
            }
            return false;
            break;
        case JIS_TG:
            if (record->event.pressed) {
                config.is_jis ^= 1;
                set_layer(_RAISE_US, !config.is_jis);
                set_layer(_RAISE_JIS, config.is_jis);
                is_oled_extra_on = true;
            }
            return false;
            break;
        case OS_TG:
            if (record->event.pressed) {
                config.is_mac ^= 1;
                set_mod_layer();
                is_oled_extra_on = true;
            }
            return false;
            break;
        case MOD_TG:
            if (record->event.pressed) {
                is_mods_swapped ^= 1;
                set_mod_layer();
                is_oled_extra_on = true;
            }
            return false;
            break;
        case OLED_TG:
            if (record->event.pressed) {
                config.is_oled_on ^= 1;
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
                    clear_weak_mods();
                }
                return false;
            }
            return true;
            break;

        // ":" and for jis and "Ctrl-;" send "Ctrl-["
        case KC_SCLN:
            if (record->event.pressed && IS_RAISE_OFF) {
                if (HAS_CTL) {
                    tap_key(KC_LBRC);
                    return false;
                }
                else if (config.is_jis && HAS_SFT) {
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
                    clear_weak_mods();
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
                    clear_weak_mods();
                }

                if (is_left_pressed)  register_code(KC_LCTL);
                if (is_right_pressed) register_code(KC_RCTL);
                return false;
            }
            return true;
            break;

        // "Ctrl-[" send "Ctrl-;"
        case KC_LBRC:
            if (record->event.pressed && !config.is_jis && IS_RAISE_ON && HAS_CTL) {
                tap_key(KC_SCLN);
                return false;
            }
            return true;
            break;
        case JP_LBRC:
            if (record->event.pressed && config.is_jis && IS_RAISE_ON && HAS_CTL) {
                tap_key(KC_SCLN);
                return false;
            }
            return true;
            break;

        // "PrintScreen" for Mac
        case KC_PSCR:
            if (record->event.pressed && config.is_mac) {
                if (HAS_CTL) {
                    is_left_pressed  = HAS_LCTL;
                    is_right_pressed = HAS_RCTL;
                    if (is_left_pressed)  unregister_code(KC_LCTL);
                    if (is_right_pressed) unregister_code(KC_RCTL);

                    add_weak_mods(MOD_LGUI | MOD_LSFT);
                    tap_key(KC_4);
                    clear_weak_mods();

                    if (is_left_pressed)  register_code(KC_LCTL);
                    if (is_right_pressed) register_code(KC_RCTL);
                }
                else {
                    add_weak_mods(MOD_LGUI | MOD_LSFT);
                    tap_key(KC_3);
                    clear_weak_mods();
                }
                return false;
            }
            return true;
            break;

        case FLS_ROM:
            if (record->event.pressed) {
                eeconfig_update_user(config.raw);
                flashed_eeprom = true;
                is_oled_extra_on = true;
            }
            return false;
            break;

        case RESET:
            if (record->event.pressed) {
                is_reset_pressed = true;
                matrix_scan();
            }
            return true;
            break;
    }
    return true;
}

//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED
void matrix_scan_user(void) {
    iota_gfx_task(); // this is what updates the display continuously
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
    if (memcmp(dest->display, source->display, sizeof(dest->display))) {
        memcpy(dest->display, source->display, sizeof(dest->display));
        dest->dirty = true;
    }
}

void render_status(struct CharacterMatrix *matrix) {
    /* ,---------------------.
     * |lo    US   [ Layer ] |
     * |go  Qwerty  NumLock  |
     * | SwapMods  CapsLock  |
     * |  EJRep   ScrollLock |
     * `---------------------'
     */
    static char os_logo[][2][3]={{{0x95, 0x96, 0}, {0xB5, 0xB6, 0}}, {{0x97, 0x98, 0}, {0xB7, 0xB8, 0}}};
    char led[24];
    snprintf(led, sizeof(led), "%s   %s   [%s]\n",
            os_logo[config.is_mac ? 0 : 1][0],
            config.is_jis ? "JIS"
                          : " US",
            IS_NUMERIC_ON ? "Numeric"
          : IS_EXTRA_ON   ? " Extra "
          : IS_LOWER_ON   ? " Lower "
          : IS_RAISE_ON   ? " Raise "
                          : "Default"
    );
    matrix_write(matrix, led);

    snprintf(led, sizeof(led), "%s %s %s\n",
            os_logo[config.is_mac ? 0 : 1][1],
            config.is_orig_layout_enabled ? "Original"
                                          : " Qwerty ",
            IS_NUMLOCK_ENABLE             ? "NumLock"
                                          : "       "
    );
    matrix_write(matrix, led);

    snprintf(led, sizeof(led), " %s  %s\n",
            is_mods_swapped    ? "SwapMods"
                               : "        ",
            IS_CAPSLOCK_ENABLE ? "CapsLock"
                               : "        "
    );
    matrix_write(matrix, led);

    snprintf(led, sizeof(led), "  %s   %s",
            config.is_zhtg_replaced ? "EJRep"
                                    : "     ",
            IS_SCROLL_LOCK_ENABLE   ? "ScrollLock"
                                    : "          "
    );
    matrix_write(matrix, led);
}

#define EMPTY     {{   0,    0,    0,    0,    0, 0}, {   0,    0,    0,    0,    0, 0}, \
                   {   0,    0,    0,    0,    0, 0}, {   0,    0,    0,    0,    0, 0}}
#define CHAR_EXLM {{0xA4,    0,    0,    0,    0, 0}, {0xA4,    0,    0,    0,    0, 0}, \
                   {0x20,    0,    0,    0,    0, 0}, {0xA4,    0,    0,    0,    0, 0}}
#define CHAR_C    {{0x20, 0xA4, 0xA4, 0xA4,    0, 0}, {0xA4, 0x20, 0x20, 0x20,    0, 0}, \
                   {0xA4, 0x20, 0x20, 0x20,    0, 0}, {0x20, 0xA4, 0xA4, 0xA4,    0, 0}}
#define CHAR_D    {{0xA4, 0xA4, 0xA4, 0x20,    0, 0}, {0xA4, 0x20, 0x20, 0xA4,    0, 0}, \
                   {0xA4, 0x20, 0x20, 0xA4,    0, 0}, {0xA4, 0xA4, 0xA4, 0x20,    0, 0}}
#define CHAR_F    {{0xA4, 0xA4, 0xA4, 0xA4,    0, 0}, {0xA4, 0x20, 0x20, 0x20,    0, 0}, \
                   {0xA4, 0xA4, 0xA4, 0x20,    0, 0}, {0xA4, 0x20, 0x20, 0x20,    0, 0}}
#define CHAR_H    {{0xA4, 0x20, 0x20, 0xA4,    0, 0}, {0xA4, 0x20, 0x20, 0xA4,    0, 0}, \
                   {0xA4, 0xA4, 0xA4, 0xA4,    0, 0}, {0xA4, 0x20, 0x20, 0xA4,    0, 0}}
#define CHAR_K    {{0xA4, 0x20, 0x20,    0,    0, 0}, {0xA4, 0x20, 0xA4,    0,    0, 0}, \
                   {0xA4, 0xA4, 0x20,    0,    0, 0}, {0xA4, 0x20, 0xA4,    0,    0, 0}}
#define CHAR_L    {{0xA4, 0x20, 0x20, 0x20,    0, 0}, {0xA4, 0x20, 0x20, 0x20,    0, 0}, \
                   {0xA4, 0x20, 0x20, 0x20,    0, 0}, {0xA4, 0xA4, 0xA4, 0xA4,    0, 0}}
#define CHAR_M    {{0xA4, 0x20, 0x20, 0x20, 0xA4, 0}, {0xA4, 0xA4, 0x20, 0xA4, 0xA4, 0}, \
                   {0xA4, 0x20, 0xA4, 0x20, 0xA4, 0}, {0xA4, 0x20, 0x20, 0x20, 0xA4, 0}}
#define CHAR_N    {{0xA4, 0x20, 0x20, 0xA4,    0, 0}, {0xA4, 0xA4, 0x20, 0xA4,    0, 0}, \
                   {0xA4, 0x20, 0xA4, 0xA4,    0, 0}, {0xA4, 0x20, 0x20, 0xA4,    0, 0}}
#define CHAR_P    {{0xA4, 0xA4, 0xA4, 0x20,    0, 0}, {0xA4, 0x20, 0x20, 0xA4,    0, 0}, \
                   {0xA4, 0xA4, 0xA4, 0x20,    0, 0}, {0xA4, 0x20, 0x20, 0x20,    0, 0}}
#define CHAR_S    {{0x20, 0xA4, 0xA4, 0xA4,    0, 0}, {0x20, 0xA4, 0x20, 0x20,    0, 0}, \
                   {0x20, 0x20, 0xA4, 0x20,    0, 0}, {0xA4, 0xA4, 0xA4, 0x20,    0, 0}}
#define CHAR_U    {{0xA4, 0x20, 0x20, 0xA4,    0, 0}, {0xA4, 0x20, 0x20, 0xA4,    0, 0}, \
                   {0xA4, 0x20, 0x20, 0xA4,    0, 0}, {0x20, 0xA4, 0xA4, 0x20,    0, 0}}
#define MAX_LEN 5

void render_alart(struct CharacterMatrix *matrix) {
    /* ,---------------------.---------------------.---------------------.---------------------.---------------------.---------------------.
     * |@@@  @@@@ @  @       |@  @ @  @ @   @      |@ @  @ @  @ @    @   | @@@ @@@  @    @     | @@@  @@@ @    @     |@@@@ @     @@@ @  @  |
     * |@  @ @    @  @       |@@ @ @  @ @@ @@      |@ @@ @ @  @ @    @ @ |@    @  @ @    @ @   | @   @    @    @ @   |@    @     @   @  @  |
     * |@  @ @@@  @  @       |@ @@ @  @ @ @ @      |  @ @@ @  @ @    @@  |@    @@@  @    @@    |  @  @    @    @@    |@@@  @      @  @@@@  |
     * |@@@  @     @@        |@  @  @@  @   @      |@ @  @  @@  @@@@ @ @ | @@@ @    @@@@ @ @   |@@@   @@@ @@@@ @ @   |@    @@@@ @@@  @  @  |
     * `---------------------'---------------------'---------------------'---------------------'---------------------'---------------------'
     */
    static char str_alarts[][MAX_LEN][4][6] = {
        {CHAR_D,    CHAR_F, CHAR_U, EMPTY,  EMPTY},  // DFU Mode
        {CHAR_N,    CHAR_U, CHAR_M, EMPTY,  EMPTY},  // Numeric
        {CHAR_EXLM, CHAR_N, CHAR_U, CHAR_L, CHAR_K}, // !NumLock
        {CHAR_C,    CHAR_P, CHAR_L, CHAR_K, EMPTY},  // CapsLock
        {CHAR_S,    CHAR_C, CHAR_L, CHAR_K, EMPTY},  // ScrollLock
        {CHAR_F,    CHAR_L, CHAR_S, CHAR_H, EMPTY}   // Flash EEPROM
    };

    int index = is_reset_pressed      ? 0
              : IS_NUMERIC_ON         ? (IS_NUMLOCK_ENABLE || config.is_mac) ? 1 : 2
              : IS_SCROLL_LOCK_ENABLE ? 4
              : IS_CAPSLOCK_ENABLE    ? 3
                                      : -1;
    if (index < 0) return;

    for (int i=0; i<4; i++) {
        matrix_write(matrix, "\n");
        for (int j=0; j<MAX_LEN; j++) {
            matrix_write(matrix, str_alarts[index][j][i]);
            if (j != MAX_LEN - 1) {
                matrix_write(matrix, " ");
            }
        }
    }
}

void iota_gfx_task_user(void) {
    struct CharacterMatrix matrix;
    matrix_clear(&matrix);

    if (is_master) {
        if (is_reset_pressed || IS_NUMERIC_ON || (!is_oled_extra_on && (IS_CAPSLOCK_ENABLE || IS_SCROLL_LOCK_ENABLE))) {
            render_alart(&matrix);
        }
        else if (config.is_oled_on || is_oled_extra_on){
            render_status(&matrix);
        }
    }
    matrix_update(&display, &matrix);
}
#endif
