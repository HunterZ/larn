#
# MinGW (pure MinGW i.e. no MSYS) make file 
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

CFLAGS=-Wall -Wextra -ansi -pedantic -Wpointer-arith \
-Wstrict-prototypes -Wmissing-prototypes -Wdeclaration-after-statement \
-Wshadow -Wmissing-declarations -Wold-style-definition -Wredundant-decls \
-g -D_FORTIFY_SOURCE=2 -DDEBUG -DWINDOWS
LDFLAGS=-lpdcurses

STRIP_BINARY=no


#
# goal: release 
#
else ifeq ($(MAKECMDGOALS),release)

CFLAGS=-Os -Wall -fomit-frame-pointer -DWINDOWS
LDFLAGS=-Wl,-O1 -lpdcurses

STRIP_BINARY=yes

endif


OBJ=$(SRC:.c=.o)
EXE=larn.exe

CC=gcc
RM=del


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
	-$(RM) *.exe

%.o: %.c
	@echo Building $<
	@$(CC) $(CFLAGS) -o $@ -c $<

$(EXE): $(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) -o $@

