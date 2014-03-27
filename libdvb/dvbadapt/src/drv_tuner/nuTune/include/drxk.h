/**
* \file $Id: drxk.h,v 1.115 2010/01/20 12:01:03 danielg Exp $
*
* \brief DRXK specific header file
*
* Contains DRXK specfic typdefs
*/

/*
* $(c) 2007-2010 Trident Microsystems, Inc. - All rights reserved.
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


#ifndef __DRXK_H__
#define __DRXK_H__

/*------------------------------------------------------------------------------
INCLUDES
------------------------------------------------------------------------------*/

#include "drx_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
DEFINES
------------------------------------------------------------------------------*/

/** /brief Identifier for DRXDemodFunc_t structure */
#ifndef DRX3913K_TYPE_ID
#define DRX3913K_TYPE_ID (0x3913004BUL)
#endif
/** /brief Identifier for DRXDemodFunc_t structure */
#ifndef DRX3915K_TYPE_ID
#define DRX3915K_TYPE_ID (0x3915004BUL)
#endif
/** /brief Identifier for DRXDemodFunc_t structure */
#ifndef DRX3916K_TYPE_ID
#define DRX3916K_TYPE_ID (0x3916004BUL)
#endif
/** /brief Identifier for DRXDemodFunc_t structure */
#ifndef DRX3918K_TYPE_ID
#define DRX3918K_TYPE_ID (0x3918004BUL)
#endif
/** /brief Identifier for DRXDemodFunc_t structure */
#ifndef DRX3921K_TYPE_ID
#define DRX3921K_TYPE_ID (0x3921004BUL)
#endif
/** /brief Identifier for DRXDemodFunc_t structure */
#ifndef DRX3923K_TYPE_ID
#define DRX3923K_TYPE_ID (0x3923004BUL)
#endif
/** /brief Identifier for DRXDemodFunc_t structure */
#ifndef DRX3925K_TYPE_ID
#define DRX3925K_TYPE_ID (0x3925004BUL)
#endif
/** /brief Identifier for DRXDemodFunc_t structure */
#ifndef DRX3926K_TYPE_ID
#define DRX3926K_TYPE_ID (0x3926004BUL)
#endif

#define DRXK_PRODUCT_IDN(__n) (0x390 + (__n))
#define DRXK_PRODUCT_ID DRXK_PRODUCT_IDN(2)

/*---------------------*/

/** /brief Intermediate lockstate, found a carrier */
#ifndef DRXK_DEMOD_LOCK
#define DRXK_DEMOD_LOCK    (DRX_LOCK_STATE_1)
#endif

/** /brief Intermediate lockstate, FEC is in lock */
#ifndef DRXK_FEC_LOCK
#define DRXK_FEC_LOCK      (DRX_LOCK_STATE_2)
#endif

/** /brief Macro to convert drxk lock status to human readable form */
#define DRXK_STR_LOCKSTATUS(x) ( \
   ( x == DRX_NEVER_LOCK               )  ?  "Never     "      : \
   ( x == DRX_NOT_LOCKED               )  ?  "No        "      : \
   ( x == DRXK_DEMOD_LOCK              )  ?  "Demod lock"      : \
   ( x == DRXK_FEC_LOCK                )  ?  "FEC lock  "      : \
   ( x == DRX_LOCKED                   )  ?  "Locked    "      : \
                                             "(Invalid) " )

/*---------------------*/

/** /brief Intermediate power mode for DRXK, power down OFDM clock domain */
#ifndef DRXK_POWER_DOWN_OFDM
#define DRXK_POWER_DOWN_OFDM        DRX_POWER_MODE_1
#endif

/** /brief Intermediate power mode for DRXK, power down core (sysclk) */
#ifndef DRXK_POWER_DOWN_CORE
#define DRXK_POWER_DOWN_CORE        DRX_POWER_MODE_9
#endif

/** /brief Intermediate power mode for DRXK, power down pll (only osc runs) */
#ifndef DRXK_POWER_DOWN_PLL
#define DRXK_POWER_DOWN_PLL         DRX_POWER_MODE_10
#endif

