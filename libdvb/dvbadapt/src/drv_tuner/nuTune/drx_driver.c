/**
* \file $Id: drx_driver.c,v 1.42 2010/04/07 03:04:21 lfeng Exp $
*
* \brief Generic DRX functionality, DRX driver core.
*
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
#include "./include/drx_driver.h"
#include "./include/bsp_host.h"

#define VERSION_FIXED 0
#if     VERSION_FIXED
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#else
#include "./include/drx_driver_version.h"
#endif

/*------------------------------------------------------------------------------
DEFINES
------------------------------------------------------------------------------*/

/*============================================================================*/
/*=== MICROCODE RELATED DEFINES ==============================================*/
/*============================================================================*/

/** \brief Magic word for checking correct Endianess of microcode data. */
#ifndef DRX_UCODE_MAGIC_WORD
#define DRX_UCODE_MAGIC_WORD         ((((u16_t)'H')<<8)+((u16_t)'L'))
#endif

/** \brief CRC flag in ucode header, flags field. */
#ifndef DRX_UCODE_CRC_FLAG
#define DRX_UCODE_CRC_FLAG           (0x0001)
#endif

/** \brief Compression flag in ucode header, flags field. */
#ifndef DRX_UCODE_COMPRESSION_FLAG
#define DRX_UCODE_COMPRESSION_FLAG   (0x0002)
#endif

/** \brief Maximum size of buffer used to verify the microcode.
      Must be an even number. */
#ifndef DRX_UCODE_MAX_BUF_SIZE
#define DRX_UCODE_MAX_BUF_SIZE       (DRXDAP_MAX_RCHUNKSIZE)
#endif
#if DRX_UCODE_MAX_BUF_SIZE & 1
#error DRX_UCODE_MAX_BUF_SIZE must be an even number
#endif

/*============================================================================*/
/*=== CHANNEL SCAN RELATED DEFINES ===========================================*/
/*============================================================================*/

/**
* \brief Maximum progress indication.
*
* Progress indication will run from 0 upto DRX_SCAN_MAX_PROGRESS during scan.
*
*/
#ifndef DRX_SCAN_MAX_PROGRESS
#define DRX_SCAN_MAX_PROGRESS 1000
#endif

/*============================================================================*/
/*=== MACROS =================================================================*/
/*============================================================================*/

#define DRX_ISPOWERDOWNMODE( mode ) (  ( mode == DRX_POWER_MODE_9  ) || \
                                       ( mode == DRX_POWER_MODE_10 ) || \
                                       ( mode == DRX_POWER_MODE_11 ) || \
                                       ( mode == DRX_POWER_MODE_12 ) || \
                                       ( mode == DRX_POWER_MODE_13 ) || \
                                       ( mode == DRX_POWER_MODE_14 ) || \
                                       ( mode == DRX_POWER_MODE_15 ) || \
                                       ( mode == DRX_POWER_MODE_16 ) || \
                                       ( mode == DRX_POWER_DOWN    ) )

/*------------------------------------------------------------------------------
GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
STRUCTURES
------------------------------------------------------------------------------*/
/** \brief  Structure of the microcode block headers */
typedef struct {
   u32_t addr;    /**< Destination address of the data in this block */
   u16_t size;    /**< Size of the block data following this header counted in
                        16 bits words */
   u16_t flags;   /**< Flags for this data block:
                        - bit[0]= CRC on/off
                        - bit[1]= compression on/off
                        - bit[15..2]=reserved */
   u16_t CRC;     /**< CRC value of the data block, only valid if CRC flag is
                        set. */
} DRXUCodeBlockHdr_t, *pDRXUCodeBlockHdr_t;

/*------------------------------------------------------------------------------
FUNCTIONS
------------------------------------------------------------------------------*/

/*============================================================================*/
/*============================================================================*/
/*== Channel Scan Functions ==================================================*/
/*============================================================================*/
/*============================================================================*/

#ifndef DRX_EXCLUDE_SCAN

/* Prototype of default scanning function */
static DRXStatus_t
ScanFunctionDefault( void                 *scanContext,
                     DRXScanCommand_t    scanCommand,
                     pDRXChannel_t       scanChannel,
                     pBool_t             getNextChannel  );

/**
* \brief Get pointer to scanning function.
* \param demod:    Pointer to demodulator instance.
* \return DRXScanFunc_t.
*/
static DRXScanFunc_t
GetScanFunction( pDRXDemodInstance_t demod )
{
   pDRXCommonAttr_t commonAttr = (pDRXCommonAttr_t)(NULL);
   DRXScanFunc_t    scanFunc   = (DRXScanFunc_t)(NULL);

   /* get scan function from common attributes */
   commonAttr  = (pDRXCommonAttr_t)demod -> myCommonAttr;
   scanFunc    = commonAttr -> scanFunction;

   if ( scanFunc != NULL )
   {
      /* return device-specific scan function if it's not NULL */
      return scanFunc;
   }
   /* otherwise return default scan function in core driver */
   return &ScanFunctionDefault;
}

/**
* \brief Get Context pointer.
* \param demod:    Pointer to demodulator instance.
* \param scanContext: Context Pointer.
* \return DRXScanFunc_t.
*/
void  *GetScanContext(  pDRXDemodInstance_t  demod,
                        void                 *scanContext)
{
   pDRXCommonAttr_t commonAttr = (pDRXCommonAttr_t)(NULL);

   /* get scan function from common attributes */
   commonAttr  = (pDRXCommonAttr_t) demod->myCommonAttr;
   scanContext = commonAttr->scanContext;

   if ( scanContext == NULL )
   {
      scanContext = (void *) demod;
   }

   return scanContext;
}

