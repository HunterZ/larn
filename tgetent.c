/************************************************************************
 *									*
 *			Copyright (c) 1982, Fred Fish			*
 *			    All Rights Reserved				*
 *									*
 *	This software and/or documentation is released for public	*
 *	distribution for personal, non-commercial use only.		*
 *	Limited rights to use, modify, and redistribute are hereby	*
 *	granted for non-commercial purposes, provided that all		*
 *	copyright notices remain intact and all changes are clearly	*
 *	documented.  The author makes no warranty of any kind with	*
 *	respect to this product and explicitly disclaims any implied	*
 *	warranties of merchantability or fitness for any particular	*
 *	purpose.							*
 *									*
 ************************************************************************
 */

/*
 *  LIBRARY FUNCTION
 *
 *	tgetent   load buffer with entry for specified terminal
 *
 *  KEY WORDS
 *
 *	termcap functions
 *	utility routines
 *
 *  SYNOPSIS
 *
 *	int tgetent(bp,name)
 *	char *bp;
 *	char *name;
 *
 *  DESCRIPTION
 *
 *	Extracts the entry for terminal <name> from the termcap file
 *	and places it in the character buffer <bp>.   It is currently
 *	assumed that bp is at least 1024 characters.  If the entry in
 *	the termcap file is larger than 1023 characters the excess
 *	characters will be discarded and appropriate status will
 *	be returned.
 *
 *	Also note that since bp is used by other termcap
 *	routines, the storage associated with the termcap entry
 *	cannot be freed until all termcap calls are completed.
 *
 *	Tgetent can be directed to look in a file other than
 *	the default (/etc/termcap) by defining an environment
 *	variable called TERMCAP to be the pathname of the desired
 *	termcap file.  This is useful for debugging new entries.
#ifndef VMS
 *	NOTE: the pathname MUST begin with a '/' character.
 *
 *	Also, if the string assigned to TERMCAP does not begin with
 *	a '/' and if the environment variable TERM matches <name> then
 *	the string assigned to TERMCAP is copied to buffer <bp> 
 *	instead of reading a termcap file.
#endif
 *	
 *	If the termcap entry contains a "tc" string then the termcap
 *	entry named in the string is appended to the buffer (minus the
 *	names).
 *
 *  RETURNS
 *
 *	-1  if the termcap file cannot be opened
 *	 0  if no entry in termcap file matches <name>
 *	 1  if extraction is successful with no errors
 *	 2  if extraction is successful but entry truncated
 *
 *  SEE ALSO
 *
 *	tgetnum   extract numeric type capability
 *	tgetflag  test boolean type capability
 *	tgetstr   get string value of capability
 *
 *  AUTHOR
 *
 *	Fred Fish
 *
 */

#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define BUFSIZE 1024			/* Assumed size of external buffer */

#define NO_FILE	 -1			/* Returned if can't open file */
#define NO_ENTRY  0			/* Returned if can't find entry */
#define SUCCESS   1			/* Returned if entry found ok */
#define TRUNCATED 2			/* Returned if entry found but trunc */

#ifdef DGK_MSDOS
FILE *fopenp();
#endif

#define DEFAULT_ROOT "termcap"		/* name without path component */
#ifdef VMS
#define DEFAULT_FILE "sys$library:termcap"
#else
#define DEFAULT_FILE "/etc/termcap"	/* default termcap filename */
#endif VMS

char *_tcpbuf;				/* Place to remember buffer pointer */

/*
 *  PSEUDO CODE
 *
 *	Begin tgetent
 *	    Erase any previous buffer contents.
 *	    Remember the buffer pointer.
 *	    If termcap file is not found then
 *		If buffer was filled anyway then
 *		    Return SUCCESS.
 *		Else
 *		    Return NO_FILE.
 *		End if
 *	    Else
 *		While records left to process
 *		    If this is entry is what we want then
 *			Close the termcap file.
 *			If entry was truncated then
 *			    Return TRUNCATED status
 *			Else
 *			    Return SUCCESS status.
 *			End if
 *		    End if
 *		End while
 *		Return NO_ENTRY status.
 *	    End if
 *	End tgetent
 *			
 */

int tgetent(bp,name)
char *bp;				/* Pointer to buffer (1024 char min) */
char *name;				/* Pointer to terminal entry to find */
{
    FILE *fp, *find_file();
    char *nbp, tc[80];

    *bp = (char)NULL;
    _tcpbuf = bp;
    if ((fp = find_file(bp)) == NULL) {
	if (*bp != NULL) {
	    return(SUCCESS);
	} else {
	    return(NO_FILE);
	}
    } else {
	while (fgetlr(bp,BUFSIZE,fp)) {
	    if (gotcha(bp,name)) {
		fclose(fp);
		nbp = &bp[strlen(bp)-1];
		if (*nbp != '\n') {
		    return(TRUNCATED);
		} else {
		    /* check for a recursive call (i.e. :tc=vt100:)
		     * added 18-dec-86 RDE (single recursion...)
		     */
		    char *area;
		    area = &tc[0];
		    if (tgetstr("tc", &area) == NULL)		
			return(SUCCESS);
		    else {
			fp = find_file(0);	/* know it works and is file */
			while (fgetlr(nbp, BUFSIZE-(nbp-bp), fp)) {
			    if (gotcha(nbp,tc)) {
				char *cp1, *cp2;	/* scrunch out names */
				fclose(fp);
				cp1 = nbp;
				while (*cp1++ != ':')	/* search for first  */
					;
				cp2 = nbp;
				while (*cp2++ = *cp1++) /* move the chars.   */
					;
				if (bp[strlen(bp)-1] != '\n') {
				    return(TRUNCATED);
				} else {
				    return(SUCCESS);
				}
			    }
			}
			return (NO_ENTRY);
		    }
		}
	    }
	}
	return(NO_ENTRY);
    }
}

