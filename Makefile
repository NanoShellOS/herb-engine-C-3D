APPLICATION_NAME = HerbEngine

SRC_DIR = .

APP_C_FILES = \
	./nanoshell/fastsort.c \
	./nanoshell/main.c \
	./nanoshell/math.c \
	./herbengineC3D.c

APP_S_FILES=

RESOURCE = nanoshell/resource.rc
LINK_SCRIPT = nanoshell/link.ld

include ../CommonMakefile
