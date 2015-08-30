/* fortune.c */
# include <stdio.h>

# include <sys/types.h>
# include <sys/stat.h>
#
#include <fcntl.h>

#include "larncons.h"
#include "larndata.h"
#include "larnfunc.h"




static void		msdosfortune(void);



static int		fortsize = 0;




void outfortune(void)
{

	lprcat("\nThe cookie was delicious.");

	if (c[BLINDCOUNT]) {

		return;
	}

	msdosfortune();
}



/* 
 * Rumors has been entirely rewritten to be disk based.  This is marginally
 * slower, but requires no mallocked memory.  Notice this in only valid for
 * files smaller than 32K.
 */
static void msdosfortune(void)
{
	int fd, status, i;
	char buf[BUFSIZ], ch;
	
	/* We couldn't open fortunes */
	if (fortsize < 0) {
		return;
	}

	fd = open(fortfile, O_RDONLY | O_BINARY);

	if (fd < 0) {
		
		/* Don't try opening it again */
		fortsize = -1; 
		
		return;
	}
		
        if (fortsize == 0) {

		fortsize = (int)lseek(fd, 0L, 2);
	}
	
        if (lseek(fd, (long)rund(fortsize), 0) < 0) {

		return;
	}

	/* 
	 * Skip to next newline or EOF
	 */
	do {

		status = read(fd, &ch, 1);
		
	} while (status != EOF && ch != '\n');
	
	if (status == EOF) {

		/* back to the beginning */
		if (lseek(fd, 0L, 0) < 0) {
			return;
		}
	}

        /* 
	 * Read in the line.  Search for CR ('\r'), not NL
         */
	for (i = 0; i < BUFSIZ - 1; i++) {
		
		if (read(fd, &buf[i], 1) == EOF || buf[i] == '\r') {

			break;
		}
	}

        buf[i] = '\0';

        /* 
	 * And spit it out
         */
        lprcat("  Inside you find a scrap of paper that says:\n");
        lprcat(buf);

        close(fd);
}