/**
* \brief Wait for lock while scanning.
* \param demod:    Pointer to demodulator instance.
* \param lockStat: Pointer to bool indicating if end result is lock or not.
* \return DRXStatus_t.
* \retval DRX_STS_OK:    Success
* \retval DRX_STS_ERROR: I2C failure or bsp function failure.
*
* Wait until timeout, desired lock or NEVER_LOCK.
* Assume:
* - lock function returns : at least DRX_NOT_LOCKED and a lock state
*   higher than DRX_NOT_LOCKED.
* - BSP has a clock function to retrieve a millisecond ticker value.
* - BSP has a sleep function to enable sleep of n millisecond.
*
* In case DRX_NEVER_LOCK is returned the poll-wait will be aborted.
*
*/
static DRXStatus_t
ScanWaitForLock(  pDRXDemodInstance_t demod,
                  pBool_t             isLocked )
{
   Bool_t           doneWaiting = FALSE;
   DRXLockStatus_t  lockState = DRX_NOT_LOCKED;
   DRXLockStatus_t  desiredLockState = DRX_NOT_LOCKED;
   u32_t            timeoutValue = 0;
   u32_t            startTimeLockStage = 0;
   u32_t            currentTime = 0;
   u32_t            timerValue = 0;

   *isLocked = FALSE;
   timeoutValue = (u32_t) demod->myCommonAttr->scanDemodLockTimeout;
   desiredLockState = demod->myCommonAttr->scanDesiredLock;
   startTimeLockStage=DRXBSP_HST_Clock();

   /* Start polling loop, checking for lock & timeout */
   while ( doneWaiting == FALSE ) 
{

      if ( DRX_Ctrl( demod, DRX_CTRL_LOCK_STATUS, &lockState ) != DRX_STS_OK )
      {
         return DRX_STS_ERROR;
      }
      currentTime=DRXBSP_HST_Clock();

      timerValue = currentTime - startTimeLockStage;
      if ( lockState >= desiredLockState )
      {
               *isLocked = TRUE;
               doneWaiting = TRUE;
      } /* if ( lockState >= desiredLockState ) .. */
      else if ( lockState == DRX_NEVER_LOCK )
      {
               doneWaiting = TRUE;
      } /* if ( lockState == DRX_NEVER_LOCK ) .. */
      else if ( timerValue > timeoutValue )
      {
         /* lockState==DRX_NOT_LOCKED  and timeout */
         doneWaiting = TRUE;
      }
      else
      {
         if ( DRXBSP_HST_Sleep( 10 ) != DRX_STS_OK )
         {
            return DRX_STS_ERROR;
         }
      }/* if ( timerValue > timeoutValue ) .. */

   } /* while */

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Determine next frequency to scan.
* \param demod: Pointer to demodulator instance.
* \param skip : Minimum frequency step to take.
* \return DRXStatus_t.
* \retval DRX_STS_OK:          Succes.
* \retval DRX_STS_INVALID_ARG: Invalid frequency plan.
*
* Helper function for CtrlScanNext() function.
* Compute next frequency & index in frequency plan.
* Check if scan is ready.
*
*/
static DRXStatus_t
ScanPrepareNextScan (   pDRXDemodInstance_t  demod,
                        DRXFrequency_t       skip )
{
   pDRXCommonAttr_t     commonAttr=(pDRXCommonAttr_t)(NULL);
   u16_t                tableIndex = 0;
   u16_t                frequencyPlanSize = 0;
   pDRXFrequencyPlan_t  frequencyPlan = (pDRXFrequencyPlan_t)(NULL);
   DRXFrequency_t       nextFrequency = 0;
   DRXFrequency_t       tunerMinFrequency = 0;
   DRXFrequency_t       tunerMaxFrequency = 0;

   commonAttr        = (pDRXCommonAttr_t)demod->myCommonAttr;
   tableIndex        = commonAttr->scanFreqPlanIndex;
   frequencyPlan     = commonAttr->scanParam->frequencyPlan;
   nextFrequency     = commonAttr->scanNextFrequency;
   tunerMinFrequency = commonAttr->tunerMinFreqRF;
   tunerMaxFrequency = commonAttr->tunerMaxFreqRF;

   do 
  {
      /* Search next frequency to scan */

      /* always take at least one step */
      (commonAttr->scanChannelsScanned) ++;
      nextFrequency += frequencyPlan[tableIndex].step;
      skip -= frequencyPlan[tableIndex].step;

      /* and then as many steps necessary to exceed 'skip'
         without exceeding end of the band */
      while (  ( skip > 0 ) &&
               ( nextFrequency <= frequencyPlan[tableIndex].last ) )
      {
         (commonAttr->scanChannelsScanned) ++;
         nextFrequency += frequencyPlan[tableIndex].step;
         skip -= frequencyPlan[tableIndex].step;
      }
      /* reset skip, in case we move to the next band later */
      skip = 0;

      if ( nextFrequency > frequencyPlan[tableIndex].last )
      {
         /* reached end of this band */
         tableIndex++;
         frequencyPlanSize = commonAttr->scanParam->frequencyPlanSize;
         if ( tableIndex >= frequencyPlanSize )
         {
            /* reached end of frequency plan */
            commonAttr->scanReady = TRUE;
         }
         else 
        {
            nextFrequency = frequencyPlan[tableIndex].first;
         }
      }
      if ( nextFrequency > (tunerMaxFrequency) )
      {
         /* reached end of tuner range */
         commonAttr->scanReady = TRUE;
      }
   } while( ( nextFrequency < tunerMinFrequency ) &&
            ( commonAttr->scanReady == FALSE ) );

   /* Store new values */
   commonAttr->scanFreqPlanIndex = tableIndex;
   commonAttr->scanNextFrequency = nextFrequency;

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Default DTV scanning function.
*
* \param demod:          Pointer to demodulator instance.
* \param scanCommand:    Scanning command: INIT, NEXT or STOP.
* \param scanChannel:    Channel to check: frequency and bandwidth, others AUTO
* \param getNextChannel: Return TRUE if next frequency is desired at next call
*
* \return DRXStatus_t.
* \retval DRX_STS_OK:      Channel found, DRX_CTRL_GET_CHANNEL can be used
*                             to retrieve channel parameters.
* \retval DRX_STS_BUSY:    Channel not found (yet).
* \retval DRX_STS_ERROR:   Something went wrong.
*
* scanChannel and getNextChannel will be NULL for INIT and STOP.
*/
static DRXStatus_t
ScanFunctionDefault (   void                 *scanContext,
                     DRXScanCommand_t    scanCommand,
                     pDRXChannel_t       scanChannel,
                     pBool_t             getNextChannel  )
{
   pDRXDemodInstance_t demod = NULL;
   DRXStatus_t status   = DRX_STS_ERROR;
   Bool_t      isLocked = FALSE;

   demod = (pDRXDemodInstance_t) scanContext;

   if ( scanCommand != DRX_SCAN_COMMAND_NEXT )
   {
      /* just return OK if not doing "scan next" */
      return DRX_STS_OK;
   }

   *getNextChannel = FALSE;

   status = DRX_Ctrl( demod, DRX_CTRL_SET_CHANNEL, scanChannel );
   if ( status != DRX_STS_OK )
   {
      return (status);
   }

   status = ScanWaitForLock( demod, &isLocked );
   if ( status != DRX_STS_OK )
   {
      return status;
   }

   /* done with this channel, move to next one */
   *getNextChannel = TRUE;

   if ( isLocked == FALSE )
   {
      /* no channel found */
      return DRX_STS_BUSY;
   }
   /* channel found */
   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Initialize for channel scan.
* \param demod:     Pointer to demodulator instance.
* \param scanParam: Pointer to scan parameters.
* \return DRXStatus_t.
* \retval DRX_STS_OK:          Initialized for scan.
* \retval DRX_STS_ERROR:       No overlap between frequency plan and tuner
*                              range.
* \retval DRX_STS_INVALID_ARG: Wrong parameters.
*
* This function should be called before starting a complete channel scan.
* It will prepare everything for a complete channel scan.
* After calling this function the DRX_CTRL_SCAN_NEXT control function can be
* used to perform the actual scanning. Scanning will start at the first
* center frequency of the frequency plan that is within the tuner range.
*
*/
static DRXStatus_t
CtrlScanInit(  pDRXDemodInstance_t   demod,
               pDRXScanParam_t       scanParam )
{
   DRXStatus_t status = DRX_STS_ERROR;
   pDRXCommonAttr_t commonAttr=(pDRXCommonAttr_t)(NULL);
   DRXFrequency_t maxTunerFreq = 0;
   DRXFrequency_t minTunerFreq = 0;
   u16_t nrChannelsInPlan = 0;
   u16_t i = 0;
   void              *scanContext      = NULL;

   commonAttr = (pDRXCommonAttr_t)demod -> myCommonAttr;
   commonAttr->scanActive = TRUE;

   /* invalidate a previous SCAN_INIT */
   commonAttr->scanParam = (pDRXScanParam_t)(NULL);
   commonAttr->scanNextFrequency = 0;

   /* Check parameters */
   if (  (  ( demod->myTuner == NULL )          &&
            ( scanParam->numTries !=1) )        ||

         ( scanParam == NULL) ||
         ( scanParam->numTries == 0) ||
         ( scanParam->frequencyPlan == NULL) ||
         ( scanParam->frequencyPlanSize == 0 )
      )
   {
      commonAttr->scanActive = FALSE;
      return DRX_STS_INVALID_ARG;
   }

   /* Check frequency plan contents */
   maxTunerFreq = commonAttr->tunerMaxFreqRF;
   minTunerFreq = commonAttr->tunerMinFreqRF;
   for( i = 0; i < (scanParam->frequencyPlanSize); i++ )
   {
      DRXFrequency_t width = 0;
      DRXFrequency_t step      = scanParam->frequencyPlan[i].step;
      DRXFrequency_t firstFreq = scanParam->frequencyPlan[i].first;
      DRXFrequency_t lastFreq  = scanParam->frequencyPlan[i].last;
      DRXFrequency_t minFreq = 0;
      DRXFrequency_t maxFreq = 0;

      if ( step <= 0 )
      {
         /* Step must be positive and non-zero */
         commonAttr->scanActive = FALSE;
         return DRX_STS_INVALID_ARG;
      }

      if ( firstFreq > lastFreq )
      {
         /* First center frequency is higher than last center frequency */
            commonAttr->scanActive = FALSE;
         return DRX_STS_INVALID_ARG;
      }

      width = lastFreq - firstFreq;

      if ( (width % step) != 0 )
      {
         /* Difference between last and first center frequency is not
         an integer number of steps */
            commonAttr->scanActive = FALSE;
         return DRX_STS_INVALID_ARG;
      }

      /* Check if frequency plan entry intersects with tuner range */
      if ( lastFreq >= minTunerFreq )
      {
         if ( firstFreq <= maxTunerFreq )
         {
            if (  firstFreq >= minTunerFreq )
            {
               minFreq = firstFreq;
            } 
            else 
            {
               DRXFrequency_t n=0;

               n=( minTunerFreq - firstFreq )/step;
               if ( (( minTunerFreq - firstFreq )%step) != 0 )
               {
                  n++;
               }
               minFreq = firstFreq + n*step;
            }

            if ( lastFreq <= maxTunerFreq )
            {
               maxFreq = lastFreq;
            }
            else 
            {
               DRXFrequency_t n=0;

               n=( lastFreq - maxTunerFreq )/step;
               if ( (( lastFreq - maxTunerFreq )%step) !=0 )
               {
                  n++;
               }
               maxFreq = lastFreq - n*step;
            }
         }
      }

      /* Keep track of total number of channels within tuner range
         in this frequency plan. */
      if ( (minFreq!=0) && (maxFreq!=0) )
      {
         nrChannelsInPlan += (u16_t)(( (maxFreq-minFreq) / step ) +1 );

         /* Determine first frequency (within tuner range) to scan */
         if ( commonAttr->scanNextFrequency == 0 )
         {
            commonAttr->scanNextFrequency = minFreq;
            commonAttr->scanFreqPlanIndex = i;
         }
      }

   }/* for ( ... ) */

   if ( nrChannelsInPlan == 0 )
   {
      /* Tuner range and frequency plan ranges do not overlap */
      commonAttr->scanActive = FALSE;
      return DRX_STS_ERROR;
   }

   /* Store parameters */
   commonAttr->scanReady = FALSE;
   commonAttr->scanMaxChannels = nrChannelsInPlan;
   commonAttr->scanChannelsScanned = 0;
   commonAttr->scanParam = scanParam; /* SCAN_NEXT is now allowed */

   scanContext = GetScanContext(demod, scanContext);

   status = (*(GetScanFunction( demod )))
            ( scanContext, DRX_SCAN_COMMAND_INIT, NULL, NULL );

   commonAttr->scanActive = FALSE;

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Stop scanning.
* \param demod:         Pointer to demodulator instance.
* \return DRXStatus_t.
* \retval DRX_STS_OK:          Scan stopped.
* \retval DRX_STS_ERROR:       Something went wrong.
* \retval DRX_STS_INVALID_ARG: Wrong parameters.
*/
static DRXStatus_t
CtrlScanStop( pDRXDemodInstance_t  demod  )
{
   DRXStatus_t status = DRX_STS_ERROR;
   pDRXCommonAttr_t commonAttr = (pDRXCommonAttr_t)(NULL);
   void              *scanContext   = NULL;

   commonAttr              = (pDRXCommonAttr_t)demod->myCommonAttr;
   commonAttr->scanActive = TRUE;

   if (  ( commonAttr->scanParam == NULL ) ||
         ( commonAttr->scanMaxChannels == 0 ))
   {
      /* Scan was not running, just return OK */
      commonAttr->scanActive = FALSE;
      return DRX_STS_OK;
   }

   /* Call default or device-specific scanning stop function */
   scanContext = GetScanContext(demod, scanContext);

   status = (*(GetScanFunction( demod )))
            ( scanContext, DRX_SCAN_COMMAND_STOP, NULL, NULL );

   /* All done, invalidate scan-init */
   commonAttr->scanParam         = NULL;
   commonAttr->scanMaxChannels   = 0;
   commonAttr->scanActive        = FALSE;

   return status;
}

/*============================================================================*/

/**
* \brief Scan for next channel.
* \param demod:         Pointer to demodulator instance.
* \param scanProgress:  Pointer to scan progress.
* \return DRXStatus_t.
* \retval DRX_STS_OK:          Channel found, DRX_CTRL_GET_CHANNEL can be used
*                              to retrieve channel parameters.
* \retval DRX_STS_BUSY:        Tried part of the channels, as specified in
*                              numTries field of scan parameters. At least one
*                              more call to DRX_CTRL_SCAN_NEXT is needed to
*                              complete scanning.
* \retval DRX_STS_READY:       Reached end of scan range.
* \retval DRX_STS_ERROR:       Something went wrong.
* \retval DRX_STS_INVALID_ARG: Wrong parameters. The scanProgress may be NULL.
*
* Progress indication will run from 0 upto DRX_SCAN_MAX_PROGRESS during scan.
*
*/
static DRXStatus_t
CtrlScanNext(  pDRXDemodInstance_t  demod,
               pu16_t               scanProgress )
{
   pDRXCommonAttr_t  commonAttr  = (pDRXCommonAttr_t)(NULL);
   pBool_t           scanReady   = (pBool_t)(NULL);
   u16_t             maxProgress = DRX_SCAN_MAX_PROGRESS;
   u32_t             numTries    = 0;
   u32_t             i           = 0;

   commonAttr = (pDRXCommonAttr_t)demod -> myCommonAttr;

   /* Check scan parameters */
   if ( scanProgress == NULL )
   {
      commonAttr->scanActive = FALSE;
      return DRX_STS_INVALID_ARG;
   }

   *scanProgress           = 0;
   commonAttr->scanActive  = TRUE;
   if (  ( commonAttr->scanParam == NULL) ||
         ( commonAttr->scanMaxChannels == 0 ) )
   {
      /* CtrlScanInit() was not called succesfully before CtrlScanNext() */
      commonAttr->scanActive = FALSE;
      return DRX_STS_ERROR;
   }

   *scanProgress = (u16_t)(((commonAttr->scanChannelsScanned)*
                              ((u32_t)(maxProgress)))/
                                 (commonAttr->scanMaxChannels));

   /* Scan */
   numTries = commonAttr->scanParam->numTries;
   scanReady = &(commonAttr->scanReady);

   for ( i = 0; ( (i < numTries) && ( (*scanReady) == FALSE) ); i++)
   {
      DRXChannel_t         scanChannel    = { 0 };
      DRXStatus_t          status      = DRX_STS_ERROR;
      pDRXFrequencyPlan_t  freqPlan    = (pDRXFrequencyPlan_t)(NULL);
      Bool_t               nextChannel = FALSE;
      void                 *scanContext   = NULL;

      /* Next channel to scan */
      freqPlan =
         &(commonAttr->scanParam->frequencyPlan[commonAttr->scanFreqPlanIndex]);
      scanChannel.frequency      = commonAttr->scanNextFrequency;
      scanChannel.bandwidth      = freqPlan->bandwidth;
      scanChannel.mirror         = DRX_MIRROR_AUTO;
      scanChannel.constellation  = DRX_CONSTELLATION_AUTO;
      scanChannel.hierarchy      = DRX_HIERARCHY_AUTO;
      scanChannel.priority       = DRX_PRIORITY_HIGH;
      scanChannel.coderate       = DRX_CODERATE_AUTO;
      scanChannel.guard          = DRX_GUARD_AUTO;
      scanChannel.fftmode        = DRX_FFTMODE_AUTO;
      scanChannel.classification = DRX_CLASSIFICATION_AUTO;
      scanChannel.symbolrate     = 0;
      scanChannel.interleavemode = DRX_INTERLEAVEMODE_AUTO;
      scanChannel.ldpc           = DRX_LDPC_AUTO;
      scanChannel.carrier        = DRX_CARRIER_AUTO;
      scanChannel.framemode      = DRX_FRAMEMODE_AUTO;
      scanChannel.pilot          = DRX_PILOT_AUTO;

      /* Call default or device-specific scanning function */
      scanContext = GetScanContext(demod, scanContext);

      status = (*(GetScanFunction( demod )))
               ( scanContext,DRX_SCAN_COMMAND_NEXT,&scanChannel,&nextChannel );

      /* Proceed to next channel if requested */
      if ( nextChannel == TRUE )
      {
         DRXStatus_t nextStatus = DRX_STS_ERROR;
         DRXFrequency_t skip = 0;

         if ( status == DRX_STS_OK )
         {
            /* a channel was found, so skip some frequency steps */
            skip = commonAttr->scanParam->skip;
         }
         nextStatus = ScanPrepareNextScan( demod, skip );

         /* keep track of progress */
         *scanProgress = (u16_t)(((commonAttr->scanChannelsScanned)*
                              ((u32_t)(maxProgress)))/
                              (commonAttr->scanMaxChannels));

         if ( nextStatus != DRX_STS_OK )
         {
            commonAttr->scanActive = FALSE;
            return (nextStatus);
         }
      }
      if ( status != DRX_STS_BUSY )
      {
         /* channel found or error */
         commonAttr->scanActive = FALSE;
         return status;
      }
   } /* for ( i = 0; i < ( ... numTries); i++) */

   if ( (*scanReady) == TRUE )
   {
      /* End of scan reached: call stop-scan, ignore any error */
      CtrlScanStop( demod );
      commonAttr->scanActive = FALSE;
      return (DRX_STS_READY);
   }

   commonAttr->scanActive = FALSE;

   return DRX_STS_BUSY;
}

#endif /* #ifndef DRX_EXCLUDE_SCAN */

/*============================================================================*/

/**
* \brief Program tuner.
* \param demod:         Pointer to demodulator instance.
* \param tunerChannel:  Pointer to tuning parameters.
* \return DRXStatus_t.
* \retval DRX_STS_OK:          Tuner programmed successfully.
* \retval DRX_STS_ERROR:       Something went wrong.
* \retval DRX_STS_INVALID_ARG: Wrong parameters.
*
* tunerChannel passes parameters to program the tuner,
* but also returns the actual RF and IF frequency from the tuner.
*
*/
static DRXStatus_t
CtrlProgramTuner( pDRXDemodInstance_t  demod,
                  pDRXChannel_t        channel )
{
   pDRXCommonAttr_t  commonAttr     = (pDRXCommonAttr_t)(NULL);
   DRXStandard_t     standard       = DRX_STANDARD_UNKNOWN;
   TUNERMode_t       tunerMode      = 0;
   DRXStatus_t       status         = DRX_STS_ERROR;
   DRXFrequency_t    ifFrequency    = 0;
   Bool_t            tunerSlowMode  = FALSE;

   /* can't tune without a tuner */
   if ( demod->myTuner == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   commonAttr = (pDRXCommonAttr_t) demod->myCommonAttr;

   /* select analog or digital tuner mode based on current standard */
   if ( DRX_Ctrl( demod, DRX_CTRL_GET_STANDARD, &standard ) != DRX_STS_OK )
   {
      return DRX_STS_ERROR;
   }

   if ( DRX_ISATVSTD( standard ) )
   {
      tunerMode |= TUNER_MODE_ANALOG;
   }
   else /* note: also for unknown standard */
   {
      tunerMode |= TUNER_MODE_DIGITAL;
   }

   /* select tuner bandwidth */
   switch ( channel->bandwidth )
   {
      case DRX_BANDWIDTH_6MHZ:
         tunerMode |= TUNER_MODE_6MHZ;
         break;
      case DRX_BANDWIDTH_7MHZ:
         tunerMode |= TUNER_MODE_7MHZ;
         break;
      case DRX_BANDWIDTH_8MHZ:
         tunerMode |= TUNER_MODE_8MHZ;
         break;
      default: /* note: also for unknown bandwidth */
         return DRX_STS_INVALID_ARG;
   }

   DRX_GET_TUNERSLOWMODE (demod, tunerSlowMode);

   /* select fast (switch) or slow (lock) tuner mode */
   if ( tunerSlowMode )
   {
      tunerMode |= TUNER_MODE_LOCK;
   }
   else
   {
      tunerMode |= TUNER_MODE_SWITCH;
   }

   if ( commonAttr->tunerPortNr == 1 )
   {
      Bool_t      bridgeClosed = TRUE;
      DRXStatus_t statusBridge = DRX_STS_ERROR;

      statusBridge = DRX_Ctrl( demod, DRX_CTRL_I2C_BRIDGE, &bridgeClosed );
      if ( statusBridge != DRX_STS_OK )
      {
         return statusBridge;
      }
   }

   status = DRXBSP_TUNER_SetFrequency( demod->myTuner,
                                       tunerMode,
                                       channel->frequency );

   /* attempt restoring bridge before checking status of SetFrequency */
   if ( commonAttr->tunerPortNr == 1 )
   {
      Bool_t      bridgeClosed = FALSE;
      DRXStatus_t statusBridge = DRX_STS_ERROR;

      statusBridge = DRX_Ctrl( demod, DRX_CTRL_I2C_BRIDGE, &bridgeClosed );
      if ( statusBridge != DRX_STS_OK )
      {
         return statusBridge;
      }
   }

   /* now check status of DRXBSP_TUNER_SetFrequency */
   if ( status != DRX_STS_OK )
   {
      return status;
   }

   /* get actual RF and IF frequencies from tuner */
   status = DRXBSP_TUNER_GetFrequency( demod->myTuner,
                                       tunerMode,
                                       &(channel->frequency),
                                       &(ifFrequency) );
   if ( status != DRX_STS_OK )
   {
      return status;
   }

   /* update common attributes with information available from this function;
      TODO: check if this is required and safe */
   DRX_SET_INTERMEDIATEFREQ( demod, ifFrequency );

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief function to do a register dump.
* \param demod:            Pointer to demodulator instance.
* \param registers:        Registers to dump.
* \return DRXStatus_t.
* \retval DRX_STS_OK:          Dump executed successfully.
* \retval DRX_STS_ERROR:       Something went wrong.
* \retval DRX_STS_INVALID_ARG: Wrong parameters.
*
*/
DRXStatus_t CtrlDumpRegisters(   pDRXDemodInstance_t  demod,
                                 pDRXRegDump_t        registers )
{
   u16_t i = 0;

   if ( registers == NULL )
   {
      /* registers not supplied */
      return DRX_STS_INVALID_ARG;
   }

   /* start dumping registers */
   while ( registers[i].address != 0 )
      {
      DRXStatus_t status = DRX_STS_ERROR;
      u16_t       value  = 0;
      u32_t       data   = 0;

      status = demod->myAccessFunct->readReg16Func(
                        demod->myI2CDevAddr, registers[i].address, &value, 0 );

      data = (u32_t)value;

      if ( status != DRX_STS_OK )
         {
         /* no breakouts;
               depending on device ID, some HW blocks might not be available*/
         data |= ( (u32_t)status ) << 16;
         }
      registers[i].data = data;
      i++;
   }

   /* all done, all OK (any errors are saved inside data) */
   return DRX_STS_OK;
}

/*============================================================================*/
/*============================================================================*/
/*===Microcode related functions==============================================*/
/*============================================================================*/
/*============================================================================*/

/**
* \brief Read a 16 bits word, expects big endian data.
* \param addr: Pointer to memory from which to read the 16 bits word.
* \return u16_t The data read.
*
* This function takes care of the possible difference in endianness between the
* host and the data contained in the microcode image file.
*
*/
static u16_t
UCodeRead16( pu8_t addr)
{
   /* Works fo any host processor */

   u16_t word=0;

   word = ((u16_t)addr[0]);
   word <<= 8;
   word |=((u16_t)addr[1]);

   return word;
}

/*============================================================================*/

/**
* \brief Read a 32 bits word, expects big endian data.
* \param addr: Pointer to memory from which to read the 32 bits word.
* \return u32_t The data read.
*
* This function takes care of the possible difference in endianness between the
* host and the data contained in the microcode image file.
*
*/
static u32_t
UCodeRead32( pu8_t addr)
{
   /* Works fo any host processor */

   u32_t word=0;

   word = ((u16_t)addr[0]);
   word <<= 8;
   word |= ((u16_t)addr[1]);
   word <<= 8;
   word |= ((u16_t)addr[2]);
   word <<= 8;
   word |= ((u16_t)addr[3]);

   return word ;
}

/*============================================================================*/

/**
* \brief Compute CRC of block of microcode data.
* \param blockData: Pointer to microcode data.
* \param nrWords:   Size of microcode block (number of 16 bits words).
* \return u16_t The computed CRC residu.
*/
static u16_t
UCodeComputeCRC (pu8_t blockData, u16_t nrWords)
{
   u16_t i = 0;
   u16_t j = 0;
   u32_t CRCWord=0;
   u32_t carry=0;

   while (i < nrWords)
   {
      CRCWord |= (u32_t) UCodeRead16(blockData);
      for (j = 0; j < 16; j++)
      {
         CRCWord <<= 1;
         if (carry != 0)
         {
            CRCWord ^= 0x80050000UL;
         }
         carry = CRCWord & 0x80000000UL;
      }
      i++;
      blockData += (sizeof(u16_t));
   }
   return ((u16_t) (CRCWord >> 16));
}

/*============================================================================*/

/**
* \brief Handle microcode upload or verify.
* \param devAddr: Address of device.
* \param mcInfo:  Pointer to information about microcode data.
* \param action:  Either UCODE_UPLOAD or UCODE_VERIFY
* \return DRXStatus_t.
* \retval DRX_STS_OK:
*                    - In case of UCODE_UPLOAD: code is successfully uploaded.
*                    - In case of UCODE_VERIFY: image on device is equal to
*                      image provided to this control function.
* \retval DRX_STS_ERROR:
*                    - In case of UCODE_UPLOAD: I2C error.
*                    - In case of UCODE_VERIFY: I2C error or image on device
*                      is not equal to image provided to this control function.
* \retval DRX_STS_INVALID_ARG:
*                    - Invalid arguments.
*                    - Provided image is corrupt
*/
static DRXStatus_t
CtrlUCode(  pDRXDemodInstance_t demod,
            pDRXUCodeInfo_t  mcInfo,
            DRXUCodeAction_t action)
{
   DRXStatus_t rc;
   u16_t  i = 0;
   u16_t  mcNrOfBlks = 0;
   u16_t  mcMagicWord = 0;
   pu8_t  mcData = (pu8_t)(NULL);
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t)(NULL);

   devAddr = demod->myI2CDevAddr;

   /* Check arguments */
   if (  ( mcInfo == NULL ) ||
         ( mcInfo->mcData == NULL ) )
   {
      return DRX_STS_INVALID_ARG;
   }

   mcData = mcInfo->mcData;

   /* Check data */
   mcMagicWord = UCodeRead16( mcData );
   mcData += sizeof( u16_t );
   mcNrOfBlks = UCodeRead16( mcData );
   mcData += sizeof( u16_t );

   if (  ( mcMagicWord != DRX_UCODE_MAGIC_WORD ) ||
         ( mcNrOfBlks == 0 ) )
   {
      /* wrong endianess or wrong data ? */
      return DRX_STS_INVALID_ARG;
   }

   /* Scan microcode blocks first for version info if uploading */
   if (action == UCODE_UPLOAD)
   {
      /* Clear version block */
      DRX_SET_MCVERTYPE (demod, 0);
      DRX_SET_MCDEV     (demod, 0);
      DRX_SET_MCVERSION (demod, 0);
      DRX_SET_MCPATCH   (demod, 0);
      for (i = 0; i < mcNrOfBlks; i++)
      {
         DRXUCodeBlockHdr_t blockHdr;

         /* Process block header */
         blockHdr.addr = UCodeRead32( mcData );
         mcData += sizeof(u32_t);
         blockHdr.size = UCodeRead16( mcData );
         mcData += sizeof(u16_t);
         blockHdr.flags = UCodeRead16( mcData );
         mcData += sizeof(u16_t);
         blockHdr.CRC = UCodeRead16( mcData );
         mcData += sizeof(u16_t);

         if (blockHdr.flags & 0x8)
         {
            /* Aux block. Check type */
            pu8_t auxblk = mcInfo->mcData + blockHdr.addr;
            u16_t auxtype = UCodeRead16 (auxblk);
            if (DRX_ISMCVERTYPE (auxtype))
            {
               DRX_SET_MCVERTYPE (demod, UCodeRead16 (auxblk));
               auxblk += sizeof (u16_t);
               DRX_SET_MCDEV     (demod, UCodeRead32 (auxblk));
               auxblk += sizeof (u32_t);
               DRX_SET_MCVERSION (demod, UCodeRead32 (auxblk));
               auxblk += sizeof (u32_t);
               DRX_SET_MCPATCH   (demod, UCodeRead32 (auxblk));
            }
         }

         /* Next block */
         mcData += blockHdr.size * sizeof (u16_t);
      }

      /* After scanning, validate the microcode.
         It is also valid if no validation control exists.
      */
      rc = DRX_Ctrl (demod, DRX_CTRL_VALIDATE_UCODE, NULL);
      if (rc != DRX_STS_OK && rc != DRX_STS_FUNC_NOT_AVAILABLE)
      {  printf("%s error line %d error: %d \n",__FUNCTION__,__LINE__,rc);
         return rc;
      }

      /* Restore data pointer */
      mcData = mcInfo->mcData + 2 * sizeof( u16_t );
   }

   /* Process microcode blocks */
   for( i = 0 ; i<mcNrOfBlks ; i++ )
   {
      DRXUCodeBlockHdr_t blockHdr;
      u16_t mcBlockNrBytes = 0;

      /* Process block header */
      blockHdr.addr = UCodeRead32( mcData );
      mcData += sizeof(u32_t);
      blockHdr.size = UCodeRead16( mcData );
      mcData += sizeof(u16_t);
      blockHdr.flags = UCodeRead16( mcData );
      mcData += sizeof(u16_t);
      blockHdr.CRC = UCodeRead16( mcData );
      mcData += sizeof(u16_t);

      /* Check block header on:
         - data larger than 64Kb
         - if CRC enabled check CRC
      */
      if (  ( blockHdr.size > 0x7FFF ) ||
            ( (( blockHdr.flags & DRX_UCODE_CRC_FLAG ) != 0) &&
               ( blockHdr.CRC != UCodeComputeCRC( mcData, blockHdr.size)) )
         )
      {
      	//printf("%s error line %d \n",__FUNCTION__,__LINE__);
         /* Wrong data ! */
         return DRX_STS_INVALID_ARG;
      }

      mcBlockNrBytes = blockHdr.size * ((u16_t)sizeof(u16_t));

      if (blockHdr.size != 0 )
      {
         /* Perform the desired action */
         switch ( action ) {
            /*================================================================*/
            case UCODE_UPLOAD :
               {
                  /* Upload microcode */
                  if ( demod->myAccessFunct->writeBlockFunc(
                                    devAddr,
                                    (DRXaddr_t) blockHdr.addr,
                                    mcBlockNrBytes,
                                    mcData,
                                    0x0000) != DRX_STS_OK)
                  {//printf("%s error line %d \n",__FUNCTION__,__LINE__);
                     return (DRX_STS_ERROR);
                  } /* if */
               };
               break;

            /*================================================================*/
            case UCODE_VERIFY :
               {
                  int         result = 0;
                  u8_t        mcDataBuffer[DRX_UCODE_MAX_BUF_SIZE];
                  u32_t       bytesToCompare=0;
                  u32_t       bytesLeftToCompare=0;
                  DRXaddr_t   currAddr = (DRXaddr_t)0;
                  pu8_t       currPtr =NULL;

                  bytesLeftToCompare = mcBlockNrBytes;
                  currAddr           = blockHdr.addr;
                  currPtr            = mcData;

                  while( bytesLeftToCompare != 0 )
                  {
                     if (bytesLeftToCompare > ((u32_t)DRX_UCODE_MAX_BUF_SIZE) )
                     {
                        bytesToCompare = ((u32_t)DRX_UCODE_MAX_BUF_SIZE);
                     } 
                     else 
                     {
                        bytesToCompare = bytesLeftToCompare;
                     }

                     if ( demod->myAccessFunct->readBlockFunc(
                                    devAddr,
                                    currAddr,
                                    (u16_t)bytesToCompare,
                                    (pu8_t)mcDataBuffer,
                                    0x0000) != DRX_STS_OK)
                     {
                        return (DRX_STS_ERROR);
                     }

                     result = DRXBSP_HST_Memcmp(currPtr,
                                                mcDataBuffer,
                                                bytesToCompare);

                     if ( result != 0 )
                     {
                        return DRX_STS_ERROR;
                     }

                     currAddr           += ((DRXaddr_t)(bytesToCompare/2));
                     currPtr            = &(currPtr[bytesToCompare]);
                     bytesLeftToCompare -= ((u32_t)bytesToCompare);
                  } /* while( bytesToCompare > DRX_UCODE_MAX_BUF_SIZE ) */
               };
               break;

            /*================================================================*/
            default:
               return DRX_STS_INVALID_ARG;
               break;

         } /* switch ( action ) */
      } /* if (blockHdr.size != 0 ) */

      /* Next block */
      mcData += mcBlockNrBytes;

   } /* for( i = 0 ; i<mcNrOfBlks ; i++ ) */

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Build list of version information.
* \param demod: A pointer to a demodulator instance.
* \param versionList: Pointer to linked list of versions.
* \return DRXStatus_t.
* \retval DRX_STS_OK:          Version information stored in versionList
* \retval DRX_STS_INVALID_ARG: Invalid arguments.
*/
static DRXStatus_t
CtrlVersion(   pDRXDemodInstance_t demod,
               pDRXVersionList_t   *versionList )
{
   static char drxDriverCoreModuleName[]  = "Core driver";
   static char drxDriverCoreVersionText[] =
         DRX_VERSIONSTRING( VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH );

   static DRXVersion_t drxDriverCoreVersion;
   static DRXVersionList_t drxDriverCoreVersionList;

   pDRXVersionList_t demodVersionList = (pDRXVersionList_t)(NULL);
   DRXStatus_t returnStatus = DRX_STS_ERROR;

   /* Check arguments */
   if ( versionList == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   /* Get version info list from demod */
   returnStatus = (*(demod->myDemodFunct->ctrlFunc))(
                           demod,
                           DRX_CTRL_VERSION,
                           (void *) &demodVersionList );

   /* Always fill in the information of the driver SW . */
   drxDriverCoreVersion.moduleType  = DRX_MODULE_DRIVERCORE;
   drxDriverCoreVersion.moduleName  = drxDriverCoreModuleName;
   drxDriverCoreVersion.vMajor      = VERSION_MAJOR;
   drxDriverCoreVersion.vMinor      = VERSION_MINOR;
   drxDriverCoreVersion.vPatch      = VERSION_PATCH;
   drxDriverCoreVersion.vString     = drxDriverCoreVersionText;

   drxDriverCoreVersionList.version = &drxDriverCoreVersion;
   drxDriverCoreVersionList.next    = (pDRXVersionList_t)(NULL);

   if (( returnStatus == DRX_STS_OK ) && ( demodVersionList != NULL ))
   {
      /* Append versioninfo from driver to versioninfo from demod  */
      /* Return version info in "bottom-up" order. This way, multiple
         devices can be handled without using malloc. */
      pDRXVersionList_t currentListElement = demodVersionList;
      while ( currentListElement->next != NULL )
      {
         currentListElement = currentListElement->next;
      }
      currentListElement->next = &drxDriverCoreVersionList;

      *versionList = demodVersionList;
   }
   else
   {
      /* Just return versioninfo from driver */
      *versionList = &drxDriverCoreVersionList;
   }

   return DRX_STS_OK;
}

/*============================================================================*/
/*============================================================================*/
/*== Exported functions ======================================================*/
/*============================================================================*/
/*============================================================================*/



/**
* \brief This function is obsolete.
* \param demods: Don't care, parameter is ignored.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK: Initialization completed.
*
* This function is obsolete, prototype available for backward compatability.
*
*/

DRXStatus_t
DRX_Init(  pDRXDemodInstance_t demods[]  )
{
   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief This function is obsolete.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK: Terminated driver successful.
*
* This function is obsolete, prototype available for backward compatability.
*
*/

DRXStatus_t
DRX_Term( void )
{
   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Open a demodulator instance.
* \param demod: A pointer to a demodulator instance.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK:          Opened demod instance with succes.
* \retval DRX_STS_ERROR:       Driver not initialized or unable to initialize
*                              demod.
* \retval DRX_STS_INVALID_ARG: Demod instance has invalid content.
*
*/

DRXStatus_t
DRX_Open(pDRXDemodInstance_t demod)
{
   DRXStatus_t status = DRX_STS_OK;

   if (  ( demod == NULL )               ||
         ( demod->myDemodFunct == NULL ) ||
         ( demod->myCommonAttr == NULL ) ||
         ( demod->myExtAttr == NULL )    ||
         ( demod->myI2CDevAddr == NULL ) ||
         ( demod->myCommonAttr->isOpened == TRUE ))
   {
      return (DRX_STS_INVALID_ARG);
   }

   status = (*(demod->myDemodFunct->openFunc))( demod );

   if ( status == DRX_STS_OK )
   {
      demod->myCommonAttr->isOpened = TRUE;
   }

   return status;
}

/*============================================================================*/

/**
* \brief Close device.
* \param demod: A pointer to a demodulator instance.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK:          Closed demod instance with succes.
* \retval DRX_STS_ERROR:       Driver not initialized or error during close
*                              demod.
* \retval DRX_STS_INVALID_ARG: Demod instance has invalid content.
*
* Free resources occupied by device instance.
* Put device into sleep mode.
*/

DRXStatus_t
DRX_Close(pDRXDemodInstance_t demod)
{
   DRXStatus_t status = DRX_STS_OK;

   if (  ( demod == NULL )               ||
         ( demod->myDemodFunct == NULL ) ||
         ( demod->myCommonAttr == NULL ) ||
         ( demod->myExtAttr == NULL )    ||
         ( demod->myI2CDevAddr == NULL ) ||
         ( demod->myCommonAttr->isOpened == FALSE ))
   {
      return DRX_STS_INVALID_ARG;
   }

   status = (*(demod->myDemodFunct->closeFunc))( demod );

   DRX_SET_ISOPENED (demod, FALSE);

   return status;
}

/*============================================================================*/

/**
* \brief Control the device.
* \param demod:    A pointer to a demodulator instance.
* \param ctrl:     Reference to desired control function.
* \param ctrlData: Pointer to data structure for control function.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK:                 Control function completed successfully.
* \retval DRX_STS_ERROR:              Driver not initialized or error during
*                                     control demod.
* \retval DRX_STS_INVALID_ARG:        Demod instance or ctrlData has invalid
*                                     content.
* \retval DRX_STS_FUNC_NOT_AVAILABLE: Specified control function is not
*                                     available.
*
* Data needed or returned by the control function is stored in ctrlData.
*
*/

DRXStatus_t
DRX_Ctrl(pDRXDemodInstance_t demod, DRXCtrlIndex_t ctrl, void *ctrlData)
{
   DRXStatus_t status = DRX_STS_ERROR;

   if (  ( demod               == NULL ) ||
         ( demod->myDemodFunct == NULL ) ||
         ( demod->myCommonAttr == NULL ) ||
         ( demod->myExtAttr    == NULL ) ||
         ( demod->myI2CDevAddr == NULL )
      )
   {
      return (DRX_STS_INVALID_ARG);
   }

   if ( ( ( demod->myCommonAttr->isOpened == FALSE ) &&
            ( ctrl != DRX_CTRL_PROBE_DEVICE ) &&
            ( ctrl != DRX_CTRL_VERSION) )
      )
   {
      return (DRX_STS_INVALID_ARG);
   }

   if ( ( DRX_ISPOWERDOWNMODE( demod->myCommonAttr->currentPowerMode ) &&
            ( ctrl != DRX_CTRL_POWER_MODE )   &&
            ( ctrl != DRX_CTRL_PROBE_DEVICE ) &&
            ( ctrl != DRX_CTRL_NOP )          &&
            ( ctrl != DRX_CTRL_VERSION)
         )
      )
   {
      return DRX_STS_FUNC_NOT_AVAILABLE;
   }

   /* Fixed control functions */
   switch ( ctrl ) {
      /*======================================================================*/
      case DRX_CTRL_NOP:
         /* No operation */
         return DRX_STS_OK;
         break;

      /*======================================================================*/
      case DRX_CTRL_VERSION:
            return CtrlVersion( demod, (pDRXVersionList_t *) ctrlData );
         break;

      /*======================================================================*/
      default :
         /* Do nothing */
         break;
   }

   /* Virtual functions */
   /* First try calling function from derived class */
   status = (*(demod->myDemodFunct->ctrlFunc))( demod, ctrl, ctrlData );
   if ( status == DRX_STS_FUNC_NOT_AVAILABLE )
   {
      /* Now try calling a the base class function */
      switch ( ctrl ) {
         /*===================================================================*/
         case DRX_CTRL_LOAD_UCODE:
               return CtrlUCode (   demod,
                                    (pDRXUCodeInfo_t) ctrlData,
                                    UCODE_UPLOAD);
            break;

         /*===================================================================*/
         case DRX_CTRL_VERIFY_UCODE:
            {
               return CtrlUCode (   demod,
                                    (pDRXUCodeInfo_t) ctrlData,
                                    UCODE_VERIFY);
            }
            break;

#ifndef DRX_EXCLUDE_SCAN
         /*===================================================================*/
         case DRX_CTRL_SCAN_INIT:
            {
               return CtrlScanInit( demod, (pDRXScanParam_t) ctrlData );
            }
            break;

         /*===================================================================*/
         case DRX_CTRL_SCAN_NEXT:
            {
               return CtrlScanNext( demod, (pu16_t) ctrlData );
            }
            break;

         /*===================================================================*/
         case DRX_CTRL_SCAN_STOP:
            {
               return CtrlScanStop( demod );
            }
            break;
#endif /* #ifndef DRX_EXCLUDE_SCAN */

         /*===================================================================*/
         case DRX_CTRL_PROGRAM_TUNER:
            {
               return CtrlProgramTuner( demod, (pDRXChannel_t) ctrlData );
            }
            break;

         /*===================================================================*/
         case DRX_CTRL_DUMP_REGISTERS:
            {
               return CtrlDumpRegisters( demod, (pDRXRegDump_t) ctrlData );
            }
            break;

         /*===================================================================*/
         default :
            return DRX_STS_FUNC_NOT_AVAILABLE;
      }
   } 
   else 
   {
      return (status);
   }
   return DRX_STS_OK;
}


/*============================================================================*/

/* END OF FILE */
