#ifndef MYUTIL_H
#define MYUTIL_H

#include "quantum.h"

#define HAS_LSFT ((get_mods() | get_weak_mods() | get_oneshot_mods()) & MOD_BIT(KC_LSFT))
#define HAS_LCTL ((get_mods() | get_weak_mods() | get_oneshot_mods()) & MOD_BIT(KC_LCTL))
#define HAS_LGUI ((get_mods() | get_weak_mods() | get_oneshot_mods()) & MOD_BIT(KC_LGUI))
#define HAS_LALT ((get_mods() | get_weak_mods() | get_oneshot_mods()) & MOD_BIT(KC_LALT))
#define HAS_RSFT ((get_mods() | get_weak_mods() | get_oneshot_mods()) & MOD_BIT(KC_RSFT))
#define HAS_RCTL ((get_mods() | get_weak_mods() | get_oneshot_mods()) & MOD_BIT(KC_RCTL))
#define HAS_RGUI ((get_mods() | get_weak_mods() | get_oneshot_mods()) & MOD_BIT(KC_RGUI))
#define HAS_RALT ((get_mods() | get_weak_mods() | get_oneshot_mods()) & MOD_BIT(KC_RALT))
#define HAS_SFT  ((get_mods() | get_weak_mods() | get_oneshot_mods()) & (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT)))
#define HAS_CTL  ((get_mods() | get_weak_mods() | get_oneshot_mods()) & (MOD_BIT(KC_LCTL) | MOD_BIT(KC_RCTL)))
#define HAS_GUI  ((get_mods() | get_weak_mods() | get_oneshot_mods()) & (MOD_BIT(KC_LGUI) | MOD_BIT(KC_RGUI)))
#define HAS_ALT  ((get_mods() | get_weak_mods() | get_oneshot_mods()) & (MOD_BIT(KC_LALT) | MOD_BIT(KC_RALT)))

#define IS_NUMLOCK_ENABLE     (host_keyboard_leds() & (1 << USB_LED_NUM_LOCK))
#define IS_CAPSLOCK_ENABLE    (host_keyboard_leds() & (1 << USB_LED_CAPS_LOCK))
#define IS_SCROLL_LOCK_ENABLE (host_keyboard_leds() & (1 << USB_LED_SCROLL_LOCK))

#endif
