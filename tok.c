/* tok.c */
/*
   yylex()
   flushall()
   sethard()
*/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <fcntl.h>


#include "larncons.h"
#include "larndata.h"
#include "larnfunc.h"




# define CHKPTINT   400

static char lastok=0;
int yrepcount=0;
signed char move_no_pickup = FALSE ;



/*
 * lexical analyzer for larn
 */
int yylex(void)
{
	char cc;
	char firsttime = TRUE;

    if (hit2flag)
        {
        hit2flag=0;
        yrepcount=0;
        return(' ');
        }
    if (yrepcount>0)
        {
        --yrepcount;
        return(lastok);
        }
    else
        yrepcount=0;
    if (yrepcount==0) 
        { 
        bottomdo(); 
        showplayer();               /* show where the player is */
        move_no_pickup = FALSE;     /* clear 'm' flag */
        }

    lflush();
    while (1)
        {
        c[BYTESIN]++;

        cc = ttgetch();

        /* get repeat count, showing to player
        */
        if ((cc <= '9') && (cc >= '0'))
            {
            yrepcount = yrepcount*10 + cc - '0';

            /* show count to player for feedback
            */
            if ( yrepcount >= 10 )
                {
                cursors();
                if (firsttime)
                    lprcat("\n");
                lprintf("count: %d", (long)yrepcount );
                firsttime=FALSE;
                lflush();  /* show count */
                }
            }
        else
            {
            /* check for multi-character commands and handle.
            */
            if ( cc == 'm')
                {
                move_no_pickup = TRUE ;
                cc = ttgetch();
                }
            if ( yrepcount > 0 )
                --yrepcount;
            return(lastok = cc);
            }
        }
}





/*
    function to set the desired hardness
    enter with hard= -1 for default hardness, else any desired hardness
 */
void sethard(int hard)
{
	int j, k;
	long i;
	struct monst *mp;

	j = c[HARDGAME];
	hashewon();

	/* don't set c[HARDGAME] if restoring game */
	if (restorflag == 0) {
	
		if (hard >= 0) {
			
			c[HARDGAME] = hard;
		}

        } else {

		/* set c[HARDGAME] to proper value if restoring game */
		c[HARDGAME] = j; 
	}

	k = c[HARDGAME];

	if (k == 0) {

		return;
	}
	
	for (j = 0; j <= MAXMONST + 8; j++) {

		mp = &monster[j];
		
		i = ((6 + k) * mp->hitpoints + 1) / 6;
		mp->hitpoints = (i < 0) ? 32767 : i;
		
		i = ((6 + k) * mp->damage + 1) / 5;
		mp->damage = (i > 127) ? 127 : i;
		
		i = (10 * mp->gold) / (10 + k);
		mp->gold = (i > 32767) ? 32767 : i;
		
		i = mp->armorclass - k;
		mp->armorclass = (i < -127) ? -127 : i;

		i = (7 * mp->experience) / (7 + k) + 1;
		mp->experience = (i <= 0) ? 1 : i;
	}
}

