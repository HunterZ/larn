/*  main.c      */

#ifdef MSDOS
#include "errno.h"
#include "setjmp.h"
#include "stdlib.h"
#include "patchlev.h"
#endif

#include "header.h"
#ifndef MSDOS
#include "patchlevel.h"
# ifndef VMS
#  include <pwd.h>
# endif VMS
#endif MSDOS

static char copyright[]="\nLarn is copyrighted 1986 by Noah Morgan.\n";
int srcount=0;  /* line counter for showstr()   */
int dropflag=0; /* if 1 then don't lookforobject() next round */
int rmst=80;    /*  random monster creation counter     */
int userid;     /* the players login user id number */
char nowelcome=0,nomove=0; /* if (nomove) then don't count next iteration as a
                              move */
static char viewflag=0;    /* if viewflag then we have done a 99 stay here 
                              and don't showcell in the main loop */
char restorflag=0;         /* 1 means restore has been done    */
char prompt_mode = 0;         /* 1 if prompting for actions */

#ifndef MSDOS
# ifndef VMS
int ospeed = 0;
char PC = 0;
# endif VMS
#endif MSDOS

#ifdef MSDOS

static char cmdhelp[] = "\
Cmd line format: larn [-slicnhp] [-o<optsifle>] [-##] [++]\n\
  -s   show the scoreboard\n\
  -l   show the logfile (wizard id only)\n\
  -i   show scoreboard with inventories of dead characters\n\
  -c   create new scoreboard (wizard id only)\n\
  -n   suppress welcome message on starting game\n\
  -##  specify level of difficulty (example: -5)\n\
  -h   print this help text\n\
  -p   prompt for actions on objects\n\
  ++   restore game from checkpoint file\n\
  -o<optsfile>   specify larnopts filename to be used instead of \"larn.opt\"\n\
";

# else

static char cmdhelp[] = "\
Cmd line format: larn [-slicnhp] [-o<optsifle>] [-##] [++]\n\
  -s   show the scoreboard\n\
  -l   show the logfile (wizard id only)\n\
  -i   show scoreboard with inventories of dead characters\n\
  -c   create new scoreboard (wizard id only)\n\
  -n   suppress welcome message on starting game\n\
  -##  specify level of difficulty (example: -5)\n\
  -h   print this help text\n\
  -p   prompt for actions on objects\n\
  ++   restore game from checkpoint file\n\
  -o<optsfile>   specify .larnopts filename to be used instead of \"~/.larnopts\"\n\
";

# endif

#ifdef DGK_MSDOS
int save_mode = 0;      /* 1 if doing a save game */
jmp_buf save_jbuf;      /* To recover from disk full errors */
#endif

#ifdef VT100
static char *termtypes[] = { "vt100", "vt101", "vt102", "vt103", "vt125",
    "vt131", "vt140", "vt180", "vt220", "vt240", "vt241", "vt320", "vt340",
    "vt341"  };
