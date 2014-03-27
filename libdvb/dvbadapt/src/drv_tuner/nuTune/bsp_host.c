/**
* \file $Id: bsp_host.c,v 1.3 2009/07/07 14:17:54 justin Exp $
*
* \brief Host and OS dependent functions
*
*
*
* $(c) 2004-2009 Trident Microsystems, Inc. - All rights reserved.
*
* This software and related documentation (the 'Software') are intellectual
* property owned by Trident and are copyright of Trident, unless specifically
* noted otherwise.
*
* Any use of the Software is permitted only pursuant to the terms of the
* license agreement, if any, which accompanies, is included with or applicable
* to the Software ('License Agreement') or upon express written consent of
* Trident. Any copying, reproduction or redistribution of the Software in
* whole or in part by any means not in accordance with the License Agreement
* or as agreed in writing by Trident is expressly prohibited.
*
* THE SOFTWARE IS WARRANTED, IF AT ALL, ONLY ACCORDING TO THE TERMS OF THE
* LICENSE AGREEMENT. EXCEPT AS WARRANTED IN THE LICENSE AGREEMENT THE SOFTWARE
* IS DELIVERED 'AS IS' AND TRIDENT HEREBY DISCLAIMS ALL WARRANTIES AND
* CONDITIONS WITH REGARD TO THE SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
* AND CONDITIONS OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIT
* ENJOYMENT, TITLE AND NON-INFRINGEMENT OF ANY THIRD PARTY INTELLECTUAL
* PROPERTY OR OTHER RIGHTS WHICH MAY RESULT FROM THE USE OR THE INABILITY
* TO USE THE SOFTWARE.
*
* IN NO EVENT SHALL TRIDENT BE LIABLE FOR INDIRECT, INCIDENTAL, CONSEQUENTIAL,
* PUNITIVE, SPECIAL OR OTHER DAMAGES WHATSOEVER INCLUDING WITHOUT LIMITATION,
* DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS
* INFORMATION, AND THE LIKE, ARISING OUT OF OR RELATING TO THE USE OF OR THE
* INABILITY TO USE THE SOFTWARE, EVEN IF TRIDENT HAS BEEN ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGES, EXCEPT PERSONAL INJURY OR DEATH RESULTING FROM
* TRIDENT'S NEGLIGENCE.                                                        $
*
*/

/*------------------------------------------------------------------------------
INCLUDE FILES
------------------------------------------------------------------------------*/
#include <string.h>  /* for memcpy() */
#include <sys/time.h>
#include "osapi.h"



#if 0
/* Includes for the millisecond ticker */
#if defined(__CYGWIN__)
#include <sys/time.h>
#include <sys/types.h>
#elif defined(WINDOWS) || defined(_WIN32)
#include <windows.h>
#include <winsock.h>    /* For definition of "timeval" structure */
#include <sys/timeb.h>  /* For prototype of "_ftime()" */
#else
#error "Platform not supported by bsp_host."
#error "Re-implement DRXBSP_HST_Clock() or gettimeofday() routines."
*; /* Generate a fatal compiler error to make sure it stops here,
      this is necesarry because not all compilers stop after a #error. */
#endif

#endif


#include "./include/bsp_host.h"

/**
   @def THOUSAND
   @brief magic value to convert between us, ms and s
*/
#define THOUSAND (1000L)
/**
   @def MILLION
   @brief magic value to convert between us and s
*/
#define MILLION  (1000000L)

/*------------------------------------------------------------------------------
VARIABLES
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
FUNCTIONS
------------------------------------------------------------------------------*/

#if defined(WINDOWS) || defined(_WIN32)

/**
* \brief  Emulation of gettimeofday() function from gcc.
* \return int
* \retval 0: Success
* \retval -1: Failure
*
* The tzp structure may be ignored. For the tp structure the following is
* assumed:
* - tp->tv_sec field: seconds since midnight (00:00:00) 1 Jan 1970
* - tp->tv_usec field: fraction of a second in microseconds
* Accuracy of _ftime seems to be 15 millisec, while gcc/gettimeofday()
* shows a accuracy of 1 millisec.
*
*/
int gettimeofday(struct timeval *tp, void *tzp)
{
   struct _timeb windowsTime;

   if (tp == (struct timeval *) NULL)
   {
      return (-1);
   }

   _ftime(&windowsTime);

   tp->tv_sec  = (long) (windowsTime.time + windowsTime.timezone);
   tp->tv_usec = windowsTime.millitm * THOUSAND;

   return(0);
}

