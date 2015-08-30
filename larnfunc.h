

/*
 *
 * macros
 *
 */

/* macro to create scroll #'s with probability of occurrence */
#define newscroll() (scprob[rund(81)])

/* macro to return a potion # created with probability of occurrence */
#define newpotion() (potprob[rund(41)])

/* macro to return the + points on created leather armor */
#define newleather() (nlpts[rund(c[HARDGAME]?13:15)])

/* macro to return the + points on chain armor */
#define newchain() (nch[rund(10)])

/* macro to return + points on plate armor */
#define newplate() (nplt[rund(c[HARDGAME]?4:12)])

/* macro to return + points on new daggers */
#define newdagger() (ndgg[rund(13)])

/* macro to return + points on new swords */
#define newsword() (nsw[rund(c[HARDGAME]?6:13)])

/* macro to destroy object at present location */
#define forget() (item[playerx][playery]=know[playerx][playery]=0)

/* macro to wipe out a monster at a location */
#define disappear(x,y) (mitem[x][y]=know[x][y]=0)

/* macro to turn on bold display for the terminal */
#define setbold() (*lpnt++ = ST_START)

/* macro to turn off bold display for the terminal */
#define resetbold() (*lpnt++ = ST_END)

/* macro to setup the scrolling region for the terminal */
#define setscroll() enable_scroll=1

/* macro to clear the scrolling region for the terminal */
#define resetscroll() enable_scroll=0

/* macro to clear the screen and home the cursor */
#define clear() (*lpnt++ =CLEAR, regen_bottom=TRUE)

/* macro to clear to end of line */
#define cltoeoln() (*lpnt++ = CL_LINE)

/* macro to seed the random number generator */
#define srand(x) (lrandx=x)

/* min/max */
#define min(x,y) (((x)>(y))?(y):(x))
#define max(x,y) (((x)>(y))?(x):(y))



/*
 *
 * function declarations
 *
 */
 


/*
 * action.c
 */
void		act_remove_gems(int);

void		act_sit_throne(int);

void		act_up_stairs(void);

void		act_down_stairs(void);

void		act_drink_fountain(void);

void		act_wash_fountain(void);

void		act_down_shaft(void);

void		act_up_shaft(void);

void		act_desecrate_altar(void);

void		act_donation_pray(void);

void		act_just_pray(void);

void		act_ignore_altar(void);

void		act_open_chest(int, int);

int		act_open_door(int, int);



/*
 * bill.c
 */
void		readmail(long);


 
/*
 * create.c
 */
void		makeplayer(void);

void		newcavelevel(int);

void		eat(int, int);

int		fillmonst(signed char);



/*
 * diag.c
 */
#ifdef EXTRA
int		diag(void);

void		diagdrawscreen(void);
#endif

int		savegame(char *);

void		restoregame(char *);



/*
 * display.c
 */
void		bottomline(void);

void		bottomhp(void);

void		bottomspell(void);

void		bottomdo(void);

void		bot_linex(void);

void		bottomgold(void);

void		draws(int, int, int, int);

void		drawscreen(void);

void		showcell(int, int);

void		show1cell(int, int);

void		showplayer(void);

int		moveplayer(int);

void		seemagic(int);




/*
 * fortune.c
 */
void		outfortune(void);



/*
 * global.c 
 */
void		raiselevel(void);

void		loselevel(void);

void		raiseexperience(long);

void		loseexperience(long);

void		losehp(int);

void		losemhp(int);

void		raisehp(int);

void		raisemhp(int);

void		raisemspells(int);

void		losemspells(int);

int		makemonst(int);

void		positionplayer(void);

void		recalc(void);

void		quit(void);

int		more(char);

void		enchantarmor(void);
 
void		enchweapon(void);

int		nearbymonst(void);

int		stealsomething(void);

int		emptyhanded(void);
 
void		creategem(void);
 
void		adjustcvalues(int, int);
 
