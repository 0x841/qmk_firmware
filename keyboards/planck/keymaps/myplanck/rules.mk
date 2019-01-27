ifndef QUANTUM_DIR
  include ../../../../Makefile
endif

AUDIO_ENABLE     = no
BACKLIGHT_ENABLE = yes

USER_NAME := oka-h

SRC += ./keymaps/myplanck/audio_user.c \
       ./keymaps/myplanck/backlight_user.c