#endif
/*
    ************
    MAIN PROGRAM
    ************
*/
main(argc,argv)
    int argc;
    char **argv;
    {
    register int i,j;
    int hard = -1;
    char *ptr=0,*ttype;
#ifndef MSDOS
    struct passwd *pwe,*getpwuid();
#endif

/*
 *  first task is to identify the player
 */
#ifndef VT100
    init_term();    /* setup the terminal (find out what type) for termcap */
#endif
#ifdef MSDOS
    ptr = "PLAYER";
#else
#ifdef VMS
    ptr = getenv("USER");
#else
    if (((ptr = getlogin()) == 0) || (*ptr==0)) /* try to get login name */
      if (pwe=getpwuid(getuid())) /* can we get it from /etc/passwd? */
        ptr = pwe->pw_name;
      else
      if ((ptr = getenv("USER")) == 0)
        if ((ptr = getenv("LOGNAME")) == 0)
          {
          noone: write(2, "Can't find your logname.  Who Are You?\n",39);
                 exit();
          }
    if (ptr==0) goto noone;
    if (strlen(ptr)==0) goto noone;
#endif
#endif

/*
 *  second task is to prepare the pathnames the player will need
 */
    strcpy(loginname,ptr); /* save loginname of the user for logging purposes */
    strcpy(logname,ptr);    /* this will be overwritten with the players name */

/* Set up the input and output buffers.
 */
    lpbuf    = (char *)malloc((5* BUFBIG)>>2);  /* output buffer */
    inbuffer = (char *)malloc((5*MAXIBUF)>>2);  /* output buffer */
    if ((lpbuf==0) || (inbuffer==0)) died(-285); /* malloc() failure */

# ifdef DGK_MSDOS
    /* LARNHOME now comes from the options file, so it must be read in
     * before constructing the other file names.  Unfortunately we have
     * to look for the -o option now.
     */
    strcpy(optsfile, LARNOPTS);
    for (i = 1; i < argc; i++)
        if (strncmp(argv[i], "-o", 2) == 0) {
            argv[i][0] = 0;         /* remove this argv */
            if (argv[i][2] != '\0')
                strncpy(optsfile, &argv[i][2], PATHLEN);
            else {
                strncpy(optsfile, argv[i + 1], PATHLEN);
                argv[i + 1][0] = 0; /* and this argv */
            }
            optsfile[PATHLEN - 1] = 0;
            break;
        }
    readopts();
    append_slash(larndir);

    /* Savefile and swapfile can be given explicitly as options
     */
    if (!savefilename[0]) {
        strcpy(savefilename, larndir);
        strcat(savefilename, SAVEFILE);
    }
    if (!swapfile[0]) {
        strcpy(swapfile, larndir);
        strcat(swapfile, SWAPFILE);
    }
    strcpy(scorefile, larndir);
    strcpy(logfile, larndir);
    strcpy(helpfile, larndir);
    strcpy(larnlevels, larndir);
    strcpy(fortfile, larndir);
    strcpy(playerids, larndir);
    strcpy(ckpfile, larndir);

# else /* DGK_MSDOS */

    if ((ptr = getenv("HOME")) == 0) ptr = ".";
#ifdef SAVEINHOME
    /* save file name in home directory */
# ifdef VMS
    sprintf(savefilename, "%s%s",ptr, SAVEFILE);
# else
    sprintf(savefilename, "%s/%s",ptr, SAVEFILE);
# endif VMS
#else
    strcat(savefilename,logname);   /* prepare savefile name */
    strcat(savefilename,".sav");    /* prepare savefile name */
#endif
#ifdef VMS
    sprintf(optsfile, "%s%s",ptr, LARNOPTS);   /* the options filename */
#else
    sprintf(optsfile, "%s/%s",ptr, LARNOPTS);   /* the options filename */
#endif VMS

# endif /* DGK_MSDOS */

    strcat(scorefile, SCORENAME);   /* the larn scoreboard filename */
    strcat(logfile, LOGFNAME);      /* larn activity logging filename */
    strcat(helpfile, HELPNAME);     /* the larn on-line help file */
    strcat(larnlevels, LEVELSNAME); /* the pre-made cave level data file */
    strcat(fortfile, FORTSNAME);    /* the fortune data file name */
    strcat(playerids, PLAYERIDS);   /* the playerid data file name */
    strcat(ckpfile, CKPFILE);

# ifdef TIMECHECK
    strcat(holifile, HOLIFILE);     /* the holiday data file name */
# endif

#ifdef VT100
/*
 *  check terminal type to avoid users who have not vt100 type terminals
 */
    ttype = getenv("TERM");
    for (j=1, i=0; i<sizeof(termtypes)/sizeof(char *); i++)
        if (strcmp(ttype,termtypes[i]) == 0) { j=0;  break; }
    if (j)
        {
        lprcat("Sorry, Larn needs a VT100 family terminal for all it's features.\n"); lflush();
        exit();
        }
#endif

/*
 *  now make scoreboard if it is not there (don't clear) 
 */
    if (access(scorefile,0) == -1) /* not there */
        makeboard();

/*
 *  now process the command line arguments 
 */
    for (i=1; i<argc; i++)
        {
        if (argv[i][0] == '-')
          switch(argv[i][1])
            {
	    case 's':          /* show scoreboard   */
		showscores();
#ifdef VMS
		exit();
#else
		exit(0);
#endif

            case 'l': /* show log file     */
			diedlog();
#ifdef VMS
		exit();
#else
		exit(0);
#endif

	    case 'i': showallscores();
	    /* show all scoreboard */
#ifdef VMS
		exit();
#else
		exit(0);
#endif

            case 'c':        /* anyone with password can create scoreboard */
                      lprcat("Preparing to initialize the scoreboard.\n");
                      if (getpassword() != 0)  /*make new scoreboard*/
                            {
                            makeboard(); lprc('\n'); showscores();
                            }
#ifdef VMS
		exit();
#else
		exit(0);
#endif

            case 'n':   /* no welcome msg   */ nowelcome=1; argv[i][0]=0; break;

            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': /* for hardness */
                hard = atoi(&argv[i][1]);
                break;

            case 'h':   /* print out command line arguments */
			write(1,cmdhelp,sizeof(cmdhelp));
#ifdef VMS
		exit();
#else
		exit(0);
#endif

            case 'o':   /* specify a .larnopts filename */
                        strncpy(optsfile,argv[i]+2,127);  break;

            case 'p':   /* set 'prompt_mode' flag */
                prompt_mode = 1 ;
                break ;

	    default:    printf("Unknown option <%s>\n",argv[i]);
#ifdef VMS
		exit();
#else
		exit(0);
#endif
            };

        if (strcmp(argv[i], "++") == 0)
            restorflag = 1;
    }

#ifndef DGK_MSDOS
    readopts();     /* read the options file if there is one */
#endif

#ifdef TIMECHECK
/*
 *  this section of code checks to see if larn is allowed during working hours
 */
    if (dayplay==0) /* check for not-during-daytime-hours */
      if (playable())
        {
        write(2,"Sorry, Larn can not be played during working hours.\n",52);
        exit();
        }
#endif TIMECHECK

#ifdef UIDSCORE
    userid = geteuid(); /* obtain the user's effective id number */
#else UIDSCORE
    userid = getplid(logname);  /* obtain the players id number */
#endif UIDSCORE
#ifdef VMS
    wisid = userid;
#endif
    if (userid < 0) { write(2,"Can't obtain playerid\n",22);
#ifdef VMS
		exit();
#else
		exit(0);
#endif
    }

#ifdef HIDEBYLINK
/*
 *  this section of code causes the program to look like something else to ps
 */
    if (strcmp(psname,argv[0])) /* if a different process name only */
        {
        if ((i=access(psname,1)) < 0)
            {       /* link not there */
            if (link(argv[0],psname)>=0)
                {
                argv[0] = psname;   execv(psname,argv);
                }
            }
        else
            unlink(psname);
        }

    for (i=1; i<argc; i++)
        {
        szero(argv[i]); /* zero the argument to avoid ps snooping */
        }
#endif HIDEBYLINK

/*
 *  He really wants to play, so malloc the memory for the dungeon.
 */
# ifdef MSDOS
    allocate_memory();
# else
    cell = (struct cel *)malloc(sizeof(struct cel)*(MAXLEVEL+MAXVLEVEL)*MAXX*MAXY);
    if (cell == 0) died(-285);  /* malloc failure */
# endif
    lcreat((char*)0);   newgame();      /*  set the initial clock  */

    if (restorflag == 1) {          /* restore checkpoint file */
        clear();
        hitflag = 1;
        restoregame(ckpfile);
    } else if (access(savefilename,0)==0)   /* restore game if need to */
        {
        clear();    restorflag = 1;
        hitflag=1;  restoregame(savefilename);  /* restore last game    */
        }
    sigsetup();     /* trap all needed signals  */
    setupvt100();   /*  setup the terminal special mode             */
    sethard(hard);  /* set up the desired difficulty                */
    if (c[HP]==0)   /* create new game */
        {
        makeplayer();   /*  make the character that will play           */
        newcavelevel(0);/*  make the dungeon                            */
        predostuff = 1; /* tell signals that we are in the welcome screen */
        if (nowelcome==0) welcome();     /* welcome the player to the game */
# ifdef DGK_MSDOS
        /* Display their mail if they've just won the previous game
         */
        checkmail();
# endif
        }

    lprc(T_INIT);   /* Reinit the screen because of welcome and check mail
             * having embedded escape sequences.*/
    drawscreen();   /*  show the initial dungeon                    */
    predostuff = 2; /* tell the trap functions that they must do a showplayer()
               from here on */
#if 0
    /* nice(1); /* games should be run niced */
#endif
    yrepcount = hit2flag = 0;
    while (1)
        {
        if (dropflag==0)
            /* see if there is an object here.

	       If in prompt mode, identify and prompt; else
	       identify, pickup if ( auto pickup and not move-no-pickup ),
	       never prompt.
	    */
	    if (prompt_mode)
		lookforobject( TRUE, FALSE, TRUE );
	    else
		lookforobject( TRUE, ( auto_pickup ), FALSE );
        else
            dropflag=0; /* don't show it just dropped an item */

	/* Move the monsters
	*/
	if (hitflag==0)
	    {
	    if (c[HASTEMONST])
		movemonst();
	    movemonst();
	    }

	/* show stuff around the player
	*/
	if (viewflag==0)
	    showcell(playerx,playery);
	else
	    viewflag=0;

	if (hit3flag) 
#ifdef OS2LARN
	    flushallkbd();
#else
	    flushall();
#endif
        hitflag=hit3flag=0; nomove=1;
        bot_linex();    /* update bottom line */
        while (nomove)
            {
            if (hit3flag) 
#ifdef OS2LARN
		flushallkbd();
#else
		flushall();
#endif
            nomove=0; parse();
            }   /*  get commands and make moves */
        regen();            /*  regenerate hp and spells            */
        if (c[TIMESTOP]==0)
            if (--rmst <= 0)
                { rmst = 120-(level<<2); fillmonst(makemonst(level)); }
        }
    }

