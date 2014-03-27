/**
* \file $Id: bsp_tuner.c,v 1.10 2010/03/16 15:51:53 carlo Exp $
*
* \brief DRXBSP tuner dependant functions implementation.
*
*/

/*
* $(c) 2004-2005,2008-2010 Trident Microsystems, Inc. - All rights reserved.
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

#include "./include/bsp_tuner.h"

/*------------------------------------------------------------------------------
DEFINES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
STATIC VARIABLES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
STRUCTURES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
FUNCTIONS
------------------------------------------------------------------------------*/

/**
* \fn DRXStatus_t DRXBSP_TUNER_Open( pTUNERInstance_t tuner )
*
* \brief Open a tuner.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Opened tuner with success.
* \retval DRX_STS_ERROR Something went wrong.
* \retval DRX_STS_INVALID_ARG Invalid tuner instance.
*/
DRXStatus_t
DRXBSP_TUNER_Open( pTUNERInstance_t tuner )
{
   DRXStatus_t err;

   /*
      do restricted sanity checks of the tuner instance data structures,
      so other functions can rely on the parameters in the structures
   */

   if (  ( tuner == NULL )             ||
         ( tuner->myCommonAttr == NULL ) ||
         ( tuner->myFunct == NULL ) )

   {
      /* incomplete or no tuner structure */
      return DRX_STS_INVALID_ARG;
   }

   if ( tuner->myCommonAttr->selfCheck == &(tuner->myCommonAttr->selfCheck) )
   {
      /* tuner is currently open, attempt closing it, ignore any error */
      DRXBSP_TUNER_Close( tuner );
   }

   err = (*(tuner->myFunct->openFunc))( tuner );

   if ( err == DRX_STS_OK )
   {
      tuner->myCommonAttr->programmed = FALSE;
      tuner->myCommonAttr->selfCheck  = &tuner->myCommonAttr->selfCheck;
   } else {
      tuner->myCommonAttr->selfCheck  = NULL;
   }

   return ( err );
}

/*============================================================================*/

/**
* \fn DRXStatus_t DRXBSP_TUNER_Close( pTUNERInstance_t tuner )
*
* \brief Close a tuner.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Closed tuner with success.
* \retval DRX_STS_ERROR Something went wrong. Tuner is closed anyhow.
* \retval DRX_STS_INVALID_ARG Invalid tuner instance.
*/
DRXStatus_t
DRXBSP_TUNER_Close( pTUNERInstance_t tuner )
{
   DRXStatus_t err = DRX_STS_OK;

   if (  ( tuner == NULL )             ||
         ( tuner->myCommonAttr == NULL ) ||
         ( tuner->myFunct == NULL ) )

   {
      /* incomplete or no tuner structure */
      return DRX_STS_INVALID_ARG;
   }

   if ( tuner->myCommonAttr->selfCheck == &(tuner->myCommonAttr->selfCheck) )
   {
      /* tuner was open, so close it */
      err = (*(tuner->myFunct->closeFunc))( tuner );
   }

   /* always mark as closed (even in case of error) */
   tuner->myCommonAttr->selfCheck = NULL;

   return( err );
}

/*============================================================================*/

/**
* \fn DRXStatus_t DRXBSP_TUNER_SetFrequency( pTUNERInstance_t tuner,
*                                            TUNERMode_t mode,
*                                            DRXFrequency_t centerFrequency )
*
* \brief Program tuner at given center frequency for given mode.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Programmed tuner successfully.
* \retval DRX_STS_ERROR Something went wrong.
* \retval DRX_STS_INVALID_ARG Invalid tuner instance.
*/
DRXStatus_t
DRXBSP_TUNER_SetFrequency( pTUNERInstance_t tuner,
                           TUNERMode_t      mode,
                           DRXFrequency_t   centerFrequency )
{
   DRXStatus_t err;

   if (  ( tuner == NULL )             ||
         ( tuner->myCommonAttr == NULL ) ||
         ( tuner->myFunct == NULL ) )

   {
      /* incomplete or no tuner structure */
      return DRX_STS_INVALID_ARG;
   }

   if ( tuner->myCommonAttr->selfCheck != &(tuner->myCommonAttr->selfCheck) )
   {
      /* tuner not opened */
      return DRX_STS_ERROR;
   }

   err = (*(tuner->myFunct->setFrequencyFunc))( tuner, mode, centerFrequency );

   if ( err == DRX_STS_OK )
   {
      tuner->myCommonAttr->programmed = TRUE;
   } else {
      tuner->myCommonAttr->programmed = FALSE;
   }

   return (err);
}