#endif /*  defined(WINDOWS) || defined(_WIN32) */

/*------------------------------------------------------------------------------
EXPORTED FUNCTIONS
------------------------------------------------------------------------------*/

/**
* \brief  Function to initialise the host bsp module.
* \return DRXStatus_t.
* \retval DRX_STS_OK:    Successful initialization.
* \retval DRX_STS_ERROR: Something went wrong.
*
*/
DRXStatus_t DRXBSP_HST_Init( void )
{
   return (DRX_STS_OK);
}

/*============================================================================*/

/**
* \brief  Function to terminate the host bsp module.
* \return DRXStatus_t.
* \retval DRX_STS_OK:    Successful termination.
* \retval DRX_STS_ERROR: Something went wrong.
*
*/
DRXStatus_t DRXBSP_HST_Term( void )
{
   return (DRX_STS_OK);
}

/*============================================================================*/

/**
* \brief  Copy n bytes.
* \param  to:   destination address
* \param  from: source address
* \param  n:    size in bytes
*
* Behavior should by like ansi memcpy() function from <string.h> .
*/
void* DRXBSP_HST_Memcpy( void *to, void *from, u32_t n)
{
   return ( memcpy( to, from, n ) );
}

/*============================================================================*/

/**
* \brief  Compare the first n bytes of byte arrays s1 and s2.
* \param  s1: pointer to first array of bytes
* \param  s2: pointer to second array of bytes
* \param  n:  number of bytes to compare
*
* Behavior should by like ansi memcmp() or strcmp() function from <string.h>.
*/
int DRXBSP_HST_Memcmp( void *s1, void *s2, u32_t n)
{
   return ( memcmp( s1, s2, n ) );
}

/*============================================================================*/

/**
* \brief  Function returning the value of a millisecond ticker.
* \return Current value of the millisecond ticker.
*
* For cygwin/gcc the type of the tv_sec & tv_usec is time_t.
* For windows this type is long.
* To let this function work both long or time_t must be double sizeof(u16_t).
* This is ok for cygwin/gcc and windows/MSVS C++ 6.0
*
*/
u32_t DRXBSP_HST_Clock( void )
{
#if 1
   static Bool_t firstTimeCalled=TRUE;
   static struct timeval tp_start;
   u32_t  ticker = 0UL;
   struct timeval tp_curr;

   if ( gettimeofday( &tp_curr, NULL) != 0 )
   {
      /* Error,
         no suitable error value available but to give some indication ... */
      return 0xFFFFFFFFL;
   }

   if (firstTimeCalled == TRUE)
   {
      tp_start=tp_curr;
      firstTimeCalled=FALSE;
   }

   /* The -1 and +MILLION are used to avoid a signed division */
   ticker  = ( tp_curr.tv_sec  - tp_start.tv_sec  -1L      ) * THOUSAND;
   ticker += ( tp_curr.tv_usec - tp_start.tv_usec +MILLION ) / THOUSAND;

#ifdef COMPILE_FOR_NONRT
#ifndef RT_CLOCK_DIVIDER
#define RT_CLOCK_DIVIDER (750UL)
#endif
   /* Beware: non-real-time ticker will wrap much sooner */
   ticker /= RT_CLOCK_DIVIDER;
#endif /* COMPILE_FOR_NONRT */

   return ( ticker ); /* millisec */

#endif
//return 0;
}

/*============================================================================*/

/**
* \brief  Function will wait at least n milliseconds before it returns.
* \param  n number of milliseconds to wait before return
* \retval DRX_STS_OK:     Successful sleep.
* \retval DRX_STS_ERROR:  Unsuccessful sleep.
*
* Wait at least n milliseconds before returning.
* This is a provison to allow the host to spend this waiting time on other
* tasks.
*
* NOTE:
* In case an operating system is used it can happen that the sleep function
* provided by the OS can return before n milliseconds have elapsed. This can be
* caused by, for instance, an OS signal. Such events must be handled in the
* implementation this BSP function.
*
*/
DRXStatus_t DRXBSP_HST_Sleep( u32_t n )
{

#if 0
   u32_t start   = 0UL;
   u32_t current = 0UL;
   u32_t delta   = 0UL;

   start = DRXBSP_HST_Clock();

   do{
      current = DRXBSP_HST_Clock();
      delta = current - start;
   } while( delta < n );
#endif
	OS_TaskDelay (n);

   return (DRX_STS_OK);
}

/* End of file */