/*
    showstr()

    show character's inventory
 */
showstr()
    {
    register int i,number;
    for (number=3, i=0; i<26; i++)
        if (iven[i]) number++;  /* count items in inventory */
    t_setup(number);    qshowstr();   t_endup(number);
    }

qshowstr()
    {
    register int i,j,k,sigsav;
    srcount=0;  sigsav=nosignal;  nosignal=1; /* don't allow ^c etc */
    if (c[GOLD]) { lprintf(".)   %d gold pieces",(long)c[GOLD]); srcount++; }
    for (k=25; k>=0; k--)   /* bug fix - dgk */
/*  for (k=26; k>=0; k--)            */
      if (iven[k])
        {  for (i=22; i<84; i++)
             for (j=0; j<=k; j++)  if (i==iven[j])  show3(j); k=0; }

    lprintf("\nElapsed time is %d.  You have %d mobuls left",(long)((gtime+99)/100+1),(long)((TIMELIMIT-gtime)/100));
    more();     nosignal=sigsav;
    }

/*
 *  subroutine to clear screen depending on # lines to display
 */
t_setup(count)
    register int count;
    {
    if (count<20)  /* how do we clear the screen? */
        {
        cl_up(79,count);  cursor(1,1);
        }
    else
        {
        resetscroll(); clear();
        }
    }

/*
 *  subroutine to restore normal display screen depending on t_setup()
 */
t_endup(count)
    register int count;
    {
    if (count<18)  /* how did we clear the screen? */
        draws(0,MAXX,0,(count>MAXY) ? MAXY : count);
    else
        {
        drawscreen(); setscroll();
        }
    }

/*
    function to show the things player is wearing only
 */
showwear()
    {
    register int i,j,sigsav,count;
    sigsav=nosignal;  nosignal=1; /* don't allow ^c etc */
    srcount=0;

     for (count=2,j=0; j<26; j++)   /* count number of items we will display */
       if (i=iven[j])
        switch(i)
            {
            case OLEATHER:  case OPLATE:    case OCHAIN:
            case ORING:     case OSTUDLEATHER:  case OSPLINT:
            case OPLATEARMOR:   case OSSPLATE:  case OSHIELD:
            count++;
            };

    t_setup(count);

    for (i=22; i<84; i++)
         for (j=0; j<26; j++)
           if (i==iven[j])
            switch(i)
                {
                case OLEATHER:  case OPLATE:    case OCHAIN:
                case ORING:     case OSTUDLEATHER:  case OSPLINT:
                case OPLATEARMOR:   case OSSPLATE:  case OSHIELD:
                show3(j);
                };
    more();     nosignal=sigsav;    t_endup(count);
    }

/*
    function to show the things player can wield only
 */
