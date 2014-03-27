/**
* \file $Id: drxk_preset.c.inc,v 1.281 2010/04/07 15:33:13 jasper Exp $
*
* \brief board specific presets for DRXK
*
* $(c) 2008-2010 Trident Microsystems, Inc. - All rights reserved.
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

/*-----------------------------------------------------------------------------
INCLUDE FILES
-----------------------------------------------------------------------------*/

/* none, current file should only be used as include from drxk.c context */
#include "./include/drx_driver.h"
#include "./include/drxk.h"
#include "./include/drxk_map.h"

/*-----------------------------------------------------------------------------
DEFINES
-----------------------------------------------------------------------------*/

#ifndef TUNER_ALL
/* By default compiling preset code for all known/tested tuners;
   to optimize code, edit line below to define only tuner(s) of your choice,
   e.g. TUNER_THOMSON_DTT_71306_BALANCED */
//#define TUNER_ALL
#endif

/**
* \brief Maximum u16_t value.
*/
#ifndef MAX_U16
#define MAX_U16  ((u16_t) (0xFFFF))
#endif

/*-----------------------------------------------------------------------------
INCLUDE FILES
-----------------------------------------------------------------------------*/
/* board specific channel filters */

/* list all required filters here with appropriate precompiler switches */
#define DRXK_TAPS_DK_CN       1
#define DRXK_TAPS_BG_HS       1
#define DRXK_TAPS_DKLLP_HS    1
#define DRXK_TAPS_I_HS        1
#define DRXK_TAPS_NTSC_HS     1

#if ( defined (TUNER_ALL) || defined (TUNER_SSEM_DTOS403SH081A) )

#define DRXK_TAPS_SSK_BG      1
#define DRXK_TAPS_SSK_DKIL    1
#define DRXK_TAPS_SSK_LP      1
#define DRXK_TAPS_SSK_NTSC    1

#endif

#if ( defined (TUNER_ALL) || defined (TUNER_SONY_SUTRA111) )

#define DRXK_TAPS_SSR111_BG   1
#define DRXK_TAPS_SSR111_DKI  1
#define DRXK_TAPS_SSR111_LLP  1

#endif

#include "./include/drxk_filters.h"

/*-----------------------------------------------------------------------------
FUNCTIONS
-----------------------------------------------------------------------------*/
/**
* \brief  Local (adapted) implementation of strcmp.
* \return 0 if strings are equal, -1 if strings are not equal.
*/

static int strComp( char *s1, char *s2 )
{
   int i1=0, i2=0;
   char c1, c2;

   if ( ( s1 == NULL ) || ( s2 == NULL ) )
   {
      /* at least one NULL pointer, strings are not equal */
      return -1;
   }
   /* step through strings until (mis)match is found; */
   /* compares alphanumerical characters only, case-independent */
   while ( 1 )
   {
      c1 = s1[i1];
      c2 = s2[i2];

      /* convert lower case to upper case */
      if ( ( c1 >= 'a' ) && ( c1 <= 'z' ) ) { c1 += 'A'-'a'; }
      if ( ( c2 >= 'a' ) && ( c2 <= 'z' ) ) { c2 += 'A'-'a'; }

      if (  ( c1 != 0 ) &&
            ( ( c1 < '0' ) || ( c1 > '9' ) ) &&
            ( ( c1 < 'A' ) || ( c1 > 'Z' ) )    )
      {
         /* skip non-alphanumerical characters in s1 */
         i1++;
         continue;
      }
      if (  ( c2 != 0 ) &&
            ( ( c2 < '0' ) || ( c2 > '9' ) ) &&
            ( ( c2 < 'A' ) || ( c2 > 'Z' ) )    )
      {
         /* skip non-alphanumerical characters in s2 */
         i2++;
         continue;
      }
      if ( c1 != c2 )
      {
         /* if any character is different, strings are not equal */
         return -1;
      }
      if ( c1 == 0 ) /* hence c2 == 0, too */
      {
         /* end of both strings, so fully equal */
         break;
      }
      /* get next characters */
      i1++;
      i2++;
   }
   return 0;
}

/**
* \brief  Apply alignment offset to the RF AGC TOP value.
* \return Nothing
*
*/

static void DRXK_PresetApplyAlignment( pDRXKPresets_t pre )
{
   s32_t rfAgcTop = 0;

   rfAgcTop =  (s32_t) pre->rfAgcCfg.top;
   rfAgcTop += (s32_t) pre->rfAgcTopOffset;

   /* clipping */
   if (rfAgcTop < 0)
   {
      rfAgcTop = 0;
   }
   if (rfAgcTop > (s32_t) MAX_U16)
   {
      rfAgcTop = MAX_U16;
   }

   pre->rfAgcCfg.top = (u16_t) rfAgcTop;
}

/**
* \brief  Decide tuner/board/standard/channel/... specific demod settings.
*  No settings are changed in HW nor in driver attributes by this function.
*  Settings are decided for the levels _up_to_and_including_ requested level.
*
* \return Nothing
*
*/