/** /brief Macro to convert drxk power mode to human readable form */
#define DRXK_STR_POWER_MODE(x) ( \
   ( x == DRX_POWER_UP                 )  ?  "DRX_POWER_UP        "  : \
   ( x == DRXK_POWER_DOWN_OFDM         )  ?  "DRXK_POWER_DOWN_OFDM"  : \
   ( x == DRXK_POWER_DOWN_CORE         )  ?  "DRXK_POWER_DOWN_CORE"  : \
   ( x == DRXK_POWER_DOWN_PLL          )  ?  "DRXK_POWER_DOWN_PLL "  : \
   ( x == DRX_POWER_DOWN               )  ?  "DRX_POWER_DOWN      "  : \
                                             "(Invalid)           " )

/*---------------------*/
/**
* \def DRX_CARRIER_OFFSET_PAL_NTSC
* \brief Offset from picture carrier to centre frequency in kHz, in RF domain
*/
#define DRX_CARRIER_OFFSET_PAL_NTSC             ((DRXFrequency_t)(1750))

/**
* \def DRX_CARRIER_OFFSET_PAL_SECAM_BG_8MHZ
* \brief Offset from picture carrier to centre frequency in kHz, in RF domain
* for 8MHz channels
*/
#define DRX_CARRIER_OFFSET_PAL_SECAM_BG_8MHZ    ((DRXFrequency_t)(2750))

/**
* \def DRX_CARRIER_OFFSET_PAL_SECAM_BG_7MHZ
* \brief Offset from picture carrier to centre frequency in kHz, in RF domain
* for 7MHz channels*
*/
#define DRX_CARRIER_OFFSET_PAL_SECAM_BG_7MHZ    ((DRXFrequency_t)(2250))

/**
* \def DRX_CARRIER_OFFSET_PAL_SECAM_DKIL
* \brief Offset from picture carrier to centre frequency in kHz, in RF domain
*/
#define DRX_CARRIER_OFFSET_PAL_SECAM_DKIL       ((DRXFrequency_t)(2750))

/**
* \def DRX_CARRIER_OFFSET_PAL_SECAM_LP
* \brief Offset from picture carrier to centre frequency in kHz, in RF domain
*/
#define DRX_CARRIER_OFFSET_PAL_SECAM_LP         ((DRXFrequency_t)(-2750))

/**
* \def DRX_CARRIER_OFFSET_FM
* \brief Offset from sound carrier to centre frequency in kHz, in RF domain
*/
#ifndef DRX_CARRIER_OFFSET_FM
#define DRX_CARRIER_OFFSET_FM                   ((DRXFrequency_t)(-3000))
#endif

/**
* \def   DRXK_QAM_BANDWIDTH_BOUNDARY_6MHZ
* \brief QAM Bandwidth boundary (switching from 6MHz to 7MHz).
*/
#ifndef DRXK_QAM_BANDWIDTH_BOUNDARY_6MHZ
#define DRXK_QAM_BANDWIDTH_BOUNDARY_6MHZ (5500000)
#endif

/**
* \def   DRXK_QAM_BANDWIDTH_BOUNDARY_7MHZ
* \brief QAM Bandwidth boundary (switching from 7MHz to 8MHz).
*/
#ifndef DRXK_QAM_BANDWIDTH_BOUNDARY_7MHZ
#define DRXK_QAM_BANDWIDTH_BOUNDARY_7MHZ (6400000)
#endif

/**
* \brief DRXK specific UIO operational modes
*/
#ifndef DRXK_UIO_MODE_FIRMWARE_SAW
#define DRXK_UIO_MODE_FIRMWARE_SAW (DRX_UIO_MODE_FIRMWARE0)
#endif

/**
* \brief DRXK specific UIO operational modes
*/
#ifndef DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR
#define DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR (DRX_UIO_MODE_FIRMWARE1)
#endif

/*---------------------*/

/** /brief Base for ctrl cfg function identifiers */
#define DRXK_CTRL_CFG_BASE (0x1000)

/*------------------------------------------------------------------------------
TYPEDEFS
------------------------------------------------------------------------------*/
#define DRXK_ISATVSTD( std ) ( ( std == DRX_STANDARD_PAL_SECAM_BG ) || \
                               ( std == DRX_STANDARD_PAL_SECAM_DK ) || \
                               ( std == DRX_STANDARD_PAL_SECAM_I  ) || \
                               ( std == DRX_STANDARD_PAL_SECAM_L  ) || \
                               ( std == DRX_STANDARD_PAL_SECAM_LP ) || \
                               ( std == DRX_STANDARD_NTSC ) || \
                               ( std == DRX_STANDARD_FM ) )

