#include QMK_KEYBOARD_H
#include <limits.h>
#include "myutil.h"

#undef LAYOUT
#define LAYOUT( \
    k00, k01, k02, k03, \
    k10, k11, k12, k13  \
) { \
    {k13, k12, k11, k10}, \
    {k03, k02, k01, k00}  \
}

#define SEED 12345
#define PASSWORD "password"
#define LIMIT_COUNT 1000

typedef union {
    uint32_t raw;
    struct {
		uint16_t seed: 16;
		uint16_t count: 16;
    };
} user_config_t;
user_config_t config;

enum layers {
	HOME,
	INPUT_PIN,
	INPUT_JUMP
};

enum custom_keycodes {
	START = SAFE_RANGE,
	JUMP,
	END_PIN,
	END_JUMP,
	ESC,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5
};

#define BUFFER_LENGTH 16
#define BASE_NUMBER 6
uint8_t buffer[BUFFER_LENGTH];
uint8_t digit = 0;

uint32_t myrand_x = 0;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [HOME] = LAYOUT(
		START, _______, _______, _______,
		JUMP,  _______, _______, _______
    ),
	[INPUT_PIN] = LAYOUT(
		END_PIN, KEY_0, KEY_1, KEY_2,
		ESC,     KEY_3, KEY_4, KEY_5
	),
	[INPUT_JUMP] = LAYOUT(
		END_JUMP, KEY_0, KEY_1, KEY_2,
		ESC,      KEY_3, KEY_4, KEY_5
	)
};

void initialize_buffer(void) {
	for (int i=0; i<BUFFER_LENGTH; ++i) {
		buffer[i] = 0;
	}

	digit = 0;
}

void add_buffer(uint8_t number) {
	if (digit < BUFFER_LENGTH) {
		buffer[digit++] = number;
	}
}

uint32_t get_buffer_as_base(uint8_t base) {
	uint32_t result = 0;

	for (int i=0; i<digit; ++i) {
		result = result * base + buffer[i];
	}

	return result <= 0x7FFFFFFF ? result : -1;
}

void mysrand(uint32_t seed) {
  myrand_x = seed;
}

uint32_t myrand(void) {
  myrand_x = myrand_x * 524287 + 6700417;
  return myrand_x & 0x7FFFFFFF;
}

void matrix_init_user(void) {
    config.raw = eeconfig_read_user();
	
	if (config.seed != SEED) {
		config.seed = SEED;
		config.count = 0;
		eeconfig_update_user(config.raw);
	}

	initialize_buffer();
	mysrand(SEED);

	for (int i=0; i<config.count; ++i) {
		myrand();
	}
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	uint32_t temp;

	if (!record->event.pressed) {
		return false;
	}

    switch (keycode) {
		case START:
			if (config.count < LIMIT_COUNT) {
				layer_on(INPUT_PIN);
			}

			break;
		case JUMP:
			layer_on(INPUT_JUMP);
			break;
		case END_PIN:
			temp = myrand();

			if (get_buffer_as_base(BASE_NUMBER) == temp) {
				SEND_STRING(PASSWORD);
				SEND_STRING("\n");
			}

			++config.count;
			eeconfig_update_user(config.raw);
			layer_off(INPUT_PIN);
			initialize_buffer();
			break;
		case END_JUMP:
			temp = get_buffer_as_base(10);

			if (temp > config.count && temp < LIMIT_COUNT) {
				for (int i=config.count; i<temp; ++i) {
					myrand();
				}

				config.count = temp;
				eeconfig_update_user(config.raw);
			}

			layer_off(INPUT_JUMP);
			initialize_buffer();
			break;
		case ESC:
			layer_off(INPUT_PIN);
			layer_off(INPUT_JUMP);
			initialize_buffer();
			break;

		case KEY_0:
			add_buffer(0);
			break;
		case KEY_1:
			add_buffer(1);
			break;
		case KEY_2:
			add_buffer(2);
			break;
		case KEY_3:
			add_buffer(3);
			break;
		case KEY_4:
			add_buffer(4);
			break;
		case KEY_5:
			add_buffer(5);
			break;
    }

    return false;
}
