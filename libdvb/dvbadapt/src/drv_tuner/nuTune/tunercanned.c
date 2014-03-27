/**
* \file $Id: tunercanned.c,v 1.10 2010/03/16 15:51:55 carlo Exp $
*
* \brief DRXBSP tuner implementation for canned tuners .
*
*/

/*
* $(c) 2004-2010 Trident Microsystems, Inc. - All rights reserved.
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

#include "./include/tunercanned.h"

/*------------------------------------------------------------------------------
DEFINES
------------------------------------------------------------------------------*/
/* DEMOD_MAX_FREQ_OFFSET defines the margin on centerFrequency for deciding
   which TCU-entry to use; margin determined by frequency mismatch in
   transmitted or received signal that can be handled by demod
*/

#define DEMOD_MAX_FREQ_OFFSET 500 /* kHz */

/*------------------------------------------------------------------------------
STATIC VARIABLES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
GLOBAL VARIABLES
------------------------------------------------------------------------------*/
TUNERFunc_t TUNERCANNEDFunctions_g = {
   TUNER_CANNED_Open,
   TUNER_CANNED_Close,
   TUNER_CANNED_SetFrequency,
   TUNER_CANNED_GetFrequency,
   TUNER_CANNED_LockStatus,
   DRXBSP_TUNER_DefaultI2CWriteRead
};

/*------------------------------------------------------------------------------
STRUCTURES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
FUNCTIONS
------------------------------------------------------------------------------*/