showwield()
    {
    register int i,j,sigsav,count;
    sigsav=nosignal;  nosignal=1; /* don't allow ^c etc */
    srcount=0;

     for (count=2,j=0; j<26; j++)  /* count how many items */
       if (i=iven[j])
        switch(i)
            {
            case ODIAMOND:  case ORUBY:  case OEMERALD:  case OSAPPHIRE:
            case OBOOK:     case OCHEST:  case OLARNEYE: case ONOTHEFT:
            case OSPIRITSCARAB:  case OCUBEofUNDEAD:
            case OPOTION:   case OSCROLL:  break;
            default:  count++;
            };

    t_setup(count);

    for (i=22; i<84; i++)
         for (j=0; j<26; j++)
           if (i==iven[j])
            switch(i)
                {
                case ODIAMOND:  case ORUBY:  case OEMERALD:  case OSAPPHIRE:
                case OBOOK:     case OCHEST:  case OLARNEYE: case ONOTHEFT:
                case OSPIRITSCARAB:  case OCUBEofUNDEAD:
                case OPOTION:   case OSCROLL:  break;
                default:  show3(j);
                };
    more();     nosignal=sigsav;    t_endup(count);
    }

/*
 *  function to show the things player can read only
 */
showread()
    {
    register int i,j,sigsav,count;
    sigsav=nosignal;  nosignal=1; /* don't allow ^c etc */
    srcount=0;

    for (count=2,j=0; j<26; j++)
        switch(iven[j])
            {
            case OBOOK: case OSCROLL:   count++;
            };
    t_setup(count);

    for (i=22; i<84; i++)
         for (j=0; j<26; j++)
           if (i==iven[j])
            switch(i)
                {
                case OBOOK: case OSCROLL:   show3(j);
                };
    more();     nosignal=sigsav;    t_endup(count);
    }

/*
 *  function to show the things player can eat only
 */
showeat()
    {
    register int i,j,sigsav,count;
    sigsav=nosignal;  nosignal=1; /* don't allow ^c etc */
    srcount=0;

    for (count=2,j=0; j<26; j++)
        switch(iven[j])
            {
            case OCOOKIE:   count++;
            };
    t_setup(count);

    for (i=22; i<84; i++)
         for (j=0; j<26; j++)
           if (i==iven[j])
            switch(i)
                {
                case OCOOKIE:   show3(j);
                };
    more();     nosignal=sigsav;    t_endup(count);
    }

/*
    function to show the things player can quaff only
 */
showquaff()
    {
    register int i,j,sigsav,count;
    sigsav=nosignal;  nosignal=1; /* don't allow ^c etc */
    srcount=0;

    for (count=2,j=0; j<26; j++)
        switch(iven[j])
            {
            case OPOTION:   count++;
            };
    t_setup(count);

    for (i=22; i<84; i++)
         for (j=0; j<26; j++)
           if (i==iven[j])
            switch(i)
                {
                case OPOTION:   show3(j);
                };
    more();     nosignal=sigsav;        t_endup(count);
    }

show1(idx,str2)
    register int idx;
    register char *str2[];
    {
        lprc('\n');
        cltoeoln();
    if (str2==0)
        lprintf("%c)   %s",idx+'a',objectname[iven[idx]]);
    else if (*str2[ivenarg[idx]]==0)
        lprintf("%c)   %s",idx+'a',objectname[iven[idx]]);
    else
        lprintf("%c)   %s of%s",idx+'a',objectname[iven[idx]],str2[ivenarg[idx]]);
    }

show3(index)
    register int index;
    {
    switch(iven[index])
        {
        case OPOTION:   show1(index,potionname);  break;
        case OSCROLL:   show1(index,scrollname);  break;

        case OLARNEYE:      case OBOOK:         case OSPIRITSCARAB:
        case ODIAMOND:      case ORUBY:         case OCUBEofUNDEAD:
        case OEMERALD:      case OCHEST:        case OCOOKIE:
        case OSAPPHIRE:     case ONOTHEFT:      show1(index,(char **)0);  break;

        default:
        lprc('\n');
        cltoeoln();
        lprintf("%c)   %s",index+'a',objectname[iven[index]]);
        if (ivenarg[index]>0)
            lprintf(" + %d",(long)ivenarg[index]);
        else if (ivenarg[index]<0)
            lprintf(" %d",(long)ivenarg[index]);
        break;
        }
    if (c[WIELD]==index) lprcat(" (weapon in hand)");
    if ((c[WEAR]==index) || (c[SHIELD]==index))  lprcat(" (being worn)");
    if (++srcount>=22) { srcount=0; more(); clear(); }
    }

/*
    subroutine to randomly create monsters if needed
 */
randmonst()
    {
    if (c[TIMESTOP]) return;    /*  don't make monsters if time is stopped  */
    if (--rmst <= 0)
        {
        rmst = 120 - (level<<2);  fillmonst(makemonst(level));
        }
    }


/*
    parse()

    get and execute a command
 */