void DRXK_Preset(   pDRXDemodInstance_t demod,
                           DRXKPresetLevel_t   level,
                           pDRXKPresets_t      pre,
                           pDRXChannel_t       channel )
{
   DRXStandard_t    std        = DRX_STANDARD_UNKNOWN;
   DRXSubstandard_t substd     = DRX_SUBSTANDARD_UNKNOWN;
   pDRXKData_t      extAttr    = (pDRXKData_t) NULL;
   char            *presetName = "";

   if ( demod == NULL )
   {
      /* no demod, nothing to do */
      return;
   }
   extAttr = (pDRXKData_t) demod->myExtAttr;
   if ( extAttr == NULL )
   {
      /* no attributes, nothing to do */
      return;
   }
   if ( pre == NULL )
   {
      /* no target presets structure, nothing to do */
      return;
   }

   if ( level <= DRXK_PRESET_LEVEL_BASELINE )
   {
      /* baseline level clears members of the 'pre' structure */
      pre->rfAgcTopOffset = 0;
      return;
   }

   /* get standard and substandard;
      but it can only be used if level >= DRXK_PRESET_LEVEL_STANDARD */
   std    = extAttr->standard;
   substd = extAttr->substandard;

   /* default settings for all tuners, standards, channels */
   if ( level >= DRXK_PRESET_LEVEL_OPEN )
   {
      pre->subMode                 = 0;
      pre->tunerIfAgcPol           = FALSE; /* Normal IF-AGC polarity */
      pre->tunerPortNr             = 1;
      pre->tunerRfGain             = 50;
      pre->tunerIfGain             = 40;
      pre->tunerMirrors            = FALSE;

      pre->rfAgcCfg.ctrlMode       = DRXK_AGC_CTRL_AUTO;
      pre->rfAgcCfg.outputLevel    =     0;
      pre->rfAgcCfg.minOutputLevel =  6023;
      pre->rfAgcCfg.maxOutputLevel = 27000;
      pre->rfAgcCfg.speed          =     3;
      pre->rfAgcCfg.cutOffCurrent  =  4000;
                                 /*   4000 = ~3.5V with default configuration */
                                 /*   3000 = ~4V with default configuration   */

      pre->rfAgcSwitch             = TRUE;

      pre->preSawCfg.reference     = 4;
      pre->preSawCfg.usePreSaw     = TRUE;

      pre->ifAgcCfg.ctrlMode       = DRXK_AGC_CTRL_AUTO;
      pre->ifAgcCfg.outputLevel    =     0;
      pre->ifAgcCfg.minOutputLevel =     0;
      pre->ifAgcCfg.maxOutputLevel =  9000;
      pre->ifAgcCfg.speed          =     3;
      pre->ifAgcCfg.cutOffCurrent  =     0; /* Not used */

      pre->agcFastClipCtrlDelay    = 50;

      pre->useTgtCarrierIf         = TRUE;
      pre->tgtCarrierIf            = (DRXFrequency_t) 38900;

      pre->atvTopEqu[0]            = DRX_S9TOS16( ATV_TOP_EQU0_EQU_C0_BG );
      pre->atvTopEqu[1]            = DRX_S9TOS16( ATV_TOP_EQU1_EQU_C1_BG );
      pre->atvTopEqu[2]            = DRX_S9TOS16( ATV_TOP_EQU2_EQU_C2_BG );
      pre->atvTopEqu[3]            = DRX_S9TOS16( ATV_TOP_EQU3_EQU_C3_BG );

      pre->atvCvbsRefLvl           = SCU_RAM_ATV_AMS_MAX_REF_AMS_MAX_REF_BG_MN;
      pre->atvNAgcKiMin            = SCU_RAM_ATV_NAGC_KI_MIN__PRE;

      pre->peakFilter              = ATV_TOP_VID_PEAK__PRE /* 1 */;

      pre->qamBWBound6MHz          = DRXK_QAM_BANDWIDTH_BOUNDARY_6MHZ;
      pre->qamBWBound7MHz          = DRXK_QAM_BANDWIDTH_BOUNDARY_7MHZ;
      pre->atvTopNoiseTh           = 0; /* By default disable n.r. */


      pre->dvbtEchoThres2k          = 100;
      pre->dvbtEchoThres8k          = 40;
      pre->dvbtFrEnable             = TRUE;
      pre->dvbtImpNoiseCrunchEnable = FALSE;
      pre->agcIngainTgtMax          = 9000;
      pre->hasChannelPresets        = FALSE;

      /* channel filter (default load from ROM)*/
      pre->channelFilter.dataRe     = (pu8_t)NULL;
      pre->channelFilter.dataIm     = (pu8_t)NULL;
      pre->channelFilter.sizeRe     = 0;
      pre->channelFilter.sizeIm     = 0;
      pre->cfScale                  = IQM_CF_SCALE__PRE;

      /* audio presets */
      pre->prescale.fmDeviation     = 100;
      pre->prescale.nicamGain       = 90;

      pre->lockRangeMin             = -1000; /* kHz */
      pre->lockRangeMax             = 700;   /* kHz */

   }

   /* settings for all tuners, but now standard-dependent */
   if ( level >= DRXK_PRESET_LEVEL_STANDARD )
   {
      if ( DRXK_ISATVSTD(  std ) )
      {
         pre->rfAgcCfg.standard       = DRX_STANDARD_NTSC;
         pre->rfAgcCfg.top            = 0x2400;

         pre->preSawCfg.standard      = DRX_STANDARD_NTSC;

         pre->ifAgcCfg.standard       = DRX_STANDARD_NTSC;
         pre->ifAgcCfg.top            = 0x0960;

         pre->agcIngainTgtMax         = 9000;

      }
      if ( std == DRX_STANDARD_PAL_SECAM_BG )
      {
         if ( substd == DRX_SUBSTANDARD_ATV_BG_SCANDINAVIA )
         {
            pre->atvTopEqu[0]         = DRX_S9TOS16( -36 );
            pre->atvTopEqu[1]         = DRX_S9TOS16( 130 );
            pre->atvTopEqu[2]         = DRX_S9TOS16( 164 );
            pre->atvTopEqu[3]         = DRX_S9TOS16( 187 );
         }

         pre->atvCvbsRefLvl = SCU_RAM_ATV_AMS_MAX_REF_AMS_MAX_REF_BG_MN;
         pre->atvCrAmpThres = ATV_TOP_CR_AMP_TH_BG;
         pre->atvCrOvmThres = ATV_TOP_CR_OVM_TH_BG;

         pre->channelFilter.dataRe = (pu8_t) DRXK_TAPS_BG_HS_RE;
         pre->channelFilter.dataIm = (pu8_t) DRXK_TAPS_BG_HS_IM;
         pre->channelFilter.sizeRe = (u16_t) sizeof (DRXK_TAPS_BG_HS_RE);
         pre->channelFilter.sizeIm = (u16_t) sizeof (DRXK_TAPS_BG_HS_IM);
         pre->cfScale              =  DRXK_TAPS_BG_HS_CFSCALE;

         pre->prescale.fmDeviation     = 100;
         pre->prescale.nicamGain       = 90;


      }
      if ( std == DRX_STANDARD_PAL_SECAM_DK )
      {
         if (  ( substd == DRX_SUBSTANDARD_ATV_DK_CHINA  ) ||
               ( substd == DRX_SUBSTANDARD_ATV_DK_POLAND )    )
         {
            pre->atvTopEqu[0]         = DRX_S9TOS16( 0 );
            pre->atvTopEqu[1]         = DRX_S9TOS16( 0 );
            pre->atvTopEqu[2]         = DRX_S9TOS16( 0 );
            pre->atvTopEqu[3]         = DRX_S9TOS16( 0 );
         }
         else
         {
            pre->atvTopEqu[0]         = DRX_S9TOS16( ATV_TOP_EQU0_EQU_C0_DK );
            pre->atvTopEqu[1]         = DRX_S9TOS16( ATV_TOP_EQU1_EQU_C1_DK );
            pre->atvTopEqu[2]         = DRX_S9TOS16( ATV_TOP_EQU2_EQU_C2_DK );
            pre->atvTopEqu[3]         = DRX_S9TOS16( ATV_TOP_EQU3_EQU_C3_DK );
         }

         pre->atvCvbsRefLvl = SCU_RAM_ATV_AMS_MAX_REF_AMS_MAX_REF_DK;
         pre->atvCrAmpThres = ATV_TOP_CR_AMP_TH_DK;
         pre->atvCrOvmThres = ATV_TOP_CR_OVM_TH_DK;

         pre->channelFilter.dataRe = (pu8_t) DRXK_TAPS_DKLLP_HS_RE;
         pre->channelFilter.dataIm = (pu8_t) DRXK_TAPS_DKLLP_HS_IM;
         pre->channelFilter.sizeRe = (u16_t) sizeof (DRXK_TAPS_DKLLP_HS_RE);
         pre->channelFilter.sizeIm = (u16_t) sizeof (DRXK_TAPS_DKLLP_HS_IM);
         pre->cfScale              =  DRXK_TAPS_DKLLP_HS_CFSCALE;

         if ( substd == DRX_SUBSTANDARD_ATV_DK_CHINA )
         {
            pre->channelFilter.dataRe = (pu8_t) DRXK_TAPS_DK_CN_RE;
            pre->channelFilter.dataIm = (pu8_t) DRXK_TAPS_DK_CN_IM;
            pre->channelFilter.sizeRe = (u16_t) sizeof (DRXK_TAPS_DK_CN_RE);
            pre->channelFilter.sizeIm = (u16_t) sizeof (DRXK_TAPS_DK_CN_IM);

            pre->cfScale = IQM_CF_SCALE__PRE;
         }

         pre->prescale.fmDeviation     = 100;
         pre->prescale.nicamGain       = 90;
      }
      if ( std == DRX_STANDARD_PAL_SECAM_I )
      {
         pre->ifAgcCfg.top            = 0x0AA0;

         pre->atvTopEqu[0]            = DRX_S9TOS16( ATV_TOP_EQU0_EQU_C0_I );
         pre->atvTopEqu[1]            = DRX_S9TOS16( ATV_TOP_EQU1_EQU_C1_I );
         pre->atvTopEqu[2]            = DRX_S9TOS16( ATV_TOP_EQU2_EQU_C2_I );
         pre->atvTopEqu[3]            = DRX_S9TOS16( ATV_TOP_EQU3_EQU_C3_I );

         pre->atvCvbsRefLvl = SCU_RAM_ATV_AMS_MAX_REF_AMS_MAX_REF_I;
         pre->atvCrAmpThres = ATV_TOP_CR_AMP_TH_I;
         pre->atvCrOvmThres = ATV_TOP_CR_OVM_TH_I;

         pre->channelFilter.dataRe = (pu8_t) DRXK_TAPS_I_HS_RE;
         pre->channelFilter.dataIm = (pu8_t) DRXK_TAPS_I_HS_IM;
         pre->channelFilter.sizeRe = (u16_t) sizeof (DRXK_TAPS_I_HS_RE);
         pre->channelFilter.sizeIm = (u16_t) sizeof (DRXK_TAPS_I_HS_IM);
         pre->cfScale              =  DRXK_TAPS_I_HS_CFSCALE;

         pre->prescale.fmDeviation     = 100;
         pre->prescale.nicamGain       = 90;

      }
      if ( std == DRX_STANDARD_PAL_SECAM_L )
      {
         pre->ifAgcCfg.top            = 0x0B00;

         pre->atvTopEqu[0]            = DRX_S9TOS16( ATV_TOP_EQU0_EQU_C0_L );
         pre->atvTopEqu[1]            = DRX_S9TOS16( ATV_TOP_EQU1_EQU_C1_L );
         pre->atvTopEqu[2]            = DRX_S9TOS16( ATV_TOP_EQU2_EQU_C2_L );
         pre->atvTopEqu[3]            = DRX_S9TOS16( ATV_TOP_EQU3_EQU_C3_L );

         pre->atvCvbsRefLvl = SCU_RAM_ATV_AMS_MAX_REF_AMS_MAX_REF_LLP;
         pre->atvCrAmpThres = ATV_TOP_CR_AMP_TH_L;
         pre->atvCrOvmThres = ATV_TOP_CR_OVM_TH_L;

         pre->channelFilter.dataRe = (pu8_t) DRXK_TAPS_DKLLP_HS_RE;
         pre->channelFilter.dataIm = (pu8_t) DRXK_TAPS_DKLLP_HS_IM;
         pre->channelFilter.sizeRe = (u16_t) sizeof (DRXK_TAPS_DKLLP_HS_RE);
         pre->channelFilter.sizeIm = (u16_t) sizeof (DRXK_TAPS_DKLLP_HS_IM);
         pre->cfScale              =  DRXK_TAPS_DKLLP_HS_CFSCALE;

         pre->prescale.fmDeviation     = 29;
         pre->prescale.nicamGain       = 90;

         pre->lockRangeMin             = -700;     /* kHz */
         pre->lockRangeMax             = 1000;     /* kHz */

      }
      if ( std == DRX_STANDARD_PAL_SECAM_LP )
      {
         pre->ifAgcCfg.top            = 0x0B00;

         pre->atvTopEqu[0]            = DRX_S9TOS16(    9 );
         pre->atvTopEqu[1]            = DRX_S9TOS16(    0 );
         pre->atvTopEqu[2]            = DRX_S9TOS16(   40 );
         pre->atvTopEqu[3]            = DRX_S9TOS16( -110 );

         pre->tgtCarrierIf            = (DRXFrequency_t) 33400;

         pre->atvCvbsRefLvl = SCU_RAM_ATV_AMS_MAX_REF_AMS_MAX_REF_LLP;
         pre->atvCrAmpThres = ATV_TOP_CR_AMP_TH_LP;
         pre->atvCrOvmThres = ATV_TOP_CR_OVM_TH_LP;

         pre->channelFilter.dataRe = (pu8_t) DRXK_TAPS_DKLLP_HS_RE;
         pre->channelFilter.dataIm = (pu8_t) DRXK_TAPS_DKLLP_HS_IM;
         pre->channelFilter.sizeRe = (u16_t) sizeof (DRXK_TAPS_DKLLP_HS_RE);
         pre->channelFilter.sizeIm = (u16_t) sizeof (DRXK_TAPS_DKLLP_HS_IM);
         pre->cfScale              =  DRXK_TAPS_DKLLP_HS_CFSCALE;

         pre->prescale.fmDeviation     = 29;
         pre->prescale.nicamGain       = 90;

      }
      if ( std == DRX_STANDARD_NTSC )
      {
         pre->ifAgcCfg.top            = 0x0C1C;

         pre->atvTopEqu[0]            = DRX_S9TOS16(   -5 );
         pre->atvTopEqu[1]            = DRX_S9TOS16(  -50 );
         pre->atvTopEqu[2]            = DRX_S9TOS16(  210 );
         pre->atvTopEqu[3]            = DRX_S9TOS16( -160 );

         pre->ifAgcCfg.top            = 3100;

         pre->atvCvbsRefLvl = SCU_RAM_ATV_AMS_MAX_REF_AMS_MAX_REF_BG_MN;
         pre->atvCrAmpThres = ATV_TOP_CR_AMP_TH_MN;
         pre->atvCrOvmThres = ATV_TOP_CR_OVM_TH_MN;

         pre->channelFilter.dataRe = (pu8_t) DRXK_TAPS_NTSC_HS_RE;
         pre->channelFilter.dataIm = (pu8_t) DRXK_TAPS_NTSC_HS_IM;
         pre->channelFilter.sizeRe = (u16_t) sizeof (DRXK_TAPS_NTSC_HS_RE);
         pre->channelFilter.sizeIm = (u16_t) sizeof (DRXK_TAPS_NTSC_HS_IM);
         pre->cfScale              =  DRXK_TAPS_NTSC_HS_CFSCALE;

         pre->prescale.fmDeviation     = 75;
         pre->prescale.nicamGain       = 90;

      }
      if ( std == DRX_STANDARD_FM )
      {
         pre->atvTopEqu[0]            = DRX_S9TOS16( ATV_TOP_EQU0_EQU_C0_FM );
         pre->atvTopEqu[1]            = DRX_S9TOS16( ATV_TOP_EQU1_EQU_C1_FM );
         pre->atvTopEqu[2]            = DRX_S9TOS16( ATV_TOP_EQU2_EQU_C2_FM );
         pre->atvTopEqu[3]            = DRX_S9TOS16( ATV_TOP_EQU3_EQU_C3_FM );

         pre->atvCrAmpThres = ATV_TOP_CR_AMP_TH_FM;
         pre->atvCrOvmThres = ATV_TOP_CR_OVM_TH_FM;

         pre->prescale.fmDeviation     = 50;
         pre->prescale.nicamGain       = 90;
      }
      if ( DRXK_ISDVBTSTD(  std ) )
      {
         pre->rfAgcCfg.standard       = DRX_STANDARD_DVBT;
         pre->rfAgcCfg.top            = 0x2100;

         pre->preSawCfg.standard      = DRX_STANDARD_DVBT;

         pre->ifAgcCfg.standard       = DRX_STANDARD_DVBT;
         pre->ifAgcCfg.top            = 13424;
         pre->agcIngainTgtMax         = 30000;
         pre->agcFastClipCtrlDelay    = 30;
      }
      if ( DRXK_ISQAMSTD(  std ) )
      {
         pre->rfAgcCfg.standard       = DRX_STANDARD_ITU_A;
         pre->rfAgcCfg.top            = 0x2380;

         pre->preSawCfg.standard      = DRX_STANDARD_ITU_A;
         pre->ifAgcCfg.standard       = DRX_STANDARD_ITU_A;
         pre->ifAgcCfg.top            = 0x0511;

         pre->agcIngainTgtMax         = 5119;
      }
   }

/* Section below changes presets primarily based on tuner
   and partly still based on standard, channel, ... */

   presetName = extAttr->presetName;
   if ( presetName == NULL )
   {
      /* no tuner-specific preset available, nothing else to do */
      /* do apply RF AGC top alignment */
      DRXK_PresetApplyAlignment( pre );
      return;
   }

#ifdef  TUNER_ALL
#ifndef TUNER_DUMMY_TUNER
#define TUNER_DUMMY_TUNER
#endif
#endif
#ifdef  TUNER_DUMMY_TUNER
   /* Dummy Tuner specific */
   if ( !strComp( presetName, "Dummy Tuner 36MHz" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->tunerPortNr = 0;
      }
   }

   if ( !strComp( presetName, "Dummy Tuner 4.56MHz" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->tunerPortNr = 0;
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
   }

   if ( !strComp( presetName, "Dummy Tuner 43.75MHz" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->tunerPortNr = 0;
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( std == DRX_STANDARD_NTSC)
         {
            pre->tgtCarrierIf         = 45500;
         }
      }
   }

   if ( !strComp( presetName, "Dummy Tuner 8MHz" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->tunerPortNr = 0;
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_ALPS_TDAGX
#define TUNER_ALPS_TDAGX
#endif
#endif
#ifdef  TUNER_ALPS_TDAGX
   /* Alps Electric TDAGX specific */
   if ( !strComp( presetName, "Alps Electric TDAGx" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->rfAgcCfg.speed       = 1;
         pre->ifAgcCfg.speed       = 1;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 12;
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->subMode      = 2; /* BG I DK L */
            pre->rfAgcCfg.top = 10305; /* ~ 52.6dBuV */

         }
         if ( std == DRX_STANDARD_PAL_SECAM_L  )
         {
            pre->subMode      = 2; /* BG I DK L */
            pre->rfAgcCfg.top = 10720; /* ~ 56.3dBuV */
         }

         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->subMode      = 3; /* L' */
            pre->rfAgcCfg.top = 10720; /* ~ 56.3dBuV */
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode      = 0; /* DVB-T */
            pre->rfAgcCfg.top = 10495; /* ~ -60.8dBm */
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode      = 1; /* DVB-C */
            pre->rfAgcCfg.top = 10395; /* ~ -58.7dBm */
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_ALPS_TDAG4_NIM
#define TUNER_ALPS_TDAG4_NIM
#endif
#endif
#ifdef  TUNER_ALPS_TDAG4_NIM
   /* Alps Electric TDAG4 NIM specific */
   if ( !strComp( presetName, "Alps Electric TDAG4 NIM" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
         pre->ifAgcCfg.speed       = 1;
         pre->qamBWBound7MHz       = 6399750;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD(  std ) ) pre->subMode = 2;
         if ( DRXK_ISQAMSTD(  std ) ) pre->subMode = 1;
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_ALPS_TDQG4X003A_K
#define TUNER_ALPS_TDQG4X003A_K
#endif
#endif
#ifdef  TUNER_ALPS_TDQG4X003A_K
   /* Alps Electric TDQG4X003A specific */
   if ( !strComp( presetName, "Alps Electric TDQG4X003A" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->rfAgcCfg.speed       = 1;
         pre->ifAgcCfg.speed       = 1;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD(  std ) ) pre->rfAgcCfg.top = 0x1580;
         if ( DRXK_ISQAMSTD(  std ) ) pre->rfAgcCfg.top = 0x1620;
         if ( DRXK_ISDVBTSTD( std ) ) pre->rfAgcCfg.top = 0x1400;
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_CHINA_DRAGON_CDT_SFT338_40
#define TUNER_CHINA_DRAGON_CDT_SFT338_40
#endif
#endif
#ifdef  TUNER_CHINA_DRAGON_CDT_SFT338_40
   /* China Dragon CDT-SFT338-40 specific */
   if ( !strComp( presetName, "China Dragon CDT-SFT338-40" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->rfAgcCfg.cutOffCurrent   = 2000;
         pre->preSawCfg.reference      = 4;
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->preSawCfg.reference   = 5;
            pre->atvTopNoiseTh         = 6; /* n.r. */
            pre->peakFilter            = 2;
            pre->subMode               = 1; /* External RF-AGC, 62.5 kHz */
            pre->rfAgcCfg.top          = 12200;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_BG  )
         {
            pre->atvCvbsRefLvl   = 780;
            pre->atvTopEqu[0]    = DRX_S9TOS16(    7 );
            pre->atvTopEqu[1]    = DRX_S9TOS16(  -99 );
            pre->atvTopEqu[2]    = DRX_S9TOS16(  191 );
            pre->atvTopEqu[3]    = DRX_S9TOS16( -210 );
         }
         if ( std == DRX_STANDARD_PAL_SECAM_DK  )
         {
            pre->atvTopEqu[0]    = DRX_S9TOS16(   39 );
            pre->atvTopEqu[1]    = DRX_S9TOS16( -115 );
            pre->atvTopEqu[2]    = DRX_S9TOS16(  103 );
            pre->atvTopEqu[3]    = DRX_S9TOS16(   66 );
         }
         if ( std == DRX_STANDARD_PAL_SECAM_I  )
         {
            pre->atvTopEqu[0]    = DRX_S9TOS16(    2 );
            pre->atvTopEqu[1]    = DRX_S9TOS16(  -10 );
            pre->atvTopEqu[2]    = DRX_S9TOS16(   40 );
            pre->atvTopEqu[3]    = DRX_S9TOS16( -110 );
         }
         if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
               ( std == DRX_STANDARD_PAL_SECAM_LP ) )
         {
            pre->rfAgcCfg.top          = 12050;
            pre->prescale.fmDeviation  = 280;

         }
         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->peakFilter      = -4;
            pre->atvTopEqu[0]    = 1;
         }

         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode         = 0; /* External RF-AGC, 166.7kHz */
            pre->rfAgcCfg.top    = 10630;
            pre->rfAgcCfg.speed  = 2;
         }

         if ( DRXK_ISQAMSTD( std ) )
         {
            pre->subMode         = 1; /* External RF-AGC, 62.5 kHz */
            pre->rfAgcCfg.top    = 10555;
         }
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_EARDA_EDT3021II2A
#define TUNER_EARDA_EDT3021II2A
#endif
#endif
#ifdef  TUNER_EARDA_EDT3021II2A
   /* Earda EDT-3021II2A specific */
   if ( !strComp( presetName, "Earda EDT-3021II2A" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->preSawCfg.reference = 6;
            pre->rfAgcCfg.top        = 0x2f80 ;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->preSawCfg.reference = 6;
            pre->rfAgcCfg.top        = 0x2f80 ;
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->preSawCfg.reference = 6;
            pre->rfAgcCfg.top        = 0x3110 ;
         }
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_EARDA_EHU100X1
#define TUNER_EARDA_EHU100X1
#endif
#endif
#ifdef  TUNER_EARDA_EHU100X1
   /* Earda Earda EHU-100x1 specific */
   if ( !strComp( presetName, "Earda EHU-100x1" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->subMode               = 1;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode               = 0;
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode               = 1;
         }
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_ENTROPIC_EN4020
#define TUNER_ENTROPIC_EN4020
#endif
#endif
#ifdef  TUNER_ENTROPIC_EN4020
   /* Entropic EN4020 specific */
   if ( !strComp( presetName, "Entropic EN4020" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->tunerPortNr = 1;
         pre->rfAgcCfg.ctrlMode = DRXK_AGC_CTRL_OFF;
         pre->subMode = 0;
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_JINXIN_UVS1X05DCW1_K
#define TUNER_JINXIN_UVS1X05DCW1_K
#endif
#endif
#ifdef  TUNER_JINXIN_UVS1X05DCW1_K
   /* Jinxin UVS1x05DCW1 specific */
   if ( !strComp( presetName, "Jinxin UVS1x05DCW1" ) )
   {
      /* signal strength mapping */
      pre->tunerRfGain             = 40;
      pre->rfAgcCfg.minOutputLevel = 9000;
      pre->rfAgcCfg.maxOutputLevel = 21000;
      pre->tunerIfGain             = 40;
      pre->ifAgcCfg.minOutputLevel = 7500;
      pre->ifAgcCfg.maxOutputLevel = 12000;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference    = 6;
         pre->rfAgcCfg.cutOffCurrent = 10000;
         pre->rfAgcCfg.speed = 1;
         pre->ifAgcCfg.speed = 1;

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->rfAgcCfg.top = 11329;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->rfAgcCfg.top = 11535;
            pre->ifAgcCfg.top = 13000;
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->rfAgcCfg.top = 11329;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_LGE_SITU
#define TUNER_LGE_SITU
#endif
#endif
#ifdef  TUNER_LGE_SITU
   /* LGE Silicon Tuner specific */
   if ( !strComp( presetName, "LGE Silicon Tuner" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         /* RF-AGC is controlled by the SiTu, switch DRXK RF-AGC off */
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDFW_G235D_NIM_K
#define TUNER_LGI_TDFW_G235D_NIM_K
#endif
#endif
#ifdef  TUNER_LGI_TDFW_G235D_NIM_K
   /* LG Innotek TDFW-G235D NIM specific */
   if ( !strComp( presetName, "LG Innotek TDFW-G235D NIM" ) )
   {
      pre->dvbtEchoThres2k          = 100;
      pre->dvbtFrEnable             = TRUE;
      pre->dvbtImpNoiseCrunchEnable = FALSE;
      pre->hasChannelPresets        = TRUE;

      /* signal strength mapping */
      pre->tunerRfGain             = 55;
      pre->rfAgcCfg.minOutputLevel = 9000;
      pre->rfAgcCfg.maxOutputLevel = 25000;
      pre->tunerIfGain             = 30;
      pre->ifAgcCfg.minOutputLevel = 5000;
      pre->ifAgcCfg.maxOutputLevel = 10000;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->rfAgcCfg.cutOffCurrent = 0x0900; /* ~4.2V */
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode              = 0; /* 167kHz */
         }

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->atvTopNoiseTh      = ATV_TOP_NOISE_TH__PRE; /* 8 */
            pre->subMode            = 2; /* 50kHz */
            if (  ( std == DRX_STANDARD_PAL_SECAM_LP ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_L  ) )
            {
               pre->agcIngainTgtMax = 6000;
            }
            else
            {
               pre->atvCrAmpThres   = 12;
            }

            /* EQ settings */
            if ( std == DRX_STANDARD_PAL_SECAM_BG )
            {
               pre->atvTopEqu[0] = (s16_t)    9;
               pre->atvTopEqu[1] = (s16_t) -100;
               pre->atvTopEqu[2] = (s16_t)  207;
               pre->atvTopEqu[3] = (s16_t) -204;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_DK )
            {
               pre->atvTopEqu[0] = (s16_t)    6;
               pre->atvTopEqu[1] = (s16_t) -104;
               pre->atvTopEqu[2] = (s16_t)  176;
               pre->atvTopEqu[3] = (s16_t) -114;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_I )
            {
               pre->atvTopEqu[0] = (s16_t)   12;
               pre->atvTopEqu[1] = (s16_t)  -10;
               pre->atvTopEqu[2] = (s16_t)   40;
               pre->atvTopEqu[3] = (s16_t)  -51;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_L )
            {
               pre->atvTopEqu[0] = (s16_t)   10;
               pre->atvTopEqu[1] = (s16_t)  -10;
               pre->atvTopEqu[2] = (s16_t)   60;
               pre->atvTopEqu[3] = (s16_t)  -64;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->tgtCarrierIf = (DRXFrequency_t) 33900;
               pre->atvTopEqu[0] = (s16_t)   -8;
               pre->atvTopEqu[1] = (s16_t)    0;
               pre->atvTopEqu[2] = (s16_t)   40;
               pre->atvTopEqu[3] = (s16_t) -148;
            }
         }

         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode              = 1; /* 62.5kHz */
         }
      }

      if ( level >= DRXK_PRESET_LEVEL_CHANNEL )
      {
         if (channel != NULL)
         {
            /* QAM */
            if ( DRXK_ISQAMSTD(  std ) )
            {
               pre->ifAgcCfg.speed      = 3;
               pre->preSawCfg.reference = 3;
               pre->rfAgcCfg.top        = 0x2400;
            }


            /* ATV */
            if ( DRXK_ISATVSTD(  std ) )
            {
               pre->ifAgcCfg.speed = 3;

               /* non Secam L' settings */
               if ( channel->frequency < 147000 ) /* VHF-L */
               {
                  pre->peakFilter = -3;
               }
               else
               {
                  pre->peakFilter = 1;
               }

               if ( channel->frequency < 431000 ) /* VHF */
               {
                  pre->preSawCfg.reference = 4;
                  pre->rfAgcCfg.top        = 0x26AC; /* dec. 9900 */
               }
               else
               {
                  pre->preSawCfg.reference = 3;
                  pre->rfAgcCfg.top        = 0x2580; /* dec. 9600 */
               }

               /* Secam L' settings */
               if ( std == DRX_STANDARD_PAL_SECAM_LP )
               {
                  pre->preSawCfg.reference = 4;
                  pre->rfAgcCfg.top        = 0x26AC; /* dec. 9900 */
                  pre->peakFilter          = -6;
               }
            }

            /* DVBT */
            if ( DRXK_ISDVBTSTD( std ) )
            {
               pre->ifAgcCfg.speed = 2;
               if ( channel->frequency < 431000 ) /* VHF */
               {
                  pre->preSawCfg.reference = 4;
                  pre->rfAgcCfg.top        = 0x2640;
               }
               else
               {
                  pre->preSawCfg.reference = 3;
                  pre->rfAgcCfg.top        = 0x2500;
               }
            }
         } /* if (channel != NULL) */
      }
   }
#endif /* TUNER_LGI_TDFW_G235D_NIM_K */

#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDFW_G235D_NIM2_K
#define TUNER_LGI_TDFW_G235D_NIM2_K
#endif
#endif
#ifdef  TUNER_LGI_TDFW_G235D_NIM2_K
   /* LG Innotek TDFW-G235D NIM2 specific */
   if ( !strComp( presetName, "LG Innotek TDFW-G235D NIM2" ) )
   {
      pre->dvbtEchoThres2k          = 100;
      pre->dvbtFrEnable             = TRUE;
      pre->dvbtImpNoiseCrunchEnable = FALSE;
      pre->hasChannelPresets        = TRUE;

      /* signal strength mapping */
      pre->tunerRfGain             = 55;
      pre->rfAgcCfg.minOutputLevel = 9000;
      pre->rfAgcCfg.maxOutputLevel = 25000;
      pre->tunerIfGain             = 30;
      pre->ifAgcCfg.minOutputLevel = 5000;
      pre->ifAgcCfg.maxOutputLevel = 10000;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->rfAgcCfg.cutOffCurrent = 0x0900; /* ~4.2V */
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode              = 0; /* 167kHz */
         }

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->atvTopNoiseTh      = ATV_TOP_NOISE_TH__PRE; /* 8 */
            pre->subMode            = 2; /* 50kHz */
            if (  ( std == DRX_STANDARD_PAL_SECAM_LP ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_L  ) )
            {
               pre->agcIngainTgtMax = 6000;
            }
            else
            {
               pre->atvCrAmpThres   = 12;
            }


            /* EQ settings */
            if ( std == DRX_STANDARD_PAL_SECAM_BG )
            {
               pre->atvTopEqu[0] = (s16_t)    9;
               pre->atvTopEqu[1] = (s16_t) -100;
               pre->atvTopEqu[2] = (s16_t)  207;
               pre->atvTopEqu[3] = (s16_t) -204;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_DK )
            {
               pre->atvTopEqu[0] = (s16_t)    6;
               pre->atvTopEqu[1] = (s16_t) -104;
               pre->atvTopEqu[2] = (s16_t)  176;
               pre->atvTopEqu[3] = (s16_t) -114;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_I )
            {
               pre->atvTopEqu[0] = (s16_t)   12;
               pre->atvTopEqu[1] = (s16_t)  -10;
               pre->atvTopEqu[2] = (s16_t)   40;
               pre->atvTopEqu[3] = (s16_t)  -51;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_L )
            {
               pre->atvTopEqu[0] = (s16_t)   10;
               pre->atvTopEqu[1] = (s16_t)  -10;
               pre->atvTopEqu[2] = (s16_t)   60;
               pre->atvTopEqu[3] = (s16_t)  -64;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->tgtCarrierIf = (DRXFrequency_t) 33900;
               pre->atvTopEqu[0] = (s16_t)   -8;
               pre->atvTopEqu[1] = (s16_t)    0;
               pre->atvTopEqu[2] = (s16_t)   40;
               pre->atvTopEqu[3] = (s16_t) -148;
            }
         }

         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode              = 1; /* 62.5kHz */
         }
      }

      if ( level >= DRXK_PRESET_LEVEL_CHANNEL )
      {
         if (channel != NULL)
         {
            /* QAM */
            if ( DRXK_ISQAMSTD(  std ) )
            {
               pre->ifAgcCfg.speed      = 3;
               pre->preSawCfg.reference = 3;
               pre->rfAgcCfg.top        = 0x2400;
            }


            /* ATV */
            if ( DRXK_ISATVSTD(  std ) )
            {
               pre->ifAgcCfg.speed = 3;

               /* non Secam L' settings */

               if (  ( channel->frequency > 112250) &&
                     ( channel->frequency > 140250)  )
               {
                  pre->peakFilter = -1;
               }
               else if ( channel->frequency < 147000 ) /* VHF-L */
               {
                  pre->peakFilter = -4;
               }
               else
               {
                  pre->peakFilter = -2;
               }

               if ( channel->frequency < 431000 ) /* VHF */
               {
                  pre->preSawCfg.reference = 5;
                  pre->rfAgcCfg.top        = 0x28A0; /* dec. 10400 */
               }
               else /* UHF */
               {
                  pre->preSawCfg.reference = 3;
                  pre->rfAgcCfg.top        = 0x2710; /* dec. 10000 */
               }

               /* Secam L' settings */
               if ( std == DRX_STANDARD_PAL_SECAM_LP )
               {
                  pre->preSawCfg.reference = 5;
                  pre->rfAgcCfg.top        = 0x28A0; /* dec. 10400 */
                  pre->peakFilter          = -6;
               }
            }

            /* DVBT */
            if ( DRXK_ISDVBTSTD( std ) )
            {
               pre->ifAgcCfg.speed = 2;
               if ( channel->frequency < 431000 ) /* VHF */
               {
                  pre->preSawCfg.reference = 4;
                  pre->rfAgcCfg.top        = 0x2640;
               }
               else
               {
                  pre->preSawCfg.reference = 3;
                  pre->rfAgcCfg.top        = 0x2500;
               }
            }
         } /* if (channel != NULL) */
      }
   }
#endif /* TUNER_LGI_TDFW_G235D_NIM2_K */

#ifdef  TUNER_ALL
#ifndef TUNER_MIC_LGI_TDFW_G235D_NIM_K
#define TUNER_MIC_LGI_TDFW_G235D_NIM_K
#endif
#endif
#ifdef  TUNER_MIC_LGI_TDFW_G235D_NIM_K
   /* LG Innotek TDFW-G235D NIM specific, Trident settings */
   if ( !strComp( presetName, "Mic LG Innotek TDFW-G235D NIM" ) )
   {
      pre->dvbtEchoThres2k          = 100;
      pre->dvbtFrEnable             = TRUE;
      pre->dvbtImpNoiseCrunchEnable = FALSE;
      pre->hasChannelPresets        = TRUE;

      /* signal strength mapping */
      pre->tunerRfGain             = 55;
      pre->rfAgcCfg.minOutputLevel = 9000;
      pre->rfAgcCfg.maxOutputLevel = 25000;
      pre->tunerIfGain             = 30;
      pre->ifAgcCfg.minOutputLevel = 5000;
      pre->ifAgcCfg.maxOutputLevel = 10000;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->rfAgcCfg.cutOffCurrent = 0x0900; /* ~4.2V */
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode              = 0; /* 167kHz */
         }

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->subMode            = 2; /* 50kHz */
            if (  ( std == DRX_STANDARD_PAL_SECAM_LP ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_L  ) )
            {
               pre->agcIngainTgtMax = 6000;
            }
            else
            {
               pre->atvCrAmpThres   = 12;
            }


            /* EQ settings */
            if ( std == DRX_STANDARD_PAL_SECAM_BG )
            {
               pre->atvTopEqu[0] = (s16_t)    6;
               pre->atvTopEqu[1] = (s16_t) -100;
               pre->atvTopEqu[2] = (s16_t)  207;
               pre->atvTopEqu[3] = (s16_t) -204;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_DK )
            {
               pre->atvTopEqu[0] = (s16_t)    3;
               pre->atvTopEqu[1] = (s16_t) -104;
               pre->atvTopEqu[2] = (s16_t)  176;
               pre->atvTopEqu[3] = (s16_t) -114;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_I )
            {
               pre->atvTopEqu[0] = (s16_t)   9;
               pre->atvTopEqu[1] = (s16_t)  -10;
               pre->atvTopEqu[2] = (s16_t)   40;
               pre->atvTopEqu[3] = (s16_t)  -51;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_L )
            {
               pre->atvTopEqu[0] = (s16_t)    7;
               pre->atvTopEqu[1] = (s16_t)  -10;
               pre->atvTopEqu[2] = (s16_t)   60;
               pre->atvTopEqu[3] = (s16_t)  -64;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->tgtCarrierIf = (DRXFrequency_t) 33400;
               pre->atvTopEqu[0] = (s16_t)   -3;
               pre->atvTopEqu[1] = (s16_t)    0;
               pre->atvTopEqu[2] = (s16_t)   40;
               pre->atvTopEqu[3] = (s16_t) -148;
            }
         }

         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode              = 1; /* 62.5kHz */
         }
      }

      if ( level >= DRXK_PRESET_LEVEL_CHANNEL )
      {
         if (channel != NULL)
         {
            /* QAM */
            if ( DRXK_ISQAMSTD(  std ) )
            {
               pre->ifAgcCfg.speed      = 3;
               pre->preSawCfg.reference = 3;
               pre->rfAgcCfg.top        = 0x2400;
            }


            /* ATV */
            if ( DRXK_ISATVSTD(  std ) )
            {
               pre->ifAgcCfg.speed = 3;

               /* non Secam L' settings */
               if ( channel->frequency < 147000 ) /* VHF-L */
               {
                  pre->peakFilter = -3;
               }
               else
               {
                  pre->peakFilter = 1;
               }

               if ( channel->frequency < 431000 ) /* VHF */
               {
                  pre->preSawCfg.reference = 4;
                  pre->rfAgcCfg.top        = 0x26AC; /* dec. 9900 */
               }
               else
               {
                  pre->preSawCfg.reference = 3;
                  pre->rfAgcCfg.top        = 0x2580; /* dec. 9600 */
               }

               /* Secam L' settings */
               if ( std == DRX_STANDARD_PAL_SECAM_LP )
               {
                  pre->preSawCfg.reference = 4;
                  pre->rfAgcCfg.top        = 0x26AC; /* dec. 9900 */
                  pre->peakFilter          = -6;
               }
            }

            /* DVBT */
            if ( DRXK_ISDVBTSTD( std ) )
            {
               pre->ifAgcCfg.speed = 2;
               if ( channel->frequency < 431000 ) /* VHF */
               {
                  pre->preSawCfg.reference = 4;
                  pre->rfAgcCfg.top        = 0x2640;
               }
               else
               {
                  pre->preSawCfg.reference = 3;
                  pre->rfAgcCfg.top        = 0x2500;
               }
            }
         } /* if (channel != NULL) */
      }
   }