int		getpassword(void);

int		getyn(void);

int		packweight(void);

int		rnd(int);

int		rund(int);



/*
 * help.c
 */
void		help(void);

void		welcome(void);

void		retcont(void);



/*
 * iventory.c
 */
void		init_inventory(void);

int		showstr(char);

int		showwear(void);

int		showwield(void);

int		showread(void);

int		showeat(void);

int		showquaff(void);

void		show1(int);

int		show3(int);

int		take(int, int);

int		drop_object(int);

int		pocketfull(void);
 

/*
 * io.c
 */
void		setupvt100(void);
void		clearvt100(void);

char		ttgetch(void);

void		scbr(void);
void		sncbr(void);

void		newgame(void);

void		lprintf(const char *, ...);

void		lprint(long);

void		lprc(char);

void		lwrite(char *, int);

long		lgetc(void);

long		larint(void);

void		lrfill(char *, int);

char *		lgetw(void);

char *		lgetl(void);

int		lcreat(char *);

int		lopen(char *);

int		lappend(char *);

void		lrclose(void);

void		lwclose(void);

void		lprcat(char *);

void		cursor(int, int);

void		cursors(void);

void		init_term(void);

void		cl_line(int, int);

void		cl_up(int, int);

void		cl_dn(int, int);

void		lstandout(char *);

void		set_score_output(void);

void		lflush(void);

void		lflushall(void);

char *		tmcapcnv(char *, char *);

void		enter_name(void);

void		select_sex(void);




/*
 * main.c
 */ 
void		parse2(void);

unsigned long	readnum(long);



/*
 * monster.c
 */
void		checkloss(int);

void		createitem(int, int);

void		createmonster(int);

void		dropgold(int);

int		hitm(int, int, int);

void		hitmonster(int, int);

void		hitplayer(int, int);

int		newobject(int, int *);

void		something(int);

int		vxy(int *, int *);



/*
 * moreobj.c
 */
void		oaltar(void);

void		othrone(int);

void		odeadthrone(void);

void		ochest(void);

void		ofountain(void);

void		fntchange(int);

void		drink_fountain(void);

void		wash_fountain(void);

void		enter(void);

void		remove_gems(void);

void		sit_on_throne(void);

void		up_stairs(void);

void		down_stairs(void);

void		open_something(void);

void		close_something(void);

void		desecrate_altar(void);

void		pray_at_altar(void);

void		specify_object(void);



/*
 * movem.c
 */
void		movemonst(void);



/*
 * object.c
 */
void		lookforobject(char, char, char);

void		oteleport(int);

void		quaffpotion(int, int);

void		adjtime(long);

void		read_scroll(int);
 
void		readbook(int); 
 
void		ohome(void);

void		iopts(void);

void		ignore(void);



/*
 * regen.c
 */
void		regen(void);



/*
 * savelev.c
 */
void		savelevel(void);

void		getlevel(void);



/*
 * scores.c
 */
int 		makeboard(void);

int		hashewon(void);

void		checkmail(void);

long		paytaxes(long);

void		showscores(void);

void		showallscores(void);

void		died(int);

void		diedlog(void);

int		getplid(char *);



/*
 * spells.c
 */
void		cast(void);

int		fullhit(int);

void		godirect(int, int, char *, int, char);

void		ifblind(int, int);

int		dirsub(int *, int *);

void		annihilate(void);



/*
 * spheres.c
 */
int		newsphere(int, int, int, int);

int		rmsphere(int, int);

void		movsphere(void);




/*
 * store.c
 */
void		dndstore(void);

void		oschool(void);

void		obank(void);
void		obank2(void);

void		ointerest(void);

void		otradepost(void);

void		olrs(void);



/*
 * sysdep.c
 */
void		nap(int);



/*
 * tgoto.c
 */
const char *	atgoto(const char *, int, int);



/*
 * tok.c
 */
int		yylex(void);

void		sethard(int);




