 /* Larn is copyrighted 1986 by Noah Morgan. */


/*
 *
 * types
 *
 */

/*  this is the structure that holds the entire dungeon specifications  */
struct cel
    {
    short   hitp;   /*  monster's hit points    */
    signed char    mitem;  /*  the monster ID          */
    signed char    item;   /*  the object's ID         */
    short   iarg;   /*  the object's argument   */
    signed char    know;   /*  have we been here before*/
    };

/* this is the structure for maintaining & moving the spheres of annihilation */
struct sphere
    {
    struct sphere *p;   /* pointer to next structure */
    signed char x,y,lev;       /* location of the sphere */
    signed char dir;           /* direction sphere is going in */
    signed char lifetime;      /* duration of the sphere */
    };


/*  this is the structure definition of the monster data
*/
struct monst
{
    char    *name;
    signed char    level;
    short   armorclass;
    signed char    damage;
    signed char    attack;
    signed char    defense;
    signed char    genocided;
    signed char    intelligence; /* monsters intelligence -- used to choose movement */
    short   gold;
    short   hitpoints;
    unsigned long experience;
};


/*  this is the structure definition for the items in the dnd store */
struct _itm
{

	short		price;
	
	signed char	obj;
	signed char	arg;
	signed char	qty;

};
    

/*
 *
 * data declarations
 *
 */
extern signed char regen_bottom;
extern char floorc, wallc;
extern signed char VERSION, SUBVERSION;
extern signed char beenhere[],cheat;
extern signed char course[];
extern signed char item[MAXX][MAXY],iven[],know[MAXX][MAXY];

extern char aborted[];

extern char *classname[];

extern char lastmonst[];

extern char *lpnt, *lpbuf, *lpend, *inbuffer;

extern signed char level;
extern signed char mitem[MAXX][MAXY],monstlevel[];
extern signed char nch[],ndgg[],nlpts[],nomove;
extern signed char nplt[],nsw[];
extern signed char potprob[];



extern char monstnamelist[];




extern char *levelname[];

extern char objnamelist[];

extern char logname[];

extern char larnlevels[];
extern char diagfile[];
extern char fortfile[];
extern char helpfile[];
extern char logfile[];
extern char playerids[];




extern signed char predostuff,restorflag;
extern char savefilename[];
extern char scorefile[];
extern signed char scprob[];
extern signed char screen[MAXX][MAXY],sex;
extern signed char spelknow[];

extern char *spelmes[];
extern char *speldescript[];
extern char *spelcode[];



extern char *spelname[];


extern signed char splev[],stealth[MAXX][MAXY],wizard;
extern short diroffx[],diroffy[],hitflag,hit2flag,hit3flag,hitp[MAXX][MAXY];
extern short iarg[MAXX][MAXY],ivenarg[],lasthx,lasthy,lastnum,lastpx,lastpy;
extern short oldx,oldy,playerx,playery;
extern int enable_scroll,yrepcount,userid,wisid,lfd,fd;
extern long initialtime,outstanding_taxes,skill[],gtime,c[],cbak[];
extern unsigned long lrandx;
extern struct cel *cell;
extern struct sphere *spheres;


extern struct monst monster[];
	
extern struct _itm dnd_item[];



/*
 * config.c
 */
extern char *password;



/*
 * data.c
 */
extern signed char prayed;

extern char scrollname[MAXSCROLL+1][MAXSCROLLNAME];
extern char potionname[MAXPOTION+1][MAXPOTIONNAME];

extern char *objectname[];


extern signed char spelweird[MAXMONST+8][SPNUM];



/*
 * main.c
 */
extern int rmst;
extern int dropflag;
extern int save_mode;


/*
 * store.c
 */
extern int lasttime;


/*
 * tok.c
 */
extern signed char move_no_pickup;



