

#if defined(WINDOWS)

/********************************************
 *                 WINDOWS                  *
 ********************************************/

#include <windows.h>


/*
 *  routine to take a nap for n milliseconds
 */
void nap(int x)
{

	/* eliminate chance for infinite loop */
	if (x <= 0) {

		return; 
	}
	
	lflush();
	
	Sleep(x);
}


#endif




#if defined(UNIX)

/********************************************
 *                 UNIX                     *
 ********************************************/

#include <unistd.h>


/*
 *  routine to take a nap for n milliseconds
 */
void nap(int x)
{

	/* eliminate chance for infinite loop */
	if (x <= 0) {

		return; 
	}
	
	lflush();
	
	usleep(x);
}


#endif

