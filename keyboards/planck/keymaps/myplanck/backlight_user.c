#include "backlight_user.h"

#include QMK_KEYBOARD_H
#include "action_layer.h"
#include "backlight.h"

#define IS_CAPSLOCK_ENABLE (host_keyboard_leds() & (1 << USB_LED_CAPS_LOCK))

bool is_numeric            = false;
bool is_extra_backlight_on = false;

void backlight_strong(void) {
    backlight_level(BACKLIGHT_LEVELS);
}

void backlight_weak(void) {
    backlight_level(BACKLIGHT_LEVELS / 2);
}

void backlight_off(void) {
    backlight_level(0);
}

void backlight_extra_bool(bool b) {
    is_extra_backlight_on = true;
    if (b) {
        backlight_strong();
    }
    else {
        backlight_weak();
    }
}

void backlight_numeric(bool is_enable) {
    is_numeric = is_enable;
    if (is_enable) {
        backlight_weak();
    }
    else {
        backlight_capslock();
    }
}

void backlight_extra_off(void) {
    is_extra_backlight_on = false;
    backlight_capslock();
}

void backlight_capslock(void) {
    if (IS_CAPSLOCK_ENABLE) {
        backlight_strong();
    }
    else {
        backlight_off();
    }
}

void led_set_user(uint8_t usb_led) {
    if (!(is_numeric || is_extra_backlight_on)) {
        if (usb_led & (1 << USB_LED_CAPS_LOCK)) {
            backlight_strong();
        }
        else {
            backlight_off();
        }
    }
}