#endif /* TUNER_MIC_LGI_TDFW_G235D_NIM_K */

#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDTC_G2XXD_K
#define TUNER_LGI_TDTC_G2XXD_K
#endif
#endif
#ifdef  TUNER_LGI_TDTC_G2XXD_K
   /* LG Innotek TDTC-G2xxD specific */
   if ( !strComp( presetName, "LG Innotek TDTC-G2xxD" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode        = 0; /* 167kHz */
            pre->rfAgcCfg.top   = 0x2700;
         }
         else
         {
            pre->subMode        = 1; /* 62.5kHz */
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDTC_G323D
#define TUNER_LGI_TDTC_G323D
#endif
#endif
#ifdef  TUNER_LGI_TDTC_G323D
   /* LG Innotek TDTC-G323D specific */
   if ( !strComp( presetName, "LG Innotek TDTC-G323D" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 4;
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode        = 0; /* 167kHz */
            pre->rfAgcCfg.top   = 9950;

         }
         if ( DRXK_ISQAMSTD ( std ) )
         {
            pre->subMode        = 1; /* 62.5kHz */
            pre->rfAgcCfg.top   = 9870;

         }
         if ( DRXK_ISATVSTD ( std ) )
         {
            pre->subMode        = 1; /* 62.5kHz */

            /* AGC settings */
            pre->rfAgcCfg.top   = 9800;
            if (  ( std == DRX_STANDARD_PAL_SECAM_L ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_L ) )
            {
               pre->rfAgcCfg.top   = 9880;
            }

            /* EQ settings */
            if ( std == DRX_STANDARD_PAL_SECAM_BG )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16( 4 );
            }

            if ( std == DRX_STANDARD_PAL_SECAM_DK )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16( 2 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -113 );
            }

            if ( std == DRX_STANDARD_PAL_SECAM_I )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16( -2 );
            }

            if ( std == DRX_STANDARD_PAL_SECAM_L )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16( 0 );
            }

            if ( std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16( -5 );
            }

         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDTC_G3XXD_K