parse()
    {
    register int i,j,k,flag;

    while   (1)
        {
        k = yylex();
        switch(k)   /*  get the token from the input and switch on it   */
            {
            case 'h':   moveplayer(4);  return;     /*  west        */
            case 'H':   run(4);         return;     /*  west        */
            case 'l':   moveplayer(2);  return;     /*  east        */
            case 'L':   run(2);         return;     /*  east        */
            case 'j':   moveplayer(1);  return;     /*  south       */
            case 'J':   run(1);         return;     /*  south       */
            case 'k':   moveplayer(3);  return;     /*  north       */
            case 'K':   run(3);         return;     /*  north       */
            case 'u':   moveplayer(5);  return;     /*  northeast   */
            case 'U':   run(5);         return;     /*  northeast   */
            case 'y':   moveplayer(6);  return;     /*  northwest   */
            case 'Y':   run(6);         return;     /*  northwest   */
            case 'n':   moveplayer(7);  return;     /*  southeast   */
            case 'N':   run(7);         return;     /*  southeast   */
            case 'b':   moveplayer(8);  return;     /*  southwest   */
            case 'B':   run(8);         return;     /*  southwest   */

            case '.':                               /*  stay here       */
                if (yrepcount) viewflag=1;
                return; 

            case 'c':
                yrepcount=0;    
                cast();     
                return;     /*  cast a spell    */

            case 'd':
                yrepcount=0;
                if (c[TIMESTOP]==0)
                    dropobj();
                return; /*  to drop an object   */

            case 'e':
                yrepcount=0;
                if (c[TIMESTOP]==0) 
                    eatcookie(); 
                return; /*  to eat a fortune cookie */

            case 'g':   
                yrepcount = 0 ;
                cursors();
                lprintf("\nThe stuff you are carrying presently weighs %d pounds",(long)packweight());
                break ;

            case 'i':
                yrepcount=0;    
                nomove=1;  
                showstr();  
                return;     /*  status      */

        case 'p':           /* pray at an altar */
        yrepcount = 0;
        if (!prompt_mode)
            pray_at_altar();
        else
            nomove = 1;
        return;

            case 'q':           /* quaff a potion */
                yrepcount=0;
                if (c[TIMESTOP]==0)
                    quaff();
                return; 

            case 'r':
                yrepcount=0;
                if (c[BLINDCOUNT])
                    { 
                    cursors(); 
                    lprcat("\nYou can't read anything when you're blind!");
                    }
                else if (c[TIMESTOP]==0)
                    readscr();
                return;     /*  to read a scroll    */

            case 's':
                yrepcount = 0 ;
                if (!prompt_mode)
                    sit_on_throne();
        else
            nomove = 1;
                return ;

            case 't':                       /* Tidy up at fountain */
                yrepcount = 0 ;
                if (!prompt_mode)
                    wash_fountain() ;
        else
            nomove = 1;
                return ;

            case 'v':   
        yrepcount=0;    
        cursors();
                lprintf("\nCaverns of Larn, Version %d.%d.%d, Diff=%d",(long)VERSION,(long)SUBVERSION,(long)PATCHLEVEL,(long)c[HARDGAME]);
                if (wizard) lprcat(" Wizard"); nomove=1;
                if (cheat) lprcat(" Cheater");
                lprcat(copyright);
                return;

            case 'w':                       /*  wield a weapon */
                yrepcount=0;
                wield();
                return;

        case 'A':
        yrepcount = 0;
        if (!prompt_mode)
            desecrate_altar();
        else
            nomove = 1;
        return;

            case 'C':                       /* Close something */
                yrepcount = 0 ;
                if (!prompt_mode)
                    close_something();
        else
            nomove = 1;
                return;

            case 'D':                       /* Drink at fountain */
                yrepcount = 0 ;
                if (!prompt_mode)
                    drink_fountain() ;
        else
            nomove = 1;
                return ;

            case 'E':               /* Enter a building */
                yrepcount = 0 ;
                if (!prompt_mode)
                    enter() ;
        else
            nomove = 1;
                break ;

            case 'G':               /* Give the stairs a kick */
                yrepcount = 0 ;
                if (!prompt_mode)
                    kick_stairs();
        else
            nomove = 1;
                return ;

            case 'I':              /*  list spells and scrolls */
                yrepcount=0;
                seemagic(0);
                nomove=1;
                return;

            case 'O':               /* Open something */
                yrepcount = 0 ;
                if (!prompt_mode)
                    open_something();
        else
            nomove = 1;
                return;

            case 'P':   
        cursors();
                if (outstanding_taxes>0)
                    lprintf("\nYou presently owe %d gp in taxes.",(long)outstanding_taxes);
                else
                    lprcat("\nYou do not owe any taxes.");
                return;

            case 'Q':    /*  quit        */
        yrepcount=0;
        quit(); 
        nomove=1;   
        return;

            case 'R' :          /* remove gems from a throne */
                yrepcount = 0 ;
                if (!prompt_mode)
                    remove_gems( );
        else
            nomove = 1;
                return ;

# ifdef DGK_MSDOS
            case 'S':
                /* Set up error recovery
                 */
                if (setjmp(save_jbuf) != 0) {

                    /* can't use lwclose!
                     */
                    if (lfd > 2)
                        close(lfd);
                    lcreat(NULL);
                    setscroll();
                    cursors();
                    lprcat("\nSave failed !\n");
                    if (errno == ENOSPC)
                        lprcat("Disk is full !\n");
                    beep();
                    (void) unlink(savefilename);
                    save_mode = 0;
                    yrepcount = 0;
                    nomove = 1;
                    break;
                }

                /* And do the save.
                 */
                cursors();
                lprintf("\nSaving to `%s' . . . ", savefilename);
                lflush();
                save_mode = 1;
                savegame(savefilename);
                clear();
                lflush();
                wizard=1;
                died(-257); /* doesn't return */
                break;
# else
            case 'S':   clear();  lprcat("Saving . . ."); lflush();  
                        savegame(savefilename); wizard=1; died(-257);   /*  save the game - doesn't return  */
# endif DGK_MSDOS

            case 'T':   yrepcount=0;    cursors();  if (c[SHIELD] != -1) { c[SHIELD] = -1; lprcat("\nYour shield is off"); bottomline(); } else
                                        if (c[WEAR] != -1) { c[WEAR] = -1; lprcat("\nYour armor is off"); bottomline(); }
                        else lprcat("\nYou aren't wearing anything");
                        return;

            case 'W':
                yrepcount=0;
                wear();
                return; /*  wear armor  */

            case 'Z':   
                yrepcount=0;
                if (c[LEVEL]>9) 
                    { 
                    oteleport(1);
                    return; 
                    }
                cursors(); 
                lprcat("\nAs yet, you don't have enough experience to use teleportation");
                return; /*  teleport yourself   */

            case ' ':   yrepcount=0;    nomove=1;  return;

# ifdef DGK_MSDOS
            case 'V':
                yrepcount = 0;
                nomove = 1;
                cursors();
                lprcat("\nMSDOS version 12a by Don Kneller, Berkeley CA.\n");
                lprcat("Version 12.1 by Kevin Routley\n");
                return;

            case 'D'-64:
                yrepcount = 0;
                nomove = 1;
                levelinfo();
                return;
# endif

            case 'L'-64:  yrepcount=0;  drawscreen();  nomove=1; return;    /*  look        */

#if WIZID
#ifdef EXTRA
            case 'A'-64:    yrepcount=0;    nomove=1; if (wizard) { diag(); return; }  /*   create diagnostic file */
                        return;
#endif
#endif
            case '<':                       /* Go up stairs or vol shaft */
                yrepcount = 0 ;
                if (!prompt_mode)
                    up_stairs();
        else
            nomove = 1;
                return ;

            case '>':                       /* Go down stairs or vol shaft*/
                yrepcount = 0 ;
                if (!prompt_mode)
                    down_stairs();
        else
            nomove = 1;
                return ;

            case '?':                       /* give the help screen */
                yrepcount=0;    
                help(); 
                nomove=1;
                return; 

        case ',':                       /* pick up an item */
            yrepcount = 0 ;
            if (!prompt_mode)
            /* pickup, don't identify or prompt for action */
            lookforobject( FALSE, TRUE, FALSE );
        else
            nomove = 1;
        return;

            case ':':                       /* look at object */
                yrepcount = 0 ;
                if (!prompt_mode)
		    /* identify, don't pick up or prompt for action */
                    lookforobject( TRUE, FALSE, FALSE );
                nomove = 1;  /* assumes look takes no time */
                return;

	    case '@':		/* toggle auto-pickup */
		yrepcount = 0 ;
		nomove = 1;
		cursors();
		lprcat("\nAuto pickup: ");
		auto_pickup = !auto_pickup;
		if (auto_pickup)
		    lprcat("On.");
		else
		    lprcat("Off.");
		return;

	    case '^':                       /* identify traps */
                flag = yrepcount = 0;
                cursors();
                lprc('\n');
                for (j=playery-1; j<playery+2; j++)
                    {
                    if (j < 0) 
                        j=0;        
                    if (j >= MAXY)
                        break;
                    for (i=playerx-1; i<playerx+2; i++)
                        {
                        if (i < 0) 
                            i=0;
                        if (i >= MAXX) 
                            break;
                        switch(item[i][j])
                            {
                            case OTRAPDOOR:     case ODARTRAP:
                            case OTRAPARROW:    case OTELEPORTER:
                            case OPIT:
                                lprcat("\nIts ");
                                lprcat(objectname[item[i][j]]);  
                                flag++;
                            };
                        }
                    }
                if (flag==0) 
                    lprcat("\nNo traps are visible");
                return;

#if WIZID
            case '_':   /*  this is the fudge player password for wizard mode*/
                        yrepcount=0;    cursors(); nomove=1;
# ifndef MSDOS
                        if (userid!=wisid)
                            {
                            lprcat("Sorry, you are not empowered to be a wizard.\n");
                            scbr(); /* system("stty -echo cbreak"); */
                            lflush();  return;
                            }
# endif
                        if (getpassword()==0)
                            {
                            scbr(); /* system("stty -echo cbreak"); */ return;
                            }
                        wizard=1;  scbr(); /* system("stty -echo cbreak"); */
                        for (i=0; i<6; i++)  c[i]=70;  iven[0]=iven[1]=0;
                        take(OPROTRING,50);   take(OLANCE,25);  c[WIELD]=1;
                        c[LANCEDEATH]=1;   c[WEAR] = c[SHIELD] = -1;
                        raiseexperience(6000000L);  c[AWARENESS] += 25000;
                        {
                        register int i,j;
                        for (i=0; i<MAXY; i++)
                            for (j=0; j<MAXX; j++)  know[j][i]=KNOWALL;
                        for (i=0; i<SPNUM; i++) spelknow[i]=1;
                        for (i=0; i<MAXSCROLL; i++)  scrollname[i][0]=' ';
                        for (i=0; i<MAXPOTION; i++)  potionname[i][0]=' ';
                        }
                        for (i=0; i<MAXSCROLL; i++)
                          if (strlen(scrollname[i])>2) /* no null items */
                            { item[i][0]=OSCROLL; iarg[i][0]=i; }
                        for (i=MAXX-1; i>MAXX-1-MAXPOTION; i--)
                          if (strlen(potionname[i-MAXX+MAXPOTION])>2) /* no null items */
                            { item[i][0]=OPOTION; iarg[i][0]=i-MAXX+MAXPOTION; }
                        for (i=1; i<MAXY; i++)
                            { item[0][i]=i; iarg[0][i]=0; }
                        for (i=MAXY; i<MAXY+MAXX; i++)
                            { item[i-MAXY][MAXY-1]=i; iarg[i-MAXY][MAXY-1]=0; }
			for (i=MAXX+MAXY; i<MAXOBJECT; i++)
			    {
			    item[MAXX-1][i-MAXX-MAXY]=i;
			    iarg[MAXX-1][i-MAXX-MAXY]=0;
			    }
                        c[GOLD]+=250000;    drawscreen();   return;
#endif

            };
        }
    }