/*============================================================================*/

/**
* \fn DRXStatus_t DRXBSP_TUNER_GetFrequency( pTUNERInstance_t tuner,
*                                            pDRXFrequency_t  RFfrequency,
*                                            pDRXFrequency_t  IFfrequency )
*
* \brief Get tuned center frequency.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Returned frequency successfully.
* \retval DRX_STS_ERROR Something went wrong.
* \retval DRX_STS_INVALID_ARG Invalid tuner instance.
*/
DRXStatus_t
DRXBSP_TUNER_GetFrequency( pTUNERInstance_t tuner,
                           TUNERMode_t       mode,
                           pDRXFrequency_t  RFfrequency,
                           pDRXFrequency_t  IFfrequency )
{
   if (  ( tuner == NULL )             ||
         ( tuner->myCommonAttr == NULL ) ||
         ( tuner->myFunct == NULL ) )

   {
      /* incomplete or no tuner structure */
      return DRX_STS_INVALID_ARG;
   }

   return( (*(tuner->myFunct->getFrequencyFunc))
                  ( tuner, mode, RFfrequency, IFfrequency) );
}

/*============================================================================*/

/**
* \fn DRXStatus_t DRXBSP_TUNER_LockStatus( pTUNERInstance_t tuner,
*                                          pTUNERLockStatus_t lockStat )
*
* \brief Get tuner locking status.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Acquired locking status successfully.
* \retval DRX_STS_ERROR Something went wrong.
* \retval DRX_STS_INVALID_ARG Invalid tuner instance.
*/
DRXStatus_t
DRXBSP_TUNER_LockStatus(   pTUNERInstance_t    tuner,
                           pTUNERLockStatus_t  lockStat )
{
   if (  ( tuner == NULL )             ||
         ( tuner->myCommonAttr == NULL ) ||
         ( tuner->myFunct == NULL ) )

   {
      /* incomplete or no tuner structure */
      return DRX_STS_INVALID_ARG;
   }

   if ( tuner->myCommonAttr->selfCheck != &(tuner->myCommonAttr->selfCheck) )
   {
      /* tuner not opened */
      return DRX_STS_ERROR;
   }

   return ( (*(tuner->myFunct->lockStatusFunc))( tuner, lockStat ) );
}

/*============================================================================*/

/**
* \fn DRXStatus_t
*     DRXBSP_TUNER_DefaultI2CWriteRead( pTUNERInstance_t tuner,
*                                       pI2CDeviceAddr_t wDevAddr,
*                                       u16_t            wCount,
*                                       pu8_t            wData,
*                                       pI2CDeviceAddr_t rDevAddr,
*                                       u16_t            rCount,
*                                       pu8_t            rData);
*
* \brief Standard I2C read write function.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Acquired locking status successfully.
* \retval DRX_STS_ERROR Something went wrong.
* \retval DRX_STS_INVALID_ARG Invalid tuner instance.
*
* Default implementation of I2CWriteRead function for tuners, via bsp_i2c.
*
*/
DRXStatus_t
DRXBSP_TUNER_DefaultI2CWriteRead(   pTUNERInstance_t   tuner,
                                    pI2CDeviceAddr_t wDevAddr,
                                    u16_t            wCount,
                                    pu8_t            wData,
                                    pI2CDeviceAddr_t rDevAddr,
                                    u16_t            rCount,
                                    pu8_t            rData)
{
   /* Default to BSP_I2C routine */
   return ( DRXBSP_I2C_WriteRead(   wDevAddr, wCount, wData,
                                    rDevAddr, rCount, rData) );
}

/* End of file */