#define TUNER_LGI_TDTC_G3XXD_K
#endif
#endif
#ifdef  TUNER_LGI_TDTC_G3XXD_K
   /* LG Innotek TDTC-G3xxD specific */
   if ( !strComp( presetName, "LG Innotek TDTC-G3xxD" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 5;
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode        = 0; /* 167kHz */
            pre->rfAgcCfg.top   = 8400;

         }
         if ( DRXK_ISQAMSTD ( std ) )
         {
            pre->subMode        = 1; /* 62.5kHz */
            pre->rfAgcCfg.top   = 8300;

         }
         if ( DRXK_ISATVSTD ( std ) )
         {
            pre->subMode        = 1; /* 62.5kHz */
            pre->rfAgcCfg.top   = 8375;

            if (  ( std == DRX_STANDARD_PAL_SECAM_L ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_L ) )
            {
               pre->rfAgcCfg.top   = 8925;
            }

            if ( std == DRX_STANDARD_PAL_SECAM_DK )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16(   -1 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -113 );
            }

            if ( std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16( -9 );
            }

         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDTC_G4XXD_K
#define TUNER_LGI_TDTC_G4XXD_K
#endif
#endif
#ifdef  TUNER_LGI_TDTC_G4XXD_K
   /* LG Innotek TDTC-G4xxD specific */
   if ( !strComp( presetName, "LG Innotek TDTC-G4xxD" ) )
   {
      pre->hasChannelPresets      = TRUE;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 9;
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode               = 0; /* 167kHz */
            pre->rfAgcCfg.top     = 8570;
         }
         if ( DRXK_ISQAMSTD( std ) )
         {
            pre->subMode               = 1; /* 62.5kHz */
            pre->rfAgcCfg.top     = 8400;
         }
         if ( DRXK_ISATVSTD( std ) )
         {
            pre->subMode = 1; /* 62.5kHz */
            pre->atvTopNoiseTh    = 0; /* Switch off n.r. */

            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               /* positive modulation */
               pre->rfAgcCfg.top  = 8200;

            }
            else
            {
               /* negative modulation */
               pre->rfAgcCfg.top  = 8420;
            }

            if ( std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->atvTopEqu[0]  = (s16_t)-6;
               pre->atvTopEqu[1]  = (s16_t)-2;
         }
      }
   }

      if ( level >= DRXK_PRESET_LEVEL_CHANNEL )
      {
         if (channel != NULL)
         {
            if (  DRXK_ISATVSTD( std ) &&
                  ( std != DRX_STANDARD_PAL_SECAM_LP ) )
            {
               if ( channel->frequency < 431000 ) /* VHF */
               {
                  pre->peakFilter = 3;
               }
               else if ( channel->frequency < 654000 ) /* UHF < Ch44 */
               {
                  pre->peakFilter = 0;
               }
               else /* UHF >= Ch44 */
               {
                  pre->peakFilter = 2;
               }
            }
         } /* if (channel != NULL) */
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDTC_G675D_K
#define TUNER_LGI_TDTC_G675D_K
#endif
#endif
#ifdef  TUNER_LGI_TDTC_G675D_K
   /* LG Innotek TDTC-G675D specific */
   if ( !strComp( presetName, "LG Innotek TDTC-G675D" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 4;

         if ( DRXK_ISATVSTD ( std ) )
         {
            pre->subMode         = 1; /* 62.5kHz */
            pre->rfAgcCfg.top    = 9920;
            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top = 10495;
            }

         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode        = 0; /* 167kHz */
            pre->rfAgcCfg.top    = 9800;
         }
         if ( DRXK_ISQAMSTD ( std ))
         {
            pre->subMode        = 1; /* 62.5kHz */
            pre->rfAgcCfg.top    = 9695;
         }
      }
   }
#endif



#ifdef  TUNER_ALL
#ifndef TUNER_LOEWE_FET08
#define TUNER_LOEWE_FET08
#endif
#endif
#ifdef  TUNER_LOEWE_FET08
   /* Loewe FET08 specific */
   if (  ( !strComp( presetName, "Loewe FET08 tuner 1" ) ) ||
         ( !strComp( presetName, "Loewe FET08 tuner 2" ) ) )
   {
      pre->preSawCfg.reference = 8;
      pre->rfAgcCfg.cutOffCurrent = 0;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {

         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode         = 0; /* 166.7kHz */
            pre->rfAgcCfg.top    = 11200;
         }
         if ( DRXK_ISATVSTD ( std ) )
         {
            pre->subMode         = 1; /* 62.5kHz */
            pre->rfAgcCfg.top    = 11500;
            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top = 11700;
            }
         }
         if ( DRXK_ISQAMSTD ( std ))
         {
            pre->subMode         = 1; /* 62.5kHz */
            pre->rfAgcCfg.top    = 11400;
            pre->ifAgcCfg.top    = 1800;
         }
      }
   }
#endif



