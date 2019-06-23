ifndef QUANTUM_DIR
  include ../../../../Makefile
endif

USER_NAME := oka-h

AUDIO_ENABLE = no
SRC += ./keymaps/myplanck/audio_user.c

ifneq ($(RGB_MATRIX_ENABLE), yes)
BACKLIGHT_ENABLE = yes
SRC += ./keymaps/myplanck/backlight_user.c
endif