/*
 *  INTERNAL FUNCTION
 *
 *	find_file    find the termcap file and open it if possible
 *
 *  KEY WORDS
 *
 *	internal functions
 *	find_file
 *
 *  SYNOPSIS
 *
 *	static FILE *find_file(bp)
 *	char *bp;
 *
 *  DESCRIPTION
 *
 *	Attempts to locate and open the termcap file.  Also handles
 *	using the environment TERMCAP string as the actual buffer
 *	(that's why bp has to be an input parameter).
 *
#ifdef VMS
 *	If TERMCAP is defined as a valid filespec then it will be
 *	opened.  If this fails then the default termcap file will
 *	be used.
#else
 *	If TERMCAP is defined an begins with a '/' character then
 *	it is taken to be the pathname of the termcap file and
 *	an attempt is made to open it.  If this fails then
 *	the default termcap file is used instead.
 *
 *	If TERMCAP is defined but does not begin with a '/' then
 *	it is assumed to be the actual buffer contents provided
 *	that <name> matches the environment variable TERM.
#endif
 *
 *  BUGS
 *
 *	There is currently no way to be sure which termcap
 *	file was opened since the default will always be
 *	tried.
 *
 */

/*
 *  PSEUDO CODE
 *
 *	Begin find_file
 *	    If there is a TERMCAP environment string then
 *		If the string is not null then
 *		    If the string is a pathname then
 *			If that file is opened successfully then
 *			    Return its pointer.
 *			End if
 *		    Else
 *			If there is a TERM environment string then
 *			    If TERM matches <name> then
 *				Copy TERMCAP string to buffer.
 *				Return NULL for no file.
 *			    End if
 *			End if
 *		    End if
 *		End if
 *	    End if
 *	    Open default termcap file and return results.
 *	End find_file
 *
 */

static FILE *find_file(bp)
char *bp;
{
    FILE *fp, *fopen();
    char *cp, *ncp, *getenv();

    if ((cp = getenv("TERMCAP")) != NULL) {
	if (*cp != NULL) {
#ifdef VMS
	    if ((fp = fopen(cp, "r")) != NULL)
		return(fp);
#else
	    if (*cp == '/' || *cp == '\\') {
		if ((fp = fopen(cp,"r")) != NULL) {
		    return(fp);
		}
	    } else {
		if ((ncp = getenv("TERM")) != NULL) {
		    if (strcmp(cp,ncp) == 0) {
			strcpy(bp,cp);
			return((FILE *)NULL);
		    }
		}
	    }
#endif
	}
    }
    /*
     * Try current directory, then /etc/termcap
     */
	if (fp = fopen(DEFAULT_ROOT, "r"))
		return fp;
	else 
#ifdef DGK_MSDOS
        if (fp = fopen(DEFAULT_FILE, "r") )
            return fp;
        else	/* try along the PATH */
            return( fopenp(DEFAULT_ROOT, "r", NULL));
#else
		return (fopen(DEFAULT_FILE, "r"));
#endif
}


/*
 *  INTERNAL FUNCTION
 *
 *	gotcha   test to see if entry is for specified terminal
 *
 *  SYNOPSIS
 *
 *	gotcha(bp,name)
 *	char *bp;
 *	char *name;
 *
 *  DESCRIPTION
 *
 *	Tests to see if the entry in buffer bp matches the terminal
 *	specified by name.  Returns TRUE if match is detected, FALSE
 *	otherwise.
 *
 */

/*
 *  PSEUDO CODE
 *
 *	Begin gotcha
 *	    If buffer character is comment character then
 *		Return FALSE since remainder is comment
 *	    Else
 *		Initialize name scan pointer.
 *		Compare name and buffer until end or mismatch.
 *		If valid terminators for both name and buffer strings
 *		    Return TRUE since a match was found.
 *		Else
 *		    Find next non-name character in buffer.
 *		    If not an alternate name separater character
 *			Return FALSE since no more names to check.
 *		    Else
 *			Test next name and return results.
 *		    End if
 *		End if
 *	    End if
 *	End gotcha
 *
 */

gotcha(bp,name)
char *bp;
char *name;
{
    char *np;
 
    if (*bp == '#') {
	return(FALSE);
    } else {
	np = name;
	while (*np == *bp && *np != NULL) {np++; bp++;}
	if (*np == NULL && (*bp == NULL || *bp == '|' || *bp == ':')) {
	    return(TRUE);
	} else {
	    while (*bp != NULL && *bp != ':' && *bp != '|') {bp++;}
	    if (*bp != '|') {
		return(FALSE);
	    } else {
		return(gotcha(++bp,name));
	    }
	}
    }
}

#ifdef MSDOS
/*
 * index(buffer, char)
 * Find character in buffer.  Return the pointer
 * to it. Shouldn't be necessary to write this
 * myself but VMS didn't.  Oh Well...
 * Used by lots of files in the termcap library.
 * Rich Ellison, 16-DEC-1986
 */
char
*index(buf, ch)
char	*buf;
char	ch;
{
	register int	c;
	while ((c= *buf++) != '\0')
		if (c == ch)
			return (buf-1);
	return (NULL);
}
#endif