parse2()
    {
    if (c[HASTEMONST]) movemonst(); movemonst(); /* move the monsters       */
    randmonst();    regen();
    }

run(dir)
    int dir;
    {
    register int i;
    i=1; while (i)
        {
        i=moveplayer(dir);
        if (i>0) {  if (c[HASTEMONST]) movemonst();  movemonst(); randmonst(); regen(); }
        if (hitflag) i=0;
        if (i!=0)  showcell(playerx,playery);
        }
    }

/*
    function to wield a weapon
 */
wield()
    {
    register int i;
    while (1)
        {
	if ((i = whatitem("wield (- for nothing)"))=='\33')  return;
        if (i != '.')
            {
            if (i=='*') showwield();
	    else if ( i == '-' ) { c[WIELD] = -1 ; return; }
	    else if (iven[i-'a']==0) { ydhi(i); return; }
            else if (iven[i-'a']==OPOTION) { ycwi(i); return; }
            else if (iven[i-'a']==OSCROLL) { ycwi(i); return; }
            else  if ((c[SHIELD]!= -1) && (iven[i-'a']==O2SWORD)) { lprcat("\nBut one arm is busy with your shield!"); return; }
            else  { c[WIELD]=i-'a'; if (iven[i-'a'] == OLANCE) c[LANCEDEATH]=1; else c[LANCEDEATH]=0;  bottomline(); return; }
            }
        }
    }