/**
* \brief Macro to check if std is an QAM standard
* \retval TRUE std is a QAM standard
* \retval FALSE std is not a QAM standard
*/
#define DRXK_ISQAMSTD( std ) ( ( std == DRX_STANDARD_ITU_A ) || \
                               ( std == DRX_STANDARD_ITU_C ) )

/**
* \brief Macro to check if std is DVBT standard
* \retval TRUE std is DVBT standard
* \retval FALSE std is not DVBT standard
*/
#define DRXK_ISDVBTSTD( std )  ( std == DRX_STANDARD_DVBT )




/**
* /brief SCU command.
*
* Exported for test purposes.
*
*/
typedef struct {
   u16_t command;       /**< Command number */
   u16_t parameterLen;  /**< Data length in byte */
   u16_t resultLen;     /**< result length in byte */
   u16_t *parameter;    /**< General purpous param */
   u16_t *result;       /**< General purpous param */
} DRXKSCUCmd_t, *pDRXKSCUCmd_t;


/**
* /brief Function identifiers for DRX_CTRL_SET/GET_CFG.
*/
typedef enum {
   DRXK_CFG_AGC_RF = DRXK_CTRL_CFG_BASE,
   DRXK_CFG_AGC_IF,
   DRXK_CFG_PRE_SAW,
   DRXK_CFG_SYMBOL_CLK_OFFSET,
   DRXK_CFG_PACKET_ERR_MODE,

   /* ATV (FM) */
   DRXK_CFG_ATV_OUTPUT,
   DRXK_CFG_ATV_MISC,
   DRXK_CFG_ATV_EQU_COEF,
   DRXK_CFG_ATV_AGC_STATUS,
   DRXK_CFG_ATV_MODE,

   /* QAM */
   DRXK_CFG_QAM_SYMBOLRATE,

   /* DVBT */
   DRXK_CFG_DVBT_FR,
   DRXK_CFG_DVBT_ECHO_THRES,
   DRXK_CFG_DVBT_INC,
   DRXK_CFG_DVBT_SQI_SPEED,
   DRXK_CFG_RESET_BER,

   DRXK_CFG_MISC,
   DRXK_CFG_MAX /* dummy, never to be used */

} DRXKCfgType_t, *pDRXKCfgType_t;

/* DRXK_CFG_AGC_RF, DRXK_CFG_AGC_IF */


/* Spin of silicon device */
typedef enum {
   DRXK_SPIN_A1 = 0,
   DRXK_SPIN_A2,
   DRXK_SPIN_A3,
   DRXK_SPIN_UNKNOWN
} DRXKSpinType_t, *pDRXKSpinType_t;


/**
* /brief Available AGCs modes in the DRXK.
*/
typedef enum {
   DRXK_AGC_CTRL_AUTO = 0,
   DRXK_AGC_CTRL_USER,
   DRXK_AGC_CTRL_OFF
} DRXKAgcCtrlMode_t, *pDRXKAgcCtrlMode_t;

/**
* /brief Generic interface for all AGCs present on the DRXK.
*/
typedef struct {
   DRXStandard_t     standard;       /* associated standard */
   DRXKAgcCtrlMode_t ctrlMode;       /* off, user, auto */
   u16_t             outputLevel;    /* range dependent on AGC */
   u16_t             minOutputLevel; /* range dependent on AGC */
   u16_t             maxOutputLevel; /* range dependent on AGC */
   u16_t             speed;          /* range dependent on AGC */
   u16_t             top;            /* rf-agc take over point */
   u16_t             cutOffCurrent;  /*   rf-agc is accelerated if output
                                          current is below cut-off current */
}DRXKCfgAgc_t, *pDRXKCfgAgc_t;

/* DRXK_CFG_PRE_SAW */

/**
* /brief Interface to configure pre SAW sense.
*/
typedef struct {
   DRXStandard_t standard;  /* standard to which these settings apply */
   u16_t         reference; /* pre SAW reference value, range 0 .. 31 */
   Bool_t        usePreSaw; /* TRUE algorithms must use pre SAW sense */
} DRXKCfgPreSaw_t, *pDRXKCfgPreSaw_t;