/**
* \fn DRXStatus_t TUNER_CANNED_Open( pTUNERInstance_t tuner )
* \brief Open a tuner.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Opened tuner with success.
* \retval DRX_STS_ERROR Something went wrong.
*/
DRXStatus_t
TUNER_CANNED_Open( pTUNERInstance_t tuner )
{
   DRXStatus_t err = DRX_STS_OK;
   u8_t status;
   pTUNERCANNEDControlTable_t TCT;
   u16_t i, nTCT;

   pTUNERCANNEDData_t extAttr = (pTUNERCANNEDData_t) tuner->myExtAttr;

   nTCT = extAttr->controlTables;
   if ( ( nTCT == 0 ) || ( extAttr->controlTable == NULL ) )
   {
      /* no TCT present */
      return DRX_STS_ERROR;
   }

   for ( i=0; i < nTCT; i++ )
   {
      TCT = &(extAttr->controlTable[i]);

      if (  ( TCT->refFreqNumerator   == ( DRXFrequency_t )0 ) ||
            ( TCT->refFreqDenominator == 0 ) )
      {
         /* zero component in reference frequency, cannot do calculations */
         return DRX_STS_ERROR;
      }
      if ( TCT->ControlUnits == NULL )
      {
         /* no TCU present */
         return DRX_STS_ERROR;
      }
   }

   /* check presence of tuner by reading status */

   err = tuner->myFunct->i2cWriteReadFunc( tuner,
                                 (pI2CDeviceAddr_t)(NULL), 0, (pu8_t)(NULL),
                                 &(tuner->myI2CDevAddr), 1, &status );
   if ( err != DRX_STS_OK )
   {
      return( err );
   }

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \fn DRXStatus_t TUNER_CANNED_Close( pTUNERInstance_t tuner )
* \brief Close a tuner.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Closed tuner with success.
* \retval DRX_STS_ERROR Something went wrong.
*/
DRXStatus_t
TUNER_CANNED_Close( pTUNERInstance_t tuner )
{
   return( DRX_STS_OK );
}

/*============================================================================*/

/**
* \fn DRXStatus_t TUNER_CANNED_SetFrequency( pTUNERInstance_t tuner,
                                             TUNERMode_t mode,
                                             DRXFrequency_t centerFrequency )
* \brief Program tuner at given center frequency for given mode.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Programmed tuner successfully.
* \retval DRX_STS_ERROR Something went wrong.
*/
DRXStatus_t
TUNER_CANNED_SetFrequency( pTUNERInstance_t tuner,
                           TUNERMode_t      mode,
                           DRXFrequency_t   centerFrequency )
{
   u16_t i;
   u16_t telegramsSent = 0;
   pTUNERCANNEDData_t extAttr = (pTUNERCANNEDData_t) tuner->myExtAttr;

   if (  ( centerFrequency <
               ( tuner->myCommonAttr->minFreqRF - DEMOD_MAX_FREQ_OFFSET ) ) ||
         ( centerFrequency >
               ( tuner->myCommonAttr->maxFreqRF + DEMOD_MAX_FREQ_OFFSET ) ) )
   {
      /* frequency out of range for this tuner */
      return DRX_STS_ERROR;
   }

   /* Clear sub-mode flags */
   mode &= ~TUNER_MODE_SUBALL;

   /* Check if sub-mode flag needs to be added */
   if ( tuner->myCommonAttr->subModes > 1 )
   {
      /* More than one submode; verify validity of sub-mode */
      if (  ( tuner->myCommonAttr->subMode  >= TUNER_MODE_SUB_MAX ) ||
            ( tuner->myCommonAttr->subMode  >= tuner->myCommonAttr->subModes ) )
      {
         /* invalid submode */
         return DRX_STS_ERROR;
      }
      /* set sub-mode flag */
      mode |= ( TUNER_MODE_SUB0 << tuner->myCommonAttr->subMode );
   }

   /* Check all control tables for desired mode: multiple packets may be sent */
   for ( i=0; i < extAttr->controlTables ; i++ )
   {
      pTUNERCANNEDControlTable_t TCT = &extAttr->controlTable[i];

      if ( ( TCT->modes & mode ) == mode )
      {
         /* this controlTable supports -at least- the requested mode bits     */

         DRXFrequency_t oscillatorFrequency;
         u32_t divider;
         u16_t controlByteCnt;
         u8_t  tunerTelegram[MAX_NR_CONTROL_BYTES + 2];

         pTUNERCANNEDControlUnit_t TCU;
         DRXStatus_t err;

         oscillatorFrequency = centerFrequency + TCT->outputFreq;
         divider = ( oscillatorFrequency * TCT->refFreqDenominator +
                     ( TCT->refFreqNumerator >> 1 ) ) / TCT->refFreqNumerator;

         if ( divider >= (1<<15) )
         {
            /* divider out-of-range, only 15-bit value supported */
            return DRX_STS_ERROR;
         }

         tunerTelegram[0] = (u8_t)( ( divider >> 8 ) & 0x7F );
         tunerTelegram[1] = (u8_t)(   divider        & 0xFF );

         TCU = TCT->ControlUnits;

         while ( ( centerFrequency - DEMOD_MAX_FREQ_OFFSET ) > TCU->maxFreq )
         {
            TCU++;
         }

         for ( controlByteCnt = 0 ;
               controlByteCnt < extAttr->nrOfControlbytes ;
               controlByteCnt++ )
         {
            tunerTelegram[controlByteCnt + 2] = TCU->CB[controlByteCnt];
         }

         err = tuner->myFunct->i2cWriteReadFunc( tuner,
                  &tuner->myI2CDevAddr, extAttr->nrOfControlbytes + 2,
                  tunerTelegram, (pI2CDeviceAddr_t)(NULL), 0, (pu8_t)(NULL) );

         if ( err != DRX_STS_OK )
         {
            return( err );
         }

         telegramsSent++;

         /* preserve information for GetFrequency call */
         tuner->myCommonAttr->RFfrequency = -TCT->outputFreq +
         ( divider * TCT->refFreqNumerator + ( TCT->refFreqDenominator >> 1 ) )
                           / TCT->refFreqDenominator;

         tuner->myCommonAttr->IFfrequency = TCT->outputFreq;
      }
   }

   if ( telegramsSent == 0 ) {
      /* no match found in control tables for the mode requested */
      return DRX_STS_ERROR;
   }
   return( DRX_STS_OK );
}

/*============================================================================*/

/**
* \fn DRXStatus_t TUNER_CANNED_GetFrequency( pTUNERInstance_t tuner,
                                             pDRXFrequency_t  RFfrequency,
                                             pDRXFrequency_t  IFfrequency )
* \brief Get tuned center frequency.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Returned frequency successfully.
* \retval DRX_STS_ERROR Something went wrong.
*/
DRXStatus_t
TUNER_CANNED_GetFrequency( pTUNERInstance_t tuner,
                           TUNERMode_t      mode,
                           pDRXFrequency_t  RFfrequency,
                           pDRXFrequency_t  IFfrequency )
{
   u16_t i;
   pTUNERCANNEDData_t extAttr = (pTUNERCANNEDData_t) tuner->myExtAttr;

   if ( IFfrequency != NULL )
   {
      *IFfrequency = 0;

      if ( mode != 0 )
      {
      /* Clear sub-mode flags */
      mode &= ~TUNER_MODE_SUBALL;

      /* Check if sub-mode flag needs to be added */
      if ( tuner->myCommonAttr->subModes > 1 )
      {
         /* More than one submode; verify validity of sub-mode */
         if (  ( tuner->myCommonAttr->subMode  >= TUNER_MODE_SUB_MAX ) ||
                  ( tuner->myCommonAttr->subMode >= tuner->myCommonAttr->subModes )
               )
         {
            /* invalid submode */
            return DRX_STS_ERROR;
         }
         /* set sub-mode flag */
         mode |= ( TUNER_MODE_SUB0 << tuner->myCommonAttr->subMode );
      }

      /* Check all control tables for desired mode */
      for ( i=0; i < extAttr->controlTables ; i++ )
      {
         pTUNERCANNEDControlTable_t TCT = &extAttr->controlTable[i];

         if ( ( TCT->modes & mode ) == mode )
         {
            *IFfrequency = TCT->outputFreq;
         }
      }
   }

      if ( *IFfrequency == 0)
      {
         *IFfrequency = tuner->myCommonAttr->IFfrequency;
      }
   }

   if ( RFfrequency != NULL )
   {
      *RFfrequency = tuner->myCommonAttr->RFfrequency;
   }

   return( DRX_STS_OK );
}

/*============================================================================*/

/**
* \fn DRXStatus_t TUNER_CANNED_LockStatus( pTUNERInstance_t tuner,
      pTUNERLockStatus_t lockStat )
* \brief Get tuner locking status.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Acquired locking status successfully.
* \retval DRX_STS_ERROR Something went wrong.
*/
DRXStatus_t
TUNER_CANNED_LockStatus(   pTUNERInstance_t    tuner,
                           pTUNERLockStatus_t  lockStat )
{
   DRXStatus_t err;
   u8_t status;
   pTUNERCANNEDData_t extAttr = (pTUNERCANNEDData_t) tuner->myExtAttr;

   /* read tuner status */
   err = tuner->myFunct->i2cWriteReadFunc( tuner,
                              (pI2CDeviceAddr_t)(NULL), 0, (pu8_t)(NULL),
                              &tuner->myI2CDevAddr, 1, &status );
   if ( err != DRX_STS_OK )
   {
      return( err );
   }
   /* report lock */
   if ( lockStat != NULL )
   {
      *lockStat = TUNER_NOT_LOCKED;
      if ( ( status & extAttr->lockMask ) == extAttr->lockValue )
      {
         *lockStat = TUNER_LOCKED;
      }
   }
   return DRX_STS_OK;
}

/* End of file */