/*
    common routine to say you don't have an item
 */
ydhi(x)
    int x;
    { cursors();  lprintf("\nYou don't have item %c!",x); }
ycwi(x)
    int x;
    { cursors();  lprintf("\nYou can't wield item %c!",x); }

/*
    function to wear armor
 */
wear()
    {
    register int i;
    while (1)
        {
        if ((i = whatitem("wear"))=='\33')  return;
	if (i != '.' && i != '-')
            {
            if (i=='*') showwear(); else
            switch(iven[i-'a'])
                {
                case 0:  ydhi(i); return;
                case OLEATHER:  case OCHAIN:  case OPLATE:  case OSTUDLEATHER:
                case ORING:     case OSPLINT:   case OPLATEARMOR:   case OSSPLATE:
                        if (c[WEAR] != -1) { lprcat("\nYou're already wearing some armor"); return; }
                            c[WEAR]=i-'a';  bottomline(); return;
                case OSHIELD:   if (c[SHIELD] != -1) { lprcat("\nYou are already wearing a shield"); return; }
                                if (iven[c[WIELD]]==O2SWORD) { lprcat("\nYour hands are busy with the two handed sword!"); return; }
                                c[SHIELD] = i-'a';  bottomline(); return;
                default:    lprcat("\nYou can't wear that!");
                };
            }
        }
    }

/*
    function to drop an object
 */
dropobj()
    {
    register int i;
    register char *p;
    long amt;
    p = &item[playerx][playery];
    while (1)
        {
        if ((i = whatitem("drop"))=='\33')  return;
	if (i=='*') showstr();
	else if ( i != '-' )
            {
            if (i=='.') /* drop some gold */
                {
                if (*p) { lprcat("\nThere's something here already!"); return; }
                lprcat("\n\n");
                cl_dn(1,23);
                lprcat("How much gold do you drop? ");
                if ((amt=readnum((long)c[GOLD])) == 0) return;
                if (amt>c[GOLD])
                    {
#ifdef VMS
            lprcat("You don't have that much!");
#else
            lprcat("\nYou don't have that much!");
#endif VMS
            return; }
                if (amt<=32767)
                    { *p=OGOLDPILE; i=amt; }
                else if (amt<=327670L)
                    { *p=ODGOLD; i=amt/10; amt = 10L*i; }
                else if (amt<=3276700L)
                    { *p=OMAXGOLD; i=amt/100; amt = 100L*i; }
                else if (amt<=32767000L)
                    { *p=OKGOLD; i=amt/1000; amt = 1000L*i; }
                else
                    { *p=OKGOLD; i=32767; amt = 32767000L; }
                c[GOLD] -= amt;
#ifdef VMS
                lprintf("You drop %d gold pieces",(long)amt);
#else
                lprintf("\nYou drop %d gold pieces",(long)amt);
#endif VMS
                iarg[playerx][playery]=i; bottomgold();
                know[playerx][playery]=0; dropflag=1;  return;
                }
            drop_object(i-'a');
            return;
            }
        }
    }

/*
    readscr(), eatcookie(), quaff() could all be common code
*/
/*
 *  readscr()       Subroutine to read a scroll one is carrying
 */
readscr()
    {
    register int i;
    char tempc;

    /* check for player standing on a book/scroll.  If he is, prompt for
       and let him read it.  If player ESCs from prompt, quit the Read
       command.
    */
    cursors();
    i = item[playerx][playery];
    if ((i == OSCROLL) ||
        (i == OBOOK))
        {
        lprintf("\nThere is %s", objectname[i]); 
        if (i==OSCROLL)
            if (scrollname[iarg[playerx][playery]][0] != 0 )
                lprintf(" of%s", scrollname[iarg[playerx][playery]] );
        lprcat(" here.  Read it?");
        if ((tempc = getyn()) == 'y')
            {
	    int temp = iarg[playerx][playery];
	/* destroy the scroll first, in case its a teleport scroll. but
	   also must update player's knowledge to prevent a blank spot.
	*/
        forget();
	know[playerx][playery] = KNOWALL ;
        if (i==OBOOK)
                readbook( temp );
        else
        read_scroll( temp );
            return;
            }
        else if (tempc != 'n' )
            return;
        }

    while (1)
        {
        if ((i = whatitem("read"))=='\33')  return;
	if (i != '.' && i != '-')
            {
            if (i=='*') showread(); else
                {
                if (iven[i-'a']==OSCROLL) { read_scroll(ivenarg[i-'a']); iven[i-'a']=0; return; }
                if (iven[i-'a']==OBOOK)   { readbook(ivenarg[i-'a']);  iven[i-'a']=0; return; }
                if (iven[i-'a']==0) { ydhi(i); return; }
                lprcat("\nThere's nothing on it to read");  return;
                }
            }
        }
    }

/*
 *  subroutine to eat a cookie one is carrying
 */