/**
* /struct DRXKCfgMisc_t
* Change TEI bit of Mpeg output
*/
typedef struct{
   Bool_t    disableTEIHandling;        /**< if TRUE do not change TEI bit */
}DRXKCfgMisc_t, *pDRXKCfgMisc_t;

/*
 * DRXKAtvImbalanceCtrlMode_t
 */
typedef enum {
   DRXK_ATV_IMBALANCE_OFF,        /* do not perform imbalance control */
   DRXK_ATV_IMBALANCE_MODE_A,     /* preffered */
   DRXK_ATV_IMBALANCE_MODE_B,     /* only handle strong signal level */
   DRXK_ATV_IMBALANCE_FREEZE      /* freeze current setting */
} DRXKAtvImbalanceCtrlMode_t, *pDRXKAtvImbalanceCtrlMode_t;

/*
*  DRXK_CFG_ATV_MISC
*/
typedef struct{
   DRXKAtvImbalanceCtrlMode_t  imbalanceCtrlMode;
   Bool_t                      phaseCorrectionBypass; /* TRUE=bypass */
   s16_t                       peakFilter;  /* -8 .. 15 */
   u16_t                       noiseFilter; /* 0 .. 15 */
}DRXKCfgAtvMisc_t, *pDRXKCfgAtvMisc_t;

/*
*  DRXK_CFG_ATV_EQU_COEF
*/
typedef struct {
   s16_t coef0;  /* -256 .. 255 */
   s16_t coef1;  /* -256 .. 255 */
   s16_t coef2;  /* -256 .. 255 */
   s16_t coef3;  /* -256 .. 255 */
} DRXKCfgAtvEquCoef_t, *pDRXKCfgAtvEquCoef_t;

/*
*  DRXK_CFG_ATV_OUTPUT
*/

/**
* /enum DRXKAttenuation_t
* Attenuation setting for SIF AGC.
*
*/
typedef enum {
   DRXK_SIF_ATTENUATION_0DB,
   DRXK_SIF_ATTENUATION_3DB,
   DRXK_SIF_ATTENUATION_6DB,
   DRXK_SIF_ATTENUATION_9DB
} DRXKSIFAttenuation_t, *pDRXKSIFAttenuation_t;

/**
* /struct DRXKCfgAtvOutput_t
* SIF attenuation setting.
*
*/
typedef struct {
   Bool_t               enableCVBSOutput; /* TRUE= enabled */
   Bool_t               enableSIFOutput;  /* TRUE= enabled */
   DRXKSIFAttenuation_t sifAttenuation;
} DRXKCfgAtvOutput_t, *pDRXKCfgAtvOutput_t;

/**
* /enum DRXKAtvMode_t
* ATV operational mode.
*
*/
typedef enum {
   DRXK_ATV_MODE_SCAN,
   DRXK_ATV_MODE_LOCK
} DRXKAtvMode_t, *pDRXKAtvMode_t;

/**
* /enum DRXKCfgPacketErr_t
* PacketError configuration setting.
*
*/
typedef enum {
   DRXK_PACKET_ERR_AVERAGE,
   DRXK_PACKET_ERR_ACCUMULATIVE
 } DRXKCfgPacketErrMode_t, *pDRXKCfgPacketErrMode_t;

/**
* /struct DRXKCfgDvbtEchoThres_t
* Echo Threshold setting.
*
*/
typedef struct {
   u16_t             threshold;
   DRXFftmode_t      fftMode;
} DRXKCfgDvbtEchoThres_t, *pDRXKCfgDvbtEchoThres_t;

typedef enum {
   DRXK_DVBT_SQI_SPEED_FAST = 0,
   DRXK_DVBT_SQI_SPEED_MEDIUM = 3,
   DRXK_DVBT_SQI_SPEED_SLOW   = 5,
   DRXK_DVBT_SQI_SPEED_UNKNOWN = DRX_UNKNOWN
} DRXKCfgDvbtSqiSpeed_t, *pDRXKCfgDvbtSqiSpeed_t;

/*
   DRXK_CFG_ATV_AGC_STATUS (get only)
*/
/* TODO : AFE interface not yet finished, subject to change */
typedef struct {
   u16_t rfAgcGain       ; /* 0 .. 877 uA */
   u16_t ifAgcGain       ; /* 0 .. 877  uA */
   s16_t videoAgcGain    ; /* -75 .. 1972 in 0.1 dB steps */
   s16_t audioAgcGain    ; /* -4 .. 1020 in 0.1 dB steps */
   u16_t rfAgcLoopGain   ; /* 0 .. 7 */
   u16_t ifAgcLoopGain   ; /* 0 .. 7 */
   u16_t videoAgcLoopGain; /* 0 .. 7 */
} DRXKCfgAtvAgcStatus_t, *pDRXKCfgAtvAgcStatus_t;

