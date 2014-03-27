/**
* \file $Id: tunercanned.h,v 1.7 2010/03/16 15:51:55 carlo Exp $
*
* \brief DRXBSP tuner implementation for canned tuners .
*
*/

/*
* $(c) 2005,2007-2010 Trident Microsystems, Inc. - All rights reserved.
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

#ifndef __TUNERCANNED_H__
#define __TUNERCANNED_H__
/*------------------------------------------------------------------------------
INCLUDES
------------------------------------------------------------------------------*/
#include "bsp_tuner.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
DEFINES
------------------------------------------------------------------------------*/
/* MAX_NR_CONTROLBYTES defines the default maximum number of controlbytes that
   can be sent using this module */
#ifndef MAX_NR_CONTROL_BYTES
#define MAX_NR_CONTROL_BYTES 4
#endif

/*------------------------------------------------------------------------------
TYPEDEFS
------------------------------------------------------------------------------*/

typedef struct {

   DRXFrequency_t maxFreq;    /* Maximum RF center frequency (kHz) for
                                 which these control bytes are still valid,
                                 set to ((u32_t)-1) for last entry in array   */

   u8_t CB[MAX_NR_CONTROL_BYTES]; /* Array of control bytes */

} TUNERCANNEDControlUnit_t, *pTUNERCANNEDControlUnit_t;


typedef struct {

   TUNERMode_t         modes;     /* modes supported by this control table    */

   char               *description;        /* description of this control mode,
                                             e.g.: bandswitch, auxiliary, ... */

   DRXFrequency_t      refFreqNumerator;   /* Ref. frequency, numerator [kHz] */
   u32_t               refFreqDenominator; /* Ref. frequency, denominator     */

   DRXFrequency_t      outputFreq;         /* IF output frequency [kHz]       */

   pTUNERCANNEDControlUnit_t ControlUnits;
      /* Pointer to an array of control units,
         one for each 'frequency band' that is defined in the tuner datasheet
      */

} TUNERCANNEDControlTable_t, *pTUNERCANNEDControlTable_t;


typedef struct {
   u8_t            lockMask;            /* Mask to apply to check tuner lock  */
   u8_t            lockValue;           /* Value that indicates tuner lock    */

   pTUNERCANNEDControlTable_t controlTable;
      /* Pointer to an array  control tables.
         Multiple control tables can be used to for different modes
         of the same tuner, e.g. bandswitch control, auxiliary control,
         analog modem, etc.
      */
   u16_t controlTables;      /* number of entries in the ControlTable array   */
   u16_t nrOfControlbytes;   /* number control bytes                          */

} TUNERCANNEDData_t, *pTUNERCANNEDData_t;

/*------------------------------------------------------------------------------
ENUM
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
STRUCTS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Exported FUNCTIONS
------------------------------------------------------------------------------*/

extern DRXStatus_t TUNER_CANNED_Open (          pTUNERInstance_t tuner );

extern DRXStatus_t TUNER_CANNED_Close(          pTUNERInstance_t tuner );

extern DRXStatus_t TUNER_CANNED_SetFrequency(   pTUNERInstance_t tuner,
                                                TUNERMode_t      mode,
                                                DRXFrequency_t   frequency );

extern DRXStatus_t TUNER_CANNED_GetFrequency(   pTUNERInstance_t tuner,
                                                TUNERMode_t      mode,
                                                pDRXFrequency_t  RFfrequency,
                                                pDRXFrequency_t  IFfrequency );

extern DRXStatus_t TUNER_CANNED_LockStatus(     pTUNERInstance_t   tuner,
                                                pTUNERLockStatus_t lockStat );

/*------------------------------------------------------------------------------
Exported GLOBAL VARIABLES
------------------------------------------------------------------------------*/
extern TUNERFunc_t TUNERCANNEDFunctions_g;

/*------------------------------------------------------------------------------
THE END
------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif   /* __TUNERCANNED_H__ */

/* End of file */