eatcookie()
{
register int i;
char *p;
while (1)
    {
    char tempc;

    /* check for player standing on a cookie.  If he is, prompt for 
       and let him eat it.  If player ESCs from prompt, quit the Eat
       command.
    */
    cursors();
    i = item[playerx][playery];
    if (i == OCOOKIE)
        {
        lprcat("\nThere is a fortune cookie here.  Eat it?");
        if ((tempc = getyn()) == 'y')
            {
/* THIS COULD BE COMMON CODE !!! */
            lprcat("\nThe cookie was delicious.");
        forget();
            if (!c[BLINDCOUNT])
                {
# ifdef MSDOS
                outfortune();
# else
                if (p=fortune(fortfile))
                    {
                    lprcat("  Inside you find a scrap of paper that says:\n");
                    lprcat(p);
                    }
# endif
        }
            return;
            }
        else if (tempc != 'n' )
            return;
        }

    if ((i = whatitem("eat"))=='\33')  return;
    if (i != '.' && i != '-' )
        if (i=='*') showeat(); else
            {
            if (iven[i-'a']==OCOOKIE)
                {
                lprcat("\nThe cookie was delicious.");
                iven[i-'a']=0;
                if (!c[BLINDCOUNT])
                    {
# ifdef MSDOS
                    outfortune();
# else
                    if (p=fortune(fortfile))
                        {
                        lprcat("  Inside you find a scrap of paper that says:\n");
                        lprcat(p);
                        }
# endif
                    }
                return;
                }
            if (iven[i-'a']==0) { ydhi(i); return; }
            lprcat("\nYou can't eat that!");  return;
            }
    }
}

/*
 *  subroutine to quaff a potion one is carrying
 */
quaff()
    {
    register int i;
    char tempc;

    /* check for player standing on a potion.  If he is, prompt for
       and let him quaff it.  If player ESCs from prompt, quit the Quaff
       command.
    */
    cursors();
    i = item[playerx][playery];
    if (i == OPOTION)
        {
        lprintf("\nThere is %s", objectname[i]); 
        if (potionname[iarg[playerx][playery]][0] != 0 )
            lprintf(" of%s", potionname[iarg[playerx][playery]] );
        lprcat(" here.  Quaff it?");
        if ((tempc = getyn()) == 'y')
            {
        quaffpotion( iarg[playerx][playery] );
        forget();
            return;
            }
        else if (tempc != 'n' )
            return;
        }

    while (1)
        {
        if ((i = whatitem("quaff"))=='\33')
            return;
	if (i != '.' && i != '-' )
            {
            if (i=='*')
                showquaff();
            else
                {
                if (iven[i-'a']==OPOTION)
                    {
                    quaffpotion(ivenarg[i-'a'], TRUE);
                    iven[i-'a']=0;
                    return;
                    }
                if (iven[i-'a']==0) 
                    { 
                    ydhi(i); 
                    return;
                    }
                lprcat("\nYou wouldn't want to quaff that, would you? ");
                return;
                }
            }
        }
    }

/*
    function to ask what player wants to do
 */
static whatitem(str)
    char *str;
    {
    int i;
    cursors();  lprintf("\nWhat do you want to %s [* for all] ? ",str);
    i=0; while (i>'z' || (i<'a' && i!='-' && i!='*' && i!='\33' && i!='.' && i!=' '))
	i=ttgetch();
    if (i=='\33' || i==' ')  { lprcat(" aborted"); i='\33';}
    return(i);
    }

/*
    subroutine to get a number from the player
    and allow * to mean return amt, else return the number entered
 */
unsigned long readnum(mx)
    long mx;
    {
    register int i;
    register unsigned long amt=0;

    sncbr();
    /* allow him to say * for all gold 
    */
    if ((i=ttgetch()) == '*')
        amt = mx;
    else
        /* read chars into buffer, deleting when requested */
    while (i != '\n')
            {
        if (i=='\033') { scbr(); lprcat(" aborted"); return(0); }
        if ((i <= '9') && (i >= '0') && (amt<999999999))
            amt = amt*10+i-'0';
        if ((i=='\010') || (i=='\177'))
            amt = (long)(amt / 10) ;
        i = ttgetch();
        }
    scbr();  
    return(amt);
    }

#ifdef HIDEBYLINK
/*
 *  routine to zero every byte in a string
 */
szero(str)
    register char *str;
    {
    while (*str)
        *str++ = 0;
    }
#endif HIDEBYLINK

#ifdef TIMECHECK
/*
 *  routine to check the time of day and return 1 if its during work hours
 *  checks the file ".holidays" for forms like "mmm dd comment..."
 */
int playable()
    {
    long g_time,time();
    int hour,day,year;
    char *date,*month,*p;

    time(&g_time);  /* get the time and date */
    date = ctime(&g_time); /* format: Fri Jul  4 00:27:56 EDT 1986 */
    year = atoi(date+20);
    hour = (date[11]-'0')*10 + date[12]-'0';
    day  = (date[8]!=' ') ? ((date[8]-'0')*10 + date[9]-'0') : (date[9]-'0');
    month = date+4;  date[7]=0; /* point to and NULL terminate month */

    if (((hour>=8 && hour<17)) /* 8AM - 5PM */
        && strncmp("Sat",date,3)!=0     /* not a Saturday */
        && strncmp("Sun",date,3)!=0)    /* not a Sunday */
            {
        /* now check for a .holidays datafile */
            lflush();
            if (lopen(holifile) >= 0)
                for ( ; ; )
                    {
                    if ((p=lgetw())==0) break;
                    if (strlen(p)<6) continue;
                    if ((strncmp(p,month,3)==0) && (day==atoi(p+4)) && (year==atoi(p+7)))
                        return(0); /* a holiday */
                    }
            lrclose();  lcreat((char*)0);
            return(1);
            }
    return(0);
    }
#endif TIMECHECK