/* ATV config changed flags */
#define DRXK_ATV_CHANGED_COEF          ( 0x00000001UL )
#define DRXK_ATV_CHANGED_IMB_CTRL      ( 0x00000002UL )
#define DRXK_ATV_CHANGED_PC_BYPASS     ( 0x00000004UL )
#define DRXK_ATV_CHANGED_PEAK_FLT      ( 0x00000008UL )
#define DRXK_ATV_CHANGED_NOISE_FLT     ( 0x00000010UL )
#define DRXK_ATV_CHANGED_STDBY         ( 0x00000020UL )
#define DRXK_ATV_CHANGED_SIF_ATT       ( 0x00000040UL )

/**
* /brief DRXK specific preset levels.
*
*/
typedef enum
{
   DRXK_PRESET_LEVEL_BASELINE,
   DRXK_PRESET_LEVEL_OPEN,
   DRXK_PRESET_LEVEL_STANDARD,
   DRXK_PRESET_LEVEL_CHANNEL,
   DRXK_PRESET_LEVEL_MAX

} DRXKPresetLevel_t, *pDRXKPresetLevel_t;

/**
* /brief DRXK specific presets.
*
*/
typedef struct
{
   /* Tuner */
   u8_t            subMode;            /**< Desired tuner submode */
   Bool_t          tunerIfAgcPol;      /**< IF AGC polarity
                                          (FALSE = Normal, TRUE = Inverted) */
   u8_t            tunerPortNr;        /**< I2C port that tuner is connected to
                                          (0 = primary, 1 = secundary) */
   u16_t           tunerRfGain;        /**< Var. gain of the RF stage [dB] */
   u16_t           tunerIfGain;        /**< Var. gain of the IF stage [dB] */
   Bool_t          tunerMirrors;       /**< Tuner mirrors */

   /* AGC */
   DRXKCfgAgc_t    rfAgcCfg;           /**< RF AGC configuration */
   s16_t           rfAgcTopOffset;
   Bool_t          rfAgcSwitch;        /**< Default UIO2 RF AGC switch value,
                                          in case UIO2 is in FIRMWARE mode */

   DRXKCfgPreSaw_t preSawCfg;          /**< Pre SAW configuration */
   DRXKCfgAgc_t    ifAgcCfg;           /**< IF AGC configuration */
   u16_t           agcFastClipCtrlDelay; /**< AGC fast clipping control delay */
   u16_t           agcIngainTgtMax;    /**< Inner Loop gain Tgt maximum */


   /* ATV */
   Bool_t          useTgtCarrierIf;  /**< TRUE to use target IF freq values
                                          (ATV standards only) */
   DRXFrequency_t  tgtCarrierIf;     /**< Target IF frequency values
                                          (ATV standards only) */
   s16_t           atvTopEqu[4];     /**< ATV_TOP_EQU[0..3] configuration
                                          (ATV standards only) */
   u16_t           atvCvbsRefLvl;    /**< CVBS reference level, (ATV standards
                                        with negative modulation only) */
   u16_t           atvCrAmpThres;    /**< Carrier Recovery Amplitude Threshold
                                          (ATV standards only) */
   u16_t           atvCrOvmThres;    /**< Carrier Recovery Overmodulation
                                          Threshold
                                          (ATV standards only) */
   u16_t           atvTopNoiseTh;    /**< Noise reduction  */
   u16_t           atvNAgcKiMin;     /**< Minimum KI setting negative mod. */
   s16_t           peakFilter;       /**< peak filter (ATV standards only) */



   /* QAM */
   u32_t           qamBWBound6MHz;     /**< Bandwidth boundary for saw switch
                                         from 6 to 7 Mhz*/
   u32_t           qamBWBound7MHz;     /**< Bandwidth boundary for saw switch
                                         from 7 to 8 MHz*/

   /* OFDM */
   u16_t           dvbtEchoThres2k;       /**< Echo threshold for 2k FFT mode */
   u16_t           dvbtEchoThres8k;       /**< Echo threshold for 8k FFT mode */
   Bool_t          dvbtFrEnable;          /**< Resonator enabled */
   Bool_t          dvbtImpNoiseCrunchEnable;
                                          /**< INC enabled */

   Bool_t          hasChannelPresets;     /**<  channel specific presets */
   DRXFilterInfo_t   channelFilter;          /**< channel filter */
   u16_t             cfScale;                /**< channel filter scale */

   DRXCfgAudPrescale_t prescale;             /**< audio prescaleres */
   DRXFrequency_t       lockRangeMin;     /**< lock-in range minimum */
   DRXFrequency_t       lockRangeMax;     /**< lock-in range maximum */
} DRXKPresets_t, *pDRXKPresets_t;


