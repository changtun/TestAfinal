/**
* \file $Id: drxk_tuner_tables.h,v 1.203 2010/04/01 09:17:25 danielg Exp $
*
* \brief Tuner configurations
*
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
/*TICS -M3.7.3#1 : numerical constants allowed */

#ifndef __DRXK_TUNER_TABLES_H__
#define __DRXK_TUNER_TABLES_H__

/*------------------------------------------------------------------------------
INCLUDES
------------------------------------------------------------------------------*/

#include "bsp_tuner.h"

#ifdef __cplusplus
extern "C" {
#endif


/*-------------------------------------------------------------------------
DEFINES
-------------------------------------------------------------------------*/

#ifndef ARRAYREF
#define ARRAYREF(array) array, ( sizeof(array) / sizeof(array[0]) )
#endif

#ifndef MAXFRQ
#define MAXFRQ  ((DRXFrequency_t)(0x7FFFFFFF))
#endif

#ifndef TUNER_I2C_ADDR
#define TUNER_I2C_ADDR ((0x60)<<1) /* default I2C address for tuners */
#endif

#ifndef TUNER_DEV_ID
#define TUNER_DEV_ID   (2)         /* default tuner device id */
#endif

/* application flags */
#ifndef TUNER_EXTERNAL_SAW_SWITCHING
#define TUNER_EXTERNAL_SAW_SWITCHING  0x10
#endif
#ifndef TUNER_INVERTED_IFAGC
#define TUNER_INVERTED_IFAGC  0x8
#endif
#ifndef TUNER_MIRRORS
#define TUNER_MIRRORS         0x4
#endif
#ifndef TUNER_WITHOUT_RFAGC
#define TUNER_WITHOUT_RFAGC   0x2
#endif
#ifndef TUNER_WITHOUT_IFAMP
#define TUNER_WITHOUT_IFAMP   0x1
#endif


/*------------------------------------------------------------------------------
TUNER SWITCHES
------------------------------------------------------------------------------*/

/* #define TUNER_ALL                          */ /* uncomment to compile all tuner configurations     */
/* #define TUNER_DUMMY_TUNER                  */ /* uncomment to compile dummy tuner                  */
/* #define TUNER_ALPS_TDQG4X003A_K            */ /* uncomment to configure Alps TDQG4X003A            */
/* #define TUNER_ALPS_TDAGX                   */ /* uncomment to configure Alps TDQAG                 */
/* #define TUNER_ALPS_TDAG4_NIM               */ /* uncomment to configure Alps TDQAG4 NIM            */
/* #define TUNER_CHINA_DRAGON_CDT_SFT338_40   */ /* uncomment to configure China Dragon CDT-SFT338-40 */
/* #define TUNER_ENTROPIC_EN4020              */ /* uncomment to configure Entropic EN4020            */
/* #define TUNER_EARDA_EDT3021II2A            */ /* uncomment to configure Earda EDT-3021II2A         */
/* #define TUNER_EARDA_EHU100X1               */ /* uncomment to configure Earda EHU-100x1            */
/* #define TUNER_GOLDEN_DRAGON_DTT5K6_K       */ /* uncomment to configure Golden Dragon DTT-5K6      */
/* #define TUNER_JINXIN_UVS1X05DCW1_K         */ /* uncomment to configure Jinxin UVS1x05DCW1         */
/* #define TUNER_LGI_TDFW_G235D_NIM_K         */ /* uncomment to configure LG Innotek TDFW G235D      */
/* #define TUNER_LGI_TDFW_G235D_NIM2_K        */ /* uncomment to configure LG Innotek TDFW G235D      */
/* #define TUNER_LGI_TDTC_G2XXD_K             */ /* uncomment to configure LG Innotek TDTC G2xxD      */
/* #define TUNER_LGI_TDTC_G323D               */ /* uncomment to configure LG Innotek TDTC G323D      */
/* #define TUNER_LGI_TDTC_G3XXD_K             */ /* uncomment to configure LG Innotek TDTC G3xxD      */
/* #define TUNER_LGI_TDTC_G4XXD_K             */ /* uncomment to configure LG Innotek TDTC G4xxD      */
/* #define TUNER_LGI_TDTC_G675D_K             */ /* uncomment to configure LG Innotek TDTC G675D      */
/* #define TUNER_LOEWE_FET08                  */ /* uncomment to configure Loewe FET08                */
/* #define TUNER_MAXIM_MAX3542                */ /* uncomment to configure Maxim Max3542              */
/* #define TUNER_MAXIM_MAX3543                */ /* uncomment to configure Maxim Max3543              */
/* #define TUNER_MAXLINEAR_MXL5007            */ /* uncomment to configure Maxlinear MXL5007          */
/* #define TUNER_MIC_LGI_TDFW_G235D_NIM_K     */ /* uncomment to configure Mic LG Innotek TDFW G235D  */
/* #define TUNER_MIC_PANASONIC_ENG37F01KF     */ /* uncomment to configure Mic Panasonic ENG37F01KF   */
/* #define TUNER_MICROTUNE_MT2063_K           */ /* uncomment to configure Microtune 2063             */
/* #define TUNER_MICROTUNE_MT2063_TAIWAN      */ /* uncomment to configure Microtune 2063 (6MHz)      */
/* #define TUNER_MICROTUNE_MT2131_K           */ /* uncomment to configure Microtune 2131             */
/* #define TUNER_NUTUNE_HD1856AF              */ /* uncomment to configure NuTune HD1856AF            */
/* #define TUNER_NUTUNE_NH3660_NIM            */ /* uncomment to configure NuTune NH3660 NIM          */
/* #define TUNER_NUTUNE_TH2603                */ /* uncomment to configure NuTune TH2603              */
/* #define TUNER_NXP_HD1816AF_BHXP_K          */ /* uncomment to configure NXP HD1816AF_BHXP          */
/* #define TUNER_NXP_TDA18271                 */ /* uncomment to configure NXP TDA18271               */
/* #define TUNER_NXP_TDA18272                 */ /* uncomment to configure NXP TDA18272               */
/* #define TUNER_NXP_TD1116AL_K               */ /* uncomment to configure NXP TD1116AL               */
/* #define TUNER_NXP_TD1716F_3_K              */ /* uncomment to configure NXP TD1716F 3              */
/* #define TUNER_NXP_TD1716F_3_BALANCED_K     */ /* uncomment to configure NXP TD1716F 3 Balanced     */
/* #define TUNER_NXP_TD1716F_4_K              */ /* uncomment to configure NXP TD1716F 4              */
/* #define TUNER_PANASONIC_ENG37EXXKF_K       */ /* uncomment to configure Panasonic ENG37ExxKF       */
/* #define TUNER_PANASONIC_ENG37F01KF         */ /* uncomment to configure Panasonic ENG37F01KF       */
/* #define TUNER_PANASONIC_ENV57M06D8F        */ /* uncomment to configure Panasonic ENV57M06D8F      */
/* #define TUNER_PANASONIC_ET35DRV            */ /* uncomment to configure Panasonic ET35DRV          */
/* #define TUNER_SANYO_115U7A20C              */ /* uncomment to configure Sanyo 115U7A20C            */
/* #define TUNER_SHARP_VA1E1ED2403            */ /* uncomment to configure Sharp VA1E1ED2403          */
/* #define TUNER_SHARP_VA1E9ED2001            */ /* uncomment to configure Sharp VA1E9ED2001          */
/* #define TUNER_SHARP_VA1Y9ED200X            */ /* uncomment to configure Sharp VA1Y9ED200x          */
/* #define TUNER_SSEM_DNOS40AZH201J           */ /* uncomment to configure Samsung DNOS40AZH201J      */
/* #define TUNER_SSEM_DTOS403LH121B           */ /* uncomment to configure Samsung DTOS403LH121B      */
/* #define TUNER_SSEM_DTOS403SH081A           */ /* uncomment to configure Samsung DTOS403SH081A      */
/* #define TUNER_SSEM_DTOS403SH081A_36        */ /* uncomment to configure Samsung DTOS403SH081A 36MHz*/
/* #define TUNER_SSEM_DTOS403PH17AS_K         */ /* uncomment to configure Samsung DTOS403PH17AS      */
/* #define TUNER_SSEM_DTOS40FPL111A_K         */ /* uncomment to configure Samsung DTOS40FPL111A      */
/* #defnie TUNER_SSEM_DTVS22CVL161A_EXT       */ /* uncomment to configure Samsung DTVS22CVL161A ext RF-AGC */
/* #define TUNER_SSEM_DTVS22CVL161A_INT       */ /* uncomment to configure Samsung DTVS22CVL161A int RF-AGC */
/* #define TUNER_SONY_SUTRA111                */ /* uncomment to configure Sony SUT-RA111x            */
/* #define TUNER_TCL_DT67WI3RE                */ /* uncomment to configure TLC DT67WI-R3-E            */
/* #define TUNER_THOMSON_DTT_71303A           */ /* uncomment to configure Thomson DTT 71303A         */
/* #define TUNER_THOMSON_DTT_71303_BALANCED   */ /* uncomment to configure Thomson DTT 71303 balanced   */
/* #define TUNER_THOMSON_DTT_71306_BALANCED   */ /* uncomment to configure Thomson DTT 71306 balanced   */
/* #define TUNER_THOMSON_DTT_71306_UNBALANCED */ /* uncomment to configure Thomson DTT 71306 unbalanced */
/* #define TUNER_THOMSON_DTT_7130X_INTERNAL   */ /* uncomment to configure Thomson DTT 7130x int RF-AGC */
/* #define TUNER_THOMSON_DTT_7540X_K          */ /* uncomment to configure Thomson DTT 7540x          */
/* #define TUNER_THOMSON_DTT_7543X_K          */ /* uncomment to configure Thomson DTT 7543x          */
/* #define TUNER_THOMSON_DTT_7544X_K          */ /* uncomment to configure Thomson DTT 7544x          */
/* #define TUNER_THOMSON_DTT_7546X_K          */ /* uncomment to configure Thomson DTT 7546x          */
/* #define TUNER_THOMSON_DTT_768XX            */ /* uncomment to configure Thomson DTT 768xx ext RF-AGC */
/* #define TUNER_THOMSON_DTT_768XX_INT        */ /* uncomment to configure Thomson DTT 768xx int RF-AGC */
/* #define TUNER_XUGUANG_DTT_8X1C             */ /* uncomment to configure Xuguang DTT-8X1C           */
/* #define TUNER_XUGUANG_DTT_8X1C_EXT_IF      */ /* uncomment to configure Xuguang DTT-8X1C (modified)*/
/* #define TUNER_XUGUANG_DTT_8X1C_T1146       */ /* uncomment to configure Xuguang DTT-8X1C-T1146     */
/* #define TUNER_XUGUANG_SDTT_6A              */ /* uncomment to configure Xuguang DTT-SDTT_6A        */


/*-----------------------------------------------------------------------------
DEFINES FOR MODE CONTRACTION
-----------------------------------------------------------------------------*/

#define TUNER_MODE_DIGITALANALOG   ( TUNER_MODE_DIGITAL | TUNER_MODE_ANALOG )
#define TUNER_MODE_SWITCHLOCK      ( TUNER_MODE_SWITCH  | TUNER_MODE_LOCK )
#define TUNER_MODE_67MHZ           ( TUNER_MODE_6MHZ    | TUNER_MODE_7MHZ )
#define TUNER_MODE_678MHZ          ( TUNER_MODE_6MHZ    | TUNER_MODE_7MHZ | \
                                     TUNER_MODE_8MHZ )


/*------------------------------------------------------------------------------
TUNER SETTINGS
------------------------------------------------------------------------------*/
#define  TUNER_NUTUNE_NH3660_NIM   	/* Pre-configured tuner selection */


/*----------------------------------------------------------------------------*/
/* All tuners compiled                                                        */
/*----------------------------------------------------------------------------*/

#ifdef  TUNER_ALL
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER                  /* do not use PRE_CONF_TUNER */
#endif
#endif



/*----------------------------------------------------------------------------*/
/* Alps Electric TDQG4X003A                                                   */
/*----------------------------------------------------------------------------*/

#ifdef  TUNER_ALL
#ifndef TUNER_ALPS_TDQG4X003A_K
#define TUNER_ALPS_TDQG4X003A_K
#endif
#endif

#ifdef  TUNER_ALPS_TDQG4X003A_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER tunerAlpsTDQG4X003A_K
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_AlpsTDQG4X003A_EXT_RFAGC_7MHZ_K[] =
{
   /*  -EXT, AGST = 000, 166kHz, R = 000 ; 10000000 = 0x80 */
   /*     - 153 MHz: CP = 010 , TS = 1011 , BS = 0001 */ { 153000 , { 0x80 , 0x61 , 0xE6 } },
   /* 153 - 398 MHz: CP = 010 , TS = 1011 , BS = 0010 */ { 398000 , { 0x80 , 0x62 , 0xE6 } },
   /* 398 - 430 MHz: CP = 100 , TS = 1011 , BS = 0010 */ { 430000 , { 0x80 , 0xA2 , 0xE6 } },
   /* 430 - 782 MHz: CP = 100 , TS = 1011 , BS = 1000 */ { 782000 , { 0x80 , 0xA8 , 0xE6 } },
   /* 782 - 862 MHz: CP = 110 , TS = 1011 , BS = 1000 */ { MAXFRQ , { 0x80 , 0xE8 , 0xE6 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDQG4X003A_EXT_RFAGC_8MHZ_K[] =
{
   /*  -EXT, AGST = 000, 166kHz, R = 000 ; 10000000 = 0x80 */
   /*     - 153 MHz: CP = 010 , TS = 1011 , BS = 0101 */ { 153000 , { 0x80 , 0x65 , 0xE6 } },
   /* 153 - 398 MHz: CP = 010 , TS = 1011 , BS = 0110 */ { 398000 , { 0x80 , 0x66 , 0xE6 } },
   /* 398 - 430 MHz: CP = 100 , TS = 1011 , BS = 0110 */ { 430000 , { 0x80 , 0xA6 , 0xE6 } },
   /* 430 - 782 MHz: CP = 100 , TS = 1011 , BS = 1100 */ { 782000 , { 0x80 , 0xAC , 0xE6 } },
   /* 782 - 862 MHz: CP = 110 , TS = 1011 , BS = 1100 */ { MAXFRQ , { 0x80 , 0xEC , 0xE6 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDQG4X003A_N3DB_7MHZ_K[] =
{
   /*  -3dB, AGST = 001, 166kHz, R = 000 ; 10001000 = 0x88 */
   /*     - 153 MHz: CP = 010 , TS = 1011 , BS = 0001 */ { 153000 , { 0x88 , 0x61 , 0xE6 } },
   /* 153 - 398 MHz: CP = 010 , TS = 1011 , BS = 0010 */ { 398000 , { 0x88 , 0x62 , 0xE6 } },
   /* 398 - 430 MHz: CP = 100 , TS = 1011 , BS = 0010 */ { 430000 , { 0x88 , 0xA2 , 0xE6 } },
   /* 430 - 782 MHz: CP = 100 , TS = 1011 , BS = 1000 */ { 782000 , { 0x88 , 0xA8 , 0xE6 } },
   /* 782 - 862 MHz: CP = 110 , TS = 1011 , BS = 1000 */ { MAXFRQ , { 0x88 , 0xE8 , 0xE6 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDQG4X003A_N3DB_8MHZ_K[] =
{
   /*  -3dB, AGST = 001, 166kHz, R = 000 ; 10001000 = 0x88 */
   /*     - 153 MHz: CP = 010 , TS = 1011 , BS = 0101 */ { 153000 , { 0x88 , 0x65 , 0xE6 } },
   /* 153 - 398 MHz: CP = 010 , TS = 1011 , BS = 0110 */ { 398000 , { 0x88 , 0x66 , 0xE6 } },
   /* 398 - 430 MHz: CP = 100 , TS = 1011 , BS = 0110 */ { 430000 , { 0x88 , 0xA6 , 0xE6 } },
   /* 430 - 782 MHz: CP = 100 , TS = 1011 , BS = 1100 */ { 782000 , { 0x88 , 0xAC , 0xE6 } },
   /* 782 - 862 MHz: CP = 110 , TS = 1011 , BS = 1100 */ { MAXFRQ , { 0x88 , 0xEC , 0xE6 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDQG4X003A_N1DB_7MHZ_K[] =
{
   /*  -3dB, AGST = 010, 166kHz, R = 000 ; 10010000 = 0x90 */
   /*     - 153 MHz: CP = 010 , TS = 1011 , BS = 0001 */ { 153000 , { 0x90 , 0x61 , 0xE6 } },
   /* 153 - 398 MHz: CP = 010 , TS = 1011 , BS = 0010 */ { 398000 , { 0x90 , 0x62 , 0xE6 } },
   /* 398 - 430 MHz: CP = 100 , TS = 1011 , BS = 0010 */ { 430000 , { 0x90 , 0xA2 , 0xE6 } },
   /* 430 - 782 MHz: CP = 100 , TS = 1011 , BS = 1000 */ { 782000 , { 0x90 , 0xA8 , 0xE6 } },
   /* 782 - 862 MHz: CP = 110 , TS = 1011 , BS = 1000 */ { MAXFRQ , { 0x90 , 0xE8 , 0xE6 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDQG4X003A_N1DB_8MHZ_K[] =
{
   /*  -3dB, AGST = 010, 166kHz, R = 000 ; 10010000 = 0x90 */
   /*     - 153 MHz: CP = 010 , TS = 1011 , BS = 0101 */ { 153000 , { 0x90 , 0x65 , 0xE6 } },
   /* 153 - 398 MHz: CP = 010 , TS = 1011 , BS = 0110 */ { 398000 , { 0x90 , 0x66 , 0xE6 } },
   /* 398 - 430 MHz: CP = 100 , TS = 1011 , BS = 0110 */ { 430000 , { 0x90 , 0xA6 , 0xE6 } },
   /* 430 - 782 MHz: CP = 100 , TS = 1011 , BS = 1100 */ { 782000 , { 0x90 , 0xAC , 0xE6 } },
   /* 782 - 862 MHz: CP = 110 , TS = 1011 , BS = 1100 */ { MAXFRQ , { 0x90 , 0xEC , 0xE6 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDQG4X003A_P1DB_7MHZ_K[] =
{
   /*  -3dB, AGST = 011, 166kHz, R = 000 ; 10011000 = 0x98 */
   /*     - 153 MHz: CP = 010 , TS = 1011 , BS = 0001 */ { 153000 , { 0x98 , 0x61 , 0xE6 } },
   /* 153 - 398 MHz: CP = 010 , TS = 1011 , BS = 0010 */ { 398000 , { 0x98 , 0x62 , 0xE6 } },
   /* 398 - 430 MHz: CP = 100 , TS = 1011 , BS = 0010 */ { 430000 , { 0x98 , 0xA2 , 0xE6 } },
   /* 430 - 782 MHz: CP = 100 , TS = 1011 , BS = 1000 */ { 782000 , { 0x98 , 0xA8 , 0xE6 } },
   /* 782 - 862 MHz: CP = 110 , TS = 1011 , BS = 1000 */ { MAXFRQ , { 0x98 , 0xE8 , 0xE6 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDQG4X003A_P1DB_8MHZ_K[] =
{
   /*  -3dB, AGST = 011, 166kHz, R = 000 ; 10011000 = 0x98 */
   /*     - 153 MHz: CP = 010 , TS = 1011 , BS = 0101 */ { 153000 , { 0x98 , 0x65 , 0xE6 } },
   /* 153 - 398 MHz: CP = 010 , TS = 1011 , BS = 0110 */ { 398000 , { 0x98 , 0x66 , 0xE6 } },
   /* 398 - 430 MHz: CP = 100 , TS = 1011 , BS = 0110 */ { 430000 , { 0x98 , 0xA6 , 0xE6 } },
   /* 430 - 782 MHz: CP = 100 , TS = 1011 , BS = 1100 */ { 782000 , { 0x98 , 0xAC , 0xE6 } },
   /* 782 - 862 MHz: CP = 110 , TS = 1011 , BS = 1100 */ { MAXFRQ , { 0x98 , 0xEC , 0xE6 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDQG4X003A_P3DB_7MHZ_K[] =
{
   /*  -3dB, AGST = 100, 166kHz, R = 000 ; 10100000 = 0xA8 */
   /*     - 153 MHz: CP = 010 , TS = 1011 , BS = 0001 */ { 153000 , { 0xA8 , 0x61 , 0xE6 } },
   /* 153 - 398 MHz: CP = 010 , TS = 1011 , BS = 0010 */ { 398000 , { 0xA8 , 0x62 , 0xE6 } },
   /* 398 - 430 MHz: CP = 100 , TS = 1011 , BS = 0010 */ { 430000 , { 0xA8 , 0xA2 , 0xE6 } },
   /* 430 - 782 MHz: CP = 100 , TS = 1011 , BS = 1000 */ { 782000 , { 0xA8 , 0xA8 , 0xE6 } },
   /* 782 - 862 MHz: CP = 110 , TS = 1011 , BS = 1000 */ { MAXFRQ , { 0xA8 , 0xE8 , 0xE6 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDQG4X003A_P3DB_8MHZ_K[] =
{
   /*  -3dB, AGST = 100, 166kHz, R = 000 ; 10100000 = 0xA8 */
   /*     - 153 MHz: CP = 010 , TS = 1011 , BS = 0101 */ { 153000 , { 0xA8 , 0x65 , 0xE6 } },
   /* 153 - 398 MHz: CP = 010 , TS = 1011 , BS = 0110 */ { 398000 , { 0xA8 , 0x66 , 0xE6 } },
   /* 398 - 430 MHz: CP = 100 , TS = 1011 , BS = 0110 */ { 430000 , { 0xA8 , 0xA6 , 0xE6 } },
   /* 430 - 782 MHz: CP = 100 , TS = 1011 , BS = 1100 */ { 782000 , { 0xA8 , 0xAC , 0xE6 } },
   /* 782 - 862 MHz: CP = 110 , TS = 1011 , BS = 1100 */ { MAXFRQ , { 0xA8 , 0xEC , 0xE6 } }
};


static TUNERCANNEDControlTable_t TCT_AlpsTDQG4X003A_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "EXT, 7MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDQG4X003A_EXT_RFAGC_7MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "EXT, 8MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDQG4X003A_EXT_RFAGC_8MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "-3dB, 7MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDQG4X003A_N3DB_7MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "-3dB, 8MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDQG4X003A_N3DB_8MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "-1dB, 7MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDQG4X003A_N1DB_7MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "-1dB, 8MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDQG4X003A_N1DB_8MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "+1dB, 7MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDQG4X003A_P1DB_7MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "+1dB, 8MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDQG4X003A_P1DB_8MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "+3dB, 7MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDQG4X003A_P3DB_7MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "+3dB, 8MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDQG4X003A_P3DB_8MHZ_K
   }
};

static TUNERSubMode_t subModesAlpsTDQG4X003A_K[] =
{
   "External RF-AGC",
   "TOP -3dB",
   "TOP -1dB",
   "TOP +1dB",
   "TOP +3dB"
};

static TUNERCANNEDData_t dataAlpsTDQG4X003A_K =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_AlpsTDQG4X003A_K),
   /* nrOfControlBytes  */          3
};

static TUNERCommonAttr_t commonAttrAlpsTDQG4X003A_K =
{
   /* name            */  "Alps Electric TDQG4X003A",
   /* minFreqRF [kHz] */      44000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesAlpsTDQG4X003A_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36167,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerAlpsTDQG4X003A_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrAlpsTDQG4X003A_K,
   /* myExtAttr    */ &dataAlpsTDQG4X003A_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Alps-Electric-TDQG4X003A--------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Alps Electric TDAG                                                         */
/*----------------------------------------------------------------------------*/

#ifdef  TUNER_ALL
#ifndef TUNER_ALPS_TDAGX
#define TUNER_ALPS_TDAGX
#endif
#endif

#ifdef  TUNER_ALPS_TDAGX
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER tunerAlpsTDAGx
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_AlpsTDAGx_DVBT_7MHz[] =
{
   /*     - 125 MHz: BS = 0001 */ { 125000 , { 0x80 , 0x81 , 0xCA } },
   /* 125 - 366 MHz: BS = 0010 */ { 366000 , { 0x80 , 0x82 , 0xCA } },
   /* 366 - 726 MHz: BS = 1000 */ { 726000 , { 0x80 , 0xC8 , 0xCA } },
   /* 726 -     MHz: BS = 1000 */ { MAXFRQ , { 0x80 , 0x08 , 0xCA } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAGx_DVBT_8MHz[] =
{
   /*     - 125 MHz: BS = 0101 */ { 125000 , { 0x80 , 0x85 , 0xCA } },
   /* 125 - 366 MHz: BS = 0110 */ { 366000 , { 0x80 , 0x86 , 0xCA } },
   /* 366 - 726 MHz: BS = 1100 */ { 726000 , { 0x80 , 0xCC , 0xCA } },
   /* 726 -     MHz: BS = 1100 */ { MAXFRQ , { 0x80 , 0x0C , 0xCA } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAGx_DVBC_7MHz[] =
{
   /*     - 125 MHz: BS = 0001 */ { 125000 , { 0x83 , 0x01 , 0xCA } },
   /* 125 - 366 MHz: BS = 0010 */ { 366000 , { 0x83 , 0x02 , 0xCA } },
   /* 366 -     MHz: BS = 1000 */ { MAXFRQ , { 0x83 , 0x08 , 0xCA } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAGx_DVBC_8MHz[] =
{
   /*     - 125 MHz: BS = 0101 */ { 125000 , { 0x83 , 0x04 , 0xCA } },
   /* 125 - 366 MHz: BS = 0110 */ { 366000 , { 0x83 , 0x06 , 0xCA } },
   /* 366 -     MHz: BS = 1100 */ { MAXFRQ , { 0x83 , 0x0C , 0xCA } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAGx_BGIDKL_7MHz[] =
{
   /*     - 125 MHz: BS = 0001 */ { 125000 , { 0x82 , 0x01 , 0xEA } },
   /* 125 - 224 MHz: BS = 0010 */ { 224000 , { 0x82 , 0x02 , 0xEA } },
   /* 224 -     MHz: BS = 1000 */ { MAXFRQ , { 0x82 , 0x08 , 0xEA } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAGx_BGIDKL_8MHz[] =
{
   /*     - 125 MHz: BS = 0101 */ { 125000 , { 0x82 , 0x05 , 0xEA } },
   /* 125 - 224 MHz: BS = 0110 */ { 224000 , { 0x82 , 0x06 , 0xEA } },
   /* 224 -     MHz: BS = 1100 */ { MAXFRQ , { 0x82 , 0x0C , 0xEA } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAGx_Lp[] =
{
   /* VHF-L only, nevertheless implemented for full range */
   /*     - 125 MHz: BS = 0001 */ { 125000 , { 0x82 , 0x01 , 0xCA } },
   /* 125 - 366 MHz: BS = 0010 */ { 366000 , { 0x82 , 0x02 , 0xCA } },
   /* 366 -     MHz: BS = 1100 */ { MAXFRQ , { 0x82 , 0x0C , 0xCA } }
};


static TUNERCANNEDControlTable_t TCT_AlpsTDAGx[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "DVB-T, 7MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDAGx_DVBT_7MHz
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "DVB-T, 8MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDAGx_DVBT_8MHz
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "DVB-C, 7MHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_AlpsTDAGx_DVBC_7MHz
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "DVB-C, 8MHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_AlpsTDAGx_DVBC_8MHz
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "BG I DK L, 7MHz",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_AlpsTDAGx_BGIDKL_7MHz
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "BG I DK L, 8MHz",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_AlpsTDAGx_BGIDKL_8MHz
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "L'",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_AlpsTDAGx_Lp
   }
};

static TUNERSubMode_t subModesAlpsTDAGx[] =
{
   "DVB-T",
   "DVB-C",
   "BG I DK L",
   "L'"
};

static TUNERCANNEDData_t dataAlpsTDAGx =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_AlpsTDAGx),
   /* nrOfControlBytes  */          3
};


static TUNERCommonAttr_t commonAttrAlpsTDAGx =
{
   /* name            */  "Alps Electric TDAGx",
   /* minFreqRF [kHz] */      44000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesAlpsTDAGx),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */          0,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerAlpsTDAGx =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrAlpsTDAGx,
   /* myExtAttr    */ &dataAlpsTDAGx,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Alps-Electric-TDAG--------------------------------------------------*/




/*----------------------------------------------------------------------------*/
/* Alps Electric TDAG4 NIM                                                    */
/*----------------------------------------------------------------------------*/

#ifdef  TUNER_ALL
#ifndef TUNER_ALPS_TDAG4_NIM
#define TUNER_ALPS_TDAG4_NIM
#endif
#endif

#ifdef  TUNER_ALPS_TDAG4_NIM
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER tunerAlpsTDAG4_NIM
#endif

#include "tunercanned.h"



static TUNERCANNEDControlUnit_t TCU_AlpsTDAG4_NIM_OFDM_7[] =
{
   /*       -  64.5 MHz */ { 174000 , { 0x98 , 0x81 , 0xC2 } },
   /* 177.5 - 226.5 MHz */ { 470000 , { 0x98 , 0x82 , 0xC2 } },
   /* 474   - 658   MHz */ { 662000 , { 0x98 , 0x88 , 0xC2 } },
   /* 666   - 762   MHz */ { 766000 , { 0x98 , 0xC8 , 0xC2 } },
   /* 770   - 862   MHz */ { MAXFRQ , { 0x98 , 0x08 , 0xC2 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAG4_NIM_OFDM_8[] =
{
   /*       -  64.5 MHz */ { 174000 , { 0x98 , 0x85 , 0xC2 } },
   /* 177.5 - 226.5 MHz */ { 470000 , { 0x98 , 0x86 , 0xC2 } },
   /* 474   - 658   MHz */ { 662000 , { 0x98 , 0x8C , 0xC2 } },
   /* 666   - 762   MHz */ { 766000 , { 0x98 , 0xCC , 0xC2 } },
   /* 770   - 862   MHz */ { MAXFRQ , { 0x98 , 0x0C , 0xC2 } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAG4_NIM_QAM_7[] =
{
   /*     - 125 MHz */ { 125000 , { 0x9B , 0x01 , 0xCA } },
   /* 125 - 174 MHz */ { 174000 , { 0x9B , 0x02 , 0xCA } },
   /* 174 - 366 MHz */ { 366000 , { 0x9B , 0x02 , 0xCA } },
   /* 366 - 470 MHz */ { 470000 , { 0x9B , 0x08 , 0xCA } },
   /* 470 - 766 MHz */ { 766000 , { 0x9B , 0x08 , 0xCA } },
   /* 766 - 862 MHz */ { MAXFRQ , { 0x9B , 0x08 , 0xCA } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAG4_NIM_QAM_8[] =
{
   /*     - 125 MHz */ { 125000 , { 0x9B , 0x05 , 0xCA } },
   /* 125 - 174 MHz */ { 174000 , { 0x9B , 0x06 , 0xCA } },
   /* 174 - 366 MHz */ { 366000 , { 0x9B , 0x06 , 0xCA } },
   /* 366 - 470 MHz */ { 470000 , { 0x9B , 0x0C , 0xCA } },
   /* 470 - 766 MHz */ { 766000 , { 0x9B , 0x0C , 0xCA } },
   /* 766 - 862 MHz */ { MAXFRQ , { 0x9B , 0x0C , 0xCA } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAG4_NIM_ATV_NOT_LP[] =
{
   /*     - 125 MHz */ { 125000 , { 0x92 , 0x01 , 0xEE } },
   /* 125 - 366 MHz */ { 366000 , { 0x92 , 0x02 , 0xEE } },
   /* 366 - 862 MHz */ { MAXFRQ , { 0x92 , 0x08 , 0xEE } }
};

static TUNERCANNEDControlUnit_t TCU_AlpsTDAG4_NIM_ATV_LP[] =
{
   /*     - 125 MHz */ { 125000 , { 0x92 , 0x01 , 0xCE } },
   /* 125 - 366 MHz */ { 366000 , { 0x92 , 0x02 , 0xCE } },
   /* 366 - 862 MHz */ { MAXFRQ , { 0x92 , 0x08 , 0xCE } }
};



static TUNERCANNEDControlTable_t TCT_AlpsTDAG4_NIM[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "DVB-T, 7MHz, TOP -3dB",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDAG4_NIM_OFDM_7
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "DVB-T, 8MHz, TOP -3dB",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_AlpsTDAG4_NIM_OFDM_8
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "DVB-C, 7 MHz, TOP -3dB",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_AlpsTDAG4_NIM_QAM_7
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "DVB-C, 8 MHz, TOP -3dB",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_AlpsTDAG4_NIM_QAM_8
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "ATV, not L', TOP -5dB",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_AlpsTDAG4_NIM_ATV_NOT_LP
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "ATV, L', TOP -5dB",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_AlpsTDAG4_NIM_ATV_LP
   }
};

static TUNERSubMode_t subModesAlpsTDAG4_NIM[] =
{
   "DVB-T",
   "DVB-C",
   "ATV, not L'",
   "ATV, L'"
};

static TUNERCANNEDData_t dataAlpsTDAG4_NIM =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_AlpsTDAG4_NIM),
   /* nrOfControlBytes  */          3
};

static TUNERCommonAttr_t commonAttrAlpsTDAG4_NIM =
{
   /* name            */  "Alps Electric TDAG4 NIM",
   /* minFreqRF [kHz] */      44000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesAlpsTDAG4_NIM),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerAlpsTDAG4_NIM =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrAlpsTDAG4_NIM,
   /* myExtAttr    */ &dataAlpsTDAG4_NIM,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Alps-Electric-TDAG4-------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* China Dragon CDT-SFT338-40 tuner configuration                             */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_CHINA_DRAGON_CDT_SFT338_40
#define TUNER_CHINA_DRAGON_CDT_SFT338_40
#endif
#endif


#ifdef  TUNER_CHINA_DRAGON_CDT_SFT338_40
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerChinaDragonCDTSFT33840
#endif
#endif


#ifdef TUNER_CHINA_DRAGON_CDT_SFT338_40

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_AUX_EXT[] =
{
   /* ATC = 0/110 */ { MAXFRQ , { 0x9C , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_AUX_SCAN_N3DB[] =
{
   /* ATC = 1/010 */ { MAXFRQ , { 0x9C , 0xA0 } }
};

static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_AUX_LOCK_N3DB[] =
{
   /* ATC = 0/010 */ { MAXFRQ , { 0x9C , 0x20 } }
};

static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_AUX_SCAN_N6DB[] =
{
   /* ATC = 1/011 */ { MAXFRQ , { 0x9C , 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_AUX_LOCK_N6DB[] =
{
   /* ATC = 0/011 */ { MAXFRQ , { 0x9C , 0x30 } }
};

static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_AUX_SCAN_N12DB[] =
{
   /* ATC = 1/101 */ { MAXFRQ , { 0x9C , 0xD0 } }
};

static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_AUX_LOCK_N12DB[] =
{
   /* ATC = 0/101 */ { MAXFRQ , { 0x9C , 0x50 } }
};


static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_BS_7_166[] =
{
   /* R = 10 */
   /* Fosc     - 180 MHz: CP/T = 0/111, SP = 00001 */ { 144000 , { 0xBC , 0x01 } },
   /* Fosc 180 - 197 MHz: CP/T = 1/110, SP = 00001 */ { 161000 , { 0xF4 , 0x01 } },
   /* Fosc 197 - 366 MHz: CP/T = 0/111, SP = 00010 */ { 330000 , { 0xBC , 0x02 } },
   /* Fosc 366 - 484 MHz: CP/T = 1/110, SP = 00010 */ { 448000 , { 0xF4,  0x02 } },
   /* Fosc 484 - 646 MHz: CP/T = 0/111, SP = 00100 */ { 610000 , { 0xBE , 0x04 } },
   /* Fosc 646 - 790 MHz: CP/T = 1/110, SP = 00100 */ { 754000 , { 0xFC , 0x04 } },
   /* Fosc 790 - 861 MHz: CP/T = 1/111, SP = 00100 */ { MAXFRQ , { 0xF4 , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_BS_8_166[] =
{
   /* R = 10 */
   /* Fosc     - 180 MHz: CP/T = 0/111, SP = 01001 */ { 144000 , { 0xBC , 0x09 } },
   /* Fosc 180 - 197 MHz: CP/T = 1/110, SP = 01001 */ { 161000 , { 0xF4 , 0x09 } },
   /* Fosc 197 - 366 MHz: CP/T = 0/111, SP = 01010 */ { 330000 , { 0xBC , 0x0A } },
   /* Fosc 366 - 484 MHz: CP/T = 1/110, SP = 01010 */ { 448000 , { 0xF4 , 0x0A } },
   /* Fosc 484 - 646 MHz: CP/T = 0/111, SP = 01100 */ { 610000 , { 0xBC , 0x0C } },
   /* Fosc 646 - 790 MHz: CP/T = 1/110, SP = 01100 */ { 754000 , { 0xF4 , 0x0C } },
   /* Fosc 790 - 861 MHz: CP/T = 1/111, SP = 01100 */ { MAXFRQ , { 0xFC , 0x0C } }
};

static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_BS_7_625[] =
{
   /* R = 11 */
   /* Fosc     - 180 MHz: CP/T = 0/111, SP = 00001 */ { 144000 , { 0xBE , 0x01 } },
   /* Fosc 180 - 197 MHz: CP/T = 1/110, SP = 00001 */ { 161000 , { 0xF6 , 0x01 } },
   /* Fosc 197 - 366 MHz: CP/T = 0/111, SP = 00010 */ { 330000 , { 0xBE , 0x02 } },
   /* Fosc 366 - 484 MHz: CP/T = 1/110, SP = 00010 */ { 448000 , { 0xF6 , 0x02 } },
   /* Fosc 484 - 646 MHz: CP/T = 0/111, SP = 00100 */ { 610000 , { 0xBE , 0x04 } },
   /* Fosc 646 - 790 MHz: CP/T = 1/110, SP = 00100 */ { 754000 , { 0xF6 , 0x04 } },
   /* Fosc 790 - 861 MHz: CP/T = 1/111, SP = 00100 */ { MAXFRQ , { 0xFE , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_ChinaDragonCDTSFT33840_BS_8_625[] =
{
   /* R = 11 */
   /* Fosc     - 180 MHz: CP/T = 0/111, SP = 01001 */ { 144000 , { 0xBE , 0x09 } },
   /* Fosc 180 - 197 MHz: CP/T = 1/110, SP = 01001 */ { 161000 , { 0xF6 , 0x09 } },
   /* Fosc 197 - 366 MHz: CP/T = 0/111, SP = 01010 */ { 330000 , { 0xBE , 0x0A } },
   /* Fosc 366 - 484 MHz: CP/T = 1/110, SP = 01010 */ { 448000 , { 0xF6 , 0x0A } },
   /* Fosc 484 - 646 MHz: CP/T = 0/111, SP = 01100 */ { 610000 , { 0xBE , 0x0C } },
   /* Fosc 646 - 790 MHz: CP/T = 1/110, SP = 01100 */ { 754000 , { 0xF6 , 0x0C } },
   /* Fosc 790 - 861 MHz: CP/T = 1/111, SP = 01100 */ { MAXFRQ , { 0xFE , 0x0C } }
};



static TUNERCANNEDControlTable_t TCT_ChinaDragonCDTSFT33840[] =
{

   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0 |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext. AGC",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_AUX_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2 |
                                     TUNER_MODE_SUB3,
      /* description              */ "TOP -3dB, SCAN",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_AUX_SCAN_N3DB
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2 |
                                     TUNER_MODE_SUB3,
      /* description              */ "TOP -3dB, LOCK",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_AUX_LOCK_N3DB
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB4 |
                                     TUNER_MODE_SUB5,
      /* description              */ "TOP -6dB, SCAN",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_AUX_SCAN_N6DB
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB4 |
                                     TUNER_MODE_SUB5,
      /* description              */ "TOP -6dB, LOCK",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_AUX_LOCK_N6DB
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB6 |
                                     TUNER_MODE_SUB7,
      /* description              */ "TOP -12dB, SCAN",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_AUX_SCAN_N12DB
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB6 |
                                     TUNER_MODE_SUB7,
      /* description              */ "TOP -12dB, LOCK",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_AUX_LOCK_N12DB
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0 |
                                     TUNER_MODE_SUB2 |
                                     TUNER_MODE_SUB4 |
                                     TUNER_MODE_SUB6,
      /* description              */ "8MHz, 166.7KHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_BS_8_166
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0 |
                                     TUNER_MODE_SUB2 |
                                     TUNER_MODE_SUB4 |
                                     TUNER_MODE_SUB6,
      /* description              */ "7MHz, 166.7KHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_BS_7_166
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1 |
                                     TUNER_MODE_SUB3 |
                                     TUNER_MODE_SUB5 |
                                     TUNER_MODE_SUB7,
      /* description              */ "8MHz, 62.5KHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_BS_8_625
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1 |
                                     TUNER_MODE_SUB3 |
                                     TUNER_MODE_SUB5 |
                                     TUNER_MODE_SUB7,
      /* description              */ "7MHz, 62.5KHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_ChinaDragonCDTSFT33840_BS_7_625
   }
};

static TUNERSubMode_t subModesChinaDragonCDTSFT33840[] =
{
   "Ext. RF-AGC, 166.7kHz", /* Submode 0 */
   "Ext. RF-AGC,  62.5kHz", /* Submode 1 */
   "TOP  -3dB,   166.7kHz", /* Submode 2 */
   "TOP  -3dB,    62.5kHz", /* Submode 3 */
   "TOP  -6dB,   166.7kHz", /* Submode 4 */
   "TOP  -6dB,    62.5kHz", /* Submode 5 */
   "TOP -12dB,   166.7kHz", /* Submode 6 */
   "TOP -12dB,    62.5kHz"  /* Submode 7 */
};

static TUNERCANNEDData_t dataChinaDragonCDTSFT33840 =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_ChinaDragonCDTSFT33840),
   /* nrOfControlBytes  */          2,
};



static TUNERCommonAttr_t commonAttrChinaDragonCDTSFT33840 =
{
   /* name            */  "China Dragon CDT-SFT338-40",
   /* minFreqRF [kHz] */      49000,
   /* maxFreqRF [kHz] */     861000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesChinaDragonCDTSFT33840),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};
static TUNERInstance_t tunerChinaDragonCDTSFT33840 =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrChinaDragonCDTSFT33840,
   /* myExtAttr    */ &dataChinaDragonCDTSFT33840,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif

/*----------------------------------------------------------------------------*/
/*----end-China-Dragon-CDT-SFT338-40------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Earda EDT-3021II2A tuner configuration                                     */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_EARDA_EDT3021II2A
#define TUNER_EARDA_EDT3021II2A
#endif
#endif

#ifdef  TUNER_EARDA_EDT3021II2A
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerEardaEDT3021II2A
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_EardaEDT3021II2A_AUX_166_67_EXT[] =
{
   /* R1 / R0 = 10, ATC = 0, AL = 110 */
   /*     - 442 MHz: CP/T = 1011,  */ { 446000 , { 0x9C , 0x60 } },
   /* 450 - 754 MHz: CP/T = 0011,  */ { 758000 , { 0xDC , 0x60 } },
   /* 652 - 864 MHz: CP/T = 1011,  */ { MAXFRQ , { 0x9C , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEDT3021II2A_AUX_62_5_EXT[] =
{
   /* R1 / R0 = 11, ATC = 0, AL = 110 */
   /*     -     MHz: CP/T = 0011,  */ { MAXFRQ , { 0x9E , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEDT3021II2A_BS_166_67_7MHZ[] =
{
   /* R1 / R0 = 10 */
   /*     - 158 MHz: CP/T = 1110, P = 0001 */ { 162000 , { 0xF4 , 0x01 } },
   /* 166 - 442 MHz: CP/T = 1110, P = 0010 */ { 446000 , { 0xF4 , 0x02 } },
   /* 450 - 754 MHz: CP/T = 0111, P = 0100 */ { 758000 , { 0xBC , 0x04 } },
   /* 762 - 769 MHz: CP/T = 1110, P = 0100 */ { MAXFRQ , { 0xF4 , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEDT3021II2A_BS_166_67_8MHZ[] =
{
   /* R1 / R0 = 10 */
   /*     - 158 MHz: CP/T = 1110, P = 1001 */ { 162000 , { 0xF4 , 0x09 } },
   /* 166 - 442 MHz: CP/T = 1110, P = 1010 */ { 446000 , { 0xF4 , 0x0A } },
   /* 450 - 754 MHz: CP/T = 0111, P = 1100 */ { 758000 , { 0xBC , 0x0C } },
   /* 762 - 769 MHz: CP/T = 1110, P = 1100 */ { MAXFRQ , { 0xF4 , 0x0C } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEDT3021II2A_BS_62_5_7MHZ[] =
{
   /* R1 / R0 = 11 */
   /*     - 159 MHz: CP/T = 0111, P = 0001 */ { 160000 , { 0xBE , 0x01 } },
   /* 162 - 444 MHz: CP/T = 0111, P = 0010 */ { 446000 , { 0xBE , 0x02 } },
   /* 448 -     MHz: CP/T = 0111, P = 0100 */ { MAXFRQ , { 0xBE , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEDT3021II2A_BS_62_5_8MHZ[] =
{
   /* R1 / R0 = 11 */
   /*     - 159 MHz: CP/T = 0111, P = 1001 */ { 160000 , { 0xBE , 0x09 } },
   /* 162 - 444 MHz: CP/T = 0111, P = 1010 */ { 446000 , { 0xBE , 0x0A } },
   /* 448 -     MHz: CP/T = 0111, P = 1100 */ { MAXFRQ , { 0xBE , 0x0C } }
};

static TUNERCANNEDControlTable_t TCT_EardaEDT3021II2A[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Aux, 166.67, External RF-AGC ",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_EardaEDT3021II2A_AUX_166_67_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "BS, 166.67",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_EardaEDT3021II2A_BS_166_67_7MHZ
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "BS, 166.67",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_EardaEDT3021II2A_BS_166_67_8MHZ
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Aux, 62.5, External RF-AGC",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_EardaEDT3021II2A_AUX_62_5_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "BS, 62.5",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_EardaEDT3021II2A_BS_62_5_7MHZ
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "BS, 62.5",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_EardaEDT3021II2A_BS_62_5_8MHZ
   }
};

static TUNERSubMode_t subModesEardaEDT3021II2A[] =
{
   "Ext. RF-AGC, 166.7kHz",   /* Submode 0 */
   "Ext. RF-AGC,  62.5kHz",   /* Submode 1 */
};

static TUNERCANNEDData_t dataEardaEDT3021II2A =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_EardaEDT3021II2A),
   /* nrOfControlBytes  */          2,
};

static TUNERCommonAttr_t commonAttrEardaEDT3021II2A =
{
   /* name            */  "Earda EDT-3021II2A",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesEardaEDT3021II2A),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerEardaEDT3021II2A =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrEardaEDT3021II2A,
   /* myExtAttr    */ &dataEardaEDT3021II2A,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-Earda-EDT-3021II2A--------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Earda EHU-100x1 tuner configuration                                        */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_EARDA_EHU100X1
#define TUNER_EARDA_EHU100X1
#endif
#endif

#ifdef  TUNER_EARDA_EHU100X1
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerEardaEHU100x1
#endif

#include "tunercanned.h"
static TUNERCANNEDControlUnit_t TCU_EardaEHU100x1_166_67_115_SWITCH[] =
{
   /* programming: CB-BB-CB-AB */
   /* CP/T(1) = 0111 */
   /* CP/T(2) = 0011 */
   /* RS = 10, OS = 0, XTB = 0, ATC = 1, AL = 001 ,LP = 0 */
   /*     - 159 MHz: P = 00001,  */ { 162000 , { 0xBC , 0x01 , 0x9C, 0x90 } },
   /* 162 - 444 MHz: P = 00010,  */ { 448000 , { 0xBC , 0x02 , 0x9C, 0x90 } },
   /* 448 - 862 MHz: P = 00100,  */ { MAXFRQ , { 0xBC , 0x04 , 0x9C, 0x90 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEHU100x1_166_8_115_SWITCH[] =
{
   /* programming: CB-BB-CB-AB */
   /* CP/T(1) = 0111 */
   /* CP/T(2) = 0011 */
   /* RS = 10, OS = 0, XTB = 0, ATC = 1, AL = 001 ,LP = 0 */
   /*     - 159 MHz: P = 01001,  */ { 162000 , { 0xBC , 0x09 , 0x9C, 0x90 } },
   /* 162 - 444 MHz: P = 01010,  */ { 448000 , { 0xBC , 0x0A , 0x9C, 0x90 } },
   /* 448 - 862 MHz: P = 01100,  */ { MAXFRQ , { 0xBC , 0x0C , 0x9C, 0x90 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEHU100x1_625_67_115_SWITCH[] =
{
   /* programming: CB-BB-CB-AB */
   /* CP/T(1) = 0111 */
   /* CP/T(2) = 0011 */
   /* RS = 11, OS = 0, XTB = 0, ATC = 1, AL = 001 ,LP = 0 */
   /*     - 159 MHz: P = 00001,  */ { 162000 , { 0xBE , 0x01 , 0x9E, 0x90 } },
   /* 162 - 444 MHz: P = 00010,  */ { 448000 , { 0xBE , 0x02 , 0x9E, 0x90 } },
   /* 448 - 862 MHz: P = 00100,  */ { MAXFRQ , { 0xBE , 0x04 , 0x9E, 0x90 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEHU100x1_625_8_115_SWITCH[] =
{
   /* programming: CB-BB-CB-AB */
   /* CP/T(1) = 0111 */
   /* CP/T(2) = 0011 */
   /* RS = 11, OS = 0, XTB = 0, ATC = 1, AL = 001 ,LP = 0 */
   /*     - 159 MHz: P = 01001,  */ { 162000 , { 0xBE , 0x09 , 0x9E, 0x90 } },
   /* 162 - 444 MHz: P = 01010,  */ { 448000 , { 0xBE , 0x0A , 0x9E, 0x90 } },
   /* 448 - 862 MHz: P = 01100,  */ { MAXFRQ , { 0xBE , 0x0C , 0x9E, 0x90 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEHU100x1_166_67_115_LOCK[] =
{
   /* programming: CB-BB-CB-AB */
   /* CP/T(1) = 0111 */
   /* CP/T(2) = 0011 */
   /* RS = 10, OS = 0, XTB = 0, ATC = 0, AL = 001 ,LP = 0 */
   /*     - 159 MHz: P = 00001,  */ { 162000 , { 0xBC , 0x01 , 0x9C, 0x10 } },
   /* 162 - 444 MHz: P = 00010,  */ { 448000 , { 0xBC , 0x02 , 0x9C, 0x10 } },
   /* 448 - 862 MHz: P = 00100,  */ { MAXFRQ , { 0xBC , 0x04 , 0x9C, 0x10 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEHU100x1_166_8_115_LOCK[] =
{
   /* programming: CB-BB-CB-AB */
   /* CP/T(1) = 0111 */
   /* CP/T(2) = 0011 */
   /* RS = 10, OS = 0, XTB = 0, ATC = 1, AL = 001 ,LP = 0 */
   /*     - 159 MHz: P = 01001,  */ { 162000 , { 0xBC , 0x09 , 0x9C, 0x10 } },
   /* 162 - 444 MHz: P = 01010,  */ { 448000 , { 0xBC , 0x0A , 0x9C, 0x10 } },
   /* 448 - 862 MHz: P = 01100,  */ { MAXFRQ , { 0xBC , 0x0C , 0x9C, 0x10 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEHU100x1_625_67_115_LOCK[] =
{
   /* programming: CB-BB-CB-AB */
   /* CP/T(1) = 0111 */
   /* CP/T(2) = 0011 */
   /* RS = 11, OS = 0, XTB = 0, ATC = 0, AL = 001 ,LP = 0 */
   /*     - 159 MHz: P = 00001,  */ { 162000 , { 0xBE , 0x01 , 0x9E, 0x10 } },
   /* 162 - 444 MHz: P = 00010,  */ { 448000 , { 0xBE , 0x02 , 0x9E, 0x10 } },
   /* 448 - 862 MHz: P = 00100,  */ { MAXFRQ , { 0xBE , 0x04 , 0x9E, 0x10 } }
};

static TUNERCANNEDControlUnit_t TCU_EardaEHU100x1_625_8_115_LOCK[] =
{
   /* programming: CB-BB-CB-AB */
   /* CP/T(1) = 0111 */
   /* CP/T(2) = 0011 */
   /* RS = 11, OS = 0, XTB = 0, ATC = 0, AL = 001 ,LP = 0 */
   /*     - 159 MHz: P = 01001,  */ { 162000 , { 0xBE , 0x09 , 0x9E, 0x10 } },
   /* 162 - 444 MHz: P = 01010,  */ { 448000 , { 0xBE , 0x0A , 0x9E, 0x10 } },
   /* 448 - 862 MHz: P = 01100,  */ { MAXFRQ , { 0xBE , 0x0C , 0x9E, 0x10 } }
};

static TUNERCANNEDControlTable_t TCT_EardaEHU100x1[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH|
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.67, TOP 115dBuV",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_EardaEHU100x1_166_67_115_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.67, TOP 115dBuV",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_EardaEHU100x1_166_8_115_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5, TOP 115dBuV",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_EardaEHU100x1_625_67_115_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5, TOP 115dBuV",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_EardaEHU100x1_625_8_115_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.67, TOP 115dBuV",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_EardaEHU100x1_166_67_115_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.67, TOP 115dBuV",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_EardaEHU100x1_166_8_115_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5, TOP 115dBuV",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_EardaEHU100x1_625_67_115_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5, TOP 115dBuV",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_EardaEHU100x1_625_8_115_LOCK
   }
};

static TUNERSubMode_t subModesEardaEHU100x1[] =
{
   "TOP 115dBuV, 166.7kHz",   /* Submode 0 */
   "TOP 115dBuV,  62.5kHz",   /* Submode 1 */
};

static TUNERCANNEDData_t dataEardaEHU100x1 =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_EardaEHU100x1),
   /* nrOfControlBytes  */          4,
};

static TUNERCommonAttr_t commonAttrEardaEHU100x1 =
{
   /* name            */  "Earda EHU-100x1",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     862000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesEardaEHU100x1),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerEardaEHU100x1 =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrEardaEHU100x1,
   /* myExtAttr    */ &dataEardaEHU100x1,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-Earda-EHU-100x1----------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Entropic EN4020 configuration                                             */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_ENTROPIC_EN4020
#define TUNER_ENTROPIC_EN4020
#endif
#endif

#ifdef  TUNER_ENTROPIC_EN4020
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerEntropicEN4020
#endif

#include "tuneren4020.h"

static TUNEREN4020Data_t dataEntropicEN4020 =
{
   {
      ENTR_b2wAddress_C0,                                /* b2wAddress        */
      0,                                                 /* FchanHz           */
      0,                                                 /* FchanErrHz        */
      ENTR_ltaEnaSel_off,                                /* ltaEnaSel         */
      ENTR_broadcastMedium_ota,                          /* broadcastMedium   */
      ENTR_rfAttMode_seriesPin_shuntPin_internalAtt,     /* rfAttMode         */
      ENTR_rfAttBias_1p8v,                               /* rfAttBias         */
      ENTR_specInvSel_on,                               /* specInvSel        */
      ENTR_ifBw_8MHz,                                    /* ifBw              */
      0,                                                 /* ifBwUserHz        */
      NULL,                                              /* *ifBwUserFIR      */
      36000000,                                          /* FifHz             */
      0,                                                 /* FifErrHz          */
      ENTR_ifOutputSel_1,                                /* ifOutputSel       */
      ENTR_ifOutPutLevel_Auto,                           /* ifOutPutLevel_IF1 */
      ENTR_ifOutPutLevel_Auto,                           /* ifOutPutLevel_IF2 */
      ENTR_ifDacMode_Optimize,                           /* ifDacMode_IF1     */
      ENTR_ifDacMode_Optimize,                           /* ifDacMode_IF2     */
      ENTR_agcCtrlMode_demod,                            /* agcCtrlMode       */
      ENTR_modulation_DVB_C,                             /* modulation        */
      ENTR_tunerCnt_single,                              /* tunerCnt          */
      ENTR_rxPwr_auto,                                   /* rxPwr             */
      ENTR_lnaModeStatus_ota,                            /* lnaModeStatus     */
      0,                                                 /* RSSI              */
      {0}                                                /* tunerData         */
   }
};

static TUNERSubMode_t subModesEntropicEN4020[] =
{
   "Demod IF Gain Control",   /* Submode 0 */
   "Tuner IF Gain Control"    /* Submode 1 */
};

static TUNERCommonAttr_t commonAttrEntropicEN4020 =
{
   /* name            */  "Entropic EN4020",
   /* minFreqRF [kHz] */      44000,
   /* maxFreqRF [kHz] */    1100000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF( subModesEntropicEN4020 ),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerEntropicEN4020 =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrEntropicEN4020,
   /* myExtAttr    */ &dataEntropicEN4020,
   /* myFunct      */ &TUNEREN4020Functions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Entropic-EN4020-----------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Golden Dragon DTT-5K6 configuration                                        */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_GOLDEN_DRAGON_DTT5K6_K
#define TUNER_GOLDEN_DRAGON_DTT5K6_K
#endif
#endif

#ifdef  TUNER_GOLDEN_DRAGON_DTT5K6_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerGoldenDragonDTT5K6_K
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_GoldenDragonDTT5K6_K_DIV[] =
{
   /*     - 568 MHz: CP = 011 , BS = 01100                      */ { 568000 , { 0xC2 , 0x6C } },
   /* 568 - 640 MHz: CP = 100 , BS = 01100                      */ { 640000 , { 0xC2 , 0x8C } },
   /* 640 - 716 MHz: CP = 101 , BS = 01100                      */ { 716000 , { 0xC2 , 0xAC } },
   /* 716 - 832 MHz: CP = 110 , BS = 01100                      */ { 832000 , { 0xC2 , 0xCC } },
   /* 832 - 894 MHz: CP = 111 , BS = 01100                      */ { MAXFRQ , { 0xC2 , 0xEC } }
};

static TUNERCANNEDControlUnit_t TCU_GoldenDragonDTT5K6_K_AUX_SCAN2[] =
{
   /*     - 568 MHz: CP = 011 , BS = 01100 , ATC = 1 , AL = 010 */ { 568000 , { 0x8A , 0x6C } },
   /* 568 - 640 MHz: CP = 100 , BS = 01100 , ATC = 1 , AL = 010 */ { 640000 , { 0x8A , 0x8C } },
   /* 640 - 716 MHz: CP = 101 , BS = 01100 , ATC = 1 , AL = 010 */ { 716000 , { 0x8A , 0xAC } },
   /* 716 - 832 MHz: CP = 110 , BS = 01100 , ATC = 1 , AL = 010 */ { 832000 , { 0x8A , 0xCC } },
   /* 832 - 894 MHz: CP = 111 , BS = 01100 , ATC = 1 , AL = 010 */ { MAXFRQ , { 0x8A , 0xEC } }
};

static TUNERCANNEDControlUnit_t TCU_GoldenDragonDTT5K6_K_AUX_LOCK2[] =
{
   /*     - 568 MHz: CP = 011 , BS = 01100 , ATC = 0 , AL = 010 */ { 568000 , { 0x82 , 0x6C } },
   /* 568 - 640 MHz: CP = 100 , BS = 01100 , ATC = 0 , AL = 010 */ { 640000 , { 0x82 , 0x8C } },
   /* 640 - 716 MHz: CP = 101 , BS = 01100 , ATC = 0 , AL = 010 */ { 716000 , { 0x82 , 0xAC } },
   /* 716 - 832 MHz: CP = 110 , BS = 01100 , ATC = 0 , AL = 010 */ { 832000 , { 0x82 , 0xCC } },
   /* 832 - 894 MHz: CP = 111 , BS = 01100 , ATC = 0 , AL = 010 */ { MAXFRQ , { 0x82 , 0xEC } }
};

static TUNERCANNEDControlUnit_t TCU_GoldenDragonDTT5K6_K_AUX_SCAN1[] =
{
   /*     - 568 MHz: CP = 011 , BS = 01100 , ATC = 1 , AL = 011 */ { 568000 , { 0x8B , 0x6C } },
   /* 568 - 640 MHz: CP = 100 , BS = 01100 , ATC = 1 , AL = 011 */ { 640000 , { 0x8B , 0x8C } },
   /* 640 - 716 MHz: CP = 101 , BS = 01100 , ATC = 1 , AL = 011 */ { 716000 , { 0x8B , 0xAC } },
   /* 716 - 832 MHz: CP = 110 , BS = 01100 , ATC = 1 , AL = 011 */ { 832000 , { 0x8B , 0xCC } },
   /* 832 - 894 MHz: CP = 111 , BS = 01100 , ATC = 1 , AL = 011 */ { MAXFRQ , { 0x8B , 0xEC } }
};

static TUNERCANNEDControlUnit_t TCU_GoldenDragonDTT5K6_K_AUX_LOCK1[] =
{
   /*     - 568 MHz: CP = 011 , BS = 01100 , ATC = 0 , AL = 011 */ { 568000 , { 0x83 , 0x6C } },
   /* 568 - 640 MHz: CP = 100 , BS = 01100 , ATC = 0 , AL = 011 */ { 640000 , { 0x83 , 0x8C } },
   /* 640 - 716 MHz: CP = 101 , BS = 01100 , ATC = 0 , AL = 011 */ { 716000 , { 0x83 , 0xAC } },
   /* 716 - 832 MHz: CP = 110 , BS = 01100 , ATC = 0 , AL = 011 */ { 832000 , { 0x83 , 0xCC } },
   /* 832 - 894 MHz: CP = 111 , BS = 01100 , ATC = 0 , AL = 011 */ { MAXFRQ , { 0x83 , 0xEC } }
};

static TUNERCANNEDControlUnit_t TCU_GoldenDragonDTT5K6_K_AUX_EXT[] =
{
   /*     - 568 MHz: CP = 011 , BS = 01100 , ATC = 0 , AL = 110 */ { 568000 , { 0x86 , 0x6C } },
   /* 568 - 640 MHz: CP = 100 , BS = 01100 , ATC = 0 , AL = 110 */ { 640000 , { 0x86 , 0x8C } },
   /* 640 - 716 MHz: CP = 101 , BS = 01100 , ATC = 0 , AL = 110 */ { 716000 , { 0x86 , 0xAC } },
   /* 716 - 832 MHz: CP = 110 , BS = 01100 , ATC = 0 , AL = 110 */ { 832000 , { 0x86 , 0xCC } },
   /* 832 - 894 MHz: CP = 111 , BS = 01100 , ATC = 0 , AL = 110 */ { MAXFRQ , { 0x86 , 0xEC } }
};

static TUNERCANNEDControlUnit_t TCU_GoldenDragonDTT5K6_K_AUX_SCAN3[] =
{
   /*     - 568 MHz: CP = 011 , BS = 01100 , ATC = 1 , AL = 010 */ { 568000 , { 0x89 , 0x6C } },
   /* 568 - 640 MHz: CP = 100 , BS = 01100 , ATC = 1 , AL = 010 */ { 640000 , { 0x89 , 0x8C } },
   /* 640 - 716 MHz: CP = 101 , BS = 01100 , ATC = 1 , AL = 010 */ { 716000 , { 0x89 , 0xAC } },
   /* 716 - 832 MHz: CP = 110 , BS = 01100 , ATC = 1 , AL = 010 */ { 832000 , { 0x89 , 0xCC } },
   /* 832 - 894 MHz: CP = 111 , BS = 01100 , ATC = 1 , AL = 010 */ { MAXFRQ , { 0x89 , 0xEC } }
};

static TUNERCANNEDControlUnit_t TCU_GoldenDragonDTT5K6_K_AUX_LOCK3[] =
{
   /*     - 568 MHz: CP = 011 , BS = 01100 , ATC = 0 , AL = 010 */ { 568000 , { 0x81 , 0x6C } },
   /* 568 - 640 MHz: CP = 100 , BS = 01100 , ATC = 0 , AL = 010 */ { 640000 , { 0x81 , 0x8C } },
   /* 640 - 716 MHz: CP = 101 , BS = 01100 , ATC = 0 , AL = 010 */ { 716000 , { 0x81 , 0xAC } },
   /* 716 - 832 MHz: CP = 110 , BS = 01100 , ATC = 0 , AL = 010 */ { 832000 , { 0x81 , 0xCC } },
   /* 832 - 894 MHz: CP = 111 , BS = 01100 , ATC = 0 , AL = 010 */ { MAXFRQ , { 0x81 , 0xEC } }
};

static TUNERCANNEDControlUnit_t TCU_GoldenDragonDTT5K6_K_AUX_SCAN4[] =
{
   /*     - 568 MHz: CP = 011 , BS = 01100 , ATC = 1 , AL = 010 */ { 568000 , { 0x88 , 0x6C } },
   /* 568 - 640 MHz: CP = 100 , BS = 01100 , ATC = 1 , AL = 010 */ { 640000 , { 0x88 , 0x8C } },
   /* 640 - 716 MHz: CP = 101 , BS = 01100 , ATC = 1 , AL = 010 */ { 716000 , { 0x88 , 0xAC } },
   /* 716 - 832 MHz: CP = 110 , BS = 01100 , ATC = 1 , AL = 010 */ { 832000 , { 0x88 , 0xCC } },
   /* 832 - 894 MHz: CP = 111 , BS = 01100 , ATC = 1 , AL = 010 */ { MAXFRQ , { 0x88 , 0xEC } }
};

static TUNERCANNEDControlUnit_t TCU_GoldenDragonDTT5K6_K_AUX_LOCK4[] =
{
   /*     - 568 MHz: CP = 011 , BS = 01100 , ATC = 0 , AL = 010 */ { 568000 , { 0x80 , 0x6C } },
   /* 568 - 640 MHz: CP = 100 , BS = 01100 , ATC = 0 , AL = 010 */ { 640000 , { 0x80 , 0x8C } },
   /* 640 - 716 MHz: CP = 101 , BS = 01100 , ATC = 0 , AL = 010 */ { 716000 , { 0x80 , 0xAC } },
   /* 716 - 832 MHz: CP = 110 , BS = 01100 , ATC = 0 , AL = 010 */ { 832000 , { 0x80 , 0xCC } },
   /* 832 - 894 MHz: CP = 111 , BS = 01100 , ATC = 0 , AL = 010 */ { MAXFRQ , { 0x80 , 0xEC } }
};


static TUNERCANNEDControlTable_t TCT_GoldenDragonDTT5K6_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUBALL,
      /* description              */ "divider",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_GoldenDragonDTT5K6_K_DIV
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "auxiliary/scan",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_GoldenDragonDTT5K6_K_AUX_SCAN1
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "auxiliary/locked",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_GoldenDragonDTT5K6_K_AUX_LOCK1
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "auxiliary/scan",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_GoldenDragonDTT5K6_K_AUX_SCAN2
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "auxiliary/locked",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_GoldenDragonDTT5K6_K_AUX_LOCK2
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "auxiliary/external",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_GoldenDragonDTT5K6_K_AUX_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "auxiliary/scan",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_GoldenDragonDTT5K6_K_AUX_SCAN3
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "auxiliary/locked",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_GoldenDragonDTT5K6_K_AUX_LOCK3
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "auxiliary/scan",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_GoldenDragonDTT5K6_K_AUX_SCAN4
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "auxiliary/locked",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_GoldenDragonDTT5K6_K_AUX_LOCK4
   }
};

static TUNERSubMode_t subModesGoldenDragonDTT5K6_K[] =
{  /* NOTE acc. Philips TDA6650TT spec */
   "External RF-AGC",
   "TOP 115dBuV",
   "TOP 118dBuV",
   "TOP 121dBuV",
   "TOP 124dBuV"
};


static TUNERCANNEDData_t dataGoldenDragonDTT5K6_K =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_GoldenDragonDTT5K6_K),
   /* nrOfControlBytes  */          2
};

static TUNERCommonAttr_t commonAttrGoldenDragonDTT5K6_K =
{
   /* name            */  "Golden Dragon DTT-5K6",
   /* minFreqRF [kHz] */     469000,
   /* maxFreqRF [kHz] */     863000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesGoldenDragonDTT5K6_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36167,
   /* myUserData      */       NULL,
   /* myCapabilities  */  TUNER_WITHOUT_IFAMP
};

static TUNERInstance_t tunerGoldenDragonDTT5K6_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrGoldenDragonDTT5K6_K,
   /* myExtAttr    */ &dataGoldenDragonDTT5K6_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Golden-Dragon-DTT-5K6-U2--------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Jinxin UVS1x05DCW1 configuration                                           */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_JINXIN_UVS1X05DCW1_K
#define TUNER_JINXIN_UVS1X05DCW1_K
#endif
#endif

#ifdef  TUNER_JINXIN_UVS1X05DCW1_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerJinxinUVS1X05DCW1_K
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_JinxinUVS1X05DCW1_K_EXT_166[] =
{  /* R  = 010; CB1 = 0xC2 */
   /* AL = 110; CB3 = 0x86 */
   /* Fosc:                */
   /*     - 144 MHz: CP = 000 , BS = 00001 */ { 108000 , { 0xC2 , 0x01 , 0x86 , 0x01 } },
   /* 144 - 156 MHz: CP = 010 , BS = 00001 */ { 120000 , { 0xC2 , 0x41 , 0x86 , 0x41 } },
   /* 156 - 175 MHz: CP = 100 , BS = 00001 */ { 139000 , { 0xC2 , 0x81 , 0x86 , 0x81 } },
   /* 175 - 180 MHz: CP = 101 , BS = 00001 */ { 144000 , { 0xC2 , 0xA1 , 0x86 , 0xA1 } },
   /* 180 - 184 MHz: CP = 111 , BS = 00001 */ { 148125 , { 0xC2 , 0xE1 , 0x86 , 0xE1 } },
   /* 184 - 224 MHz: CP = 001 , BS = 00010 */ { 188000 , { 0xC2 , 0x22 , 0x86 , 0x22 } },
   /* 224 - 296 MHz: CP = 010 , BS = 00010 */ { 260000 , { 0xC2 , 0x42 , 0x86 , 0x42 } },
   /* 296 - 380 MHz: CP = 011 , BS = 00010 */ { 344000 , { 0xC2 , 0x62 , 0x86 , 0x62 } },
   /* 380 - 404 MHz: CP = 100 , BS = 00010 */ { 368000 , { 0xC2 , 0x82 , 0x86 , 0x82 } },
   /* 404 - 464 MHz: CP = 101 , BS = 00010 */ { 428000 , { 0xC2 , 0xA2 , 0x86 , 0xA2 } },
   /* 464 - 544 MHz: CP = 010 , BS = 01000 */ { 508000 , { 0xC2 , 0x48 , 0x86 , 0x48 } },
   /* 544 - 604 MHz: CP = 011 , BS = 01000 */ { 568000 , { 0xC2 , 0x68 , 0x86 , 0x68 } },
   /* 604 - 800 MHz: CP = 110 , BS = 01000 */ { 764000 , { 0xC2 , 0xC8 , 0x86 , 0xC8 } },
   /* 800 - 872 MHz: CP = 101 , BS = 01000 */ { 836000 , { 0xC2 , 0xB8 , 0x86 , 0xB8 } },
   /* 872 - 883 MHz: CP = 110 , BS = 01000 */ { 847000 , { 0xC2 , 0xC8 , 0x86 , 0xC8 } },
   /* 883 - 894 MHz: CP = 111 , BS = 01000 */ { MAXFRQ , { 0xC2 , 0xE8 , 0x86 , 0xE8 } }
};

static TUNERCANNEDControlUnit_t TCU_JinxinUVS1X05DCW1_K_EXT_50[] =
{  /* R  = 000; CB1 = 0xC0 */
   /* AL = 110; CB3 = 0x86 */
   /* Fosc:                */
   /*     - 144 MHz: CP = 000 , BS = 00001 */ { 108000 , { 0xC0 , 0x01 , 0x86 , 0x01 } },
   /* 144 - 156 MHz: CP = 010 , BS = 00001 */ { 120000 , { 0xC0 , 0x41 , 0x86 , 0x41 } },
   /* 156 - 175 MHz: CP = 100 , BS = 00001 */ { 139000 , { 0xC0 , 0x81 , 0x86 , 0x81 } },
   /* 175 - 180 MHz: CP = 101 , BS = 00001 */ { 144000 , { 0xC0 , 0xA1 , 0x86 , 0xA1 } },
   /* 180 - 184 MHz: CP = 111 , BS = 00001 */ { 148125 , { 0xC0 , 0xE1 , 0x86 , 0xE1 } },
   /* 184 - 224 MHz: CP = 001 , BS = 00010 */ { 188000 , { 0xC0 , 0x22 , 0x86 , 0x22 } },
   /* 224 - 296 MHz: CP = 010 , BS = 00010 */ { 260000 , { 0xC0 , 0x42 , 0x86 , 0x42 } },
   /* 296 - 380 MHz: CP = 011 , BS = 00010 */ { 344000 , { 0xC0 , 0x62 , 0x86 , 0x62 } },
   /* 380 - 404 MHz: CP = 100 , BS = 00010 */ { 368000 , { 0xC0 , 0x82 , 0x86 , 0x82 } },
   /* 404 - 464 MHz: CP = 101 , BS = 00010 */ { 428000 , { 0xC0 , 0xA2 , 0x86 , 0xA2 } },
   /* 464 - 544 MHz: CP = 010 , BS = 01000 */ { 508000 , { 0xC0 , 0x48 , 0x86 , 0x48 } },
   /* 544 - 604 MHz: CP = 011 , BS = 01000 */ { 568000 , { 0xC0 , 0x68 , 0x86 , 0x68 } },
   /* 604 - 800 MHz: CP = 110 , BS = 01000 */ { 764000 , { 0xC0 , 0xC8 , 0x86 , 0xC8 } },
   /* 800 - 872 MHz: CP = 101 , BS = 01000 */ { 836000 , { 0xC0 , 0xB8 , 0x86 , 0xB8 } },
   /* 872 - 883 MHz: CP = 110 , BS = 01000 */ { 847000 , { 0xC0 , 0xC8 , 0x86 , 0xC8 } },
   /* 883 - 894 MHz: CP = 111 , BS = 01000 */ { MAXFRQ , { 0xC0 , 0xE8 , 0x86 , 0xE8 } }
};

static TUNERCANNEDControlTable_t TCT_JinxinUVS1X05DCW1_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext., 166kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_JinxinUVS1X05DCW1_K_EXT_166
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext., 50kHz",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_JinxinUVS1X05DCW1_K_EXT_50
   }
};

static TUNERSubMode_t subModesJinxinUVS1X05DCW1_K[] =
{
   "External RF-AGC"
};

static TUNERCANNEDData_t dataJinxinUVS1X05DCW1_K =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_JinxinUVS1X05DCW1_K),
   /* nrOfControlBytes  */          4
};

static TUNERCommonAttr_t commonAttrJinxinUVS1X05DCW1_K =
{
   /* name            */  "Jinxin UVS1x05DCW1",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     863000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesJinxinUVS1X05DCW1_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */  TUNER_WITHOUT_IFAMP
};

static TUNERInstance_t tunerJinxinUVS1X05DCW1_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrJinxinUVS1X05DCW1_K,
   /* myExtAttr    */ &dataJinxinUVS1X05DCW1_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Jinxin-UVS1X05DCW1--------------------------------------------------*/




/*----------------------------------------------------------------------------*/
/* LG Innotek TDFW-G235D NIM tuner configuration                              */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDFW_G235D_NIM_K
#define TUNER_LGI_TDFW_G235D_NIM_K
#endif
#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDFW_G235D_NIM2_K
#define TUNER_LGI_TDFW_G235D_NIM2_K
#endif
#ifndef TUNER_MIC_LGI_TDFW_G235D_NIM_K
#define TUNER_MIC_LGI_TDFW_G235D_NIM_K
#endif
#endif
#endif

#ifdef  TUNER_LGI_TDFW_G235D_NIM_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerLGInnotekTDFW_G235D_K
#endif
#endif

#ifdef  TUNER_LGI_TDFW_G235D_NIM2_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerLGInnotekTDFW_G235D2_K
#endif
#endif

#ifdef  TUNER_MIC_LGI_TDFW_G235D_NIM_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerMicLGInnotekTDFW_G235D_K
#endif
#endif

#if defined TUNER_LGI_TDFW_G235D_NIM_K || \
    defined TUNER_LGI_TDFW_G235D_NIM2_K || \
    defined TUNER_MIC_LGI_TDFW_G235D_NIM_K

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_LGInnotekTDFW_G235D_CB1BB_EXT_166_K[] =
{
   /* CB1: ATP=111, RS=000 (== 0xB8) , ATC = 0 (== 0xC1)*/
   /*     - 147 MHz: BB: CP=10, P5=0, BS=0001 */ { 147000 , { 0xB8 , 0x81 , 0xC1 } },
   /* 147 - 431 MHz: BB: CP=10, P5=0, BS=0010 */ { 431000 , { 0xB8 , 0x82 , 0xC1 } },
   /* 431 - 670 MHz: BB: CP=10, P5=0, BS=1000 */ { 670000 , { 0xB8 , 0x88 , 0xC1 } },
   /* 670 -     MHz: BB: CP=11, P5=0, BS=1000 */ { MAXFRQ , { 0xB8 , 0xC8 , 0xC1 } }
};

static TUNERCANNEDControlUnit_t TCU_LGInnotekTDFW_G235D_CB1BB_EXT_62_5_K[] =
{
   /* CB1: ATP=111, RS=011 (== 0xBB) ,  ATC = 0 (== 0xC1) */
   /*     - 147 MHz: BB: CP=10, P5=0, BS=0001 */ { 147000 , { 0xBB , 0x81 , 0xC1} },
   /* 147 - 431 MHz: BB: CP=10, P5=0, BS=0010 */ { 431000 , { 0xBB , 0x82 , 0xC1} },
   /* 431 - 670 MHz: BB: CP=10, P5=0, BS=1000 */ { 670000 , { 0xBB , 0x88 , 0xC1} },
   /* 670 -     MHz: BB: CP=11, P5=0, BS=1000 */ { MAXFRQ , { 0xBB , 0xC8 , 0xC1} }
};

static TUNERCANNEDControlUnit_t TCU_LGInnotekTDFW_G235D_CB1BB_EXT_50_K[] =
{
   /* CB1: ATP=111, RS=101 (== 0xBD) , ATC = 0 (== 0xC1) */
   /*     - 147 MHz: BB: CP=00, P5=0, BS=0001 */ { 147000 , { 0xBD , 0x01 ,0xC1 } },
   /* 147 - 431 MHz: BB: CP=00, P5=0, BS=0010 */ { 431000 , { 0xBD , 0x02 ,0xC1 } },
   /* 670 -     MHz: BB: CP=00, P5=0, BS=1000 */ { MAXFRQ , { 0xBD , 0x08 ,0xC1 } }
};


static TUNERCANNEDControlTable_t TCT_LGInnotekTDFW_G235D_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCH  |
                                     TUNER_MODE_678MHZ  |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext RF-AGC, 166kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_LGInnotekTDFW_G235D_CB1BB_EXT_166_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCH  |
                                     TUNER_MODE_678MHZ  |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext RF-AGC, 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_LGInnotekTDFW_G235D_CB1BB_EXT_62_5_K
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Ext RF-AGC, 50kHz",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_LGInnotekTDFW_G235D_CB1BB_EXT_50_K
   }
};

static TUNERSubMode_t subModesLGInnotekTDFW_G235D_K [] =
{
   "External RF-AGC, 166.7kHz", /* SUBMODE 0 */
   "External RF-AGC,  62.5kHz", /* SUBMODE 1 */
   "External RF-AGC,  50  kHz"  /* SUBMODE 2 */
};

static TUNERCANNEDData_t dataLGInnotekTDFW_G235D_K =
{
   /* lockMask         */       0x40,
   /* lockValue        */       0x40,
   /* controlTable     */ ARRAYREF(TCT_LGInnotekTDFW_G235D_K),
   /* nrOfControlBytes */          3
};

#endif   /*    defined TUNER_LGI_TDFW_G235D_NIM_K || \
               defined TUNER_LGI_TDFW_G235D_NIM2_K || \
               defined TUNER_MIC_LGI_TDFW_G235D_NIM_K */

#ifdef  TUNER_LGI_TDFW_G235D_NIM_K
static TUNERCommonAttr_t commonAttrLGInnotekTDFW_G235D_K =
{
   /* name            */  "LG Innotek TDFW-G235D NIM",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesLGInnotekTDFW_G235D_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerLGInnotekTDFW_G235D_K =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrLGInnotekTDFW_G235D_K,
   /* myExtAttr    */ &dataLGInnotekTDFW_G235D_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif

#ifdef  TUNER_LGI_TDFW_G235D_NIM2_K
static TUNERCommonAttr_t commonAttrLGInnotekTDFW_G235D2_K =
{
   /* name            */  "LG Innotek TDFW-G235D NIM2",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesLGInnotekTDFW_G235D_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerLGInnotekTDFW_G235D2_K =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrLGInnotekTDFW_G235D2_K,
   /* myExtAttr    */ &dataLGInnotekTDFW_G235D_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif

#ifdef  TUNER_MIC_LGI_TDFW_G235D_NIM_K
static TUNERCommonAttr_t commonAttrMicLGInnotekTDFW_G235D_K =
{
   /* name            */  "Mic LG Innotek TDFW-G235D NIM",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesLGInnotekTDFW_G235D_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerMicLGInnotekTDFW_G235D_K =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrMicLGInnotekTDFW_G235D_K,
   /* myExtAttr    */ &dataLGInnotekTDFW_G235D_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-LG-Innotek-TDFW-G235D-NIM-------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* LG Innotek TDTC-GxxxD tuner configuration                                  */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDTC_G2XXD_K
#define TUNER_LGI_TDTC_G2XXD_K
#endif
#ifndef TUNER_LGI_TDTC_G3XXD_K
#define TUNER_LGI_TDTC_G3XXD_K
#endif
#ifndef TUNER_LGI_TDTC_G4XXD_K
#define TUNER_LGI_TDTC_G4XXD_K
#endif
#ifndef TUNER_LGI_TDTC_G675D_K
#define TUNER_LGI_TDTC_G675D_K
#endif
#endif /* TUNER_ALL */

#ifdef  TUNER_LGI_TDTC_G2XXD_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerLGInnotekTDTC_G2xxD_K
#endif
#endif /* TUNER_LGI_TDTC_G2XXD_K */

#ifdef  TUNER_LGI_TDTC_G3XXD_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerLGInnotekTDTC_G3xxD_K
#endif
#endif /* TUNER_LGI_TDTC_G3XXD_K */

#ifdef  TUNER_LGI_TDTC_G4XXD_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerLGInnotekTDTC_G3xxD_K
#endif
#endif /* TUNER_LGI_TDTC_G4XXD_K */

#ifdef  TUNER_LGI_TDTC_G675D_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerLGInnotekTDTC_G675D_K
#endif
#endif /* TUNER_LGI_TDTC_G675D_K */

#if defined TUNER_LGI_TDTC_G2XXD_K || \
    defined TUNER_LGI_TDTC_G3XXD_K || \
    defined TUNER_LGI_TDTC_G4XXD_K || \
    defined TUNER_LGI_TDTC_G675D_K

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_LGInnotekTDTC_GxxxD_AGCDSB_7_166_K[] =
{
   /* CB1: ATP=111, RS=000 (== 0xB8) */
   /* CB2: ATC=0, STBY=0, T=0011 (== 0xC3) */
   /*     - 100 MHz: BB: CP=10, P5=0, BS=0001 */ { 100000 , { 0xB8 , 0x81 , 0xC3 } },
   /* 100 - 145 MHz: BB: CP=11, P5=0, BS=0001 */ { 145000 , { 0xB8 , 0xC1 , 0xC3 } },
   /* 145 - 300 MHz: BB: CP=10, P5=0, BS=0010 */ { 300000 , { 0xB8 , 0x82 , 0xC3 } },
   /* 300 - 430 MHz: BB: CP=11, P5=0, BS=0010 */ { 430000 , { 0xB8 , 0xC2 , 0xC3 } },
   /* 430 - 700 MHz: BB: CP=10, P5=0, BS=1000 */ { 700000 , { 0xB8 , 0x88 , 0xC3 } },
   /* 700 -     MHz: BB: CP=10, P5=0, BS=1000 */ { MAXFRQ , { 0xB8 , 0xC8 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_LGInnotekTDTC_GxxxD_AGCDSB_8_166_K[] =
{
   /* CB1: ATP=111, RS=000 (== 0xB8) */
   /* CB2: ATC=0, STBY=0, T=0011 (== 0xC3) */
   /*     - 100 MHz: BB: CP=10, P5=1, BS=0001 */ { 100000 , { 0xB8 , 0x91 , 0xC3 } },
   /* 100 - 145 MHz: BB: CP=11, P5=1, BS=0001 */ { 145000 , { 0xB8 , 0xD1 , 0xC3 } },
   /* 145 - 300 MHz: BB: CP=10, P5=1, BS=0010 */ { 300000 , { 0xB8 , 0x92 , 0xC3 } },
   /* 300 - 430 MHz: BB: CP=11, P5=1, BS=0010 */ { 430000 , { 0xB8 , 0xD2 , 0xC3 } },
   /* 430 - 700 MHz: BB: CP=10, P5=1, BS=1000 */ { 700000 , { 0xB8 , 0x98 , 0xC3 } },
   /* 700 -     MHz: BB: CP=10, P5=1, BS=1000 */ { MAXFRQ , { 0xB8 , 0xD8 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_LGInnotekTDTC_GxxxD_AGCDSB_7_62_5_K[] =
{
   /* CB1: ATP=111, RS=011 (== 0xBB) */
   /* CB2: ATC=0, STBY=0, T=0011 (== 0xC3) */
   /*     - 100 MHz: BB: CP=10, P5=0, BS=0001 */ { 100000 , { 0xBB , 0x81 , 0xC3 } },
   /* 100 - 145 MHz: BB: CP=11, P5=0, BS=0001 */ { 145000 , { 0xBB , 0xC1 , 0xC3 } },
   /* 145 - 300 MHz: BB: CP=10, P5=0, BS=0010 */ { 300000 , { 0xBB , 0x82 , 0xC3 } },
   /* 300 - 430 MHz: BB: CP=11, P5=0, BS=0010 */ { 430000 , { 0xBB , 0xC2 , 0xC3 } },
   /* 430 - 700 MHz: BB: CP=10, P5=0, BS=1000 */ { 700000 , { 0xBB , 0x88 , 0xC3 } },
   /* 700 -     MHz: BB: CP=10, P5=0, BS=1000 */ { MAXFRQ , { 0xBB , 0xC8 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_LGInnotekTDTC_GxxxD_AGCDSB_8_62_5_K[] =
{
   /* CB1: ATP=111, RS=011 (== 0xBB) */
   /* CB2: ATC=0, STBY=0, T=0011 (== 0xC3) */
   /*     - 100 MHz: BB: CP=10, P5=1, BS=0001 */ { 100000 , { 0xBB , 0x91 , 0xC3 } },
   /* 100 - 145 MHz: BB: CP=11, P5=1, BS=0001 */ { 145000 , { 0xBB , 0xD1 , 0xC3 } },
   /* 145 - 300 MHz: BB: CP=10, P5=1, BS=0010 */ { 300000 , { 0xBB , 0x92 , 0xC3 } },
   /* 300 - 430 MHz: BB: CP=11, P5=1, BS=0010 */ { 430000 , { 0xBB , 0xD2 , 0xC3 } },
   /* 430 - 700 MHz: BB: CP=10, P5=1, BS=1000 */ { 700000 , { 0xBB , 0x98 , 0xC3 } },
   /* 700 -     MHz: BB: CP=10, P5=1, BS=1000 */ { MAXFRQ , { 0xBB , 0xD8 , 0xC3 } }
};


static TUNERCANNEDControlTable_t TCT_LGInnotekTDTC_GxxxD_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK  |
                                     TUNER_MODE_67MHZ   |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext RF-AGC, 7MHz, 166kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_LGInnotekTDTC_GxxxD_AGCDSB_7_166_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext RF-AGC, 8MHz, 166kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_LGInnotekTDTC_GxxxD_AGCDSB_8_166_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext RF-AGC, 7MHz, 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_LGInnotekTDTC_GxxxD_AGCDSB_7_62_5_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext RF-AGC, 8MHz, 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_LGInnotekTDTC_GxxxD_AGCDSB_8_62_5_K
   }
};

static TUNERSubMode_t subModesLGInnotekTDTC_GxxxD_K [] =
{
   "External RF-AGC, 166.7kHz", /* SUBMODE 0 */
   "External RF-AGC,  62.5kHz"  /* SUBMODE 1 */
};

static TUNERCANNEDData_t dataLGInnotekTDTC_GxxxD_K =
{
   /* lockMask         */       0x40,
   /* lockValue        */       0x40,
   /* controlTable     */ ARRAYREF(TCT_LGInnotekTDTC_GxxxD_K),
   /* nrOfControlBytes */          3
};

#endif   /* defined TUNER_LGI_TDTC_G2XXD_K || \
            defined TUNER_LGI_TDTC_G3XXD_K || \
            defined TUNER_LGI_TDTC_G4XXD_K  */

#ifdef TUNER_LGI_TDTC_G2XXD_K
static TUNERCommonAttr_t commonAttrLGInnotekTDTC_G2xxD_K =
{
   /* name            */  "LG Innotek TDTC-G2xxD",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesLGInnotekTDTC_GxxxD_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerLGInnotekTDTC_G2xxD_K =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrLGInnotekTDTC_G2xxD_K,
   /* myExtAttr    */ &dataLGInnotekTDTC_GxxxD_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif /* TUNER_LGI_TDTC_G2XXD_K */

#ifdef TUNER_LGI_TDTC_G3XXD_K
static TUNERCommonAttr_t commonAttrLGInnotekTDTC_G3xxD_K =
{
   /* name            */  "LG Innotek TDTC-G3xxD",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesLGInnotekTDTC_GxxxD_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerLGInnotekTDTC_G3xxD_K =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrLGInnotekTDTC_G3xxD_K,
   /* myExtAttr    */ &dataLGInnotekTDTC_GxxxD_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif /* TUNER_LGI_TDTC_G3XXD_K */

#ifdef TUNER_LGI_TDTC_G4XXD_K
static TUNERCommonAttr_t commonAttrLGInnotekTDTC_G4xxD_K =
{
   /* name            */  "LG Innotek TDTC-G4xxD",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesLGInnotekTDTC_GxxxD_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerLGInnotekTDTC_G4xxD_K =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrLGInnotekTDTC_G4xxD_K,
   /* myExtAttr    */ &dataLGInnotekTDTC_GxxxD_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif /* TUNER_LGI_TDTC_G4XXD_K */

#ifdef TUNER_LGI_TDTC_G675D_K
static TUNERCommonAttr_t commonAttrLGInnotekTDTC_G675D_K =
{
   /* name            */  "LG Innotek TDTC-G675D",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesLGInnotekTDTC_GxxxD_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerLGInnotekTDTC_G675D_K =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrLGInnotekTDTC_G675D_K,
   /* myExtAttr    */ &dataLGInnotekTDTC_GxxxD_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif /* TUNER_LGI_TDTC_G4XXD_K */
/*----------------------------------------------------------------------------*/
/*----end-LG-Innotek-TDTC-GxxxD-----------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* LG Innotek G323D configuration                                             */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_LGI_TDTC_G323D
#define TUNER_LGI_TDTC_G323D
#endif
#endif

#ifdef  TUNER_LGI_TDTC_G323D
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerLGInnotekTDTC_G323D
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_LGInnotekTDTC_G323D_EXT_166[] =
{
   /* CB1: ATP=111, RS=000 (== 0xB8) */
   /* BB:  P5 = 1  */
   /* CB2: ATC=0, STBY=0, T=0011, T1/ATSS = 1 for RF-AGC setting */
   /*     - 100 MHz: BB: CP=10, BS=0001 */ { 100000 , { 0xB8 , 0x91 , 0xC3 } },
   /* 100 - 145 MHz: BB: CP=11, BS=0001 */ { 145000 , { 0xB8 , 0xD1 , 0xC3 } },
   /* 145 - 300 MHz: BB: CP=10, BS=0010 */ { 300000 , { 0xB8 , 0x92 , 0xC3 } },
   /* 300 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0xB8 , 0xD2 , 0xC3 } },
   /* 430 - 700 MHz: BB: CP=10, BS=1000 */ { 700000 , { 0xB8 , 0x98 , 0xC3 } },
   /* 700 -     MHz: BB: CP=11, BS=1000 */ { MAXFRQ , { 0xB8 , 0xD8 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_LGInnotekTDTC_G323D_EXT_62_5[] =
{
   /* CB1: ATP=111, RS=011 (== 0xBB) */
   /* BB:  P5 = 1, CP = 00 (70uA) */
   /* CB2: ATC=0, STBY=0, T=0011, T1/ATSS = 1 for RF-AGC setting */

   /* CB2: ATC=0, STBY=0, T=0011 (== 0xC3) */
   /*     - 100 MHz: BB: CP=00, BS=0001 */ { 145000 , { 0xBB , 0x11 , 0xC3 } },
   /* 145 - 300 MHz: BB: CP=00, BS=0010 */ { 430000 , { 0xBB , 0x12 , 0xC3 } },
   /* 430 - 700 MHz: BB: CP=00, BS=1000 */ { MAXFRQ , { 0xBB , 0x18 , 0xC3 } }
};


static TUNERCANNEDControlTable_t TCT_LGInnotekTDTC_G323D[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK  |
                                     TUNER_MODE_678MHZ   |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext RF-AGC, 166kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_LGInnotekTDTC_G323D_EXT_166
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext RF-AGC, 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_LGInnotekTDTC_G323D_EXT_62_5
   }
};

static TUNERSubMode_t subModesLGInnotekTDTC_G323D [] =
{
   "External RF-AGC, 166.7kHz", /* SUBMODE 0 */
   "External RF-AGC,  62.5kHz"  /* SUBMODE 1 */
};

static TUNERCANNEDData_t dataLGInnotekTDTC_G323D =
{
   /* lockMask         */       0x40,
   /* lockValue        */       0x40,
   /* controlTable     */ ARRAYREF(TCT_LGInnotekTDTC_G323D),
   /* nrOfControlBytes */          3
};

static TUNERCommonAttr_t commonAttrLGInnotekTDTC_G323D =
{
   /* name            */  "LG Innotek TDTC-G323D",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesLGInnotekTDTC_G323D),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerLGInnotekTDTC_G323D =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrLGInnotekTDTC_G323D,
   /* myExtAttr    */ &dataLGInnotekTDTC_G323D,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif /* TUNER_LGI_TDTC_G323D */
/*----------------------------------------------------------------------------*/
/*----end-LG-Innotek-TDTC-G323D-----------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Loewe FET08 configuration                                                  */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_LOEWE_FET08
#define TUNER_LOEWE_FET08
#endif
#endif

#ifdef  TUNER_LOEWE_FET08
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerLoeweFET08_tuner1
#endif

#include "tunerloewefet08.h"


static TUNERSubMode_t subModesLoeweFET08 [] =
{
   "166.7kHz", /* SUBMODE 0 */
   " 62.5kHz"  /* SUBMODE 1 */
};

TUNERLoeweFET08Data_t dataLoeweFET08_tuner1 =
{
   /* bandLimit1 */     157000,
   /* bandLimit2 */     442000,
   /* eepromOffset */   0,
   /* calib */          { { 0, 0, 0, 0 }, }
};

static TUNERCommonAttr_t commonAttrLoeweFET08_tuner1 =
{
   /* name            */  "Loewe FET08 tuner 1",
   /* minFreqRF [kHz] */      46000,
   /* maxFreqRF [kHz] */     860000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF( subModesLoeweFET08 ),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerLoeweFET08_tuner1 =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrLoeweFET08_tuner1,
   /* myExtAttr    */ &dataLoeweFET08_tuner1,
   /* myFunct      */ &TUNERLOEWEFET08Functions_g
};

TUNERLoeweFET08Data_t dataLoeweFET08_tuner2 =
{
   /* bandLimit1 */     157000,
   /* bandLimit2 */     442000,
   /* eepromOffset */   512,
   /* calib */          { { 0, 0, 0, 0 }, }
};

static TUNERCommonAttr_t commonAttrLoeweFET08_tuner2 =
{
   /* name            */  "Loewe FET08 tuner 2",
   /* minFreqRF [kHz] */      46000,
   /* maxFreqRF [kHz] */     860000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF( subModesLoeweFET08 ),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerLoeweFET08_tuner2 =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrLoeweFET08_tuner2,
   /* myExtAttr    */ &dataLoeweFET08_tuner2,
   /* myFunct      */ &TUNERLOEWEFET08Functions_g
};

#endif /* TUNER_LOEWE_FET08 */
/*----------------------------------------------------------------------------*/
/*----end-Loewe-FET08---------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Maxim Max3542 configuration                                                */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_MAXIM_MAX3542
#define TUNER_MAXIM_MAX3542
#endif
#endif

#ifdef  TUNER_MAXIM_MAX3542
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerMaximMax3542
#endif

#include "tunermax3542.h"

static TUNERSubMode_t subModesMaximMax3542[] =
{
   "IFOVLD = TRUE",    /* TUNER_MODE_SUB0 */
   "IFOVLD = FALSE"    /* TUNER_MODE_SUB1 */
};

static TUNERCommonAttr_t commonAttrMaximMax3542 =
{
   /* name            */  "Maxim Max3542",
   /* minFreqRF [kHz] */      30000,
   /* maxFreqRF [kHz] */     900000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesMaximMax3542),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */     36000L,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerMaximMax3542 =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrMaximMax3542,
   /* myExtAttr    */ NULL,
   /* myFunct      */ &TUNERMAX3542Functions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-Maxim-Max3542-------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Maxim Max3543 configuration                                                */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_MAXIM_MAX3543
#define TUNER_MAXIM_MAX3543
#endif
#endif

#ifdef  TUNER_MAXIM_MAX3543
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerMaximMax3543
#endif

#include "tunermax3543.h"

static TUNERSubMode_t subModesMaximMax3543[] =
{
   "Differential output"   /* TUNER_MODE_SUB0 */
};

static TUNERCommonAttr_t commonAttrMaximMax3543 =
{
   /* name            */  "Maxim Max3543",
   /* minFreqRF [kHz] */      30000,
   /* maxFreqRF [kHz] */     900000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesMaximMax3543),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */     36125L,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerMaximMax3543 =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrMaximMax3543,
   /* myExtAttr    */ NULL,
   /* myFunct      */ &TUNERMAX3543Functions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-Maxim-Max3543-------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* MaxLinear MXL201RF configuration                                           */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_MAXLINEAR_MXL301RF
#define TUNER_MAXLINEAR_MXL301RF
#endif
#endif

#ifdef  TUNER_MAXLINEAR_MXL301RF
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerMaxLinearMXL301RF
#endif

#include "tunermxl301rf.h"

static TUNERSubMode_t subModesMaxLinearMXL301RF[] =
{
   "If Frequency = 4.57 MHz",    /* TUNER_MODE_SUB0 */
   "If Frequency = 5.38 MHz",    /* TUNER_MODE_SUB1 */
   "If Frequency = 6.00 MHz",    /* TUNER_MODE_SUB2 */
   "If Frequency = 6.28 MHz",    /* TUNER_MODE_SUB3 */
   "If Frequency = 36.15 MHz",   /* TUNER_MODE_SUB4 */
   "If Frequency = 44 MHz"       /* TUNER_MODE_SUB5 */
};

static TUNERCommonAttr_t commonAttrMaxLinearMXL301RF =
{
   /* name            */  "MaxLinear MXL301RF",
   /* minFreqRF [kHz] */      28000,
   /* maxFreqRF [kHz] */     885000,
   /* sub-mode        */          1,
   /* sub-modes       */     ARRAYREF(subModesMaxLinearMXL301RF),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */          0,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERMXL301RFData_t dataMaxLinearMXL301RF =
{
   /* ifFrequency       */    MxL_IF_5_38_MHZ,
   /* xtalFreq          */    MxL_XTAL_24_MHZ,
   /* invertIf          */    1,
   /* clkOutEnable      */    0,
   /* clkOutAmp         */    0,
   /* xtalCap           */    MxL_XTAL_CAP_18_PF
};

static TUNERInstance_t tunerMaxLinearMXL301RF =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrMaxLinearMXL301RF,
   /* myExtAttr    */ &dataMaxLinearMXL301RF,
   /* myFunct      */ &TUNERMXL301RFFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-MaxLinear-MXL301RF---------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* MaxLinear MXL5007 configuration                                            */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_MAXLINEAR_MXL5007
#define TUNER_MAXLINEAR_MXL5007
#endif
#endif

#ifdef  TUNER_MAXLINEAR_MXL5007
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerMaxLinearMXL5007
#endif

#include "tunermxl5007.h"

static TUNERMXL5007Data_t dataMaxLinearMXL5007 =
{
   /* ifFrequency       */    5380000UL,
   /* ifDiffOutLevel    */    -6,         /* -4dBm (used for QAM only) */
   /* xtalFrequency     */    24000000UL,
   /* invertIf          */    0,
   /* clkOutEnable      */    0,
   /* clkOutAmp         */    0
};

static TUNERSubMode_t subModesMaxLinearMXL5007[] =
{
   "IF = 4.57 MHz",    /* TUNER_MODE_SUB0 */
   "IF = 5.38 MHz",    /* TUNER_MODE_SUB1 */
   "IF = 36.15 MHz",   /* TUNER_MODE_SUB2 */
   "IF = 44 MHz"       /* TUNER_MODE_SUB3 */
};

static TUNERCommonAttr_t commonAttrMaxLinearMXL5007 =
{
   /* name            */  "MaxLinear MxL5007",
   /* minFreqRF [kHz] */      28000,
   /* maxFreqRF [kHz] */     885000,
   /* sub-mode        */          1,
   /* sub-modes       */  ARRAYREF(subModesMaxLinearMXL5007),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */   4570000L,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0 /* Only in case of Low-IF */
};

static TUNERInstance_t tunerMaxLinearMXL5007 =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrMaxLinearMXL5007,
   /* myExtAttr    */ &dataMaxLinearMXL5007,
   /* myFunct      */ &TUNERMXL5007Functions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-MaxLinear-MXL5007---------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Microtune MT2063 configuration                                             */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_MICROTUNE_MT2063_K
#define TUNER_MICROTUNE_MT2063_K
#endif
#ifndef TUNER_MICROTUNE_MT2063_TAIWAN
#define TUNER_MICROTUNE_MT2063_TAIWAN
#endif
#endif

#ifdef  TUNER_MICROTUNE_MT2063_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerMicrotuneMT2063_K
#endif
#endif

#ifdef  TUNER_MICROTUNE_MT2063_TAIWAN
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerMicrotuneMT2063Taiwan
#endif
#endif

#if   defined TUNER_MICROTUNE_MT2063_K || \
      defined TUNER_MICROTUNE_MT2063_TAIWAN

#include "tunermt2063.h"


static TUNERSubMode_t subModesMicrotuneMT2063_K[] =
{
   "AMPGC 12dB",  /* TUNER_MODE_SUB0 */
   "AMPGC 18dB",  /* TUNER_MODE_SUB1 */
   "AMPGC 24dB",  /* TUNER_MODE_SUB2 */
   "AMPGC  0ff"   /* TUNER_MODE_SUB3 */
};
#endif

#ifdef  TUNER_MICROTUNE_MT2063_K

static TUNERMT2063Data_t dataMicrotuneMT2063_K =
{
   /* handleTuner    */            NULL,
   /* ifOut          */       36125000L,
   /* VGAGC          */               3
};

static TUNERCommonAttr_t commonAttrMicrotuneMT2063_K =
{
   /* name            */  "Microtune MT2063",
   /* minFreqRF [kHz] */      44000,
   /* maxFreqRF [kHz] */    1100000,
   /* sub-mode        */          1,
   /* sub-modes       */  ARRAYREF( subModesMicrotuneMT2063_K ),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerMicrotuneMT2063_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrMicrotuneMT2063_K,
   /* myExtAttr    */ &dataMicrotuneMT2063_K,
   /* myFunct      */ &TUNERMT2063Functions_g
};
#endif

#ifdef  TUNER_MICROTUNE_MT2063_TAIWAN

static TUNERMT2063Data_t dataMicrotuneMT2063Taiwan =
{
   /* handleTuner    */           NULL,
   /* ifOut          */      44000000L,
   /* VGAGC          */              3
};

static TUNERCommonAttr_t commonAttrMicrotuneMT2063Taiwan =
{
   /* name            */  "Microtune MT2063 Taiwan",
   /* minFreqRF [kHz] */      44000,
   /* maxFreqRF [kHz] */    1100000,
   /* sub-mode        */          1,
   /* sub-modes       */  ARRAYREF( subModesMicrotuneMT2063_K ),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      44000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerMicrotuneMT2063Taiwan =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrMicrotuneMT2063Taiwan,
   /* myExtAttr    */ &dataMicrotuneMT2063Taiwan,
   /* myFunct      */ &TUNERMT2063Functions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-Microtune-MT2063----------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Microtune MT2131 configuration                                             */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_MICROTUNE_MT2131_K
#define TUNER_MICROTUNE_MT2131_K
#endif
#endif

#ifdef  TUNER_MICROTUNE_MT2131_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerMicrotuneMT2131_K
#endif

#include "tunermt2131.h"

static TUNERMT2131Data_t dataMicrotuneMT2131_K =
{
   /* handleTuner    */            NULL,
   /* ifOut          */       36125000L,
   /* VGA mode       */ MT2131_VGA_AUTO,
   /* VGA current    */               3,
   /* SRO enable     */ MT2131_EXT_SRO_OFF
};

static TUNERSubMode_t subModesMicrotuneMT2131_K[] =
{
   "Internal RF-AGC"  /* Submode 0 */
};

static TUNERCommonAttr_t commonAttrMicrotuneMT2131_K =
{
   /* name            */  "Microtune MT2131",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     863000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF( subModesMicrotuneMT2131_K ),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerMicrotuneMT2131_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrMicrotuneMT2131_K,
   /* myExtAttr    */ &dataMicrotuneMT2131_K,
   /* myFunct      */ &TUNERMT2131Functions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Microtune-MT2131----------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* NuTune HD1856AF MK2 tuner configuration                                    */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_NUTUNE_HD1856AF
#define TUNER_NUTUNE_HD1856AF
#endif
#endif

#ifdef  TUNER_NUTUNE_HD1856AF
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerNuTuneHD1856AF
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_NuTuneHD1856AF_BS_166_67_SWITCH_K[] =
{
   /* R = 010 , D/A = 1 */

   /* Fosc */
   /* Low Band */
   /*     - 127 MHz: CP = 010 , SP = 00001 */ {  91000 , { 0xCA , 0x41 , 0x8E } },
   /* 127 - 151 MHz: CP = 011 , SP = 00001 */ { 115000 , { 0xCA , 0x61 , 0x8E } },
   /* 151 - 176 MHz: CP = 100 , SP = 00001 */ { 140000 , { 0xCA , 0x81 , 0x8E } },
   /* 176 - 193 MHz: CP = 110 , SP = 00001 */ { 157000 , { 0xCA , 0xC1 , 0x8E } },
   /* Mid Band */
   /* 193 - 337 MHz: CP = 010 , SP = 00010 */ { 301000 , { 0xCA , 0x42 , 0x8E } },
   /* 337 - 394 MHz: CP = 011 , SP = 00010 */ { 358000 , { 0xCA , 0x62 , 0x8E } },
   /* 394 - 426 MHz: CP = 100 , SP = 00010 */ { 390000 , { 0xCA , 0x82 , 0x8E } },
   /* 426 - 457 MHz: CP = 101 , SP = 00010 */ { 421000 , { 0xCA , 0xA2 , 0x8E } },
   /* 457 - 480 MHz: CP = 110 , SP = 00010 */ { 444000 , { 0xCA , 0xC1 , 0x8E } },
   /* High band */
   /* 480 - 692 MHz: CP = 011 , SP = 00100 */ { 656000 , { 0xCA , 0x64 , 0x8E } },
   /* 692 - 812 MHz: CP = 100 , SP = 00100 */ { 776000 , { 0xCA , 0x84 , 0x8E } },
   /* 812 - 860 MHz: CP = 101 , SP = 00100 */ { 824000 , { 0xCA , 0xA4 , 0x8E } },
   /* 860 - 900 MHz: CP = 111 , SP = 00100 */ { 864000 , { 0xCA , 0xE4 , 0x8E } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneHD1856AF_BS_166_67_LOCK_K[] =
{
   /* R = 010 , D/A = 1 */

   /* Fosc */
   /* Low Band */
   /*     - 127 MHz: CP = 010 , SP = 00001 */ {  91000 , { 0xCA , 0x41 , 0x86 } },
   /* 127 - 151 MHz: CP = 011 , SP = 00001 */ { 115000 , { 0xCA , 0x61 , 0x86 } },
   /* 151 - 176 MHz: CP = 100 , SP = 00001 */ { 140000 , { 0xCA , 0x81 , 0x86 } },
   /* 176 - 193 MHz: CP = 110 , SP = 00001 */ { 157000 , { 0xCA , 0xC1 , 0x86 } },
   /* Mid Band */
   /* 193 - 337 MHz: CP = 010 , SP = 00010 */ { 301000 , { 0xCA , 0x42 , 0x86 } },
   /* 337 - 394 MHz: CP = 011 , SP = 00010 */ { 358000 , { 0xCA , 0x62 , 0x86 } },
   /* 394 - 426 MHz: CP = 100 , SP = 00010 */ { 390000 , { 0xCA , 0x82 , 0x86 } },
   /* 426 - 457 MHz: CP = 101 , SP = 00010 */ { 421000 , { 0xCA , 0xA2 , 0x86 } },
   /* 457 - 480 MHz: CP = 110 , SP = 00010 */ { 444000 , { 0xCA , 0xC1 , 0x86 } },
   /* High band */
   /* 480 - 692 MHz: CP = 011 , SP = 00100 */ { 656000 , { 0xCA , 0x64 , 0x86 } },
   /* 692 - 812 MHz: CP = 100 , SP = 00100 */ { 776000 , { 0xCA , 0x84 , 0x86 } },
   /* 812 - 860 MHz: CP = 101 , SP = 00100 */ { 824000 , { 0xCA , 0xA4 , 0x86 } },
   /* 860 - 900 MHz: CP = 111 , SP = 00100 */ { 864000 , { 0xCA , 0xE4 , 0x86 } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneHD1856AF_BS_62_5_SWITCH_K[] =
{
   /* R = 000 , D/A = 1 */

   /* Fosc */
   /* Low Band */
   /*     - 127 MHz: CP = 100 , SP = 00001 */ {  91000 , { 0xC8 , 0x81 , 0x8E } },
   /* 127 - 167 MHz: CP = 101 , SP = 00001 */ { 131000 , { 0xC8 , 0xA1 , 0x8E } },
   /* 167 - 177 MHz: CP = 110 , SP = 00001 */ { 141000 , { 0xC8 , 0xC1 , 0x8E } },
   /* 177 - 197 MHz: CP = 111 , SP = 00001 */ { 161000 , { 0xC8 , 0xE1 , 0x8E } },
   /* Mid Band */
   /* 197 - 307 MHz: CP = 100 , SP = 00010 */ { 271000 , { 0xC8 , 0x82 , 0x8E } },
   /* 307 - 387 MHz: CP = 101 , SP = 00010 */ { 351000 , { 0xC8 , 0xA2 , 0x8E } },
   /* 387 - 436 MHz: CP = 110 , SP = 00010 */ { 400000 , { 0xC8 , 0xC2 , 0x8E } },
   /* 436 - 483 MHz: CP = 111 , SP = 00010 */ { 447000 , { 0xC8 , 0xE2 , 0x8E } },
   /* High band */
   /* 483 - 637 MHz: CP = 100 , SP = 00100 */ { 601000 , { 0xC8 , 0x84 , 0x8E } },
   /* 637 - 737 MHz: CP = 101 , SP = 00100 */ { 701000 , { 0xC8 , 0xA4 , 0x8E } },
   /* 737 - 837 MHz: CP = 110 , SP = 00100 */ { 801000 , { 0xC8 , 0xC4 , 0x8E } },
   /* 837 - 903 MHz: CP = 111 , SP = 00100 */ { 867000 , { 0xC8 , 0xE4 , 0x8E } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneHD1856AF_BS_62_5_LOCK_K[] =
{
   /* R = 000 , D/A = 1 */

   /* Fosc */
   /* Low Band */
   /*     - 127 MHz: CP = 100 , SP = 00001 */ {  91000 , { 0xC8 , 0x81 , 0x86 } },
   /* 127 - 167 MHz: CP = 101 , SP = 00001 */ { 131000 , { 0xC8 , 0xA1 , 0x86 } },
   /* 167 - 177 MHz: CP = 110 , SP = 00001 */ { 141000 , { 0xC8 , 0xC1 , 0x86 } },
   /* 177 - 197 MHz: CP = 111 , SP = 00001 */ { 161000 , { 0xC8 , 0xE1 , 0x86 } },
   /* Mid Band */
   /* 197 - 307 MHz: CP = 100 , SP = 00010 */ { 271000 , { 0xC8 , 0x82 , 0x86 } },
   /* 307 - 387 MHz: CP = 101 , SP = 00010 */ { 351000 , { 0xC8 , 0xA2 , 0x86 } },
   /* 387 - 436 MHz: CP = 110 , SP = 00010 */ { 400000 , { 0xC8 , 0xC2 , 0x86 } },
   /* 436 - 483 MHz: CP = 111 , SP = 00010 */ { 447000 , { 0xC8 , 0xE2 , 0x86 } },
   /* High band */
   /* 483 - 637 MHz: CP = 100 , SP = 00100 */ { 601000 , { 0xC8 , 0x84 , 0x86 } },
   /* 637 - 737 MHz: CP = 101 , SP = 00100 */ { 701000 , { 0xC8 , 0xA4 , 0x86 } },
   /* 737 - 837 MHz: CP = 110 , SP = 00100 */ { 801000 , { 0xC8 , 0xC4 , 0x86 } },
   /* 837 - 903 MHz: CP = 111 , SP = 00100 */ { 867000 , { 0xC8 , 0xE4 , 0x86 } }
};

static TUNERCANNEDControlTable_t TCT_NuTuneHD1856AF[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.67, External RF-AGC ",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NuTuneHD1856AF_BS_166_67_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.67, External RF-AGC ",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NuTuneHD1856AF_BS_166_67_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5, External RF-AGC ",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_NuTuneHD1856AF_BS_62_5_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5, External RF-AGC ",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_NuTuneHD1856AF_BS_62_5_LOCK_K
   }
};

static TUNERSubMode_t subModesNuTuneHD1856AF[] =
{
   "Ext. RF-AGC, 166.7kHz",   /* Submode 0 */
   "Ext. RF-AGC,  62.5kHz",   /* Submode 1 */
};

static TUNERCANNEDData_t dataNuTuneHD1856AF =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_NuTuneHD1856AF),
   /* nrOfControlBytes  */          3,
};

static TUNERCommonAttr_t commonAttrNuTuneHD1856AF =
{
   /* name            */  "NuTune HD1856AF MK2",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     887000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesNuTuneHD1856AF),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerNuTuneHD1856AF =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrNuTuneHD1856AF,
   /* myExtAttr    */ &dataNuTuneHD1856AF,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-NuTune-HD1856AF-----------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* NuTune NH3660 NIM configuration                                            */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_NUTUNE_NH3660_NIM
#define TUNER_NUTUNE_NH3660_NIM
#endif
#endif

#ifdef  TUNER_NUTUNE_NH3660_NIM
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerNuTuneNH3660
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_NuTuneNH3660_CBBB_166[] =
{
   /* RSA / RSB = 10 */
   /*     -  85 MHz: BB: CP/T=0110, P=0001 */ {  85000 , { 0xB4 , 0x81 } },
   /*  85 - 105 MHz: BB: CP/T=0111, P=0001 */ { 105000 , { 0xBC , 0x81 } },
   /* 105 - 130 MHz: BB: CP/T=1110, P=0001 */ { 130000 , { 0xF4 , 0x81 } },
   /* 130 - 146 MHz: BB: CP/T=1111, P=0001 */ { 146000 , { 0xFC , 0x81 } },
   /* 146 - 250 MHz: BB: CP/T=0110, P=0010 */ { 250000 , { 0xB4 , 0x82 } },
   /* 250 - 350 MHz: BB: CP/T=0111, P=0010 */ { 350000 , { 0xBC , 0x82 } },
   /* 350 - 410 MHz: BB: CP/T=1110, P=0010 */ { 410000 , { 0xF4 , 0x82 } },
   /* 410 - 430 MHz: BB: CP/T=1111, P=0010 */ { 430000 , { 0xFC , 0x82 } },
   /* 430 - 470 MHz: BB: CP/T=0110, P=0100 */ { 470000 , { 0xB4 , 0x84 } },
   /* 470 - 725 MHz: BB: CP/T=0111, P=0100 */ { 725000 , { 0xBC , 0x84 } },
   /* 725 - 810 MHz: BB: CP/T=1110, P=0100 */ { 810000 , { 0xF4 , 0x84 } },
   /* 810 -     MHz: BB: CP/T=1111, P=0100 */ { MAXFRQ , { 0xFC , 0x84 } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneNH3660_CBBB_62_5[] =
{
   /* RSA / RSB = 11 */
   /*     -  85 MHz: BB: CP/T=0110, P=1001 */ {  85000 , { 0xB6 , 0x89 } },
   /*  85 - 105 MHz: BB: CP/T=0111, P=1001 */ { 105000 , { 0xBE , 0x89 } },
   /* 105 - 130 MHz: BB: CP/T=1110, P=1001 */ { 130000 , { 0xF6 , 0x89 } },
   /* 130 - 146 MHz: BB: CP/T=1111, P=1001 */ { 146000 , { 0xFE , 0x89 } },
   /* 146 - 250 MHz: BB: CP/T=0110, P=1010 */ { 250000 , { 0xB6 , 0x8A } },
   /* 250 - 350 MHz: BB: CP/T=0111, P=1010 */ { 350000 , { 0xBE , 0x8A } },
   /* 350 - 410 MHz: BB: CP/T=1110, P=1010 */ { 410000 , { 0xF6 , 0x8A } },
   /* 410 - 430 MHz: BB: CP/T=1111, P=1010 */ { 430000 , { 0xFE , 0x8A } },
   /* 430 - 470 MHz: BB: CP/T=0110, P=1100 */ { 470000 , { 0xB6 , 0x8C } },
   /* 470 - 725 MHz: BB: CP/T=0111, P=1100 */ { 725000 , { 0xBE , 0x8C } },
   /* 725 - 810 MHz: BB: CP/T=1110, P=1100 */ { 810000 , { 0xF6 , 0x8C } },
   /* 810 -     MHz: BB: CP/T=1111, P=1100 */ { MAXFRQ , { 0xFE , 0x8C } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneNH3660_CBAB_166_EXT[] =
{
   /* RSA / RSB = 10, ATC = 0, AL = 111,  */
   /* AB: CP/T = 0011 */ { MAXFRQ , { 0x9C , 0x70 } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneNH3660_CBAB_62_5_EXT[] =
{
   /* RSA / RSB = 11, ATC = 0, AL = 111,  */
   /* AB: CP/T = 0011 */ { MAXFRQ , { 0x9E , 0x70 } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneNH3660_CBAB_166_112_LOCK[] =
{
   /* RSA / RSB = 10, ATC = 0, AL = 010,  */
   /* AB: CP/T = 0011 */ { MAXFRQ , { 0x9C , 0x20 } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneNH3660_CBAB_166_112_SWITCH[] =
{
   /* RSA / RSB = 10, ATC = 1, AL = 010,  */
   /* AB: CP/T = 0011 */ { MAXFRQ , { 0x9C , 0xA0 } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneNH3660_CBAB_62_5_112_LOCK[] =
{
   /* RSA / RSB = 11, ATC = 0, AL = 010,  */
   /* AB: CP/T = 0011 */ { MAXFRQ , { 0x9E , 0x20 } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneNH3660_CBAB_62_5_112_SWITCH[] =
{
   /* RSA / RSB = 11, ATC = 1, AL = 010,  */
   /* AB: CP/T = 0011 */ { MAXFRQ , { 0x9E , 0xA0 } }
};

static TUNERCANNEDControlTable_t TCT_NuTuneNH3660[] =
{

   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Aux Ext, ref.div.=62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_NuTuneNH3660_CBAB_62_5_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Aux Ext, ref.div.=166.7kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_NuTuneNH3660_CBAB_166_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Aux 112, ref.div.=62.5kHz, lock",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_NuTuneNH3660_CBAB_62_5_112_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Aux 112, ref.div.=62.5kHzm switch",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_NuTuneNH3660_CBAB_62_5_112_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "Aux Ext, ref.div.=166.7kHz, lock",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_NuTuneNH3660_CBAB_166_112_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "Aux Ext, ref.div.=166.7kHz, switch",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_NuTuneNH3660_CBAB_166_112_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0 | TUNER_MODE_SUB2,
      /* description              */ "CB, ref.div.=65.2kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_NuTuneNH3660_CBBB_62_5
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1 | TUNER_MODE_SUB3,
      /* description              */ "CB, ref.div.=166.7kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_NuTuneNH3660_CBBB_166
   }
};

static TUNERSubMode_t subModesNuTuneNH3660[] =
{
   "Ext. RF-AGC,  62.5kHz", /* TUNER_MODE_SUB0 */
   "Ext. RF-AGC, 166.7kHz", /* TUNER_MODE_SUB1 */
   "TOP 112dBuV,  62.5kHz", /* TUNER_MODE_SUB2 */
   "TOP 112dBuV, 166.7kHz", /* TUNER_MODE_SUB3 */
};

static TUNERCANNEDData_t dataNuTuneNH3660 =
{
   /* lockMask     */       0x40,
   /* lockValue    */       0x40,
   /* controlTable */ ARRAYREF(TCT_NuTuneNH3660),
   /* nrOfControlBytes  */     2
};

static TUNERCommonAttr_t commonAttrNuTuneNH3660 =
{
   /* name            */  "NuTune NH3660 NIM",
   /* minFreqRF [kHz] */      47000,
   /* maxFreqRF [kHz] */     869000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesNuTuneNH3660),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerNuTuneNH3660 =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrNuTuneNH3660,
   /* myExtAttr    */ &dataNuTuneNH3660,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-NuTune-NH-3660------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* NuTune TH2603 tuner configuration                                          */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_NUTUNE_TH2603
#define TUNER_NUTUNE_TH2603
#endif
#endif

#ifdef  TUNER_NUTUNE_TH2603
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerNuTuneTH2603
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_NuTuneTH2603_DIV166[] =
{
  /* D/A = 1, XT = 0, R = 010 */
  /* Low band */
  /*  48  -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0xC2 , 0x41 } },
  /*  91  - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0xC2 , 0x61 } },
  /* 115  - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0xC2 , 0x81 } },
  /* 140  - 157 MHz: CP = 101, SPS = 00001 */ { 157000 , { 0xC2 , 0xA1 } },
  /* Mid band */
  /* 157  - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0xC2 , 0x42 } },
  /* 301  - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0xC2 , 0x62 } },
  /* 358  - 410 MHz: CP = 100, SPS = 00010 */ { 410000 , { 0xC2 , 0x82 } },
  /* 410  - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0xC2 , 0xA2 } },
  /* 421  - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0xC2 , 0xC2 } },
  /* High band */
  /* 444  - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0xC2 , 0x64 } },
  /* 656  - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0xC2 , 0x84 } },
  /* 776  - 854 MHz: CP = 101, SPS = 00100 */ { MAXFRQ , { 0xC2 , 0xA4 } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneTH2603_DIV62_5[] =
{
  /* D/A = 1, XT = 0, R = 000 */
  /* AL = 110 */
  /* Low band */
  /*  48  -  91 MHz: CP = 100, SPS = 00001 */ {  91000 , { 0xC0 , 0x81 } },
  /*  91  - 131 MHz: CP = 101, SPS = 00001 */ { 131000 , { 0xC0 , 0xA1 } },
  /* 131  - 144 MHz: CP = 110, SPS = 00001 */ { 141000 , { 0xC0 , 0xC1 } },
  /* 141  - 161 MHz: CP = 111, SPS = 00001 */ { 160875 , { 0xC0 , 0xD1 } },
  /* Mid band */
  /* 161  - 271 MHz: CP = 100, SPS = 00010 */ { 271000 , { 0xC0 , 0x82 } },
  /* 271  - 351 MHz: CP = 101, SPS = 00010 */ { 351000 , { 0xC0 , 0xA2 } },
  /* 351  - 400 MHz: CP = 110, SPS = 00010 */ { 400000 , { 0xC0 , 0xC2 } },
  /* 400  - 447 MHz: CP = 111, SPS = 00010 */ { 446875 , { 0xC0 , 0xD2 } },
  /* High band */
  /* 447  - 601 MHz: CP = 100, SPS = 00100 */ { 601000 , { 0xC0 , 0x84 } },
  /* 601  - 701 MHz: CP = 101, SPS = 00100 */ { 701000 , { 0xC0 , 0xA4 } },
  /* 701  - 801 MHz: CP = 110, SPS = 00100 */ { 801000 , { 0xC0 , 0xC4 } },
  /* 801  - 867 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0xC0 , 0xD4 } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneTH2603_AGC166[] =
{
  /* D/A = 0, AL = 110 */
  /* Low band */
  /*  48  -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0x86 , 0x41 } },
  /*  91  - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0x86 , 0x61 } },
  /* 115  - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0x86 , 0x81 } },
  /* 140  - 157 MHz: CP = 101, SPS = 00001 */ { 157000 , { 0x86 , 0xA1 } },
  /* Mid band */
  /* 157  - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0x86 , 0x42 } },
  /* 301  - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0x86 , 0x62 } },
  /* 358  - 410 MHz: CP = 100, SPS = 00010 */ { 410000 , { 0x86 , 0x82 } },
  /* 410  - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0x86 , 0xA2 } },
  /* 421  - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0x86 , 0xC2 } },
  /* High band */
  /* 444  - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0x86 , 0x64 } },
  /* 656  - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0x86 , 0x84 } },
  /* 776  - 854 MHz: CP = 101, SPS = 00100 */ { MAXFRQ , { 0x86 , 0xA4 } }
};

static TUNERCANNEDControlUnit_t TCU_NuTuneTH2603_AGC62_5[] =
{
  /* D/A = 0, AL = 110 */
  /* Low band */
  /*  48  -  91 MHz: CP = 100, SPS = 00001 */ {  91000 , { 0x86 , 0x81 } },
  /*  91  - 131 MHz: CP = 101, SPS = 00001 */ { 131000 , { 0x86 , 0xA1 } },
  /* 131  - 144 MHz: CP = 110, SPS = 00001 */ { 141000 , { 0x86 , 0xC1 } },
  /* 141  - 161 MHz: CP = 111, SPS = 00001 */ { 160875 , { 0x86 , 0xD1 } },
  /* Mid band */
  /* 161  - 271 MHz: CP = 100, SPS = 00010 */ { 271000 , { 0x86 , 0x82 } },
  /* 271  - 351 MHz: CP = 101, SPS = 00010 */ { 351000 , { 0x86 , 0xA2 } },
  /* 351  - 400 MHz: CP = 110, SPS = 00010 */ { 400000 , { 0x86 , 0xC2 } },
  /* 400  - 447 MHz: CP = 111, SPS = 00010 */ { 446875 , { 0x86 , 0xD2 } },
  /* High band */
  /* 447  - 601 MHz: CP = 100, SPS = 00100 */ { 601000 , { 0x86 , 0x84 } },
  /* 601  - 701 MHz: CP = 101, SPS = 00100 */ { 701000 , { 0x86 , 0xA4 } },
  /* 701  - 801 MHz: CP = 110, SPS = 00100 */ { 801000 , { 0x86 , 0xC4 } },
  /* 801  - 867 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0x86 , 0xD4 } }
};


static TUNERCANNEDControlTable_t TCT_NuTuneTH2603[] =
{
    {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Divider, 166kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NuTuneTH2603_DIV166
   },
    {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "AGC, 166kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NuTuneTH2603_AGC166
   },
    {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Divider, 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_NuTuneTH2603_DIV62_5
   },
    {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "AGC, 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_NuTuneTH2603_AGC62_5
   }
};

static TUNERSubMode_t subModesNuTuneTH2603[] =
{
   "Ext. RF-AGC, 166.7kHz", /* Submode 0 */
   "Ext. RF-AGC,  62.5kHz"  /* Submode 1 */
};

static TUNERCANNEDData_t dataNuTuneTH2603 =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_NuTuneTH2603),
   /* nrOfControlBytes  */          2,
};

static TUNERCommonAttr_t commonAttrNuTuneTH2603 =
{
   /* name            */  "NuTune TH2603",
   /* minFreqRF [kHz] */      44000,
   /* maxFreqRF [kHz] */     868000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesNuTuneTH2603),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerNuTuneTH2603 =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrNuTuneTH2603,
   /* myExtAttr    */ &dataNuTuneTH2603,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-NuTune-TH2603-------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* NXP HD1816AF BHXP tuner configuration                                      */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_NXP_HD1816AF_BHXP_K
#define TUNER_NXP_HD1816AF_BHXP_K
#endif
#endif

#ifdef  TUNER_NXP_HD1816AF_BHXP_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerNXPHD1816AFBHXP_K
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_NXPHD1816AFBHXP_BS_166_67_SWITCH_K[] =
{
   /* R = 010 , D/A = 1 */

   /* Fosc */
   /* Low Band */
   /*     - 127 MHz: CP = 010 , SP = 00001 */ {  91000 , { 0xCA , 0x41 , 0x8E } },
   /* 127 - 151 MHz: CP = 011 , SP = 00001 */ { 115000 , { 0xCA , 0x61 , 0x8E } },
   /* 151 - 176 MHz: CP = 100 , SP = 00001 */ { 140000 , { 0xCA , 0x81 , 0x8E } },
   /* 176 - 193 MHz: CP = 110 , SP = 00001 */ { 157000 , { 0xCA , 0xC1 , 0x8E } },
   /* Mid Band */
   /* 193 - 337 MHz: CP = 010 , SP = 00010 */ { 301000 , { 0xCA , 0x42 , 0x8E } },
   /* 337 - 394 MHz: CP = 011 , SP = 00010 */ { 358000 , { 0xCA , 0x62 , 0x8E } },
   /* 394 - 426 MHz: CP = 100 , SP = 00010 */ { 390000 , { 0xCA , 0x82 , 0x8E } },
   /* 426 - 457 MHz: CP = 101 , SP = 00010 */ { 421000 , { 0xCA , 0xA2 , 0x8E } },
   /* 457 - 480 MHz: CP = 110 , SP = 00010 */ { 444000 , { 0xCA , 0xC1 , 0x8E } },
   /* High band */
   /* 480 - 692 MHz: CP = 011 , SP = 00100 */ { 656000 , { 0xCA , 0x64 , 0x8E } },
   /* 692 - 812 MHz: CP = 100 , SP = 00100 */ { 776000 , { 0xCA , 0x84 , 0x8E } },
   /* 812 - 860 MHz: CP = 101 , SP = 00100 */ { 824000 , { 0xCA , 0xA4 , 0x8E } },
   /* 860 - 900 MHz: CP = 111 , SP = 00100 */ { 864000 , { 0xCA , 0xE4 , 0x8E } }
};

static TUNERCANNEDControlUnit_t TCU_NXPHD1816AFBHXP_BS_166_67_LOCK_K[] =
{
   /* R = 010 , D/A = 1 */

   /* Fosc */
   /* Low Band */
   /*     - 127 MHz: CP = 010 , SP = 00001 */ {  91000 , { 0xCA , 0x41 , 0x86 } },
   /* 127 - 151 MHz: CP = 011 , SP = 00001 */ { 115000 , { 0xCA , 0x61 , 0x86 } },
   /* 151 - 176 MHz: CP = 100 , SP = 00001 */ { 140000 , { 0xCA , 0x81 , 0x86 } },
   /* 176 - 193 MHz: CP = 110 , SP = 00001 */ { 157000 , { 0xCA , 0xC1 , 0x86 } },
   /* Mid Band */
   /* 193 - 337 MHz: CP = 010 , SP = 00010 */ { 301000 , { 0xCA , 0x42 , 0x86 } },
   /* 337 - 394 MHz: CP = 011 , SP = 00010 */ { 358000 , { 0xCA , 0x62 , 0x86 } },
   /* 394 - 426 MHz: CP = 100 , SP = 00010 */ { 390000 , { 0xCA , 0x82 , 0x86 } },
   /* 426 - 457 MHz: CP = 101 , SP = 00010 */ { 421000 , { 0xCA , 0xA2 , 0x86 } },
   /* 457 - 480 MHz: CP = 110 , SP = 00010 */ { 444000 , { 0xCA , 0xC1 , 0x86 } },
   /* High band */
   /* 480 - 692 MHz: CP = 011 , SP = 00100 */ { 656000 , { 0xCA , 0x64 , 0x86 } },
   /* 692 - 812 MHz: CP = 100 , SP = 00100 */ { 776000 , { 0xCA , 0x84 , 0x86 } },
   /* 812 - 860 MHz: CP = 101 , SP = 00100 */ { 824000 , { 0xCA , 0xA4 , 0x86 } },
   /* 860 - 900 MHz: CP = 111 , SP = 00100 */ { 864000 , { 0xCA , 0xE4 , 0x86 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPHD1816AFBHXP_BS_62_5_SWITCH_K[] =
{
   /* R = 000 , D/A = 1 */

   /* Fosc */
   /* Low Band */
   /*     - 127 MHz: CP = 100 , SP = 00001 */ {  91000 , { 0xC8 , 0x81 , 0x8E } },
   /* 127 - 167 MHz: CP = 101 , SP = 00001 */ { 131000 , { 0xC8 , 0xA1 , 0x8E } },
   /* 167 - 177 MHz: CP = 110 , SP = 00001 */ { 141000 , { 0xC8 , 0xC1 , 0x8E } },
   /* 177 - 197 MHz: CP = 111 , SP = 00001 */ { 161000 , { 0xC8 , 0xE1 , 0x8E } },
   /* Mid Band */
   /* 197 - 307 MHz: CP = 100 , SP = 00010 */ { 271000 , { 0xC8 , 0x82 , 0x8E } },
   /* 307 - 387 MHz: CP = 101 , SP = 00010 */ { 351000 , { 0xC8 , 0xA2 , 0x8E } },
   /* 387 - 436 MHz: CP = 110 , SP = 00010 */ { 400000 , { 0xC8 , 0xC2 , 0x8E } },
   /* 436 - 483 MHz: CP = 111 , SP = 00010 */ { 447000 , { 0xC8 , 0xE2 , 0x8E } },
   /* High band */
   /* 483 - 637 MHz: CP = 100 , SP = 00100 */ { 601000 , { 0xC8 , 0x84 , 0x8E } },
   /* 637 - 737 MHz: CP = 101 , SP = 00100 */ { 701000 , { 0xC8 , 0xA4 , 0x8E } },
   /* 737 - 837 MHz: CP = 110 , SP = 00100 */ { 801000 , { 0xC8 , 0xC4 , 0x8E } },
   /* 837 - 903 MHz: CP = 111 , SP = 00100 */ { 867000 , { 0xC8 , 0xE4 , 0x8E } }
};

static TUNERCANNEDControlUnit_t TCU_NXPHD1816AFBHXP_BS_62_5_LOCK_K[] =
{
   /* R = 000 , D/A = 1 */

   /* Fosc */
   /* Low Band */
   /*     - 127 MHz: CP = 100 , SP = 00001 */ {  91000 , { 0xC8 , 0x81 , 0x86 } },
   /* 127 - 167 MHz: CP = 101 , SP = 00001 */ { 131000 , { 0xC8 , 0xA1 , 0x86 } },
   /* 167 - 177 MHz: CP = 110 , SP = 00001 */ { 141000 , { 0xC8 , 0xC1 , 0x86 } },
   /* 177 - 197 MHz: CP = 111 , SP = 00001 */ { 161000 , { 0xC8 , 0xE1 , 0x86 } },
   /* Mid Band */
   /* 197 - 307 MHz: CP = 100 , SP = 00010 */ { 271000 , { 0xC8 , 0x82 , 0x86 } },
   /* 307 - 387 MHz: CP = 101 , SP = 00010 */ { 351000 , { 0xC8 , 0xA2 , 0x86 } },
   /* 387 - 436 MHz: CP = 110 , SP = 00010 */ { 400000 , { 0xC8 , 0xC2 , 0x86 } },
   /* 436 - 483 MHz: CP = 111 , SP = 00010 */ { 447000 , { 0xC8 , 0xE2 , 0x86 } },
   /* High band */
   /* 483 - 637 MHz: CP = 100 , SP = 00100 */ { 601000 , { 0xC8 , 0x84 , 0x86 } },
   /* 637 - 737 MHz: CP = 101 , SP = 00100 */ { 701000 , { 0xC8 , 0xA4 , 0x86 } },
   /* 737 - 837 MHz: CP = 110 , SP = 00100 */ { 801000 , { 0xC8 , 0xC4 , 0x86 } },
   /* 837 - 903 MHz: CP = 111 , SP = 00100 */ { 867000 , { 0xC8 , 0xE4 , 0x86 } }
};

static TUNERCANNEDControlTable_t TCT_NXPHD1816AFBHXP_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.67, External RF-AGC ",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPHD1816AFBHXP_BS_166_67_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.67, External RF-AGC ",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPHD1816AFBHXP_BS_166_67_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5, External RF-AGC ",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_NXPHD1816AFBHXP_BS_62_5_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5, External RF-AGC ",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36127,
      /* controlUnits             */ TCU_NXPHD1816AFBHXP_BS_62_5_LOCK_K
   }
};

static TUNERSubMode_t subModesNXPHD1816AFBHXP_K[] =
{
   "Ext. RF-AGC, 166.7kHz",   /* Submode 0 */
   "Ext. RF-AGC,  62.5kHz",   /* Submode 1 */
};

static TUNERCANNEDData_t dataNXPHD1816AFBHXP_K =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_NXPHD1816AFBHXP_K),
   /* nrOfControlBytes  */          3,
};

static TUNERCommonAttr_t commonAttrNXPHD1816AFBHXP_K =
{
   /* name            */  "NXP HD1816AF_BHXP",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     887000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesNXPHD1816AFBHXP_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerNXPHD1816AFBHXP_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrNXPHD1816AFBHXP_K,
   /* myExtAttr    */ &dataNXPHD1816AFBHXP_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-NXP-HD1816AF_BHXP---------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* NXP TD1116AL tuner configuration                                           */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TD1116AL_K
#define TUNER_NXP_TD1116AL_K
#endif
#endif

#ifdef  TUNER_NXP_TD1116AL_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerNXPTD1116AL_K
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_NXPTD1116AL_167KHZ_EXT_K[] =
{
   /* CB2 = AUX: T2 T1 T0 = 0 1 1
      166kHz: R1 R0 = 1 0 =>       CB1 = 0x9C
      External RF-AGC:
      AL2 Al1 AL0 = 1 1 0 =>       CB2 = 0x60
    */

   /* Full Freq range : */ { MAXFRQ , { 0x9C , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1116AL_62KHZ_EXT_K[] =
{
   /* CB2 = AUX: T2 T1 T0 = 0 1 1
      62.5kHz: R1 R0 = 1 1 =>      CB1 = 0x9E
      External RF-AGC:
      AL2 Al1 AL0 = 1 1 0 =>       CB2 = 0x60
    */

   /* Full Freq range : */ { MAXFRQ , { 0x9E , 0x60 } }
};

#if 0
static TUNERCANNEDControlUnit_t TCU_NXPTD1116AL_167KHZ_BS_SWITCH_K[] =
{
   /* During search tuning charge pump current set to low:
      CP T2 T1 T0 = 0 1 1 0 (50uA)
      166kHz: R1 R0 = 1 0 =>       CB1 = 0xB4
    */

   /*     - 161 MHz: SP = 00001 */
   { 161000 , { 0xB4 , ( 0x01 ) } },
   /* 161 - 446 MHz: SP = 00010 */
   { 446000 , { 0xB4 , ( 0x02 ) } },
   /* 446 - 864 MHz: SP = 00100 */
   { MAXFRQ , { 0xB4 , ( 0x04 ) } }

};

static TUNERCANNEDControlUnit_t TCU_NXPTD1116AL_62KHZ_BS_SWITCH_K[] =
{
   /* During search tuning charge pump current set to low:
      CP T2 T1 T0 = 0 1 1 0 (50uA)
      62.5kHz: R1 R0 = 1 1 =>      CB1 = 0xB6
    */

   /*     - 161 MHz: SP = 00001 */
   { 161000 , { 0xB6 , ( 0x01 ) } },
   /* 161 - 446 MHz: SP = 00010 */
   { 446000 , { 0xB6 , ( 0x02 ) } },
   /* 446 - 864 MHz: SP = 00100 */
   { MAXFRQ , { 0xB6 , ( 0x04 ) } }

};
#endif

static TUNERCANNEDControlUnit_t TCU_NXPTD1116AL_167KHZ_BS_LOCK_K[] =
{
   /* 166kHz: R1 R0 = 1 0 */

   /* Fosc */
   /*     - 150 MHz: CP/T = 0110, SP = 00001 */ { 114000 , { 0xB4 , ( 0x01 ) } },
   /* 150 - 181 MHz: CP/T = 0111, SP = 00001 */ { 145000 , { 0xBC , ( 0x01 ) } },
   /* 181 - 197 MHz: CP/T = 1110, SP = 00001 */ { 161000 , { 0xF4 , ( 0x01 ) } },
   /* 197 - 349 MHz: CP/T = 0110, SP = 00010 */ { 313000 , { 0xB4 , ( 0x02 ) } },
   /* 349 - 445 MHz: CP/T = 0111, SP = 00010 */ { 409000 , { 0xBC , ( 0x02 ) } },
   /* 445 - 485 MHz: CP/T = 1110, SP = 00010 */ { 449000 , { 0xF4 , ( 0x02 ) } },
   /* 485 - 685 MHz: CP/T = 0111, SP = 00100 */ { 649000 , { 0xBC , ( 0x04 ) } },
   /* 685 - 837 MHz: CP/T = 1110, SP = 00100 */ { 649000 , { 0xF4 , ( 0x04 ) } },
   /* 837 -     MHz: CP/T = 1111, SP = 00100 */ { MAXFRQ , { 0xFC , ( 0x04 ) } }

};

static TUNERCANNEDControlUnit_t TCU_NXPTD1116AL_62KHZ_BS_LOCK_K[] =
{
   /* 62.5kHz: R1 R0 = 1 1 */

   /* Fosc */
   /*     - 150 MHz: CP/T = 0110, SP = 00001 */ { 114000 , { 0xB6 , ( 0x01 ) } },
   /* 150 - 181 MHz: CP/T = 0111, SP = 00001 */ { 145000 , { 0xBE , ( 0x01 ) } },
   /* 181 - 197 MHz: CP/T = 1110, SP = 00001 */ { 161000 , { 0xF6 , ( 0x01 ) } },
   /* 197 - 349 MHz: CP/T = 0110, SP = 00010 */ { 313000 , { 0xB6 , ( 0x02 ) } },
   /* 349 - 445 MHz: CP/T = 0111, SP = 00010 */ { 409000 , { 0xBE , ( 0x02 ) } },
   /* 445 - 485 MHz: CP/T = 1110, SP = 00010 */ { 449000 , { 0xF6 , ( 0x02 ) } },
   /* 485 - 685 MHz: CP/T = 0111, SP = 00100 */ { 649000 , { 0xBE , ( 0x04 ) } },
   /* 685 - 837 MHz: CP/T = 1110, SP = 00100 */ { 649000 , { 0xF6 , ( 0x04 ) } },
   /* 837 -     MHz: CP/T = 1111, SP = 00100 */ { MAXFRQ , { 0xFE , ( 0x04 ) } }

};


static TUNERCANNEDControlTable_t TCT_NXPTD1116AL_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Aux, 167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1116AL_167KHZ_EXT_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Aux, 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_NXPTD1116AL_62KHZ_EXT_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "BS, 167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1116AL_167KHZ_BS_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "BS, 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_NXPTD1116AL_62KHZ_BS_LOCK_K
   }
};


static TUNERSubMode_t subModesNXPTD1116AL_K[] =
{
   "Ext. RF-AGC, 166.7kHz",   /* Submode 0 */
   "Ext. RF-AGC,  62.5kHz",   /* Submode 1 */
};

static TUNERCANNEDData_t dataNXPTD1116AL_K =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_NXPTD1116AL_K),
   /* nrOfControlBytes  */          2,
};

static TUNERCommonAttr_t commonAttrNXPTD1116AL_K =
{
   /* name            */  "NXP TD1116AL",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesNXPTD1116AL_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36130,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerNXPTD1116AL_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrNXPTD1116AL_K,
   /* myExtAttr    */ &dataNXPTD1116AL_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-NXP-TD1116AL--------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* NXP TD1716F 3 tuner configuration                                          */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TD1716F_3_K
#define TUNER_NXP_TD1716F_3_K
#endif
#ifndef TUNER_NXP_TD1716F_3_BALANCED_K
#define TUNER_NXP_TD1716F_3_BALANCED_K
#endif
#endif


#ifdef  TUNER_NXP_TD1716F_3_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerNXPTD1716F3_K
#endif
#endif

#ifdef  TUNER_NXP_TD1716F_3_BALANCED_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerNXPTD1716F3_Balanced_K
#endif
#endif


#if   defined TUNER_NXP_TD1716F_3_K || \
      defined TUNER_NXP_TD1716F_3_BALANCED_K

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F3_AUX_166_67_EXT_K[] =
{
   /* R1 / R0 = 10, ATC = 0, AL = 110 */
   /*     - 652 MHz: CP/T = 0011,  */ { 652000 , { 0x9C , 0x60 } },
   /* 652 - 864 MHz: CP/T = 1011,  */ { MAXFRQ , { 0xDC , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F3_AUX_62_5_EXT_K[] =
{
   /* R1 / R0 = 11, ATC = 0, AL = 110 */
   /*     - 652 MHz: CP/T = 0011,  */ { 652000 , { 0x9E , 0x60 } },
   /* 652 - 864 MHz: CP/T = 1011,  */ { MAXFRQ , { 0xDE , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F3_BS_166_67_K[] =
{
   /* R1 / R0 = 10 */
   /*     - 141 MHz: CP/T = 0111, SP = 00001 */ { 141000 , { 0xBC , 0x01 } },
   /* 141 - 446 MHz: CP/T = 0111, SP = 00010 */ { 446000 , { 0xBC , 0x02 } },
   /* 446 - 652 MHz: CP/T = 0111, SP = 00100 */ { 652000 , { 0xBC , 0x04 } },
   /* 652 - 769 MHz: CP/T = 1110, SP = 00100 */ { 769000 , { 0xF4 , 0x04 } },
   /* 769 - 864 MHz: CP/T = 1111, SP = 00100 */ { MAXFRQ , { 0xFC , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F3_BS_62_5_K[] =
{
   /* R1 / R0 = 11 */
   /*     - 141 MHz: CP/T = 0111, SP = 00001 */ { 141000 , { 0xBE , 0x01 } },
   /* 141 - 446 MHz: CP/T = 0111, SP = 00010 */ { 446000 , { 0xBE , 0x02 } },
   /* 446 - 652 MHz: CP/T = 0111, SP = 00100 */ { 652000 , { 0xBE , 0x04 } },
   /* 652 - 769 MHz: CP/T = 1110, SP = 00100 */ { 769000 , { 0xF6 , 0x04 } },
   /* 769 - 864 MHz: CP/T = 1111, SP = 00100 */ { MAXFRQ , { 0xFE , 0x04 } }
};

static TUNERCANNEDControlTable_t TCT_NXPTD1716F3_K[] =
{

   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Aux, 166.67, External RF-AGC ",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F3_AUX_166_67_EXT_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Aux, 62.5, External RF-AGC",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_NXPTD1716F3_AUX_62_5_EXT_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "BS, 166.67",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F3_BS_166_67_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "BS, 62.5",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_NXPTD1716F3_BS_62_5_K
   }

};

static TUNERSubMode_t subModesNXPTD1716F3_K[] =
{
   "Ext. RF-AGC, 166.7kHz",   /* Submode 0 */
   "Ext. RF-AGC,  62.5kHz",   /* Submode 1 */
};

static TUNERCANNEDData_t dataNXPTD1716F3_K =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_NXPTD1716F3_K),
   /* nrOfControlBytes  */          2,
};
#endif

#ifdef  TUNER_NXP_TD1716F_3_K
static TUNERCommonAttr_t commonAttrNXPTD1716F3_K =
{
   /* name            */  "NXP TD1716F 3",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesNXPTD1716F3_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */  ( TUNER_WITHOUT_IFAMP )
};

static TUNERInstance_t tunerNXPTD1716F3_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrNXPTD1716F3_K,
   /* myExtAttr    */ &dataNXPTD1716F3_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif

#ifdef  TUNER_NXP_TD1716F_3_BALANCED_K
static TUNERCommonAttr_t commonAttrNXPTD1716F3_Balanced_K =
{
   /* name            */  "NXP TD1716F 3 balanced",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesNXPTD1716F3_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */  ( TUNER_WITHOUT_IFAMP )
};

static TUNERInstance_t tunerNXPTD1716F3_Balanced_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrNXPTD1716F3_Balanced_K,
   /* myExtAttr    */ &dataNXPTD1716F3_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-NXP-TD1716F-3-------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* NXP TD1716F 4 tuner configuration                                          */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TD1716F_4_K
#define TUNER_NXP_TD1716F_4_K
#endif
#endif

#ifdef  TUNER_NXP_TD1716F_4_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerNXPTD1716F4_K
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F4_CB1_DIV_K[] =
{
   /* D/A = 1, R = 010 */
   /*  48 -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0xCA , 0x51 } },
   /*  91 - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0xCA , 0x61 } },
   /* 115 - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0xCA , 0x81 } },
   /* 140 - 157 MHz: CP = 110, SPS = 00001 */ { 157000 , { 0xCA , 0xC1 } },
   /* 157 - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0xCA , 0x52 } },
   /* 301 - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0xCA , 0x62 } },
   /* 358 - 390 MHz: CP = 100, SPS = 00010 */ { 390000 , { 0xCA , 0x82 } },
   /* 390 - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0xCA , 0xA2 } },
   /* 421 - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0xCA , 0xC2 } },
   /* 444 - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0xCA , 0x64 } },
   /* 656 - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0xCA , 0x84 } },
   /* 776 - 824 MHz: CP = 101, SPS = 00100 */ { 824000 , { 0xCA , 0xA4 } },
   /* 824 - 864 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0xCA , 0xE4 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F4_CB1_EXT_SWITCH_K[] =
{
   /* D/A = 0, ATC = 1, AL = 110 */
   /*  48 -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0x8E , 0x41 } },
   /*  91 - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0x8E , 0x61 } },
   /* 115 - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0x8E , 0x81 } },
   /* 140 - 157 MHz: CP = 110, SPS = 00001 */ { 157000 , { 0x8E , 0xC1 } },
   /* 157 - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0x8E , 0x52 } },
   /* 301 - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0x8E , 0x62 } },
   /* 358 - 390 MHz: CP = 100, SPS = 00010 */ { 390000 , { 0x8E , 0x82 } },
   /* 390 - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0x8E , 0xA2 } },
   /* 421 - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0x8E , 0xC2 } },
   /* 444 - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0x8E , 0x64 } },
   /* 656 - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0x8E , 0x84 } },
   /* 776 - 824 MHz: CP = 101, SPS = 00100 */ { 824000 , { 0x8E , 0xA4 } },
   /* 824 - 864 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0x8E , 0xE4 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F4_CB1_EXT_LOCK_K[] =
{
   /* D/A = 0, ATC = 0, AL = 110 */
   /*  48 -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0x86 , 0x41 } },
   /*  91 - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0x86 , 0x61 } },
   /* 115 - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0x86 , 0x81 } },
   /* 140 - 157 MHz: CP = 110, SPS = 00001 */ { 157000 , { 0x86 , 0xC1 } },
   /* 157 - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0x86 , 0x52 } },
   /* 301 - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0x86 , 0x62 } },
   /* 358 - 390 MHz: CP = 100, SPS = 00010 */ { 390000 , { 0x86 , 0x82 } },
   /* 390 - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0x86 , 0xA2 } },
   /* 421 - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0x86 , 0xC2 } },
   /* 444 - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0x86 , 0x64 } },
   /* 656 - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0x86 , 0x84 } },
   /* 776 - 824 MHz: CP = 101, SPS = 00100 */ { 824000 , { 0x86 , 0xA4 } },
   /* 824 - 864 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0x86 , 0xE4 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F4_CB1_112_SWITCH_K[] =
{
   /* D/A = 0, ATC = 1, AL = 001 */
   /*  48 -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0x89 , 0x51 } },
   /*  91 - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0x89 , 0x61 } },
   /* 115 - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0x89 , 0x81 } },
   /* 140 - 157 MHz: CP = 110, SPS = 00001 */ { 157000 , { 0x89 , 0xC1 } },
   /* 157 - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0x89 , 0x52 } },
   /* 301 - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0x89 , 0x62 } },
   /* 358 - 390 MHz: CP = 100, SPS = 00010 */ { 390000 , { 0x89 , 0x82 } },
   /* 390 - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0x89 , 0xA2 } },
   /* 421 - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0x89 , 0xC2 } },
   /* 444 - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0x89 , 0x64 } },
   /* 656 - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0x89 , 0x84 } },
   /* 776 - 824 MHz: CP = 101, SPS = 00100 */ { 824000 , { 0x89 , 0xA4 } },
   /* 824 - 864 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0x89 , 0xE4 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F4_CB1_112_LOCK_K[] =
{
   /* D/A = 0, ATC= 0, AL = 001 */
   /*  48 -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0x81 , 0x41 } },
   /*  91 - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0x81 , 0x61 } },
   /* 115 - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0x81 , 0x81 } },
   /* 140 - 157 MHz: CP = 110, SPS = 00001 */ { 157000 , { 0x81 , 0xC1 } },
   /* 157 - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0x81 , 0x52 } },
   /* 301 - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0x81 , 0x62 } },
   /* 358 - 390 MHz: CP = 100, SPS = 00010 */ { 390000 , { 0x81 , 0x82 } },
   /* 390 - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0x81 , 0xA2 } },
   /* 421 - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0x81 , 0xC2 } },
   /* 444 - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0x81 , 0x64 } },
   /* 656 - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0x81 , 0x84 } },
   /* 776 - 824 MHz: CP = 101, SPS = 00100 */ { 824000 , { 0x81 , 0xA4 } },
   /* 824 - 864 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0x81 , 0xE4 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F4_CB1_109_SWITCH_K[] =
{
   /* D/A = 0, ATC = 1, AL = 010 */
   /*  48 -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0x8A , 0x41 } },
   /*  91 - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0x8A , 0x61 } },
   /* 115 - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0x8A , 0x81 } },
   /* 140 - 157 MHz: CP = 110, SPS = 00001 */ { 157000 , { 0x8A , 0xC1 } },
   /* 157 - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0x8A , 0x52 } },
   /* 301 - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0x8A , 0x62 } },
   /* 358 - 390 MHz: CP = 100, SPS = 00010 */ { 390000 , { 0x8A , 0x82 } },
   /* 390 - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0x8A , 0xA2 } },
   /* 421 - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0x8A , 0xC2 } },
   /* 444 - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0x8A , 0x64 } },
   /* 656 - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0x8A , 0x84 } },
   /* 776 - 824 MHz: CP = 101, SPS = 00100 */ { 824000 , { 0x8A , 0xA4 } },
   /* 824 - 864 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0x8A , 0xE4 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F4_CB1_109_LOCK_K[] =
{
   /* D/A = 0, ATC= 0, AL = 011 */
   /*  48 -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0x83 , 0x41 } },
   /*  91 - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0x83 , 0x61 } },
   /* 115 - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0x83 , 0x81 } },
   /* 140 - 157 MHz: CP = 110, SPS = 00001 */ { 157000 , { 0x83 , 0xC1 } },
   /* 157 - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0x83 , 0x52 } },
   /* 301 - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0x83 , 0x62 } },
   /* 358 - 390 MHz: CP = 100, SPS = 00010 */ { 390000 , { 0x83 , 0x82 } },
   /* 390 - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0x83 , 0xA2 } },
   /* 421 - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0x83 , 0xC2 } },
   /* 444 - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0x83 , 0x64 } },
   /* 656 - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0x83 , 0x84 } },
   /* 776 - 824 MHz: CP = 101, SPS = 00100 */ { 824000 , { 0x83 , 0xA4 } },
   /* 824 - 864 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0x83 , 0xE4 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F4_CB1_106_SWITCH_K[] =
{
   /* D/A = 0, ATC = 1, AL = 011 */
   /*  48 -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0x8B , 0x41 } },
   /*  91 - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0x8B , 0x61 } },
   /* 115 - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0x8B , 0x81 } },
   /* 140 - 157 MHz: CP = 110, SPS = 00001 */ { 157000 , { 0x8B , 0xC1 } },
   /* 157 - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0x8B , 0x52 } },
   /* 301 - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0x8B , 0x62 } },
   /* 358 - 390 MHz: CP = 100, SPS = 00010 */ { 390000 , { 0x8B , 0x82 } },
   /* 390 - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0x8B , 0xA2 } },
   /* 421 - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0x8B , 0xC2 } },
   /* 444 - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0x8B , 0x64 } },
   /* 656 - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0x8B , 0x84 } },
   /* 776 - 824 MHz: CP = 101, SPS = 00100 */ { 824000 , { 0x8B , 0xA4 } },
   /* 824 - 864 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0x8B , 0xE4 } }
};

static TUNERCANNEDControlUnit_t TCU_NXPTD1716F4_CB1_106_LOCK_K[] =
{
   /* D/A = 0, ATC= 0, AL = 010 */
   /*  48 -  91 MHz: CP = 010, SPS = 00001 */ {  91000 , { 0x82 , 0x41 } },
   /*  91 - 115 MHz: CP = 011, SPS = 00001 */ { 115000 , { 0x82 , 0x61 } },
   /* 115 - 140 MHz: CP = 100, SPS = 00001 */ { 140000 , { 0x82 , 0x81 } },
   /* 140 - 157 MHz: CP = 110, SPS = 00001 */ { 157000 , { 0x82 , 0xC1 } },
   /* 157 - 301 MHz: CP = 010, SPS = 00010 */ { 301000 , { 0x82 , 0x52 } },
   /* 301 - 358 MHz: CP = 011, SPS = 00010 */ { 358000 , { 0x82 , 0x62 } },
   /* 358 - 390 MHz: CP = 100, SPS = 00010 */ { 390000 , { 0x82 , 0x82 } },
   /* 390 - 421 MHz: CP = 101, SPS = 00010 */ { 421000 , { 0x82 , 0xA2 } },
   /* 421 - 444 MHz: CP = 110, SPS = 00010 */ { 444000 , { 0x82 , 0xC2 } },
   /* 444 - 656 MHz: CP = 011, SPS = 00100 */ { 656000 , { 0x82 , 0x64 } },
   /* 656 - 776 MHz: CP = 100, SPS = 00100 */ { 776000 , { 0x82 , 0x84 } },
   /* 776 - 824 MHz: CP = 101, SPS = 00100 */ { 824000 , { 0x82 , 0xA4 } },
   /* 824 - 864 MHz: CP = 111, SPS = 00100 */ { MAXFRQ , { 0x82 , 0xE4 } }
};

static TUNERCANNEDControlTable_t TCT_NXPTD1716F4_K[] =
{
    {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUBALL,
      /* description              */ "All Submodes Lock",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F4_CB1_DIV_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "External RF-AGC, switch",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F4_CB1_EXT_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK|
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "External RF-AGC, lock",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F4_CB1_EXT_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Internal RF-AGC TOP 112, switch",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F4_CB1_112_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Internal RF-AGC TOP 112, lock",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F4_CB1_112_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Internal RF-AGC TOP 109, switch",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F4_CB1_109_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Internal RF-AGC TOP 109, lock",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F4_CB1_109_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "Internal RF-AGC TOP 106, switch",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F4_CB1_106_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "Internal RF-AGC TOP 106, lock",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_NXPTD1716F4_CB1_106_LOCK_K
   }
};

static TUNERSubMode_t subModesNXPTD1716F4_K[] =
{
   "External RF-AGC", /* Submode 0 */
   "TOP 112dBuV",     /* Submode 1 */
   "TOP 109dBuV",     /* Submode 2 */
   "TOP 103dBuV"      /* Submode 3 */
};

static TUNERCANNEDData_t dataNXPTD1716F4_K =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_NXPTD1716F4_K),
   /* nrOfControlBytes  */          2,
};

static TUNERCommonAttr_t commonAttrNXPTD1716F4_K =
{
   /* name            */  "NXP TD1716F 4",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesNXPTD1716F4_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36167,
   /* myUserData      */       NULL,
   /* myCapabilities  */  ( TUNER_WITHOUT_IFAMP | TUNER_EXTERNAL_SAW_SWITCHING )
};

static TUNERInstance_t tunerNXPTD1716F4_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrNXPTD1716F4_K,
   /* myExtAttr    */ &dataNXPTD1716F4_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-NXP-TD1716F-4-------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* NXP TDA18271 tuner configuration                                           */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TDA18271
#define TUNER_NXP_TDA18271
#endif
#endif

#ifdef  TUNER_NXP_TDA18271
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerNXPTDA18271_K
#endif

#include "tunertda18271.h"

static TUNERTDA18271Data_t dataNXPTDA18271_K;

static TUNERSubMode_t subModesNXPTDA18271_K[] =
{
   "IFlvl = 2    Vpp [ 0 dB att]", /* TUNER_MODE_SUB0 */
   "IFlvl = 1.25 Vpp [-4 dB att]", /* TUNER_MODE_SUB1 */
   "IFlvl = 1    Vpp [-6 dB att]", /* TUNER_MODE_SUB2 */
   "IFlvl = 0.8  Vpp [-8 dB att]", /* TUNER_MODE_SUB3 */
   "IFlvl =          [-7.5 dB att]", /* TUNER_MODE_SUB4 */
   "IFlvl =          [-9.4 dB att]", /* TUNER_MODE_SUB4 */
   "IFlvl =          [-10.4 dB att]", /* TUNER_MODE_SUB4 */
   "IFlvl = 0.5  Vpp [-12 dB att]", /* TUNER_MODE_SUB4 */
};

static TUNERCommonAttr_t commonAttrNXPTDA18271_K =
{
   /* name            */  "NXP TDA18271",
   /* minFreqRF [kHz] */      40000,
   /* maxFreqRF [kHz] */     900000,
   /* sub-mode        */          3,
   /* sub-modes       */ ARRAYREF(subModesNXPTDA18271_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */       5000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerNXPTDA18271_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrNXPTDA18271_K,
   /* myExtAttr    */ &dataNXPTDA18271_K,
   /* myFunct      */ &TUNERTDA18271Functions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-NXP-TDA18271--------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* NXP TDA18272 tuner configuration                                           */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_NXP_TDA18272
#define TUNER_NXP_TDA18272
#endif
#endif

#ifdef  TUNER_NXP_TDA18272
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerNXPTDA18272_K
#endif

#include "tunertda18272.h"

static TUNERTDA18272Data_t dataNXPTDA18272_K =
{
   {
      {NULL}  /* sSrvTunerFunc */
   },
   0,          /* unit */
   FALSE,      /* isOpened */

   TUNER_TDA18272_CM_COUPLED, /* coupling */
   DRX_STANDARD_UNKNOWN       /* standard */
};

static TUNERSubMode_t subModesNXPTDA18272_K[] =
{
   "NO SUBMODES"  /* Submode 0 */
};

static TUNERCommonAttr_t commonAttrNXPTDA18272_K =
{
   /* name            */  "NXP TDA18272",
   /* minFreqRF [kHz] */      40000,
   /* maxFreqRF [kHz] */     900000,
   /* sub-mode        */          3,
   /* sub-modes       */ ARRAYREF(subModesNXPTDA18272_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */       4000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerNXPTDA18272_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrNXPTDA18272_K,
   /* myExtAttr    */ &dataNXPTDA18272_K,
   /* myFunct      */ &TUNERTDA18272Functions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-NXP-TDA18271--------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Panasonic ENG37ExxKF tuner configuration                              */
/*-----------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_PANASONIC_ENG37EXXKF_K
#define TUNER_PANASONIC_ENG37EXXKF_K
#endif
#endif

#ifdef  TUNER_PANASONIC_ENG37EXXKF_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerPanasonicENG37ExxKF_K
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_ANA_BS_K[] =
{
   /* RSA/RSB = 00, XTB = 0 */
   /*     - 143 MHz : CP/T = 0001, BS = 00001 */  { 143000 , { 0x88 , 0x01 } },
   /* 143 - 434 MHz : CP/T = 0001, BS = 00010 */  { 434000 , { 0x88 , 0x02 } },
   /* 434 - 857 MHz : CP/T = 0001, BS = 00100 */  { MAXFRQ , { 0x88 , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_QAM_BS_K[] =
{
   /* RSA/RSB = 00, XTB = 1 */
   /* E2  - E4   50  -  64   MHz: CP/T 0110, BS = 10001 */ {  64000 , { 0xB0 , 0x81 } },
   /* S1  - S5  107  - 135   MHz: CP/T 0111, BS = 10010 */ { 135000 , { 0xB8 , 0x82 } },
   /* S6        142          MHz: CP/T 1110, BS = 10010 */ { 142000 , { 0xF0 , 0x82 } },
   /* S7  - S10 149  - 170   MHz: CP/T 0110, BS = 10010 */ { 170000 , { 0xB0 , 0x82 } },
   /* E5  - S14 177  - 254   MHz: CP/T 0110, BS = 10010 */ { 254000 , { 0xB0 , 0x82 } },
   /* S15 - S20 261  - 296   MHz: CP/T 0111, BS = 10010 */ { 296000 , { 0xB8 , 0x82 } },
   /* S33 - S37 401  - 433   MHz: CP/T 1110, BS = 10100 */ { 433000 , { 0xF0 , 0x84 } },
   /* S38 - E32 441  - 561   MHz: CP/T 0110, BS = 10100 */ { 561000 , { 0xB0 , 0x84 } },
   /* E33 - E63 569  - 809   MHz: CP/T 0111, BS = 10100 */ { 809000 , { 0xB8 , 0x84 } },
   /* E64 - E69 817  - 857   MHz: CP/T 1110, BS = 10100 */ { MAXFRQ , { 0xF0 , 0x84 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_OFDM_BS_7_K[] =
{
   /* RSA/RSB = 10, XTB = 1 */
   /* E5  - E12 177 -   226  MHz: CP/T 0110, BS = 10010 */ { 226000 , { 0xB4 , 0x92 } },
   /* E21 - E32 473   - 561  MHz: CP/T 0110, BS = 10100 */ { 561000 , { 0xB4 , 0x94 } },
   /* E33 - E63 569   - 809  MHz: CP/T 0111, BS = 10100 */ { 809000 , { 0xBC , 0x94 } },
   /* E64 - E69 817   - 857  MHz: CP/T 1110, BS = 10100 */ { MAXFRQ , { 0xF4 , 0x94 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_OFDM_BS_8_K[] =
{
   /* RSA/RSB = 10, XTB = 1 */
   /* E5  - E12 177 -   226  MHz: CP/T 0110, BS = 00010 */ { 226000 , { 0xB4 , 0x82 } },
   /* E21 - E32 473   - 561  MHz: CP/T 0110, BS = 00100 */ { 561000 , { 0xB4 , 0x84 } },
   /* E33 - E63 569   - 809  MHz: CP/T 0111, BS = 00100 */ { 809000 , { 0xBC , 0x84 } },
   /* E64 - E69 817   - 857  MHz: CP/T 1110, BS = 00100 */ { MAXFRQ , { 0xF4 , 0x84 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_106_SWITCH_K[] =
{
   /* RSA/RSB = 00, ATC = 0, AL = 100 */
   /* NZ1 -  C57 : CP/T = 0011             */  { MAXFRQ, { 0x98 , 0xC0 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_106_LOCK_K[] =
{
   /* RSA/RSB = 00, ATC = 0, AL = 100 */
   /* NZ1 -  C57 : CP/T = 0011             */  { MAXFRQ, { 0x98 , 0x40 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_109_SWITCH_K[] =
{
   /* RSA/RSB = 00, ATC = 0, AL = 011 */
   /* NZ1 -  C57 : CP/T = 0011             */  { MAXFRQ, { 0x98 , 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_109_LOCK_K[] =
{
   /* RSA/RSB = 00, ATC = 0, AL = 011 */
   /* NZ1 -  C57 : CP/T = 0011             */  { MAXFRQ, { 0x98 , 0x30 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_112_SWITCH_K[] =
{
   /* RSA/RSB = 00, ATC = 0, AL = 010 */
   /* NZ1 -  C57 : CP/T = 0011             */  { MAXFRQ, { 0x98 , 0xA0 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_112_LOCK_K[] =
{
   /* RSA/RSB = 00, ATC = 0, AL = 010 */
   /* NZ1 -  C57 : CP/T = 0011             */  { MAXFRQ, { 0x98 , 0x20 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_EXT_K[] =
{
   /* RSA/RSB = 00, ATC = 0, AL = 110 */
   /* NZ1 -  C57 : CP/T = 0011             */  { MAXFRQ, { 0x98 , 0x60 } }
};


static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_OFDM_AUX_106_SWITCH_K[] =
{
   /* RSA/RSB = 10, ATC =0, AL = 100 */
   /* E5 - E69   177 - 857    MHz: CP/T 0011 */ { MAXFRQ , { 0x9C , 0xC0 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_OFDM_AUX_106_LOCK_K[] =
{
   /* RSA/RSB = 10, ATC =0, AL = 100 */
   /* E5 - E69   177 - 857    MHz: CP/T 0011 */ { MAXFRQ , { 0x9C , 0x40 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_OFDM_AUX_112_SWITCH_K[] =
{
   /* RSA/RSB = 10, ATC = 1, AL = 010 */
   /* E5 - E69   177 - 857    MHz: CP/T 0011 */ { MAXFRQ , { 0x9C , 0xA0 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_OFDM_AUX_112_LOCK_K[] =
{
   /* RSA/RSB = 10, ATC = 0, AL = 010 */
   /* E5 - E69   177 - 857    MHz: CP/T 0011 */ { MAXFRQ , { 0x9C , 0x20 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37EXXKF_OFDM_AUX_EXT_K[] =
{
   /* RSA/RSB = 10, ATC = 0, AL = 110 */
   /* E5 - E69  177  - 857   MHz: CP/T 0011 */ { MAXFRQ , { 0x9C , 0x60 } }
};

static TUNERCANNEDControlTable_t TCT_PANASONIC_ENG37EXXKF_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Aux, stepsize 50/125kHz, TOP 106",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_106_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK|
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Aux, stepsize 50/125kHz, TOP 106",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_106_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Aux, stepsize 50/125kHz, TOP 109",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_109_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Aux, stepsize 50/125kHz, TOP 109",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_109_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Aux, stepsize 50/125kHz, TOP 112",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_112_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Aux, stepsize 50/125kHz, TOP 112",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_112_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "Aux, stepsize 50/125kHz, Ext",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_ANA_QAM_AUX_EXT_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "Aux, stepsize 166.67kHz, 106 dBuV",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_OFDM_AUX_106_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "Aux, stepsize 166.67kHz, 106 dBuV",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_OFDM_AUX_106_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB5,
      /* description              */ "Aux, stepsize 166.67kHz, 112 dBuV",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_OFDM_AUX_112_SWITCH_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB5,
      /* description              */ "Aux, stepsize 166.67kHz, 112 dBuV",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_OFDM_AUX_112_LOCK_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB6,
      /* description              */ "Aux, stepsize 166.67kHz, external",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_OFDM_AUX_EXT_K
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0 |
                                     TUNER_MODE_SUB1 |
                                     TUNER_MODE_SUB2 |
                                     TUNER_MODE_SUB3,
      /* description              */ "Normal, stepsize 50kHz",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_ANA_BS_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0 |
                                     TUNER_MODE_SUB1 |
                                     TUNER_MODE_SUB2 |
                                     TUNER_MODE_SUB3,
      /* description              */ "Normal, stepsize 125kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_QAM_BS_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB4  |
                                     TUNER_MODE_SUB5  |
                                     TUNER_MODE_SUB6,
      /* description              */ "Normal, stepsize 166.67kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_OFDM_BS_7_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB4 |
                                     TUNER_MODE_SUB5 |
                                     TUNER_MODE_SUB6,
      /* description              */  "Normal, stepsize 166.67kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENG37EXXKF_OFDM_BS_8_K
   },

};

static TUNERSubMode_t subModesPanasonicENG37ExxKF_K[] =
{
   "TOP 106dBuV, ATV/QAM", /* Submode 0 */
   "TOP 109dBuV, ATV/QAM", /* Submode 1 */
   "TOP 112dBuV, ATV/QAM", /* Submode 2 */
   "Ext. RF-AGC, ATV/QAM", /* Submode 3 */
   "TOP 106dBuV, OFDM",    /* Submode 4 */
   "TOP 112dBuV, OFDM",    /* Submode 5 */
   "Ext. RF-AGC, OFDM"     /* Submode 6 */
};

static TUNERCANNEDData_t dataPanasonicENG37ExxKF_K =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_PANASONIC_ENG37EXXKF_K),
   /* nrOfControlBytes  */          2,
};

static TUNERCommonAttr_t commonAttrPanasonicENG37ExxKF_K =
{
   /* name            */  "Panasonic ENG37ExxKF",
   /* minFreqRF [kHz] */      47000,
   /* maxFreqRF [kHz] */     862000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesPanasonicENG37ExxKF_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerPanasonicENG37ExxKF_K =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrPanasonicENG37ExxKF_K,
   /* myExtAttr    */ &dataPanasonicENG37ExxKF_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Panasonic-ENG37ExxKF------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Panasonic ENG37F01KF tuner configuration                                  */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_PANASONIC_ENG37F01KF
#define TUNER_PANASONIC_ENG37F01KF
#endif
#ifndef TUNER_PANASONIC_ET35DRV
#define TUNER_PANASONIC_ET35DRV
#endif
#ifndef TUNER_MIC_PANASONIC_ENG37F01KF
#define TUNER_MIC_PANASONIC_ENG37F01KF
#endif
#endif

#ifdef  TUNER_PANASONIC_ENG37F01KF
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerPanasonicENG37F01KF
#endif
#endif

#ifdef  TUNER_MIC_PANASONIC_ENG37F01KF
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerMicPanasonicENG37F01KF
#endif
#endif

#ifdef  TUNER_PANASONIC_ET35DRV
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerET35DRV
#endif
#endif

#if   defined TUNER_PANASONIC_ENG37F01KF     || \
      defined TUNER_MIC_PANASONIC_ENG37F01KF || \
      defined TUNER_PANASONIC_ET35DRV

#include "tunercanned.h"

/* Bandswitch ranges */
/* VHF-LOW:  IRA - S6   49.0 MHz - 142.5 MHz */
/* VHF-HIGH: S7  - S36 149.5 MHz - 426.5 MHz */
/* UHF:      S37 - E69 433.5 MHz - 858.0 MHz */

/* Charge-Pump ranges (DVB-T) */
/* E5  - E12: 177.5 MHz - 226.5 MHz */
/* E21 - E28: 474.0 MHz - 530.0 MHz */
/* E29 - E65: 538.0 MHZ - 826.0 MHz */
/* E66 - E67: 834.0 MHz - 842.0 MHz */
/* E68 - E69: 850.0 MHz - 858.0 MHz */

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_108_SWITCH[] =
{
   /* CB1: ATP = 010, RS = 000 (TOP108dBuV, 166.7kHz)
           => 10010000 = 0x90 */
   /* CB2: ATC = 1, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11110000/11110010 = 0xF0 / 0xF2*/

   /* BB:  AISL = 0, P5 = 0 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x90 , 0x01 , 0xF0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x90 , 0x02 , 0xF0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x90 , 0x44 , 0xF0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x90 , 0x84 , 0xF0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x90 , 0xC4 , 0xF0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x90 , 0x04 , 0xF2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_108_SWITCH[] =
{
   /* CB1: ATP = 010, RS = 000 (TOP108dBuV, 166.7kHz)
           => 10010000 = 0x90 */
   /* CB2: ATC = 1, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11110000/11110010 = 0xF0 / 0xF2*/

   /* BB:  AISL = 0, P5 = 1 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x90 , 0x11 , 0xF0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x90 , 0x12 , 0xF0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x90 , 0x54 , 0xF0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x90 , 0x94 , 0xF0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x90 , 0xD4 , 0xF0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x90 , 0x14 , 0xF2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_108_LOCK[] =
{
   /* CB1: ATP = 010, RS = 000 (TOP108dBuV, 166.7kHz)
           => 10010000 = 0x90 */
   /* CB2: ATC = 0, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11010000/11010010 = 0xD0 / 0xD2*/

   /* BB:  AISL = 0, P5 = 0 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x90 , 0x01 , 0xD0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x90 , 0x02 , 0xD0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x90 , 0x44 , 0xD0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x90 , 0x84 , 0xD0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x90 , 0xC4 , 0xD0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x90 , 0x04 , 0xD2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_108_LOCK[] =
{
   /* CB1: ATP = 010, RS = 000 (TOP108dBuV, 166.7kHz)
           => 10001000 = 0x90 */
   /* CB2: ATC = 0, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11010000/11010010 = 0xD0 / 0xD2*/

   /* BB:  AISL = 0, P5 = 1 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x90 , 0x11 , 0xD0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x90 , 0x12 , 0xD0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x90 , 0x54 , 0xD0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x90 , 0x94 , 0xD0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x90 , 0xD4 , 0xD0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x90 , 0x14 , 0xD2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_110_SWITCH[] =
{
   /* CB1: ATP = 001, RS = 000 (TOP110dBuV, 166.7kHz)
           => 10001000 = 0x8 */
   /* CB2: ATC = 1, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11110000/11110010 = 0xF0 / 0xF2*/

   /* BB:  AISL = 0, P5 = 0 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x88 , 0x01 , 0xF0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x88 , 0x02 , 0xF0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x88 , 0x44 , 0xF0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x88 , 0x84 , 0xF0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x88 , 0xC4 , 0xF0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x88 , 0x04 , 0xF2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_110_SWITCH[] =
{
   /* CB1: ATP = 001, RS = 000 (TOP110dBuV, 166.7kHz)
           => 10001000 = 0x88 */
   /* CB2: ATC = 1, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11110000/11110010 = 0xF0 / 0xF2*/

   /* BB:  AISL = 0, P5 = 1 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x88 , 0x11 , 0xF0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x88 , 0x12 , 0xF0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x88 , 0x54 , 0xF0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x88 , 0x94 , 0xF0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x88 , 0xD4 , 0xF0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x88 , 0x14 , 0xF2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_110_LOCK[] =
{
   /* CB1: ATP = 001, RS = 000 (TOP110dBuV, 166.7kHz)
           => 10001000 = 0x88 */
   /* CB2: ATC = 0, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11010000/11010010 = 0xD0 / 0xD2*/

   /* BB:  AISL = 0, P5 = 0 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x88 , 0x01 , 0xD0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x88 , 0x02 , 0xD0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x88 , 0x44 , 0xD0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x88 , 0x84 , 0xD0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x88 , 0xC4 , 0xD0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x88 , 0x04 , 0xD2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_110_LOCK[] =
{
   /* CB1: ATP = 001, RS = 000 (TOP110dBuV, 166.7kHz)
           => 10001000 = 0x88 */
   /* CB2: ATC = 0, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11010000/11010010 = 0xD0 / 0xD2*/

   /* BB:  AISL = 0, P5 = 1 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x88 , 0x11 , 0xD0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x88 , 0x12 , 0xD0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x88 , 0x54 , 0xD0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x88 , 0x94 , 0xD0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x88 , 0xD4 , 0xD0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x88 , 0x14 , 0xD2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_112_SWITCH[] =
{
   /* CB1: ATP = 000, RS = 000 (TOP112dBuV, 166.7kHz)
           => 10000000 = 0x80 */
   /* CB2: ATC = 1, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11110000/11110010 = 0xF0 / 0xF2*/

   /* BB:  AISL = 0, P5 = 0 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x80 , 0x01 , 0xF0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x80 , 0x02 , 0xF0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x80 , 0x44 , 0xF0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x80 , 0x84 , 0xF0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x80 , 0xC4 , 0xF0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x80 , 0x04 , 0xF2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_112_SWITCH[] =
{
   /* CB1: ATP = 000, RS = 000 (TOP112dBuV, 166.7kHz)
           => 10000000 = 0x80 */
   /* CB2: ATC = 1, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11110000/11110010 = 0xF0 / 0xF2*/

   /* BB:  AISL = 0, P5 = 1 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x80 , 0x11 , 0xF0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x80 , 0x12 , 0xF0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x80 , 0x54 , 0xF0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x80 , 0x94 , 0xF0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x80 , 0xD4 , 0xF0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x80 , 0x14 , 0xF2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_112_LOCK[] =
{
   /* CB1: ATP = 000, RS = 000 (TOP112dBuV, 166.7kHz)
           => 10000000 = 0x80 */
   /* CB2: ATC = 0, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11010000/11010010 = 0xD0 / 0xD2*/

   /* BB:  AISL = 0, P5 = 0 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x80 , 0x01 , 0xD0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x80 , 0x02 , 0xD0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x80 , 0x44 , 0xD0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x80 , 0x84 , 0xD0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x80 , 0xC4 , 0xD0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x80 , 0x04 , 0xD2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_112_LOCK[] =
{
   /* CB1: ATP = 000, RS = 000 (TOP112dBuV, 166.7kHz)
           => 10000000 = 0x80 */
   /* CB2: ATC = 0, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11010000/11010010 = 0xD0 / 0xD2*/

   /* BB:  AISL = 0, P5 = 1 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0x80 , 0x11 , 0xD0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0x80 , 0x12 , 0xD0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0x80 , 0x54 , 0xD0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0x80 , 0x94 , 0xD0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0x80 , 0xD4 , 0xD0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0x80 , 0x14 , 0xD2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_166KHZ_EXT[] =
{
   /* CB1: ATP = 111, RS = 000 (Ext., 166.7kHz)
           => 10111000 = 0xB8 */
   /* CB2: ATC = 0, DISGCA = 0, IFDA = 0, CP2 = 1/0
           => 11010000/11010010 = 0xD0 / 0xD2*/

   /* BB:  AISL = 0, P5 = 1 */
   /*     - S6  : CP 000, BS = 0001 */ { 146000 , { 0xB8 , 0x11 , 0xD0 } },
   /* E5  - E12 : CP 000, BS = 0010 */ { 230000 , { 0xB8 , 0x12 , 0xD0 } },
   /* E21 - E28 : CP 001, BS = 0100 */ { 534000 , { 0xB8 , 0x54 , 0xD0 } },
   /* E29 - E65 : CP 010, BS = 0100 */ { 830000 , { 0xB8 , 0x94 , 0xD0 } },
   /* E66 - E67 : CP 011, BS = 0100 */ { 846000 , { 0xB8 , 0xD4 , 0xD0 } },
   /* E68 - E69 : CP 100, BS = 0100 */ { MAXFRQ , { 0xB8 , 0x14 , 0xD2 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_62KHZ_7MHZ_SWITCH[] =
{
   /* CB1: ATP = 100, RS = 011 (TOP104dBuV, 62.5kHz)
           => 10100011 = 0xA3 */
   /* CB2: ATC = 1, DISGCA = 0, IFDA = 0, CP2 = 0
           => 11110000 = 0xF0 */

   /* BB:  CP = 000, AISL = 0, P5 = 0 */
   /* IRA - S6  : BS = 0001 */ { 146000 , { 0xA3 , 0x01 , 0xF0 } },
   /* S7  - S36 : BS = 0010 */ { 430000 , { 0xA3 , 0x02 , 0xF0 } },
   /* S37 - E69 : BS = 0100 */ { MAXFRQ , { 0xA3 , 0x04 , 0xF0 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_62KHZ_8MHZ_SWITCH[] =
{
   /* CB1: ATP = 100, RS = 011 (TOP104dBuV, 62.5kHz)
           => 10100011 = 0xA3 */
   /* CB2: ATC = 1, DISGCA = 0, IFDA = 0, CP2 = 0
           => 11110000 = 0xF0 */

   /* BB:  CP = 000, AISL = 0, P5 = 1 */
   /* IRA - S6  : BS = 0001 */ { 146000 , { 0xA3 , 0x11 , 0xF0 } },
   /* S7  - S36 : BS = 0010 */ { 430000 , { 0xA3 , 0x12 , 0xF0 } },
   /* S37 - E69 : BS = 0100 */ { MAXFRQ , { 0xA3 , 0x14 , 0xF0 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_62KHZ_7MHZ_LOCK[] =
{
   /* CB1: ATP = 100, RS = 011 (TOP104dBuV, 62.5kHz)
           => 10100011 = 0xA3 */
   /* CB2: ATC = 0, DISGCA = 0, IFDA = 0, CP2 = 0
           => 11110000 = 0xF0 */

   /* BB:  CP = 000, AISL = 0, P5 = 0 */
   /* IRA - S6  : BS = 0001 */ { 146000 , { 0xA3 , 0x01 , 0xD0 } },
   /* S7  - S36 : BS = 0010 */ { 430000 , { 0xA3 , 0x02 , 0xD0 } },
   /* S37 - E69 : BS = 0100 */ { MAXFRQ , { 0xA3 , 0x04 , 0xD0 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_62KHZ_8MHZ_LOCK[] =
{
   /* CB1: ATP = 100, RS = 011 (TOP104dBuV, 62.5kHz)
           => 10100011 = 0xA3 */
   /* CB2: ATC = 0, DISGCA = 0, IFDA = 0, CP2 = 0
           => 11110000 = 0xF0 */

   /* BB:  CP = 000, AISL = 0, P5 = 1 */
   /* IRA - S6  : BS = 0001 */ { 146000 , { 0xA3 , 0x11 , 0xD0 } },
   /* S7  - S36 : BS = 0010 */ { 430000 , { 0xA3 , 0x12 , 0xD0 } },
   /* S37 - E69 : BS = 0100 */ { MAXFRQ , { 0xA3 , 0x14 , 0xD0 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENG37F01KF_62KHZ_EXT[] =
{
   /* CB1: ATP = 111, RS = 011 (Ext, 62.5kHz)
           => 10111011 = 0xBB */
   /* CB2: ATC = 0, DISGCA = 0, IFDA = 0, CP2 = 0
           => 11110000 = 0xF0 */

   /* BB:  CP = 000, AISL = 0, P5 = 1 */
   /* IRA - S6  : BS = 0001 */ { 146000 , { 0xBB , 0x11 , 0xD0 } },
   /* S7  - S36 : BS = 0010 */ { 430000 , { 0xBB , 0x12 , 0xD0 } },
   /* S37 - E69 : BS = 0100 */ { MAXFRQ , { 0xBB , 0x14 , 0xD0 } }
};


static TUNERCANNEDControlTable_t TCT_PANASONIC_ENG37F01KF_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz, TOP 108dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_108_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz, TOP 108dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_108_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz, TOP 108dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_108_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz, TOP 108dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_108_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz, TOP 104dBuV",
      /* refFreqNumerator   [kHz] */    125,
      /* refFreqDenominator       */      2,
      /* outputFrequency    [kHz] */  36125,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_62KHZ_7MHZ_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz, TOP 104dBuV",
      /* refFreqNumerator   [kHz] */    125,
      /* refFreqDenominator       */      2,
      /* outputFrequency    [kHz] */  36125,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_62KHZ_7MHZ_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz, TOP 104dBuV",
      /* refFreqNumerator   [kHz] */    125,
      /* refFreqDenominator       */      2,
      /* outputFrequency    [kHz] */  36125,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_62KHZ_8MHZ_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz, TOP 104dBuV",
      /* refFreqNumerator   [kHz] */    125,
      /* refFreqDenominator       */      2,
      /* outputFrequency    [kHz] */  36125,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_62KHZ_8MHZ_LOCK
   },
   {

      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "166kHz, Ext.",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_EXT
   },
   {

      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "62.5kHz, Ext.",
      /* refFreqNumerator   [kHz] */    125,
      /* refFreqDenominator       */      2,
      /* outputFrequency    [kHz] */  36125,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_62KHZ_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "166.7kHz, TOP 110dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_110_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "166.7kHz, TOP 110dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_110_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "166.7kHz, TOP 110dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_110_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "166.7kHz, TOP 110dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_110_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB5,
      /* description              */ "166.7kHz, TOP 110dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_112_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB5,
      /* description              */ "166.7kHz, TOP 112dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_7MHZ_112_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB5,
      /* description              */ "166.7kHz, TOP 112dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_112_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB5,
      /* description              */ "166.7kHz, TOP 112dBuV",
      /* refFreqNumerator   [kHz] */    500,
      /* refFreqDenominator       */      3,
      /* outputFrequency    [kHz] */  36167,
      /* controlUnits             */ TCU_PANASONIC_ENG37F01KF_166KHZ_8MHZ_112_LOCK
   }

};

static TUNERSubMode_t subModesPanasonicENG37F01KF_K[] =
{
   "TOP 108dBuV, OFDM",    /* Submode 0 */
   "TOP 104dBuV, QAM",     /* Submode 1 */
   "Ext. RF-AGC 166kHz",   /* Submode 2 */
   "Ext. RF-AGC 62.5kHz",  /* Submode 3 */
   "TOP 110dBuV, OFDM",    /* Submode 4 */
   "TOP 112dBuV, OFDM"     /* Submode 5 */

};

static TUNERCANNEDData_t dataPanasonicENG37F01KF_K =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_PANASONIC_ENG37F01KF_K),
   /* nrOfControlBytes  */          3,
};

#endif /*   defined TUNER_PANASONIC_ENG37F01KF || \
            defined TUNER_PANASONIC_ET35DRV */

#ifdef TUNER_PANASONIC_ENG37F01KF
static TUNERCommonAttr_t commonAttrPanasonicENG37F01KF_K =
{
   /* name            */  "Panasonic ENG37F01KF",
   /* minFreqRF [kHz] */      47000,
   /* maxFreqRF [kHz] */     862000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesPanasonicENG37F01KF_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerPanasonicENG37F01KF =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrPanasonicENG37F01KF_K,
   /* myExtAttr    */ &dataPanasonicENG37F01KF_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif

#ifdef TUNER_MIC_PANASONIC_ENG37F01KF
static TUNERCommonAttr_t commonAttrMicPanasonicENG37F01KF_K =
{
   /* name            */  "Mic Panasonic ENG37F01KF",
   /* minFreqRF [kHz] */      47000,
   /* maxFreqRF [kHz] */     862000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesPanasonicENG37F01KF_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerMicPanasonicENG37F01KF =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrMicPanasonicENG37F01KF_K,
   /* myExtAttr    */ &dataPanasonicENG37F01KF_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif

#ifdef TUNER_PANASONIC_ET35DRV
static TUNERCommonAttr_t commonAttrPanasonicET35DRV_K =
{
   /* name            */  "Panasonic ET35DRV",
   /* minFreqRF [kHz] */      47000,
   /* maxFreqRF [kHz] */     862000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesPanasonicENG37F01KF_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerPanasonicET35DRV =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrPanasonicET35DRV_K,
   /* myExtAttr    */ &dataPanasonicENG37F01KF_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Panasonic-ENG37F01KF------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Panasonic ENV57M06D8F tuner configuration                                  */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_PANASONIC_ENV57M06D8F
#define TUNER_PANASONIC_ENV57M06D8F
#endif
#endif

#ifdef  TUNER_PANASONIC_ENV57M06D8F
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerPanasonicENV57M06D8F
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENV57M06D8F_BS_8_ANA[] =
{
   /* RS = 11 , OS = 0 , XTB = 0 */
   /* E2  - S8       - 160 MHz: CP/T = 0110, BS = 00001 */ { 160000 , { 0xB6 , 0x01 } },
   /* S9  - S38  160 - 446 MHz: CP/T = 0110, BS = 00010 */ { 446000 , { 0xB6 , 0x02 } },
   /* S39 - E69  446 -     MHz: CP/T = 0110, BS = 00100 */ { MAXFRQ , { 0xB6 , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENV57M06D8F_BS_7_ANA[] =
{
   /* RS = 11 , OS = 0 , XTB = 0 */
   /* E2  - S8       - 160 MHz: CP/T = 0110, BS = 00001 */ { 160000 , { 0xB6 , 0x11 } },
   /* S9  - S38  160 - 446 MHz: CP/T = 0110, BS = 00010 */ { 446000 , { 0xB6 , 0x12 } },
   /* S39 - E69  446 -     MHz: CP/T = 0110, BS = 00100 */ { MAXFRQ , { 0xB6 , 0x14 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENV57M06D8F_BS_8_166_DIG[] =
{
   /* RS = 10 , OS = 0, XTB = 1 */
   /* E2  - S5      - 139  MHz: CP/T 0110, BS = 00001 */ { 139000 , { 0xB4 , 0x81 } },
   /* S6  - S8  139 - 160  MHz: CP/T 0111, BS = 00001 */ { 160000 , { 0xBC , 0x81 } },
   /* S9  - S30 160 - 382  MHz: CP/T 0110, BS = 00010 */ { 382000 , { 0xB4 , 0x82 } },
   /* S31 - S38 382 - 446  MHz: CP/T 0111, BS = 00010 */ { 446000 , { 0xBC , 0x82 } },
   /* S39 - E32 446 - 566  MHz: CP/T 0110, BS = 00100 */ { 566000 , { 0xB4 , 0x84 } },
   /* E33 - E63 566 - 814  MHz: CP/T 0111, BS = 00100 */ { 814000 , { 0xBC , 0x84 } },
   /* E63 - E69 814 - 862  MHz: CP/T 1110, BS = 00100 */ { MAXFRQ , { 0xF4 , 0x84 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENV57M06D8F_BS_7_166_DIG[] =
{
   /* RS = 10 , OS = 0, XTB = 1 */
   /* E2  - S5      - 139  MHz: CP/T 0110, BS = 10001 */ { 139000 , { 0xB4 , 0x91 } },
   /* S6  - S8  139 - 160  MHz: CP/T 0111, BS = 10001 */ { 160000 , { 0xBC , 0x91 } },
   /* S9  - S30 160 - 382  MHz: CP/T 0110, BS = 10010 */ { 382000 , { 0xB4 , 0x92 } },
   /* S31 - S38 382 - 446  MHz: CP/T 0111, BS = 10010 */ { 446000 , { 0xBC , 0x92 } },
   /* S39 - E32 446 - 566  MHz: CP/T 0110, BS = 10100 */ { 566000 , { 0xB4 , 0x94 } },
   /* E33 - E63 566 - 814  MHz: CP/T 0111, BS = 10100 */ { 814000 , { 0xBC , 0x94 } },
   /* E63 - E69 814 - 862  MHz: CP/T 1110, BS = 10100 */ { MAXFRQ , { 0xF4 , 0x94 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENV57M06D8F_BS_8_625_DIG[] =
{
   /* RS = 11 , OS = 0, XTB = 1 */
   /* E2  - S5      - 139  MHz: CP/T 0110, BS = 00001 */ { 139000 , { 0xB6 , 0x81 } },
   /* S6  - S8  139 - 160  MHz: CP/T 0111, BS = 00001 */ { 160000 , { 0xBE , 0x81 } },
   /* S9  - S30 160 - 382  MHz: CP/T 0110, BS = 00010 */ { 382000 , { 0xB6 , 0x82 } },
   /* S31 - S38 382 - 446  MHz: CP/T 0111, BS = 00010 */ { 446000 , { 0xBE , 0x82 } },
   /* S39 - E32 446 - 566  MHz: CP/T 0110, BS = 00100 */ { 566000 , { 0xB6 , 0x84 } },
   /* E33 - E63 566 - 814  MHz: CP/T 0111, BS = 00100 */ { 814000 , { 0xBE , 0x84 } },
   /* E63 - E69 814 - 862  MHz: CP/T 1110, BS = 00100 */ { MAXFRQ , { 0xF6 , 0x84 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENV57M06D8F_BS_7_625_DIG[] =
{
   /* RS = 11 , OS = 0, XTB = 1 */
   /* E2  - S5      - 139  MHz: CP/T 0110, BS = 10001 */ { 139000 , { 0xB6 , 0x91 } },
   /* S6  - S8  139 - 160  MHz: CP/T 0111, BS = 10001 */ { 160000 , { 0xBE , 0x91 } },
   /* S9  - S30 160 - 382  MHz: CP/T 0110, BS = 10010 */ { 382000 , { 0xB6 , 0x92 } },
   /* S31 - S38 382 - 446  MHz: CP/T 0111, BS = 10010 */ { 446000 , { 0xBE , 0x92 } },
   /* S39 - E32 446 - 566  MHz: CP/T 0110, BS = 10100 */ { 566000 , { 0xB6 , 0x94 } },
   /* E33 - E63 566 - 814  MHz: CP/T 0111, BS = 10100 */ { 814000 , { 0xBE , 0x94 } },
   /* E63 - E69 814 - 862  MHz: CP/T 1110, BS = 10100 */ { MAXFRQ , { 0xF6 , 0x94 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENV57M06D8F_AUX_625_EXT_LOCK[] =
{
   /* CP = 0 , T = 011 , RS = 11 */
   /* ATC = 0 , AL = 110 */ { MAXFRQ , { 0xB6 , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENV57M06D8F_AUX_166_EXT_LOCK[] =
{
   /* CP = 0 , T = 011 , RS = 10 */
   /* ATC = 0 , AL = 110 */ { MAXFRQ , { 0xB4 , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENV57M06D8F_AUX_625_EXT_SWITCH[] =
{
   /* CP = 0 , T = 011 , RS = 11 */
   /* ATC = 1 , AL = 110 */ { MAXFRQ , { 0xB6 , 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_PANASONIC_ENV57M06D8F_AUX_166_EXT_SWITCH[] =
{
   /* CP = 0 , T = 011 , RS = 10 */
   /* ATC = 1 , AL = 110 */ { MAXFRQ , { 0xB4 , 0xE0 } }
};


static TUNERCANNEDControlTable_t TCT_PANASONIC_ENV57M06D8F[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0 |
                                     TUNER_MODE_SUB1,
      /* description              */ "Aux, stepsize 62.5kHz, EXT. RFAGC, Switch",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENV57M06D8F_AUX_625_EXT_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0 |
                                     TUNER_MODE_SUB1,
      /* description              */ "Aux, stepsize 62.5kHz, EXT. RFAGC, Lock",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENV57M06D8F_AUX_625_EXT_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Aux, stepsize 166kHz, EXT. RFAGC, Switch",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENV57M06D8F_AUX_166_EXT_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Aux, stepsize 166kHz, EXT. RFAGC, Lock",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENV57M06D8F_AUX_166_EXT_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "BS, stepsize 62.5kHz, ANA",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENV57M06D8F_BS_7_ANA
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "BS, stepsize 62.5kHz, ANA",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENV57M06D8F_BS_8_ANA
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "BS, stepsize 62.5kHz, DIG",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENV57M06D8F_BS_7_625_DIG
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "BS, stepsize 62.5kHz, DIG",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENV57M06D8F_BS_8_625_DIG
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "BS, stepsize 166kHz, DIG",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENV57M06D8F_BS_7_166_DIG
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "BS, stepsize 166kHz, DIG",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_PANASONIC_ENV57M06D8F_BS_8_166_DIG
   }


};

static TUNERSubMode_t subModesPanasonicENV57M06D8F[] =
{
   "EXT. RF-AGC, ATV 62.5 kHz",  /* Submode 0 */
   "EXT. RF-AGC, DIG 62.5 kHz",  /* Submode 1 */
   "EXT. RF-AGC, DIG 166.7 kHz"  /* Submode 2 */
};

static TUNERCANNEDData_t dataPanasonicENV57M06D8F =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_PANASONIC_ENV57M06D8F),
   /* nrOfControlBytes  */          2,
};

static TUNERCommonAttr_t commonAttrPanasonicENV57M06D8F =
{
   /* name            */  "Panasonic ENV57M06D8F",
   /* minFreqRF [kHz] */      47000,
   /* maxFreqRF [kHz] */     862000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesPanasonicENV57M06D8F),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerPanasonicENV57M06D8F =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrPanasonicENV57M06D8F,
   /* myExtAttr    */ &dataPanasonicENV57M06D8F,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Panasonic-ENG37ExxKF------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Samsung DNOS40AZH201J tuner configuration                                  */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DNOS40AZH201J
#define TUNER_SSEM_DNOS40AZH201J
#endif
#endif

#ifdef  TUNER_SSEM_DNOS40AZH201J
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSSEM_DNOS40AZH201J
#endif

#include "tunersamsungdnosazh201j.h"

static TUNERCANNEDControlUnit_t TCU_SSEM_DNOS40AZH201J_166KHZ_DIV[] =
{
   /*         - 144 MHz : VHF-L  */
   /* 144 MHz - 430 MHz : VHF-H  */
   /* 430 MHz -         : UHF    */
   /* CB1 = 0xC2; T/A = 1, T = 000 (Xbuf off), R = 010 (166.7 kHz) */

   /*  50.5 - 142.5 MHz: CP = 010, BS = 00001 */ { 144000 , { 0xC2 , 0x41 } },
   /* 144.0 - 296.5 MHz: CP = 011, BS = 00010 */ { 300000 , { 0xC2 , 0x62 } },
   /* 306.0 - 426.0 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xC2 , 0x82 } },
   /* 434.0 - 602.0 MHz: CP = 101, BS = 01000 */ { 606000 , { 0xC2 , 0xA8 } },
   /* 610.0 - 858.0 MHz: CP = 110, BS = 01000 */ { MAXFRQ , { 0xC2 , 0xC8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DNOS40AZH201J_50KHZ_DIV[] =
{
   /*         - 144 MHz : VHF-L  */
   /* 144 MHz - 430 MHz : VHF-H  */
   /* 430 MHz -         : UHF    */
   /* CB1 = 0xC3; T/A = 1, T = 000 (Xbuf off), R = 011 (166.7 kHz) */

   /*  50.5 - 142.5 MHz: CP = 010, BS = 00001 */ { 144000 , { 0xC3 , 0x41 } },
   /* 144.0 - 296.5 MHz: CP = 011, BS = 00010 */ { 300000 , { 0xC3 , 0x62 } },
   /* 306.0 - 426.0 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xC3 , 0x82 } },
   /* 434.0 - 602.0 MHz: CP = 101, BS = 01000 */ { 606000 , { 0xC3 , 0xA8 } },
   /* 610.0 - 858.0 MHz: CP = 110, BS = 01000 */ { MAXFRQ , { 0xC3 , 0xC8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DNOS40AZH201J_SWITCH_LNAOFF[] =
{
   /*         - 144 MHz : VHF-L  */
   /* 144 MHz - 430 MHz : VHF-H  */
   /* 430 MHz -         : UHF    */
   /* CB1 = 0x89; ATC = 1, AL = 001 TOP 121dBuV for VHF */
   /* CB1 = 0x8A; ATC = 1, AL = 010 TOP 118dBuV for UHF */

   /* BS3 = 0; LNA OFF */
   /*  50.5 - 142.5 MHz: CP = 010, BS = 00001 */ { 144000 , { 0x89, 0x41 } },
   /* 144.0 - 296.5 MHz: CP = 011, BS = 00010 */ { 300000 , { 0x89, 0x62 } },
   /* 306.0 - 426.0 MHz: CP = 100, BS = 00010 */ { 430000 , { 0x89, 0x82 } },
   /* 434.0 - 602.0 MHz: CP = 101, BS = 01000 */ { 606000 , { 0x8A, 0xA8 } },
   /* 610.0 - 858.0 MHz: CP = 110, BS = 01000 */ { MAXFRQ , { 0x8A, 0xC8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DNOS40AZH201J_LOCK_LNAOFF[] =
{
   /*         - 144 MHz : VHF-L  */
   /* 144 MHz - 430 MHz : VHF-H  */
   /* 430 MHz -         : UHF    */
   /* CB1 = 0x81; ATC = 0, AL = 001 TOP 121dBuV for VHF */
   /* CB1 = 0x82; ATC = 0, AL = 010 TOP 118dBuV for UHF */

   /* BS3 = 0; LNA OFF */
   /*  50.5 - 142.5 MHz: CP = 010, BS = 00001 */ { 144000 , { 0x81, 0x41 } },
   /* 144.0 - 296.5 MHz: CP = 011, BS = 00010 */ { 300000 , { 0x81, 0x62 } },
   /* 306.0 - 426.0 MHz: CP = 100, BS = 00010 */ { 430000 , { 0x81, 0x82 } },
   /* 434.0 - 602.0 MHz: CP = 101, BS = 01000 */ { 606000 , { 0x82, 0xA8 } },
   /* 610.0 - 858.0 MHz: CP = 110, BS = 01000 */ { MAXFRQ , { 0x82, 0xC8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DNOS40AZH201J_SWITCH_LNAON[] =
{
   /*         - 144 MHz : VHF-L  */
   /* 144 MHz - 430 MHz : VHF-H  */
   /* 430 MHz -         : UHF    */
   /* CB1 = 0x89; ATC = 1, AL = 001 TOP 121dBuV for VHF */
   /* CB1 = 0x8A; ATC = 1, AL = 010 TOP 118dBuV for UHF */

   /* BS3 = 1; LNA ON */
   /*  50.5 - 142.5 MHz: CP = 010, BS = 00101 */ { 144000 , { 0x89, 0x45 } },
   /* 144.0 - 296.5 MHz: CP = 011, BS = 00110 */ { 300000 , { 0x89, 0x66 } },
   /* 306.0 - 426.0 MHz: CP = 100, BS = 00110 */ { 430000 , { 0x89, 0x86 } },
   /* 434.0 - 602.0 MHz: CP = 101, BS = 01100 */ { 606000 , { 0x8A, 0xAC } },
   /* 610.0 - 858.0 MHz: CP = 110, BS = 01100 */ { MAXFRQ , { 0x8A, 0xCC } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DNOS40AZH201J_LOCK_LNAON[] =
{
   /*         - 144 MHz : VHF-L  */
   /* 144 MHz - 430 MHz : VHF-H  */
   /* 430 MHz -         : UHF    */
   /* CB1 = 0x81; ATC = 0, AL = 001 TOP 121dBuV for VHF */
   /* CB1 = 0x82; ATC = 0, AL = 010 TOP 118dBuV for UHF */

   /* BS3 = 1; LNA ON */
   /*  50.5 - 142.5 MHz: CP = 010, BS = 00101 */ { 144000 , { 0x81, 0x45 } },
   /* 144.0 - 296.5 MHz: CP = 011, BS = 00110 */ { 300000 , { 0x81, 0x66 } },
   /* 306.0 - 426.0 MHz: CP = 100, BS = 00110 */ { 430000 , { 0x81, 0x86 } },
   /* 434.0 - 602.0 MHz: CP = 101, BS = 01100 */ { 606000 , { 0x82, 0xAC } },
   /* 610.0 - 858.0 MHz: CP = 110, BS = 01100 */ { MAXFRQ , { 0x82, 0xCC } }
};

static TUNERCANNEDControlTable_t TCT_SSEM_DNOS40AZH201J[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH        |
                                     TUNER_MODE_678MHZ        |
                                     TUNER_MODE_SUB0          |
                                     TUNER_MODE_SUB1,
      /* description              */ "167kHZ stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DNOS40AZH201J_166KHZ_DIV
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH        |
                                     TUNER_MODE_678MHZ        |
                                     TUNER_MODE_SUB0,
      /* description              */ "167kHZ stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DNOS40AZH201J_SWITCH_LNAOFF
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK          |
                                     TUNER_MODE_678MHZ        |
                                     TUNER_MODE_SUB0,
      /* description              */ "167kHZ stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DNOS40AZH201J_LOCK_LNAOFF
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH        |
                                     TUNER_MODE_678MHZ        |
                                     TUNER_MODE_SUB1,
      /* description              */ "167kHZ stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DNOS40AZH201J_SWITCH_LNAON
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK          |
                                     TUNER_MODE_678MHZ        |
                                     TUNER_MODE_SUB1,
      /* description              */ "167kHZ stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DNOS40AZH201J_LOCK_LNAON
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH        |
                                     TUNER_MODE_678MHZ        |
                                     TUNER_MODE_SUB2          |
                                     TUNER_MODE_SUB3,
      /* description              */ "50kHZ stepsize",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DNOS40AZH201J_50KHZ_DIV
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH        |
                                     TUNER_MODE_678MHZ        |
                                     TUNER_MODE_SUB2,
      /* description              */ "50kHZ stepsize",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DNOS40AZH201J_SWITCH_LNAOFF
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK          |
                                     TUNER_MODE_678MHZ        |
                                     TUNER_MODE_SUB2,
      /* description              */ "50kHZ stepsize",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DNOS40AZH201J_LOCK_LNAOFF
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH        |
                                     TUNER_MODE_678MHZ        |
                                     TUNER_MODE_SUB3,
      /* description              */ "50kHZ stepsize",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DNOS40AZH201J_SWITCH_LNAON
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK          |
                                     TUNER_MODE_678MHZ        |
                                     TUNER_MODE_SUB3,
      /* description              */ "50kHZ stepsize",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DNOS40AZH201J_LOCK_LNAON
   }
};

static TUNERSubMode_t subModesSSEM_DNOS40AZH201J[] =
{
   "Auto-TOP, LNA Off, 167 kHz",  /* Submode 0 */
   "Auto-TOP, LNA On,  167 kHz",  /* Submode 1 */
   "Auto-TOP, LNA Off,  50 kHz",  /* Submode 2 */
   "Auto-TOP, LNA On,   50 kHz"   /* Submode 3 */
};

static TUNERCANNEDData_t dataSSEM_DNOS40AZH201J =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_SSEM_DNOS40AZH201J),
   /* nrOfControlBytes  */          2,
};

static TUNERCommonAttr_t commonAttrSSEM_DNOS40AZH201J =
{
   /* name            */  "Samsung DNOS40AZH201J",
   /* minFreqRF [kHz] */      47000,
   /* maxFreqRF [kHz] */     862000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesSSEM_DNOS40AZH201J),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerSSEM_DNOS40AZH201J =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSSEM_DNOS40AZH201J,
   /* myExtAttr    */ &dataSSEM_DNOS40AZH201J,
   /* myFunct      */ &TUNERSSEMDNOS40AZH201JFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Samsung-DNOS40AZH201J-----------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Samsung DTOS403LH121B tuner configuration                                  */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTOS403LH121B
#define TUNER_SSEM_DTOS403LH121B
#endif
#endif

#ifdef  TUNER_SSEM_DTOS403LH121B
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSSEM_DTOS403LH121B
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_166_7_EXT[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5 - 114.5 MHz: CP/T = 1110, P = 00001 */ { 114500, { 0xF4, 0x01, 0xDC, 0xE0 } },
   /* 121.5 - 135.5 MHz: CP/T = 1111, P = 00001 */ { 135500, { 0xFC, 0x01, 0xDC, 0xE0 } },
   /* 142.5 - 170.5 MHz: CP/T = 0110, P = 00001 */ { 170500, { 0xB4, 0x01, 0x9C, 0xE0 } },
   /* 177.5 - 314   MHz: CP/T = 1110, P = 00010 */ { 314000, { 0xF4, 0x02, 0xDC, 0xE0 } },
   /* 322   - 410   MHz: CP/T = 1111, P = 00010 */ { 410000, { 0xFC, 0x02, 0xDC, 0xE0 } },
   /* 418   - 434   MHz: CP/T = 0110, P = 00010 */ { 434000, { 0xB4, 0x02, 0x9C, 0xE0 } },
   /* 442   - 466   MHz: CP/T = 1111, P = 00010 */ { 466000, { 0xFC, 0x02, 0xDC, 0xE0 } },
   /* 474   - 706   MHz: CP/T = 1111, P = 10000 */ { 706000, { 0xFC, 0x10, 0xDC, 0xE0 } },
   /* 714   - 802   MHz: CP/T = 0110, P = 10000 */ { 802000, { 0xB4, 0x10, 0x9C, 0xE0 } },
   /* 810   - 858   MHz: CP/T = 0111, P = 10000 */ { MAXFRQ, { 0xBC, 0x10, 0x9C, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_166_8_EXT[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5 - 114.5 MHz: CP/T = 1110, P = 00101 */ { 114500, { 0xF4, 0x05, 0xDC, 0xE0 } },
   /* 121.5 - 135.5 MHz: CP/T = 1111, P = 00101 */ { 135500, { 0xFC, 0x05, 0xDC, 0xE0 } },
   /* 142.5 - 170.5 MHz: CP/T = 0110, P = 00101 */ { 170500, { 0xB4, 0x05, 0x9C, 0xE0 } },
   /* 177.5 - 314   MHz: CP/T = 1110, P = 00110 */ { 314000, { 0xF4, 0x06, 0xDC, 0xE0 } },
   /* 322   - 410   MHz: CP/T = 1111, P = 00110 */ { 410000, { 0xFC, 0x06, 0xDC, 0xE0 } },
   /* 418   - 434   MHz: CP/T = 0110, P = 00110 */ { 434000, { 0xB4, 0x06, 0x9C, 0xE0 } },
   /* 442   - 466   MHz: CP/T = 1111, P = 00110 */ { 466000, { 0xFC, 0x06, 0xDC, 0xE0 } },
   /* 474   - 706   MHz: CP/T = 1111, P = 10100 */ { 706000, { 0xFC, 0x14, 0xDC, 0xE0 } },
   /* 714   - 802   MHz: CP/T = 0110, P = 10100 */ { 802000, { 0xB4, 0x14, 0x9C, 0xE0 } },
   /* 810   - 858   MHz: CP/T = 0111, P = 10100 */ { MAXFRQ, { 0xBC, 0x14, 0x9C, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_625_7_EXT[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  114.5 MHz: CP/T = 1110, P = 00001 */ { 114500, { 0xF6, 0x01, 0xDE, 0xE0 } },
   /* 121.5   -  135.5 MHz: CP/T = 1111, P = 00001 */ { 135500, { 0xFE, 0x01, 0xDE, 0xE0 } },
   /* 142.5   -  170.5 MHz: CP/T = 0110, P = 00001 */ { 170500, { 0xB6, 0x01, 0x9E, 0xE0 } },
   /* 177.5   -  314   MHz: CP/T = 1110, P = 00010 */ { 314000, { 0xF6, 0x02, 0xDE, 0xE0 } },
   /* 322     -  410   MHz: CP/T = 1111, P = 00010 */ { 410000, { 0xFE, 0x02, 0xDE, 0xE0 } },
   /* 418     -  434   MHz: CP/T = 0110, P = 00010 */ { 434000, { 0xB6, 0x02, 0x9E, 0xE0 } },
   /* 442     -  466   MHz: CP/T = 1111, P = 00010 */ { 466000, { 0xFE, 0x02, 0xDE, 0xE0 } },
   /* 474     -  706   MHz: CP/T = 1111, P = 10000 */ { 706000, { 0xFE, 0x10, 0xDE, 0xE0 } },
   /* 714     -  802   MHz: CP/T = 0110, P = 10000 */ { 802000, { 0xB6, 0x10, 0x9E, 0xE0 } },
   /* 810     -  858   MHz: CP/T = 0111, P = 10000 */ { MAXFRQ, { 0xBE, 0x10, 0x9E, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_625_8_EXT[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  114.5 MHz: CP/T = 1110, P = 00101 */ { 114500, { 0xF6, 0x05, 0xDE, 0xE0 } },
   /* 121.5   -  135.5 MHz: CP/T = 1111, P = 00101 */ { 135500, { 0xFE, 0x05, 0xDE, 0xE0 } },
   /* 142.5   -  170.5 MHz: CP/T = 0110, P = 00101 */ { 170500, { 0xB6, 0x05, 0x9E, 0xE0 } },
   /* 177.5   -  314   MHz: CP/T = 1110, P = 00110 */ { 314000, { 0xF6, 0x06, 0xDE, 0xE0 } },
   /* 322     -  410   MHz: CP/T = 1111, P = 00110 */ { 410000, { 0xFE, 0x06, 0xDE, 0xE0 } },
   /* 418     -  434   MHz: CP/T = 0110, P = 00110 */ { 434000, { 0xB6, 0x06, 0x9E, 0xE0 } },
   /* 442     -  466   MHz: CP/T = 1111, P = 00110 */ { 466000, { 0xFE, 0x06, 0xDE, 0xE0 } },
   /* 474     -  706   MHz: CP/T = 1111, P = 10100 */ { 706000, { 0xFE, 0x14, 0xDE, 0xE0 } },
   /* 714     -  802   MHz: CP/T = 0110, P = 10100 */ { 802000, { 0xB6, 0x14, 0x9E, 0xE0 } },
   /* 810     -  858   MHz: CP/T = 0111, P = 10100 */ { MAXFRQ, { 0xBE, 0x14, 0x9E, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_625_7_EXT_ATV[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  170.5 MHz: CP/T = 11000, P = 00001 */ { 170500, { 0xC6, 0x01, 0xDE, 0xE0 } },
   /* 177.5   -  466   MHz: CP/T = 11000, P = 00010 */ { 466000, { 0xC6, 0x02, 0xDE, 0xE0 } },
   /* 474     -  858   MHz: CP/T = 11000, P = 10000 */ { MAXFRQ, { 0xC6, 0x10, 0xDE, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_625_8_EXT_ATV[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  170.5 MHz: CP/T = 1000, P = 00101 */ { 170500, { 0xC6, 0x05, 0xDE, 0xE0 } },
   /* 177.5   -  466   MHz: CP/T = 1000, P = 00110 */ { 466000, { 0xC6, 0x06, 0xDE, 0xE0 } },
   /* 474     -  858   MHz: CP/T = 1000, P = 10100 */ { MAXFRQ, { 0xC6, 0x14, 0xDE, 0xE0 } }
};

static TUNERCANNEDControlTable_t TCT_SSEM_DTOS403LH121B[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_67MHZ         |
                                     TUNER_MODE_SUB0,
      /* description              */ "167kHz stepsize, 7MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_166_7_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_8MHZ          |
                                     TUNER_MODE_SUB0,
      /* description              */ "167kHz stepsize, 8Mhz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_166_8_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_67MHZ         |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz stepsize, 7Mhz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_625_7_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_8MHZ          |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz stepsize, 8Mhz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_625_8_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_67MHZ         |
                                     TUNER_MODE_SUB2,
      /* description              */ "62.5kHz stepsize, 7Mhz, CP 50uA",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125, /* NOTE: IF center frequency */
      /* controlUnits             */ TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_625_7_EXT_ATV
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_8MHZ          |
                                     TUNER_MODE_SUB2,
      /* description              */ "62.5kHz stepsize, 8Mhz, CP 50uA",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125, /* NOTE: IF center frequency */
      /* controlUnits             */ TCU_SSEM_DTOS403LH121B_CB_BB_CBT_AB_625_8_EXT_ATV
   }
};

static TUNERSubMode_t subModesSSEM_DTOS403LH121B[] =
{
   "Ext. RF-AGC, DVBT",  /* Submode 0 */
   "Ext. RF-AGC, QAM",   /* Submode 1 */
   "Ext. RF-AGC, ATV"    /* Submode 2 */
};

static TUNERCANNEDData_t dataSSEM_DTOS403LH121B =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_SSEM_DTOS403LH121B),
   /* nrOfControlBytes  */          4,
};

static TUNERCommonAttr_t commonAttrSSEM_DTOS403LH121B =
{
   /* name            */  "Samsung DTOS403LH121B",
   /* minFreqRF [kHz] */      43000,
   /* maxFreqRF [kHz] */     870000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesSSEM_DTOS403LH121B),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerSSEM_DTOS403LH121B =
{
   /* I2C address  */ { 0xC0, TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSSEM_DTOS403LH121B,
   /* myExtAttr    */ &dataSSEM_DTOS403LH121B,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Samsung-DTOS403LH121B-----------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Samsung DTOS403SH081A tuner configuration                                  */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTOS403SH081A
#define TUNER_SSEM_DTOS403SH081A
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_166_7_EXT[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5 - 114.5 MHz: CP/T = 1110, P = 00001 */ { 114500, { 0xF4, 0x01, 0xDC, 0xE0 } },
   /* 121.5 - 135.5 MHz: CP/T = 1111, P = 00001 */ { 135500, { 0xFC, 0x01, 0xDC, 0xE0 } },
   /* 142.5 - 170.5 MHz: CP/T = 0110, P = 00001 */ { 170500, { 0xB4, 0x01, 0x9C, 0xE0 } },
   /* 177.5 - 314   MHz: CP/T = 1110, P = 00010 */ { 314000, { 0xF4, 0x02, 0xDC, 0xE0 } },
   /* 322   - 410   MHz: CP/T = 1111, P = 00010 */ { 410000, { 0xFC, 0x02, 0xDC, 0xE0 } },
   /* 418   - 434   MHz: CP/T = 0110, P = 00010 */ { 434000, { 0xB4, 0x02, 0x9C, 0xE0 } },
   /* 442   - 466   MHz: CP/T = 1111, P = 00010 */ { 466000, { 0xFC, 0x02, 0xDC, 0xE0 } },
   /* 474   - 706   MHz: CP/T = 1111, P = 10000 */ { 706000, { 0xFC, 0x10, 0xDC, 0xE0 } },
   /* 714   - 802   MHz: CP/T = 0110, P = 10000 */ { 802000, { 0xB4, 0x10, 0x9C, 0xE0 } },
   /* 810   - 858   MHz: CP/T = 0111, P = 10000 */ { MAXFRQ, { 0xBC, 0x10, 0x9C, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_166_8_EXT[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5 - 114.5 MHz: CP/T = 1110, P = 01001 */ { 114500, { 0xF4, 0x09, 0xDC, 0xE0 } },
   /* 121.5 - 135.5 MHz: CP/T = 1111, P = 01001 */ { 135500, { 0xFC, 0x09, 0xDC, 0xE0 } },
   /* 142.5 - 170.5 MHz: CP/T = 0110, P = 01001 */ { 170500, { 0xB4, 0x09, 0x9C, 0xE0 } },
   /* 177.5 - 314   MHz: CP/T = 1110, P = 01010 */ { 314000, { 0xF4, 0x0A, 0xDC, 0xE0 } },
   /* 322   - 410   MHz: CP/T = 1111, P = 01010 */ { 410000, { 0xFC, 0x0A, 0xDC, 0xE0 } },
   /* 418   - 434   MHz: CP/T = 0110, P = 01010 */ { 434000, { 0xB4, 0x0A, 0x9C, 0xE0 } },
   /* 442   - 466   MHz: CP/T = 1111, P = 01010 */ { 466000, { 0xFC, 0x0A, 0xDC, 0xE0 } },
   /* 474   - 706   MHz: CP/T = 1111, P = 11000 */ { 706000, { 0xFC, 0x18, 0xDC, 0xE0 } },
   /* 714   - 802   MHz: CP/T = 0110, P = 11000 */ { 802000, { 0xB4, 0x18, 0x9C, 0xE0 } },
   /* 810   - 858   MHz: CP/T = 0111, P = 11000 */ { MAXFRQ, { 0xBC, 0x18, 0x9C, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_7_EXT[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  114.5 MHz: CP/T = 1110, P = 00001 */ { 114500, { 0xF6, 0x01, 0xDE, 0xE0 } },
   /* 121.5   -  135.5 MHz: CP/T = 1111, P = 00001 */ { 135500, { 0xFE, 0x01, 0xDE, 0xE0 } },
   /* 142.5   -  170.5 MHz: CP/T = 0110, P = 00001 */ { 170500, { 0xB6, 0x01, 0x9E, 0xE0 } },
   /* 177.5   -  314   MHz: CP/T = 1110, P = 00010 */ { 314000, { 0xF6, 0x02, 0xDE, 0xE0 } },
   /* 322     -  410   MHz: CP/T = 1111, P = 00010 */ { 410000, { 0xFE, 0x02, 0xDE, 0xE0 } },
   /* 418     -  434   MHz: CP/T = 0110, P = 00010 */ { 434000, { 0xB6, 0x02, 0x9E, 0xE0 } },
   /* 442     -  466   MHz: CP/T = 1111, P = 00010 */ { 466000, { 0xFE, 0x02, 0xDE, 0xE0 } },
   /* 474     -  706   MHz: CP/T = 1111, P = 10000 */ { 706000, { 0xFE, 0x10, 0xDE, 0xE0 } },
   /* 714     -  802   MHz: CP/T = 0110, P = 10000 */ { 802000, { 0xB6, 0x10, 0x9E, 0xE0 } },
   /* 810     -  858   MHz: CP/T = 0111, P = 10000 */ { MAXFRQ, { 0xBE, 0x10, 0x9E, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_8_EXT[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  114.5 MHz: CP/T = 1110, P = 01001 */ { 114500, { 0xF6, 0x09, 0xDE, 0xE0 } },
   /* 121.5   -  135.5 MHz: CP/T = 1111, P = 01001 */ { 135500, { 0xFE, 0x09, 0xDE, 0xE0 } },
   /* 142.5   -  170.5 MHz: CP/T = 0110, P = 01001 */ { 170500, { 0xB6, 0x09, 0x9E, 0xE0 } },
   /* 177.5   -  314   MHz: CP/T = 1110, P = 01010 */ { 314000, { 0xF6, 0x0A, 0xDE, 0xE0 } },
   /* 322     -  410   MHz: CP/T = 1111, P = 01010 */ { 410000, { 0xFE, 0x0A, 0xDE, 0xE0 } },
   /* 418     -  434   MHz: CP/T = 0110, P = 01010 */ { 434000, { 0xB6, 0x0A, 0x9E, 0xE0 } },
   /* 442     -  466   MHz: CP/T = 1111, P = 01010 */ { 466000, { 0xFE, 0x0A, 0xDE, 0xE0 } },
   /* 474     -  706   MHz: CP/T = 1111, P = 11000 */ { 706000, { 0xFE, 0x18, 0xDE, 0xE0 } },
   /* 714     -  802   MHz: CP/T = 0110, P = 11000 */ { 802000, { 0xB6, 0x18, 0x9E, 0xE0 } },
   /* 810     -  858   MHz: CP/T = 0111, P = 11000 */ { MAXFRQ, { 0xBE, 0x18, 0x9E, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_7_EXT_ATV[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  170.5 MHz: CP/T = 1000, P = 00001 */ { 170500, { 0xC6, 0x01, 0xDE, 0xE0 } },
   /* 177.5   -  466   MHz: CP/T = 1000, P = 00010 */ { 466000, { 0xC6, 0x02, 0xDE, 0xE0 } },
   /* 474     -  858   MHz: CP/T = 1000, P = 10000 */ { MAXFRQ, { 0xC6, 0x10, 0xDE, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_8_EXT_ATV[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  170.5 MHz: CP/T = 1000, P = 01001 */ { 170500, { 0xC6, 0x09, 0xDE, 0xE0 } },
   /* 177.5   -  466   MHz: CP/T = 1000, P = 01010 */ { 466000, { 0xC6, 0x0A, 0xDE, 0xE0 } },
   /* 474     -  858   MHz: CP/T = 1000, P = 11000 */ { MAXFRQ, { 0xC6, 0x18, 0xDE, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_166_7_INT[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5 - 114.5 MHz: CP/T = 1110, P = 00001 */ { 114500, { 0xF4, 0x01, 0xDC, 0xB0 } },
   /* 121.5 - 135.5 MHz: CP/T = 1111, P = 00001 */ { 135500, { 0xFC, 0x01, 0xDC, 0xB0 } },
   /* 142.5 - 170.5 MHz: CP/T = 0110, P = 00001 */ { 170500, { 0xB4, 0x01, 0x9C, 0xB0 } },
   /* 177.5 - 314   MHz: CP/T = 1110, P = 00010 */ { 314000, { 0xF4, 0x02, 0xDC, 0xB0 } },
   /* 322   - 410   MHz: CP/T = 1111, P = 00010 */ { 410000, { 0xFC, 0x02, 0xDC, 0xB0 } },
   /* 418   - 434   MHz: CP/T = 0110, P = 00010 */ { 434000, { 0xB4, 0x02, 0x9C, 0xB0 } },
   /* 442   - 466   MHz: CP/T = 1111, P = 00010 */ { 466000, { 0xFC, 0x02, 0xDC, 0xB0 } },
   /* 474   - 706   MHz: CP/T = 1111, P = 10000 */ { 706000, { 0xFC, 0x10, 0xDC, 0xB0 } },
   /* 714   - 802   MHz: CP/T = 0110, P = 10000 */ { 802000, { 0xB4, 0x10, 0x9C, 0xB0 } },
   /* 810   - 858   MHz: CP/T = 0111, P = 10000 */ { MAXFRQ, { 0xBC, 0x10, 0x9C, 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_166_8_INT[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5 - 114.5 MHz: CP/T = 1110, P = 01001 */ { 114500, { 0xF4, 0x09, 0xDC, 0xB0 } },
   /* 121.5 - 135.5 MHz: CP/T = 1111, P = 01001 */ { 135500, { 0xFC, 0x09, 0xDC, 0xB0 } },
   /* 142.5 - 170.5 MHz: CP/T = 0110, P = 01001 */ { 170500, { 0xB4, 0x09, 0x9C, 0xB0 } },
   /* 177.5 - 314   MHz: CP/T = 1110, P = 01010 */ { 314000, { 0xF4, 0x0A, 0xDC, 0xB0 } },
   /* 322   - 410   MHz: CP/T = 1111, P = 01010 */ { 410000, { 0xFC, 0x0A, 0xDC, 0xB0 } },
   /* 418   - 434   MHz: CP/T = 0110, P = 01010 */ { 434000, { 0xB4, 0x0A, 0x9C, 0xB0 } },
   /* 442   - 466   MHz: CP/T = 1111, P = 01010 */ { 466000, { 0xFC, 0x0A, 0xDC, 0xB0 } },
   /* 474   - 706   MHz: CP/T = 1111, P = 11000 */ { 706000, { 0xFC, 0x18, 0xDC, 0xB0 } },
   /* 714   - 802   MHz: CP/T = 0110, P = 11000 */ { 802000, { 0xB4, 0x18, 0x9C, 0xB0 } },
   /* 810   - 858   MHz: CP/T = 0111, P = 11000 */ { MAXFRQ, { 0xBC, 0x18, 0x9C, 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_7_INT[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  114.5 MHz: CP/T = 1110, P = 00001 */ { 114500, { 0xF6, 0x01, 0xDE, 0xB0 } },
   /* 121.5   -  135.5 MHz: CP/T = 1111, P = 00001 */ { 135500, { 0xFE, 0x01, 0xDE, 0xB0 } },
   /* 142.5   -  170.5 MHz: CP/T = 0110, P = 00001 */ { 170500, { 0xB6, 0x01, 0x9E, 0xB0 } },
   /* 177.5   -  314   MHz: CP/T = 1110, P = 00010 */ { 314000, { 0xF6, 0x02, 0xDE, 0xB0 } },
   /* 322     -  410   MHz: CP/T = 1111, P = 00010 */ { 410000, { 0xFE, 0x02, 0xDE, 0xB0 } },
   /* 418     -  434   MHz: CP/T = 0110, P = 00010 */ { 434000, { 0xB6, 0x02, 0x9E, 0xB0 } },
   /* 442     -  466   MHz: CP/T = 1111, P = 00010 */ { 466000, { 0xFE, 0x02, 0xDE, 0xB0 } },
   /* 474     -  706   MHz: CP/T = 1111, P = 10000 */ { 706000, { 0xFE, 0x10, 0xDE, 0xB0 } },
   /* 714     -  802   MHz: CP/T = 0110, P = 10000 */ { 802000, { 0xB6, 0x10, 0x9E, 0xB0 } },
   /* 810     -  858   MHz: CP/T = 0111, P = 10000 */ { MAXFRQ, { 0xBE, 0x10, 0x9E, 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_8_INT[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  114.5 MHz: CP/T = 1110, P = 01001 */ { 114500, { 0xF6, 0x09, 0xDE, 0xB0 } },
   /* 121.5   -  135.5 MHz: CP/T = 1111, P = 01001 */ { 135500, { 0xFE, 0x09, 0xDE, 0xB0 } },
   /* 142.5   -  170.5 MHz: CP/T = 0110, P = 01001 */ { 170500, { 0xB6, 0x09, 0x9E, 0xB0 } },
   /* 177.5   -  314   MHz: CP/T = 1110, P = 01010 */ { 314000, { 0xF6, 0x0A, 0xDE, 0xB0 } },
   /* 322     -  410   MHz: CP/T = 1111, P = 01010 */ { 410000, { 0xFE, 0x0A, 0xDE, 0xB0 } },
   /* 418     -  434   MHz: CP/T = 0110, P = 01010 */ { 434000, { 0xB6, 0x0A, 0x9E, 0xB0 } },
   /* 442     -  466   MHz: CP/T = 1111, P = 01010 */ { 466000, { 0xFE, 0x0A, 0xDE, 0xB0 } },
   /* 474     -  706   MHz: CP/T = 1111, P = 11000 */ { 706000, { 0xFE, 0x18, 0xDE, 0xB0 } },
   /* 714     -  802   MHz: CP/T = 0110, P = 11000 */ { 802000, { 0xB6, 0x18, 0x9E, 0xB0 } },
   /* 810     -  858   MHz: CP/T = 0111, P = 11000 */ { MAXFRQ, { 0xBE, 0x18, 0x9E, 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_7_INT_ATV[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  170.5 MHz: CP/T = 1000, P = 00001 */ { 170500, { 0xC6, 0x01, 0xDE, 0xB0 } },
   /* 177.5   -  466   MHz: CP/T = 1000, P = 00010 */ { 466000, { 0xC6, 0x02, 0xDE, 0xB0 } },
   /* 474     -  858   MHz: CP/T = 1000, P = 10000 */ { MAXFRQ, { 0xC6, 0x10, 0xDE, 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_8_INT_ATV[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*  50.5   -  170.5 MHz: CP/T = 1000, P = 01001 */ { 170500, { 0xC6, 0x09, 0xDE, 0xB0 } },
   /* 177.5   -  466   MHz: CP/T = 1000, P = 01010 */ { 466000, { 0xC6, 0x0A, 0xDE, 0xB0 } },
   /* 474     -  858   MHz: CP/T = 1000, P = 11000 */ { MAXFRQ, { 0xC6, 0x18, 0xDE, 0xB0 } }
};

static TUNERCANNEDControlTable_t TCT_SSEM_DTOS403SH081A[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_67MHZ         |
                                     TUNER_MODE_SUB0,
      /* description              */ "167kHz stepsize, 7MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_166_7_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_8MHZ          |
                                     TUNER_MODE_SUB0,
      /* description              */ "167kHz stepsize, 8Mhz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_166_8_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_67MHZ         |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz stepsize, 7Mhz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_7_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_8MHZ          |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz stepsize, 8Mhz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_8_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_67MHZ         |
                                     TUNER_MODE_SUB2,
      /* description              */ "62.5kHz stepsize, 7Mhz, CP 50uA",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125, /* NOTE: IF center frequency */
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_7_EXT_ATV
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_8MHZ          |
                                     TUNER_MODE_SUB2,
      /* description              */ "62.5kHz stepsize, 8Mhz, CP 50uA",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125, /* NOTE: IF center frequency */
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_8_EXT_ATV
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_67MHZ         |
                                     TUNER_MODE_SUB3,
      /* description              */ "167kHz stepsize, 7MHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_166_7_INT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_8MHZ          |
                                     TUNER_MODE_SUB3,
      /* description              */ "167kHz stepsize, 8Mhz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_166_8_INT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_67MHZ         |
                                     TUNER_MODE_SUB4,
      /* description              */ "62.5kHz stepsize, 7Mhz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_7_INT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_8MHZ          |
                                     TUNER_MODE_SUB4,
      /* description              */ "62.5kHz stepsize, 8Mhz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_8_INT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_67MHZ         |
                                     TUNER_MODE_SUB5,
      /* description              */ "62.5kHz stepsize, 7Mhz, CP 50uA",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_7_INT_ATV
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK    |
                                     TUNER_MODE_8MHZ          |
                                     TUNER_MODE_SUB5,
      /* description              */ "62.5kHz stepsize, 8Mhz, CP 50uA",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS403SH081A_CB_BB_CBT_AB_625_8_INT_ATV
   }

};

static TUNERSubMode_t subModesSSEM_DTOS403SH081A[] =
{
   "Ext. RF-AGC, DVBT",  /* Submode 0 */
   "Ext. RF-AGC, QAM",   /* Submode 1 */
   "Ext. RF-AGC, ATV",   /* Submode 2 */
   "Int. RF-AGC, DVBT",  /* Submode 3 */
   "Int. RF-AGC, QAM",   /* Submode 4 */
   "Int. RF-AGC, ATV"    /* Submode 5 */
};

static TUNERCANNEDData_t dataSSEM_DTOS403SH081A =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_SSEM_DTOS403SH081A),
   /* nrOfControlBytes  */          4,
};

static TUNERCommonAttr_t commonAttrSSEM_DTOS403SH081A =
{
   /* name            */  "Samsung DTOS403SH081A",
   /* minFreqRF [kHz] */      43000,
   /* maxFreqRF [kHz] */     870000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesSSEM_DTOS403SH081A),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerSSEM_DTOS403SH081A =
{
   /* I2C address  */ { 0xC0, TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSSEM_DTOS403SH081A,
   /* myExtAttr    */ &dataSSEM_DTOS403SH081A,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Samsung-DTOS403SH081A-----------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Samsung DTOS403PH17AS tuner configuration                                  */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTOS403PH17AS_K
#define TUNER_SSEM_DTOS403PH17AS_K
#endif
#endif

#ifdef  TUNER_SSEM_DTOS403PH17AS_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSSEM_DTOS403PH17AS_K
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_DIV[] =
{
   /* CB1 = 0xC0; T/A = 1, T = 000 (Xbuf off), R = 000 (62.5kHz) */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0xC0 , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0xC0 , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0xC0 , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0xC0 , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0xC0 , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0xC0 , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0xC0 , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0xC0 , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0xC0 , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0xC0 , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0xC0 , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0xC0 , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0xC0 , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_EXTAGC[] =
{
   /* CB1 = 0x86; T/A = 0, ATC = 0, AL = 110 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x86 , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x86 , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x86 , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x86 , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x86 , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x86 , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x86 , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x86 , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x86 , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x86 , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x86 , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x86 , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x86 , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP109_SWITCH[] =
{
   /* CB1 = 0x8D; T/A = 0, ATC = 1, AL = 101 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x8D , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x8D , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x8D , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x8D , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x8D , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x8D , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x8D , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x8D , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x8D , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x8D , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x8D , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x8D , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x8D , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP109_LOCK[]=
{
   /* CB1 = 0x85; T/A = 0, ATC = 0, AL = 101 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x85 , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x85 , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x85 , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x85 , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x85 , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x85 , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x85 , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x85 , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x85 , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x85 , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x85 , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x85 , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x85 , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP112_SWITCH[] =
{
   /* CB1 = 0x8C; T/A = 0, ATC = 1, AL = 100 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x8C , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x8C , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x8C , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x8C , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x8C , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x8C , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x8C , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x8C , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x8C , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x8C , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x8C , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x8C , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x8C , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP112_LOCK[] =
{
   /* CB1 = 0x84; T/A = 0, ATC = 0, AL = 100 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x84 , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x84 , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x84 , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x84 , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x84 , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x84 , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x84 , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x84 , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x84 , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x84 , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x84 , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x84 , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x84 , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP115_SWITCH[] =
{
   /* CB1 = 0x8B; T/A = 0, ATC = 1, AL = 011 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x8B , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x8B , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x8B , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x8B , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x8B , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x8B , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x8B , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x8B , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x8B , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x8B , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x8B , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x8B , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x8B , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP115_LOCK[] =
{
   /* CB1 = 0x83; T/A = 0, ATC = 0, AL = 011 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x83 , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x83 , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x83 , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x83 , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x83 , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x83 , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x83 , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x83 , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x83 , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x83 , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x83 , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x83 , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x83 , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP118_SWITCH[] =
{
   /* CB1 = 0x8A; T/A = 0, ATC = 1, AL = 010 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x8A , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x8A , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x8A , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x8A , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x8A , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x8A , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x8A , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x8A , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x8A , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x8A , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x8A , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x8A , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x8A , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP118_LOCK[] =
{
   /* CB1 = 0x82; T/A = 0, ATC = 0, AL = 010 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x82 , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x82 , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x82 , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x82 , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x82 , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x82 , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x82 , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x82 , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x82 , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x82 , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x82 , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x82 , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x82 , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP121_SWITCH[] =
{
   /* CB1 = 0x8A; T/A = 0, ATC = 1, AL = 001 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x89 , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x89 , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x89 , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x89 , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x89 , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x89 , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x89 , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x89 , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x89 , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x89 , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x89 , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x89 , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x89 , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP121_LOCK[]=
{
   /* CB1 = 0x82; T/A = 0, ATC = 0, AL = 001 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x81 , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x81 , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x81 , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x81 , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x81 , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x81 , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x81 , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x81 , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x81 , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x81 , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x81 , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x81 , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x81 , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP124_SWITCH[] =
{
   /* CB1 = 0x8A; T/A = 0, ATC = 1, AL = 000 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x88 , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x88 , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x88 , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x88 , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x88 , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x88 , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x88 , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x88 , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x88 , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x88 , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x88 , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x88 , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x88 , 0xA8 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS403PH17AS_K_TOP124_LOCK[] =
{
   /* CB1 = 0x82; T/A = 0, ATC = 0, AL = 000 */
   /*  47 -  54 MHz: CP = 001, BS = 00001 */ {  54000 , { 0x80 , 0x21 } },
   /*  54 - 111 MHz: CP = 010, BS = 00001 */ { 111000 , { 0x80 , 0x41 } },
   /* 111 - 118 MHz: CP = 011, BS = 00001 */ { 118000 , { 0x80 , 0x61 } },
   /* 118 - 139 MHz: CP = 100, BS = 00001 */ { 139000 , { 0x80 , 0x81 } },
   /* 139 - 174 MHz: CP = 101, BS = 00001 */ { 174000 , { 0x80 , 0xA1 } },
   /* 174 - 188 MHz: CP = 001, BS = 00010 */ { 188000 , { 0x80 , 0x22 } },
   /* 188 - 258 MHz: CP = 010, BS = 00010 */ { 258000 , { 0x80 , 0x42 } },
   /* 258 - 342 MHz: CP = 011, BS = 00010 */ { 342000 , { 0x80 , 0x62 } },
   /* 342 - 366 MHz: CP = 100, BS = 00010 */ { 366000 , { 0x80 , 0x82 } },
   /* 366 - 470 MHz: CP = 101, BS = 00010 */ { 470000 , { 0x80 , 0xA2 } },
   /* 470 - 566 MHz: CP = 011, BS = 01000 */ { 566000 , { 0x80 , 0x68 } },
   /* 566 - 638 MHz: CP = 100, BS = 01000 */ { 638000 , { 0x80 , 0x88 } },
   /* 638 - 862 MHz: CP = 101, BS = 01000 */ { MAXFRQ , { 0x80 , 0xA8 } }
};


static TUNERCANNEDControlTable_t TCT_SSEM_DTOS403PH17AS_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUBALL,
      /* description              */ "Divider, 62.5kHZ stepsize",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_DIV
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext.Agc",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_EXTAGC
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "TOP109, switch",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP109_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "TOP109, lock",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP109_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "TOP112, switch",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP112_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "TOP112, lock",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP112_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "TOP115, switch",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP115_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "TOP115, lock",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP115_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "TOP118, switch",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP118_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB4,
      /* description              */ "TOP118, lock",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP118_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB5,
      /* description              */ "TOP121, switch",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP121_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB5,
      /* description              */ "TOP121, lock",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP121_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB6,
      /* description              */ "TOP124, switch",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP124_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB6,
      /* description              */ "TOP124, lock",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SSEM_DTOS403PH17AS_K_TOP124_LOCK
   }
};

static TUNERSubMode_t subModesSSEM_DTOS403PH17AS_K[] =
{
   "External RF-AGC", /* Submode 0 */
   "TOP 109dBuV",     /* Submode 1 */
   "TOP 112dBuV",     /* Submode 2 */
   "TOP 115dBuV",     /* Submode 3 */
   "TOP 118dBuV",     /* Submode 4 */
   "TOP 121dBuV",     /* Submode 5 */
   "TOP 124dBuV"      /* Submode 6 */
};

static TUNERCANNEDData_t dataSSEM_DTOS403PH17AS_K =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_SSEM_DTOS403PH17AS_K),
   /* nrOfControlBytes  */          2,
};

static TUNERCommonAttr_t commonAttrSSEM_DTOS403PH17AS_K =
{
   /* name            */  "Samsung DTOS403PH17AS",
   /* minFreqRF [kHz] */      47000,
   /* maxFreqRF [kHz] */     862000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesSSEM_DTOS403PH17AS_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerSSEM_DTOS403PH17AS_K =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSSEM_DTOS403PH17AS_K,
   /* myExtAttr    */ &dataSSEM_DTOS403PH17AS_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Samsung-DTOS403PH17AS-----------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Samsung DTOS40FPL111A tuner configuration                                  */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTOS40FPL111A_K
#define TUNER_SSEM_DTOS40FPL111A_K
#endif
#endif

#ifdef  TUNER_SSEM_DTOS40FPL111A_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSSEM_DTOS40FPL111A_K
#endif

#include "tunersamsungdtosfpl111a.h"

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS40FPL111A_K_DIV_166KHZ_115DBUV_SWITCH[] =
{
   /* CB1 = 0xCA; T/A = 1, T = 001, R = 010 */
   /*  47 - 145 MHz: CP = 001, BS = 00001 */ { 145000 , { 0xCA , 0x21 , 0x8B } },
   /* 145 - 250 MHz: CP = 010, BS = 00010 */ { 250000 , { 0xCA , 0x42 , 0x8B } },
   /* 250 - 400 MHz: CP = 011, BS = 00010 */ { 400000 , { 0xCA , 0x62 , 0x8B } },
   /* 400 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCA , 0x82 , 0x8B } },
   /* 430 - 470 MHz: CP = 010, BS = 00100 */ { 470000 , { 0xCA , 0x44 , 0x8B } },
   /* 470 - 600 MHz: CP = 011, BS = 00100 */ { 600000 , { 0xCA , 0x64 , 0x8B } },
   /* 600 - 730 MHz: CP = 100, BS = 00100 */ { 730000 , { 0xCA , 0x84 , 0x8B } },
   /* 730 - 870 MHz: CP = 101, BS = 00100 */ { MAXFRQ , { 0xCA , 0xA4 , 0x8B } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS40FPL111A_K_DIV_166KHZ_115DBUV_LOCK[] =
{
   /* CB1 = 0xCA; T/A = 1, T = 001, R = 010 */
   /*  47 - 145 MHz: CP = 001, BS = 00001 */ { 145000 , { 0xCA , 0x21 , 0x83 } },
   /* 145 - 250 MHz: CP = 010, BS = 00010 */ { 250000 , { 0xCA , 0x42 , 0x83 } },
   /* 250 - 400 MHz: CP = 011, BS = 00010 */ { 400000 , { 0xCA , 0x62 , 0x83 } },
   /* 400 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCA , 0x82 , 0x83 } },
   /* 430 - 470 MHz: CP = 010, BS = 00100 */ { 470000 , { 0xCA , 0x44 , 0x83 } },
   /* 470 - 600 MHz: CP = 011, BS = 00100 */ { 600000 , { 0xCA , 0x64 , 0x83 } },
   /* 600 - 730 MHz: CP = 100, BS = 00100 */ { 730000 , { 0xCA , 0x84 , 0x83 } },
   /* 730 - 870 MHz: CP = 101, BS = 00100 */ { MAXFRQ , { 0xCA , 0xA4 , 0x83 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS40FPL111A_K_DIV_62KHZ_115DBUV_SWITCH[] =
{
   /* CB1 = 0xCB; T/A = 1, T = 001, R = 011 */
   /*  47 - 145 MHz: CP = 001, BS = 00001 */ { 145000 , { 0xCB , 0x21 , 0x8B } },
   /* 145 - 250 MHz: CP = 010, BS = 00010 */ { 250000 , { 0xCB , 0x42 , 0x8B } },
   /* 250 - 400 MHz: CP = 011, BS = 00010 */ { 400000 , { 0xCB , 0x62 , 0x8B } },
   /* 400 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCB , 0x82 , 0x8B } },
   /* 430 - 470 MHz: CP = 010, BS = 00100 */ { 470000 , { 0xCB , 0x44 , 0x8B } },
   /* 470 - 600 MHz: CP = 011, BS = 00100 */ { 600000 , { 0xCB , 0x64 , 0x8B } },
   /* 600 - 730 MHz: CP = 100, BS = 00100 */ { 730000 , { 0xCB , 0x84 , 0x8B } },
   /* 730 - 870 MHz: CP = 101, BS = 00100 */ { MAXFRQ , { 0xCB , 0xA4 , 0x8B } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS40FPL111A_K_DIV_62KHZ_115DBUV_LOCK[] =
{
   /* CB1 = 0xCB; T/A = 1, T = 001, R = 011 */
   /*  47 - 145 MHz: CP = 001, BS = 00001 */ { 145000 , { 0xCB , 0x21 , 0x83 } },
   /* 145 - 250 MHz: CP = 010, BS = 00010 */ { 250000 , { 0xCB , 0x42 , 0x83 } },
   /* 250 - 400 MHz: CP = 011, BS = 00010 */ { 400000 , { 0xCB , 0x62 , 0x83 } },
   /* 400 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCB , 0x82 , 0x83 } },
   /* 430 - 470 MHz: CP = 010, BS = 00100 */ { 470000 , { 0xCB , 0x44 , 0x83 } },
   /* 470 - 600 MHz: CP = 011, BS = 00100 */ { 600000 , { 0xCB , 0x64 , 0x83 } },
   /* 600 - 730 MHz: CP = 100, BS = 00100 */ { 730000 , { 0xCB , 0x84 , 0x83 } },
   /* 730 - 870 MHz: CP = 101, BS = 00100 */ { MAXFRQ , { 0xCB , 0xA4 , 0x83 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS40FPL111A_K_DIV_166KHZ_124DBUV_SWITCH[] =
{
   /* CB1 = 0xCA; T/A = 1, T = 001, R = 010 */
   /*  47 - 145 MHz: CP = 001, BS = 00001 */ { 145000 , { 0xCA , 0x21 , 0x88 } },
   /* 145 - 250 MHz: CP = 010, BS = 00010 */ { 250000 , { 0xCA , 0x42 , 0x88 } },
   /* 250 - 400 MHz: CP = 011, BS = 00010 */ { 400000 , { 0xCA , 0x62 , 0x88 } },
   /* 400 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCA , 0x82 , 0x88 } },
   /* 430 - 470 MHz: CP = 010, BS = 00100 */ { 470000 , { 0xCA , 0x44 , 0x88 } },
   /* 470 - 600 MHz: CP = 011, BS = 00100 */ { 600000 , { 0xCA , 0x64 , 0x88 } },
   /* 600 - 730 MHz: CP = 100, BS = 00100 */ { 730000 , { 0xCA , 0x84 , 0x88 } },
   /* 730 - 870 MHz: CP = 101, BS = 00100 */ { MAXFRQ , { 0xCA , 0xA4 , 0x88 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS40FPL111A_K_DIV_166KHZ_124DBUV_LOCK[] =
{
   /* CB1 = 0xCA; T/A = 1, T = 001, R = 010 */
   /*  47 - 145 MHz: CP = 001, BS = 00001 */ { 145000 , { 0xCA , 0x21 , 0x80 } },
   /* 145 - 250 MHz: CP = 010, BS = 00010 */ { 250000 , { 0xCA , 0x42 , 0x80 } },
   /* 250 - 400 MHz: CP = 011, BS = 00010 */ { 400000 , { 0xCA , 0x62 , 0x80 } },
   /* 400 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCA , 0x82 , 0x80 } },
   /* 430 - 470 MHz: CP = 010, BS = 00100 */ { 470000 , { 0xCA , 0x44 , 0x80 } },
   /* 470 - 600 MHz: CP = 011, BS = 00100 */ { 600000 , { 0xCA , 0x64 , 0x80 } },
   /* 600 - 730 MHz: CP = 100, BS = 00100 */ { 730000 , { 0xCA , 0x84 , 0x80 } },
   /* 730 - 870 MHz: CP = 101, BS = 00100 */ { MAXFRQ , { 0xCA , 0xA4 , 0x80 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS40FPL111A_K_DIV_62KHZ_124DBUV_SWITCH[] =
{
   /* CB1 = 0xCB; T/A = 1, T = 001, R = 011 */
   /*  47 - 145 MHz: CP = 001, BS = 00001 */ { 145000 , { 0xCB , 0x21 , 0x88 } },
   /* 145 - 250 MHz: CP = 010, BS = 00010 */ { 250000 , { 0xCB , 0x42 , 0x88 } },
   /* 250 - 400 MHz: CP = 011, BS = 00010 */ { 400000 , { 0xCB , 0x62 , 0x88 } },
   /* 400 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCB , 0x82 , 0x88 } },
   /* 430 - 470 MHz: CP = 010, BS = 00100 */ { 470000 , { 0xCB , 0x44 , 0x88 } },
   /* 470 - 600 MHz: CP = 011, BS = 00100 */ { 600000 , { 0xCB , 0x64 , 0x88 } },
   /* 600 - 730 MHz: CP = 100, BS = 00100 */ { 730000 , { 0xCB , 0x84 , 0x88 } },
   /* 730 - 870 MHz: CP = 101, BS = 00100 */ { MAXFRQ , { 0xCB , 0xA4 , 0x88 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTOS40FPL111A_K_DIV_62KHZ_124DBUV_LOCK[] =
{
   /* CB1 = 0xCB; T/A = 1, T = 001, R = 011 */
   /*  47 - 145 MHz: CP = 001, BS = 00001 */ { 145000 , { 0xCB , 0x21 , 0x80 } },
   /* 145 - 250 MHz: CP = 010, BS = 00010 */ { 250000 , { 0xCB , 0x42 , 0x80 } },
   /* 250 - 400 MHz: CP = 011, BS = 00010 */ { 400000 , { 0xCB , 0x62 , 0x80 } },
   /* 400 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCB , 0x82 , 0x80 } },
   /* 430 - 470 MHz: CP = 010, BS = 00100 */ { 470000 , { 0xCB , 0x44 , 0x80 } },
   /* 470 - 600 MHz: CP = 011, BS = 00100 */ { 600000 , { 0xCB , 0x64 , 0x80 } },
   /* 600 - 730 MHz: CP = 100, BS = 00100 */ { 730000 , { 0xCB , 0x84 , 0x80 } },
   /* 730 - 870 MHz: CP = 101, BS = 00100 */ { MAXFRQ , { 0xCB , 0xA4 , 0x80 } }
};

static TUNERCANNEDControlTable_t TCT_SSEM_DTOS40FPL111A_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Divider, 166kHz stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SSEM_DTOS40FPL111A_K_DIV_166KHZ_115DBUV_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Divider, 166kHz stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SSEM_DTOS40FPL111A_K_DIV_166KHZ_115DBUV_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Divider, 62.5kHz stepsize",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS40FPL111A_K_DIV_62KHZ_115DBUV_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Divider, 62.5kHz stepsize",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS40FPL111A_K_DIV_62KHZ_115DBUV_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Divider, 166kHz stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SSEM_DTOS40FPL111A_K_DIV_166KHZ_124DBUV_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Divider, 166kHz stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SSEM_DTOS40FPL111A_K_DIV_166KHZ_124DBUV_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "Divider, 62.5kHz stepsize",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS40FPL111A_K_DIV_62KHZ_124DBUV_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "Divider, 62.5kHz stepsize",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_SSEM_DTOS40FPL111A_K_DIV_62KHZ_124DBUV_LOCK
   }
};

static TUNERSubMode_t subModesSSEM_DTOS40FPL111A_K[] =
{
   "TOP 115dBuV, 166kHz",  /* Submode 0 */
   "TOP 115dBuV, 62.5kHz", /* Submode 1 */
   "TOP 124dBuV, 166kHz",  /* Submode 2 */
   "TOP 124dBuV, 62.5kHz"  /* Submode 3 */
};

static TUNERCANNEDData_t dataSSEM_DTOS40FPL111A_K =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_SSEM_DTOS40FPL111A_K),
   /* nrOfControlBytes  */          3,
};

static TUNERCommonAttr_t commonAttrSSEM_DTOS40FPL111A_K =
{
   /* name            */  "Samsung DTOS40FPL111A",
   /* minFreqRF [kHz] */      43000,
   /* maxFreqRF [kHz] */     870000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesSSEM_DTOS40FPL111A_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerSSEM_DTOS40FPL111A_K =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSSEM_DTOS40FPL111A_K,
   /* myExtAttr    */ &dataSSEM_DTOS40FPL111A_K,
   /* myFunct      */ &TUNERSSEMDTOSFPL111AFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Samsung-DTOS40FPL111A-----------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Samsung DTVS22CVL161A configuration                                        */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTVS22CVL161A_INT
#define TUNER_SSEM_DTVS22CVL161A_INT
#endif
#endif

#ifdef  TUNER_ALL
#ifndef TUNER_SSEM_DTVS22CVL161A_EXT
#define TUNER_SSEM_DTVS22CVL161A_EXT
#endif
#endif


#ifdef  TUNER_SSEM_DTVS22CVL161A_INT
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSSEM_DTVS22CVL161A_INT
#endif
#endif

#ifdef  TUNER_SSEM_DTVS22CVL161A_EXT
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSSEM_DTVS22CVL161A_EXT
#endif
#endif


#if   defined  TUNER_SSEM_DTVS22CVL161A_INT || \
      defined  TUNER_SSEM_DTVS22CVL161A_EXT

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_EXT_DTV_LNA_OFF[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 106 MHz: CP/T = 1111, P = 00001 */ { 106000, { 0xFC, 0x01, 0xDC, 0xE0 } },
   /* 106 - 166 MHz: CP/T = 0110, P = 00001 */ { 166000, { 0xB4, 0x01, 0x9C, 0xE0 } },
   /* 166 - 310 MHz: CP/T = 1111, P = 00010 */ { 310000, { 0xFC, 0x02, 0xDC, 0xE0 } },
   /* 310 - 454 MHz: CP/T = 0110, P = 00010 */ { 454000, { 0xB4, 0x02, 0x9C, 0xE0 } },
   /* 454 - 600 MHz: CP/T = 0110, P = 10000 */ { 600000, { 0xB4, 0x10, 0x9C, 0xE0 } },
   /* 600 -     MHz: CP/T = 0111, P = 10000 */ { MAXFRQ, { 0xBC, 0x10, 0x9C, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_EXT_DTV_LNA_ON[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 106 MHz: CP/T = 1111, P = 01001 */ { 106000, { 0xFC, 0x09, 0xDC, 0xE0 } },
   /* 106 - 166 MHz: CP/T = 0110, P = 01001 */ { 166000, { 0xB4, 0x09, 0x9C, 0xE0 } },
   /* 166 - 310 MHz: CP/T = 1111, P = 01010 */ { 310000, { 0xFC, 0x0A, 0xDC, 0xE0 } },
   /* 310 - 454 MHz: CP/T = 0110, P = 01010 */ { 454000, { 0xB4, 0x0A, 0x9C, 0xE0 } },
   /* 454 - 600 MHz: CP/T = 0110, P = 11000 */ { 600000, { 0xB4, 0x18, 0x9C, 0xE0 } },
   /* 600 -     MHz: CP/T = 0111, P = 11000 */ { MAXFRQ, { 0xBC, 0x18, 0x9C, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_EXT_ATV_LNA_OFF[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 166 MHz: CP/T = 1000, P = 00001 */ { 166500, { 0xC6, 0x01, 0xDE, 0xE0 } },
   /* 166 - 454 MHz: CP/T = 1000, P = 00010 */ { 454000, { 0xC6, 0x02, 0xDE, 0xE0 } },
   /* 454 -     MHz: CP/T = 1000, P = 10000 */ { MAXFRQ, { 0xC6, 0x10, 0xDE, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_EXT_ATV_LNA_ON[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 166 MHz: CP/T = 1000, P = 01001 */ { 166500, { 0xC6, 0x09, 0xDE, 0xE0 } },
   /* 166 - 454 MHz: CP/T = 1000, P = 01010 */ { 454000, { 0xC6, 0x0A, 0xDE, 0xE0 } },
   /* 454 -     MHz: CP/T = 1000, P = 11000 */ { MAXFRQ, { 0xC6, 0x18, 0xDE, 0xE0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_INT_DTV_SWITCH_LNA_OFF[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 106 MHz: CP/T = 1111, P = 00001 */ { 106000, { 0xFC, 0x01, 0xDC, 0xB0 } },
   /* 106 - 166 MHz: CP/T = 0110, P = 00001 */ { 166000, { 0xB4, 0x01, 0x9C, 0xB0 } },
   /* 166 - 310 MHz: CP/T = 1111, P = 00010 */ { 310000, { 0xFC, 0x02, 0xDC, 0xB0 } },
   /* 310 - 454 MHz: CP/T = 0110, P = 00010 */ { 454000, { 0xB4, 0x02, 0x9C, 0xB0 } },
   /* 454 - 600 MHz: CP/T = 0110, P = 10000 */ { 600000, { 0xB4, 0x10, 0x9C, 0xB0 } },
   /* 600 -     MHz: CP/T = 0111, P = 10000 */ { MAXFRQ, { 0xBC, 0x10, 0x9C, 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_INT_DTV_SWITCH_LNA_ON[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 106 MHz: CP/T = 1111, P = 01001 */ { 106000, { 0xFC, 0x09, 0xDC, 0xB0 } },
   /* 106 - 166 MHz: CP/T = 0110, P = 01001 */ { 166000, { 0xB4, 0x09, 0x9C, 0xB0 } },
   /* 166 - 310 MHz: CP/T = 1111, P = 01010 */ { 310000, { 0xFC, 0x0A, 0xDC, 0xB0 } },
   /* 310 - 454 MHz: CP/T = 0110, P = 01010 */ { 454000, { 0xB4, 0x0A, 0x9C, 0xB0 } },
   /* 454 - 600 MHz: CP/T = 0110, P = 11000 */ { 600000, { 0xB4, 0x18, 0x9C, 0xB0 } },
   /* 600 -     MHz: CP/T = 0111, P = 11000 */ { MAXFRQ, { 0xBC, 0x18, 0x9C, 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_INT_DTV_LOCK_LNA_OFF[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 106 MHz: CP/T = 1111, P = 00001 */ { 106000, { 0xFC, 0x01, 0xDC, 0x30 } },
   /* 106 - 166 MHz: CP/T = 0110, P = 00001 */ { 166000, { 0xB4, 0x01, 0x9C, 0x30 } },
   /* 166 - 310 MHz: CP/T = 1111, P = 00010 */ { 310000, { 0xFC, 0x02, 0xDC, 0x30 } },
   /* 310 - 454 MHz: CP/T = 0110, P = 00010 */ { 454000, { 0xB4, 0x02, 0x9C, 0x30 } },
   /* 454 - 600 MHz: CP/T = 0110, P = 10000 */ { 600000, { 0xB4, 0x10, 0x9C, 0x30 } },
   /* 600 -     MHz: CP/T = 0111, P = 10000 */ { MAXFRQ, { 0xBC, 0x10, 0x9C, 0x30 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_INT_DTV_LOCK_LNA_ON[] =
{
   /* CB:  RS = 10, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 106 MHz: CP/T = 1111, P = 00001 */ { 106000, { 0xFC, 0x09, 0xDC, 0x30 } },
   /* 106 - 166 MHz: CP/T = 0110, P = 00001 */ { 166000, { 0xB4, 0x09, 0x9C, 0x30 } },
   /* 166 - 310 MHz: CP/T = 1111, P = 00010 */ { 310000, { 0xFC, 0x0A, 0xDC, 0x30 } },
   /* 310 - 454 MHz: CP/T = 0110, P = 00010 */ { 454000, { 0xB4, 0x0A, 0x9C, 0x30 } },
   /* 454 - 600 MHz: CP/T = 0110, P = 10000 */ { 600000, { 0xB4, 0x18, 0x9C, 0x30 } },
   /* 600 -     MHz: CP/T = 0111, P = 10000 */ { MAXFRQ, { 0xBC, 0x18, 0x9C, 0x30 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_INT_ATV_SWITCH_LNA_OFF[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 166 MHz: CP/T = 1000, P = 00001 */ { 166000, { 0xC6, 0x01, 0xDE, 0xB0 } },
   /* 166 - 454 MHz: CP/T = 1000, P = 00010 */ { 454000, { 0xC6, 0x02, 0xDE, 0xB0 } },
   /* 454 -     MHz: CP/T = 1000, P = 10000 */ { MAXFRQ, { 0xC6, 0x10, 0xDE, 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_INT_ATV_SWITCH_LNA_ON[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 166 MHz: CP/T = 1000, P = 01001 */ { 166000, { 0xC6, 0x09, 0xDE, 0xB0 } },
   /* 166 - 454 MHz: CP/T = 1000, P = 01010 */ { 454000, { 0xC6, 0x0A, 0xDE, 0xB0 } },
   /* 454 -     MHz: CP/T = 1000, P = 11000 */ { MAXFRQ, { 0xC6, 0x18, 0xDE, 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_INT_ATV_LOCK_LNA_OFF[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 166 MHz: CP/T = 1000, P = 00001 */ { 166000, { 0xC6, 0x01, 0xDE, 0x30 } },
   /* 166 - 454 MHz: CP/T = 1000, P = 00010 */ { 454000, { 0xC6, 0x02, 0xDE, 0x30 } },
   /* 454 -     MHz: CP/T = 1000, P = 10000 */ { MAXFRQ, { 0xC6, 0x10, 0xDE, 0x30 } }
};

static TUNERCANNEDControlUnit_t TCU_SSEM_DTVS22CVL161A_INT_ATV_LOCK_LNA_ON[] =
{
   /* CB:  RS = 11, OS = 0 */
   /* BB:  IFA = 0 */
   /* CBT: T = 011 -> AB follows */
   /* AB:  ATC = 0, AL = 110, LP = 0 */
   /*     - 166 MHz: CP/T = 1000, P = 00001 */ { 166000, { 0xC6, 0x09, 0xDE, 0x30 } },
   /* 166 - 454 MHz: CP/T = 1000, P = 00010 */ { 454000, { 0xC6, 0x0A, 0xDE, 0x30 } },
   /* 454 -     MHz: CP/T = 1000, P = 10000 */ { MAXFRQ, { 0xC6, 0x18, 0xDE, 0x30 } }
};


static TUNERCANNEDControlTable_t TCT_SSEMDTVS22CVL161A[] =
{
   {
      /* modes                    */   TUNER_MODE_ANALOG     |
                                       TUNER_MODE_SWITCHLOCK |
                                       TUNER_MODE_678MHZ     |
                                       TUNER_MODE_SUB0,
      /* description              */   "ATV, ext. RF-AGC",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */   2,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_EXT_ATV_LNA_OFF
   },
   {
      /* modes                    */   TUNER_MODE_DIGITAL    |
                                       TUNER_MODE_SWITCHLOCK |
                                       TUNER_MODE_678MHZ     |
                                       TUNER_MODE_SUB0,
      /* description              */   "DTV, ext. RF-AGC",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */   3,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_EXT_DTV_LNA_OFF
   },
   {
      /* modes                    */   TUNER_MODE_ANALOG     |
                                       TUNER_MODE_SWITCHLOCK |
                                       TUNER_MODE_678MHZ     |
                                       TUNER_MODE_SUB1,
      /* description              */   "ATV, ext. RF-AGC",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */   2,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_EXT_ATV_LNA_ON
   },
   {
      /* modes                    */   TUNER_MODE_DIGITAL    |
                                       TUNER_MODE_SWITCHLOCK |
                                       TUNER_MODE_678MHZ     |
                                       TUNER_MODE_SUB1,
      /* description              */   "DTV, ext. RF-AGC",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */   3,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_EXT_DTV_LNA_ON
   },
   {
      /* modes                    */   TUNER_MODE_ANALOG |
                                       TUNER_MODE_SWITCH |
                                       TUNER_MODE_678MHZ |
                                       TUNER_MODE_SUB2,
      /* description              */   "ATV, int. RF-AGC switch",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */   2,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_INT_ATV_SWITCH_LNA_OFF
   },
   {
      /* modes                    */   TUNER_MODE_DIGITAL |
                                       TUNER_MODE_SWITCH  |
                                       TUNER_MODE_678MHZ  |
                                       TUNER_MODE_SUB2,
      /* description              */   "DTV, int. RF-AGC switch",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */   3,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_INT_DTV_SWITCH_LNA_OFF
   },
   {
      /* modes                    */   TUNER_MODE_ANALOG |
                                       TUNER_MODE_LOCK   |
                                       TUNER_MODE_678MHZ |
                                       TUNER_MODE_SUB2,
      /* description              */   "ATV, int. RF-AGC lock",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */   2,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_INT_ATV_LOCK_LNA_OFF
   },
   {
      /* modes                    */   TUNER_MODE_DIGITAL |
                                       TUNER_MODE_LOCK    |
                                       TUNER_MODE_678MHZ  |
                                       TUNER_MODE_SUB2,
      /* description              */   "DTV, int. RF-AGC lock",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */   3,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_INT_DTV_LOCK_LNA_OFF
   },
   {
      /* modes                    */   TUNER_MODE_ANALOG |
                                       TUNER_MODE_SWITCH |
                                       TUNER_MODE_678MHZ |
                                       TUNER_MODE_SUB3,
      /* description              */   "ATV, int. RF-AGC switch",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */   2,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_INT_ATV_SWITCH_LNA_ON
   },
   {
      /* modes                    */   TUNER_MODE_DIGITAL |
                                       TUNER_MODE_SWITCH  |
                                       TUNER_MODE_678MHZ  |
                                       TUNER_MODE_SUB3,
      /* description              */   "DTV, int. RF-AGC switch",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */   3,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_INT_DTV_SWITCH_LNA_ON
   },
   {
      /* modes                    */   TUNER_MODE_ANALOG |
                                       TUNER_MODE_LOCK   |
                                       TUNER_MODE_678MHZ |
                                       TUNER_MODE_SUB3,
      /* description              */   "ATV, int. RF-AGC lock",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */   2,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_INT_ATV_LOCK_LNA_ON
   },
   {
      /* modes                    */   TUNER_MODE_DIGITAL |
                                       TUNER_MODE_LOCK    |
                                       TUNER_MODE_678MHZ  |
                                       TUNER_MODE_SUB3,
      /* description              */   "DTV, int. RF-AGC lock",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */   3,
      /* outputFrequency    [kHz] */   44000,
      /* controlUnits             */   TCU_SSEM_DTVS22CVL161A_INT_DTV_LOCK_LNA_ON
   }
};

static TUNERSubMode_t subModesSSEMDTVS22CVL161A[] =
{
   "Ext. RF-AGC, LNA OFF",       /* TUNER_MODE_SUB0 */
   "Ext. RF-AGC, LNA ON",        /* TUNER_MODE_SUB1 */
   "Int. RF-AGC, LNA OFF",       /* TUNER_MODE_SUB2 */
   "Int. RF-AGC, LNA ON"         /* TUNER_MODE_SUB3 */
};


static TUNERCANNEDData_t dataSSEMDTVS22CVL161A =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_SSEMDTVS22CVL161A),
   /* nrOfControlBytes  */          4
};
#endif

#ifdef  TUNER_SSEM_DTVS22CVL161A_INT
static TUNERCommonAttr_t commonAttrSSEMDTVS22CVL161A_INT=
{
   /* name            */  "Samsung DTVS22CVL161A Int. RF-AGC",
   /* minFreqRF [kHz] */      57000,
   /* maxFreqRF [kHz] */     863000,
   /* sub-mode        */          1,
   /* sub-modes       */  ARRAYREF(subModesSSEMDTVS22CVL161A),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      44000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerSSEM_DTVS22CVL161A_INT =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSSEMDTVS22CVL161A_INT,
   /* myExtAttr    */ &dataSSEMDTVS22CVL161A,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif

#ifdef  TUNER_SSEM_DTVS22CVL161A_EXT
static TUNERCommonAttr_t commonAttrSSEMDTVS22CVL161A_EXT =
{
   /* name            */  "Samsung DTVS22CVL161A Ext. RF-AGC",
   /* minFreqRF [kHz] */      57000,
   /* maxFreqRF [kHz] */     863000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesSSEMDTVS22CVL161A),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      44000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerSSEM_DTVS22CVL161A_EXT =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSSEMDTVS22CVL161A_EXT,
   /* myExtAttr    */ &dataSSEMDTVS22CVL161A,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-SSEM_DTVS22CVL161A--------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Sanyo 115U7A20C tuner configuration                                        */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SANYO_115U7A20C
#define TUNER_SANYO_115U7A20C
#endif
#endif

#ifdef  TUNER_SANYO_115U7A20C
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSANYO_115U7A20C
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_SANYO_115U7A20C_62_5_SWITCH[] =
{
   /* RS = 011 , ATP = 111 , ATC = 1 , AISL = 0 , ALS = 0 */
   /*  47 - 160 MHz: CP = 00, BS = 0?01 */ { 160000 , { 0xBB , 0x01 , 0xE1 } },
   /* 160 - 430 MHz: CP = 00, BS = 0?10 */ { 430000 , { 0xBB , 0x02 , 0xE1 } },
   /* 430 - 870 MHz: CP = 00, BS = 1?00 */ { MAXFRQ , { 0xBB , 0x08 , 0xE1 } }
};

static TUNERCANNEDControlUnit_t TCU_SANYO_115U7A20C_62_5_LOCK[] =
{
   /* RS = 011 , ATP = 111 , ATC = 0 , AISL = 0 , ALS = 0 */
   /*  47 - 160 MHz: CP = 00, BS = 0?01 */ { 160000 , { 0xBB , 0x01 , 0xC1 } },
   /* 160 - 430 MHz: CP = 00, BS = 0?10 */ { 430000 , { 0xBB , 0x02 , 0xC1 } },
   /* 430 - 870 MHz: CP = 00, BS = 1?00 */ { MAXFRQ , { 0xBB , 0x08 , 0xC1 } }
};

static TUNERCANNEDControlUnit_t TCU_SANYO_115U7A20C_167_SWITCH[] =
{
   /* RS = 000 , ATP = 111 , ATC = 1 , AISL = 0 , ALS = 0 */
   /*  47 - 160 MHz: CP = 00, BS = 0?01 */ { 160000 , { 0xB8 , 0x01 , 0xE1 } },
   /* 160 - 430 MHz: CP = 00, BS = 0?10 */ { 430000 , { 0xB8 , 0x02 , 0xE1 } },
   /* 430 - 870 MHz: CP = 00, BS = 1?00 */ { MAXFRQ , { 0xB8 , 0x08 , 0xE1 } }
};

static TUNERCANNEDControlUnit_t TCU_SANYO_115U7A20C_167_LOCK[] =
{
   /* RS = 000 , ATP = 111 , ATC = 0 , AISL = 0 , ALS = 0 */
   /*  47 - 160 MHz: CP = 00, BS = 0?01 */ { 160000 , { 0xB8 , 0x01 , 0xC1 } },
   /* 160 - 430 MHz: CP = 00, BS = 0?10 */ { 430000 , { 0xB8 , 0x02 , 0xC1 } },
   /* 430 - 870 MHz: CP = 00, BS = 1?00 */ { MAXFRQ , { 0xB8 , 0x08 , 0xC1 } }
};

static TUNERCANNEDControlTable_t TCT_SANYO_115U7A20C[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Divider, 62.5kHZ stepsize",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SANYO_115U7A20C_62_5_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Divider, 62.5kHZ stepsize",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SANYO_115U7A20C_62_5_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Divider, 167kHZ stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SANYO_115U7A20C_167_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Divider, 167kHZ stepsize",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_SANYO_115U7A20C_167_LOCK
   }
};

static TUNERSubMode_t subModesSANYO_115U7A20C[] =
{
   "EXT. RF-AGC, 62.5 kHz",  /* Submode 0 */
   "EXT. RF-AGC, 166.7 kHz"  /* Submode 1 */
};

static TUNERCANNEDData_t dataSANYO_115U7A20C =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_SANYO_115U7A20C),
   /* nrOfControlBytes  */          3,
};

static TUNERCommonAttr_t commonAttrSANYO_115U7A20C =
{
   /* name            */  "Sanyo 115U7A20C",
   /* minFreqRF [kHz] */      47000,
   /* maxFreqRF [kHz] */     862000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesSANYO_115U7A20C),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerSANYO_115U7A20C =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSANYO_115U7A20C,
   /* myExtAttr    */ &dataSANYO_115U7A20C,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Sanyo-115U7A20C-----------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Sharp VA1E1ED2403 tuner configuration                                      */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SHARP_VA1E1ED2403
#define TUNER_SHARP_VA1E1ED2403
#endif
#endif

#ifdef  TUNER_SHARP_VA1E1ED2403
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSharpVA1E1ED2403
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_DVBT_7MHz_SWITCH[] =
{
   /* ATP = 000, R = 000, CP = 010, P5 = 0, T3 = 1, T2 = 0, ATC = 1 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x80 , 0x81, 0xE1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x80 , 0x82, 0xE1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x80 , 0x84, 0xE1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_DVBT_8MHz_SWITCH[] =
{
   /* ATP = 000,, R = 000, CP = 010, P5 = 1, T3 = 1, T2 = 0, ATC = 1 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x80 , 0x91, 0xE1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x80 , 0x92, 0xE1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x80 , 0x94, 0xE1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_DVBT_7MHz_LOCK[] =
{
   /* ATP = 000, R = 000, CP = 010, P5 = 0, T3 = 1, T2 = 0, ATC = 0 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x80 , 0x81, 0xC1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x80 , 0x82, 0xC1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x80 , 0x84, 0xC1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_DVBT_8MHz_LOCK[] =
{
   /* ATP = 000,, R = 000, CP = 010, P5 = 1, T3 = 1, T2 = 0, ATC = 0 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x80 , 0x91, 0xC1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x80 , 0x92, 0xC1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x80 , 0x94, 0xC1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_DVBC_7MHz_SWITCH[] =
{
   /* ATP = 000,, R = 011, CP = 010, P5 = 0, T3 = 1, T2 = 0, ATC = 1 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x83 , 0x81, 0xE1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x83 , 0x82, 0xE1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x83 , 0x84, 0xE1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_DVBC_8MHz_SWITCH[] =
{
   /* ATP = 000,, R = 011, CP = 010, P5 = 1, T3 = 1, T2 = 0, ATC = 1 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x83 , 0x91, 0xE1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x83 , 0x92, 0xE1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x83 , 0x94, 0xE1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_DVBC_7MHz_LOCK[] =
{
   /* ATP = 000,, R = 011, CP = 010, P5 = 0, T3 = 1, T2 = 0, ATC = 0 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x83 , 0x81, 0xC1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x83 , 0x82, 0xC1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x83 , 0x84, 0xC1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_DVBC_8MHz_LOCK[] =
{
   /* ATP = 000,, R = 011, CP = 010, P5 = 1, T3 = 1, T2 = 0, ATC = 0 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x83 , 0x91, 0xC1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x83 , 0x92, 0xC1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x83 , 0x94, 0xC1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_ATV_7MHz_SWITCH[] =
{
   /* ATP = 000,, R = 101, CP = 010, P5 = 0, T3 = 1, T2 = 0, ATC = 1 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x85 , 0x81, 0xE1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x85 , 0x82, 0xE1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x85 , 0x84, 0xE1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_ATV_8MHz_SWITCH[] =
{
   /* ATP = 000,, R = 101, CP = 010, P5 = 1, T3 = 1, T2 = 0, ATC = 1 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x85 , 0x91, 0xE1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x85 , 0x92, 0xE1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x85 , 0x94, 0xE1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_ATV_7MHz_LOCK[] =
{
   /* ATP = 000,, R = 101, CP = 010, P5 = 0, T3 = 1, T2 = 0, ATC = 0 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x85 , 0x81, 0xC1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x85 , 0x82, 0xC1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x85 , 0x84, 0xC1} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E1ED2403_ATV_8MHz_LOCK[] =
{
   /* ATP = 000,, R = 101, CP = 010, P5 = 1, T3 = 1, T2 = 0, ATC = 0 */
   /*  44 - 146 MHz: BS = 001 */ { 146000 , { 0x85 , 0x91, 0xC1} },
   /* 147 - 430 MHz: BS = 010 */ { 430000 , { 0x85 , 0x92, 0xC1} },
   /* 430 - 858 MHz: BS = 100 */ { MAXFRQ , { 0x85 , 0x94, 0xC1} }
};

static TUNERCANNEDControlTable_t TCT_SharpVA1E1ED2403[] =
{
    {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz, DVB-T",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_DVBT_7MHz_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz, DVB-T",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_DVBT_8MHz_SWITCH
   },
    {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz, DVB-T",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_DVBT_7MHz_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz, DVB-T",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_DVBT_8MHz_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz, DVB-C",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_DVBC_7MHz_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz, DVB-C",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_DVBC_8MHz_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz, DVB-C",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_DVBC_7MHz_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz, DVB-C",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_DVBC_8MHz_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "50kHz, ATV",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_ATV_7MHz_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "50kHz, ATV",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_ATV_8MHz_SWITCH
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "50kHz, ATV",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_ATV_7MHz_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "50kHz, ATV",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1E1ED2403_ATV_8MHz_LOCK
   }

};

static TUNERSubMode_t subModesSharpVA1E1ED2403[] =
{
   "Ext. RF-AGC, DVB-T",  /* Submode 0 */
   "Ext. RF-AGC, DVB-C",  /* Submode 1 */
   "Ext. RF-AGC, ATV"     /* Submode 2 */
};

static TUNERCANNEDData_t dataSharpVA1E1ED2403 =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_SharpVA1E1ED2403),
   /* nrOfControlBytes  */          3,
};

static TUNERCommonAttr_t commonAttrSharpVA1E1ED2403 =
{
   /* name            */  "Sharp VA1E1ED2403",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesSharpVA1E1ED2403),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0,
};

static TUNERInstance_t tunerSharpVA1E1ED2403 =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSharpVA1E1ED2403,
   /* myExtAttr    */ &dataSharpVA1E1ED2403,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Sharp-VA1E1ED2403---------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Sharp VA1E9ED2001 tuner configuration                                      */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SHARP_VA1E9ED2001
#define TUNER_SHARP_VA1E9ED2001
#endif
#endif

#ifdef  TUNER_SHARP_VA1E9ED2001
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSharpVA1E9ED2001
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_SharpVA1E9ED2001_166_EXT[] =
{
   /* T = 001, R = 010, ATC = 0, AL = 110 */
   /*  44 - 100 MHz: CP = 010, BS = 00001 */ { 100000,  { 0xCA , 0x41, 0x86} },
   /* 100 - 122 MHz: CP = 011, BS = 00001 */ { 122000 , { 0xCA , 0x61, 0x86} },
   /* 122 - 129 MHz: CP = 100, BS = 00001 */ { 129000 , { 0xCA , 0x81, 0x86} },
   /* 129 - 136 MHz: CP = 101, BS = 00001 */ { 136000 , { 0xCA , 0xA1, 0x86} },
   /* 136 - 143 MHz: CP = 110, BS = 00001 */ { 143000 , { 0xCA , 0xC1, 0x86} },
   /* 147 - 240 MHz: CP = 001, BS = 00010 */ { 240000 , { 0xCA , 0x22, 0x86} },
   /* 240 - 310 MHz: CP = 010, BS = 00010 */ { 310000 , { 0xCA , 0x42, 0x86} },
   /* 310 - 380 MHz: CP = 011, BS = 00010 */ { 380000 , { 0xCA , 0x62, 0x86} },
   /* 380 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCA , 0x82, 0x86} },
   /* 430 - 578 MHz: CP = 100, BS = 00100 */ { 578000 , { 0xCA , 0x84, 0x86} },
   /* 578 - 650 MHz: CP = 101, BS = 00100 */ { 650000 , { 0xCA , 0xA4, 0x86} },
   /* 650 - 858 MHz: CP = 110, BS = 00100 */ { MAXFRQ , { 0xCA , 0xC4, 0x86} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E9ED2001_50_QAM_EXT[] =
{
   /* T = 001, R = 011, ATC = 0, AL = 110 */
   /*  44 - 100 MHz: CP = 010, BS = 00001 */ { 100000,  { 0xCB , 0x41, 0x86} },
   /* 100 - 122 MHz: CP = 011, BS = 00001 */ { 122000 , { 0xCB , 0x61, 0x86} },
   /* 122 - 129 MHz: CP = 100, BS = 00001 */ { 129000 , { 0xCB , 0x81, 0x86} },
   /* 129 - 136 MHz: CP = 101, BS = 00001 */ { 136000 , { 0xCB , 0xA1, 0x86} },
   /* 136 - 147 MHz: CP = 110, BS = 00001 */ { 147000 , { 0xCB , 0xC1, 0x86} },
   /* 147 - 240 MHz: CP = 101, BS = 00010 */ { 240000 , { 0xCB , 0xA2, 0x86} },
   /* 240 - 310 MHz: CP = 010, BS = 00010 */ { 310000 , { 0xCB , 0x42, 0x86} },
   /* 310 - 380 MHz: CP = 011, BS = 00010 */ { 380000 , { 0xCB , 0x62, 0x86} },
   /* 380 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCB , 0x82, 0x86} },
   /* 430 - 578 MHz: CP = 100, BS = 00100 */ { 578000 , { 0xCB , 0x84, 0x86} },
   /* 578 - 650 MHz: CP = 101, BS = 00100 */ { 650000 , { 0xCB , 0xA4, 0x86} },
   /* 650 - 858 MHz: CP = 110, BS = 00100 */ { MAXFRQ , { 0xCB , 0xC4, 0x86} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1E9ED2001_50_ATV_EXT[] =
{
   /* T = 001, R = 011, ATC = 0, AL = 110 */
   /*  44 - 147 MHz: CP = 000, BS = 00001 */ { 147000 , { 0xCB , 0x01, 0x86} },
   /* 147 - 430 MHz: CP = 000, BS = 00010 */ { 430000 , { 0xCB , 0x02, 0x86} },
   /* 430 - 858 MHz: CP = 000, BS = 00100 */ { MAXFRQ , { 0xCB , 0x04, 0x86} }
};

static TUNERCANNEDControlTable_t TCT_SharpVA1E9ED2001[] =
{
    {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SharpVA1E9ED2001_166_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "50kHz QAM",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1E9ED2001_50_QAM_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "50kHz ATV",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1E9ED2001_50_ATV_EXT
   }

};

static TUNERSubMode_t subModesSharpVA1E9ED2001[] =
{
   "Ext. RF-AGC, 166.7kHz",   /* Submode 0 */
   "Ext. RF-AGC, 50kHz QAM",  /* Submode 1 */
   "Ext. RF-AGC, 50kHz ATV"   /* Submode 2 */
};

static TUNERCANNEDData_t dataSharpVA1E9ED2001 =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_SharpVA1E9ED2001),
   /* nrOfControlBytes  */          3,
};

static TUNERCommonAttr_t commonAttrSharpVA1E9ED2001 =
{
   /* name            */  "Sharp VA1E9ED2001",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesSharpVA1E9ED2001),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36150,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0,
};

static TUNERInstance_t tunerSharpVA1E9ED2001 =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSharpVA1E9ED2001,
   /* myExtAttr    */ &dataSharpVA1E9ED2001,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Sharp-VA1E9ED2001---------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Sharp VA1Y9ED200x tuner configuration                                      */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SHARP_VA1Y9ED200X
#define TUNER_SHARP_VA1Y9ED200X
#endif
#endif

#ifdef  TUNER_SHARP_VA1Y9ED200X
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSharpVA1Y9ED200x
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_SharpVA1Y9ED200X_166_EXT[] =
{
   /* T = 001, R = 010, ATC = 0, AL = 110 */
   /*  44 - 100 MHz: CP = 010, BS = 00001 */ { 100000,  { 0xCA , 0x41, 0x86} },
   /* 100 - 122 MHz: CP = 011, BS = 00001 */ { 122000 , { 0xCA , 0x61, 0x86} },
   /* 122 - 129 MHz: CP = 100, BS = 00001 */ { 129000 , { 0xCA , 0x81, 0x86} },
   /* 129 - 136 MHz: CP = 101, BS = 00001 */ { 136000 , { 0xCA , 0xA1, 0x86} },
   /* 136 - 147 MHz: CP = 110, BS = 00001 */ { 147000 , { 0xCA , 0xC1, 0x86} },
   /* 147 - 240 MHz: CP = 101, BS = 00010 */ { 240000 , { 0xCA , 0xA2, 0x86} },
   /* 240 - 310 MHz: CP = 010, BS = 00010 */ { 310000 , { 0xCA , 0x42, 0x86} },
   /* 310 - 380 MHz: CP = 011, BS = 00010 */ { 380000 , { 0xCA , 0x62, 0x86} },
   /* 380 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCA , 0x82, 0x86} },
   /* 430 - 578 MHz: CP = 100, BS = 00100 */ { 578000 , { 0xCA , 0x84, 0x86} },
   /* 578 - 650 MHz: CP = 101, BS = 00100 */ { 650000 , { 0xCA , 0xA4, 0x86} },
   /* 650 - 858 MHz: CP = 110, BS = 00100 */ { MAXFRQ , { 0xCA , 0xC4, 0x86} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1Y9ED200X_50_QAM_EXT[] =
{
   /* T = 001, R = 011, ATC = 0, AL = 110 */
   /*  44 - 100 MHz: CP = 010, BS = 00001 */ { 100000,  { 0xCB , 0x41, 0x86} },
   /* 100 - 122 MHz: CP = 011, BS = 00001 */ { 122000 , { 0xCB , 0x61, 0x86} },
   /* 122 - 129 MHz: CP = 100, BS = 00001 */ { 129000 , { 0xCB , 0x81, 0x86} },
   /* 129 - 136 MHz: CP = 101, BS = 00001 */ { 136000 , { 0xCB , 0xA1, 0x86} },
   /* 136 - 147 MHz: CP = 110, BS = 00001 */ { 147000 , { 0xCB , 0xC1, 0x86} },
   /* 147 - 240 MHz: CP = 101, BS = 00010 */ { 240000 , { 0xCB , 0xA2, 0x86} },
   /* 240 - 310 MHz: CP = 010, BS = 00010 */ { 310000 , { 0xCB , 0x42, 0x86} },
   /* 310 - 380 MHz: CP = 011, BS = 00010 */ { 380000 , { 0xCB , 0x62, 0x86} },
   /* 380 - 430 MHz: CP = 100, BS = 00010 */ { 430000 , { 0xCB , 0x82, 0x86} },
   /* 430 - 578 MHz: CP = 100, BS = 00100 */ { 578000 , { 0xCB , 0x84, 0x86} },
   /* 578 - 650 MHz: CP = 101, BS = 00100 */ { 650000 , { 0xCB , 0xA4, 0x86} },
   /* 650 - 858 MHz: CP = 110, BS = 00100 */ { MAXFRQ , { 0xCB , 0xC4, 0x86} }
};

static TUNERCANNEDControlUnit_t TCU_SharpVA1Y9ED200X_50_ATV_EXT[] =
{
   /* T = 001, R = 011, ATC = 0, AL = 110 */
   /*  44 - 147 MHz: CP = 000, BS = 00001 */ { 147000 , { 0xCB , 0x01, 0x86} },
   /* 147 - 430 MHz: CP = 000, BS = 00010 */ { 430000 , { 0xCB , 0x02, 0x86} },
   /* 430 - 858 MHz: CP = 000, BS = 00100 */ { MAXFRQ , { 0xCB , 0x04, 0x86} }
};

static TUNERCANNEDControlTable_t TCT_SharpVA1Y9ED200X[] =
{
    {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_SharpVA1Y9ED200X_166_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "50kHz QAM",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1Y9ED200X_50_QAM_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "50kHz ATV",
      /* refFreqNumerator   [kHz] */   100,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_SharpVA1Y9ED200X_50_ATV_EXT
   }

};

static TUNERSubMode_t subModesSharpVA1Y9ED200X[] =
{
   "Ext. RF-AGC, 166.7kHz",   /* Submode 0 */
   "Ext. RF-AGC, 50kHz QAM",  /* Submode 1 */
   "Ext. RF-AGC, 50kHz ATV"   /* Submode 2 */
};

static TUNERCANNEDData_t dataSharpVA1Y9ED200X =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_SharpVA1Y9ED200X),
   /* nrOfControlBytes  */          3,
};

static TUNERCommonAttr_t commonAttrSharpVA1Y9ED200X =
{
   /* name            */  "Sharp VA1Y9ED200x",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesSharpVA1Y9ED200X),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36150,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0,
};

static TUNERInstance_t tunerSharpVA1Y9ED200X =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSharpVA1Y9ED200X,
   /* myExtAttr    */ &dataSharpVA1Y9ED200X,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Sharp-VA1Y9ED200x---------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Sony SUT-RA111x tuner configuration                                        */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_SONY_SUTRA111
#define TUNER_SONY_SUTRA111
#endif
#endif

#ifdef  TUNER_SONY_SUTRA111
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerSonySutRa111
#endif

#include "tunersonysutra111.h"


static TUNERSubMode_t subModesSonySutRa111[] =
{
   "NO SUBMODES"  /* Submode 0 */
};

static TUNERCommonAttr_t commonAttrSonySutRa111 =
{
   /* name            */  "Sony SUT-RA111x",
   /* minFreqRF [kHz] */      48000,
   /* maxFreqRF [kHz] */     863000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF( subModesSonySutRa111 ),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */       5000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerSonySutRa111 =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrSonySutRa111,
   /* myExtAttr    */ NULL,
   /* myFunct      */ &TUNERSONYSUTRA111Functions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Sony-SUT-RA111x ----------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* TLC DT67WI-R3-E tuner configuration                                        */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_TCL_DT67WI3RE
#define TUNER_TCL_DT67WI3RE
#endif
#endif

#ifdef  TUNER_TCL_DT67WI3RE
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerTCLDT67WI3RE
#endif

#include "tunercanned.h"


static TUNERCANNEDControlUnit_t TCU_TCLDT67WI3RE_166_BB[] =
{
   /* RS = 10 */
   /*  44 - 144 MHz: CP/T = 0111, BS = 00001 */ { 144000 , { 0xBC , 0x01 } },
   /* 144 - 162 MHz: CP/T = 1110, BS = 00001 */ { 162000 , { 0xF4 , 0x01 } },
   /* 162 - 330 MHz: CP/T = 0111, BS = 00010 */ { 330000 , { 0xBC , 0x02 } },
   /* 330 - 444 MHz: CP/T = 1110, BS = 00010 */ { 444000 , { 0xF4 , 0x02 } },
   /* 444 - 610 MHz: CP/T = 0111, BS = 00100 */ { 610000 , { 0xBC , 0x04 } },
   /* 610 - 754 MHz: CP/T = 1110, BS = 00100 */ { 754000 , { 0xF4 , 0x04 } },
   /* 754 - 858 MHz: CP/T = 1111, BS = 00100 */ { MAXFRQ , { 0xFC , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_TCLDT67WI3RE_166_AUX[] =
{
   /* R = 10, ATC = 0, AL = 110 */
   /*  44 - 144 MHz: CP/T = 0011, */ { 144000 , { 0x9C , 0x60 } },
   /* 144 - 162 MHz: CP/T = 1011, */ { 162000 , { 0xDC , 0x60 } },
   /* 162 - 330 MHz: CP/T = 0011, */ { 330000 , { 0x9C , 0x60 } },
   /* 330 - 444 MHz: CP/T = 1011, */ { 444000 , { 0xDC , 0x60 } },
   /* 444 - 610 MHz: CP/T = 0011, */ { 610000 , { 0x9C , 0x60 } },
   /* 610 - 858 MHz: CP/T = 1011, */ { MAXFRQ , { 0xDC , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_TCLDT67WI3RE_625_BB[] =
{
   /* RS = 11 */
   /*  44 - 144 MHz: CP/T = 0111, BS = 00001 */ { 144000 , { 0xBE , 0x01 } },
   /* 144 - 162 MHz: CP/T = 1110, BS = 00001 */ { 162000 , { 0xF6 , 0x01 } },
   /* 162 - 330 MHz: CP/T = 0111, BS = 00010 */ { 330000 , { 0xBE , 0x02 } },
   /* 330 - 444 MHz: CP/T = 1110, BS = 00010 */ { 444000 , { 0xF6 , 0x02 } },
   /* 444 - 610 MHz: CP/T = 0111, BS = 00100 */ { 610000 , { 0xBE , 0x04 } },
   /* 610 - 754 MHz: CP/T = 1110, BS = 00100 */ { 754000 , { 0xF6 , 0x04 } },
   /* 754 - 858 MHz: CP/T = 1111, BS = 00100 */ { MAXFRQ , { 0xFE , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_TCLDT67WI3RE_625_AUX[] =
{
   /* R = 11, ATC = 0, AL = 110 */
   /*  44 - 144 MHz: CP/T = 0011, */ { 144000 , { 0x9E , 0x60 } },
   /* 144 - 162 MHz: CP/T = 1011, */ { 162000 , { 0xDE , 0x60 } },
   /* 162 - 330 MHz: CP/T = 0011, */ { 330000 , { 0x9E , 0x60 } },
   /* 330 - 444 MHz: CP/T = 1011, */ { 444000 , { 0xDE , 0x60 } },
   /* 444 - 610 MHz: CP/T = 0011, */ { 610000 , { 0x9E , 0x60 } },
   /* 610 - 858 MHz: CP/T = 1011, */ { MAXFRQ , { 0xDE , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_TCLDT67WI3RE_50_BB[] =
{
   /* RS = 00 */
   /*  44 - 144 MHz: CP/T = 0001, BS = 00001 */ { 144000 , { 0x88 , 0x01 } },
   /* 144 - 162 MHz: CP/T = 1000, BS = 00001 */ { 162000 , { 0xC0 , 0x01 } },
   /* 162 - 330 MHz: CP/T = 0001, BS = 00010 */ { 330000 , { 0x88 , 0x02 } },
   /* 330 - 444 MHz: CP/T = 1000, BS = 00010 */ { 444000 , { 0xC0 , 0x02 } },
   /* 444 - 610 MHz: CP/T = 0001, BS = 00100 */ { 610000 , { 0x88 , 0x04 } },
   /* 610 - 754 MHz: CP/T = 1000, BS = 00100 */ { 754000 , { 0xC0 , 0x04 } },
   /* 754 - 858 MHz: CP/T = 1001, BS = 00100 */ { MAXFRQ , { 0xC8 , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_TCLDT67WI3RE_50_AUX[] =
{
   /* R = 00, ATC = 0, AL = 110 */
   /*  44 - 144 MHz: CP/T = 0011, */ { 144000 , { 0x98 , 0x60 } },
   /* 144 - 162 MHz: CP/T = 1011, */ { 162000 , { 0xD8 , 0x60 } },
   /* 162 - 330 MHz: CP/T = 0011, */ { 330000 , { 0x98 , 0x60 } },
   /* 330 - 444 MHz: CP/T = 1011, */ { 444000 , { 0xD8 , 0x60 } },
   /* 444 - 610 MHz: CP/T = 0011, */ { 610000 , { 0x98 , 0x60 } },
   /* 610 - 858 MHz: CP/T = 1011, */ { MAXFRQ , { 0xD8 , 0x60 } }
};

static TUNERCANNEDControlTable_t TCT_TCLDT67WI3RE[] =
{
    {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_TCLDT67WI3RE_166_BB
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "166.7kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_TCLDT67WI3RE_166_AUX
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_TCLDT67WI3RE_625_BB
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_TCLDT67WI3RE_625_AUX
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "50kHz",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_TCLDT67WI3RE_50_BB
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "50kHz",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36150,
      /* controlUnits             */ TCU_TCLDT67WI3RE_50_AUX
   },
};

static TUNERSubMode_t subModesTCLDT67WI3RE[] =
{
   "Ext. RF-AGC, 166.7kHz",   /* Submode 0 */
   "Ext. RF-AGC,  62.5kHz",   /* Submode 1 */
   "Ext. RF-AGC,  50  kHz"    /* Submode 2 */
};

static TUNERCANNEDData_t dataTCLDT67WI3RE =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_TCLDT67WI3RE),
   /* nrOfControlBytes  */          2,
};

static TUNERCommonAttr_t commonAttrTCLDT67WI3RE =
{
   /* name            */  "TCL DT67WI-3R-E",
   /* minFreqRF [kHz] */      40000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesTCLDT67WI3RE),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36130,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0,
};

static TUNERInstance_t tunerTCLDT67WI3RE =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrTCLDT67WI3RE,
   /* myExtAttr    */ &dataTCLDT67WI3RE,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-TCL-DT67WI-R3-E-----------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Thomson DTT 7130x tuner configuration                                      */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_71303_BALANCED
#define TUNER_THOMSON_DTT_71303_BALANCED
#endif
#ifndef TUNER_THOMSON_DTT_71306_BALANCED
#define TUNER_THOMSON_DTT_71306_BALANCED
#endif
#ifndef TUNER_THOMSON_DTT_71306_UNBALANCED
#define TUNER_THOMSON_DTT_71306_UNBALANCED
#endif
#ifndef TUNER_THOMSON_DTT_71303A
#define TUNER_THOMSON_DTT_71303A
#endif
#ifndef TUNER_THOMSON_DTT_7130X_INTERNAL
#define TUNER_THOMSON_DTT_7130X_INTERNAL
#endif
#endif

#if defined TUNER_THOMSON_DTT_71303_BALANCED || \
    defined TUNER_THOMSON_DTT_71306_BALANCED || \
    defined TUNER_THOMSON_DTT_71306_UNBALANCED || \
    defined TUNER_THOMSON_DTT_71303A || \
    defined TUNER_THOMSON_DTT_7130X_INTERNAL

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7130X_CB1BB_166KHZ_106[] =
{
   /* CB1: ATP=010, RS=000 (== 0x90) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x90 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x90 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x90 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x90 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0110 */ { 250000 , { 0x90 , 0x06 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0110 */ { 350000 , { 0x90 , 0x46 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0110 */ { 410000 , { 0x90 , 0x86 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0110 */ { 430000 , { 0x90 , 0xC6 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1100 */ { 470000 , { 0x90 , 0x0C } },
   /* 470 - 725 MHz: BB: CP=01, BS=1100 */ { 725000 , { 0x90 , 0x4C } },
   /* 750 - 810 MHz: BB: CP=10, BS=1100 */ { 810000 , { 0x90 , 0x8C } },
   /* 810 -     MHz: BB: CP=11, BS=1100 */ { MAXFRQ , { 0x90 , 0xCC } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7130X_CB1BB_166KHZ_109[] =
{
   /* CB1: ATP=001, RS=000 (== 0x88) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x88 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x88 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x88 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x88 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0110 */ { 250000 , { 0x88 , 0x06 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0110 */ { 350000 , { 0x88 , 0x46 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0110 */ { 410000 , { 0x88 , 0x86 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0110 */ { 430000 , { 0x88 , 0xC6 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1100 */ { 470000 , { 0x88 , 0x0C } },
   /* 470 - 725 MHz: BB: CP=01, BS=1100 */ { 725000 , { 0x88 , 0x4C } },
   /* 750 - 810 MHz: BB: CP=10, BS=1100 */ { 810000 , { 0x88 , 0x8C } },
   /* 810 -     MHz: BB: CP=11, BS=1100 */ { MAXFRQ , { 0x88 , 0xCC } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7130X_CB1BB_166KHZ_112[] =
{
   /* CB1: ATP=000, RS=000 (== 0x80) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x80 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x80 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x80 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x80 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0110 */ { 250000 , { 0x80 , 0x06 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0110 */ { 350000 , { 0x80 , 0x46 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0110 */ { 410000 , { 0x80 , 0x86 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0110 */ { 430000 , { 0x80 , 0xC6 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1100 */ { 470000 , { 0x80 , 0x0C } },
   /* 470 - 725 MHz: BB: CP=01, BS=1100 */ { 725000 , { 0x80 , 0x4C } },
   /* 750 - 810 MHz: BB: CP=10, BS=1100 */ { 810000 , { 0x80 , 0x8C } },
   /* 810 -     MHz: BB: CP=11, BS=1100 */ { MAXFRQ , { 0x80 , 0xCC } }
};
static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7130X_CB1BB_166KHZ_EXT[] =
{
   /* CB1: ATP=111, RS=000 (== 0xB8) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0xB8 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0xB8 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0xB8 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0xB8 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0110 */ { 250000 , { 0xB8 , 0x06 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0110 */ { 350000 , { 0xB8 , 0x46 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0110 */ { 410000 , { 0xB8 , 0x86 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0110 */ { 430000 , { 0xB8 , 0xC6 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1100 */ { 470000 , { 0xB8 , 0x0C } },
   /* 470 - 725 MHz: BB: CP=01, BS=1100 */ { 725000 , { 0xB8 , 0x4C } },
   /* 750 - 810 MHz: BB: CP=10, BS=1100 */ { 810000 , { 0xB8 , 0x8C } },
   /* 810 -     MHz: BB: CP=11, BS=1100 */ { MAXFRQ , { 0xB8 , 0xCC } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7130X_CB1BB_62KHZ_106[] =
{
   /* CB1: ATP=010, RS=011 (== 0x93) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x93 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x93 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x93 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x93 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0110 */ { 250000 , { 0x93 , 0x06 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0110 */ { 350000 , { 0x93 , 0x46 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0110 */ { 410000 , { 0x93 , 0x86 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0110 */ { 430000 , { 0x93 , 0xC6 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1100 */ { 470000 , { 0x93 , 0x0C } },
   /* 470 - 725 MHz: BB: CP=01, BS=1100 */ { 725000 , { 0x93 , 0x4C } },
   /* 750 - 810 MHz: BB: CP=10, BS=1100 */ { 810000 , { 0x93 , 0x8C } },
   /* 810 -     MHz: BB: CP=11, BS=1100 */ { MAXFRQ , { 0x93 , 0xCC } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7130X_CB1BB_62KHZ_109[] =
{
   /* CB1: ATP=001, RS=011 (== 0x8B) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x8B , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x8B , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x8B , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x8B , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0110 */ { 250000 , { 0x8B , 0x06 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0110 */ { 350000 , { 0x8B , 0x46 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0110 */ { 410000 , { 0x8B , 0x86 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0110 */ { 430000 , { 0x8B , 0xC6 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1100 */ { 470000 , { 0x8B , 0x0C } },
   /* 470 - 725 MHz: BB: CP=01, BS=1100 */ { 725000 , { 0x8B , 0x4C } },
   /* 750 - 810 MHz: BB: CP=10, BS=1100 */ { 810000 , { 0x8B , 0x8C } },
   /* 810 -     MHz: BB: CP=11, BS=1100 */ { MAXFRQ , { 0x8B , 0xCC } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7130X_CB1BB_62KHZ_112[] =
{
   /* CB1: ATP=000, RS=011 (== 0x83) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x83 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x83 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x83 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x83 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0110 */ { 250000 , { 0x83 , 0x06 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0110 */ { 350000 , { 0x83 , 0x46 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0110 */ { 410000 , { 0x83 , 0x86 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0110 */ { 430000 , { 0x83 , 0xC6 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1100 */ { 470000 , { 0x83 , 0x0C } },
   /* 470 - 725 MHz: BB: CP=01, BS=1100 */ { 725000 , { 0x83 , 0x4C } },
   /* 750 - 810 MHz: BB: CP=10, BS=1100 */ { 810000 , { 0x83 , 0x8C } },
   /* 810 -     MHz: BB: CP=11, BS=1100 */ { MAXFRQ , { 0x83 , 0xCC } }
};
static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7130X_CB1BB_62KHZ_EXT[] =
{
   /* CB1: ATP=111, RS=011 (== 0xBB) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0xBB , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0xBB , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0xBB , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0xBB , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0110 */ { 250000 , { 0xBB , 0x06 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0110 */ { 350000 , { 0xBB , 0x46 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0110 */ { 410000 , { 0xBB , 0x86 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0110 */ { 430000 , { 0xBB , 0xC6 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1100 */ { 470000 , { 0xBB , 0x0C } },
   /* 470 - 725 MHz: BB: CP=01, BS=1100 */ { 725000 , { 0xBB , 0x4C } },
   /* 750 - 810 MHz: BB: CP=10, BS=1100 */ { 810000 , { 0xBB , 0x8C } },
   /* 810 -     MHz: BB: CP=11, BS=1100 */ { MAXFRQ , { 0xBB , 0xCC } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7130X_CB2BB_SCAN[] =
{
   /* CB2: ATC=1, STBY=0, XTO=0 (== 0xE2) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0xE2 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0xE2 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0xE2 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0xE2 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0110 */ { 250000 , { 0xE2 , 0x06 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0110 */ { 350000 , { 0xE2 , 0x46 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0110 */ { 410000 , { 0xE2 , 0x86 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0110 */ { 430000 , { 0xE2 , 0xC6 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1100 */ { 470000 , { 0xE2 , 0x0C } },
   /* 470 - 725 MHz: BB: CP=01, BS=1100 */ { 725000 , { 0xE2 , 0x4C } },
   /* 750 - 810 MHz: BB: CP=10, BS=1100 */ { 810000 , { 0xE2 , 0x8C } },
   /* 810 -     MHz: BB: CP=11, BS=1100 */ { MAXFRQ , { 0xE2 , 0xCC } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7130X_CB2BB_LOCK[] =
{
   /* CB2: ATC=0, STBY=0, XTO=0 (== 0xC2) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0xC2 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0xC2 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0xC2 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0xC2 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0110 */ { 250000 , { 0xC2 , 0x06 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0110 */ { 350000 , { 0xC2 , 0x46 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0110 */ { 410000 , { 0xC2 , 0x86 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0110 */ { 430000 , { 0xC2 , 0xC6 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1100 */ { 470000 , { 0xC2 , 0x0C } },
   /* 470 - 725 MHz: BB: CP=01, BS=1100 */ { 725000 , { 0xC2 , 0x4C } },
   /* 750 - 810 MHz: BB: CP=10, BS=1100 */ { 810000 , { 0xC2 , 0x8C } },
   /* 810 -     MHz: BB: CP=11, BS=1100 */ { MAXFRQ , { 0xC2 , 0xCC } }
};

static TUNERSubMode_t subModesThomsonDTT7130X[] =
{
   "External RF-AGC", /* Submode 0 */
   "TOP 112dBuV",     /* Submode 1 */
   "TOP 109dBuV",     /* Submode 2 */
   "TOP 106dBuV"      /* Submode 3 */
};



#endif /* defined TUNER_THOMSON_DTT_71303_BALANCED or
          defined TUNER_THOMSON_DTT_71306_BALANCED or
          defined TUNER_THOMSON_DTT_71306_UNBALANCED or
          defined TUNER_THOMSON_DTT_71303A */

#if defined TUNER_THOMSON_DTT_71303_BALANCED || \
    defined TUNER_THOMSON_DTT_71306_BALANCED || \
    defined TUNER_THOMSON_DTT_71303A

static TUNERCANNEDControlTable_t TCT_ThomsonDTT7130x_balanced[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB3,
      /* description              */ "TOP 106dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_166KHZ_106
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB2,
      /* description              */ "TOP 109dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_166KHZ_109
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB1,
      /* description              */ "TOP 112dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_166KHZ_112
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB0,
      /* description              */ "External RF-AGC, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_166KHZ_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUBALL,
      /* description              */ "ATC high current",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB2BB_SCAN
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUBALL,
      /* description              */ "ATC low current",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB2BB_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB3,
      /* description              */ "TOP 106dBuV, ref.div.= 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_62KHZ_106
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB2,
      /* description              */ "TOP 109dBuV, ref.div.= 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_62KHZ_109
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB1,
      /* description              */ "TOP 112dBuV, ref.div.= 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_62KHZ_112
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB0,
      /* description              */ "External RF-AGC, ref.div.= 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_62KHZ_EXT
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1   |
                                     TUNER_MODE_SUB2   |
                                     TUNER_MODE_SUB3,
      /* description              */ "ATC high current",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB2BB_SCAN
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_LOCK   |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1   |
                                     TUNER_MODE_SUB2   |
                                     TUNER_MODE_SUB3,
      /* description              */ "ATC low current",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB2BB_LOCK
   }
};

static TUNERCANNEDData_t dataThomsonDTT7130X_balanced =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_ThomsonDTT7130x_balanced),
   /* nrOfControlBytes  */          2,
};

#endif /* defined TUNER_THOMSON_DTT_71303_BALANCED || \
          defined TUNER_THOMSON_DTT_71306_BALANCED || \
          defined TUNER_THOMSON_DTT_71303A */

#ifdef  TUNER_THOMSON_DTT_71303A
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT71303A
#endif

static TUNERCommonAttr_t commonAttrThomsonDTT71303A =
{
   /* name            */  "Thomson DTT 71303A",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          2,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT7130X),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */  ( TUNER_WITHOUT_IFAMP | TUNER_EXTERNAL_SAW_SWITCHING )
};

static TUNERInstance_t tunerThomsonDTT71303A =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT71303A,
   /* myExtAttr    */ &dataThomsonDTT7130X_balanced,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif /* TUNER_THOMSON_DTT_71303_BALANCED */

#ifdef  TUNER_THOMSON_DTT_71303_BALANCED
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT71303_balanced
#endif

static TUNERCommonAttr_t commonAttrThomsonDTT71303_balanced =
{
   /* name            */  "Thomson DTT 71303 balanced",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT7130X),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */  ( TUNER_WITHOUT_IFAMP | TUNER_EXTERNAL_SAW_SWITCHING )
};

static TUNERInstance_t tunerThomsonDTT71303_balanced =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT71303_balanced,
   /* myExtAttr    */ &dataThomsonDTT7130X_balanced,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif /* TUNER_THOMSON_DTT_71303_BALANCED */

#ifdef  TUNER_THOMSON_DTT_7130X_INTERNAL
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT7130x_internal
#endif

static TUNERCommonAttr_t commonAttrThomsonDTT7130x_internal =
{
   /* name            */  "Thomson DTT 7130x int. RF-AGC",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT7130X),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */  ( TUNER_WITHOUT_IFAMP | TUNER_EXTERNAL_SAW_SWITCHING )
};

static TUNERInstance_t tunerThomsonDTT7130x_internal =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT7130x_internal,
   /* myExtAttr    */ &dataThomsonDTT7130X_balanced,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif /* TUNER_THOMSON_DTT_7130X_INTERNAL */


#ifdef  TUNER_THOMSON_DTT_71306_BALANCED
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT71306_balanced
#endif



static TUNERCommonAttr_t commonAttrThomsonDTT71306_balanced =
{
   /* name            */  "Thomson DTT 71306 balanced",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT7130X),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */  ( TUNER_WITHOUT_IFAMP | TUNER_EXTERNAL_SAW_SWITCHING )
};

static TUNERInstance_t tunerThomsonDTT71306_balanced =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT71306_balanced,
   /* myExtAttr    */ &dataThomsonDTT7130X_balanced,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif /* TUNER_THOMSON_DTT_71306_BALANCED */


#ifdef  TUNER_THOMSON_DTT_71306_UNBALANCED
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT71306_unbalanced
#endif



static TUNERCANNEDControlTable_t TCT_ThomsonDTT7130x_unbalanced[] =

{
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB3,
      /* description              */ "TOP 106dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_166KHZ_106
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB2,
      /* description              */ "TOP 109dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_166KHZ_109
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB1,
      /* description              */ "TOP 112dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_166KHZ_112
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB0,
      /* description              */ "External RF-AGC, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_166KHZ_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUBALL,
      /* description              */ "ATC high current",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB2BB_SCAN
   },
   {
      /* modes                    */ TUNER_MODE_DIGITAL |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUBALL,
      /* description              */ "ATC low current",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB2BB_LOCK
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB3,
      /* description              */ "TOP 106dBuV, ref.div.= 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_62KHZ_106
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB2,
      /* description              */ "TOP 109dBuV, ref.div.= 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_62KHZ_109
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB1,
      /* description              */ "TOP 112dBuV, ref.div.= 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_62KHZ_112
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUB0,
      /* description              */ "External RF-AGC, ref.div.= 62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB1BB_62KHZ_EXT
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUBALL,
      /* description              */ "ATC high current",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB2BB_SCAN
   },
   {
      /* modes                    */ TUNER_MODE_ANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ | TUNER_MODE_SUBALL,
      /* description              */ "ATC low current",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7130X_CB2BB_LOCK
   }
};

static TUNERCANNEDData_t dataThomsonDTT7130X_unbalanced =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_ThomsonDTT7130x_unbalanced),
   /* nrOfControlBytes  */          2,
};

static TUNERCommonAttr_t commonAttrThomsonDTT71306_unbalanced =
{
   /* name            */  "Thomson DTT 71306 unbalanced",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT7130X),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */  ( TUNER_WITHOUT_IFAMP | TUNER_EXTERNAL_SAW_SWITCHING )
};

static TUNERInstance_t tunerThomsonDTT71306_unbalanced =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT71306_unbalanced,
   /* myExtAttr    */ &dataThomsonDTT7130X_unbalanced,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif /* TUNER_THOMSON_DTT_71306_UNBALANCED */

/*----------------------------------------------------------------------------*/
/*----end-Thomson-DTT-7130x---------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Thomson DTT 7540x tuner configuration                                      */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_7540X_K
#define TUNER_THOMSON_DTT_7540X_K
#endif
#endif

#ifdef  TUNER_THOMSON_DTT_7540X_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT7540x_K
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB1BB_AGCDSB_8MHZ_K[] =
{
   /* CB1: ATP=111, RS=000 (== 0xB8), P5 = 0 (8MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0xB8 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0xB8 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0xB8 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0xB8 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0xB8 , 0x02 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0xB8 , 0x42 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0xB8 , 0x82 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0xB8 , 0xC2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0xB8 , 0x08 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0xB8 , 0x48 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0xB8 , 0x88 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0xB8 , 0xC8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB1BB_AGCDSB_7MHZ_K[] =
{
   /* CB1: ATP=111, RS=000 (== 0xB8), P5 = 1 (7MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0xB8 , 0x11 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0xB8 , 0x51 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0xB8 , 0x91 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0xB8 , 0xD1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0xB8 , 0x12 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0xB8 , 0x52 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0xB8 , 0x92 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0xB8 , 0xD2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0xB8 , 0x18 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0xB8 , 0x58 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0xB8 , 0x98 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0xB8 , 0xD8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB1BB_106_8MHZ_K[] =
{
   /* CB1: ATP=010, RS=000 (== 0x90), P5 = 0 (8MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x90 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x90 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x90 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x90 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0x90 , 0x02 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0x90 , 0x42 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0x90 , 0x82 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0x90 , 0xC2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0x90 , 0x08 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0x90 , 0x48 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0x90 , 0x88 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0x90 , 0xC8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB1BB_106_7MHZ_K[] =
{
   /* CB1: ATP=010, RS=000 (== 0x90), P5 = 1 (7MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x90 , 0x11 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x90 , 0x51 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x90 , 0x91 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x90 , 0xD1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0x90 , 0x12 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0x90 , 0x52 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0x90 , 0x92 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0x90 , 0xD2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0x90 , 0x18 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0x90 , 0x58 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0x90 , 0x98 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0x90 , 0xD8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB1BB_109_8MHZ_K[] =
{
   /* CB1: ATP=001, RS=000 (== 0x88), P5 = 0 (8MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x88 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x88 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x88 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x88 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0x88 , 0x02 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0x88 , 0x42 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0x88 , 0x82 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0x88 , 0xC2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0x88 , 0x08 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0x88 , 0x48 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0x88 , 0x88 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0x88 , 0xC8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB1BB_109_7MHZ_K[] =
{
   /* CB1: ATP=001, RS=000 (== 0x88), P5 = 1 (7MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x88 , 0x11 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x88 , 0x51 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x88 , 0x91 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x88 , 0xD1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0x88 , 0x12 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0x88 , 0x52 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0x88 , 0x92 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0x88 , 0xD2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0x88 , 0x18 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0x88 , 0x58 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0x88 , 0x98 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0x88 , 0xD8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB1BB_112_8MHZ_K[] =
{
   /* CB1: ATP=000, RS=000 (== 0x80), P5 = 0 (8MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x80 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x80 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x80 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x80 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0x80 , 0x02 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0x80 , 0x42 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0x80 , 0x82 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0x80 , 0xC2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0x80 , 0x08 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0x80 , 0x48 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0x80 , 0x88 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0x80 , 0xC8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB1BB_112_7MHZ_K[] =
{
   /* CB1: ATP=000, RS=000 (== 0x80), P5 = 1 (7MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0x80 , 0x11 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0x80 , 0x51 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0x80 , 0x91 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0x80 , 0xD1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0x80 , 0x12 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0x80 , 0x52 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0x80 , 0x92 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0x80 , 0xD2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0x80 , 0x18 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0x80 , 0x58 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0x80 , 0x98 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0x80 , 0xD8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB2BB_SCAN_8MHZ_K[] =
{
   /* CB2: ATC=1, STBY=0, XTO=0 (== 0xE2), P5 = 0 (8MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0xE2 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0xE2 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0xE2 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0xE2 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0xE2 , 0x02 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0xE2 , 0x42 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0xE2 , 0x82 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0xE2 , 0xC2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0xE2 , 0x08 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0xE2 , 0x48 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0xE2 , 0x88 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0xE2 , 0xC8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB2BB_SCAN_7MHZ_K[] =
{
   /* CB2: ATC=1, STBY=0, XTO=0 (== 0xE2), P5 = 1 (7MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0xE2 , 0x11 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0xE2 , 0x51 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0xE2 , 0x91 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0xE2 , 0xD1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0xE2 , 0x12 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0xE2 , 0x52 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0xE2 , 0x92 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0xE2 , 0xD2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0xE2 , 0x18 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0xE2 , 0x58 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0xE2 , 0x98 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0xE2 , 0xD8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB2BB_LOCK_8MHZ_K[] =
{
   /* CB2: ATC=0, STBY=0, XTO=0 (== 0xC2), P5 = 0 (8MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0xC2 , 0x01 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0xC2 , 0x41 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0xC2 , 0x81 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0xC2 , 0xC1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0xC2 , 0x02 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0xC2 , 0x42 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0xC2 , 0x82 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0xC2 , 0xC2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0xC2 , 0x08 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0xC2 , 0x48 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0xC2 , 0x88 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0xC2 , 0xC8 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7540X_CB2BB_LOCK_7MHZ_K[] =
{
   /* CB2: ATC=0, STBY=0, XTO=0 (== 0xC2), P5 = 1 (7MHz) */
   /*     -  85 MHz: BB: CP=00, BS=0001 */ {  85000 , { 0xC2 , 0x11 } },
   /*  85 - 105 MHz: BB: CP=01, BS=0001 */ { 105000 , { 0xC2 , 0x51 } },
   /* 105 - 130 MHz: BB: CP=10, BS=0001 */ { 130000 , { 0xC2 , 0x91 } },
   /* 130 - 146 MHz: BB: CP=11, BS=0001 */ { 146000 , { 0xC2 , 0xD1 } },
   /* 146 - 250 MHz: BB: CP=00, BS=0010 */ { 250000 , { 0xC2 , 0x12 } },
   /* 250 - 350 MHz: BB: CP=01, BS=0010 */ { 350000 , { 0xC2 , 0x52 } },
   /* 350 - 410 MHz: BB: CP=10, BS=0010 */ { 410000 , { 0xC2 , 0x92 } },
   /* 410 - 430 MHz: BB: CP=11, BS=0010 */ { 430000 , { 0xC2 , 0xD2 } },
   /* 430 - 470 MHz: BB: CP=00, BS=1000 */ { 470000 , { 0xC2 , 0x18 } },
   /* 470 - 725 MHz: BB: CP=01, BS=1000 */ { 725000 , { 0xC2 , 0x58 } },
   /* 725 - 810 MHz: BB: CP=10, BS=1000 */ { 810000 , { 0xC2 , 0x98 } },
   /* 810 -     MHz: BB: CP=10, BS=1000 */ { MAXFRQ , { 0xC2 , 0xD8 } }
};

static TUNERCANNEDControlTable_t TCT_ThomsonDTT7540x_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Internal RF-AGC disable, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB1BB_AGCDSB_8MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Internal RF-AGC disable, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB1BB_AGCDSB_7MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "TOP 106dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB1BB_106_8MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "TOP 106dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB1BB_106_7MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "TOP 109dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB1BB_109_8MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "TOP 109dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB1BB_109_7MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "TOP 112dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB1BB_112_8MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "TOP 112dBuV, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB1BB_112_7MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUBALL,
      /* description              */ "ATC high current",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB2BB_SCAN_8MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUBALL,
      /* description              */ "ATC high current",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB2BB_SCAN_7MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUBALL,
      /* description              */ "ATC low current",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB2BB_LOCK_8MHZ_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUBALL,
      /* description              */ "ATC low current",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7540X_CB2BB_LOCK_7MHZ_K
   }
};

static TUNERSubMode_t subModesThomsonDTT7540x_K[] =
{
   "External RF-AGC", /* TUNER_MODE_SUB0 */
   "TOP 106dBuV",     /* TUNER_MODE_SUB1 */
   "TOP 109dBuV",     /* TUNER_MODE_SUB2 */
   "TOP 112dBuV"      /* TUNER_MODE_SUB3 */
};

static TUNERCANNEDData_t dataThomsonDTT7540x_K =
{
   /* lockMask     */       0x40,
   /* lockValue    */       0x40,
   /* controlTable */ ARRAYREF(TCT_ThomsonDTT7540x_K),
   /* nrOfControlBytes  */     2
};

static TUNERCommonAttr_t commonAttrThomsonDTT7540x_K =
{
   /* name            */  "Thomson DTT 7540x",
   /* minFreqRF [kHz] */      47000,
   /* maxFreqRF [kHz] */     862000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT7540x_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerThomsonDTT7540x_K =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT7540x_K,
   /* myExtAttr    */ &dataThomsonDTT7540x_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Thomson-DTT-7540x---------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Thomson DTT 754xx tuner configuration                                      */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_7543X_K
#define TUNER_THOMSON_DTT_7543X_K
#endif
#ifndef TUNER_THOMSON_DTT_7546X_K
#define TUNER_THOMSON_DTT_7546X_K
#endif
#endif


#ifdef  TUNER_THOMSON_DTT_7543X_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT7543x_K
#endif
#endif

#ifdef  TUNER_THOMSON_DTT_7546X_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT7546x_K
#endif
#endif

#if defined TUNER_THOMSON_DTT_7543X_K || \
    defined TUNER_THOMSON_DTT_7546X_K

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT754xx_AGC_DSB_8_166_K[] =
{
   /* CB1: ATP=111, RS=000      (== 0xB8) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=0001 */ {  85000 , { 0xB8 , 0x01 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=0001 */ { 105000 , { 0xB8 , 0x41 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=0001 */ { 130000 , { 0xB8 , 0x81 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=0001 */ { 146000 , { 0xB8 , 0xC1 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=0010 */ { 250000 , { 0xB8 , 0x02 , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=0010 */ { 350000 , { 0xB8 , 0x42 , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=0010 */ { 410000 , { 0xB8 , 0x82 , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=0010 */ { 430000 , { 0xB8 , 0xC2 , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=1000 */ { 470000 , { 0xB8 , 0x08 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=1000 */ { 725000 , { 0xB8 , 0x48 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=1000 */ { 810000 , { 0xB8 , 0x88 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=1000 */ { MAXFRQ , { 0xB8 , 0xC8 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT754xx_AGC_DSB_7_166_K[] =
{
   /* CB1: ATP=111, RS=000      (== 0xB8) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /*     - 121 MHz: BB: CP=00, P5=1, BS=0001 */ {  85000 , { 0xB8 , 0x11 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=1, BS=0001 */ { 105000 , { 0xB8 , 0x51 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=1, BS=0001 */ { 130000 , { 0xB8 , 0x91 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=1, BS=0001 */ { 146000 , { 0xB8 , 0xD1 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=1, BS=0010 */ { 250000 , { 0xB8 , 0x12 , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=1, BS=0010 */ { 350000 , { 0xB8 , 0x52 , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=1, BS=0010 */ { 410000 , { 0xB8 , 0x92 , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=1, BS=0010 */ { 430000 , { 0xB8 , 0xD2 , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=1, BS=1000 */ { 470000 , { 0xB8 , 0x18 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=1, BS=1000 */ { 725000 , { 0xB8 , 0x58 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=1, BS=1000 */ { 810000 , { 0xB8 , 0x98 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=1, BS=1000 */ { MAXFRQ , { 0xB8 , 0xD8 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT754xx_AGC_DSB_8_625_K[] =
{
   /* CB1: ATP=111, RS=011      (== 0xB8) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=0001 */ {  85000 , { 0xBB , 0x01 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=0001 */ { 105000 , { 0xBB , 0x41 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=0001 */ { 130000 , { 0xBB , 0x81 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=0001 */ { 146000 , { 0xBB , 0xC1 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=0010 */ { 250000 , { 0xBB , 0x02 , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=0010 */ { 350000 , { 0xBB , 0x42 , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=0010 */ { 410000 , { 0xBB , 0x82 , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=0010 */ { 430000 , { 0xBB , 0xC2 , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=1000 */ { 470000 , { 0xBB , 0x08 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=1000 */ { 725000 , { 0xBB , 0x48 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=1000 */ { 810000 , { 0xBB , 0x88 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=1000 */ { MAXFRQ , { 0xBB , 0xC8 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT754xx_AGC_DSB_7_625_K[] =
{
   /* CB1: ATP=111, RS=011      (== 0xBB) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /*     - 121 MHz: BB: CP=00, P5=1, BS=0001 */ {  85000 , { 0xBB , 0x11 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=1, BS=0001 */ { 105000 , { 0xBB , 0x51 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=1, BS=0001 */ { 130000 , { 0xBB , 0x91 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=1, BS=0001 */ { 146000 , { 0xBB , 0xD1 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=1, BS=0010 */ { 250000 , { 0xBB , 0x12 , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=1, BS=0010 */ { 350000 , { 0xBB , 0x52 , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=1, BS=0010 */ { 410000 , { 0xBB , 0x92 , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=1, BS=0010 */ { 430000 , { 0xBB , 0xD2 , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=1, BS=1000 */ { 470000 , { 0xBB , 0x18 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=1, BS=1000 */ { 725000 , { 0xBB , 0x58 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=1, BS=1000 */ { 810000 , { 0xBB , 0x98 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=1, BS=1000 */ { MAXFRQ , { 0xBB , 0xD8 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT754xx_AGC_DSB_8_50_K[] =
{
   /* CB1: ATP=111, RS=011      (== 0xBD) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=0001 */ {  85000 , { 0xBD , 0x01 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=0001 */ { 105000 , { 0xBD , 0x41 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=0001 */ { 130000 , { 0xBD , 0x81 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=0001 */ { 146000 , { 0xBD , 0xC1 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=0010 */ { 250000 , { 0xBD , 0x02 , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=0010 */ { 350000 , { 0xBD , 0x42 , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=0010 */ { 410000 , { 0xBD , 0x82 , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=0010 */ { 430000 , { 0xBD , 0xC2 , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=1000 */ { 470000 , { 0xBD , 0x08 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=1000 */ { 725000 , { 0xBD , 0x48 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=1000 */ { 810000 , { 0xBD , 0x88 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=1000 */ { MAXFRQ , { 0xBD , 0xC8 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT754xx_AGC_DSB_7_50_K[] =
{
   /* CB1: ATP=111, RS=011      (== 0xBD) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /*     - 121 MHz: BB: CP=00, P5=1, BS=0001 */ {  85000 , { 0xBD , 0x11 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=1, BS=0001 */ { 105000 , { 0xBD , 0x51 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=1, BS=0001 */ { 130000 , { 0xBD , 0x91 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=1, BS=0001 */ { 146000 , { 0xBD , 0xD1 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=1, BS=0010 */ { 250000 , { 0xBD , 0x12 , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=1, BS=0010 */ { 350000 , { 0xBD , 0x52 , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=1, BS=0010 */ { 410000 , { 0xBD , 0x92 , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=1, BS=0010 */ { 430000 , { 0xBD , 0xD2 , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=1, BS=1000 */ { 470000 , { 0xBD , 0x18 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=1, BS=1000 */ { 725000 , { 0xBD , 0x58 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=1, BS=1000 */ { 810000 , { 0xBD , 0x98 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=1, BS=1000 */ { MAXFRQ , { 0xBD , 0xD8 , 0xC3 } }
};

static TUNERCANNEDControlTable_t TCT_ThomsonDTT754xx_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext. Agc, 7MHz, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT754xx_AGC_DSB_7_166_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext. Agc, 8MHz, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT754xx_AGC_DSB_8_166_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext. Agc, 7MHz, ref.div.=62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT754xx_AGC_DSB_7_625_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext. Agc, 8MHz, ref.div.=62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT754xx_AGC_DSB_8_625_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Ext. Agc, 7MHz, ref.div.=50kHz",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */     1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT754xx_AGC_DSB_7_50_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Ext. Agc, 8MHz, ref.div.=50kHz",
      /* refFreqNumerator   [kHz] */    50,
      /* refFreqDenominator       */    1,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT754xx_AGC_DSB_8_50_K
   }
};

static TUNERSubMode_t subModesThomsonDTT754xx_K[] =
{
   "Ext. RF-AGC 166.7kHz", /* SUBMODE 0 */
   "Ext. RF-AGC 62.5kHz",  /* SUBMODE 1 */
   "Ext. RF-AGC 50kHz"     /* SUBMODE 2 */
};

static TUNERCANNEDData_t dataThomsonDTT754xx_K =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_ThomsonDTT754xx_K),
   /* nrOfControlBytes  */          3
};

#endif /*   defined TUNER_THOMSON_DTT_7543X_K || \
            defined TUNER_THOMSON_DTT_7546X_K */
#ifdef TUNER_THOMSON_DTT_7543X_K
static TUNERCommonAttr_t commonAttrThomsonDTT7543x_K =
{
   /* name            */  "Thomson DTT 7543x",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT754xx_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerThomsonDTT7543x_K =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT7543x_K,
   /* myExtAttr    */ &dataThomsonDTT754xx_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif /* TUNER_THOMSON_DTT_7543X_K */

#ifdef TUNER_THOMSON_DTT_7546X_K
static TUNERCommonAttr_t commonAttrThomsonDTT7546x_K =
{
   /* name            */  "Thomson DTT 7546x",
   /* minFreqRF [kHz] */      51000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT754xx_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerThomsonDTT7546x_K =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT7546x_K,
   /* myExtAttr    */ &dataThomsonDTT754xx_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif /* TUNER_THOMSON_DTT_7546X_K */
/*----------------------------------------------------------------------------*/
/*----end-Thomson-DTT-754xx---------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Thomson DTT 7546x tuner configuration                                      */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_7544X_K
#define TUNER_THOMSON_DTT_7544X_K
#endif
#endif

#ifdef  TUNER_THOMSON_DTT_7544X_K
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT7544x_K
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7544x_166MHz_8MHz_AntennaOff_K[] =
{
   /* CB1: ATP=111, RS=000      (== 0xB8) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /* BS3 = 1 : 8MHz                      */
   /* BS4 = 0 : Antenna power off         */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=0101 */ {  85000 , { 0xB8 , 0x05 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=0101 */ { 105000 , { 0xB8 , 0x45 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=0101 */ { 130000 , { 0xB8 , 0x85 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=0101 */ { 146000 , { 0xB8 , 0xC5 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=0110 */ { 250000 , { 0xB8 , 0x06 , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=0110 */ { 350000 , { 0xB8 , 0x46 , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=0110 */ { 410000 , { 0xB8 , 0x86 , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=0110 */ { 430000 , { 0xB8 , 0xC6 , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=0100 */ { 470000 , { 0xB8 , 0x04 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=0100 */ { 725000 , { 0xB8 , 0x44 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=0100 */ { 810000 , { 0xB8 , 0x84 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=0100 */ { MAXFRQ , { 0xB8 , 0xC4 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7544x_166MHz_8MHz_AntennaOn_K[] =
{
   /* CB1: ATP=111, RS=000      (== 0xB8) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /* BS3 = 1 : 8MHz                      */
   /* BS4 = 1 : Antenna power on          */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=1101 */ {  85000 , { 0xB8 , 0x0D , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=1101 */ { 105000 , { 0xB8 , 0x4D , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=1101 */ { 130000 , { 0xB8 , 0x8D , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=1101 */ { 146000 , { 0xB8 , 0xCD , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=1110 */ { 250000 , { 0xB8 , 0x0E , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=1110 */ { 350000 , { 0xB8 , 0x4E , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=1110 */ { 410000 , { 0xB8 , 0x8E , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=1110 */ { 430000 , { 0xB8 , 0xCE , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=1100 */ { 470000 , { 0xB8 , 0x0C , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=1100 */ { 725000 , { 0xB8 , 0x4C , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=1100 */ { 810000 , { 0xB8 , 0x8C , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=1100 */ { MAXFRQ , { 0xB8 , 0xCC , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7544x_166MHz_7MHz_AntennaOff_K[] =
{
   /* CB1: ATP=111, RS=000      (== 0xB8) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /* BS3 = 0 : 7MHz                      */
   /* BS4 = 0 : Antenna power off         */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=0001 */ {  85000 , { 0xB8 , 0x01 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=0001 */ { 105000 , { 0xB8 , 0x41 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=0001 */ { 130000 , { 0xB8 , 0x81 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=0001 */ { 146000 , { 0xB8 , 0xC1 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=0010 */ { 250000 , { 0xB8 , 0x02 , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=0010 */ { 350000 , { 0xB8 , 0x42 , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=0010 */ { 410000 , { 0xB8 , 0x82 , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=0010 */ { 430000 , { 0xB8 , 0xC2 , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=0000 */ { 470000 , { 0xB8 , 0x00 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=0000 */ { 725000 , { 0xB8 , 0x40 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=0000 */ { 810000 , { 0xB8 , 0x80 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=0000 */ { MAXFRQ , { 0xB8 , 0xC0 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7544x_166MHz_7MHz_AntennaOn_K[] =
{
   /* CB1: ATP=111, RS=000      (== 0xB8) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /* BS3 = 0 : 7MHz                      */
   /* BS4 = 1 : Antenna power on          */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=1001 */ {  85000 , { 0xB8 , 0x09 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=1001 */ { 105000 , { 0xB8 , 0x49 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=1001 */ { 130000 , { 0xB8 , 0x89 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=1001 */ { 146000 , { 0xB8 , 0xC9 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=1010 */ { 250000 , { 0xB8 , 0x0A , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=1010 */ { 350000 , { 0xB8 , 0x4A , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=1010 */ { 410000 , { 0xB8 , 0x8A , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=1010 */ { 430000 , { 0xB8 , 0xCA , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=1000 */ { 470000 , { 0xB8 , 0x08 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=1000 */ { 725000 , { 0xB8 , 0x48 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=1000 */ { 810000 , { 0xB8 , 0x88 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=1000 */ { MAXFRQ , { 0xB8 , 0xC8 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7544x_62MHz_8MHz_AntennaOff_K[] =
{
   /* CB1: ATP=111, RS=011      (== 0xBB) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /* BS3 = 1 : 8MHz                      */
   /* BS4 = 0 : Antenna power off         */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=0101 */ {  85000 , { 0xBB , 0x05 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=0101 */ { 105000 , { 0xBB , 0x45 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=0101 */ { 130000 , { 0xBB , 0x85 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=0101 */ { 146000 , { 0xBB , 0xC5 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=0110 */ { 250000 , { 0xBB , 0x06 , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=0110 */ { 350000 , { 0xBB , 0x46 , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=0110 */ { 410000 , { 0xBB , 0x86 , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=0110 */ { 430000 , { 0xBB , 0xC6 , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=0100 */ { 470000 , { 0xBB , 0x04 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=0100 */ { 725000 , { 0xBB , 0x44 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=0100 */ { 810000 , { 0xBB , 0x84 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=0100 */ { MAXFRQ , { 0xBB , 0xC4 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7544x_62MHz_8MHz_AntennaOn_K[] =
{
   /* CB1: ATP=111, RS=011      (== 0xBB) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /* BS3 = 1 : 8MHz                      */
   /* BS4 = 1 : Antenna power on          */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=1101 */ {  85000 , { 0xBB , 0x0D , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=1101 */ { 105000 , { 0xBB , 0x4D , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=1101 */ { 130000 , { 0xBB , 0x8D , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=1101 */ { 146000 , { 0xBB , 0xCD , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=1110 */ { 250000 , { 0xBB , 0x0E , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=1110 */ { 350000 , { 0xBB , 0x4E , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=1110 */ { 410000 , { 0xBB , 0x8E , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=1110 */ { 430000 , { 0xBB , 0xCE , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=1100 */ { 470000 , { 0xBB , 0x0C , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=1100 */ { 725000 , { 0xBB , 0x4C , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=1100 */ { 810000 , { 0xBB , 0x8C , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=1100 */ { MAXFRQ , { 0xBB , 0xCC , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7544x_62MHz_7MHz_AntennaOff_K[] =
{
   /* CB1: ATP=111, RS=011      (== 0xBB) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /* BS3 = 0 : 7MHz                      */
   /* BS4 = 0 : Antenna power off         */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=0001 */ {  85000 , { 0xBB , 0x01 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=0001 */ { 105000 , { 0xBB , 0x41 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=0001 */ { 130000 , { 0xBB , 0x81 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=0001 */ { 146000 , { 0xBB , 0xC1 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=0010 */ { 250000 , { 0xBB , 0x02 , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=0010 */ { 350000 , { 0xBB , 0x42 , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=0010 */ { 410000 , { 0xBB , 0x82 , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=0010 */ { 430000 , { 0xBB , 0xC2 , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=0000 */ { 470000 , { 0xBB , 0x00 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=0000 */ { 725000 , { 0xBB , 0x40 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=0000 */ { 810000 , { 0xBB , 0x80 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=0000 */ { MAXFRQ , { 0xBB , 0xC0 , 0xC3 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT7544x_62MHz_7MHz_AntennaOn_K[] =
{
   /* CB1: ATP=111, RS=011      (== 0xBB) */
   /* CB2: ATC=0, STBY=0, XTO=1 (== 0xC3) */
   /* Fosc:                               */
   /* BS3 = 0 : 7MHz                      */
   /* BS4 = 1 : Antenna power on          */
   /*     - 121 MHz: BB: CP=00, P5=0, BS=1001 */ {  85000 , { 0xBB , 0x09 , 0xC3 } },
   /* 121 - 141 MHz: BB: CP=01, P5=0, BS=1001 */ { 105000 , { 0xBB , 0x49 , 0xC3 } },
   /* 141 - 166 MHz: BB: CP=10, P5=0, BS=1001 */ { 130000 , { 0xBB , 0x89 , 0xC3 } },
   /* 166 - 182 MHz: BB: CP=11, P5=0, BS=1001 */ { 146000 , { 0xBB , 0xC9 , 0xC3 } },
   /* 182 - 286 MHz: BB: CP=00, P5=0, BS=1010 */ { 250000 , { 0xBB , 0x0A , 0xC3 } },
   /* 286 - 386 MHz: BB: CP=01, P5=0, BS=1010 */ { 350000 , { 0xBB , 0x4A , 0xC3 } },
   /* 386 - 446 MHz: BB: CP=10, P5=0, BS=1010 */ { 410000 , { 0xBB , 0x8A , 0xC3 } },
   /* 446 - 466 MHz: BB: CP=11, P5=0, BS=1010 */ { 430000 , { 0xBB , 0xCA , 0xC3 } },
   /* 466 - 506 MHz: BB: CP=00, P5=0, BS=1000 */ { 470000 , { 0xBB , 0x08 , 0xC3 } },
   /* 506 - 761 MHz: BB: CP=01, P5=0, BS=1000 */ { 725000 , { 0xBB , 0x48 , 0xC3 } },
   /* 761 - 846 MHz: BB: CP=10, P5=0, BS=1000 */ { 810000 , { 0xBB , 0x88 , 0xC3 } },
   /* 846 -     MHz: BB: CP=11, P5=0, BS=1000 */ { MAXFRQ , { 0xBB , 0xC8 , 0xC3 } }
};

static TUNERCANNEDControlTable_t TCT_ThomsonDTT7544x_K[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext. Agc, 7MHz, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7544x_166MHz_7MHz_AntennaOff_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext. Agc, 8MHz, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7544x_166MHz_8MHz_AntennaOff_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext. Agc, 7MHz, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7544x_166MHz_7MHz_AntennaOn_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext. Agc, 8MHz, ref.div.=167kHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7544x_166MHz_8MHz_AntennaOn_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Ext. Agc, 7MHz, ref.div.=62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7544x_62MHz_7MHz_AntennaOff_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB2,
      /* description              */ "Ext. Agc, 8MHz, ref.div.=62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7544x_62MHz_8MHz_AntennaOff_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "Ext. Agc, 7MHz, ref.div.=62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7544x_62MHz_7MHz_AntennaOn_K
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB3,
      /* description              */ "Ext. Agc, 8MHz, ref.div.=62.5kHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36000,
      /* controlUnits             */ TCU_ThomsonDTT7544x_62MHz_8MHz_AntennaOn_K
   }
};

static TUNERSubMode_t subModesThomsonDTT7544x_K[] =
{
   "Ext. RF-AGC, 166 kHz, Antenna Power Off",  /* SUBMODE 0 */
   "Ext. RF-AGC, 166 kHz, Antenna Power On",   /* SUBMODE 1 */
   "Ext. RF-AGC, 62.5kHz, Antenna Power Off",  /* SUBMODE 2 */
   "Ext. RF-AGC, 62.5kHz, Antenna Power On"    /* SUBMODE 3 */
};

static TUNERCANNEDData_t dataThomsonDTT7544x_K =
{
   /* lockMask          */       0x40,
   /* lockValue         */       0x40,
   /* controlTable      */ ARRAYREF(TCT_ThomsonDTT7544x_K),
   /* nrOfControlBytes  */          3
};

static TUNERCommonAttr_t commonAttrThomsonDTT7544x_K =
{
   /* name            */  "Thomson DTT 7544x",
   /* minFreqRF [kHz] */      40000,
   /* maxFreqRF [kHz] */     864000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT7544x_K),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerThomsonDTT7544x_K =
{
   /* I2C address  */ { 0xC0 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT7544x_K,
   /* myExtAttr    */ &dataThomsonDTT7544x_K,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Thomson-DTT-7544x---------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Thomson DTT 768xx tuner configuration                                      */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_THOMSON_DTT_768XX
#define TUNER_THOMSON_DTT_768XX
#endif
#ifndef TUNER_THOMSON_DTT_768XX_INT
#define TUNER_THOMSON_DTT_768XX_INT
#endif
#endif

#ifdef  TUNER_THOMSON_DTT_768XX
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT768xx
#endif
#endif

#ifdef  TUNER_THOMSON_DTT_768XX_INT
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerThomsonDTT768xx_int
#endif
#endif

#if defined TUNER_THOMSON_DTT_768XX || \
    defined TUNER_THOMSON_DTT_768XX_INT

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT768XX_EXT_RFAGC[] =
{
   /*     - 147 MHz:  */ { 149000 , { 0x85 , 0x39, 0x14, 0xA0 } },
   /* 153 - 417 MHz:  */ { 419000 , { 0xA5 , 0x7C, 0x14, 0xA0 } },
   /* 423 - 863 MHz:  */ { MAXFRQ , { 0xA5 , 0xB5, 0x14, 0xA0 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT768XX_INT_FAST_TOP112[] =
{
   /*     - 147 MHz:  */ { 149000 , { 0x85 , 0x39, 0x14, 0x80 } },
   /* 153 - 417 MHz:  */ { 419000 , { 0xA5 , 0x7C, 0x14, 0x80 } },
   /* 423 - 863 MHz:  */ { MAXFRQ , { 0xA5 , 0xB5, 0x14, 0x80 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT768XX_INT_SLOW_TOP112[] =
{
   /*     - 147 MHz:  */ { 149000 , { 0x85 , 0x39, 0x34, 0x80 } },
   /* 153 - 417 MHz:  */ { 419000 , { 0xA5 , 0x7C, 0x34, 0x80 } },
   /* 423 - 863 MHz:  */ { MAXFRQ , { 0xA5 , 0xB5, 0x34, 0x80 } }
};

#if 0 /* TODO remove? Definitions cause compiler warnings, because not used */
static TUNERCANNEDControlUnit_t TCU_ThomsonDTT768XX_INT_FAST_TOP116[] =
{
   /*     - 147 MHz: */ { 149000 , { 0xC5 , 0x39, 0x12, 0x80 } },
   /* 153 - 417 MHz: */ { 417000 , { 0xC5 , 0x7C, 0x12, 0x80 } },
   /* 423 - 863 MHz: */ { MAXFRQ , { 0xC5 , 0xB5, 0x12, 0x80 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT768XX_INT_SLOW_TOP116[] =
{
   /*     - 147 MHz: */ { 149000 , { 0xC5 , 0x39, 0x32, 0x80 } },
   /* 153 - 417 MHz: */ { 417000 , { 0xC5 , 0x7C, 0x32, 0x80 } },
   /* 423 - 863 MHz: */ { MAXFRQ , { 0xC5 , 0xB5, 0x32, 0x80 } }
};
#endif

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT768XX_INT_FAST_TOP120[] =
{
   /*     - 147 MHz:  */ { 149000 , { 0x85 , 0x39, 0x10, 0x80 } },
   /* 153 - 417 MHz:  */ { 419000 , { 0xA5 , 0x7C, 0x10, 0x80 } },
   /* 423 - 863 MHz:  */ { MAXFRQ , { 0xA5 , 0xB5, 0x10, 0x80 } }
};

static TUNERCANNEDControlUnit_t TCU_ThomsonDTT768XX_INT_SLOW_TOP120[] =
{
   /*     - 147 MHz:  */ { 149000 , { 0x85 , 0x39, 0x30, 0x80 } },
   /* 153 - 417 MHz:  */ { 419000 , { 0xA5 , 0x7C, 0x30, 0x80 } },
   /* 423 - 863 MHz:  */ { MAXFRQ , { 0xA5 , 0xB5, 0x30, 0x80 } }
};
static TUNERCANNEDControlTable_t TCT_ThomsonDTT768xx[] =
{
   {
      /* modes                    */   TUNER_MODE_DIGITALANALOG |
                                       TUNER_MODE_SWITCHLOCK |
                                       TUNER_MODE_6MHZ |
                                       TUNER_MODE_SUB0,
      /* description              */ "6 MHz SAW, bandswitch",
      /* refFreqNumerator   [kHz] */  4000,
      /* refFreqDenominator       */    64,
      /* outputFrequency    [kHz] */ 44000,
      /* controlUnits             */ TCU_ThomsonDTT768XX_EXT_RFAGC
   },
   {
      /* modes                    */   TUNER_MODE_ANALOG |
                                       TUNER_MODE_LOCK |
                                       TUNER_MODE_6MHZ |
                                       TUNER_MODE_SUB1,
      /* description              */ "6 MHz SAW, bandswitch",
      /* refFreqNumerator   [kHz] */  4000,
      /* refFreqDenominator       */    64,
      /* outputFrequency    [kHz] */ 44000,
      /* controlUnits             */ TCU_ThomsonDTT768XX_INT_SLOW_TOP120
   },
   {
      /* modes                    */   TUNER_MODE_ANALOG |
                                       TUNER_MODE_SWITCH |
                                       TUNER_MODE_6MHZ |
                                       TUNER_MODE_SUB1,
      /* description              */ "6 MHz SAW, bandswitch",
      /* refFreqNumerator   [kHz] */  4000,
      /* refFreqDenominator       */    64,
      /* outputFrequency    [kHz] */ 44000,
      /* controlUnits             */ TCU_ThomsonDTT768XX_INT_FAST_TOP120
   },
   {
      /* modes                    */   TUNER_MODE_DIGITAL |
                                       TUNER_MODE_LOCK |
                                       TUNER_MODE_6MHZ |
                                       TUNER_MODE_SUB1,
      /* description              */ "6 MHz SAW, bandswitch",
      /* refFreqNumerator   [kHz] */  4000,
      /* refFreqDenominator       */    64,
      /* outputFrequency    [kHz] */ 44000,
      /* controlUnits             */ TCU_ThomsonDTT768XX_INT_SLOW_TOP112
   },
   {
      /* modes                    */   TUNER_MODE_DIGITAL |
                                       TUNER_MODE_SWITCH |
                                       TUNER_MODE_6MHZ |
                                       TUNER_MODE_SUB1,
      /* description              */ "6 MHz SAW, bandswitch",
      /* refFreqNumerator   [kHz] */  4000,
      /* refFreqDenominator       */    64,
      /* outputFrequency    [kHz] */ 44000,
      /* controlUnits             */ TCU_ThomsonDTT768XX_INT_FAST_TOP112
   },
};

static TUNERSubMode_t subModesThomsonDTT768xx[] =
{
   "External RF-AGC",       /* TUNER_MODE_SUB0 */
   "Internal RF-AGC"        /* TUNER_MODE_SUB1 */
};

static TUNERCANNEDData_t dataThomsonDTT768xx=
{
   /* lockMask          */ 0x40,
   /* lockValue         */ 0x40,
   /* controlTable      */ ARRAYREF(TCT_ThomsonDTT768xx),
   /* nrOfControlBytes  */    4
};

#endif

#ifdef TUNER_THOMSON_DTT_768XX
static TUNERCommonAttr_t commonAttrThomsonDTT768xx =
{
   /* name            */  "Thomson DTT 768xx",
   /* minFreqRF [kHz] */      57000,
   /* maxFreqRF [kHz] */     863000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT768xx),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */          0,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerThomsonDTT768xx =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT768xx,
   /* myExtAttr    */ &dataThomsonDTT768xx,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif

#ifdef TUNER_THOMSON_DTT_768XX_INT
static TUNERCommonAttr_t commonAttrThomsonDTT768xx_int =
{
   /* name            */  "Thomson DTT 768xx Int. RF-AGC",
   /* minFreqRF [kHz] */      57000,
   /* maxFreqRF [kHz] */     863000,
   /* sub-mode        */          1,
   /* sub-modes       */  ARRAYREF(subModesThomsonDTT768xx),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */          0,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerThomsonDTT768xx_int =
{
   /* I2C address  */ { 0xC2 , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrThomsonDTT768xx_int,
   /* myExtAttr    */ &dataThomsonDTT768xx,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-Thomson-DTT-768XX---------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Xuguang DTT-8X1C tuner configuration                                       */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_XUGUANG_DTT_8X1C
#define TUNER_XUGUANG_DTT_8X1C
#endif
#ifndef TUNER_XUGUANG_DTT_8X1C_EXT_IF
#define TUNER_XUGUANG_DTT_8X1C_EXT_IF
#endif
#ifndef TUNER_XUGUANG_DTT_8X1C_T1146
#define TUNER_XUGUANG_DTT_8X1C_T1146
#endif
#endif



#ifdef  TUNER_XUGUANG_DTT_8X1C
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerXuguangDTT8X1C
#endif
#endif

#ifdef TUNER_XUGUANG_DTT_8X1C_EXT_IF
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerXuguangDTT8X1CExtIf
#endif
#endif

#ifdef TUNER_XUGUANG_DTT_8X1C_T1146
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerXuguangDTT8X1CT1146
#endif
#endif



#if defined TUNER_XUGUANG_DTT_8X1C || \
    defined TUNER_XUGUANG_DTT_8X1C_EXT_IF || \
    defined TUNER_XUGUANG_DTT_8X1C_T1146

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_AUX_EXT[] =
{
   /* ATC = 0/110 */ { MAXFRQ , { 0x9C , 0x60 } }
};

static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_AUX_SCAN_109[] =
{
   /* ATC = 1/011 */ { MAXFRQ , { 0x9C , 0xB0 } }
};

static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_AUX_LOCK_109[] =
{
   /* ATC = 0/011 */ { MAXFRQ , { 0x9C , 0x30 } }
};

static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_AUX_SCAN_112[] =
{
   /* ATC = 1/010 */ { MAXFRQ , { 0x9C , 0xA0 } }
};

static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_AUX_LOCK_112[] =
{
   /* ATC = 0/010 */ { MAXFRQ , { 0x9C , 0x20 } }
};

static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_AUX_SCAN_115[] =
{
   /* ATC = 1/011 */ { MAXFRQ , { 0x9C , 0x90 } }
};

static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_AUX_LOCK_115[] =
{
   /* ATC = 0/001 */ { MAXFRQ , { 0x9C , 0x10 } }
};


static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_BS_7_166[] =
{
   /* R = 10 */
   /* Fosc     - 180 MHz: CP/T = 0/111, SP = 00001 */ { 144000 , { 0xBC , 0x01 } },
   /* Fosc 180 - 197 MHz: CP/T = 1/110, SP = 00001 */ { 161000 , { 0xF4 , 0x01 } },
   /* Fosc 197 - 366 MHz: CP/T = 0/111, SP = 00010 */ { 330000 , { 0xBC , 0x02 } },
   /* Fosc 366 - 484 MHz: CP/T = 1/110, SP = 00010 */ { 448000 , { 0xF4,  0x02 } },
   /* Fosc 484 - 646 MHz: CP/T = 0/111, SP = 00100 */ { 610000 , { 0xBE , 0x04 } },
   /* Fosc 646 - 790 MHz: CP/T = 1/110, SP = 00100 */ { 754000 , { 0xFC , 0x04 } },
   /* Fosc 790 - 861 MHz: CP/T = 1/111, SP = 00100 */ { MAXFRQ , { 0xF4 , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_BS_8_166[] =
{
   /* R = 10 */
   /* Fosc     - 180 MHz: CP/T = 0/111, SP = 01001 */ { 144000 , { 0xBC , 0x09 } },
   /* Fosc 180 - 197 MHz: CP/T = 1/110, SP = 01001 */ { 161000 , { 0xF4 , 0x09 } },
   /* Fosc 197 - 366 MHz: CP/T = 0/111, SP = 01010 */ { 330000 , { 0xBC , 0x0A } },
   /* Fosc 366 - 484 MHz: CP/T = 1/110, SP = 01010 */ { 448000 , { 0xF4 , 0x0A } },
   /* Fosc 484 - 646 MHz: CP/T = 0/111, SP = 01100 */ { 610000 , { 0xBC , 0x0C } },
   /* Fosc 646 - 790 MHz: CP/T = 1/110, SP = 01100 */ { 754000 , { 0xF4 , 0x0C } },
   /* Fosc 790 - 861 MHz: CP/T = 1/111, SP = 01100 */ { MAXFRQ , { 0xFC , 0x0C } }
};

static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_BS_7_625[] =
{
   /* R = 11 */
   /* Fosc     - 180 MHz: CP/T = 0/111, SP = 00001 */ { 144000 , { 0xBE , 0x01 } },
   /* Fosc 180 - 197 MHz: CP/T = 1/110, SP = 00001 */ { 161000 , { 0xF6 , 0x01 } },
   /* Fosc 197 - 366 MHz: CP/T = 0/111, SP = 00010 */ { 330000 , { 0xBE , 0x02 } },
   /* Fosc 366 - 484 MHz: CP/T = 1/110, SP = 00010 */ { 448000 , { 0xF6 , 0x02 } },
   /* Fosc 484 - 646 MHz: CP/T = 0/111, SP = 00100 */ { 610000 , { 0xBE , 0x04 } },
   /* Fosc 646 - 790 MHz: CP/T = 1/110, SP = 00100 */ { 754000 , { 0xF6 , 0x04 } },
   /* Fosc 790 - 861 MHz: CP/T = 1/111, SP = 00100 */ { MAXFRQ , { 0xFE , 0x04 } }
};

static TUNERCANNEDControlUnit_t TCU_XuguangDTT8X1C_BS_8_625[] =
{
   /* R = 11 */
   /* Fosc     - 180 MHz: CP/T = 0/111, SP = 01001 */ { 144000 , { 0xBE , 0x09 } },
   /* Fosc 180 - 197 MHz: CP/T = 1/110, SP = 01001 */ { 161000 , { 0xF6 , 0x09 } },
   /* Fosc 197 - 366 MHz: CP/T = 0/111, SP = 01010 */ { 330000 , { 0xBE , 0x0A } },
   /* Fosc 366 - 484 MHz: CP/T = 1/110, SP = 01010 */ { 448000 , { 0xF6 , 0x0A } },
   /* Fosc 484 - 646 MHz: CP/T = 0/111, SP = 01100 */ { 610000 , { 0xBE , 0x0C } },
   /* Fosc 646 - 790 MHz: CP/T = 1/110, SP = 01100 */ { 754000 , { 0xF6 , 0x0C } },
   /* Fosc 790 - 861 MHz: CP/T = 1/111, SP = 01100 */ { MAXFRQ , { 0xFE , 0x0C } }
};



static TUNERCANNEDControlTable_t TCT_XuguangDTT8X1C[] =
{

   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0 |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext. AGC",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_XuguangDTT8X1C_AUX_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2 |
                                     TUNER_MODE_SUB3,
      /* description              */ "TOP 109dBuV, SCAN",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_XuguangDTT8X1C_AUX_SCAN_109
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB2 |
                                     TUNER_MODE_SUB3,
      /* description              */ "TOP 109dBuV, LOCK",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_XuguangDTT8X1C_AUX_LOCK_109
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB4 |
                                     TUNER_MODE_SUB5,
      /* description              */ "TOP 112dBuV, SCAN",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_XuguangDTT8X1C_AUX_SCAN_112
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB4 |
                                     TUNER_MODE_SUB5,
      /* description              */ "TOP 112dBuV, LOCK",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_XuguangDTT8X1C_AUX_LOCK_112
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCH |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB6 |
                                     TUNER_MODE_SUB7,
      /* description              */ "TOP 115dBuV, SCAN",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_XuguangDTT8X1C_AUX_SCAN_115
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_LOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB6 |
                                     TUNER_MODE_SUB7,
      /* description              */ "TOP 115dBuV, LOCK",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_XuguangDTT8X1C_AUX_LOCK_115
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB0 |
                                     TUNER_MODE_SUB2 |
                                     TUNER_MODE_SUB4 |
                                     TUNER_MODE_SUB6,
      /* description              */ "8MHz, 166.7KHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_XuguangDTT8X1C_BS_8_166
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB0 |
                                     TUNER_MODE_SUB2 |
                                     TUNER_MODE_SUB4 |
                                     TUNER_MODE_SUB6,
      /* description              */ "7MHz, 166.7KHz",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_XuguangDTT8X1C_BS_7_166
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_8MHZ |
                                     TUNER_MODE_SUB1 |
                                     TUNER_MODE_SUB3 |
                                     TUNER_MODE_SUB5 |
                                     TUNER_MODE_SUB7,
      /* description              */ "8MHz, 62.5KHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_XuguangDTT8X1C_BS_8_625
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_67MHZ |
                                     TUNER_MODE_SUB1 |
                                     TUNER_MODE_SUB3 |
                                     TUNER_MODE_SUB5 |
                                     TUNER_MODE_SUB7,
      /* description              */ "7MHz, 62.5KHz",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_XuguangDTT8X1C_BS_7_625
   }
};

static TUNERSubMode_t subModesXuguangDTT8X1C[] =
{
   "External RF-AGC, 166.7kHz", /* Submode 0 */
   "External RF-AGC,  62.5kHz", /* Submode 1 */
   "TOP 109dBuV,     166.7kHz", /* Submode 2 */
   "TOP 109dBuV,      62.5kHz", /* Submode 3 */
   "TOP 112dBuV,     166.7kHz", /* Submode 4 */
   "TOP 112dBuV,      62.5kHz", /* Submode 5 */
   "TOP 115dBuV,     166.7kHz", /* Submode 6 */
   "TOP 115dBuV,      62.5kHz"  /* Submode 7 */
};

static TUNERCANNEDData_t dataXuguangDTT8X1C =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_XuguangDTT8X1C),
   /* nrOfControlBytes  */          2,
};
#endif

#ifdef TUNER_XUGUANG_DTT_8X1C
static TUNERCommonAttr_t commonAttrXuguangDTT8X1C =
{
   /* name            */  "Xuguang DTT-8x1C",
   /* minFreqRF [kHz] */      49000,
   /* maxFreqRF [kHz] */     861000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesXuguangDTT8X1C),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};
static TUNERInstance_t tunerXuguangDTT8X1C =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrXuguangDTT8X1C,
   /* myExtAttr    */ &dataXuguangDTT8X1C,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif

#ifdef TUNER_XUGUANG_DTT_8X1C_EXT_IF
static TUNERCommonAttr_t commonAttrXuguangDTT8X1CExtIf =
{
   /* name            */  "Xuguang DTT-8x1C Ext. IF",
   /* minFreqRF [kHz] */      49000,
   /* maxFreqRF [kHz] */     861000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesXuguangDTT8X1C),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerXuguangDTT8X1CExtIf =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrXuguangDTT8X1CExtIf,
   /* myExtAttr    */ &dataXuguangDTT8X1C,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif

#ifdef TUNER_XUGUANG_DTT_8X1C_T1146
static TUNERCommonAttr_t commonAttrXuguangDTT8X1CT1146 =
{
   /* name            */  "Xuguang DTT-8x1C-T1146",
   /* minFreqRF [kHz] */      49000,
   /* maxFreqRF [kHz] */     861000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesXuguangDTT8X1C),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerXuguangDTT8X1CT1146 =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrXuguangDTT8X1CT1146,
   /* myExtAttr    */ &dataXuguangDTT8X1C,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};
#endif
/*----------------------------------------------------------------------------*/
/*----end-Xuguang-DTT-8X1C----------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Xuguang SDTT-6A tuner configuration                                        */
/*----------------------------------------------------------------------------*/
#ifdef  TUNER_ALL
#ifndef TUNER_XUGUANG_SDTT_6A
#define TUNER_XUGUANG_SDTT_6A
#endif
#endif

#ifdef  TUNER_XUGUANG_SDTT_6A
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER    tunerXuguangSDTT6A
#endif

#include "tunercanned.h"

static TUNERCANNEDControlUnit_t TCU_XuguangSDTT6A_CB1_167_EXT[] =
{
   /* APT = 111, RS = 000 */
   /* ATC = 0,  STBY = 0, T = 0001 */
   /* AISL = 0, P5 = 0 */
   /* FOSC  80MHz - 148MHz: CP = 00, B = 0001 */   { 112000, { 0xB8 , 0x01 , 0xC1 } },
   /* FOSC 148MHz - 196MHz: CP = 01, B = 0001 */   { 160000, { 0xB8 , 0x41 , 0xC1 } },
   /* FOSC 197MHz - 366MHz: CP = 00, B = 0010 */   { 330000, { 0xB8 , 0x42 , 0xC1 } },
   /* FOSC 366MHz - 484MHz: CP = 10, B = 0010 */   { 448000, { 0xB8 , 0x82 , 0xC1 } },
   /* FOSC 484MHz - 646MHz: CP = 01, B = 1000 */   { 610000, { 0xB8 , 0x48 , 0xC1 } },
   /* FOSC 646MHz - 790MHz: CP = 10, B = 1000 */   { 754000, { 0xB8 , 0x88 , 0xC1 } },
   /* FOSC 790MHz - 861MHz: CP = 11, B = 1000 */   { MAXFRQ, { 0xB8 , 0xC8 , 0xC1 } }
};

static TUNERCANNEDControlUnit_t TCU_XuguangSDTT6A_CB1_625_EXT[] =
{
   /* APT = 111, RS = 011 */
   /* ATC = 0,  STBY = 0, T = 0001 */
   /* AISL = 0, P5 = 0 */
   /* FOSC  80MHz - 148MHz: CP = 00, B = 0001 */   { 112000, { 0xBB , 0x01 , 0xC1 } },
   /* FOSC 148MHz - 196MHz: CP = 01, B = 0001 */   { 160000, { 0xBB , 0x41 , 0xC1 } },
   /* FOSC 197MHz - 366MHz: CP = 00, B = 0010 */   { 330000, { 0xBB , 0x42 , 0xC1 } },
   /* FOSC 366MHz - 484MHz: CP = 10, B = 0010 */   { 448000, { 0xBB , 0x82 , 0xC1 } },
   /* FOSC 484MHz - 646MHz: CP = 01, B = 1000 */   { 610000, { 0xBB , 0x48 , 0xC1 } },
   /* FOSC 646MHz - 790MHz: CP = 10, B = 1000 */   { 754000, { 0xBB , 0x88 , 0xC1 } },
   /* FOSC 790MHz - 861MHz: CP = 11, B = 1000 */   { MAXFRQ, { 0xBB , 0xC8 , 0xC1 } }
};


static TUNERCANNEDControlTable_t TCT_XuguangSDTT6A[] =
{
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB0,
      /* description              */ "Ext AGC",
      /* refFreqNumerator   [kHz] */   500,
      /* refFreqDenominator       */     3,
      /* outputFrequency    [kHz] */ 36167,
      /* controlUnits             */ TCU_XuguangSDTT6A_CB1_167_EXT
   },
   {
      /* modes                    */ TUNER_MODE_DIGITALANALOG |
                                     TUNER_MODE_SWITCHLOCK |
                                     TUNER_MODE_678MHZ |
                                     TUNER_MODE_SUB1,
      /* description              */ "Ext AGC",
      /* refFreqNumerator   [kHz] */   125,
      /* refFreqDenominator       */     2,
      /* outputFrequency    [kHz] */ 36125,
      /* controlUnits             */ TCU_XuguangSDTT6A_CB1_625_EXT
   }
};

static TUNERSubMode_t subModesXuguangSDTT6A[] =
{
   "External RF-AGC, 166  kHz",  /* Submode 0 */
   "External RF-AGC, 62.5 kHz"   /* Submode 1 */

};

static TUNERCANNEDData_t dataXuguangSDTT6A =
{
   /* lockMask          */       0x00,
   /* lockValue         */       0x00,
   /* controlTable      */ ARRAYREF(TCT_XuguangSDTT6A),
   /* nrOfControlBytes  */          3,
};

static TUNERCommonAttr_t commonAttrXuguangSDTT6A =
{
   /* name            */  "Xuguang SDTT-6A",
   /* minFreqRF [kHz] */      49000,
   /* maxFreqRF [kHz] */     861000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesXuguangSDTT6A),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36125,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerXuguangSDTT6A =
{
   /* I2C address  */ { TUNER_I2C_ADDR , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrXuguangSDTT6A,
   /* myExtAttr    */ &dataXuguangSDTT6A,
   /* myFunct      */ &TUNERCANNEDFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Xuguang-SDTT-6A-----------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* Dummy Tuner                                                                */
/*----------------------------------------------------------------------------*/

#ifdef  TUNER_ALL
#ifndef TUNER_DUMMY_TUNER
#define TUNER_DUMMY_TUNER
#endif
#endif

#ifdef  TUNER_DUMMY_TUNER
#ifndef PRE_CONF_TUNER
#define PRE_CONF_TUNER tunerDummyTuner
#endif

#include "tunerdummy.h"

static TUNERSubMode_t subModesDummyTuner[] =
{
   "No submodes",
   "No submodes",
   "No submodes"
};

static TUNERCommonAttr_t commonAttrDummyTuner =
{
   /* name            */  "Dummy Tuner 36MHz",
   /* minFreqRF [kHz] */      30000,
   /* maxFreqRF [kHz] */    1000000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesDummyTuner),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      36000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerDummyTuner =
{
   /* I2C address  */ { 0xFF , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrDummyTuner,
   /* myExtAttr    */ NULL,
   /* myFunct      */ &TUNERDUMMYFunctions_g
};

static TUNERCommonAttr_t commonAttrDummyTuner456 =
{
   /* name            */  "Dummy Tuner 4.56MHz",
   /* minFreqRF [kHz] */      30000,
   /* maxFreqRF [kHz] */    1000000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesDummyTuner),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */       4560,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerDummyTuner456 =
{
   /* I2C address  */ { 0xFF , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrDummyTuner456,
   /* myExtAttr    */ NULL,
   /* myFunct      */ &TUNERDUMMYFunctions_g
};

static TUNERCommonAttr_t commonAttrDummyTuner4375 =
{
   /* name            */  "Dummy Tuner 43.75MHz",
   /* minFreqRF [kHz] */      30000,
   /* maxFreqRF [kHz] */    1000000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesDummyTuner),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */      43750,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerDummyTuner4375 =
{
   /* I2C address  */ { 0xFF , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrDummyTuner4375,
   /* myExtAttr    */ NULL,
   /* myFunct      */ &TUNERDUMMYFunctions_g
};

static TUNERCommonAttr_t commonAttrDummyTuner8 =
{
   /* name            */  "Dummy Tuner 8MHz",
   /* minFreqRF [kHz] */      30000,
   /* maxFreqRF [kHz] */    1000000,
   /* sub-mode        */          0,
   /* sub-modes       */  ARRAYREF(subModesDummyTuner),
   /* self check      */       NULL,
   /* programmed      */      FALSE,
   /* RFfrequency     */          0,
   /* IFfrequency     */       8000,
   /* myUserData      */       NULL,
   /* myCapabilities  */          0
};

static TUNERInstance_t tunerDummyTuner8 =
{
   /* I2C address  */ { 0xFF , TUNER_DEV_ID },
   /* myCommonAttr */ &commonAttrDummyTuner8,
   /* myExtAttr    */ NULL,
   /* myFunct      */ &TUNERDUMMYFunctions_g
};

#endif
/*----------------------------------------------------------------------------*/
/*----end-Dumy-Tuner----------------------------------------------------------*/



#ifdef TUNER_ALL
static pTUNERInstance_t drxkTunerList[] =
{
   &tunerAlpsTDAGx,
   &tunerAlpsTDAG4_NIM,
   &tunerAlpsTDQG4X003A_K,
   &tunerChinaDragonCDTSFT33840,
   &tunerEardaEDT3021II2A,
   &tunerEardaEHU100x1,
   &tunerEntropicEN4020,
   &tunerGoldenDragonDTT5K6_K,
   &tunerJinxinUVS1X05DCW1_K,
   &tunerLGInnotekTDFW_G235D_K,
   &tunerLGInnotekTDFW_G235D2_K,
   &tunerLGInnotekTDTC_G2xxD_K,
   &tunerLGInnotekTDTC_G323D,
   &tunerLGInnotekTDTC_G3xxD_K,
   &tunerLGInnotekTDTC_G4xxD_K,
   &tunerLGInnotekTDTC_G675D_K,
   &tunerLoeweFET08_tuner1,
   &tunerLoeweFET08_tuner2,
   &tunerMaximMax3542,
   &tunerMaximMax3543,
   &tunerMaxLinearMXL301RF,
   &tunerMaxLinearMXL5007,
   &tunerMicLGInnotekTDFW_G235D_K,
   &tunerMicPanasonicENG37F01KF,
   &tunerMicrotuneMT2063_K,
   &tunerMicrotuneMT2063Taiwan,
   &tunerMicrotuneMT2131_K,
   &tunerNuTuneNH3660,
   &tunerNuTuneHD1856AF,
   &tunerNuTuneTH2603,
   &tunerNXPHD1816AFBHXP_K,
   &tunerNXPTD1116AL_K,
   &tunerNXPTD1716F3_K,
   &tunerNXPTD1716F3_Balanced_K,
   &tunerNXPTD1716F4_K,
   &tunerNXPTDA18271_K,
   &tunerNXPTDA18272_K,
   &tunerPanasonicENG37ExxKF_K,
   &tunerPanasonicENG37F01KF,
   &tunerPanasonicENV57M06D8F,
   &tunerPanasonicET35DRV,
   &tunerSANYO_115U7A20C,
   &tunerSSEM_DNOS40AZH201J,
   &tunerSSEM_DTOS403LH121B,
   &tunerSSEM_DTOS403SH081A,
   &tunerSSEM_DTOS403PH17AS_K,
   &tunerSSEM_DTOS40FPL111A_K,
   &tunerSSEM_DTVS22CVL161A_INT,
   &tunerSSEM_DTVS22CVL161A_EXT,
   &tunerSharpVA1E1ED2403,
   &tunerSharpVA1E9ED2001,
   &tunerSharpVA1Y9ED200X,
   &tunerSonySutRa111,
   &tunerTCLDT67WI3RE,
   &tunerThomsonDTT71303A,
   &tunerThomsonDTT71303_balanced,
   &tunerThomsonDTT71306_balanced,
   &tunerThomsonDTT71306_unbalanced,
   &tunerThomsonDTT7130x_internal,
   &tunerThomsonDTT7540x_K,
   &tunerThomsonDTT7543x_K,
   &tunerThomsonDTT7544x_K,
   &tunerThomsonDTT7546x_K,
   &tunerThomsonDTT768xx,
   &tunerThomsonDTT768xx_int,
   &tunerXuguangDTT8X1C,
   &tunerXuguangDTT8X1CExtIf,
   &tunerXuguangDTT8X1CT1146,
   &tunerXuguangSDTT6A,
   &tunerDummyTuner,
   &tunerDummyTuner456,
   &tunerDummyTuner4375,
   &tunerDummyTuner8,
   NULL /* baseband */
};
#endif

/*------------------------------------------------------------------------------
THE END
------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif   /* __DRXK_TUNER_TABLES_H__ */

/* End of file */

