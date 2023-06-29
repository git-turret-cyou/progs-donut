PREFIX = /usr/local

CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700L -D_POSIX_C_SOURCE=200809L
CFLAGS = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os ${CPPFLAGS}
LDFLAGS = -lm -lX11 -lXext

CC = cc
