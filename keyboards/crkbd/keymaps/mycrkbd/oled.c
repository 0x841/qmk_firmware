#include "oled.h"

#include "ssd1306.h"
#include "myutil.h"

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
user_config_t *config;

bool is_oled_extra_on;
bool flashed_eeprom;
bool is_reset_pressed;

void render_status(struct CharacterMatrix *matrix) {
    /* ,---------------------.
     * |lo    US   [ Layer ] |
     * |go  Qwerty  NumLock  |
     * |   EJRep   CapsLock  |
     * |          ScrollLock |
     * `---------------------'
     */
    static char os_logo[][2][3]={{{0x95, 0x96, 0}, {0xB5, 0xB6, 0}}, {{0x97, 0x98, 0}, {0xB7, 0xB8, 0}}};
    char led[24];
    snprintf(led, sizeof(led), "%s   %s   [%s]\n",
            os_logo[(*config).is_mac ? 0 : 1][0],
            (*config).is_jis ? "JIS"
                          : " US",
            IS_NUMERIC_ON ? "Numeric"
          : IS_EXTRA_ON   ? " Extra "
          : IS_LOWER_ON   ? " Lower "
          : IS_RAISE_ON   ? " Raise "
                          : "Default"
    );
    matrix_write(matrix, led);

    snprintf(led, sizeof(led), "%s %s %s\n",
            os_logo[(*config).is_mac ? 0 : 1][1],
            (*config).is_orig_layout_enabled ? "Original"
                                          : " Qwerty ",
            IS_NUMLOCK_ENABLE             ? "NumLock"
                                          : "       "
    );
    matrix_write(matrix, led);

    snprintf(led, sizeof(led), "   %s   %s\n",
            (*config).is_zhtg_replaced ? "EJRep"
                                    : "     ",
            IS_CAPSLOCK_ENABLE      ? "CapsLock"
                                    : "        "
    );
    matrix_write(matrix, led);

    snprintf(led, sizeof(led), "          %s",
            IS_SCROLL_LOCK_ENABLE ? "ScrollLock"
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
              : IS_NUMERIC_ON         ? (IS_NUMLOCK_ENABLE || (*config).is_mac) ? 1 : 2
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

void init_oled(uint32_t *config_p) {
    config = config_p;

    is_oled_extra_on = false;
    flashed_eeprom   = false;
    is_reset_pressed = false;

    #ifdef MASTER_RIGHT
    iota_gfx_init(has_usb());
    #else
    iota_gfx_init(!has_usb());
    #endif
}

void show_oled_in_extra(void) {
    is_oled_extra_on = true;
}

void show_flash(void) {
    flashed_eeprom = true;
}

void show_reset(void) {
    is_reset_pressed = true;
}

void exit_extra(void) {
    is_oled_extra_on = false;
    flashed_eeprom   = false;
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
    if (memcmp(dest->display, source->display, sizeof(dest->display))) {
        memcpy(dest->display, source->display, sizeof(dest->display));
        dest->dirty = true;
    }
}

void iota_gfx_task_user(void) {
    struct CharacterMatrix matrix;
    matrix_clear(&matrix);

    if (is_master) {
        if (is_reset_pressed || IS_NUMERIC_ON || (!is_oled_extra_on && (IS_CAPSLOCK_ENABLE || IS_SCROLL_LOCK_ENABLE))) {
            render_alart(&matrix);
        }
        else if ((*config).is_oled_on || is_oled_extra_on){
            render_status(&matrix);
        }
    }
    matrix_update(&display, &matrix);
}

void matrix_scan_user(void) {
    iota_gfx_task();
}
