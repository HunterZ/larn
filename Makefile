############################################################################
#
# A list of available compile-time defines:
#
#   BSD           - use BSD specific features (mostly timer and signal stuff)
#   BSD4.1        - use BSD4.1 to avoid some 4.2 dependencies (must be used with
#                   BSD above; do not mix with SYSV)
#   DGK           - Don Kneller (ported Larn from Unix to MS-DOS) specific 
#                   changes.  None are MS-DOS specific.  This option will be 
#                   going away in a future version.
#   DGK_MSDOS     - MS-DOS specific Don Kneller code.  This will be merged with
#                   the MSDOS #define in a future version.
#   DECRainbow    - DEC Rainbow specific display code.
#   DOCHECKPOINTS - if not defined, checkpoint files are periodically written
#                   by the larn process (no forking) if enabled in the .larnopts
#                   description file.  Checkpointing is handy on an unreliable
#                   system, but takes CPU. Inclusion of DOCHECKPOINTS will cause
#                   fork()ing to perform the checkpoints (again if enabled in
#                   the .larnopts file).  This usually avoids pauses in larn
#                   while the checkpointing is being done (on large machines).
#   EXTRA         - incorporates code to gather additional performance stats
#   FLUSHNO=#     - Set the input queue excess flushing threshold (default 5)
#   HIDEBYLINK    - if defined, the program attempts to hide from ps
#   MACRORND      - Define to use macro version of rnd() and rund() (fast & big)
#   MAIL          - system supports mail messages (see bill.c).  Not useful on
#                   non-networked machines.
#   MSDOS         - MS-DOS specific code.
#   OS2LARN       - OS/2 Specific code.
#   NONAP         - causes napms() to return immediately instead of delaying
#                   n milliseconds.  This define may be needed on some systems
#                   if the nap stuff does not work correctly (possible hang).
#                   nap() is primarilly used to delay for effect when casting
#                   missile type spells.
#   NOVARARGS     - Define for systems that don't have varargs (a default 
#                   varargs will be used).
#   RFCMAIL       - mail messages are RFC822 conformant.  Must be used with 
#                   MAIL above.
#   SAVEINHOME    - put save files in users HOME instead of LARNHOME (default)
#   SIGTSTP       - define if your system provides the tty stop signal
#   SIGVTALRM     - define if your system supports the virtual time alarm signal
#   SYSV          - use system III/V (instead of V7) type ioctl calls
#   TIMECHECK     - incorporates code to disable play during working hours (8-5)
#   UIDSCORE      - Define to use user id's to manage scoreboard.  Leaving this
#                   out will cause player id's from the file ".playerids" to 
#                   be used instead.  (.playerids is created upon demand).  
#                   Only one entry per id # is allowed in each scoreboard
#                   (winning & non-winning).
#   VT100         - Compile for using vt100 family of terminals.  Omission of 
#                   this define will cause larn to use termcap.
#   WIZID=xxx     - this is the userid (or playerid) of the wizard.  Default is
#                   zero (superuser), which disables all wizard functions.
#                   Players must have this userid (or playerid) in order to
#                   become the non-scoring wizard player.  Definition of WIZID
#                   to non-zero will enable the special wizard debugging
#                   commands.  For root to become wizard, use WIZID= -1.
#
############################################################################
#
OBJ=	action.o bill.o config.o create.o data.o diag.o display.o \
	fortune.o global.o help.o io.o main.o monster.o \
	moreobj.o movem.o msdos.o nap.o object.o regen.o savelev.o \
	scores.o signal.o spells.o spheres.o store.o \
	tok.o vms.o
#
#  LARNHOME is the directory where the larn data files will be installed.
#
CFLAGS=-DDGK -DSYSV -DEXTRA -DRFCMAIL -D'LARNHOME="/home/hymie/lib/larn/"'

larn122: $(OBJ)
	cc -o larn122 $(OBJ) -ltermcap

.c.o: 
	cc $(CFLAGS) -c $*.c 

.c: header.h

