#
# UNIX make file (only tested on Linux)
#


#
# Source files
#
SRC=\
  action.c \
  ansiterm.c \
  bill.c \
  config.c \
  create.c \
  data.c   \
  diag.c   \
  display.c \
  fortune.c \
  global.c  \
  help.c    \
  iventory.c\
  io.c      \
  main.c    \
  monster.c \
  moreobj.c \
  movem.c   \
  object.c  \
  regen.c   \
  savelev.c \
  scores.c  \
  spheres.c \
  spells.c  \
  store.c   \
  sysdep.c  \
  tgoto.c   \
  tok.c


#
# goal: debug
#
ifeq ($(MAKECMDGOALS),debug)

CFLAGS=-pipe -Wall -Wextra -ansi -pedantic -Wpointer-arith \
-Wstrict-prototypes -Wmissing-prototypes -Wdeclaration-after-statement \
-Wshadow -Wmissing-declarations -Wold-style-definition -Wredundant-decls \
-g -D_FORTIFY_SOURCE=2 -DDEBUG -DUNIX
LDFLAGS=-lm -lcurses

STRIP_BINARY=no


#
# goal: release 
#
else ifeq ($(MAKECMDGOALS),release)

CFLAGS=-Os -pipe -Wall -fomit-frame-pointer -DUNIX 
LDFLAGS=-Wl,-O1 -lm -lcurses

STRIP_BINARY=yes

endif


OBJ=$(SRC:.c=.o)
EXE=./larn

CC=gcc
RM=rm -f


.PHONY : build
build: $(EXE)
	
	@echo Build complete!

ifeq ($(STRIP_BINARY),yes)
	@echo Stripping binary..
	@strip $(EXE)
endif


.PHONY: debug
debug: build

.PHONY: release
release: build

.PHONY : clean
clean:
	-$(RM) *.o
	-$(RM) $(EXE)

%.o: %.c
	@echo Building $<
	@$(CC) $(CFLAGS) -o $@ -c $<

$(EXE): $(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) -o $@