/**
* /brief DRXK specific attributes.
*
*/
typedef struct {

   DRXStandard_t    standard;       /**< Current standard information         */
   DRXSubstandard_t substandard;    /**< Current substandard                  */
   Bool_t           standardActive; /**< A standard has been set              */
   Bool_t           channelActive;  /**< A channel has been set               */
   DRXChannel_t     currentChannel; /**< Current channel                      */
   u32_t            sysClkInHz;     /**< high precision sys clock             */

   /* UIO configuartion */
   DRXUIOMode_t            uioSawSwMode;    /**< Current mode of SawSw/UIO1   */
   DRXUIOMode_t            uioGPIO2Mode;    /**< Current mode of GPIO2/UIO2   */
   DRXUIOMode_t            uioGPIO1Mode;    /**< Current mode of GPIO1/UIO3   */

   /* HI configuration */
   u16_t  HICfgTimingDiv;                   /**< HI Configure() parameter 2   */
   u16_t  HICfgBridgeDelay;                 /**< HI Configure() parameter 3   */
   u16_t  HICfgWakeUpKey;                   /**< HI Configure() parameter 4   */
   u16_t  HICfgCtrl;                        /**< HI Configure() parameter 5   */
   u16_t  HICfgTimeout;                     /**< HI Configure() parameter 6   */

   /* Capabilities */
   Bool_t hasLNA;                           /**< TRUE if LNA (PGA) present    */
   Bool_t hasOOB;                           /**< TRUE if OOB supported        */
   Bool_t hasATV;                           /**< TRUE if ATV supported        */
   Bool_t hasAudio;                         /**< TRUE if audio supported      */
   Bool_t hasDVBT;                          /**< TRUE if DVB-T supported      */
   Bool_t hasDVBC;                          /**< TRUE if DVB-C supported      */
   Bool_t hasSAWSW;                         /**< TRUE if SAW_SW is available  */
   Bool_t hasGPIO2;                         /**< TRUE if GPIO2 is available   */
   Bool_t hasGPIO1;                         /**< TRUE if GPIO1 is available   */
   Bool_t hasIRQN;                          /**< TRUE if IRQN is available    */

   /* hardware detection */
   u8_t   mfx;                              /**< metal fix                    */

   /* IQM fs frequecy shift and inversion */
   u32_t  iqmFsRateOfs;       /**< frequency shifter setting after setchannel */
   u32_t  iqmRcRateOfs;       /**< RC rate offset setting after setchannel    */
   Bool_t posImage;           /**< Tune: positive image                       */

   /* ATV configuration */
   u32_t        atvCfgChangedFlags;            /**< flag: flags cfg changes   */
   DRXKAtvImbalanceCtrlMode_t
                      atvImbCtrlMode;        /**< current imbalance ctrl mode */
   Bool_t             phaseCorrectionBypass; /**< flag: TRUE=bypass           */
   Bool_t             enableCVBSOutput;   /**< flag CVBS ouput enable         */
   Bool_t             enableSIFOutput;    /**< flag SIF ouput enable          */
   DRXKSIFAttenuation_t
                      sifAttenuation;     /**< current SIF att setting        */
   s16_t       atvSVRThreshold;            /**< sound/video ratio threshold   */

   /* Bootloader usage */
   Bool_t             useBootloader;   /**< TRUE if ucode upload by bootloader*/

   /* Version information */
   char               vText[2][12];       /**< allocated text versions        */
   DRXVersion_t       vVersion[2];        /**< allocated versions structs     */
   DRXVersionList_t   vListElements[2];   /**< allocated version list         */

   /* QAM specific fields */
   /* TODO: remove next field because it's obsolete */
   u16_t                   qamChannelBwOffset;
   u32_t                   qamBERFrameLength;  /**< BER frame length [ms] */
   DRXInterleaveModes_t    qamInterleaveMode;
   Bool_t                  qamChannelIsSet;
   Bool_t                  qamScanForQam128;
   DRXConstellation_t      qamScanConstellation;
   DRXQamLockRange_t       qamLockRange;


   /* ucode detection */
   Bool_t ucodeUploaded;                    /**< detect ucode upload via CTRL */

   /* pin-safe mode */
   Bool_t pdrSafeMode;                      /**< PDR safe mode activated      */
   u16_t  pdrSafeRestoreValGpio;
   u16_t  pdrSafeRestoreValVSync;
   u16_t  pdrSafeRestoreValSmaRx;
   u16_t  pdrSafeRestoreValSmaTx;


   /* packet error cumulative mode */
   DRXKCfgPacketErrMode_t packetErrMode;

   /* device spin */
   DRXKSpinType_t deviceSpin;

   /* presets */
   char          *presetName;
   DRXKPresets_t  presets;

   /* adc compenstation coefficients */
   u16_t          adcCompCoef[64];
   u16_t          adcState;
   Bool_t         adcCompPassed;
   
   DRXKCfgDvbtSqiSpeed_t sqiSpeed;  /* DVBT SQI speed */
   DRXAudData_t   audData;          /* audio storage */
   DRXFrequency_t tunerSetFreq;     /* Tuner frequency */
   




} DRXKData_t, *pDRXKData_t;


