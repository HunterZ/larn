/* help.c */
#include "larncons.h"
#include "larndata.h"
#include "larnfunc.h"


static int	openhelp(void);



/*
 *  help function to display the help info
 *
 *  format of the .larn.help file
 *
 *  1st character of file:  # of pages of help available (ascii digit)
 *  page (23 lines) for the introductory message (not counted in above)
 *  pages of help text (23 lines per page)
 */

void help(void)
{
	int i, j;


    /* open the help file and get # pages 
    */
    if ((j=openhelp()) < 0)  
    return;

    /* skip over intro message 
    */
    for (i=0; i<23; i++) 
    lgetl();
    --j;

    for (; j > 0; j--)
    {
	clear();
    
	for (i=0; i<23; i++)
		lprcat(lgetl());    /* print out each line that we read in */

	if (j>1)
        {
        lprcat("    ---- Press ");  lstandout("return");
        lprcat(" to exit, ");       lstandout("space");
        lprcat(" for more help ---- ");
        i=0; while ((i!=' ') && (i!='\n') && (i!='\33')) i=ttgetch();
        if ((i=='\n') || (i=='\33'))
        {
        lrclose();  
        setscroll();  
        drawscreen();  
        return;
        }
        }
    }
    
    lrclose();  
    retcont();  
    drawscreen();
}



/*
 * function to display the welcome message and background
 */
void welcome(void)
{
	int i;
	/* intermediate translation buffer */
	char tmbuf[128];    
    
	/* open the help file */
	if (openhelp() < 0) {
		
		return;    
	}

	clear();

	for(i = 0; i < 23; i++) {

		/* intercept \33's */
		tmcapcnv(tmbuf,lgetl());
		lprcat(tmbuf);
	} 

	lrclose();
	
	/* press return to continue */
	retcont();  
}



/*
 * function to say press return to continue and reset scroll when done
 */
void retcont(void)
{
	
	cursor(1,24);
	lprcat("Press "); lstandout("return");
	
	lprcat(" to continue: ");
	
	while (ttgetch() != '\n');

	setscroll();
}



/*
 * routine to open the help file and return the first character - '0'
 */
static int openhelp(void)
{

	if (lopen(helpfile) < 0) {
		
		lprintf("Can't open help file \"%s\" ",helpfile);
		lflush();
		nap(4000);
		drawscreen();
		setscroll();
		
		return -1;
        }
	
	resetscroll();

	return (lgetc() - '0');
}