#ifdef  TUNER_ALL
#ifndef TUNER_MAXIM_MAX3542
#define TUNER_MAXIM_MAX3542
#endif
#endif
#ifdef  TUNER_MAXIM_MAX3542
   /* Maxim Max3542 specific */
   if ( !strComp( presetName, "Maxim Max3542" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD( std ) )
         {
            pre->subMode                = 1;  /* "IFOVLD = FALSE" */
            pre->useTgtCarrierIf        = FALSE;
            pre->rfAgcSwitch            = TRUE;
            pre->preSawCfg.reference    = 12;
            pre->rfAgcCfg.ctrlMode      = DRXK_AGC_CTRL_AUTO;
            pre->rfAgcCfg.top           = 0x3380;
            pre->rfAgcCfg.cutOffCurrent = 0;

            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.speed      = 0;
               pre->ifAgcCfg.speed      = 2;

               pre->atvTopEqu[0]    = DRX_S9TOS16(    7 );
               pre->atvTopEqu[1]    = DRX_S9TOS16(   -2 );
               /* Other EQ settings are default, so don't set here */

            }
            else
            {
               pre->rfAgcCfg.speed      = 1;
               pre->ifAgcCfg.speed      = 3;
            }
         }
         else
         {
            pre->subMode            = 0;  /* "IFOVLD = TRUE" */
            pre->rfAgcSwitch        = FALSE;
            pre->rfAgcCfg.ctrlMode  = DRXK_AGC_CTRL_OFF;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_MAXIM_MAX3543
#define TUNER_MAXIM_MAX3543
#endif
#endif
#ifdef  TUNER_MAXIM_MAX3543
   /* Maxim Max3543 specific */
   if ( !strComp( presetName, "Maxim Max3543" ) )
   {
      pre->rfAgcSwitch            = FALSE;
      pre->rfAgcCfg.ctrlMode      = DRXK_AGC_CTRL_OFF;
      pre->subMode                = 0; /* the only one */
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         /* Ugly hack to prevent changing submode */
         if ( demod->myTuner )
         {
            pre->subMode = demod->myTuner->myCommonAttr->subMode;
         }

         if ( std == DRX_STANDARD_PAL_SECAM_BG )
         {
            pre->atvTopEqu[0]  = DRX_S9TOS16(   -5 );
            pre->atvTopEqu[1]  = DRX_S9TOS16(  -80 );
            pre->atvTopEqu[2]  = DRX_S9TOS16(  175 );
            pre->atvTopEqu[3]  = DRX_S9TOS16( -125 );

            pre->peakFilter    = -2;
         }

         if ( std == DRX_STANDARD_PAL_SECAM_DK )
         {
            pre->atvTopEqu[0]  = DRX_S9TOS16(  -55 );
            pre->atvTopEqu[1]  = DRX_S9TOS16(    5 );
            pre->atvTopEqu[2]  = DRX_S9TOS16(  125 );
            pre->atvTopEqu[3]  = DRX_S9TOS16(  105 );

            pre->peakFilter    = -2;
      }

         if ( std == DRX_STANDARD_PAL_SECAM_I )
         {
            pre->atvTopEqu[0]  = DRX_S9TOS16(    9 );
            pre->atvTopEqu[1]  = DRX_S9TOS16(  -10 );
            pre->atvTopEqu[2]  = DRX_S9TOS16(   40 );
            pre->atvTopEqu[3]  = DRX_S9TOS16( -110 );

            pre->peakFilter    = -2;
         }

         if ( std == DRX_STANDARD_NTSC )
         {
            pre->atvTopEqu[0]  = DRX_S9TOS16(   10 );
            pre->atvTopEqu[1]  = DRX_S9TOS16(   20 );
            pre->atvTopEqu[2]  = DRX_S9TOS16(  105 );
            pre->atvTopEqu[3]  = DRX_S9TOS16(  -20 );

            pre->peakFilter    = -1;
         }

         if ( std == DRX_STANDARD_PAL_SECAM_L )
         {
            pre->atvTopEqu[0]  = DRX_S9TOS16(    2 );
            pre->atvTopEqu[1]  = DRX_S9TOS16(    5 );
            pre->atvTopEqu[2]  = DRX_S9TOS16(   27 );
            pre->atvTopEqu[3]  = DRX_S9TOS16(  -90 );

            pre->peakFilter    = -2;
         }

         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->atvTopEqu[0]  = DRX_S9TOS16(   30 );
            pre->atvTopEqu[1]  = DRX_S9TOS16(  -30 );
            pre->atvTopEqu[2]  = DRX_S9TOS16(   85 );
            pre->atvTopEqu[3]  = DRX_S9TOS16(   35 );

            pre->peakFilter    = 2;
         }

      }

   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_MAXLINEAR_MXL5007