/*-------------------------------------------------------------------------
Access MACROS
-------------------------------------------------------------------------*/

/**
* \brief Compilable references to attributes
* \param d pointer to demod instance
*
* Used as main reference to an attribute field.
* Can be used by both macro implementation and function implementation.
* These macros are defined to avoid duplication of code in macro and function
* definitions that handle access of demod common or extended attributes.
*
*/

#define DRXK_ATTR_AGC_TOP_OFFSET( d )                       \
                        (((pDRXKData_t)(d)->myExtAttr)->presets.rfAgcTopOffset)


#define DRXK_ATTR_STD_ACTIVE( d )                       \
                        (((pDRXKData_t)(d)->myExtAttr)->standardActive)



/**
* \brief Actual access macros
* \param d pointer to demod instance
* \param x value to set or to get
*
* SET macros must be used to set the value of an attribute.
* GET macros must be used to retrieve the value of an attribute.
* Depending on the value of DRX_USE_ACCESS_FUNCTIONS the macro's will be
* substituted by "direct-access-inline-code" or a function call.
*
*/

/**************************/


#define DRXK_SET_AGC_TOP_OFFSET( d, x )                     \
   do {                                                     \
      DRXK_ATTR_AGC_TOP_OFFSET( d ) = (x);                  \
      DRXK_ATTR_STD_ACTIVE( d ) = (Bool_t) FALSE;           \
   } while(0)

#define DRXK_GET_AGC_TOP_OFFSET( d, x )                     \
   do {                                                     \
      (x) = DRXK_ATTR_AGC_TOP_OFFSET( d );                  \
   } while(0)

/*------------------------------------------------------------------------------
Exported FUNCTIONS
------------------------------------------------------------------------------*/

extern DRXStatus_t DRXK_Open(pDRXDemodInstance_t  demod);
extern DRXStatus_t DRXK_Close(pDRXDemodInstance_t demod);
extern DRXStatus_t DRXK_Ctrl(pDRXDemodInstance_t  demod,
                             DRXCtrlIndex_t       ctrl,
                             void                 *ctrlData);

/*------------------------------------------------------------------------------
Exported GLOBAL VARIABLES
------------------------------------------------------------------------------*/
extern DRXAccessFunc_t     drxDapDRXKFunct_g;

extern DRXDemodFunc_t      DRXKFunctions_g;
extern DRXKData_t          DRXKData_g;
extern I2CDeviceAddr_t     DRXKDefaultAddr_g;
extern DRXCommonAttr_t     DRXKDefaultCommAttr_g;
extern DRXDemodInstance_t  DRXKDefaultDemod_g;

/*------------------------------------------------------------------------------
THE END
------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif /* __DRXK_H__ */

