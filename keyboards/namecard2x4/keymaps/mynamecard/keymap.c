#include QMK_KEYBOARD_H

#undef LAYOUT
#define LAYOUT( \
    k00, k01, k02, k03, \
    k10, k11, k12, k13  \
) { \
    {k13, k12, k11, k10}, \
    {k03, k02, k01, k00}  \
}

#define SWITCH_NAME1 "Kailh Pro Burgundy"
#define SWITCH_NAME2 "Kailh Box Black"
#define SWITCH_NAME3 "Kailh Brown"
#define SWITCH_NAME4 "Kailh Pink"
#define SWITCH_NAME5 "Gateron Silent red 35g"
#define SWITCH_NAME6 "TTC Silent red"
#define SWITCH_NAME7 "Aliaz Silent 60g"
#define SWITCH_NAME8 "R11 Blue Zilent 62g"

uint8_t mouse_bit;
bool is_PRSW_pressed;

enum layers {
    DEF,
    LAY1, LAY2, LAY3, LAY4,
    LAY5, LAY6, LAY7, LAY8,
    MOUSE, PRSW
};

enum additional_keycodes {
    PRSW1 = SAFE_RANGE,
    PRSW2, PRSW3, PRSW4, PRSW5, PRSW6, PRSW7, PRSW8
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [DEF] = LAYOUT(
        LT(LAY1, KC_A), LT(LAY2, KC_B), LT(LAY3, KC_C), LT(LAY4, KC_D),
        LT(LAY5, KC_E), LT(LAY6, KC_F), LT(LAY7, KC_G), LT(LAY8, KC_H)
    ),

    [LAY1] = LAYOUT(
        _______,       KC_COMM,       KC_DOT,        KC_SLSH,
        OSM(MOD_LGUI), OSM(MOD_LALT), OSM(MOD_LCTL), OSM(MOD_LSFT)
    ),

    [LAY2] = LAYOUT(
        TO(MOUSE), _______, KC_UP,   TO(PRSW),
        KC_LEFT,   RESET,   KC_DOWN, KC_RGHT
    ),

    [LAY3] = LAYOUT(
        KC_1, KC_2, _______, KC_MINS,
        KC_3, KC_4, KC_EQL,  KC_5
    ),

    [LAY4] = LAYOUT(
        KC_U, KC_V, KC_W, _______,
        KC_X, KC_Y, KC_Z, KC_BSLS
    ),

    [LAY5] = LAYOUT(
        KC_SCLN, KC_I, KC_J, KC_K,
        _______, KC_L, KC_M, KC_N
    ),

    [LAY6] = LAYOUT(
        KC_TAB, KC_GRV,   KC_DEL,  KC_ESC,
        KC_SPC, _______,  KC_BSPC, KC_ENT
    ),

    [LAY7] = LAYOUT(
        KC_6, KC_7, KC_LBRC, KC_RBRC,
        KC_8, KC_9, _______, KC_0
    ),

    [LAY8] = LAYOUT(
        KC_O, KC_P, KC_Q, KC_QUOT,
        KC_R, KC_S, KC_T, _______
    ),

    [MOUSE] = LAYOUT(
        KC_WH_D, KC_MS_U, KC_WH_U, KC_BTN2,
        KC_MS_L, KC_MS_D, KC_MS_R, KC_BTN1
    ),

    [PRSW] = LAYOUT(
        PRSW1, PRSW2, PRSW3, PRSW4,
        PRSW5, PRSW6, PRSW7, PRSW8
    )
};

void matrix_init_user(void) {
    mouse_bit = 0;
    is_PRSW_pressed = false;
}

void print_switch_name(int switch_num) {
    switch (switch_num) {
        case 1: SEND_STRING(SWITCH_NAME1); break;
        case 2: SEND_STRING(SWITCH_NAME2); break;
        case 3: SEND_STRING(SWITCH_NAME3); break;
        case 4: SEND_STRING(SWITCH_NAME4); break;
        case 5: SEND_STRING(SWITCH_NAME5); break;
        case 6: SEND_STRING(SWITCH_NAME6); break;
        case 7: SEND_STRING(SWITCH_NAME7); break;
        case 8: SEND_STRING(SWITCH_NAME8); break;
    }
    SEND_STRING(" ");
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_MS_U ... KC_MS_R:
            if (record->event.pressed) {
                mouse_bit |= 1 << (keycode - KC_MS_U);
                if (mouse_bit >= 0xF) {
                    layer_off(MOUSE);
                    mouse_bit = 0;
                    return false;
                }
            }
            else {
                mouse_bit &= ~(1 << (keycode - KC_MS_U));
            }
            return true;
            break;

        case PRSW1 ... PRSW8:
            if (record->event.pressed) {
                if (is_PRSW_pressed) {
                    layer_off(PRSW);
                    is_PRSW_pressed = false;
                    return false;
                }
                else {
                    is_PRSW_pressed = true;
                }
            }
            else if (IS_LAYER_ON(PRSW)){
                print_switch_name(keycode - PRSW1 + 1);
                is_PRSW_pressed = false;
            }
            return false;
            break;
    }

    return true;
}