#define TUNER_MAXLINEAR_MXL5007
#endif
#endif
#ifdef  TUNER_MAXLINEAR_MXL5007
   /* MaxLinear MxL5007 specific */
   if ( !strComp( presetName, "MaxLinear MxL5007" ) )
   {
      pre->hasChannelPresets = TRUE;

      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->subMode              = 1;
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
         pre->useTgtCarrierIf      = FALSE;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->ifAgcCfg.speed  = 2;
         /* NOTE: for ofdm scu_ingain_tgt_min should be raised to 0x0b33 */
            pre->ifAgcCfg.top    =  0xb33;
         }
      }
      if ( level >= DRXK_PRESET_LEVEL_CHANNEL )
      {
         /* NOTE: for even qam constellations scu_ingain_tgt_min should be
                  raised to 0x0950 for odd 0x055e */
         if (channel != NULL)
         {
            if ( DRXK_ISQAMSTD( std ) )
            {
               if (  ( channel->constellation == DRX_CONSTELLATION_QAM16 ) ||
                     ( channel->constellation == DRX_CONSTELLATION_QAM64 ) ||
                     ( channel->constellation == DRX_CONSTELLATION_QAM256 ) )
               {
                  pre->ifAgcCfg.top = 0x950;
               }
               else
               {
                  pre->ifAgcCfg.top = 0x55e;
               }
            }
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_MICROTUNE_MT2063_K
#define TUNER_MICROTUNE_MT2063_K
#endif
#endif
#ifdef  TUNER_MICROTUNE_MT2063_K
   /* Microtune MT2063 specific */
   if ( !strComp( presetName, "Microtune MT2063" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->subMode              = 3; /* Internal RF-AGC */
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if (std == DRX_STANDARD_PAL_SECAM_L )
         {
            pre->tgtCarrierIf = 39300;
         }

      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_MICROTUNE_MT2063_TAIWAN
#define TUNER_MICROTUNE_MT2063_TAIWAN
#endif
#endif
#ifdef  TUNER_MICROTUNE_MT2063_TAIWAN
   /* Microtune MT2063 (6MHz) specific */
   if ( !strComp( presetName, "Microtune MT2063 Taiwan" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->subMode              = 3; /* Internal RF-AGC */
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( std == DRX_STANDARD_NTSC)
         {
            pre->tgtCarrierIf         = 45500;
         }

      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_MICROTUNE_MT2131_K
#define TUNER_MICROTUNE_MT2131_K
#endif
#endif
#ifdef  TUNER_MICROTUNE_MT2131_K
   /* Microtune MT2131 specific */
   if ( !strComp( presetName, "Microtune MT2131" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_NUTUNE_HD1856AF
#define TUNER_NUTUNE_HD1856AF
#endif
#endif
#ifdef  TUNER_NUTUNE_HD1856AF
   /* NuTune HD1856AF MK2 specific */
   if ( !strComp( presetName, "NuTune HD1856AF MK2") )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference   = 7;
         if ( DRXK_ISATVSTD( std ) )
         {
            pre->subMode         = 1; /* Ext RF-AGC, 62.5kHz */

            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top = 8470;
            }
            else
            {
               pre->rfAgcCfg.top = 7690;
            }
         }
         if ( DRXK_ISQAMSTD( std ) )
         {
            pre->subMode         = 1; /* Ext RF-AGC, 62.5kHz */
            pre->rfAgcCfg.top    = 7660;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode         = 0; /* Ext. RF-AGC, 166.7kHz */
            pre->rfAgcCfg.top    = 7845;
         }
      }
   }
#endif /* TUNER_NUTUNE_HD1856AF */

#ifdef  TUNER_ALL
#ifndef TUNER_NUTUNE_NH3660_NIM
#define TUNER_NUTUNE_NH3660_NIM
#endif
#endif
#ifdef  TUNER_NUTUNE_NH3660_NIM
   /* NuTune NH3660 specific */
   if ( !strComp( presetName, "NuTune NH3660 NIM" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         /* presaw reference same for all standards */
         pre->preSawCfg.reference  = 6;

         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 1; /* Ext. RF-AGC, 166.7kHz */
            pre->rfAgcCfg.top = 0x1e20;
         }
         if ( DRXK_ISQAMSTD (std) )
         {
            pre->subMode = 0; /* Ext. RF-AGC, 62.5kHz */
            pre->rfAgcCfg.top = 0x1d4a;
         }

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->subMode = 0; /* Ext. RF-AGC, 62.5kHz */
            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top = 0x1ed3;

            }
            else
            {
               pre->rfAgcCfg.top = 0x1d56;
            }
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_NUTUNE_TH2603
#define TUNER_NUTUNE_TH2603
#endif
#endif
#ifdef  TUNER_NUTUNE_TH2603
   /* NuTune TH2603 specific */
   if ( !strComp( presetName, "NuTune TH2603" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference  = 7;

         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 0; /* Ext. RF-AGC, 166.7kHz */
            pre->rfAgcCfg.top = 7815;
            pre->rfAgcCfg.speed  = 2; /* avoid RF AGC instability */
            pre->ifAgcCfg.speed  = 2;
         }
         if ( DRXK_ISQAMSTD (std) )
         {
            pre->subMode = 1; /* Ext. RF-AGC, 62.5kHz */
            pre->rfAgcCfg.top = 7625;
         }

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->subMode = 1; /* Ext. RF-AGC, 62.5kHz */
            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top = 8565;
            }
            else
            {
               pre->rfAgcCfg.top = 7935;
            }

            if (  std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16(  4 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -3 );
            }
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_NXP_HD1816AF_BHXP_K
#define TUNER_NXP_HD1816AF_BHXP_K
#endif
#endif
#ifdef  TUNER_NXP_HD1816AF_BHXP_K
   /* NXP HD1816AF_BHXP specific */
   if ( !strComp( presetName, "NXP HD1816AF_BHXP") )
   {
      /* signal strength mapping */
      pre->tunerRfGain             = 45;
      pre->rfAgcCfg.minOutputLevel = 8000;
      pre->rfAgcCfg.maxOutputLevel = 21000;
      pre->tunerIfGain             = 40;
      pre->ifAgcCfg.minOutputLevel = 1000;
      pre->ifAgcCfg.maxOutputLevel = 10500;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 9;
         if ( DRXK_ISATVSTD( std ) )
         {
            pre->atvTopNoiseTh    = 0; /* Switch off n.r. */
            pre->subMode          = 1; /* Ext RF-AGC, 62.5kHz */
            pre->rfAgcCfg.top     = 0x21AC;
            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top  = 0x2110 ;
            }
         }
         if ( DRXK_ISQAMSTD( std ) )
         {
            pre->subMode          = 1; /* Ext RF-AGC, 62.5kHz */
            pre->rfAgcCfg.top     = 0x20AC;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode          = 0; /* Ext. RF-AGC, 166.7kHz */
            pre->rfAgcCfg.top     = 0x2208;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TD1116AL_K
#define TUNER_NXP_TD1116AL_K
#endif
#endif
#ifdef  TUNER_NXP_TD1116AL_K
   /* NXP TD1116AL specific */
   if ( !strComp( presetName, "NXP TD1116AL") )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 10;
         if ( DRXK_ISATVSTD( std ) )
         {
            pre->subMode      = 1; /* Ext. RF-AGC 62.5kHz */
            pre->rfAgcCfg.top = 12340;
            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top  = 12840;
            }
         }
         if ( DRXK_ISQAMSTD( std ) )
         {
            pre->subMode = 1; /* Ext. RF-AGC 62.5kHz */
            pre->rfAgcCfg.top  = 12080;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode      = 0; /* Ext. RF-AGC, 166.7kHz */
            pre->rfAgcCfg.top = 12255;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TD1716F_3_K
#define TUNER_NXP_TD1716F_3_K
#endif
#endif
#ifdef  TUNER_NXP_TD1716F_3_K
   /* NXP TD1716F mark 3 specific */
   if ( !strComp( presetName, "NXP TD1716F 3") )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->rfAgcCfg.minOutputLevel =  6023;
         pre->rfAgcCfg.maxOutputLevel = 20000;

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->atvTopNoiseTh = 0; /* Switch off n.r. */
            pre->subMode       = 1; /* Ext. RF-AGC,  62.5kHz */
            pre->preSawCfg.reference = 3;
            pre->rfAgcCfg.top        = 0x1F00;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->tgtCarrierIf = (DRXFrequency_t)33200;

            pre->atvTopEqu[0] = DRX_S9TOS16(   5 );
            pre->atvTopEqu[1] = DRX_S9TOS16( -10 );
            pre->atvTopEqu[2] = DRX_S9TOS16(  40 );
            pre->atvTopEqu[3] = DRX_S9TOS16( -99 );
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode      = 1; /* Ext. RF-AGC,  62.5kHz */
            pre->rfAgcCfg.top = 0x28E0;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->rfAgcCfg.top = 0x1D80;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TD1716F_3_BALANCED_K
#define TUNER_NXP_TD1716F_3_BALANCED_K
#endif
#endif
#ifdef  TUNER_NXP_TD1716F_3_BALANCED_K
   /* NXP TD1716F mark 3 balanced specific */
   if ( !strComp( presetName, "NXP TD1716F 3 balanced") )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->rfAgcCfg.minOutputLevel =  6023;
         pre->rfAgcCfg.maxOutputLevel = 20000;
         pre->preSawCfg.reference = 8;

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->atvTopNoiseTh = 0; /* Switch off n.r. */
            pre->subMode       = 1; /* Ext. RF-AGC,  62.5kHz */

            pre->rfAgcCfg.top        = 8410;
         }
         if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
               ( std == DRX_STANDARD_PAL_SECAM_LP ) )
         {
            pre->rfAgcCfg.top        = 8970;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->tgtCarrierIf = (DRXFrequency_t)33200;

            pre->atvTopEqu[0] = DRX_S9TOS16(   5 );
            pre->atvTopEqu[1] = DRX_S9TOS16( -10 );
            pre->atvTopEqu[2] = DRX_S9TOS16(  40 );
            pre->atvTopEqu[3] = DRX_S9TOS16( -99 );
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode      = 1; /* Ext. RF-AGC,  62.5kHz */
            pre->rfAgcCfg.top = 8205;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->rfAgcCfg.top = 8395;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TD1716F_4_K
#define TUNER_NXP_TD1716F_4_K
#endif
#endif
#ifdef  TUNER_NXP_TD1716F_4_K
   /* NXP TD1716F mark 4 specific */
   if ( !strComp( presetName, "NXP TD1716F 4") )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->preSawCfg.reference  = 6;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->rfAgcCfg.top         = 0x2300;
            pre->rfAgcCfg.speed       = 1;
            pre->ifAgcCfg.speed       = 1;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->tgtCarrierIf = (DRXFrequency_t)33900;
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->rfAgcCfg.top         = 0x2300;
            pre->rfAgcCfg.speed       = 1;
            pre->ifAgcCfg.speed       = 1;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->rfAgcCfg.top         = 0x1B00;
            pre->rfAgcCfg.speed       = 0;
            pre->ifAgcCfg.speed       = 0;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TDA18271
#define TUNER_NXP_TDA18271
#endif
#endif
#ifdef  TUNER_NXP_TDA18271
   /* NXP TDA18271 specific */
   if ( !strComp( presetName, "NXP TDA18271" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->subMode              = 3;   /* IFlvl = 0.8  Vpp [-8 dB att] */
         pre->rfAgcSwitch          = FALSE;
         pre->preSawCfg.usePreSaw  = FALSE;
         pre->useTgtCarrierIf      = FALSE;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISQAMSTD(  std ) ) pre->rfAgcCfg.ctrlMode =DRXK_AGC_CTRL_OFF;
         if ( DRXK_ISDVBTSTD( std ) ) pre->rfAgcCfg.ctrlMode =DRXK_AGC_CTRL_OFF;
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TDA18272
#define TUNER_NXP_TDA18272
#endif
#endif
#ifdef  TUNER_NXP_TDA18272
   /* NXP TDA18272 specific */
   if ( !strComp( presetName, "NXP TDA18272" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->subMode              = 0;   /* NO SUBMODES */
         pre->rfAgcSwitch          = FALSE;
         pre->preSawCfg.usePreSaw  = FALSE;
         pre->useTgtCarrierIf      = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_PANASONIC_ENG37EXXKF_K
#define TUNER_PANASONIC_ENG37EXXKF_K
#endif
#endif
#ifdef  TUNER_PANASONIC_ENG37EXXKF_K
   /* Panasonic ENG37ExxKF specific */
   if ( !strComp( presetName, "Panasonic ENG37ExxKF" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->subMode              = 3; /* Ext. RF-AGC, ATV/QAM" */
            pre->rfAgcCfg.top         = 0x3600;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->atvTopEqu[0]            = DRX_S9TOS16(   -5 );
            pre->atvTopEqu[1]            = DRX_S9TOS16(    1 );
            pre->atvTopEqu[2]            = DRX_S9TOS16(   39 );
            pre->atvTopEqu[3]            = DRX_S9TOS16(   -1 );
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode              = 3; /* Ext. RF-AGC, ATV/QAM" */
            pre->rfAgcCfg.top         = 0x3600;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode              = 6; /* Ext. RF-AGC, OFDM     */
            pre->rfAgcCfg.top         = 0x3400;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_PANASONIC_ENG37F01KF
#define TUNER_PANASONIC_ENG37F01KF
#endif
#endif
#ifdef  TUNER_PANASONIC_ENG37F01KF
   /* Panasonic ENG37F01KF specific */
   if ( !strComp( presetName, "Panasonic ENG37F01KF" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->subMode = 1; /* TOP104dBuV, 62.5kHz */
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode = 1; /* TOP104dBuV, 62.5kHz */
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 0; /* TOP108dBuV, 166.7kHz */
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_PANASONIC_ENV57M06D8F
#define TUNER_PANASONIC_ENV57M06D8F
#endif
#endif
#ifdef  TUNER_PANASONIC_ENV57M06D8F
   /* Panasonic ENV57M06D8F specific */
   if ( !strComp( presetName, "Panasonic ENV57M06D8F" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference  = 3;

         if ( DRXK_ISATVSTD( std ) )
         {
            pre->subMode         = 0; /* EXT. RF-AGC, ATV 62.5 kHz */
            pre->rfAgcCfg.top    = 11475;

            if (  ( std == DRX_STANDARD_PAL_SECAM_L ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top = 12280;
      }
   }

         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode         = 1; /* EXT. RF-AGC, DIG 62.5 kHz */
            pre->rfAgcCfg.top    = 11445;
         }

         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode         = 2; /* EXT. RF-AGC, DIG 166.7 kHz */
            pre->rfAgcCfg.top    = 11700;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_MIC_PANASONIC_ENG37F01KF
#define TUNER_MIC_PANASONIC_ENG37F01KF
#endif
#endif
#ifdef  TUNER_MIC_PANASONIC_ENG37F01KF
   /* Mic Panasonic ENG37F01KF specific (Markified version) */
   if ( !strComp( presetName, "Mic Panasonic ENG37F01KF" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference  = 9;

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->atvTopNoiseTh  = 0; /* Switch off n.r. */
            pre->subMode        = 3; /* Ext, 62.5kHz */
            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top = 0x2200;
            }
            else
            {
               pre->rfAgcCfg.top = 0x2170;
            }
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode = 3; /* Ext, 62.5kHz */
            pre->rfAgcCfg.top = 0x2190;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 2; /* Ext, 166.7kHz */
            pre->rfAgcCfg.top = 0x2110;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_PANASONIC_ET35DRV
#define TUNER_PANASONIC_ET35DRV
#endif
#endif
#ifdef  TUNER_PANASONIC_ET35DRV
   /* Panasonic ET35DRV specific */
   if ( !strComp( presetName, "Panasonic ET35DRV" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference  = 7;

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->atvTopNoiseTh  = 0; /* Switch off n.r. */
            pre->subMode        = 3; /* Ext, 62.5kHz */
            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top = 9440;
            }
            else
            {
               pre->rfAgcCfg.top = 8760;
            }
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode = 3; /* Ext, 62.5kHz */
            pre->rfAgcCfg.top = 8860;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 2; /* Ext, 166.7kHz */
            pre->rfAgcCfg.top = 8935;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_SANYO_115U7A20C
#define TUNER_SANYO_115U7A20C
#endif
#endif
#ifdef  TUNER_SANYO_115U7A20C
   /* Sanyo 115U7A20C specific */
   if ( !strComp( presetName, "Sanyo 115U7A20C" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISDVBTSTD( std ) ) pre->subMode = 1;
                                 /* EXT. RF-AGC, 166.7 kHz */
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_SHARP_VA1E1ED2403
#define TUNER_SHARP_VA1E1ED2403
#endif
#endif
#ifdef  TUNER_SHARP_VA1E1ED2403
   /* Sharp VA1E1ED2403 specific */
   if ( !strComp( presetName, "Sharp VA1E1ED2403" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference   = 9;
         pre->rfAgcCfg.speed        = 1;
         pre->ifAgcCfg.speed        = 1;
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->subMode      = 2; /* Ext. RF-AGC, 50kHz ATV */
            pre->rfAgcCfg.top = 9830;

            if (  ( std == DRX_STANDARD_PAL_SECAM_L) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ))
            {

               pre->rfAgcCfg.top = 10175;
            }

         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode      = 0; /* Ext. RF-AGC, 166.7kHz */
            pre->rfAgcCfg.top = 9975;

         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode      = 1; /* Ext. RF-AGC, 50kHz QAM */
            pre->rfAgcCfg.top = 9925;
         }
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_SHARP_VA1E9ED2001
#define TUNER_SHARP_VA1E9ED2001
#endif
#endif
#ifdef  TUNER_SHARP_VA1E9ED2001
   /* Sharp VA1E9ED2001 specific */
   if ( !strComp( presetName, "Sharp VA1E9ED2001" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 7;

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->subMode      = 2; /* Ext. RF-AGC, 50kHz ATV */
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode      = 0; /* Ext. RF-AGC, 166.7kHz */

         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode      = 1; /* Ext. RF-AGC, 50kHz QAM */
         }
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_SHARP_VA1Y9ED200X
#define TUNER_SHARP_VA1Y9ED200X
#endif
#endif
#ifdef  TUNER_SHARP_VA1Y9ED200X
   /* Sharp VA1Y9ED200x specific */
   if ( !strComp( presetName, "Sharp VA1Y9ED200x" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 7;

         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->subMode      = 2; /* Ext. RF-AGC, 50kHz ATV */
            pre->rfAgcCfg.top = 7800;  /* ~57.4dBuV */

            if (  ( std == DRX_STANDARD_PAL_SECAM_L) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ))
            {
               pre->rfAgcCfg.top = 8190; /* ~59.6dBuV */
            }

            if ( std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16( -2 );
            }

            if ( std == DRX_STANDARD_PAL_SECAM_BG )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16( 12 );
            }
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode      = 0; /* Ext. RF-AGC, 166.7kHz */
            pre->rfAgcCfg.top = 7520; /* ~57.6dBuV */

         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->subMode      = 1; /* Ext. RF-AGC, 50kHz QAM */
            pre->rfAgcCfg.top = 7725; /* ~55.2dBuV */
         }
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_SONY_SUTRA111
#define TUNER_SONY_SUTRA111
#endif
#endif
#ifdef  TUNER_SONY_SUTRA111
   /* Sony SUT-RA111x specific */
   if ( !strComp( presetName, "Sony SUT-RA111x" ) )
   {
      pre->rfAgcSwitch       = FALSE;
      pre->rfAgcCfg.ctrlMode = DRXK_AGC_CTRL_OFF;
      pre->tunerIfAgcPol     = TRUE;   /* Inverted IF-AGC polarity */
      pre->useTgtCarrierIf   = FALSE;  /* Handled in tuner module */

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         switch ( std )
         {
            case DRX_STANDARD_PAL_SECAM_BG:
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSR111_BG_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSR111_BG_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof(DRXK_TAPS_SSR111_BG_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof(DRXK_TAPS_SSR111_BG_IM);

               pre->peakFilter   = -1;
               pre->atvTopEqu[0] = DRX_S9TOS16(   20 );
               pre->atvTopEqu[1] = DRX_S9TOS16(  -96 );
               pre->atvTopEqu[2] = DRX_S9TOS16(  210 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -120 );
               break;

            case DRX_STANDARD_PAL_SECAM_DK:
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSR111_DKI_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSR111_DKI_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof(DRXK_TAPS_SSR111_DKI_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof(DRXK_TAPS_SSR111_DKI_IM);

               pre->peakFilter   = -1;
               pre->atvTopEqu[0] = DRX_S9TOS16(   26 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -104 );
               pre->atvTopEqu[2] = DRX_S9TOS16(  176 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -114 );
               break;

            case DRX_STANDARD_PAL_SECAM_I:
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSR111_DKI_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSR111_DKI_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof(DRXK_TAPS_SSR111_DKI_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof(DRXK_TAPS_SSR111_DKI_IM);

               pre->peakFilter   = -1;
               pre->atvTopEqu[0] = DRX_S9TOS16(   26 );
               pre->atvTopEqu[1] = DRX_S9TOS16(  -10 );
               pre->atvTopEqu[2] = DRX_S9TOS16(   12 );
               pre->atvTopEqu[3] = DRX_S9TOS16(  -16 );
               break;

            case DRX_STANDARD_PAL_SECAM_L:
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSR111_LLP_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSR111_LLP_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof(DRXK_TAPS_SSR111_LLP_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof(DRXK_TAPS_SSR111_LLP_IM);

               pre->peakFilter   = -2;
               pre->atvTopEqu[0] = DRX_S9TOS16(   16 );
               pre->atvTopEqu[1] = DRX_S9TOS16(  -10 );
               pre->atvTopEqu[2] = DRX_S9TOS16(   45 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -137 );
               break;

            case DRX_STANDARD_PAL_SECAM_LP:
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSR111_LLP_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSR111_LLP_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof(DRXK_TAPS_SSR111_LLP_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof(DRXK_TAPS_SSR111_LLP_IM);

               pre->peakFilter   = -2;
               pre->atvTopEqu[0] = DRX_S9TOS16(   16 );
               pre->atvTopEqu[1] = DRX_S9TOS16(    0 );
               pre->atvTopEqu[2] = DRX_S9TOS16(   40 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -110 );
               break;

            case DRX_STANDARD_NTSC:
               break;
            default:
               break;
         }
      }
   }
#endif /* TUNER_SONY_SUTRA111 */

#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DNOS40AZH201J
#define TUNER_SSEM_DNOS40AZH201J
#endif
#endif
#ifdef  TUNER_SSEM_DNOS40AZH201J
   /* Samsung DNOS40AZH201J specific */
   if ( !strComp( presetName, "Samsung DNOS40AZH201J" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->rfAgcSwitch       = FALSE;
         pre->rfAgcCfg.ctrlMode = DRXK_AGC_CTRL_OFF;
         pre->tunerIfAgcPol     = TRUE; /* Inverted IF-AGC polarity */
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 1; /* 166kHz, LNA ON */
         }
         else
         {
            pre->subMode = 2; /* 50kHz, LNA OFF */
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTOS403PH17AS_K
#define TUNER_SSEM_DTOS403PH17AS_K
#endif
#endif
#ifdef  TUNER_SSEM_DTOS403PH17AS_K
   /* Samsung DTOS403PH17AS specific */
   if ( !strComp( presetName, "Samsung DTOS403PH17AS" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD(  std ) ) pre->subMode = 2; /* TOP 112dBuV */
         if ( DRXK_ISDVBTSTD( std ) ) pre->subMode = 2; /* TOP 112dBuV */
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTOS403LH121B
#define TUNER_SSEM_DTOS403LH121B
#endif
#endif
#ifdef  TUNER_SSEM_DTOS403LH121B
   /* Samsung DTOS403LH121B specific */
   if ( !strComp( presetName, "Samsung DTOS403LH121B" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 4;
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->atvTopNoiseTh   = 0; /* Switch off n.r. */
            pre->subMode         = 2; /* External RF-AGC, ATV */
            pre->rfAgcCfg.top    = 10555;
            if (  ( std == DRX_STANDARD_PAL_SECAM_L ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top = 10630;
            }

            /* EQ settings */
            if ( std == DRX_STANDARD_PAL_SECAM_DK )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16( -10 );
            }
            if ( std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->atvTopEqu[0] = DRX_S9TOS16( -1 );
            }
         }

         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 0; /* External RF-AGC, DVB-T */
            pre->rfAgcCfg.top = 10530;
         }

         if ( DRXK_ISQAMSTD( std ) )
         {
            pre->subMode = 1; /* External RF-AGC, QAM */
            pre->rfAgcCfg.top = 10465;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTOS403SH081A
#define TUNER_SSEM_DTOS403SH081A
#endif
#endif
#ifdef  TUNER_SSEM_DTOS403SH081A
   /* Samsung DTOS403SH081A specific */
   if ( !strComp( presetName, "Samsung DTOS403SH081A" ) )
   {
      pre->lockRangeMin             = -750;    /* kHz */
      pre->lockRangeMax             = 750;     /* kHz */
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 4;
         pre->rfAgcCfg.top = 10800;
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->atvTopNoiseTh   = 0; /* Switch off n.r. */
            pre->subMode         = 2; /* External RF-AGC, ATV */

            /* EQ settings */
            if ( std == DRX_STANDARD_NTSC )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_NTSC_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_NTSC_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof(DRXK_TAPS_SSK_NTSC_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof(DRXK_TAPS_SSK_NTSC_IM);
               pre->cfScale               = DRXK_TAPS_SSK_NTSC_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( 6 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -77 );
               pre->atvTopEqu[2] = DRX_S9TOS16( 205 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -190 );

               pre->peakFilter      = -7;
               pre->atvCvbsRefLvl   = 700;
            }
            if (  std == DRX_STANDARD_PAL_SECAM_BG )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_BG_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_BG_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_BG_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_BG_IM);
               pre->cfScale               = DRXK_TAPS_SSK_BG_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( -33 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -24 );
               pre->atvTopEqu[2] = DRX_S9TOS16( 119 );
                  pre->atvTopEqu[3] = DRX_S9TOS16(  -85 );

               pre->peakFilter   = -1;
               pre->atvCvbsRefLvl   = 700;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_DK )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_DKIL_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_DKIL_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_IM);
               pre->cfScale               = DRXK_TAPS_SSK_DKIL_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( -12 );
                  pre->atvTopEqu[1] = DRX_S9TOS16(  -47 );
               pre->atvTopEqu[2] = DRX_S9TOS16( 83 );
                  pre->atvTopEqu[3] = DRX_S9TOS16( -107 );


               pre->peakFilter      = 1;
               pre->atvCvbsRefLvl   = 696;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_I )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_DKIL_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_DKIL_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_IM);
               pre->cfScale               = DRXK_TAPS_SSK_DKIL_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( 3 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -29 );
               pre->atvTopEqu[2] = DRX_S9TOS16(   40 );
               pre->atvTopEqu[3] = DRX_S9TOS16(    9 );

               pre->peakFilter      = 1;
               pre->atvCvbsRefLvl   = 708;
            }
            if (  std == DRX_STANDARD_PAL_SECAM_L )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_DKIL_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_DKIL_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_IM);
               pre->cfScale               = DRXK_TAPS_SSK_DKIL_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( -5 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -2 );
               pre->atvTopEqu[2] = DRX_S9TOS16( 23 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -74 );

               pre->peakFilter      = 1;
               pre->atvCvbsRefLvl   = 612;

               pre->ifAgcCfg.top = 3500;
            }
            if (  std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_LP_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_LP_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_LP_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_LP_IM);
               pre->cfScale               = DRXK_TAPS_SSK_LP_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( 3 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -29 );
               pre->atvTopEqu[2] = DRX_S9TOS16(   83 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -149 );

               pre->peakFilter      = 1;
               pre->atvCvbsRefLvl   = 620;

               pre->ifAgcCfg.top = 3500;
            }
         }

         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 0; /* External RF-AGC, DVB-T */
            }

         if ( DRXK_ISQAMSTD( std ) )
         {
            pre->subMode = 1; /* External RF-AGC, QAM */
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTOS403SH081A_2
#define TUNER_SSEM_DTOS403SH081A_2
#endif
#endif
#ifdef  TUNER_SSEM_DTOS403SH081A_2
   /* Samsung DTOS403SH081A_2 specific */
   if ( !strComp( presetName, "Samsung DTOS403SH081A_2" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 4;
         pre->rfAgcCfg.top = 10800;
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->atvTopNoiseTh   = 0; /* Switch off n.r. */
            pre->subMode         = 2; /* External RF-AGC, ATV */

            /* EQ settings */
            if  ( std == DRX_STANDARD_NTSC )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_NTSC_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_NTSC_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof(DRXK_TAPS_SSK_NTSC_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof(DRXK_TAPS_SSK_NTSC_IM);
               pre->cfScale               = DRXK_TAPS_SSK_NTSC_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( 6 );
               pre->atvTopEqu[1] = DRX_S9TOS16(  -77 );
               pre->atvTopEqu[2] = DRX_S9TOS16(  205 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -190 );

               pre->peakFilter   = -7;
               pre->atvCvbsRefLvl   = 700;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_BG )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_BG_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_BG_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_BG_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_BG_IM);
               pre->cfScale               = DRXK_TAPS_SSK_BG_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( -33 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -24 );
               pre->atvTopEqu[2] = DRX_S9TOS16( 119 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -85 );

               pre->peakFilter      = -1;
               pre->atvCvbsRefLvl   = 700;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_DK )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_DKIL_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_DKIL_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_IM);
               pre->cfScale               = DRXK_TAPS_SSK_DKIL_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( -12 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -47 );
               pre->atvTopEqu[2] = DRX_S9TOS16( 83 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -107 );

               pre->peakFilter      = 1;
               pre->atvCvbsRefLvl   = 696;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_I )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_DKIL_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_DKIL_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_IM);
               pre->cfScale               = DRXK_TAPS_SSK_DKIL_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( 3 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -29 );
               pre->atvTopEqu[2] = DRX_S9TOS16( 40 );
               pre->atvTopEqu[3] = DRX_S9TOS16( 9 );

               pre->peakFilter      = 1;
               pre->atvCvbsRefLvl   = 708;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_L )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_DKIL_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_DKIL_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_DKIL_IM);
               pre->cfScale               = DRXK_TAPS_SSK_DKIL_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( -5 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -2 );
               pre->atvTopEqu[2] = DRX_S9TOS16( 23 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -74 );

               pre->peakFilter      = 1;
               pre->atvCvbsRefLvl   = 612;

               pre->ifAgcCfg.top = 3500;
            }
            if ( std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_LP_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_LP_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_LP_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_LP_IM);
               pre->cfScale               = DRXK_TAPS_SSK_LP_CFSCALE;

               pre->atvTopEqu[0] = DRX_S9TOS16( 3 );
               pre->atvTopEqu[1] = DRX_S9TOS16( -29 );
               pre->atvTopEqu[2] = DRX_S9TOS16( 83 );
               pre->atvTopEqu[3] = DRX_S9TOS16( -149 );

               pre->peakFilter      = 1;
               pre->atvCvbsRefLvl   = 620;
               pre->ifAgcCfg.top = 3500;
            }
         }

         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 0; /* External RF-AGC, DVB-T */
         }

         if ( DRXK_ISQAMSTD( std ) )
         {
            pre->subMode = 1; /* External RF-AGC, QAM */
         }
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTOS40FPL111A_K
#define TUNER_SSEM_DTOS40FPL111A_K
#endif
#endif
#ifdef  TUNER_SSEM_DTOS40FPL111A_K
   /* Samsung DTOS40FPL111A specific */
   if ( !strComp( presetName, "Samsung DTOS40FPL111A" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->rfAgcSwitch          = FALSE;
         pre->rfAgcCfg.ctrlMode    = DRXK_AGC_CTRL_OFF;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 0; /* 166kHz */
         }
         else
         {
            pre->subMode = 1; /* 62.5kHz */
         }
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTVS22CVL161A_INT
#define TUNER_SSEM_DTVS22CVL161A_INT
#endif
#endif
#ifdef  TUNER_SSEM_DTVS22CVL161A_INT
   /* Samsung DTVS22CVL161A Int. RF-AGC specific */
   if ( !strComp( presetName, "Samsung DTVS22CVL161A Int. RF-AGC" ))
   {
      pre->subMode = 2; /* Int. RF-AGC, LNA OFF */
      pre->rfAgcSwitch             = FALSE;
      pre->rfAgcCfg.ctrlMode       = DRXK_AGC_CTRL_OFF;
      pre->rfAgcCfg.top            = 0x7FFF;
      pre->rfAgcCfg.cutOffCurrent  = 0;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {


         if ( DRXK_ISDVBTSTD( std ) )
         {

         }
         if ( DRXK_ISATVSTD( std ) )
         {

            if ( std == DRX_STANDARD_NTSC )
            {
               pre->tgtCarrierIf = 45750;
               pre->atvNAgcKiMin = 0x227;
            }
         }
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTVS22CVL161A_EXT
#define TUNER_SSEM_DTVS22CVL161A_EXT
#endif
#endif
#ifdef  TUNER_SSEM_DTVS22CVL161A_EXT
   /* Samsung DTVS22CVL161A Ext. RF-AGC specific */
   if ( !strComp( presetName, "Samsung DTVS22CVL161A Ext. RF-AGC" ))
         {
      pre->lockRangeMin             = -750;    /* kHz */
      pre->lockRangeMax             = 750;     /* kHz */
      pre->subMode = 0; /* External RF-AGC, LNA OFF */
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
            {
         pre->preSawCfg.reference = 2;

         if ( DRXK_ISDVBTSTD ( std ) )
         {
            pre->rfAgcCfg.top = 11470;
            }
         if ( DRXK_ISQAMSTD ( std ) )
            {
            pre->rfAgcCfg.top = 10340;
         }
         if ( DRXK_ISATVSTD( std ) )
         {
            pre->rfAgcCfg.top = 10480;
            if ( std == DRX_STANDARD_NTSC )
            {
               pre->tgtCarrierIf = 45750;

               pre->atvTopEqu[0] = DRX_S9TOS16(  -54 );
               pre->atvTopEqu[1] = DRX_S9TOS16(   85 );
               pre->atvTopEqu[2] = DRX_S9TOS16(   88 );
               pre->atvTopEqu[3] = DRX_S9TOS16(  -50 );

               pre->channelFilter.dataRe  = (pu8_t) DRXK_TAPS_SSK_NTSC_RE;
               pre->channelFilter.dataIm  = (pu8_t) DRXK_TAPS_SSK_NTSC_IM;
               pre->channelFilter.sizeRe  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_NTSC_RE);
               pre->channelFilter.sizeIm  =
                  (u16_t) sizeof (DRXK_TAPS_SSK_NTSC_IM);
               pre->cfScale               = DRXK_TAPS_SSK_NTSC_CFSCALE;

               pre->rfAgcCfg.top = 11500;
               pre->preSawCfg.reference = 9;

               pre->peakFilter      = -3;
               pre->atvCvbsRefLvl   = 700;
               pre->atvNAgcKiMin    = 0x227;
            }
         }
      }
   }
#endif



#ifdef  TUNER_ALL
#ifndef TUNER_TCL_DT67WI3RE
#define TUNER_TCL_DT67WI3RE
#endif
#endif
#ifdef  TUNER_TCL_DT67WI3RE
   /* TCL DT67WI-3R-E */
   if ( !strComp( presetName, "TCL DT67WI-3R-E" ) )
   {

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 13;

         if ( DRXK_ISDVBTSTD ( std ) )
         {
            pre->subMode         = 0; /* Ext. RF-AGC, 166.7kHz */
            pre->rfAgcCfg.top    = 10920; /* ~ -61.9dBm */
         }
         if ( DRXK_ISQAMSTD ( std ) )
         {
            pre->subMode         = 1; /* Ext. RF-AGC,  62.5kHz */
            pre->rfAgcCfg.top    = 10720; /* ~ -59.9dBm */
         }
         if ( DRXK_ISATVSTD( std ) )
         {
            pre->subMode         = 2; /* Ext. RF-AGC,  50  kHz */
            pre->rfAgcCfg.top    = 10895; /* ~ -53.1dBm */

            if (  std == DRX_STANDARD_PAL_SECAM_L ||
                  std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->rfAgcCfg.top = 11430; /* ~ -54.9dBm */
            }
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_71303A
#define TUNER_THOMSON_DTT_71303A
#endif
#endif
#ifdef  TUNER_THOMSON_DTT_71303A
   /* Thomson DTT 71303A specific */
   if ( !strComp( presetName, "Thomson DTT 71303A" ))
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD( std ) )
         {
            pre->preSawCfg.reference = 2;
            pre->rfAgcCfg.top        = 0x1FAF;
            pre->peakFilter          = -1;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_L )
         {
            pre->preSawCfg.reference = 1;
            pre->rfAgcCfg.top        = 0x1DCF;
            pre->peakFilter          = -1;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->preSawCfg.reference = 1;
            pre->rfAgcCfg.top        = 0x1DCF;
            pre->peakFilter          = 3;

            pre->atvTopEqu[0]  = DRX_S9TOS16(    5 );
            pre->atvTopEqu[1]  = DRX_S9TOS16(  -10 );
            pre->atvTopEqu[2]  = DRX_S9TOS16(   40 );
            pre->atvTopEqu[3]  = DRX_S9TOS16(  -99 );
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_71306_BALANCED
#define TUNER_THOMSON_DTT_71306_BALANCED
#endif
#endif
#ifdef  TUNER_THOMSON_DTT_71306_BALANCED
   /* Thomson DTT 71306 balanced specific */
   if ( !strComp( presetName, "Thomson DTT 71306 balanced" ))
   {
      /* Currently default board, all default settings should apply */
      /* So settings below should be considered as defaults later   */
      pre->rfAgcCfg.speed = 2;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD( std ) )
         {
            pre->peakFilter           = -1;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_L )
         {
            pre->rfAgcCfg.top         = 0x2100;
            pre->ifAgcCfg.top         = 0x0B00;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->rfAgcCfg.top         = 0x2100;
            pre->ifAgcCfg.top         = 0x0B00;
            pre->peakFilter           = 3;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_71306_UNBALANCED
#define TUNER_THOMSON_DTT_71306_UNBALANCED
#endif
#endif
#ifdef  TUNER_THOMSON_DTT_71306_UNBALANCED
   /* Thomson DTT 71306 unbalanced specific */
   if ( !strComp( presetName, "Thomson DTT 71306 unbalanced" ))
   {
      pre->rfAgcCfg.speed = 2;
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->rfAgcCfg.top         = 0x1F60;
            pre->peakFilter           = -1;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_L )
         {
            pre->ifAgcCfg.top         = 0x0B00;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->ifAgcCfg.top         = 0x0B00;
            pre->peakFilter           = 3;
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->rfAgcCfg.top         = 0x1D00;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->rfAgcCfg.top         = 0x1900;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_7130X_INTERNAL
#define TUNER_THOMSON_DTT_7130X_INTERNAL
#endif
#endif
#ifdef  TUNER_THOMSON_DTT_7130X_INTERNAL
   /* Thomson DTT 7130x internal RF-AGC */
   if ( !strComp( presetName, "Thomson DTT 7130x int. RF-AGC" ))
   {
      pre->rfAgcSwitch             = FALSE;
      pre->rfAgcCfg.ctrlMode       = DRXK_AGC_CTRL_OFF;
      pre->rfAgcCfg.top            = 0x7FFF;
      pre->rfAgcCfg.cutOffCurrent  = 0;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {

         pre->subMode = 2;   /* TOP 109dBuV */
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->peakFilter           = -1;
         }
         if ( std == DRX_STANDARD_PAL_SECAM_LP )
         {
            pre->peakFilter           = 3;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 1;   /* TOP 112dBuV */
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_71303_BALANCED
#define TUNER_THOMSON_DTT_71303_BALANCED
#endif
#endif
#ifdef  TUNER_THOMSON_DTT_71303_BALANCED
   /* Thomson DTT 71303 balanced specific */
   if ( !strComp( presetName, "Thomson DTT 71303 balanced" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISQAMSTD(  std ) ) pre->rfAgcCfg.top = 0x2710;
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_7543X_K
#define TUNER_THOMSON_DTT_7543X_K
#endif
#endif
#ifdef  TUNER_THOMSON_DTT_7543X_K
   /* Thomson DTT 7543x specific */
   if ( !strComp( presetName, "Thomson DTT 7543x" ) )
   {
      pre->hasChannelPresets = TRUE;

      /* signal strength mapping */
      pre->tunerRfGain             = 45;
      pre->rfAgcCfg.minOutputLevel = 1200;
      pre->rfAgcCfg.maxOutputLevel = 27000;
      pre->tunerIfGain             = 40;
      pre->ifAgcCfg.minOutputLevel = 1000;
      pre->ifAgcCfg.maxOutputLevel = 10500;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference    = 13;
         pre->rfAgcCfg.cutOffCurrent = 0;
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 0;   /* 166kHz */
            pre->preSawCfg.reference  = 12;
            pre->rfAgcCfg.top = 0x15FF;
         }

         if ( DRXK_ISQAMSTD( std ) )
         {
            pre->subMode = 1; /* 62.5kHz */
            pre->preSawCfg.reference = 14;
            pre->rfAgcCfg.top = 0x15FF;
         }

         if ( DRXK_ISATVSTD( std ) )
         {
            pre->subMode = 1; /* 62.5kHz */

            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               /* positive modulation */
               pre->rfAgcCfg.top = 0x1550;
            }
            else
            {
               pre->rfAgcCfg.top = 0x1526;
            }
         }
      }

      if ( level >= DRXK_PRESET_LEVEL_CHANNEL )
      {
         if (channel != NULL)
         {
            /* Video peaking filter */
            if (  std == DRX_STANDARD_PAL_SECAM_L ||
                  std == DRX_STANDARD_PAL_SECAM_LP )
            {
               /* SECAM */
               if ( channel->frequency < 108000 )
               {
                  pre->peakFilter = -4;
               }
               else if ( channel->frequency < 230000 )
               {
                  pre->peakFilter = 4;
               }
               else if ( channel->frequency < 542000 )
               {
                  pre->peakFilter = 4;
               }
               else if ( channel->frequency < 605000 )
               {
                  pre->peakFilter = 4;
               }
               else
               {
                  pre->peakFilter = 3;
               }
            }
            else
            {
               /* PAL */
               if ( channel->frequency < 108000 )
               {
                  pre->peakFilter = 6;
               }
               else if ( channel->frequency < 230000 )
               {
                  pre->peakFilter = 4;
               }
               else if ( channel->frequency < 542000 )
               {
                  pre->peakFilter = 5;
               }
               else if ( channel->frequency < 605000 )
               {
                  pre->peakFilter = 4;
               }
               else
               {
                  pre->peakFilter = 3;
               }
            }

            /* Equalizer settings */
            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               /* SECAM */
               if ( channel->frequency < 108000 )
               {
                  pre->atvTopEqu[0] = (s16_t)    9;
                  pre->atvTopEqu[1] = (s16_t)    0;
                  pre->atvTopEqu[2] = (s16_t)   40;
                  pre->atvTopEqu[3] = (s16_t) -110;
               }
               else if ( channel->frequency < 230000 )
               {
                  pre->atvTopEqu[0] = (s16_t)    3;
                  pre->atvTopEqu[1] = (s16_t)  -10;
                  pre->atvTopEqu[2] = (s16_t)   40;
                  pre->atvTopEqu[3] = (s16_t) -110;
               }
               else
               {
                  pre->atvTopEqu[0] = (s16_t)    3;
                  pre->atvTopEqu[1] = (s16_t)  -10;
                  pre->atvTopEqu[2] = (s16_t)   40;
                  pre->atvTopEqu[3] = (s16_t) -110;
               }
            }
            else
            {
               /* PAL */
               if ( channel->frequency < 108000 )
               {
                  pre->atvTopEqu[0] = (s16_t)    7;
                  pre->atvTopEqu[1] = (s16_t)  -98;
                  pre->atvTopEqu[2] = (s16_t)  197;
                  pre->atvTopEqu[3] = (s16_t) -210;
               }
               else if ( channel->frequency < 230000 )
               {
                  pre->atvTopEqu[0] = (s16_t)    7;
                  pre->atvTopEqu[1] = (s16_t) -106;
                  pre->atvTopEqu[2] = (s16_t)  203;
                  pre->atvTopEqu[3] = (s16_t) -210;
               }
               else
               {
                  pre->atvTopEqu[0] = (s16_t)    7;
                  pre->atvTopEqu[1] = (s16_t) -105;
                  pre->atvTopEqu[2] = (s16_t)  197;
                  pre->atvTopEqu[3] = (s16_t) -210;
               }
            }
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_7544X_K
#define TUNER_THOMSON_DTT_7544X_K
#endif
#endif
#ifdef  TUNER_THOMSON_DTT_7544X_K
   /* Thomson DTT 7544x specific */
   if ( !strComp( presetName, "Thomson DTT 7544x" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {


      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference = 14;

         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode      = 0; /* 166 kHz, Antenna off */
            pre->rfAgcCfg.top = 0x155f;

         }
         if ( DRXK_ISQAMSTD( std ) )
         {
            pre->subMode      = 2; /* 62.5 kHz, Antenna off */
            pre->rfAgcCfg.top = 0x16c4;

         }
         if ( DRXK_ISATVSTD( std ) )
         {
            pre->subMode = 2; /* 62.5 kHz, Antenna off */
            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               /* positive modulation */
               pre->rfAgcCfg.top = 0x16d0;

            }
            else
            {
               /* negative modulation */
               pre->rfAgcCfg.top = 0x16af;

            }

         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_7546X_K
#define TUNER_THOMSON_DTT_7546X_K
#endif
#endif
#ifdef  TUNER_THOMSON_DTT_7546x_K
   /* Thomson DTT 7546x specific */
   if ( !strComp( presetName, "Thomson DTT 7546x" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->rfAgcCfg.speed       = 2;
         pre->ifAgcCfg.speed       = 2;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISATVSTD(  std ) )
         {
            pre->preSawCfg.reference  = 9;
            pre->rfAgcCfg.top         = 0x3080;
         }
         if ( DRXK_ISQAMSTD(  std ) )
         {
            pre->preSawCfg.reference  = 6;
            pre->rfAgcCfg.top         = 0x3110;
         }
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->preSawCfg.reference  = 6;
            pre->rfAgcCfg.top         = 0x3110;
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_768XX
#define TUNER_THOMSON_DTT_768XX
#endif
#endif
#ifdef  TUNER_THOMSON_DTT_768XX
   /* Thomson DTT 768XX specific */
   if ( !strComp( presetName, "Thomson DTT 768xx" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {

      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISQAMSTD( std ))
         {
            pre->qamBWBound6MHz = 6210000;
         }
         if ( DRXK_ISATVSTD(  std ) )
         {
            if  (std == DRX_STANDARD_NTSC)
            {
               pre->tgtCarrierIf = 45750;
            }
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_768XX_INT
#define TUNER_THOMSON_DTT_768XX_INT
#endif
#endif
#ifdef  TUNER_THOMSON_DTT_768XX_INT
   /* Thomson DTT 768xx Int. RF-AGCc */
   if ( !strComp( presetName, "Thomson DTT 768xx Int. RF-AGC" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_OPEN )
      {
         pre->rfAgcSwitch             = FALSE;
         pre->rfAgcCfg.ctrlMode       = DRXK_AGC_CTRL_OFF;
      }
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISQAMSTD( std ))
         {
            pre->qamBWBound6MHz = 5400000;
         }
         if ( DRXK_ISATVSTD(  std ) )
         {
            if  (std == DRX_STANDARD_NTSC)
            {
               pre->tgtCarrierIf = 45750;
            }
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_XUGUANG_DTT_8X1C
#define TUNER_XUGUANG_DTT_8X1C
#endif
#endif
#ifdef  TUNER_XUGUANG_DTT_8X1C
   /* Xuguang DTT-8X1C specific */
   if ( !strComp( presetName, "Xuguang DTT-8x1C" ) )
   {
      pre->rfAgcCfg.ctrlMode = DRXK_AGC_CTRL_OFF;
      pre->rfAgcSwitch = FALSE;

      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 4; /* TOP 112dBuV, 166.7kHz */
         }
         else if ( DRXK_ISQAMSTD ( std ) )
         {
            pre->subMode = 7; /* TOP 115dBuV, 62.5kHz */
         }
         else /* ATV */
         {
            pre->subMode = 7; /* TOP 115dBuV, 62.5kHz */
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_XUGUANG_DTT_8X1C_EXT_IF
#define TUNER_XUGUANG_DTT_8X1C_EXT_IF
#endif
#endif
#ifdef  TUNER_XUGUANG_DTT_8X1C_EXT_IF
   /* Xuguang DTT-8X1C specific */
   if ( !strComp( presetName, "Xuguang DTT-8x1C Ext. IF" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference     = 7;
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 0; /* External RF-AGC, 166.7 kHz */
            pre->rfAgcCfg.top =  9340;
         }
         else if ( DRXK_ISQAMSTD ( std ) )
         {
            pre->subMode = 1; /* External RF-AGC, 62.5 kHz */
            pre->rfAgcCfg.top =  9130;
         }
         else /* ATV */
         {
            pre->atvTopNoiseTh    = 0; /* Switch off n.r. */
            pre->subMode = 1; /* External RF-AGC, 62.5  kHz */

            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top =  9100;
            }
            else
            {
               pre->rfAgcCfg.top =  9305;
            }
         }
      }
   }
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_XUGUANG_DTT_8X1C_T1146
#define TUNER_XUGUANG_DTT_8X1C_T1146
#endif
#endif
#ifdef  TUNER_XUGUANG_DTT_8X1C_T1146
   /* Xuguang DTT-8X1C-T1146 specific */
   if ( !strComp( presetName, "Xuguang DTT-8x1C-T1146" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
         {
         pre->preSawCfg.reference = 3;
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 0; /* External RF-AGC, 166.7 kHz */
            pre->rfAgcCfg.top =  11810;
         }
         else if ( DRXK_ISQAMSTD ( std ) )
         {
            pre->subMode = 1; /* External RF-AGC, 62.5 kHz */
            pre->rfAgcCfg.top =  11575;
         }
         else /* ATV */
         {
            pre->atvTopNoiseTh    = 0; /* Switch off n.r. */
            pre->subMode = 1; /* External RF-AGC, 62.5  kHz */

            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top =  13320;
         }
            else
            {
               pre->rfAgcCfg.top =  12400;
            }

            if ( std == DRX_STANDARD_PAL_SECAM_LP )
            {
               pre->atvTopEqu[0] = (s16_t)    -5;
            }
         }
      }
   }
#endif


#ifdef  TUNER_ALL
#ifndef TUNER_XUGUANG_SDTT_6A
#define TUNER_XUGUANG_SDTT_6A
#endif
#endif
#ifdef  TUNER_XUGUANG_SDTT_6A
   /* Xuguang SDTT-6A specific */
   if ( !strComp( presetName, "Xuguang SDTT-6A" ) )
   {
      if ( level >= DRXK_PRESET_LEVEL_STANDARD )
      {
         pre->preSawCfg.reference     = 9;
         if ( DRXK_ISDVBTSTD( std ) )
         {
            pre->subMode = 0; /* External RF-AGC, 166  kHz */
            pre->rfAgcCfg.top =  8390;
         }
         else if ( DRXK_ISQAMSTD ( std ) )
         {
            pre->subMode = 1; /* External RF-AGC, 62.5 kHz */
            pre->rfAgcCfg.top =  8245;
         }
         else /* ATV */
         {
            pre->atvTopNoiseTh   = 0; /* Switch off n.r. */
            pre->subMode         = 1; /* External RF-AGC, 62.5  kHz */

            if (  ( std == DRX_STANDARD_PAL_SECAM_L  ) ||
                  ( std == DRX_STANDARD_PAL_SECAM_LP ) )
            {
               pre->rfAgcCfg.top =  8190;
            }
            else
            {
               pre->rfAgcCfg.top =  8390;
            }
         }
      }
   }
#endif

   /* apply RF AGC top alignment (if applicable) */
   DRXK_PresetApplyAlignment( pre );
}

/*============================================================================*/
/* END OF FILE */
