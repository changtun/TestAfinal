/**
* \file $Id: drxk.c,v 8.376 2010/04/01 07:31:53 lfeng Exp $
*
* \brief DRXK specific implementation of DRX driver
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

/*============================================================================*/
/*=== INCLUDE FILES ==========================================================*/
/*============================================================================*/

#include "./include/drxk.h"                /* device specific API  */
#include "./include/drx_dap_fasi.h"        /* data access protocol */
#include "./include/drxk_map.h"            /* register map         */
#include "./include/drx_driver_version.h"
#include "pbitrace.h"

/* Optional configuration file */
#ifdef DRXK_OPTIONS_H
#include "drxk_options.h"
#endif

/* If DRXKDRIVER_DEBUG is defined development debugging messages will be
   enabled */
#ifdef DRXKDRIVER_DEBUG
#include <stdio.h>
#endif

/*============================================================================*/
/*=== DEFINES ================================================================*/
/*============================================================================*/

/**
* \brief Maximum u16_t value.
*/
#ifndef MAX_U16
#define MAX_U16  ((u16_t) (0xFFFF))
#endif

/**
* \brief Maximum u32_t value.
*/
#ifndef MAX_U32
#define MAX_U32  ((u32_t) (0xFFFFFFFFL))
#endif

/**
* \brief Defines the drive current on MD0 pin for serial MPEG output.
*
*        Unit:   [10mA].
*        Range:  0...7.
*/
#ifndef DRXK_MPEG_SERIAL_OUTPUT_PIN_DRIVE_STRENGTH
#define DRXK_MPEG_SERIAL_OUTPUT_PIN_DRIVE_STRENGTH (0x02)
#endif

/**
* \brief Defines the drive current on MD0..MD7 pins for parallel MPEG output.
*
*        Unit:   [10mA].
*        Range:  0...7.
*/
#ifndef DRXK_MPEG_PARALLEL_OUTPUT_PIN_DRIVE_STRENGTH
#define DRXK_MPEG_PARALLEL_OUTPUT_PIN_DRIVE_STRENGTH (0x02)
#endif

/**
* \brief Defines the drive current on MCLK pin for MPEG output.
*
*        Unit:   [10mA].
*        Range:  0...7.
*/
#ifndef DRXK_MPEG_OUTPUT_CLK_DRIVE_STRENGTH
#define DRXK_MPEG_OUTPUT_CLK_DRIVE_STRENGTH (0x02)
#endif

/**
* \brief Timeout value for waiting on demod lock during channel scan (millisec).
*/
#ifndef DRXK_SCAN_TIMEOUT
#define DRXK_SCAN_TIMEOUT    (3000)
#endif

/**
* \brief Pointer to default DAP for a DRXK demodulator instance.
*/
#ifndef DRXK_DAP
#define DRXK_DAP (drxDapDRXKFunct_g)
#endif

/**
* \brief Wakeup key for device.
*
*/
#ifndef DRXK_WAKE_UP_KEY
#define DRXK_WAKE_UP_KEY (demod -> myI2CDevAddr -> i2cAddr)
#endif

/**
* \brief Maximum retries before reporting error during wakeup/powerup
*
*/
#ifndef DRXK_MAX_RETRIES_POWERUP
#define DRXK_MAX_RETRIES_POWERUP (10)
#endif


/**
* \brief Default I2C addres of a demodulator instance.
*/
#ifndef DRXK_DEF_I2C_ADDR
#define DRXK_DEF_I2C_ADDR (0x52)
#endif

/**
* \brief Default device identifier of a demodultor instance.
*/
#ifndef DRXK_DEF_DEMOD_DEV_ID
#define DRXK_DEF_DEMOD_DEV_ID      (1)
#endif

/**
* \brief HI timing delay for I2C timing (in nano seconds)
*/
#ifndef DRXK_HI_I2C_DELAY
#define DRXK_HI_I2C_DELAY    (42)
#endif

/**
* \brief HI timing delay for I2C timing (in nano seconds)
*/
#ifndef DRXK_HI_I2C_BRIDGE_DELAY
#define DRXK_HI_I2C_BRIDGE_DELAY   (350)
#endif

/**
* \brief HI timing delay for I2C timing (in nano seconds)
*/
#ifndef DRXK_HI_I2C_TRANSMIT_TIMEOUT
#define DRXK_HI_I2C_TRANSMIT_TIMEOUT   (0x96ff)
#endif

/**
* \brief Maximum number of retries for ADC compensation re-runs
*/
#ifndef DRXK_ADC_MAX_RETRIES
#define DRXK_ADC_MAX_RETRIES (3)
#endif


/**
* \brief Value of SCU_RAM_COMMAND when SCU is ready for a new command
*/
#ifndef SCU_RAM_COMMAND_CMD_READY
#define SCU_RAM_COMMAND_CMD_READY (SCU_RAM_COMMAND__PRE)
#endif

/**
* \brief Timeout value for SCU command completion (milliseconds)
*
* Must be at least 15 for ADC compensation algorithm.
*/
#ifndef DRXK_SCU_CMD_TIMEOUT
#define DRXK_SCU_CMD_TIMEOUT (100)
#endif

/**
* \brief Timeout value for HI command completion (milliseconds)
*
* Must be at least 15 for ADC compensation algorithm.
*/
#ifndef DRXK_HI_CMD_TIMEOUT
#define DRXK_HI_CMD_TIMEOUT (100)
#endif

/**
* \brief Timeout value for HI command completion (milliseconds)
*
* Must be at least 15 for ADC compensation algorithm.
*/
#ifndef DRXK_OFDM_CMD_TIMEOUT
#define DRXK_OFDM_CMD_TIMEOUT (100)
#endif

/**
* \brief Timeout value for shutdown OFDM token ring (milliseconds)
*
*/
#ifndef DRXK_OFDM_TR_SHUTDOWN_TIMEOUT
#define DRXK_OFDM_TR_SHUTDOWN_TIMEOUT (5)
#endif

/**
* \brief Defines for bandwidths in Hz (8Mhz).
*
* 8 MHz -> (64/7)*(8/8)*1000000 Hz
*/
#ifndef DRXK_BANDWIDTH_8MHZ_IN_HZ
#define DRXK_BANDWIDTH_8MHZ_IN_HZ  (0x8B8249L)
#endif

/**
* \brief Defines for bandwidths in Hz (7Mhz).
*
* 7 MHz -> (64/7)*(7/8)*1000000 Hz
*/
#ifndef DRXK_BANDWIDTH_7MHZ_IN_HZ
#define DRXK_BANDWIDTH_7MHZ_IN_HZ  (0x7A1200L)
#endif

/**
* \brief Defines for bandwidths in Hz (6Mhz).
*
* 6 MHz -> (64/7)*(6/8)*1000000 Hz
*/
#ifndef DRXK_BANDWIDTH_6MHZ_IN_HZ
#define DRXK_BANDWIDTH_6MHZ_IN_HZ  (0x68A1B6L)
#endif

/**
* \brief Needed for calculation of prescale feature in AUD
*/
#ifndef DRX_AUD_MAX_FM_DEVIATION
#define DRX_AUD_MAX_FM_DEVIATION  (100)     /* kHz */
#endif

/**
* \brief Needed for calculation of NICAM prescale feature in AUD
*/
#ifndef DRX_AUD_MAX_NICAM_PRESCALE
#define DRX_AUD_MAX_NICAM_PRESCALE  (9)     /* dB */
#endif

/**
* \brief configuration of Ki factors
*/
#ifndef DRXK_KI_RAGC_ATV
#define DRXK_KI_RAGC_ATV   4
#endif
#ifndef DRXK_KI_IAGC_ATV
#define DRXK_KI_IAGC_ATV   6
#endif
#ifndef DRXK_KI_DAGC_ATV
#define DRXK_KI_DAGC_ATV   7
#endif

#ifndef DRXK_KI_RAGC_QAM
#define DRXK_KI_RAGC_QAM   3
#endif
#ifndef DRXK_KI_IAGC_QAM
#define DRXK_KI_IAGC_QAM   4
#endif
#ifndef DRXK_KI_DAGC_QAM
#define DRXK_KI_DAGC_QAM   7
#endif

/**
* \brief Non-active range of the AGC DAC
*/
#ifndef DRXK_AGC_DAC_OFFSET
#define DRXK_AGC_DAC_OFFSET (0x800)
#endif

/**
* \brief Bit rate and segment rate constants used for SER and BER.
*/
#ifndef DRXK_QAM_SL_SIG_POWER_QAM_UNKNOWN
#define DRXK_QAM_SL_SIG_POWER_QAM_UNKNOWN     (0)
#endif

/**
* \brief Bit rate and segment rate constants used for SER and BER.
*/
#ifndef DRXK_QAM_SL_SIG_POWER_QPSK
#define DRXK_QAM_SL_SIG_POWER_QPSK        (32768)
#endif

/**
* \brief Bit rate and segment rate constants used for SER and BER.
*/
#ifndef DRXK_QAM_SL_SIG_POWER_QAM8
#define DRXK_QAM_SL_SIG_POWER_QAM8        (24576)
#endif

/**
* \brief Bit rate and segment rate constants used for SER and BER.
*/
#ifndef DRXK_QAM_SL_SIG_POWER_QAM16
#define DRXK_QAM_SL_SIG_POWER_QAM16       (40960)
#endif

/**
* \brief Bit rate and segment rate constants used for SER and BER.
*/
#ifndef DRXK_QAM_SL_SIG_POWER_QAM32
#define DRXK_QAM_SL_SIG_POWER_QAM32       (20480)
#endif

/**
* \brief Bit rate and segment rate constants used for SER and BER.
*/
#ifndef DRXK_QAM_SL_SIG_POWER_QAM64
#define DRXK_QAM_SL_SIG_POWER_QAM64       (43008)
#endif

/**
* \brief Bit rate and segment rate constants used for SER and BER.
*/
#ifndef DRXK_QAM_SL_SIG_POWER_QAM128
#define DRXK_QAM_SL_SIG_POWER_QAM128      (20992)
#endif

/**
* \brief Bit rate and segment rate constants used for SER and BER.
*/
#ifndef DRXK_QAM_SL_SIG_POWER_QAM256
#define DRXK_QAM_SL_SIG_POWER_QAM256      (43520)
#endif

/**
* \brief Min supported symbolrates.
*/
#ifndef DRXK_QAM_SYMBOLRATE_MIN
#define DRXK_QAM_SYMBOLRATE_MIN          (520000)
#endif

/**
* \brief Max supported symbolrates.
*/
#ifndef DRXK_QAM_SYMBOLRATE_MAX
#define DRXK_QAM_SYMBOLRATE_MAX         (7233000)
#endif

/**
* \brief channel filter coefficients ROM offsets for BootLoader.
*/
#define DRXK_BL_ROM_OFFSET_TAPS_DVBT    56
#define DRXK_BL_ROM_OFFSET_TAPS_ITU_A   64
#define DRXK_BL_ROM_OFFSET_TAPS_ITU_C   0x5FE0
#define DRXK_BL_ROM_OFFSET_TAPS_BG      24
#define DRXK_BL_ROM_OFFSET_TAPS_DKILLP  32
#define DRXK_BL_ROM_OFFSET_TAPS_NTSC    40
#define DRXK_BL_ROM_OFFSET_TAPS_FM      48

/**
* \brief ucode ROM offset for BootLoader.
*/
#define DRXK_BL_ROM_OFFSET_UCODE        0

/**
* \brief BootLoader Command (BLC) definitions.
*/
/* command timeout */
#define DRXK_BLC_TIMEOUT                50

/* chain command */
#define DRXK_BLCC_NR_ELEMENTS_TAPS      2
#define DRXK_BLCC_NR_ELEMENTS_UCODE     6

/* direct command */
#define DRXK_BLDC_NR_ELEMENTS_TAPS      28



/**
* \brief Temporary register master corrections.
*        (not correct in register master therefore adapted locally)
*/
#define DRXK_ATV_TOP_VID_AMP_BG_ADAPTED 0x334
#define DRXK_ATV_TOP_VID_AMP_DK_ADAPTED 0x346
#define DRXK_ATV_TOP_VID_AMP_I_ADAPTED  0x384

/*============================================================================*/
/**
* \brief Defines required for audio
*/
#define AUD_VOLUME_ZERO_DB                      115
#define AUD_VOLUME_DB_MIN                       -60
#define AUD_VOLUME_DB_MAX                       12
#define AUD_CARRIER_STRENGTH_QP_0DB             0x4000
#define AUD_CARRIER_STRENGTH_QP_0DB_LOG10T100   421
#define AUD_MAX_AVC_REF_LEVEL                   15
#define AUD_I2S_FREQUENCY_MAX                   48000UL
#define AUD_I2S_FREQUENCY_MIN                   12000UL
#define AUD_RDS_ARRAY_SIZE                      18

/*============================================================================*/
/* Values for I2S Master/Slave pin configurations */
#define SIO_PDR_I2S_CL_CFG_MODE__MASTER      0x0004
#define SIO_PDR_I2S_CL_CFG_DRIVE__MASTER     0x0008
#define SIO_PDR_I2S_CL_CFG_MODE__SLAVE       0x0004
#define SIO_PDR_I2S_CL_CFG_DRIVE__SLAVE      0x0000

#define SIO_PDR_I2S_DA_CFG_MODE__MASTER      0x0003
#define SIO_PDR_I2S_DA_CFG_DRIVE__MASTER     0x0008
#define SIO_PDR_I2S_DA_CFG_MODE__SLAVE       0x0003
#define SIO_PDR_I2S_DA_CFG_DRIVE__SLAVE      0x0008

#define SIO_PDR_I2S_WS_CFG_MODE__MASTER      0x0004
#define SIO_PDR_I2S_WS_CFG_DRIVE__MASTER     0x0008
#define SIO_PDR_I2S_WS_CFG_MODE__SLAVE       0x0004
#define SIO_PDR_I2S_WS_CFG_DRIVE__SLAVE      0x0000


/*============================================================================*/
/*=== REGISTER ACCESS MACROS =================================================*/
/*============================================================================*/
#undef DRXKDRIVER_DEBUG
#define PRINT_MSG

#ifdef DRXKDRIVER_DEBUG
#include <stdio.h>
/**
* \brief Macro that will print a message and jump upon an error
*/

#define CHK_ERROR( s ) \
        do{ \
            if ( (s) != DRX_STS_OK ) \
            { \
               fprintf( stderr, \
                        "ERROR[\n file        : %s \n function    : %s\n line        : %d\n]\n", \
                        __FILE__,__FUNCTION__,__LINE__); \
               goto rw_error; }; \
            } \
        while (0 != 0)
#else
/**
* \brief Macro that will jump upon an error
*/
/* BEGIN: Modified by zhwu, 2013/3/6 */
#ifdef PRINT_MSG
#include "pbitrace.h"
#define CHK_ERROR( s ) \
        do{ \
            if ( (s) != DRX_STS_OK ) \
            { \
               pbierror(  \
                        "ERROR[\n file        : %s \n function    : %s\n line        : %d\n]\n", \
                        __FILE__,__FUNCTION__,__LINE__); \
               goto rw_error; }; \
            } \
        while (0 != 0)
#else
#define CHK_ERROR( s ) \
   do{ \
      if ( (s) != DRX_STS_OK ) { goto rw_error; } \
   } while (0 != 0)
#endif /* PRINT_MSG */
/* END:   Modified by zhwu, 2013/3/6 */
#endif

/**
* \brief Write 16 bits register
*/
#define WR16( dev, addr, val) \
   CHK_ERROR( DRXK_DAP.writeReg16Func( (dev), (addr), (val), 0 ) )

/**
* \brief Read 16 bits register
*/
#define RR16( dev, addr, val) \
   CHK_ERROR( DRXK_DAP.readReg16Func( (dev), (addr), (val), 0 ) )

/**
* \brief Write 32 bits register
*/
#define WR32( dev, addr, val) \
   CHK_ERROR( DRXK_DAP.writeReg32Func( (dev), (addr), (val), 0 ) )

/**
* \brief Read 32 bits register
*/
#define RR32( dev, addr, val) \
   CHK_ERROR( DRXK_DAP.readReg32Func( (dev), (addr), (val), 0 ) )

/**
* \brief Write block of registers
*/
#define WRB( dev, addr, len, block ) \
   CHK_ERROR( DRXK_DAP.writeBlockFunc( (dev), (addr), (len), (block), 0 ) )

/**
* \brief Read block of registers
*/
#define RRB( dev, addr, len, block ) \
   CHK_ERROR( DRXK_DAP.readBlockFunc( (dev), (addr), (len), (block), 0 ) )

/**
* \brief Inverse broadcast write
*/
#define BCWR16( dev, addr, val ) \
   CHK_ERROR( DRXK_DAP.writeReg16Func( (dev), (addr), (val), \
                                                       DRXDAP_FASI_BROADCAST ) )

/**
* \brief Atomicaly read 32 bits register
*/
#define ARR32( dev, addr, val) \
   CHK_ERROR( DRXK_DAP_AtomicReadReg32( (dev), (addr), (val), 0 ) )

/**
* \brief Atomicaly read 16 bits SCU register
*/
#define SARR16( dev, addr, val) \
   CHK_ERROR( DRXK_DAP_SCU_AtomicReadReg16( (dev), (addr), (val), 0 ) )

/**
* \brief Atomicaly write 16 bits SCU register
*/
#define SAWR16( dev, addr, val) \
   CHK_ERROR( DRXK_DAP_SCU_AtomicWriteReg16( (dev), (addr), (val), 0 ) )

#if 0  /* not been used. avoid warning */
/**
* \brief Atomicaly read 32 bits register
*/
#define AWR32( dev, addr, val) \
   CHK_ERROR( DRXK_DAP_AtomicWriteReg32( (dev), (addr), (val), 0 ) )
#endif

/*============================================================================*/
/*=== STANDARD RELATED MACROS ================================================*/
/*============================================================================*/

/**
* \brief Macro to check if std is an ATV standard
* \retval TRUE std is an ATV standard
* \retval FALSE std is not an ATV standard
*/

#if 0
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

#endif

/**
* \brief Macro to check if mode is a power-up mode
* \retval TRUE mode is a power-up mode
* \retval FALSE mode is not a power-up mode
*/
#define DRXK_ISPOWERUPMODE( mode )  (  ( mode == DRX_POWER_UP ) || \
                                       ( mode == DRXK_POWER_DOWN_OFDM ) )

/*============================================================================*/
/*=== MISC MACROS ============================================================*/
/*============================================================================*/

/**
* \brief Macro to convert 16 bits words into 2 byte array elements
*
* This macro is used to create byte arrays for block writes.
* Block writes speed up I2C traffic between host and demod.
* The macro takes care of the required byte order in a 16 bits word.
* x -> lowbyte(x), highbyte(x)
*/
#define DRXK_16TO8( x ) ((u8_t) (((u16_t)x)    &0xFF)), \
                        ((u8_t)((((u16_t)x)>>8)&0xFF))

/**
* \brief Macro to detect DRXK A1 with PATCH ucode
* \param Pointer to demod instance
*/
#define DRXK_A1_PATCH_CODE( demod ) (IsA1WithPatchCode( (demod) ))

/**
* \brief Macro to detect DRXK A2 with PATCH ucode
* \param Pointer to demod instance
*/
#define DRXK_A2_PATCH_CODE( demod ) (IsA2WithPatchCode( (demod) ))

/**
* \brief Macro to detect DRXK A3 with ROM ucode
* \param Pointer to demod instance
*/
#define DRXK_A3_ROM_CODE( demod ) (IsA3WithRomCode( (demod) ))
/**
* \brief Macro to detect DRXK A3 with PATCH ucode
* \param Pointer to demod instance
*/
#if 0
#define DRXK_A3_PATCH_CODE( demod ) (IsA3WithPatchCode( (demod) ))
#endif

/*============================================================================*/
/*=== FUNCTION PROTOTYPES ====================================================*/
/*============================================================================*/
/*
 * Toplevel function prototypes, needed for default demod instance ...
 */

DRXStatus_t DRXK_Open  (  pDRXDemodInstance_t  demod );
DRXStatus_t DRXK_Close (  pDRXDemodInstance_t  demod);
DRXStatus_t DRXK_Ctrl  (  pDRXDemodInstance_t  demod,
                          DRXCtrlIndex_t       ctrl,
                          void                 *ctrlData);

/*
 * DRXK DAP function prototypes, needed for default demod instance ...
 */

static DRXStatus_t DRXK_DAP_ReadBlock (
   pI2CDeviceAddr_t devAddr,
   DRXaddr_t        addr,
   u16_t            datasize,
   pu8_t            data,
   DRXflags_t       flags);

static DRXStatus_t DRXK_DAP_ReadModifyWriteReg8 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        waddr,
    DRXaddr_t        raddr,
    u8_t             wdata,
    pu8_t            rdata);

static DRXStatus_t DRXK_DAP_ReadModifyWriteReg16 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        waddr,
    DRXaddr_t        raddr,
    u16_t            wdata,
    pu16_t           rdata);

static DRXStatus_t DRXK_DAP_ReadModifyWriteReg32 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        waddr,
    DRXaddr_t        raddr,
    u32_t            wdata,
    pu32_t           rdata);

static DRXStatus_t DRXK_DAP_ReadReg8 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    pu8_t            data,
    DRXflags_t       flags);

static DRXStatus_t DRXK_DAP_ReadReg16 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    pu16_t           data,
    DRXflags_t       flags);

static DRXStatus_t DRXK_DAP_ReadReg32 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    pu32_t           data,
    DRXflags_t       flags);

static DRXStatus_t DRXK_DAP_WriteBlock (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u16_t            datasize,
    pu8_t            data,
    DRXflags_t       flags);

static DRXStatus_t DRXK_DAP_WriteReg8 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u8_t             data,
    DRXflags_t       flags);

static DRXStatus_t DRXK_DAP_WriteReg16 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u16_t            data,
    DRXflags_t       flags);

static DRXStatus_t DRXK_DAP_WriteReg32 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u32_t            data,
    DRXflags_t       flags);

/*
 * Misc function prototypes, needed to avoid re-ordering function declarations
 */
static DRXStatus_t
CtrlPowerMode( pDRXDemodInstance_t demod,
               pDRXPowerMode_t     mode );

static Bool_t
IsA1WithPatchCode( pDRXDemodInstance_t demod );

static Bool_t
IsA2WithPatchCode( pDRXDemodInstance_t demod );

static DRXStatus_t
ATVCtrlSetCfgAtvMode(   pDRXDemodInstance_t demod,
                        DRXKAtvMode_t       atvMode );

static DRXStatus_t
CtrlStopStandard( pDRXDemodInstance_t demod );

static DRXStatus_t
CtrlSetChannel(   pDRXDemodInstance_t   demod,
                  pDRXChannel_t         channel );

static DRXStatus_t
CtrlGetChannel(   pDRXDemodInstance_t   demod,
                  pDRXChannel_t         channel );

static DRXStatus_t
CtrlLockStatus(   pDRXDemodInstance_t demod,
                  pDRXLockStatus_t    lockStat );

static DRXStatus_t
AUDCtrlGetCarrierDetectStatus (  pDRXDemodInstance_t  demod,
                                 pDRXAudStatus_t      status );
static DRXStatus_t
AUDCtrlSetStandard ( pDRXDemodInstance_t  demod,
                     pDRXAudStandard_t    standard );

static DRXStatus_t
CtrlGetStandard(  pDRXDemodInstance_t demod,
                  pDRXStandard_t      standard );

static DRXStatus_t
BL_ReadRomVersion (pDRXDemodInstance_t demod, pu32_t version);


/*============================================================================*/
/*=== GLOBAL VARIABLES =======================================================*/
/*============================================================================*/

/**
* \brief Default DAP module name (part of DAP version structure).
*/
char drxDapDRXKModuleName_g[]  = "DRXK Data Access Protocol";

/**
* \brief Default DAP human readable version (part of DAP version structure).
*/
char drxDapDRXKVersionText_g[] = "0.0.0";

/**
* \brief Default DAP version structure.
*/
DRXVersion_t drxDapDRXKVersion_g = {
  DRX_MODULE_DAP,             /**< type identifier of the module */
  drxDapDRXKModuleName_g,       /**< name or description of module */

  0,                          /**< major version number */
  0,                          /**< minor version number */
  0,                          /**< patch version number */
  drxDapDRXKVersionText_g       /**< version as text string */
};

/**
* \brief Default DAP structure.
*/
DRXAccessFunc_t drxDapDRXKFunct_g = {
   &drxDapDRXKVersion_g,
   DRXK_DAP_WriteBlock,                  /* Supported */
   DRXK_DAP_ReadBlock,                   /* Supported */
   DRXK_DAP_WriteReg8,                   /* Not supported */
   DRXK_DAP_ReadReg8,                    /* Not supported */
   DRXK_DAP_ReadModifyWriteReg8,         /* Not supported */
   DRXK_DAP_WriteReg16,                  /* Supported */
   DRXK_DAP_ReadReg16,                   /* Supported */
   DRXK_DAP_ReadModifyWriteReg16,        /* Supported */
   DRXK_DAP_WriteReg32,                  /* Supported */
   DRXK_DAP_ReadReg32,                   /* Supported */
   DRXK_DAP_ReadModifyWriteReg32,        /* Not supported */
};

/**
* \brief Default I2C address and device identifier.
*/
I2CDeviceAddr_t DRXKDefaultAddr_g = {
      DRXK_DEF_I2C_ADDR,     /* i2c address */
      DRXK_DEF_DEMOD_DEV_ID  /* device id */
};

/**
* \brief The driver methods of the DRXK
*/
DRXDemodFunc_t DRXKFunctions_g  =
{
   DRX3923K_TYPE_ID,
   DRXK_Open,
   DRXK_Close,
   DRXK_Ctrl
};



/**
* \brief Default extended attributes for a DRK demodulator instance.
*
* This structure is DRXK specific.
*
*/
DRXKData_t DRXKData_g =
{
   DRX_STANDARD_UNKNOWN,  /* standard*/
   DRX_SUBSTANDARD_MAIN,  /* substandard */
   FALSE,                 /* standardActive */
   FALSE,                 /* channelActive */
   {  /* currentChannel */
      (DRXFrequency_t) 0,         /* frequency      */
      DRX_BANDWIDTH_UNKNOWN,      /* bandwidth      */
      DRX_MIRROR_UNKNOWN,         /* mirror         */
      DRX_CONSTELLATION_UNKNOWN,  /* constellation  */
      DRX_HIERARCHY_UNKNOWN,      /* hierarchy      */
      DRX_PRIORITY_UNKNOWN,       /* priority       */
      DRX_CODERATE_UNKNOWN,       /* coderate       */
      DRX_GUARD_UNKNOWN,          /* guard          */
      DRX_FFTMODE_UNKNOWN,        /* fftmode        */
      DRX_CLASSIFICATION_UNKNOWN, /* classification */
      (u32_t) 0                   /* symbolrate     */
   },
   (u32_t) 0, /* sysClkInHz */

   /* UIO configuration */
   DRX_UIO_MODE_DISABLE,  /* uioSawSwMode */
   DRX_UIO_MODE_FIRMWARE, /* uioGPIO2Mode */
   DRX_UIO_MODE_DISABLE,  /* uioGPIO1Mode */
   /* HI cfg */
   (u16_t) 0,
   (u16_t) 0,
   (u16_t) 0,
   (u16_t) 0,
   (u16_t) 0,
   /* Capabilities */
   FALSE, /* hasLNA */
   FALSE, /* hasOOB */
   FALSE, /* hasATV */
   FALSE, /* hasAudio */
   FALSE, /* hasDVBT */
   FALSE, /* hasDVBC */
   FALSE, /* hasSAWSW */
   FALSE, /* hasGPIO2 */
   FALSE, /* hasGPIO1 */
   FALSE, /* hasIRQN */
   /* mfx */
   (u8_t) 0,  /* mfx */
   /* AUD information */
   0UL,                    /* iqmFsRateOfs      */
   0UL,                    /* iqmRcRateOfs      */
   FALSE,                  /* posImage          */
   /* ATV configuartion */
   0UL,                    /* flags cfg changes */
   DRXK_ATV_IMBALANCE_MODE_A, /* current imbalance ctrl mode */
   FALSE,                     /* flag: TRUE=bypass */
   FALSE,                     /* flag CVBS ouput enable */
   FALSE,                     /* flag SIF ouput enable */
   DRXK_SIF_ATTENUATION_0DB,  /* current SIF att setting */
   200,                       /* atvSVRThreshold [0.1dB] */
   TRUE,   /* useBootloader */
   /* Version information   */
   {
      "01234567890", /* human readable version microcode */
      "01234567890"  /* human readable version device specific code */
   },
   {
      {              /* DRXVersion_t for microcode */
         DRX_MODULE_UNKNOWN,
         (char*)(NULL),
         0,
         0,
         0,
         (char*)(NULL)
      },
      {             /* DRXVersion_t for device specific code */
         DRX_MODULE_UNKNOWN,
         (char*)(NULL),
         0,
         0,
         0,
         (char*)(NULL)
      }
   },
   {
      {             /* DRXVersionList_t for microcode */
         (pDRXVersion_t)(NULL),
         (pDRXVersionList_t)(NULL)
      },
      {            /* DRXVersionList_t for device specific code */
         (pDRXVersion_t)(NULL),
         (pDRXVersionList_t)(NULL)
      }
   },
   /* QAM specific fields */
   0,                         /* qamChannelBwOffset     */
#ifdef COMPILE_FOR_NONRT
   100,                       /* qamBERFrameLength [ms] */
#else
   500,                       /* qamBERFrameLength [ms] */
#endif
   DRX_INTERLEAVEMODE_I12_J17,/* qamInterleaveMode      */
   FALSE,                     /* qamChannelIsSet        */
   FALSE,                     /* qamScanForQam128       */
   DRX_CONSTELLATION_QAM256,  /* qamScanConstellation   */
   DRX_QAM_LOCKRANGE_EXTENDED,/* qamLockRange           */

   FALSE,  /* ucodeUploaded   */
   FALSE,  /* pdrSafeMode     */
   SIO_PDR_GPIO_CFG__PRE,   /* pdrSafeRestoreValGpio  */
   SIO_PDR_VSYNC_CFG__PRE,  /* pdrSafeRestoreValVSync */
   SIO_PDR_SMA_RX_CFG__PRE, /* pdrSafeRestoreValSmaRx */
   SIO_PDR_SMA_TX_CFG__PRE, /* pdrSafeRestoreValSmaTx */

   DRXK_PACKET_ERR_AVERAGE, /* packetErrMode */

   DRXK_SPIN_UNKNOWN,  /* deviceSpin */

   NULL, /* presetName */

   {
      0  /* presets (un-initialized) */
   },
   {
      0 /* adcCompCoef */
   },
   0,   /* adcState */
   FALSE,   /* adcCompPassed */
   DRXK_DVBT_SQI_SPEED_SLOW,    /* sqiSpeed */
   {
      FALSE  /* audData, only first member */
   },

   0   /* tunerSetFreq */
};


/**
* \brief Default common attributes for a DRK demodulator instance.
*/
DRXCommonAttr_t DRXKDefaultCommAttr_g = {
   (pu8_t)NULL,   /* ucode ptr */
   0,             /* ucode size */
   FALSE,         /* ucode verify switch */
   {0U, 0U, 0U, 0U}, /* version */
   36000,         /* IF in kHz in case no tuner instance is used */
   (151875-0),    /* system clock frequency in kHz */
   0,             /* oscillator frequency kHz*/
   0,             /* oscillator deviation in ppm, signed */
   FALSE,         /* If TRUE mirror frequency spectrum */
   /* MPEG output configuration */
   {
   TRUE,          /* If TRUE, enable MPEG ouput */
   FALSE,         /* If TRUE, insert RS byte */
   TRUE,          /* If TRUE, parallel out otherwise serial */
   FALSE,         /* If TRUE, invert DATA signals */
   FALSE,         /* If TRUE, invert ERR signal */
   FALSE,         /* If TRUE, invert STR signals */
   FALSE,         /* If TRUE, invert VAL signals */
   FALSE,         /* If TRUE, invert CLK signals */
   FALSE,         /* If TRUE, static MPEG clockrate will
                     be used, otherwise clockrate will
                     adapt to the bitrate of the TS */
   32000000UL,    /* Maximum bitrate in b/s in case
                     static clockrate is selected */
      DRX_MPEG_STR_WIDTH_1    /*  length of MPEG start in clock cycles */
   },


   /* Initilisations below require no user input and
      are initialy 0, NULL or FALSE. The compiler will initialize them to these
      values when ommited.  */

   FALSE,               /* isOpened */

   /* Scan */
   NULL,                /* no scan params yet */
   0,                   /* current scan index */
   0,                   /* next scan frequency */
   FALSE,               /* scan ready flag */
   0,                   /* max channels to scan */
   0,                   /* nr of channels scanned */
   NULL,                /* default scan function */
   NULL,                /* default context pointer          */
   0,                   /* millisec to wait for demod lock */
   DRX_LOCKED,          /* desired lock */
   FALSE,               /* scan routines active or not */

   /* Power management */
   DRX_POWER_UP,

   /* Tuner */
   1,                   /* nr of I2C port to wich tuner is */
   0L,                  /* minimum RF input frequency, in kHz */
   0L,                  /* maximum RF input frequency, in kHz */
   FALSE,               /* tunerRfAgcPol */
   FALSE,               /* tunerIfAgcPol */
   FALSE,               /* tunerSlowMode */

   {DRX_UNKNOWN},             /* currentChannel    */
   DRX_STANDARD_UNKNOWN,      /* currentStandard   */
   DRX_STANDARD_UNKNOWN,      /* prevStandard      */
   DRX_STANDARD_UNKNOWN,      /* diCacheStandard   */
   TRUE,                      /* useBootloader     */
   0L,                        /* capabilities      */
   0L                         /* productId         */
};


/**
* \brief Default DRXK demodulator instance.
*/
DRXDemodInstance_t DRXKDefaultDemod_g = {
   &DRXKFunctions_g,       /* demod functions                  */
   &DRXK_DAP,              /* data access protocol functions   */
   NULL,                   /* tuner instance                   */
   &DRXKDefaultAddr_g,     /* i2c address & device id          */
   &DRXKDefaultCommAttr_g, /* demod common attributes          */
   &DRXKData_g             /* demod device specific attributes */
};

/**
* \brief Default audio data structure for DRK demodulator instance.
*
* This structure is DRXK specific.
*
*/
DRXAudData_t DRXKDefaultAudData_g =
{
   FALSE,                  /* audioIsActive */
   DRX_AUD_STANDARD_AUTO,  /* audioStandard  */

   /* i2sdata */
   {
      FALSE,                        /* outputEnable   */
      48000,                        /* frequency      */
      DRX_I2S_MODE_MASTER,          /* mode           */
      DRX_I2S_WORDLENGTH_32,        /* wordLength     */
      DRX_I2S_POLARITY_RIGHT,       /* polarity       */
      DRX_I2S_FORMAT_WS_WITH_DATA   /* format         */
   },
   /* volume            */
   {
      TRUE,                         /* mute;          */
      0,                            /* volume         */
      DRX_AUD_AVC_OFF ,             /* avcMode        */
      0,                            /* avcRefLevel    */
      DRX_AUD_AVC_MAX_GAIN_12DB,    /* avcMaxGain     */
      DRX_AUD_AVC_MAX_ATTEN_24DB,   /* avcMaxAtten    */
      0,                            /* strengthLeft   */
      0                             /* strengthRight  */
   },
   DRX_AUD_AUTO_SOUND_SELECT_ON_CHANGE_ON,/* autoSound */
   /*  assThresholds */
   {
      440,  /* A2    */
      12,   /* BTSC  */
      700,   /* NICAM */
   },
   /* carrier */
   {
      /* a */
      {
         42,                  /* thres */
         DRX_NO_CARRIER_MUTE, /* opt   */
         0,                   /* shift */
         0                    /* dco   */
      },
      /* b */
      {
         42,                  /* thres */
         DRX_NO_CARRIER_MUTE, /* opt   */
         0,                   /* shift */
         0                    /* dco   */
      },

   },
   /* mixer */
   {
      DRX_AUD_SRC_STEREO_OR_A,      /* sourceI2S */
      DRX_AUD_I2S_MATRIX_STEREO,    /* matrixI2S */
      DRX_AUD_FM_MATRIX_SOUND_A     /* matrixFm  */
   },
   DRX_AUD_DEVIATION_NORMAL,        /* deviation */
   DRX_AUD_AVSYNC_OFF,              /* avSync */

   /* prescale */
   {
      DRX_AUD_MAX_FM_DEVIATION,    /* fmDeviation */
      DRX_AUD_MAX_NICAM_PRESCALE   /* nicamGain */
   },
   DRX_AUD_FM_DEEMPH_50US,          /* deemph */
   DRX_BTSC_STEREO,                 /* btscDetect */
   0,                               /* rdsDataCounter */
   FALSE                            /* rdsDataPresent */
};

/*============================================================================*/
/*=== STRUCTURES =============================================================*/
/*============================================================================*/

/**
* \brief Host Interface (HI) command
*/
typedef struct {
   u16_t        cmd;     /**< Command number */
   u16_t        param1;  /**< Parameter 1    */
   u16_t        param2;  /**< Parameter 2    */
   u16_t        param3;  /**< Parameter 3    */
   u16_t        param4;  /**< Parameter 4    */
   u16_t        param5;  /**< Parameter 5    */
   u16_t        param6;  /**< Parameter 6    */
} DRXKHiCmd_t, *pDRXKHiCmd_t;

/**
* \brief DVBT Sytem Controller (SC) command
*/
typedef struct {
   u16_t cmd;        /**< Command number */
   u16_t subcmd;     /**< Sub-command parameter*/
   u16_t param0;     /**< General purpous param */
   u16_t param1;     /**< General purpous param */
   u16_t param2;     /**< General purpous param */
   u16_t param3;     /**< General purpous param */
   u16_t param4;     /**< General purpous param */
} DRXKOfdmScCmd_t, *pDRXKOfdmScCmd_t;


/* Presets file */
//#include "drxk_preset.c.inc"


/*============================================================================*/
/*============================================================================*/
/*==                     START INTEGER MATH FUNCTIONS                       ==*/
/*============================================================================*/
/*============================================================================*/

/**
* \brief Checks for sign bit in a
* \param a a 32 bits integer
* \retval 0 a is not negative
* \retval 1 a is negative
*/
#define DRXK_IS_BOOTH_NEGATIVE( __a ) ((( __a ) & \
                                       (1 << (sizeof (u32_t) * 8 - 1)) ) != 0)

#define DRXK_IS_BOOTH_POSITIVE( __a ) ((( __a ) & \
                                       (1 << (sizeof (u32_t) * 8 - 1)) ) == 0)


/**
* \brief 32bitsx32bits signed multiplication. Computes ((h<<32)+l) = a* b
* \param a 32 bits multiplicant, typecast from signed to unisgned
* \param b 32 bits multiplier, typecast from signed to unisgned
* \param h pointer to high part 64 bits result, typecast from signed to unisgned
* \param l pointer to low part 64 bits result
*
* For the 2n+n addition a + b:
* if a >= 0, then h += 0 (sign extension = 0)
* but if a < 0, then h += 2^n-1 ==> h -= 1.
*
* Also, if a + b < 2^n, then a + b  >= a && a + b >= b
* but if a + b >= 2^n, then R = a + b - 2^n,
* and because a < 2^n && b < 2*n ==> R < a && R < b.
* Therefore, to detect overflow, simply compare the addition result with
* one of the operands; if the result is smaller, overflow has occurred and
* h must be incremented.
*
* Booth multiplication uses additions and subtractions to reduce the number
* of iterations. This is done by taking three subsequent bits abc and
* calculating
* the following multiplication factor: -2a + b + c. This factor is multiplied
* by the second operand and added to the result. Next, the first operand is
* shifted two bits (hence one of the three bits is reused) and the process
* repeated. The last iteration has only two bits left, but we simply add
* a zero to the end.
*
* Hence: (n=4)
*  1 * a =  0 * 4a + 1 * a
*  2 * a =  1 * 4a - 2 * a
*  3 * a =  1 * 4a - 1 * a
* -1 * a =  0 * 4a - 1 * a
* -5 * a = -1 * 4a - 1 * a
*
* etc.
*
* Note that the function is type size independent. Any unsigned integer type
* can be substituted for booth_t.
*
*/
static void  Mult32(u32_t a, u32_t b, pu32_t h, pu32_t l)
{
    unsigned int i;
    *h = *l = 0;

    /*
     n/2 iterations; shift operand a left two bits after each iteration.
     This automatically appends a zero to the operand for the last iteration.
    */
    for (i = 0; i < sizeof (a) * 8; i += 2, a = a << 2)
    {
        /* Shift result left two bits */
        *h = (*h << 2) + (*l >> (sizeof (*l) * 8 - 2));
        *l = (*l << 2);

        /* Take the first three bits of operand a for the Booth conversion: */
        /* 0, 7: do nothing  */
        /* 1, 2: add b       */
        /* 3   : add 2b      */
        /* 4   : subtract 2b */
        /* 5, 6: subtract b  */
        switch (a >> (sizeof (a) * 8 - 3))
        {
        case 3:
            *l += b;
            *h = *h - DRXK_IS_BOOTH_NEGATIVE (b) + (*l < b);
        case 1:
        case 2:
            *l += b;
            *h = *h - DRXK_IS_BOOTH_NEGATIVE (b) + (*l < b);
            break;
        case 4:
            *l -= b;
            *h = *h - DRXK_IS_BOOTH_POSITIVE (b) + !b +
                                                (*l < ((u32_t) (-((s32_t)b))) );
        case 5:
        case 6:
            *l -= b;
            *h = *h - DRXK_IS_BOOTH_POSITIVE (b) + !b +
                                                (*l < ((u32_t) (-((s32_t)b))) );
            break;
        }
    }
}

/*============================================================================*/

/*
* \brief Compute: (1<<28)*N/D
* \param N 32 bits
* \param D 32 bits
* \return (1<<28)*N/D
*
* This function is used to avoid floating-point calculations as they may
* not be present on the target platform.
* Frac28 performs an unsigned 28/28 bits division to 32-bit fixed point
* fraction used for setting the Frequency Shifter registers.
* N and D can hold numbers up to width: 28-bits.
* The 4 bits integer part and the 28 bits fractional part are calculated.
* Usage condition: ((1<<28)*n)/d < ((1<<32)-1) => (n/d) < 15.999
* N: 0...(1<<28)-1 = 268435454
* D: 0...(1<<28)-1
* Q: 0...(1<<32)-1
*
*/
static u32_t Frac28(u32_t N, u32_t D)
{
   int   i=0;
   u32_t Q1=0;
   u32_t R0=0;

   R0 = (N%D)<<4; /* 32-28 == 4 shifts possible at max */
   Q1 = N/D;      /* integer part, only the 4 least significant bits
                  will be visible in the result */

   /* division using radix 16, 7 nibbles in the result */
   for (i=0; i<7; i++) {
      Q1 = (Q1 << 4) | R0/D;
      R0 = (R0%D)<<4;
   }
   /* rounding */
   if ((R0>>3) >= D) Q1++;

   return Q1;
}

/*============================================================================*/

/**
* \brief Compute: 100*log10(x)
* \param x 32 bits
* \return u32_t
* \retval 100*log10(x)
*
* 100*log10(x)
* = 100*(log2(x)/log2(10)))
* = (100*(2^15)*log2(x))/((2^15)*log2(10))
* = ((200*(2^15)*log2(x))/((2^15)*log2(10)))/2
* = ((200*(2^15)*(log2(x/y)+log2(y)))/((2^15)*log2(10)))/2
* = ((200*(2^15)*log2(x/y))+(200*(2^15)*log2(y)))/((2^15)*log2(10)))/2
*
* where y = 2^k and 1<= (x/y) < 2
*/

static u32_t Log10Times100( u32_t x)
{
   static const u8_t scale=15;
   static const u8_t indexWidth=5;
   /*
   log2lut[n] = (1<<scale) * 200 * log2( 1.0 + ( (1.0/(1<<INDEXWIDTH)) * n ))
   0 <= n < ((1<<INDEXWIDTH)+1)
   */

   static const u32_t log2lut[] = {
      0, /* 0.000000 */
      290941, /* 290941.300628 */
      573196, /* 573196.476418 */
      847269, /* 847269.179851 */
      1113620, /* 1113620.489452 */
      1372674, /* 1372673.576986 */
      1624818, /* 1624817.752104 */
      1870412, /* 1870411.981536 */
      2109788, /* 2109787.962654 */
      2343253, /* 2343252.817465 */
      2571091, /* 2571091.461923 */
      2793569, /* 2793568.696416 */
      3010931, /* 3010931.055901 */
      3223408, /* 3223408.452106 */
      3431216, /* 3431215.635215 */
      3634553, /* 3634553.498355 */
      3833610, /* 3833610.244726 */
      4028562, /* 4028562.434393 */
      4219576, /* 4219575.925308 */
      4406807, /* 4406806.721144 */
      4590402, /* 4590401.736809 */
      4770499, /* 4770499.491025 */
      4947231, /* 4947230.734179 */
      5120719, /* 5120719.018555 */
      5291081, /* 5291081.217197 */
      5458428, /* 5458427.996830 */
      5622864, /* 5622864.249668 */
      5784489, /* 5784489.488298 */
      5943398, /* 5943398.207380 */
      6099680, /* 6099680.215452 */
      6253421, /* 6253420.939751 */
      6404702, /* 6404701.706649 */
      6553600, /* 6553600.000000 */
   };


   u8_t  i = 0;
   u32_t y = 0;
   u32_t d = 0;
   u32_t k = 0;
   u32_t r = 0;

   if (x==0) return (0);

   /* Scale x (normalize) */
   /* computing y in log(x/y) = log(x) - log(y) */
   if ( (x & (((u32_t)(-1))<<(scale+1)) ) == 0 )
   {
      for (k = scale; k>0 ; k--)
      {
        if (x & (((u32_t)1)<<scale)) break;
        x <<= 1;
      }
   } else {
      for (k = scale; k<31 ; k++)
      {
        if ((x & (((u32_t)(-1))<<(scale+1)))==0) break;
        x >>= 1;
      }
   }
   /*
     Now x has binary point between bit[scale] and bit[scale-1]
     and 1.0 <= x < 2.0 */

   /* correction for divison: log(x) = log(x/y)+log(y) */
   y = k * ( ( ((u32_t)1) << scale ) * 200 );

   /* remove integer part */
   x &= ((((u32_t)1) << scale)-1);
   /* get index */
   i = (u8_t) (x >> (scale -indexWidth));
   /* compute delta (x-a) */
   d = x & ((((u32_t)1) << (scale-indexWidth))-1);
   /* compute log, multiplication ( d* (.. )) must be within range ! */
   y += log2lut[i] + (( d*( log2lut[i+1]-log2lut[i] ))>>(scale-indexWidth));
   /* Conver to log10() */
   y /= 108853; /* (log2(10) << scale) */
   r = (y>>1);
   /* rounding */
   if (y&((u32_t)1)) r++;

   return (r);

}

/*============================================================================*/

/**
* \brief Compute: (N/D) * 1000000.
* \param N nominator 32-bits.
* \param D denominator 32-bits.
* \return u32_t
* \retval ((N/D) * 1000000), 32 bits
*
*/
static u32_t FracTimes1e6( u32_t N, u32_t D)
{
   u32_t remainder = 0;
   u32_t frac = 0;

   /* Make sure N is small enough for valid calculation */
   if (N > 0xFFFFUL)
   {
      do
      {
         N >>= 1;
         D >>= 1;
      }while (N > 0xFFFFUL);
   }

   /* Clipping in case N/D > maxu32/1e6
      This also prevents div by 0

      maxu32 / 1e6  = 4294.967295
      maxu32 / 4295 = 999992 (~= 1e6)
   */
   if ( N == 0 )
   {
      return (0);
   }
   if ( D < 999992UL )
   {
      if ( N > (D * 4295UL) )
      {
         return (0xFFFFFFFFUL);
      }
   }

   /*
      frac = (N * 1000000) / D
      To let it fit in a 32 bits computation:
      frac = (N * (1000000 >> 4)) / (D >> 4)
      This would result in a problem in case D < 16 (div by 0).
      So we do it more elaborate as shown below.
   */
   frac =  ( ((u32_t)N) * (1000000 >> 4) ) / D ;
   frac <<=  4 ;
   remainder  =  ( ((u32_t)N) * (1000000 >> 4) ) % D ;
   remainder <<= 4;
   frac += remainder / D;
   remainder  = remainder % D ;
   if( (remainder * 2) > D )
   {
      frac++;
   }

   return ( frac );
}

/*============================================================================*/

/**
* \brief Compute: 100 * 10^( GdB / 200 ).
* \param  u32_t   GdB      Gain in 0.1dB
* \return u32_t            Gainfactor in 0.01 resolution
*
*/
static u32_t dB2LinTimes100( u32_t GdB )
{
   u32_t result       = 0;
   u32_t nr6dBSteps   = 0;
   u32_t remainder    = 0;
   u32_t remainderFac = 0;

   /* start with factors 2 (6.02dB) */
   nr6dBSteps = GdB * 1000UL / 60206UL;
   if ( nr6dBSteps > 17 )
   {
      /* Result max overflow if > log2( maxu32 / 2e4 ) ~= 17.7 */
      return MAX_U32;
   }
   result = (1<<nr6dBSteps);

   /* calculate remaining factor,
      poly approximation of 10^(GdB/200):

      y = 1E-04x2 + 0.0106x + 1.0026

      max deviation < 0.005 for range x = [0 ... 60]
   */
   remainder  = ( ( GdB * 1000UL ) % 60206UL ) / 1000UL;
   /* using 1e-4 for poly calculation  */
   remainderFac =    1 * remainder * remainder;
   remainderFac +=   106 * remainder;
   remainderFac +=   10026;

   /* multiply by remaining factor */
   result *= remainderFac;

   /* conversion from 1e-4 to 1e-2 */
   return ( (result + 50 ) / 100);
}

/*============================================================================*/

#define FRAC_FLOOR    0
#define FRAC_CEIL     1
#define FRAC_ROUND    2

/**
* \brief Compute: N/D.
* \param N nominator 32-bits.
* \param D denominator 32-bits.
* \param RC-result correction: 0-floor; 1-ceil; 2-round
* \return u32_t
* \retval N/D, 32 bits
*
* If D=0 returns 0
*/
static u32_t Frac( u32_t N, u32_t D, u16_t RC )
{
   u32_t remainder = 0;
   u32_t frac = 0;
   u16_t bitCnt = 32;
#ifdef DRXKDRIVER_DEBUG
   printf (" frac %s: %lu/%lu=",
      ( ( RC==FRAC_CEIL ) ? "ceil" :
        ( RC==FRAC_ROUND ) ? "round" : "floor" ), N, D);
#endif

   if ( D == 0 )
   {
      frac = 0;
      remainder = 0;
#ifdef DRXKDRIVER_DEBUG
      printf ("  DIVISION with 0!!!\n");
#endif

      return ( frac );
   }

   if ( D > N )
   {
      frac = 0;
      remainder = N;
   }
   else
   {
      remainder = 0;
      frac = N;
      while ( bitCnt-- > 0 )
      {
         remainder <<= 1;
         remainder |= ( ( frac & 0x80000000 ) >> 31 );
         frac <<= 1;
         if ( remainder < D )
         {
            frac &= 0xFFFFFFFE;
         }
         else
         {
            remainder -= D;
            frac |= 0x1;
         }
      }

      /* result correction if needed */
      if ( ( RC == FRAC_CEIL ) && ( remainder != 0 ) )
      {
         /* ceil the result */
         /*(remainder is not zero -> value behind decimal point exists) */
         frac++;
      }
      if ( ( RC == FRAC_ROUND ) && ( remainder >= D>>1 ) )
      {
         /* remainder is bigger from D/2 -> round the result */
         frac++;
      }
   }
#ifdef DRXKDRIVER_DEBUG
   printf ("%lu\tremainder=%lu\n", frac, remainder);
#endif

   return ( frac );
}

/*============================================================================*/
/*============================================================================*/
/*==                     END INTEGER MATH FUNCTIONS                         ==*/
/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*============================================================================*/
/*==                      HOST INTERFACE FUNCTIONS                          ==*/
/*============================================================================*/
/*============================================================================*/

/**
* \brief Execute a HI command
* \param devAddr I2C address.
* \param cmd HI command.
* \param result HI command result.
* \return DRXStatus_t.
* \retval DRX_STS_OK      command has been executed.
* \retval DRX_STS_ERROR   failure; I2C error, timeout or HI error (see result)
* \retval DRX_STS_INVALID_ARG unknown command
*
*/
static DRXStatus_t
HICommand(const pI2CDeviceAddr_t devAddr,
          const pDRXKHiCmd_t cmd,
          pu16_t result)
{
   u16_t  waitCmd=0;
   u16_t  nrRetries = 0;
   Bool_t powerdown_cmd = FALSE;

   *result=SIO_HI_RA_RAM_RES_OK;

   /* Write parameters */
   switch ( cmd->cmd ) {

      case SIO_HI_RA_RAM_CMD_CONFIG:
         WR16(devAddr, SIO_HI_RA_RAM_PAR_6__A, cmd->param6);		 
         /* fallthrough */
      case SIO_HI_RA_RAM_CMD_ATOMIC_COPY:
         WR16(devAddr, SIO_HI_RA_RAM_PAR_5__A, cmd->param5);
         WR16(devAddr, SIO_HI_RA_RAM_PAR_4__A, cmd->param4);
         WR16(devAddr, SIO_HI_RA_RAM_PAR_3__A, cmd->param3);
         /* fallthrough */
      case SIO_HI_RA_RAM_CMD_BRDCTRL:
         WR16(devAddr, SIO_HI_RA_RAM_PAR_2__A, cmd->param2);	 		 
         /* fallthrough */
      case SIO_HI_RA_RAM_CMD_RESET:
         WR16(devAddr, SIO_HI_RA_RAM_PAR_1__A, cmd->param1);
         /* fallthrough */
      case SIO_HI_RA_RAM_CMD_NULL:
         /* No parameters */
         break;

      default:
         return DRX_STS_INVALID_ARG;
         break;
   }
	//printf("FUN:%s,LINE:%d,cmd->cmd:0x%X\n",__FUNCTION__,__LINE__,cmd->cmd);
   /* Write command */
   WR16(devAddr, SIO_HI_RA_RAM_CMD__A, cmd->cmd);

#ifdef COMPILE_FOR_NONRT
   /* In case of Axis & multimaster mode i2c:
      needed due to aritficial i2c speedup on Axis,
      not needed in real world */
   CHK_ERROR(DRXBSP_HST_Sleep(1));
#endif

   if ( (cmd->cmd) == SIO_HI_RA_RAM_CMD_RESET )
   {
      /* Allow for HI to reset */
      CHK_ERROR( DRXBSP_HST_Sleep(1) );
   }

   /* Detect power down to ommit reading result */
   powerdown_cmd = (Bool_t)( ( cmd->cmd == SIO_HI_RA_RAM_CMD_CONFIG ) &&
                     ( ((cmd->param5) & SIO_HI_RA_RAM_PAR_5_CFG_SLEEP__M) ==
                                    SIO_HI_RA_RAM_PAR_5_CFG_SLEEP_ZZZ ) );
   if ( powerdown_cmd == FALSE )
   {
      /* Wait until command rdy */
      do
      {
         nrRetries++;
         if ( nrRetries > 10 )
         {//printf("FUN:%s,LINE:%d\n",__FUNCTION__,__LINE__);
            return DRX_STS_ERROR;
         };

         RR16(devAddr, SIO_HI_RA_RAM_CMD__A, &waitCmd);
		 //printf("FUN:%s,LINE:%d,waitCmd:0x%X\n",__FUNCTION__,__LINE__,waitCmd);
      } while ( waitCmd != 0 );

      if ( waitCmd != 0 )
      {//printf("FUN:%s,LINE:%d\n",__FUNCTION__,__LINE__);
         return DRX_STS_ERROR;
      }

      /* Read result */
      RR16(devAddr, SIO_HI_RA_RAM_RES__A, result);

   } /* if ( powerdown_cmd == TRUE ) */

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Configure HI with settings stored in the demod structure.
* \param demod Demodulator.
* \return DRXStatus_t.
*
* This routine was created because to much orthogonal settings have
* been put into one HI API function (configure). Especially the I2C bridge
* enable/disable should not need re-configuration of the HI.
*
*/
static DRXStatus_t
HICfgCommand(const pDRXDemodInstance_t   demod)
{
   pDRXKData_t extAttr = (pDRXKData_t)(NULL);
   DRXKHiCmd_t hiCmd;
   u16_t       result=0;

   extAttr = (pDRXKData_t)demod -> myExtAttr;

   hiCmd.cmd    = SIO_HI_RA_RAM_CMD_CONFIG;
   hiCmd.param1 = SIO_HI_RA_RAM_PAR_1_PAR1_SEC_KEY;
   hiCmd.param2 = extAttr -> HICfgTimingDiv;
   hiCmd.param3 = extAttr -> HICfgBridgeDelay;
   hiCmd.param4 = extAttr -> HICfgWakeUpKey;
   hiCmd.param5 = extAttr -> HICfgCtrl;
   hiCmd.param6 = extAttr -> HICfgTimeout;

   CHK_ERROR( HICommand( demod -> myI2CDevAddr, &hiCmd, &result) );

   /* Reset power down flag (set one call only) */
   extAttr->HICfgCtrl &= (u16_t)(~(SIO_HI_RA_RAM_PAR_5_CFG_SLEEP_ZZZ));

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Initialise and configurate HI.
* \param demod pointer to demod data.
* \return DRXStatus_t Return status.
* \retval DRX_STS_OK Success.
* \retval DRX_STS_ERROR Failure.
*
* Needs to know Psys (System Clock period) and Posc (Osc Clock period)
* Need to store configuration in driver because of the way I2C
* bridging is controlled.
*
*/
static DRXStatus_t
InitHI( const pDRXDemodInstance_t demod )
{
   pDRXKData_t      extAttr    =(pDRXKData_t)(NULL);
   pDRXCommonAttr_t commonAttr =(pDRXCommonAttr_t)(NULL);
   pI2CDeviceAddr_t devAddr    =(pI2CDeviceAddr_t)(NULL);

   extAttr    = (pDRXKData_t)  demod -> myExtAttr;
   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;
   devAddr    = demod -> myI2CDevAddr;

#if defined(COMPILE_FOR_NONRT)
   extAttr -> HICfgTimingDiv   = 0x0000;
   extAttr -> HICfgBridgeDelay = 0x0707;
   extAttr -> HICfgWakeUpKey   = DRXK_WAKE_UP_KEY;
   extAttr -> HICfgCtrl        = ( SIO_HI_RA_RAM_PAR_5_CFG_SLV0_SLAVE );
   extAttr -> HICfgTimeout     = 0x010f;
#else
   /* Timing div, 250ns/Psys */
   /* Timing div, = ( delay (nano seconds) * sysclk (kHz) )/ 1000 */
   extAttr -> HICfgTimingDiv =
      (u16_t)(((commonAttr->sysClockFreq/1000)* DRXK_HI_I2C_DELAY)/1000) ;
   /* Clipping */
  if ( (extAttr -> HICfgTimingDiv) > SIO_HI_RA_RAM_PAR_2_CFG_DIV__M )
   {
      extAttr -> HICfgTimingDiv = SIO_HI_RA_RAM_PAR_2_CFG_DIV__M;
   }
   /* Bridge delay, uses oscilator clock */
   /* Delay = ( delay (nano seconds) * oscclk (kHz) )/ 1000 */
   /* SDA brdige delay */
   extAttr -> HICfgBridgeDelay =
      (u16_t)(((commonAttr->oscClockFreq/1000) *
                  DRXK_HI_I2C_BRIDGE_DELAY)/1000) ;
   /* Clipping */
   if ( (extAttr -> HICfgBridgeDelay) > SIO_HI_RA_RAM_PAR_3_CFG_DBL_SDA__M )
   {
      extAttr -> HICfgBridgeDelay = SIO_HI_RA_RAM_PAR_3_CFG_DBL_SDA__M;
   }
   /* SCL bridge delay, same as SDA for now */
   extAttr -> HICfgBridgeDelay += (extAttr -> HICfgBridgeDelay)<<
                                             SIO_HI_RA_RAM_PAR_3_CFG_DBL_SCL__B;
   /* Wakeup key, setting the read flag (as suggest in the documentation) does
      not always result into a working solution (barebones worked VI2C failed).
      Not setting the bit works in all cases . */
   extAttr -> HICfgWakeUpKey = DRXK_WAKE_UP_KEY;
   /* port/bridge/power down ctrl */
   extAttr -> HICfgCtrl = ( SIO_HI_RA_RAM_PAR_5_CFG_SLV0_SLAVE );
   /* timeout in case of SIO_HI_RA_RAM_CMD_I2C_TRANSMIT */
   extAttr->HICfgTimeout = DRXK_HI_I2C_TRANSMIT_TIMEOUT;
#endif

   CHK_ERROR( HICfgCommand( demod ) );

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/
/*==                   END HOST INTERFACE FUNCTIONS                         ==*/
/*============================================================================*/

/*============================================================================*/
/*==                      SCU INTERFACE FUNCTIONS                           ==*/
/*============================================================================*/

/**
* \brief Execute a SCU command
* \param devAddr I2C address.
* \param cmd SCU command.
* \return DRXStatus_t.
* \retval DRX_STS_OK      command has been executed.
* \retval DRX_STS_ERROR   failure; I2C error, timeout or SCU error (see result)
* \retval DRX_STS_INVALID_ARG failure: cmd==NULL, scu cmd unknown, std unknow
*
* Using devAddr instead of demod because of SCU atomic register access
* interface.
*
*/

static DRXStatus_t
SCUCommand(  pI2CDeviceAddr_t devAddr, pDRXKSCUCmd_t cmd)
{
   u16_t            curCmd    = 0;
   u32_t            startTime = 0;

   /* Check param */
   if ( cmd == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   /* Wait until SCU command interface is ready to receive command */
   RR16( devAddr, SCU_RAM_COMMAND__A, &curCmd );
   if ( curCmd != SCU_RAM_COMMAND_CMD_READY )
   {
      return DRX_STS_ERROR;
   }

   switch ( cmd->parameterLen )
   {
      case 16:
         WR16( devAddr, SCU_RAM_PARAM_15__A , *(cmd->parameter + 15));
         /* fallthrough */
      case 15:
         WR16( devAddr, SCU_RAM_PARAM_14__A , *(cmd->parameter + 14));
         /* fallthrough */
      case 14:
         WR16( devAddr, SCU_RAM_PARAM_13__A , *(cmd->parameter + 13));
         /* fallthrough */
      case 13:
         WR16( devAddr, SCU_RAM_PARAM_12__A , *(cmd->parameter + 12));
         /* fallthrough */
      case 12:
         WR16( devAddr, SCU_RAM_PARAM_11__A , *(cmd->parameter + 11));
         /* fallthrough */
      case 11:
         WR16( devAddr, SCU_RAM_PARAM_10__A , *(cmd->parameter + 10));
         /* fallthrough */
      case 10:
         WR16( devAddr, SCU_RAM_PARAM_9__A , *(cmd->parameter + 9));
         /* fallthrough */
      case 9:
         WR16( devAddr, SCU_RAM_PARAM_8__A , *(cmd->parameter + 8));
         /* fallthrough */
      case 8:
         WR16( devAddr, SCU_RAM_PARAM_7__A , *(cmd->parameter + 7));
         /* fallthrough */
      case 7:
         WR16( devAddr, SCU_RAM_PARAM_6__A , *(cmd->parameter + 6));
         /* fallthrough */
      case 6:
         WR16( devAddr, SCU_RAM_PARAM_5__A , *(cmd->parameter + 5));
         /* fallthrough */
      case 5:
         WR16( devAddr, SCU_RAM_PARAM_4__A , *(cmd->parameter + 4));
         /* fallthrough */
      case 4:
         WR16( devAddr, SCU_RAM_PARAM_3__A , *(cmd->parameter + 3));
         /* fallthrough */
      case 3:
         WR16( devAddr, SCU_RAM_PARAM_2__A , *(cmd->parameter + 2));
         /* fallthrough */
      case 2:
         WR16( devAddr, SCU_RAM_PARAM_1__A , *(cmd->parameter + 1));
         /* fallthrough */
      case 1:
         WR16( devAddr, SCU_RAM_PARAM_0__A , *(cmd->parameter + 0));
         /* fallthrough */
      case 0:
         /* do nothing */
      break;
      default:
         /* this number of parameters is not supported */
      return DRX_STS_ERROR;
   }
   WR16( devAddr, SCU_RAM_COMMAND__A, cmd->command );

   /* Wait until SCU has processed command */
   startTime = DRXBSP_HST_Clock();
   do{
      RR16( devAddr, SCU_RAM_COMMAND__A, &curCmd );
   } while ( ( curCmd != SCU_RAM_COMMAND_CMD_READY ) &&
             ( (DRXBSP_HST_Clock() - startTime) < DRXK_SCU_CMD_TIMEOUT ) );

   if ( curCmd != SCU_RAM_COMMAND_CMD_READY )
   {
      return DRX_STS_ERROR;
   }

   /* Read results */
   if ( (cmd->resultLen > 0) && (cmd->result != NULL) )
   {
      s16_t err;

      switch ( cmd->resultLen )
      {
         case 16:
            RR16( devAddr, SCU_RAM_PARAM_15__A , cmd->result + 15);
            /* fallthrough */
         case 15:
            RR16( devAddr, SCU_RAM_PARAM_14__A , cmd->result + 14);
            /* fallthrough */
         case 14:
            RR16( devAddr, SCU_RAM_PARAM_13__A , cmd->result + 13);
            /* fallthrough */
         case 13:
            RR16( devAddr, SCU_RAM_PARAM_12__A , cmd->result + 12);
            /* fallthrough */
         case 12:
            RR16( devAddr, SCU_RAM_PARAM_11__A , cmd->result + 11);
            /* fallthrough */
         case 11:
            RR16( devAddr, SCU_RAM_PARAM_10__A , cmd->result + 10);
            /* fallthrough */
         case 10:
            RR16( devAddr, SCU_RAM_PARAM_9__A , cmd->result + 9);
            /* fallthrough */
         case 9:
            RR16( devAddr, SCU_RAM_PARAM_8__A , cmd->result + 8);
            /* fallthrough */
         case 8:
            RR16( devAddr, SCU_RAM_PARAM_7__A , cmd->result + 7);
            /* fallthrough */
         case 7:
            RR16( devAddr, SCU_RAM_PARAM_6__A , cmd->result + 6);
            /* fallthrough */
         case 6:
            RR16( devAddr, SCU_RAM_PARAM_5__A , cmd->result + 5);
            /* fallthrough */
         case 5:
            RR16( devAddr, SCU_RAM_PARAM_4__A , cmd->result + 4);
            /* fallthrough */
         case 4:
            RR16( devAddr, SCU_RAM_PARAM_3__A , cmd->result + 3);
            /* fallthrough */
         case 3:
            RR16( devAddr, SCU_RAM_PARAM_2__A , cmd->result + 2);
            /* fallthrough */
         case 2:
            RR16( devAddr, SCU_RAM_PARAM_1__A , cmd->result + 1);
            /* fallthrough */
         case 1:
            RR16( devAddr, SCU_RAM_PARAM_0__A , cmd->result + 0);
            /* fallthrough */
         case 0:
            /* do nothing */
         break;
         default:
            /* this number of parameters is not supported */
         return DRX_STS_ERROR;
      }


      /* Check if an error was reported by SCU */
      err = cmd->result[0];

      /* Check a few fixed error codes */
      if ( ( ((u16_t)err) == SCU_RAM_PARAM_0_RESULT_UNKCMD ) ||
           ( ((u16_t)err) == SCU_RAM_PARAM_0_RESULT_UNKSTD ) )
      {
         return DRX_STS_INVALID_ARG;
      }
      /* here it is assumed that negative means error, and positive no error */
      else if ( err < 0 )
      {
         return DRX_STS_ERROR;
      }
      else
      {
         return DRX_STS_OK;
      }
   }

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Retreive lock status via SCU command.
* \param devAddr Pointer to demodulator device address.
* \param cmdMask Command mask for standard,will be OR-ed whith SCU get lock cmd
* \param lockStat Pointer to lock status structure.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
SCULockStatus( pDRXDemodInstance_t demod,
               u16_t cmdMask,
               pDRXLockStatus_t    lockStat )
{
   pI2CDeviceAddr_t  devAddr   = NULL;
   u16_t             data      = 0;
   DRXKSCUCmd_t      cmdSCU    = { /* command      */ 0,
                                   /* parameterLen */ 0,
                                   /* resultLen    */ 0,
                                   /* *parameter   */ NULL,
                                   /* *result      */ NULL };
   u16_t cmdResult[2] = { 0, 0 };
   devAddr  = demod->myI2CDevAddr;

   *lockStat = DRX_NOT_LOCKED;


   RR16( devAddr, SCU_COMM_EXEC__A, &data );

   if ( data != SCU_COMM_EXEC_ACTIVE)
   {
      /* scu not running, return NOT_LOCKED */
      return DRX_STS_OK;
   }

   /* define the SCU command paramters and execute the command */
   cmdSCU.command      = (u16_t)(cmdMask | SCU_RAM_COMMAND_CMD_DEMOD_GET_LOCK);
   cmdSCU.parameterLen = 0;
   cmdSCU.resultLen    = 2;
   cmdSCU.parameter    = NULL;
   cmdSCU.result       = cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

   /* set the lock status */
   if ( cmdSCU.result[1] < SCU_RAM_QAM_LOCKED_LOCKED_DEMOD_LOCKED )
   {
      /* 0x0000 NOT LOCKED */
      *lockStat = DRX_NOT_LOCKED;
   }
   else if ( cmdSCU.result[1] < SCU_RAM_QAM_LOCKED_LOCKED_LOCKED )
   {
      /* 0x4000 DEMOD LOCKED */
      *lockStat = DRXK_DEMOD_LOCK;
   }
   else if ( cmdSCU.result[1] < SCU_RAM_QAM_LOCKED_LOCKED_NEVER_LOCK )
   {
       /* 0x8000 DEMOD + FEC LOCKED (system lock) */
      *lockStat = DRX_LOCKED;
   }
   else
   {
      /* 0xC000 NEVER LOCKED */
      /* (system will never be able to lock to the signal) */
      *lockStat = DRX_NEVER_LOCK;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/*==                   END SCU INTERFACE FUNCTIONS                          ==*/
/*============================================================================*/

/*============================================================================*/
/*============================================================================*/
/*==                      DRXK DAP FUNCTIONS                                ==*/
/*============================================================================*/
/*============================================================================*/

/*
   This layer takes care of some device specific register access protocols:
   -conversion to short address format
   -access to audio block
   This layer is placed between the drx_dap_fasi and the rest of the DRXK
   specific implementation. This layer can use address map knowledge whereas
   dap_fasi may not use memory map knowledge.

   * For audio currently only 16 bits read and write register access is
     supported. More is not needed. RMW and 32 or 8 bit access on audio
     registers will have undefined behaviour. Flags (RMW, CRC reset, broadcast
     single/multi master) will be ignored.
*/

/**
/brief Timeout value for audio token ring interface register access (millisec)
*/
#ifndef DRXK_DAP_AUDTRIF_TIMEOUT
#define DRXK_DAP_AUDTRIF_TIMEOUT 80
#endif
/*============================================================================*/

/**
* \brief Check if this address is handled by the audio token ring interface.
* \param addr
* \return Bool_t
* \retval TRUE  Yes, handled by audio token ring interface
* \retval FALSE No, not handled by audio token ring interface
*
* Not using a macro because addr parameter is referenced multiple times.
*
*/
static
Bool_t IsHandledByAudTrIf( DRXaddr_t addr )
{
   Bool_t retval=FALSE;

   if ( ( DRXDAP_FASI_ADDR2BLOCK( addr ) == 4) &&
        (  DRXDAP_FASI_ADDR2BANK( addr ) > 1 ) &&
        (  DRXDAP_FASI_ADDR2BANK( addr ) < 6 ) )
   {
      retval=TRUE;
   }

   return (retval);
}

/*============================================================================*/

static DRXStatus_t DRXK_DAP_ReadBlock (
   pI2CDeviceAddr_t devAddr,
   DRXaddr_t        addr,
   u16_t            datasize,
   pu8_t            data,
   DRXflags_t       flags)
{
   return drxDapFASIFunct_g.readBlockFunc( devAddr,
                                           addr,
                                           datasize,
                                           data,
                                           flags);
}

/*============================================================================*/

static DRXStatus_t DRXK_DAP_ReadModifyWriteReg8 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        waddr,
    DRXaddr_t        raddr,
    u8_t            wdata,
    pu8_t           rdata)
{
   return drxDapFASIFunct_g.readModifyWriteReg8Func( devAddr,
                                                     waddr,
                                                     raddr,
                                                     wdata,
                                                     rdata);
}

/*============================================================================*/

/**
* \brief Read modify write 16 bits audio register using short format only.
* \param devAddr
* \param waddr    Address to write to
* \param raddr    Address to read from (usually SIO_HI_RA_RAM_S0_RMWBUF__A)
* \param wdata    Data to write
* \param rdata    Buffer for data to read
* \return DRXStatus_t
* \retval DRX_STS_OK Succes
* \retval DRX_STS_ERROR Timeout, I2C error, illegal bank
*
* 16 bits register read modify write access using short addressing format only.
* Requires knowledge of the registermap, thus device dependent.
* Using DAP FASI directly to avoid endless recursion of RMWs to audio registers.
*
* TODO Correct define should be #if ( DRXDAPFASI_SHORT_ADDR_ALLOWED==1 )
*      See comments DRXK_DAP_ReadModifyWriteReg16
*/
#if ( DRXDAPFASI_LONG_ADDR_ALLOWED == 0 )
static DRXStatus_t DRXK_DAP_RMWriteReg16Short (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        waddr,
    DRXaddr_t        raddr,
    u16_t            wdata,
    pu16_t           rdata)
{
    DRXStatus_t rc;

    if (rdata == NULL)
    {
        return DRX_STS_INVALID_ARG;
    }

    /* Set RMW flag */
    rc = drxDapFASIFunct_g.writeReg16Func (devAddr,
                                           SIO_HI_RA_RAM_S0_FLG_ACC__A,
                                           SIO_HI_RA_RAM_S0_FLG_ACC_S0_RWM__M,
                                           0x0000);
    if (rc == DRX_STS_OK)
    {
       /* Write new data: triggers RMW */
       rc = drxDapFASIFunct_g.writeReg16Func (devAddr, waddr, wdata, 0x0000 );
    }
    if (rc == DRX_STS_OK)
    {
       /* Read old data */
       rc = drxDapFASIFunct_g.readReg16Func (devAddr, raddr, rdata, 0x0000 );
    }
    if (rc == DRX_STS_OK)
    {
       /* Reset RMW flag */
       rc = drxDapFASIFunct_g.writeReg16Func (devAddr,
                                              SIO_HI_RA_RAM_S0_FLG_ACC__A,
                                              0,
                                              0x0000);
    }

    return rc;
}
#endif

/*============================================================================*/

static DRXStatus_t DRXK_DAP_ReadModifyWriteReg16 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        waddr,
    DRXaddr_t        raddr,
    u16_t            wdata,
    pu16_t           rdata)
{
   /* TODO: correct short/long addressing format decision,
            now long format has higher prio then short because short also
            needs virt bnks (not impl yet) for certain audio registers */
#if ( DRXDAPFASI_LONG_ADDR_ALLOWED==1 )
   return drxDapFASIFunct_g.readModifyWriteReg16Func( devAddr,
                                                      waddr,
                                                      raddr,
                                                      wdata,
                                                      rdata);
#else
   return DRXK_DAP_RMWriteReg16Short( devAddr,
                                      waddr,
                                      raddr,
                                      wdata,
                                      rdata);
#endif
}

/*============================================================================*/

static DRXStatus_t DRXK_DAP_ReadModifyWriteReg32 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        waddr,
    DRXaddr_t        raddr,
    u32_t            wdata,
    pu32_t           rdata)
{
   return drxDapFASIFunct_g.readModifyWriteReg32Func( devAddr,
                                                      waddr,
                                                      raddr,
                                                      wdata,
                                                      rdata);
}

/*============================================================================*/

static DRXStatus_t DRXK_DAP_ReadReg8 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    pu8_t            data,
    DRXflags_t       flags)
{
   return drxDapFASIFunct_g.readReg8Func( devAddr,
                                          addr,
                                          data,
                                          flags);
}

/*============================================================================*/

/**
* \brief Read 16 bits audio register
* \param devAddr
* \param addr
* \param data
* \return DRXStatus_t
* \retval DRX_STS_OK Succes
* \retval DRX_STS_ERROR Timeout, I2C error, illegal bank
*
* 16 bits register read access via audio token ring interface.
*
*/
static DRXStatus_t DRXK_DAP_ReadAudReg16 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    pu16_t           data)
{
   u32_t startTimer=0;
   u32_t currentTimer=0;
   u32_t deltaTimer=0;
   u16_t trStatus=0;
   DRXStatus_t stat=DRX_STS_ERROR;
   u32_t  uiRetryTimes=0;
   /* No read possible for bank 3, return with error */
   if ( DRXDAP_FASI_ADDR2BANK(addr)==3 )
   {
      stat=DRX_STS_INVALID_ARG;
   } else {
      const DRXaddr_t writeBit=((DRXaddr_t)1)<<16;

      /* Force reset write bit */
      addr&=(~writeBit);

      /* Set up read */
      startTimer = DRXBSP_HST_Clock();
      do {
         //DRXBSP_HST_Sleep(1);
         /* RMW to aud TR IF until request is granted or timeout */
         stat = DRXK_DAP_ReadModifyWriteReg16( devAddr,
                                               addr,
                                               SIO_HI_RA_RAM_S0_RMWBUF__A,
                                               0x0000,
                                               &trStatus);

         if (( stat != DRX_STS_OK )||(uiRetryTimes++ > 2))
         {
            break;
         };
         currentTimer = DRXBSP_HST_Clock();
         deltaTimer = currentTimer - startTimer;
      } while ( ( ( trStatus & AUD_TOP_TR_CTR_FIFO_LOCK__M ) ==
                                 AUD_TOP_TR_CTR_FIFO_LOCK_LOCKED ) ||
                  ( ( trStatus & AUD_TOP_TR_CTR_FIFO_FULL__M ) ==
                                 AUD_TOP_TR_CTR_FIFO_FULL_FULL ) );

      /* check if result is OK */
      if (  ( (trStatus & AUD_TOP_TR_CTR_FIFO_LOCK__M ) ==
                  AUD_TOP_TR_CTR_FIFO_LOCK_LOCKED  ) ||
            ( (trStatus & AUD_TOP_TR_CTR_FIFO_FULL__M ) ==
                  AUD_TOP_TR_CTR_FIFO_FULL_FULL) )
      {
         stat = DRX_STS_ERROR;
      }

   } /* if ( DRXDAP_FASI_ADDR2BANK(addr)!=3 ) */

   /* Wait for read ready status or timeout */
   if ( stat == DRX_STS_OK )
   {
      startTimer = DRXBSP_HST_Clock();
      uiRetryTimes=0;
      do
      {
         //DRXBSP_HST_Sleep(1);
         stat = DRXK_DAP_ReadReg16( devAddr,
                                    AUD_TOP_TR_CTR__A,
                                    &trStatus,
                                    0x0000);

         if (( stat != DRX_STS_OK )||(uiRetryTimes++ > 2))
         {
            break;
         };


         currentTimer = DRXBSP_HST_Clock();
         deltaTimer = currentTimer - startTimer;


      }  while( ( trStatus & AUD_TOP_TR_CTR_FIFO_RD_RDY__M) !=
                     AUD_TOP_TR_CTR_FIFO_RD_RDY_READY);
      
      if ((trStatus & AUD_TOP_TR_CTR_FIFO_RD_RDY__M) !=
            AUD_TOP_TR_CTR_FIFO_RD_RDY_READY )
      {
            stat = DRX_STS_ERROR;
      }
   } /* if { stat == DRX_STS_OK ) */

   /* Read value */
   if ( stat == DRX_STS_OK )
   {
         stat = DRXK_DAP_ReadModifyWriteReg16( devAddr,
                                               AUD_TOP_TR_RD_REG__A,
                                               SIO_HI_RA_RAM_S0_RMWBUF__A,
                                               0x0000,
                                               data);
   } /* if { stat == DRX_STS_OK ) */

   return stat;
}

/*============================================================================*/

static DRXStatus_t DRXK_DAP_ReadReg16 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    pu16_t           data,
    DRXflags_t       flags)
{
   DRXStatus_t stat=DRX_STS_ERROR;

   /* Check param */
   if ( ( devAddr == NULL ) || ( data == NULL ) )
   {
      return DRX_STS_INVALID_ARG;
   }

   if ( IsHandledByAudTrIf(addr) )
   {//printf("FUN:%s,addr:0x%X,%d\n",__FUNCTION__,addr,__LINE__);
      stat = DRXK_DAP_ReadAudReg16 (devAddr,
                                    addr,
                                    data);
   } else {//printf("FUN:%s,addr:0x%X,LINE:%d\n",__FUNCTION__,addr,__LINE__);
      stat = drxDapFASIFunct_g.readReg16Func( devAddr,
                                               addr,
                                               data,
                                               flags);
  }

  return stat;
}

/*============================================================================*/

static DRXStatus_t DRXK_DAP_ReadReg32 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    pu32_t           data,
    DRXflags_t       flags)
{
   return drxDapFASIFunct_g.readReg32Func( devAddr,
                                           addr,
                                           data,
                                           flags);
}

/*============================================================================*/

static DRXStatus_t DRXK_DAP_WriteBlock (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u16_t            datasize,
    pu8_t            data,
    DRXflags_t       flags)
{
   return drxDapFASIFunct_g.writeBlockFunc( devAddr,
                                            addr,
                                            datasize,
                                            data,
                                            flags);
}

/*============================================================================*/

static DRXStatus_t DRXK_DAP_WriteReg8 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u8_t            data,
    DRXflags_t       flags)
{
   return drxDapFASIFunct_g.writeReg8Func( devAddr,
                                           addr,
                                           data,
                                           flags);
}

/*============================================================================*/

/**
* \brief Write 16 bits audio register
* \param devAddr
* \param addr
* \param data
* \return DRXStatus_t
* \retval DRX_STS_OK Succes
* \retval DRX_STS_ERROR Timeout, I2C error, illegal bank
*
* 16 bits register write access via audio token ring interface.
*
*/
static DRXStatus_t DRXK_DAP_WriteAudReg16 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u16_t            data)
{
   DRXStatus_t stat=DRX_STS_ERROR;
   u32_t    uiRetryTimes=0;

   /* No write possible for bank 2, return with error */
   if ( DRXDAP_FASI_ADDR2BANK(addr)==2 )
   {
      stat=DRX_STS_INVALID_ARG;
   } else {
      u32_t startTimer     = 0;
      u32_t currentTimer   = 0;
      u32_t deltaTimer     = 0;
      u16_t trStatus       = 0;
      const DRXaddr_t writeBit=((DRXaddr_t)1)<<16;

      /* Force write bit */
      addr|=writeBit;
      startTimer = DRXBSP_HST_Clock();
      do {
         //DRXBSP_HST_Sleep(1);
         /* RMW to aud TR IF until request is granted or timeout */
         stat = DRXK_DAP_ReadModifyWriteReg16( devAddr,
                                               addr,
                                               SIO_HI_RA_RAM_S0_RMWBUF__A,
                                               data,
                                               &trStatus);

         if (( stat != DRX_STS_OK )||(uiRetryTimes++ > 2))
         {
            break;
         };

         currentTimer = DRXBSP_HST_Clock();
         deltaTimer = currentTimer - startTimer;
      } while ( ( ( trStatus & AUD_TOP_TR_CTR_FIFO_LOCK__M ) ==
                                 AUD_TOP_TR_CTR_FIFO_LOCK_LOCKED ) ||
                  ( ( trStatus & AUD_TOP_TR_CTR_FIFO_FULL__M ) ==
                                 AUD_TOP_TR_CTR_FIFO_FULL_FULL ) );

      /* check if result is OK */
      if (  ( (trStatus & AUD_TOP_TR_CTR_FIFO_LOCK__M ) ==
                                 AUD_TOP_TR_CTR_FIFO_LOCK_LOCKED ) ||
                ( ( trStatus & AUD_TOP_TR_CTR_FIFO_FULL__M ) ==
                  AUD_TOP_TR_CTR_FIFO_FULL_FULL) )
      {
         stat = DRX_STS_ERROR;
      }

   } /* if ( DRXDAP_FASI_ADDR2BANK(addr)!=2 ) */

   return stat;
}

/*============================================================================*/

static DRXStatus_t DRXK_DAP_WriteReg16 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u16_t            data,
    DRXflags_t       flags)
{
   DRXStatus_t stat=DRX_STS_ERROR;

   /* Check param */
   if ( devAddr == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }


   if ( IsHandledByAudTrIf(addr) )
   {
      stat = DRXK_DAP_WriteAudReg16 (devAddr,
                                     addr,
                                     data);
   } else {
      stat = drxDapFASIFunct_g.writeReg16Func( devAddr,
                                               addr,
                                               data,
                                               flags);
  }

  return stat;
}

/*============================================================================*/

static DRXStatus_t DRXK_DAP_WriteReg32 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u32_t            data,
    DRXflags_t       flags)
{
   return drxDapFASIFunct_g.writeReg32Func( devAddr,
                                            addr,
                                            data,
                                            flags);
}

/*============================================================================*/

/* Free data ram in SIO HI */
#define SIO_HI_RA_RAM_USR_BEGIN__A 0x420040
#define SIO_HI_RA_RAM_USR_END__A   0x420060

#define DRXK_HI_ATOMIC_BUF_START (SIO_HI_RA_RAM_USR_BEGIN__A)
#define DRXK_HI_ATOMIC_BUF_END   (SIO_HI_RA_RAM_USR_BEGIN__A + 7)
#define DRXK_HI_ATOMIC_READ      SIO_HI_RA_RAM_PAR_3_ACP_RW_READ
#define DRXK_HI_ATOMIC_WRITE     SIO_HI_RA_RAM_PAR_3_ACP_RW_WRITE

/**
* \brief Basic access routine for atomic read or write access via HI
* \param devAddr  pointer to i2c dev address
* \param addr     destination/source address
* \param datasize size of data buffer in bytes
* \param data     pointer to data buffer
* \return DRXStatus_t
* \retval DRX_STS_OK Succes
* \retval DRX_STS_ERROR Timeout, I2C error, illegal bank
*
*/
static
DRXStatus_t DRXK_DAP_AtomicReadWriteBlock (
   pI2CDeviceAddr_t devAddr,
   DRXaddr_t        addr,
   u16_t            datasize,
   pu8_t            data,
   Bool_t           readFlag)
{
   DRXKHiCmd_t hiCmd;

   u16_t word;
   u16_t dummy=0;
   u16_t i=0;

   /* Parameter check */
   if ( ( data    == NULL )  ||
        ( devAddr == NULL )  ||
        ( (datasize%2)!= 0 ) ||
        ( (datasize/2) > 8 )
      )
   {
      return DRX_STS_INVALID_ARG;
   }

   /* Set up HI parameters to read or write n bytes */
   hiCmd.cmd    = SIO_HI_RA_RAM_CMD_ATOMIC_COPY;
   hiCmd.param1 =
         (u16_t)(( DRXDAP_FASI_ADDR2BLOCK( DRXK_HI_ATOMIC_BUF_START ) << 6 ) +
                   DRXDAP_FASI_ADDR2BANK( DRXK_HI_ATOMIC_BUF_START ) );
   hiCmd.param2 = (u16_t)DRXDAP_FASI_ADDR2OFFSET( DRXK_HI_ATOMIC_BUF_START );
   hiCmd.param3 = (u16_t)((datasize/2) - 1);
   if ( readFlag==FALSE )
   {
      hiCmd.param3 |= DRXK_HI_ATOMIC_WRITE;
   } else {
      hiCmd.param3 |= DRXK_HI_ATOMIC_READ;
   }
   hiCmd.param4 = (u16_t) ( ( DRXDAP_FASI_ADDR2BLOCK(addr) << 6 ) +
                            DRXDAP_FASI_ADDR2BANK(addr) );
   hiCmd.param5 = (u16_t)DRXDAP_FASI_ADDR2OFFSET(addr);

   if ( readFlag==FALSE )
   {
      /* write data to buffer */
      for (i = 0; i < (datasize/2); i++)
      {
         word =  ((u16_t)data[2*i]);
         word += (((u16_t)data[(2*i)+1])<<8);
         DRXK_DAP_WriteReg16 (devAddr, (DRXK_HI_ATOMIC_BUF_START + i), word, 0);
      }
   }

   CHK_ERROR( HICommand( devAddr, &hiCmd, &dummy) );

   if ( readFlag==TRUE )
   {
      /* read data from buffer */
      for (i = 0; i < (datasize/2); i++)
      {
         DRXK_DAP_ReadReg16 (devAddr, (DRXK_HI_ATOMIC_BUF_START + i), &word, 0);
         data[2*i]       = (u8_t) (word & 0xFF);
         data[(2*i) + 1] = (u8_t) (word >> 8 );
      }
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;

}


/*============================================================================*/

/**
* \brief Atomic read of 32 bits words via HI
*/
static
DRXStatus_t DRXK_DAP_AtomicReadReg32 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    pu32_t           data,
    DRXflags_t       flags)
{
    u8_t buf[sizeof (*data)];
    DRXStatus_t rc = DRX_STS_ERROR;
    u32_t word = 0;

    if (!data)
    {
        return DRX_STS_INVALID_ARG;
    }

    rc = DRXK_DAP_AtomicReadWriteBlock ( devAddr, addr,
                                         sizeof (*data), buf, TRUE);

    word = (u32_t)buf[3];
    word <<= 8;
    word |= (u32_t)buf[2];
    word <<= 8;
    word |= (u32_t)buf[1];
    word <<= 8;
    word |= (u32_t)buf[0];

    *data = word;

    return rc;
}

/*============================================================================*/

#if 0
/* Not used right now, excluded to avoid compiler warning */
/**
* \brief Atomic read of 32 bits words via HI
*/
static
DRXStatus_t DRXK_DAP_AtomicWriteReg32 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u32_t            data,
    DRXflags_t       flags)

    u8_t buf[sizeof (data)];

    buf[0] = ((u8_t)data) & 0xFF;
    data>>=8;
    buf[1] = ((u8_t)data) & 0xFF;
    data>>=8;
    buf[2] = ((u8_t)data) & 0xFF;
    data>>=8;
    buf[3] = ((u8_t)data) & 0xFF;

    return ( DRXK_DAP_AtomicReadWriteBlock ( devAddr, addr,
                                             sizeof (data), buf, FALSE) );
}
#endif

/*============================================================================*/

/**
* \brief Translate register address x to internal SCU address
*/
#define ADDR_AT_SCU_SPACE(x) ((x - 0x82E000) * 2)

/**
* \brief Basic access routine for SCU atomic read or write access
* \param devAddr  pointer to i2c dev address
* \param addr     destination/source address
* \param datasize size of data buffer in bytes
* \param data     pointer to data buffer
* \return DRXStatus_t
* \retval DRX_STS_OK Succes
* \retval DRX_STS_ERROR Timeout, I2C error, illegal bank
*
*  The datasize parameter is maximized to 30 bytes because of SCU limitations.
*
*/
static
DRXStatus_t DRXK_DAP_SCU_AtomicReadWriteBlock (
   pI2CDeviceAddr_t devAddr,
   DRXaddr_t        addr,
   u16_t            datasize,
   pu8_t            data,
   Bool_t           readFlag)
{
   DRXKSCUCmd_t      scuCmd;
   u16_t             setParamParameters[15] = {0};
   u16_t             cmdResult[15]= {0};

   /* Parameter check */
   if ( ( data    == NULL )  ||
        ( devAddr == NULL )  ||
        ( (datasize%2)!= 0 ) ||
        ( (datasize/2) > 16 )
      )
   {
      return DRX_STS_INVALID_ARG;
   }

   setParamParameters[1] = (u16_t)ADDR_AT_SCU_SPACE (addr);
   if (readFlag) /* read */
   {
      setParamParameters[0] = (u16_t)((~(0x0080)) & datasize);
      scuCmd.parameterLen = 2;
      scuCmd.resultLen    = datasize/2 + 2;
   } else {
      int i = 0;

      setParamParameters[0] = 0x0080 | datasize;
      for (i = 0; i < (datasize/2); i++)
      {
         setParamParameters[i+2] =  (data[2*i] | (data[(2*i)+1]<<8));
      }
      scuCmd.parameterLen = datasize / 2 + 2;
      scuCmd.resultLen    = 1;
   }

   scuCmd.command = SCU_RAM_COMMAND_STANDARD_TOP |
                    SCU_RAM_COMMAND_CMD_AUX_SCU_ATOMIC_ACCESS;
   scuCmd.result       = cmdResult;
   scuCmd.parameter    = setParamParameters;
   CHK_ERROR( SCUCommand( devAddr, &scuCmd ) );

   if ( readFlag==TRUE )
   {
      int i = 0;
      /* read data from buffer */
      for (i = 0; i < (datasize/2); i++)
      {
         data[2*i]         = (u8_t) (scuCmd.result[i+2] & 0xFF);
         data[(2*i) + 1]   = (u8_t) (scuCmd.result[i+2] >> 8 );
      }
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;

}

/*============================================================================*/

/**
* \brief Atomic read of 16 bits SCU register
*/
static
DRXStatus_t DRXK_DAP_SCU_AtomicReadReg16 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    pu16_t           data,
    DRXflags_t       flags)
{
    u8_t buf[2];
    DRXStatus_t rc   = DRX_STS_ERROR;
    u16_t word       = 0;

    if (!data)
    {
        return DRX_STS_INVALID_ARG;
    }

    rc = DRXK_DAP_SCU_AtomicReadWriteBlock ( devAddr, addr,
                                         2, buf, TRUE);

    word = (u16_t)(buf[0] + (buf[1] << 8));

    *data = word;

    return rc;
}
/*============================================================================*/

/**
* \brief Atomic write of 16 bits SCU register
*/
static
DRXStatus_t DRXK_DAP_SCU_AtomicWriteReg16 (
    pI2CDeviceAddr_t devAddr,
    DRXaddr_t        addr,
    u16_t            data,
    DRXflags_t       flags)
{

    u8_t buf[2];
    DRXStatus_t rc = DRX_STS_ERROR;

    buf[0] = (u8_t) (data & 0xff);
    buf[1] = (u8_t) ((data >> 8) & 0xff);

    rc = DRXK_DAP_SCU_AtomicReadWriteBlock ( devAddr, addr,
                                         2, buf, FALSE);

    return rc;
}


/*============================================================================*/
/*==                        END DRXK DAP FUNCTIONS                          ==*/
/*============================================================================*/

/*============================================================================*/
/* I2C Bridge Functions - begin                                               */
/*============================================================================*/
/**
* \brief Open or close the I2C switch to tuner.
* \param demod Pointer to demodulator instance.
* \param bridgeClosed Pointer to bool indication if bridge is closed not.
* \return DRXStatus_t.

*/
static DRXStatus_t
CtrlI2CBridge( pDRXDemodInstance_t demod,
               pBool_t             bridgeClosed )
{
   DRXKHiCmd_t hiCmd;
   u16_t       result = 0;

   /* Check arguments */
   if (bridgeClosed == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   hiCmd.cmd    = SIO_HI_RA_RAM_CMD_BRDCTRL;
   hiCmd.param1 = SIO_HI_RA_RAM_PAR_1_PAR1_SEC_KEY;
   if (*bridgeClosed == TRUE)
   {
      hiCmd.param2 = SIO_HI_RA_RAM_PAR_2_BRD_CFG_CLOSED;
   }
   else
   {
      hiCmd.param2 = SIO_HI_RA_RAM_PAR_2_BRD_CFG_OPEN;
  }

   return HICommand( demod -> myI2CDevAddr, &hiCmd, &result);
}
/*============================================================================*/
/* I2C Bridge Functions - end                                                 */
/*============================================================================*/

/*============================================================================*/
/*==                       IQM RELATED FUNCTIONS                            ==*/
/*============================================================================*/


/* -------------------------------------------------------------------------- */

/**
* \brief Configure IQM AF registers
* \param demod instance of demodulator.
* \param active
* \return DRXStatus_t.
*/
static DRXStatus_t
IQMSetAf ( pDRXDemodInstance_t   demod, Bool_t active )
{
   u16_t data                 = 0;
   pI2CDeviceAddr_t devAddr   = NULL;
   pDRXKData_t      extAttr   = NULL;

   extAttr = (pDRXKData_t)demod->myExtAttr;
   devAddr = demod -> myI2CDevAddr;

   /* Configure IQM */
   RR16( devAddr, IQM_AF_STDBY__A , &data );
   if( !active )
   {
         data |= (IQM_AF_STDBY_STDBY_ADC_STANDBY
                  | IQM_AF_STDBY_STDBY_AMP_STANDBY
                  | IQM_AF_STDBY_STDBY_PD_STANDBY
                  | IQM_AF_STDBY_STDBY_TAGC_IF_STANDBY
                  | IQM_AF_STDBY_STDBY_TAGC_RF_STANDBY
                  );

   } else /* active */
   {
         data &= ((~IQM_AF_STDBY_STDBY_ADC_STANDBY)
                  & (~IQM_AF_STDBY_STDBY_AMP_STANDBY)
                  & (~IQM_AF_STDBY_STDBY_PD_STANDBY)
                  & (~IQM_AF_STDBY_STDBY_TAGC_IF_STANDBY)
                  & (~IQM_AF_STDBY_STDBY_TAGC_RF_STANDBY)
                  );
   }
   WR16( devAddr, IQM_AF_STDBY__A , data );

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/*============================================================================*/
/*==                   IQM RELATED FUNCTIONS - E N D                        ==*/
/*============================================================================*/


/*============================================================================*/
/*==                        AUXILIARY FUNCTIONS                             ==*/
/*============================================================================*/

/**
* \brief Power up device with a fixed wake up key.
* \param demod  Pointer to demodulator instance.
* \param wakeUpKey  Key to wake up device from power saving state.
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure: I2C error or no matching wake up key
*
* This routine will try to wake up the device from any power saving state.
* (1) In case the device is currently already in the power up or ofdm power
*     saving state the loop writting the wakeup key and hi state machine reset
*     is not actually not needed. But since this routine does not want to make
*     assumptions of it's starting point it is done anyway. In this case the
*     loop will be executed once.
* (2) In case the device is in any other power down state this routine
*     will try to
*     wake up the device using the specified wake up key of the device.
*
* Initializes SIO_CC_PLL_LOCK__A to 1, to enable check on loss of PLL lock.
*
*/
static DRXStatus_t
WakeUpDeviceLoop( pDRXDemodInstance_t demod, I2Caddr_t wakeUpKey )
{
   pI2CDeviceAddr_t devAddr    =(pI2CDeviceAddr_t)(NULL);
   u8_t             data       = 0 ;
   u16_t            retryCount = 0;
   I2CDeviceAddr_t  wakeUpAddr;

   devAddr = demod -> myI2CDevAddr;
   wakeUpAddr.i2cAddr  =  wakeUpKey;
   wakeUpAddr.i2cDevId =  devAddr->i2cDevId;
   wakeUpAddr.userData =  devAddr->userData;

   /* CHK_ERROR macro not used, I2C access may fail in this case: no ack
      dummy write must be used to wake up device, dummy read must be used to
      reset HI state machine (avoiding actual writes) & to check if wake up
      succeeded  */
   do
   {
      data = 0;
      /* Dummy write for wake up */
      DRXBSP_I2C_WriteRead( &wakeUpAddr, 1, &data,
                            (pI2CDeviceAddr_t)(NULL), 0, (pu8_t)(NULL) );
      CHK_ERROR( DRXBSP_HST_Sleep(5) );  //CHK_ERROR( DRXBSP_HST_Sleep(10) );
      retryCount++ ;
   }while ( (DRXBSP_I2C_WriteRead( (pI2CDeviceAddr_t)(NULL), 0, (pu8_t)(NULL),
                                   devAddr, 1, &data )
            != DRX_STS_OK ) &&
            (retryCount < DRXK_MAX_RETRIES_POWERUP) );

   if ( retryCount >= DRXK_MAX_RETRIES_POWERUP )
   {//printf("FUN:%s,LINE:%d, ERROR\n", __FUNCTION__,__LINE__);
      return DRX_STS_ERROR;
   } else {
      /* Make sure all clk domains are active */
      WR16( devAddr, SIO_CC_PWD_MODE__A, SIO_CC_PWD_MODE_LEVEL_NONE); 
      WR16( devAddr, SIO_CC_UPDATE__A  , SIO_CC_UPDATE_KEY);

      /* Enable pll lock tests */
      WR16 ( devAddr, SIO_CC_PLL_LOCK__A, 1);

      demod->myCommonAttr->currentPowerMode = DRX_POWER_UP;
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Power up device.
* \param demod  Pointer to demodulator instance.
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure, I2C or max retries reached
*
* This routine will try to wake up the device from a power saving state.
* (1) In case the device is currently already in the power up state this routine
*     will succeed.
* (2) In case the device is in a power down state this routine will try to
*     wake up the device by writing the wake up key of the device. To cover
*     several situations, multiple wake up keys are tried, in the following
*     order:
*     (a) wake up key as defined by DRXK_WAKE_UP_KEY
*     (b) wake up key as defined by demod -> myI2CDevAddr->i2cAddr
*     (c) wake up key as defined by hardware reset value (0xE0)
*/

static DRXStatus_t
WakeUpDevice( pDRXDemodInstance_t demod )
{
   DRXStatus_t sts=DRX_STS_ERROR;

   if ( ( WakeUpDeviceLoop( demod, DRXK_WAKE_UP_KEY ) == DRX_STS_OK ) ||
        ( WakeUpDeviceLoop( demod, demod ->myI2CDevAddr->i2cAddr )
                                                       == DRX_STS_OK ) ||
        ( WakeUpDeviceLoop( demod, 0xE0 ) == DRX_STS_OK ) )
   {
      /* Wake up succeeded */
      sts = DRX_STS_OK;
   }

   return (sts);
}

/*============================================================================*/

/**
* \brief Get and store device capabilities.
* \param demod  Pointer to demodulator instance.
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure: I2C error or unknown device
*
*/

static DRXStatus_t
GetDeviceCapabilities( pDRXDemodInstance_t demod )
{
   pDRXCommonAttr_t commonAttr     = (pDRXCommonAttr_t)(NULL);
   pDRXKData_t      extAttr        = (pDRXKData_t)NULL;
   pI2CDeviceAddr_t devAddr        = (pI2CDeviceAddr_t)(NULL);
   u16_t            sioPdrOhwCfg   = 0;
   u32_t            sioTopJtagidLo = 0;

   commonAttr = (pDRXCommonAttr_t)demod ->myCommonAttr;
   extAttr    = (pDRXKData_t) demod ->myExtAttr;
   devAddr    = demod ->myI2CDevAddr;

   /* stop lock indicator process */
   WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_DISABLE );

   WR16 ( devAddr, SIO_TOP_COMM_KEY__A , SIO_TOP_COMM_KEY_KEY);
   RR16 ( devAddr, SIO_PDR_OHW_CFG__A  , &sioPdrOhwCfg);/* 0x23 */
   //printf("FUN:%s,LINE:%d,sioPdrOhwCfg:0x%X\n",__FUNCTION__,__LINE__,sioPdrOhwCfg);
   WR16 ( devAddr, SIO_TOP_COMM_KEY__A , 0x0000);

   switch ( (sioPdrOhwCfg & SIO_PDR_OHW_CFG_FREF_SEL__M ) )
   {
      case 0:
         /* ignore (bypass)*/
         break;
      case 1:
         /* 27 MHz */
         commonAttr->oscClockFreq = 27000;
         break;
      case 2:
         /* 20.25 MHz */
         commonAttr->oscClockFreq = 20250;
         break;
      case 3:
         /* 20.25 MHz */
         commonAttr->oscClockFreq = 20250;
         break;
      default:
         return DRX_STS_ERROR;
   }

   /* Determine device capabilities
      Based on pinning v14 */
   RR32( devAddr, SIO_TOP_JTAGID_LO__A  , &sioTopJtagidLo);/* 0x639260D9 */
   //printf("FUN:%s,LINE:%d,sioTopJtagidLo:0x%X\n",__FUNCTION__,__LINE__,sioTopJtagidLo);
   extAttr->mfx = (u8_t)((sioTopJtagidLo>>29)&0xF) ;
   DRX_SET_MFX (demod, extAttr->mfx);
   switch( extAttr->mfx )
   {
      case 0:
         extAttr->deviceSpin = DRXK_SPIN_A1;
         break;
      case 2:
         extAttr->deviceSpin = DRXK_SPIN_A2;
         break;
      case 3:
         extAttr->deviceSpin = DRXK_SPIN_A3;
         break;
      default:
         extAttr->deviceSpin = DRXK_SPIN_UNKNOWN;
         break;
   }

   /* Set capabilities flags */
   if (  ((sioTopJtagidLo >> 16) & 0xFFF) == DRXK_PRODUCT_IDN (1) ||
         ((sioTopJtagidLo >> 16) & 0xFFF) == DRXK_PRODUCT_IDN (2))
   {
      DRX_SET_PRODUCTID( demod, DRXK_PRODUCT_ID );
   }

   switch ((sioTopJtagidLo>>12)&0xFF)
   {
      case 0x13:
         demod->myDemodFunct->typeId = DRX3913K_TYPE_ID;
         extAttr->hasLNA   = FALSE;
         extAttr->hasOOB   = FALSE;
         extAttr->hasATV   = FALSE;
         extAttr->hasAudio = FALSE;
         extAttr->hasDVBT  = TRUE;
         extAttr->hasDVBC  = TRUE;
         extAttr->hasSAWSW = TRUE;
         extAttr->hasGPIO2 = FALSE;
         extAttr->hasGPIO1 = FALSE;
         extAttr->hasIRQN  = FALSE;
         break;
      case 0x15:
         demod->myDemodFunct->typeId = DRX3915K_TYPE_ID;
         extAttr->hasLNA   = FALSE;
         extAttr->hasOOB   = FALSE;
         extAttr->hasATV   = TRUE;
         extAttr->hasAudio = FALSE;
         extAttr->hasDVBT  = TRUE;
         extAttr->hasDVBC  = FALSE;
         extAttr->hasSAWSW = TRUE;
         extAttr->hasGPIO2 = TRUE;
         extAttr->hasGPIO1 = TRUE;
         extAttr->hasIRQN  = FALSE;
         break;
      case 0x16:
         demod->myDemodFunct->typeId = DRX3916K_TYPE_ID;
         extAttr->hasLNA   = FALSE;
         extAttr->hasOOB   = FALSE;
         extAttr->hasATV   = TRUE;
         extAttr->hasAudio = FALSE;
         extAttr->hasDVBT  = TRUE;
         extAttr->hasDVBC  = FALSE;
         extAttr->hasSAWSW = TRUE;
         extAttr->hasGPIO2 = TRUE;
         extAttr->hasGPIO1 = TRUE;
         extAttr->hasIRQN  = FALSE;
         break;
      case 0x18:
         demod->myDemodFunct->typeId = DRX3918K_TYPE_ID;
         extAttr->hasLNA   = FALSE;
         extAttr->hasOOB   = FALSE;
         extAttr->hasATV   = TRUE;
         extAttr->hasAudio = TRUE;
         extAttr->hasDVBT  = TRUE;
         extAttr->hasDVBC  = FALSE;
         extAttr->hasSAWSW = TRUE;
         extAttr->hasGPIO2 = TRUE;
         extAttr->hasGPIO1 = TRUE;
         extAttr->hasIRQN  = FALSE;
         break;
      case 0x21:
         demod->myDemodFunct->typeId = DRX3921K_TYPE_ID;
         extAttr->hasLNA   = FALSE;
         extAttr->hasOOB   = FALSE;
         extAttr->hasATV   = TRUE;
         extAttr->hasAudio = TRUE;
         extAttr->hasDVBT  = TRUE;
         extAttr->hasDVBC  = TRUE;
         extAttr->hasSAWSW = TRUE;
         extAttr->hasGPIO2 = TRUE;
         extAttr->hasGPIO1 = TRUE;
         extAttr->hasIRQN  = FALSE;
         break;
      case 0x23:
         demod->myDemodFunct->typeId = DRX3923K_TYPE_ID;
         extAttr->hasLNA   = FALSE;
         extAttr->hasOOB   = FALSE;
         extAttr->hasATV   = TRUE;
         extAttr->hasAudio = TRUE;
         extAttr->hasDVBT  = TRUE;
         extAttr->hasDVBC  = TRUE;
         extAttr->hasSAWSW = TRUE;
         extAttr->hasGPIO2 = TRUE;
         extAttr->hasGPIO1 = TRUE;
         extAttr->hasIRQN  = FALSE;
         break;
      case 0x25:
         demod->myDemodFunct->typeId = DRX3925K_TYPE_ID;
         extAttr->hasLNA   = FALSE;
         extAttr->hasOOB   = FALSE;
         extAttr->hasATV   = TRUE;
         extAttr->hasAudio = TRUE;
         extAttr->hasDVBT  = TRUE;
         extAttr->hasDVBC  = TRUE;
         extAttr->hasSAWSW = TRUE;
         extAttr->hasGPIO2 = TRUE;
         extAttr->hasGPIO1 = TRUE;
         extAttr->hasIRQN  = FALSE;
         break;
      case 0x26:
         demod->myDemodFunct->typeId = DRX3926K_TYPE_ID;
         extAttr->hasLNA   = FALSE;
         extAttr->hasOOB   = FALSE;
         extAttr->hasATV   = TRUE;
         extAttr->hasAudio = FALSE;
         extAttr->hasDVBT  = TRUE;
         extAttr->hasDVBC  = TRUE;
         extAttr->hasSAWSW = TRUE;
         extAttr->hasGPIO2 = TRUE;
         extAttr->hasGPIO1 = TRUE;
         extAttr->hasIRQN  = FALSE;
         break;
      default:
         /* Unknown device variant, assume full featured */
         demod->myDemodFunct->typeId = 0;
         extAttr->hasLNA   = FALSE;
         extAttr->hasOOB   = FALSE;
         extAttr->hasATV   = TRUE;
         extAttr->hasAudio = TRUE;
         extAttr->hasDVBT  = TRUE;
         extAttr->hasDVBC  = TRUE;
         extAttr->hasSAWSW = TRUE;
         extAttr->hasGPIO2 = TRUE;
         extAttr->hasGPIO1 = TRUE;
         extAttr->hasIRQN  = FALSE;
         break;
   }

   if ( extAttr->uioSawSwMode ==
        DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR )
   {
      /* restart lock indicator process */
      WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_ENABLE );
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Setup a bootloader chain command.
* \param devAddr      I2C addres of device
* \param romOffset    Index
* \param nrOfElements Number of entries
* \param timeOut      time out value in millisec
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure: I2C error or unknown device
*/

static DRXStatus_t
BLChainCmd( pI2CDeviceAddr_t devAddr,
            u16_t romOffset,
            u16_t nrOfElements )
{
   u32_t       startWait  = 0;
   u32_t       timerValue = 0;
   u16_t       blStatus   = 0;
   DRXStatus_t sts        = DRX_STS_OK;
   u32_t       uiRetryTimes=0;
   WR16( devAddr, SIO_BL_MODE__A,       SIO_BL_MODE_CHAIN); /* 0x480011 0x1 */
   WR16( devAddr, SIO_BL_CHAIN_ADDR__A, romOffset);
   WR16( devAddr, SIO_BL_CHAIN_LEN__A,  nrOfElements);
   WR16( devAddr, SIO_BL_ENABLE__A,     SIO_BL_ENABLE_ON);

   startWait = DRXBSP_HST_Clock();
   do{
      RR16( devAddr, SIO_BL_STATUS__A, &blStatus);
	  pbiinfo("FUN:%s,blStatus:0x%X\n",__FUNCTION__,blStatus);
      timerValue = DRXBSP_HST_Clock() - startWait;
   } while( (uiRetryTimes++ <= 10 ) && ( blStatus == 0x1 ));

   if ( blStatus == 0x1 )
   {
      sts = DRX_STS_ERROR;
   }

   return sts;

 rw_error:
   return DRX_STS_ERROR;

}

/*============================================================================*/

/**
* \brief Setup a bootloader Direct command.
* \param devAddr      I2C addres of device
* \param targetAddr   Target address for transfer
* \param romOffset    Source offset for transfer
* \param nrOfElements Number of words
* \param timeOut      time out value in millisec
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure: I2C error or unknown device
*/

static DRXStatus_t
BLDirectCmd( pI2CDeviceAddr_t devAddr,
             u32_t targetAddr,
             u16_t romOffset,
               u16_t nrOfElements )
{
   u32_t       startWait  = 0;
   u32_t       timerValue = 0;
   u16_t       blStatus   = 0;
   u16_t       offset     = (u16_t)((targetAddr >> 0 ) & 0x00FFFF);
   u16_t       blockbank  = (u16_t)((targetAddr >> 16) & 0x000FFF);
   DRXStatus_t sts        = DRX_STS_OK;
   u32_t       uiRetryTimes=0;

   WR16( devAddr, SIO_BL_MODE__A,       SIO_BL_MODE_DIRECT);
   WR16( devAddr, SIO_BL_TGT_HDR__A,    blockbank);
   WR16( devAddr, SIO_BL_TGT_ADDR__A,   offset);
   WR16( devAddr, SIO_BL_SRC_ADDR__A,   romOffset);
   WR16( devAddr, SIO_BL_SRC_LEN__A,    nrOfElements);
   WR16( devAddr, SIO_BL_ENABLE__A,     SIO_BL_ENABLE_ON);

   startWait = DRXBSP_HST_Clock();
   do{
      RR16( devAddr, SIO_BL_STATUS__A, &blStatus);
      timerValue = DRXBSP_HST_Clock() - startWait;
   } while( (uiRetryTimes++ <=  2) && ( blStatus == 0x1 ));

   if ( blStatus == 0x1 )
   {
      sts = DRX_STS_ERROR;
   }

   return sts;

 rw_error:
   return DRX_STS_ERROR;

}
/*============================================================================*/

/**
* \brief Handle filter coefficients upload
* \param demod:         Pointer to demod.
* \param filterCoefs:   Pointer to information about filter coefficients
* \return DRXStatus_t.
* \retval DRX_STS_OK:
* \retval DRX_STS_ERROR:
*                    - I2C error.
* \retval DRX_STS_INVALID_ARG:
*                    - Invalid arguments.
*/

static DRXStatus_t
CtrlLoadFilter (  pDRXDemodInstance_t    demod,
                  pDRXFilterInfo_t       filter )
{
   pI2CDeviceAddr_t devAddr   = NULL;

   if (  ( demod == NULL )               ||
         ( demod->myDemodFunct == NULL ) ||
         ( demod->myCommonAttr == NULL ) ||
         ( demod->myExtAttr == NULL )    ||
         ( demod->myI2CDevAddr == NULL ) ||
         ( demod->myCommonAttr->isOpened != TRUE ) ||
         ( filter == NULL ))

   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr  = (pI2CDeviceAddr_t)demod->myI2CDevAddr;

   /* first upload the RE part */
   WRB(  devAddr, IQM_CF_TAP_RE0__A,   filter->sizeRe,
                                       filter->dataRe);

   /* now upload the IM part */
   WRB (  devAddr, IQM_CF_TAP_IM0__A,  filter->sizeIm,
                                       filter->dataIm );


   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}
/*============================================================================*/

/**
* \brief Measure result of ADC synchronisation
* \param demod demod instance
* \param count (returned) count
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure: I2C error
*
*/

static DRXStatus_t
ADCSyncMeasurement( pDRXDemodInstance_t demod,
                    pu16_t              count )
{
   pI2CDeviceAddr_t devAddr    = NULL;
   u16_t            data       = 0;

   devAddr = demod -> myI2CDevAddr;

   /* Start measurement */
   WR16( devAddr, IQM_AF_COMM_EXEC__A, IQM_AF_COMM_EXEC_ACTIVE);
   WR16( devAddr, IQM_AF_START_LOCK__A, 1);

   /* Wait at least 3*128*(1/sysclk) = ~3usec <<< 1 millisec << 1 I2C access */
   /* no need to wait using DRXBSP_HST_Sleep() */

   *count = 0;
   RR16( devAddr, IQM_AF_PHASE0__A, &data);
   if ( data == 127 )
   {
      *count = *count+1;
   }
   RR16( devAddr, IQM_AF_PHASE1__A, &data);
   if ( data == 127 )
   {
      *count = *count+1;
   }
   RR16( devAddr, IQM_AF_PHASE2__A, &data);
   if ( data == 127 )
   {
      *count = *count+1;
   }

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Synchronize analog and digital clock domains
* \param demod demod instance
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure: I2C error or failure to synchronize
*
* An IQM reset will also reset the results of this synchronization.
* After an IQM reset this routine needs to be called again.
*
*/

static DRXStatus_t
ADCSynchronization( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t devAddr    = NULL;
   u16_t            count      = 0;

   devAddr = demod -> myI2CDevAddr;

   CHK_ERROR( ADCSyncMeasurement( demod, &count ));

   if ( count == 1 )
   {
      /* Try sampling on a different edge,
         useless in case count == 0 */
      u16_t clkNeg = 0;

      RR16( devAddr, IQM_AF_CLKNEG__A, &clkNeg);
      if ( ( clkNeg &  IQM_AF_CLKNEG_CLKNEGDATA__M ) ==
                                     IQM_AF_CLKNEG_CLKNEGDATA_CLK_ADC_DATA_POS )
      {
         clkNeg &= (~(IQM_AF_CLKNEG_CLKNEGDATA__M));
         clkNeg |= IQM_AF_CLKNEG_CLKNEGDATA_CLK_ADC_DATA_NEG;
      } else {
         clkNeg &= (~(IQM_AF_CLKNEG_CLKNEGDATA__M));
         clkNeg |= IQM_AF_CLKNEG_CLKNEGDATA_CLK_ADC_DATA_POS;
      }
      WR16( devAddr, IQM_AF_CLKNEG__A, clkNeg);

      CHK_ERROR( ADCSyncMeasurement( demod, &count ));
   }

#if 0
   if ( count < 2 )
   {
      /* This should not happen,
         however if it does possible fallback scenario
         would be shifting edge of system clock (SIO_CC_CLK_MODE)
       */

      /* log if this occurs */
   }
#endif

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Start ADC compensation algorithm
* \param demod demod instance
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure: I2C error or failure to compensate
*
*/
static DRXStatus_t
ADCCompensationStart( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t devAddr               = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t      extAttr               = (pDRXKData_t) NULL;
   u16_t            setParamParameters[7] = {0};
   u16_t            cmdResult[15]         = {0};




   DRXKSCUCmd_t     cmdSCU =  {  /* command      */   0,
                                 /* parameterLen */   0,
                                 /* resultLen    */   0,
                                 /* *parameter   */   NULL,
                                 /* *result      */   NULL } ;
   extAttr = (pDRXKData_t)       demod->myExtAttr;
   devAddr = (pI2CDeviceAddr_t)  demod->myI2CDevAddr;

   /* Initialize ********/
   WR16( devAddr, ATV_COMM_EXEC__A, ATV_COMM_EXEC_ACTIVE);
   WR16( devAddr, IQM_COMM_EXEC__A, IQM_COMM_EXEC_B_ACTIVE);
   WR16( devAddr, ATV_TOP_CR_AMP_TH__A, 0);
   WR16( devAddr, ATV_TOP_CR_OVM_TH__A, 0);
   WR16( devAddr, ATV_TOP_CR_CONT__A, 0x9c);
   WR16( devAddr, ATV_TOP_STDBY__A, ATV_TOP_STDBY_SIF_STDBY_ACTIVE |
                                            ATV_TOP_STDBY_CVBS_STDBY_ACTIVE );
 
   /* Compensation is limited to 4 .. 12 MHz reference frequencies due to SIF
      output. Using 11.875MHz, which is in the passband of the internal
      highpass filter.
   */
   WR16( devAddr, ATV_TOP_COMP_FREQ__A,   159 );
   WR16( devAddr, ATV_TOP_PHSHIFT__A, 512);
   WR16( devAddr, ATV_TOP_COMP_AGC__A, 0 );
   WR16( devAddr, ATV_TOP_COMP_ACTIVE__A,
                                       ( ATV_TOP_COMP_ACTIVE_DAC2ADC_COMP |
                                         ATV_TOP_COMP_ACTIVE_TSTSIG2DAC_COMP) );
   WR16( devAddr, ATV_TOP_OVERRIDE_SFR__A, ATV_TOP_OVERRIDE_SFR_OVERRIDE);
   WR16( devAddr, ATV_TOP_SFR_VID_GAIN__A, 1280);

   WR16( devAddr, IQM_AF_AMUX__A, IQM_AF_AMUX_SIGNAL2LOWPASS);
   WR16( devAddr, IQM_AF_UPD_SEL__A, 0);
   WR16( devAddr, IQM_AF_INC_BYPASS__A, 1);
   WR16( devAddr, IQM_AF_DCF_BYPASS__A, IQM_AF_DCF_BYPASS_BYPASS);
   WR16( devAddr, IQM_AF_CMP_ACTIVE__A, 1);

   WR16( devAddr, IQM_AF_AGC_RF__A, 0x7fff);
   WR16( devAddr, IQM_AF_AGC_IF__A, 0x7fff);
   CHK_ERROR(DRXBSP_HST_Sleep(1)); /* Wait 1 ms to meet delays in hardware. */

   WR16( devAddr, IQM_FS_RATE_OFS_HI__A, 0x03b4);
   WR16( devAddr, IQM_FS_RATE_OFS_LO__A, 0xa761);
   WR16( devAddr, IQM_FS_ADJ_SEL__A, 3);

   WR16( devAddr, IQM_RC_RATE_OFS_LO__A, 0);
   WR16( devAddr, IQM_RC_RATE_OFS_HI__A, 0x20);
   WR16( devAddr, IQM_RC_ADJ_SEL__A, 0);
   WR16( devAddr, IQM_RC_STRETCH__A, 0xf);

   WR16( devAddr, IQM_RT_ROT_BP__A, 0x4);
   WR16( devAddr, IQM_RT_ACTIVE__A, ( IQM_RT_ACTIVE_ACTIVE_RT_ATV_FCR_ON |
                                      IQM_RT_ACTIVE_ACTIVE_CR_ATV_CR_ON ));


   /* Run algorithm ********/
   setParamParameters[1] = 0x41; /* compensation mode: auto, 1/64 */
   setParamParameters[2] = 440;  /* target amplitude (90%) */
   setParamParameters[3] = 20;   /* wait time lock phase */
   setParamParameters[4] = 1;    /* wait time compensation phase */
   setParamParameters[5] = 5;    /* wait time adjustment phase */
   setParamParameters[6] = 3;    /* max amplitude delta */
   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_TOP |
                         SCU_RAM_COMMAND_CMD_AUX_ADC_COMP_RESTART;
   cmdSCU.parameterLen = 7;
   cmdSCU.resultLen    = 15;
   cmdSCU.parameter    = setParamParameters;
   cmdSCU.result       = cmdResult;

      extAttr->adcCompPassed = FALSE;
   if ( SCUCommand( devAddr, &cmdSCU ) == DRX_STS_OK )
   {
      if ( cmdResult[14] != 1 )
   {
      extAttr->adcCompPassed = TRUE;
   }
   }

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/

/**
* \brief Run ADC compensation algorithm
* \param demod demod instance
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure: I2C error or failure to compensate
*
* Assuming demod is "stopped", e.g. no demod standard is active.
* Assuming ADC is synchronized.
* After an IQM reset this routine needs to be called again.
*
*/

static DRXStatus_t
ADCCompensation( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t devAddr    = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t      extAttr    = (pDRXKData_t) NULL;
   u8_t             retryCnt   = 0;
   u16_t            i          = 0;
   const u16_t      coefCnt    = 64;
   u16_t            adcState   = 0;

   devAddr = (pI2CDeviceAddr_t)  demod->myI2CDevAddr;
   extAttr = (pDRXKData_t) demod->myExtAttr;


   for(retryCnt = 0; retryCnt < DRXK_ADC_MAX_RETRIES; retryCnt++)
   {
      /* Compensation is finished, skip redoing it. */
      if(ADCCompensationStart(demod) == DRX_STS_OK)
      {
         break;
      }
   }

   /* Reset some registers  ********/
   WR16( devAddr, ATV_TOP_COMP_ACTIVE__A,
                                     ( ATV_TOP_COMP_ACTIVE_DAC2ADC_NORMAL |
                                       ATV_TOP_COMP_ACTIVE_TSTSIG2DAC_NORMAL) );
   WR16( devAddr, ATV_TOP_OVERRIDE_SFR__A, ATV_TOP_OVERRIDE_SFR_ACTIVE);
   WR16( devAddr, IQM_AF_CMP_ACTIVE__A, 0);


   /* Couldn't do compensation. */
   if( retryCnt == DRXK_ADC_MAX_RETRIES)
   {
      return DRX_STS_ERROR;
   }

   /* Read all the adc compensation coefficients and store */
   for (i = 0; i < coefCnt; i++)
   {
      RR16( devAddr, (IQM_AF_CMP_MEM0__A + i), &extAttr->adcCompCoef[i] );
   }


   RR16( devAddr, (IQM_AF_CMP_STATE__A ), &adcState );

   if ( !extAttr->adcCompPassed )
   {
      if (  (adcState & IQM_AF_CMP_STATE_STATE__M)  != 0 &&
            (adcState & IQM_AF_CMP_STATE_STATE__M)  != 3 )
      {
         /* clear the state to avoid bad compensation */
         adcState &= ~IQM_AF_CMP_STATE_STATE__M;

      }
   }

   extAttr->adcState = adcState;

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/**
* \brief Restore ADC compensation parameters
* \param demod demod instance
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure: I2C error or failure to compensate
*
*/
static DRXStatus_t
ADCCompensationRestoreParams( pDRXDemodInstance_t   demod )
{
   pDRXKData_t extAttr        = NULL;
   pI2CDeviceAddr_t devAddr   = NULL;
   u16_t i                    = 0;
   const u16_t coefCnt        = 64;

   extAttr = (pDRXKData_t)       demod->myExtAttr;
   devAddr = (pI2CDeviceAddr_t)  demod->myI2CDevAddr;

   for (i = 0; i < coefCnt; i++)
   {
      WR16( devAddr, (IQM_AF_CMP_MEM0__A + i), extAttr->adcCompCoef[i] );
   }

   WR16(devAddr, IQM_AF_CMP_STATE__A, extAttr->adcState);

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/

/**
* \brief Start MPEG TS output.
* \param demod   Pointer to demodulator instance.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
MPEGTSStart( pDRXDemodInstance_t   demod )
{
   pI2CDeviceAddr_t devAddr      = (pI2CDeviceAddr_t) NULL;
   u16_t            fecOcSncMode = 0;
   u16_t            fecOcIprMode = 0;

   devAddr = demod -> myI2CDevAddr;

   /* No longer suppress MCLK during absence of data */
   RR16( devAddr, FEC_OC_IPR_MODE__A, &fecOcIprMode );
   fecOcIprMode &= ~FEC_OC_IPR_MODE_MCLK_DIS_DAT_ABS__M;
   WR16( devAddr, FEC_OC_IPR_MODE__A, fecOcIprMode );

   /* Allow OC to sync again */
   RR16( devAddr, FEC_OC_SNC_MODE__A, &fecOcSncMode );
   fecOcSncMode &= ~FEC_OC_SNC_MODE_SHUTDOWN__M;
   WR16( devAddr, FEC_OC_SNC_MODE__A, fecOcSncMode );
   WR16( devAddr, FEC_OC_SNC_UNLOCK__A, 1 );

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Stop MPEG TS output.
* \param demod   Pointer to demodulator instance.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
MPEGTSStop( pDRXDemodInstance_t   demod )
{
   pI2CDeviceAddr_t devAddr      = (pI2CDeviceAddr_t) NULL;
   u16_t            fecOcSncMode = 0;
   u16_t            fecOcIprMode = 0;

   devAddr = demod -> myI2CDevAddr;

   /* Gracefull shutdown (byte boundaries) */
   RR16( devAddr, FEC_OC_SNC_MODE__A, &fecOcSncMode );
   fecOcSncMode |= FEC_OC_SNC_MODE_SHUTDOWN__M;
   WR16( devAddr, FEC_OC_SNC_MODE__A, fecOcSncMode );

   /* Suppress MCLK during absence of data */
   RR16( devAddr, FEC_OC_IPR_MODE__A, &fecOcIprMode );
   fecOcIprMode |= FEC_OC_IPR_MODE_MCLK_DIS_DAT_ABS__M;
   WR16( devAddr, FEC_OC_IPR_MODE__A, fecOcIprMode );

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/

/**
* \brief Init MPEG TS DTO's (standard independent constants only).
* \param demod    Pointer to demodulator instance.
* \param standard Standard that will be using MPEG TS.
* \return DRXStatus_t.
* \retval DRX_STS_OK     MPEG TS DTO's are configured.
* \retval DRX_STS_ERROR  I2C error;
*
*/
static DRXStatus_t
MPEGTSDtoInit( pI2CDeviceAddr_t devAddr )
{
   /* Rate integration settings */
   WR16 ( devAddr, FEC_OC_RCN_CTL_STEP_LO__A,  0x0000 );
   WR16 ( devAddr, FEC_OC_RCN_CTL_STEP_HI__A,  0x000C );
   WR16 ( devAddr, FEC_OC_RCN_GAIN__A,         0x000A );
   WR16 ( devAddr, FEC_OC_AVR_PARM_A__A,       0x0008 );
   WR16 ( devAddr, FEC_OC_AVR_PARM_B__A,       0x0006 );
   WR16 ( devAddr, FEC_OC_TMD_HI_MARGIN__A,    0x0680 );
   WR16 ( devAddr, FEC_OC_TMD_LO_MARGIN__A,    0x0080 );
   WR16 ( devAddr, FEC_OC_TMD_COUNT__A,        0x03F4 );

   /* Additional configuration */
   WR16( devAddr, FEC_OC_OCR_INVERT__A,        0 );
   WR16( devAddr, FEC_OC_SNC_LWM__A,           2 );
   WR16( devAddr, FEC_OC_SNC_HWM__A,          12 );

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/

/**
* \brief Configure MPEG TS DTO's.
* \param demod    Pointer to demodulator instance.
* \param standard Standard that will be using MPEG TS.
* \return DRXStatus_t.
* \retval DRX_STS_OK     MPEG TS DTO's are configured.
* \retval DRX_STS_ERROR  I2C error;
*
*/
static DRXStatus_t
MPEGTSDtoSetup( pDRXDemodInstance_t  demod ,
                DRXStandard_t        standard )
{
   pI2CDeviceAddr_t devAddr    = (pI2CDeviceAddr_t) (NULL);
   pDRXCommonAttr_t commonAttr = (pDRXCommonAttr_t) (NULL);
   u16_t fecOcRegMode       = 0;
   u16_t fecOcRegIprMode    = 0;
   u16_t fecOcDtoMode       = 0;
   u16_t fecOcFctMode       = 0;
   u16_t fecOcDtoPeriod     = 2;
   u16_t fecOcDtoBurstLen   = 188;
   u32_t fecOcRcnCtlRate    = 0;
   u16_t fecOcTmdMode       = 0;
   u16_t fecOcTmdIntUpdRate = 0;
   u32_t maxBitRate         = 0;

   devAddr    = demod -> myI2CDevAddr;
   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;

   /* Check insertion of the Reed-Solomon parity bytes */
   RR16 ( devAddr, FEC_OC_MODE__A      , &fecOcRegMode );
   RR16 ( devAddr, FEC_OC_IPR_MODE__A, &fecOcRegIprMode );
   fecOcRegMode    &= (~FEC_OC_MODE_PARITY__M);
   fecOcRegIprMode &= (~FEC_OC_IPR_MODE_MVAL_DIS_PAR__M);
   if ( commonAttr->mpegCfg.insertRSByte == TRUE )
   {
      /* enable parity symbol forward */
      fecOcRegMode    |= FEC_OC_MODE_PARITY__M;
      /* MVAL disable during parity bytes */
      fecOcRegIprMode |= FEC_OC_IPR_MODE_MVAL_DIS_PAR__M;
      /* TS burst length to 204 */
      fecOcDtoBurstLen = 204 ;
   }

   /* Check serial or parallel output */
   fecOcRegIprMode &= (~(FEC_OC_IPR_MODE_SERIAL__M));
   if ( commonAttr->mpegCfg.enableParallel == FALSE )
   {  /* MPEG data output is serial -> set ipr_mode[0] */
      fecOcRegIprMode |= FEC_OC_IPR_MODE_SERIAL__M;
   }

   /* Calculate max output rate */
   switch ( standard )
   {
      case DRX_STANDARD_DVBT:
         /* DVB-T, max = 31.67 Mb/s */
         maxBitRate      = 32000000UL;
         fecOcTmdMode    = 0x0003;
         fecOcRcnCtlRate = 0xC00000; /* Will be overwritten by ucode anyhow */
         break;
      case DRX_STANDARD_ITU_A: /* fallthrough */
      case DRX_STANDARD_ITU_C:
         /* 256QAM * max symb.rate */
         maxBitRate      = DRXK_QAM_SYMBOLRATE_MAX * 8;
         fecOcTmdMode    = 0x0004;
         fecOcRcnCtlRate = 0xD2B4EE; /* good for >63 Mb/s */
         break;
      default:
         return DRX_STS_ERROR;
         break;
   }
   /* Add RS byte margin */
   maxBitRate *= 11;
   maxBitRate /= 10;

   /* Configure DTO's */
   if ( commonAttr->mpegCfg.staticCLK == TRUE )   /* Static mode */
   {
      u32_t bitRate = 0;

      /* Rational DTO for MCLK source (static MCLK rate),
         Dynamic DTO for optimal grouping (avoid intra-packet gaps),
         DTO offset enable to sync TS burst with MSTRT */
      fecOcDtoMode = ( FEC_OC_DTO_MODE_DYNAMIC__M |
                       FEC_OC_DTO_MODE_OFFSET_ENABLE__M );
      fecOcFctMode = ( FEC_OC_FCT_MODE_RAT_ENA__M |
                       FEC_OC_FCT_MODE_VIRT_ENA__M );

      /* Check user defined bitrate */
      bitRate = commonAttr->mpegCfg.bitrate;
      if ( bitRate > 75900000UL )
      {  /* max is 75.9 Mb/s; clip value */
         bitRate = 75900000UL;
      }
      if ( commonAttr->mpegCfg.bitrate == 0 )
      {  /* Driver will select high enough bitrate to fit channel */
         bitRate = maxBitRate;
      }
      /* Rational DTO period:
               dto_period = ( Fsys / bitrate ) - 2

         Result should be floored, to make sure >= requested bitrate
      */
      fecOcDtoPeriod = (u16_t) ( ( commonAttr->sysClockFreq * 1000 )  /
                                 bitRate );
      if ( fecOcDtoPeriod <= 2 )
      {
         fecOcDtoPeriod = 0;
      }
      else
      {
         fecOcDtoPeriod -= 2;
      }
      fecOcTmdIntUpdRate = 8;
   }
   else
   {
      /* ( commonAttr->staticCLK == FALSE ) => dynamic mode */
      fecOcDtoMode = FEC_OC_DTO_MODE_DYNAMIC__M;
      fecOcFctMode = FEC_OC_FCT_MODE__PRE;
      fecOcTmdIntUpdRate = 5;
   }

   /* Write appropriate registers with requested configuration */
   WR16 ( devAddr, FEC_OC_DTO_BURST_LEN__A,   fecOcDtoBurstLen );
   WR16 ( devAddr, FEC_OC_DTO_PERIOD__A,      fecOcDtoPeriod);
   WR16 ( devAddr, FEC_OC_DTO_MODE__A,        fecOcDtoMode );
   WR16 ( devAddr, FEC_OC_FCT_MODE__A,        fecOcFctMode );
   WR16 ( devAddr, FEC_OC_MODE__A,            fecOcRegMode );
   WR16 ( devAddr, FEC_OC_IPR_MODE__A,        fecOcRegIprMode );

   /* Rate integration settings */
   WR32 ( devAddr, FEC_OC_RCN_CTL_RATE_LO__A,  fecOcRcnCtlRate );
   WR16 ( devAddr, FEC_OC_TMD_INT_UPD_RATE__A, fecOcTmdIntUpdRate );
   WR16 ( devAddr, FEC_OC_TMD_MODE__A,         fecOcTmdMode );

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Configure MPEG TS pins.
* \param demod    Pointer to demodulator instance.
* \return DRXStatus_t.
* \retval DRX_STS_OK     MPEG TS pins are configured.
* \retval DRX_STS_ERROR  I2C error;
*
*/
static DRXStatus_t
MPEGTSConfigurePins( pDRXDemodInstance_t  demod,
                     Bool_t               mpegEnable )
{
   pI2CDeviceAddr_t devAddr    = (pI2CDeviceAddr_t) (NULL);
   pDRXKData_t      extAttr    = (pDRXKData_t)      (NULL);
   pDRXCommonAttr_t commonAttr = (pDRXCommonAttr_t) (NULL);
   u16_t sioPdrMclkCfg      = 0;
   u16_t sioPdrMdxCfg       = 0;

   devAddr    = demod -> myI2CDevAddr;
   extAttr    = (pDRXKData_t)      demod -> myExtAttr;
   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;

   /* stop lock indicator process */
   WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_DISABLE );

   /*  MPEG TS pad configuration */
   WR16 ( devAddr, SIO_TOP_COMM_KEY__A, SIO_TOP_COMM_KEY_KEY );

   if( mpegEnable == FALSE )
   {
      /*  Set MPEG TS pads to inputmode */
      WR16 ( devAddr, SIO_PDR_MSTRT_CFG__A    , 0x0000);
      WR16 ( devAddr, SIO_PDR_MERR_CFG__A     , 0x0000);
      WR16 ( devAddr, SIO_PDR_MCLK_CFG__A     , 0x0000);
      WR16 ( devAddr, SIO_PDR_MVAL_CFG__A     , 0x0000);
      WR16 ( devAddr, SIO_PDR_MD0_CFG__A      , 0x0000);
      WR16 ( devAddr, SIO_PDR_MD1_CFG__A      , 0x0000);
      WR16 ( devAddr, SIO_PDR_MD2_CFG__A      , 0x0000);
      WR16 ( devAddr, SIO_PDR_MD3_CFG__A      , 0x0000);
      WR16 ( devAddr, SIO_PDR_MD4_CFG__A      , 0x0000);
      WR16 ( devAddr, SIO_PDR_MD5_CFG__A      , 0x0000);
      WR16 ( devAddr, SIO_PDR_MD6_CFG__A      , 0x0000);
      WR16 ( devAddr, SIO_PDR_MD7_CFG__A      , 0x0000);
   }
   else
   {
      /* Enable MPEG output */
      WR16 ( devAddr, SIO_PDR_MSTRT_CFG__A,  0x0013);
      WR16 ( devAddr, SIO_PDR_MERR_CFG__A,   0x0013);
      WR16 ( devAddr, SIO_PDR_MVAL_CFG__A,   0x0013);
      sioPdrMclkCfg = ( ( DRXK_MPEG_OUTPUT_CLK_DRIVE_STRENGTH <<
                          SIO_PDR_MCLK_CFG_DRIVE__B) |
                        0x0003 );
      if ( commonAttr->mpegCfg.enableParallel == TRUE )
      {
         sioPdrMdxCfg  = ( ( DRXK_MPEG_PARALLEL_OUTPUT_PIN_DRIVE_STRENGTH <<
                             SIO_PDR_MD0_CFG_DRIVE__B ) |
                           0x0003 );
         /* paralel -> enable MD1 to MD7 */
         WR16 ( devAddr, SIO_PDR_MD1_CFG__A, sioPdrMdxCfg);
         WR16 ( devAddr, SIO_PDR_MD2_CFG__A, sioPdrMdxCfg);
         WR16 ( devAddr, SIO_PDR_MD3_CFG__A, sioPdrMdxCfg);
         WR16 ( devAddr, SIO_PDR_MD4_CFG__A, sioPdrMdxCfg);
         WR16 ( devAddr, SIO_PDR_MD5_CFG__A, sioPdrMdxCfg);
         WR16 ( devAddr, SIO_PDR_MD6_CFG__A, sioPdrMdxCfg);
         WR16 ( devAddr, SIO_PDR_MD7_CFG__A, sioPdrMdxCfg);
      }
      else
      {
         sioPdrMdxCfg  = ( ( DRXK_MPEG_SERIAL_OUTPUT_PIN_DRIVE_STRENGTH <<
                             SIO_PDR_MD0_CFG_DRIVE__B ) |
                           0x0003 );
         /* serial -> disable MD1 to MD7 */
         WR16 ( devAddr, SIO_PDR_MD1_CFG__A, 0x0000);
         WR16 ( devAddr, SIO_PDR_MD2_CFG__A, 0x0000);
         WR16 ( devAddr, SIO_PDR_MD3_CFG__A, 0x0000);
         WR16 ( devAddr, SIO_PDR_MD4_CFG__A, 0x0000);
         WR16 ( devAddr, SIO_PDR_MD5_CFG__A, 0x0000);
         WR16 ( devAddr, SIO_PDR_MD6_CFG__A, 0x0000);
         WR16 ( devAddr, SIO_PDR_MD7_CFG__A, 0x0000);
      }
      WR16 ( devAddr, SIO_PDR_MCLK_CFG__A,   sioPdrMclkCfg);
      WR16 ( devAddr, SIO_PDR_MD0_CFG__A,    sioPdrMdxCfg);
   }

   /*  Enable MB output over MPEG pads and ctl input */
   WR16 ( devAddr, SIO_PDR_MON_CFG__A,    0x0000);
   /*  Write nomagic word to enable pdr reg write */
   WR16 ( devAddr, SIO_TOP_COMM_KEY__A,   0x0000);

   if ( extAttr->uioSawSwMode ==
        DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR )
   {
      /* restart lock indicator process */
      WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_ENABLE );
   }

   return DRX_STS_OK;

rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \fn DRXStatus_t SetMPEGStartWidth()
* \brief Set MPEG start width.
* \param devmod  Pointer to demodulator instance.
* \return DRXStatus_t.
*
* This routine should be called during a set channel of QAM/VSB
*
*/
static DRXStatus_t
MPEGTSSetStartWidth ( pDRXDemodInstance_t   demod )
{
   pI2CDeviceAddr_t devAddr      = (pI2CDeviceAddr_t)NULL;
   pDRXCommonAttr_t commAttr     = (pDRXCommonAttr_t)NULL;
   u16_t            fecOcCommMb  = 0;

   devAddr = demod->myI2CDevAddr;
   commAttr = demod->myCommonAttr;

   if ( commAttr->mpegCfg.enableParallel == FALSE )
   {
      RR16 ( devAddr, FEC_OC_COMM_MB__A, &fecOcCommMb );
      /* clear MSTRT clock cycles mask*/
      fecOcCommMb &= ~FEC_OC_COMM_MB_CTL_ON;
      if ( commAttr->mpegCfg.widthSTR == DRX_MPEG_STR_WIDTH_8 )
      {
         fecOcCommMb |= FEC_OC_COMM_MB_CTL_ON;
      }
      WR16 ( devAddr, FEC_OC_COMM_MB__A, fecOcCommMb);
   }

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Configure polarity of MPEG TS signals.
* \param demod    Pointer to demodulator instance.
* \return DRXStatus_t.
* \retval DRX_STS_OK     MPEG TS polarity is configured.
* \retval DRX_STS_ERROR  I2C error;
*
*/
static DRXStatus_t
MPEGTSConfigurePolarity( pDRXDemodInstance_t  demod )
{
   pI2CDeviceAddr_t devAddr    = (pI2CDeviceAddr_t) (NULL);
   pDRXCommonAttr_t commonAttr = (pDRXCommonAttr_t) (NULL);
   u16_t fecOcRegIprInvert  = 0;

   /* Data mask for the output data byte */
   u16_t InvertDataMask = FEC_OC_IPR_INVERT_MD7__M | FEC_OC_IPR_INVERT_MD6__M |
                          FEC_OC_IPR_INVERT_MD5__M | FEC_OC_IPR_INVERT_MD4__M |
                          FEC_OC_IPR_INVERT_MD3__M | FEC_OC_IPR_INVERT_MD2__M |
                          FEC_OC_IPR_INVERT_MD1__M | FEC_OC_IPR_INVERT_MD0__M;
   devAddr    = demod -> myI2CDevAddr;
   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;

   /* Control selective inversion of output bits */
   fecOcRegIprInvert &= (~(InvertDataMask));
   if ( commonAttr->mpegCfg.invertDATA == TRUE )
   {
     fecOcRegIprInvert |= InvertDataMask;
   }
   fecOcRegIprInvert &= (~(FEC_OC_IPR_INVERT_MERR__M));
   if ( commonAttr->mpegCfg.invertERR == TRUE )
   {
     fecOcRegIprInvert |= FEC_OC_IPR_INVERT_MERR__M;
   }
   fecOcRegIprInvert &= (~(FEC_OC_IPR_INVERT_MSTRT__M));
   if ( commonAttr->mpegCfg.invertSTR == TRUE )
   {
     fecOcRegIprInvert |= FEC_OC_IPR_INVERT_MSTRT__M;
   }
   fecOcRegIprInvert &= (~(FEC_OC_IPR_INVERT_MVAL__M));
   if ( commonAttr->mpegCfg.invertVAL == TRUE )
   {
     fecOcRegIprInvert |= FEC_OC_IPR_INVERT_MVAL__M;
   }
   fecOcRegIprInvert &= (~(FEC_OC_IPR_INVERT_MCLK__M));
   if ( commonAttr->mpegCfg.invertCLK == TRUE )
   {
     fecOcRegIprInvert |= FEC_OC_IPR_INVERT_MCLK__M;
   }
   WR16 ( devAddr, FEC_OC_IPR_INVERT__A,      fecOcRegIprInvert );

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/

/**
* \brief Enable MPEG TS output hardware.
* \param demod    Pointer to demodulator instance.
* \param standard Standard that will be using MPEG TS.
* \return DRXStatus_t.
* \retval DRX_STS_OK MPEG TS ouput is configured.
* \retval DRX_STS_ERROR I2C error;
*
* In case of staticCLK == TRUE:
* If bitrate ==0 the drxdriver will suggest a bitrate depending on the standard
* and channel parameters.
* The drxdriver will always attempt to setup the start rate near the expected
* bitrate to shorten the lock-in period.
*
*/
static DRXStatus_t
MPEGTSEnable( pDRXDemodInstance_t  demod ,
              DRXStandard_t        standard )
{
   /* setup polarity */
   CHK_ERROR( MPEGTSConfigurePolarity( demod ) );

   /* setup DTO's */
   CHK_ERROR( MPEGTSDtoSetup( demod , standard ) );

   /* set MSTR Width */
   CHK_ERROR ( MPEGTSSetStartWidth ( demod ));

   /* set pin configuration */
   CHK_ERROR( MPEGTSConfigurePins( demod, TRUE ) );

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Disable MPEG TS output hardware.
* \param demod  Pointer to demodulator instance.
* \return DRXStatus_t.
* \retval DRX_STS_OK MPEG TS ouput is configured.
* \retval DRX_STS_ERROR I2C error.
*
* Configure output pins such that they consume the least amount of power.
*
*/
static DRXStatus_t
MPEGTSDisable( pDRXDemodInstance_t   demod )
{
   pI2CDeviceAddr_t devAddr    = (pI2CDeviceAddr_t) (NULL);
   pDRXKData_t      extAttr    = (pDRXKData_t)      (NULL);

   devAddr    = demod -> myI2CDevAddr;
   extAttr    = (pDRXKData_t)      demod -> myExtAttr;

   CHK_ERROR( MPEGTSConfigurePins( demod, FALSE ) );

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Configure MPEG TS output configuration in hardware.
* \param demod  Pointer to demodulator instance.
* \return DRXStatus_t.
* \retval DRX_STS_OK MPEG TS ouput is configured.
* \retval DRX_STS_ERROR I2C error; current constellation or standard not
*                       supported
*
*/
static DRXStatus_t
MPEGTSConfigureHW( pDRXDemodInstance_t   demod, DRXStandard_t standard )
{
   pDRXKData_t      extAttr    = (pDRXKData_t)      (NULL);

   extAttr    = (pDRXKData_t)      demod->myExtAttr;

   if ( extAttr->channelActive == TRUE )
   {
      /* Stop at a TS boundary */
      CHK_ERROR( MPEGTSStop( demod ) );
   }

   if ( demod->myCommonAttr->mpegCfg.enableMPEGOutput == TRUE )
   {
      /* Enable MPEG pins */
      CHK_ERROR( MPEGTSEnable( demod, standard ) );
   }
   else
   {
      /* Disable MPEG pins */
      CHK_ERROR( MPEGTSDisable( demod ) );
   }

   if ( extAttr->channelActive == TRUE )
   {
      /* Start at a TS boundary / keep MPEG lock during TS disable */
      CHK_ERROR( MPEGTSStart( demod ) );
   }

   return DRX_STS_OK;

rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Store MPEG output configuration in demod structure.
* \param cfgDataDest Pointer to mpeg output configuration, destination.
* \param cfgData     Pointer to mpeg output configuration, source.
* \return None.
*
*/
static void
MPEGTSCopyCfg( pDRXCfgMPEGOutput_t   cfgDataDest,
               pDRXCfgMPEGOutput_t   cfgDataSrc )
{
   cfgDataDest->enableMPEGOutput = cfgDataSrc->enableMPEGOutput;
   cfgDataDest->insertRSByte     = cfgDataSrc->insertRSByte;
   cfgDataDest->enableParallel   = cfgDataSrc->enableParallel;
   cfgDataDest->invertDATA       = cfgDataSrc->invertDATA;
   cfgDataDest->invertERR        = cfgDataSrc->invertERR;
   cfgDataDest->invertSTR        = cfgDataSrc->invertSTR;
   cfgDataDest->invertVAL        = cfgDataSrc->invertVAL;
   cfgDataDest->invertCLK        = cfgDataSrc->invertCLK;
   cfgDataDest->staticCLK        = cfgDataSrc->staticCLK;
   cfgDataDest->bitrate          = cfgDataSrc->bitrate;
   cfgDataDest->widthSTR         = cfgDataSrc->widthSTR;
}

/*============================================================================*/

/**
* \brief Check for DRXK A1 with patch microcode.
* \param demod  Pointer to demodulator instance.
* \return Bool_t.
* \retval TRUE Current device used is A1 and using ucode from upload.
* \retval FALSE Otherwise.
*
* Assuming that bootloader has been used to upload initial microcode.
* Assuming that device is not "hot-swapped" after call to DRX_Open.
* Patch may also mean a complete upload.
* Bottom-line is that we detect latest greatest microcode for this mfx.
*
*/
static Bool_t
IsA1WithPatchCode( pDRXDemodInstance_t   demod)
{
   pDRXKData_t extAttr       = (pDRXKData_t)(NULL);
   Bool_t      isA1WithPatch = FALSE;

   extAttr    = (pDRXKData_t)  demod -> myExtAttr;

   if (( extAttr->mfx == 0 ) && (extAttr->ucodeUploaded == TRUE ))
   {
      isA1WithPatch = TRUE;
   }

   return (isA1WithPatch);
}

/*============================================================================*/

/**
* \brief Check for DRXK A2 with patch microcode.
* \param demod  Pointer to demodulator instance.
* \return Bool_t.
* \retval TRUE Current device used is A1 and using ucode from ROM.
* \retval FALSE Otherwise.
*
* Assuming that bootloader has been used to upload microcode.
* Assuming that device is not "hot-swapped" after call to DRX_Open.
*
*/
#if 1 /* re-install when needed again */
static Bool_t
IsA2WithPatchCode( pDRXDemodInstance_t   demod)
{
   pDRXKData_t extAttr       = (pDRXKData_t)(NULL);
   Bool_t      isA2WithPatch = FALSE;

   extAttr    = (pDRXKData_t)  demod -> myExtAttr;

   if (( extAttr->mfx == 2 ) && (extAttr->ucodeUploaded == TRUE ))
   {
      isA2WithPatch = TRUE;
   }

   return (isA2WithPatch);
}
#endif

/*============================================================================*/

/**
* \brief Check for DRXK A3 with microcode from ROM
* \param demod  Pointer to demodulator instance.
* \return Bool_t.
* \retval TRUE Current device used is A1 and using ucode from ROM.
* \retval FALSE Otherwise.
*
* Assuming that bootloader has been used to upload microcode.
* Assuming that device is not "hot-swapped" after call to DRX_Open.
*
*/
static Bool_t
IsA3WithRomCode( pDRXDemodInstance_t   demod)
{
   pDRXKData_t extAttr     = (pDRXKData_t)(NULL);
   Bool_t      isA3WithRom = FALSE;

   extAttr    = (pDRXKData_t)  demod -> myExtAttr;

   if (( extAttr->mfx == 3 ) && (extAttr->ucodeUploaded == FALSE ))
   {
      isA3WithRom = TRUE;
   }

   return (isA3WithRom);
}

/*============================================================================*/

/**
* \brief Check for DRXK A3 with patch microcode.
* \param demod  Pointer to demodulator instance.
* \return Bool_t.
* \retval TRUE Current device used is A1 and using ucode from ROM.
* \retval FALSE Otherwise.
*
* Assuming that bootloader has been used to upload microcode.
* Assuming that device is not "hot-swapped" after call to DRX_Open.
*
*/
#if 0
static Bool_t
IsA3WithPatchCode( pDRXDemodInstance_t   demod)
{
   pDRXKData_t extAttr       = (pDRXKData_t)(NULL);
   Bool_t      isA3WithPatch = FALSE;

   extAttr    = (pDRXKData_t)  demod -> myExtAttr;

   if (( extAttr->mfx == 3 ) && (extAttr->ucodeUploaded == TRUE ))
   {
      isA3WithPatch = TRUE;
   }

   return (isA3WithPatch);
}
#endif

/* -------------------------------------------------------------------------- */

/**
* \brief set configuration of pin-safe mode
* \param demod instance of demodulator.
* \param enable boolean; TRUE: activate pin-safe mode, FALSE: de-activate p.s.m.
* \return DRXStatus_t.
*/
static DRXStatus_t
CtrlSetCfgPdrSafeMode ( pDRXDemodInstance_t demod,
                        pBool_t             enable )
{
   pDRXKData_t      extAttr    = (pDRXKData_t)      NULL;
   pI2CDeviceAddr_t devAddr    = (pI2CDeviceAddr_t) NULL;
   pDRXCommonAttr_t commonAttr = (pDRXCommonAttr_t) NULL;

   /* check arguments */
   if ( enable == NULL)
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr    = demod->myI2CDevAddr;
   extAttr    = (pDRXKData_t) demod->myExtAttr;
   commonAttr = demod->myCommonAttr;

   /* stop lock indicator process */
   WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_DISABLE );

   /*  Write magic word to enable pdr reg write  */
   WR16( devAddr, SIO_TOP_COMM_KEY__A, SIO_TOP_COMM_KEY_KEY );

   if ( *enable == TRUE )
   {
      Bool_t bridgeEnabled = FALSE;

      /* MPEG pins to input */
      WR16 ( devAddr, SIO_PDR_MSTRT_CFG__A, 0x0000 );
      WR16 ( devAddr, SIO_PDR_MERR_CFG__A,  0x0000 );
      WR16 ( devAddr, SIO_PDR_MCLK_CFG__A,  0x0000 );
      WR16 ( devAddr, SIO_PDR_MVAL_CFG__A,  0x0000 );
      WR16 ( devAddr, SIO_PDR_MD0_CFG__A,   0x0000 );
      WR16 ( devAddr, SIO_PDR_MD1_CFG__A,   0x0000 );
      WR16 ( devAddr, SIO_PDR_MD2_CFG__A,   0x0000 );
      WR16 ( devAddr, SIO_PDR_MD3_CFG__A,   0x0000 );
      WR16 ( devAddr, SIO_PDR_MD4_CFG__A,   0x0000 );
      WR16 ( devAddr, SIO_PDR_MD5_CFG__A,   0x0000 );
      WR16 ( devAddr, SIO_PDR_MD6_CFG__A,   0x0000 );
      WR16 ( devAddr, SIO_PDR_MD7_CFG__A,   0x0000 );

      /* PD_I2C_SDA2 Bridge off, Port2 Inactive
         PD_I2C_SCL2 Bridge off, Port2 Inactive */
      CHK_ERROR( CtrlI2CBridge( demod, &bridgeEnabled ) );
      WR16 ( devAddr, SIO_PDR_I2C_SDA2_CFG__A, 0x0000 );
      WR16 ( devAddr, SIO_PDR_I2C_SCL2_CFG__A, 0x0000 );

      /*  PD_GPIO     Store and set to input
          PD_VSYNC    Store and set to input
          PD_SMA_RX   Store and set to input
          PD_SMA_TX   Store and set to input */
      RR16 ( devAddr, SIO_PDR_GPIO_CFG__A,   &extAttr->pdrSafeRestoreValGpio  );
      RR16 ( devAddr, SIO_PDR_VSYNC_CFG__A,  &extAttr->pdrSafeRestoreValVSync );
      RR16 ( devAddr, SIO_PDR_SMA_RX_CFG__A, &extAttr->pdrSafeRestoreValSmaRx );
      RR16 ( devAddr, SIO_PDR_SMA_TX_CFG__A, &extAttr->pdrSafeRestoreValSmaTx );
      WR16 ( devAddr, SIO_PDR_GPIO_CFG__A,   0x0000 );
      WR16 ( devAddr, SIO_PDR_VSYNC_CFG__A,  0x0000 );
      WR16 ( devAddr, SIO_PDR_SMA_RX_CFG__A, 0x0000 );
      WR16 ( devAddr, SIO_PDR_SMA_TX_CFG__A, 0x0000 );

      /*  PD_RF_AGC   Analog DAC outputs, cannot be set to input or tristate!
          PD_IF_AGC   Analog DAC outputs, cannot be set to input or tristate! */
      CHK_ERROR( IQMSetAf ( demod, FALSE ) );

      /*  PD_CVBS     Analog DAC output, standby mode
          PD_SIF      Analog DAC output, standby mode */
      WR16 ( devAddr, ATV_TOP_STDBY__A, ( ATV_TOP_STDBY_SIF_STDBY_STANDBY  |
                                          ATV_TOP_STDBY_CVBS_STDBY_STANDBY ) );

      /*  PD_I2S_CL   Input
          PD_I2S_DA   Input
          PD_I2S_WS   Input */
      WR16 ( devAddr, SIO_PDR_I2S_CL_CFG__A, 0x0000 );
      WR16 ( devAddr, SIO_PDR_I2S_DA_CFG__A, 0x0000 );
      WR16 ( devAddr, SIO_PDR_I2S_WS_CFG__A, 0x0000 );
   }
   else
   {
      /* No need to restore MPEG pins;
         is done in SetStandard/SetChannel */

      /* PD_I2C_SDA2 Port2 active
         PD_I2C_SCL2 Port2 active */
      WR16 ( devAddr, SIO_PDR_I2C_SDA2_CFG__A, SIO_PDR_I2C_SDA2_CFG__PRE );
      WR16 ( devAddr, SIO_PDR_I2C_SCL2_CFG__A, SIO_PDR_I2C_SCL2_CFG__PRE );

      /*  PD_GPIO     Restore
          PD_VSYNC    Restore
          PD_SMA_RX   Restore
          PD_SMA_TX   Restore */
      WR16 ( devAddr, SIO_PDR_GPIO_CFG__A,   extAttr->pdrSafeRestoreValGpio  );
      WR16 ( devAddr, SIO_PDR_VSYNC_CFG__A,  extAttr->pdrSafeRestoreValVSync );
      WR16 ( devAddr, SIO_PDR_SMA_RX_CFG__A, extAttr->pdrSafeRestoreValSmaRx );
      WR16 ( devAddr, SIO_PDR_SMA_TX_CFG__A, extAttr->pdrSafeRestoreValSmaTx );

      /*  PD_RF_AGC, PD_IF_AGC
          No need to restore; will be restored in SetStandard/SetChannel */

      /*  PD_CVBS, PD_SIF
          No need to restore; will be restored in SetStandard/SetChannel */

      /*  PD_I2S_CL, PD_I2S_DA, PD_I2S_WS
          Should be restored via DRX_CTRL_SET_AUD */
   }

   /*  Write magic word to disable pdr reg write */
   WR16 ( devAddr, SIO_TOP_COMM_KEY__A, 0x0000 );
   extAttr->pdrSafeMode = *enable;

   if ( extAttr->uioSawSwMode ==
        DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR )
   {
      /* restart lock indicator process */
      WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_ENABLE );
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/* -------------------------------------------------------------------------- */

/**
* \brief get configuration of pin-safe mode
* \param demod instance of demodulator.
* \param enable boolean indicating whether pin-safe mode is active
* \return DRXStatus_t.
*/
static DRXStatus_t
CtrlGetCfgPdrSafeMode ( pDRXDemodInstance_t demod,
                        pBool_t             enabled )
{
   pDRXKData_t extAttr = (pDRXKData_t) NULL;

   /* check arguments */
   if ( enabled == NULL)
   {
      return DRX_STS_INVALID_ARG;
   }

   extAttr  = (pDRXKData_t) demod->myExtAttr;
   *enabled = extAttr->pdrSafeMode;

   return DRX_STS_OK;
}

/*============================================================================*/
/*==                 END OF AUXILIARY FUNCTIONS                             ==*/
/*============================================================================*/

/*============================================================================*/
/*==                       AGC RELATED FUNCTIONS                            ==*/
/*============================================================================*/

/**
* \brief Set Pre-saw reference.
* \param demod demod instance
* \param pu16_t
* \return DRXStatus_t.
*
* Check arguments
* Dispatch handling to standard specific function.
*
*/
static DRXStatus_t
AGCCtrlSetCfgPreSaw (   pDRXDemodInstance_t demod,
                   pDRXKCfgPreSaw_t    preSaw )
{
   pI2CDeviceAddr_t devAddr = NULL;
   pDRXKData_t      extAttr = NULL;

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* check arguments */
   if ( ( preSaw == NULL ) ||
        ( preSaw->reference > IQM_AF_PDREF__M ) )
   {
      return DRX_STS_INVALID_ARG;
   }
   switch ( preSaw->standard){
      case DRX_STANDARD_DVBT:          /* fallthrough */
      case DRX_STANDARD_ITU_A:         /* fallthrough */
      case DRX_STANDARD_ITU_C:         /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_BG : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP : /* fallthrough */
      case DRX_STANDARD_NTSC:          /* fallthrough */
      case DRX_STANDARD_FM:
         /* Do nothing */
         break;
      default:
         return DRX_STS_INVALID_ARG;
   }

   /* only allow change if standard is currently active */
   if ( ( extAttr->standard == preSaw->standard ) ||
        ( DRXK_ISATVSTD( extAttr->standard ) &&
          DRXK_ISATVSTD( preSaw->standard  ))  ||
        ( DRXK_ISQAMSTD( extAttr->standard ) &&
          DRXK_ISQAMSTD( preSaw->standard  )) )
   {
      WR16( devAddr, IQM_AF_PDREF__A  , preSaw->reference);
      /* store new pre-saw setting (until a next preset overrides it) */
      extAttr->presets.preSawCfg = *preSaw;
   }
   else
   {
      return ( DRX_STS_INVALID_ARG );
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Get Pre-saw reference setting.
* \param demod demod instance
* \param pu16_t
* \return DRXStatus_t.
*
* Check arguments
* Dispatch handling to standard specific function.
*
*/
static DRXStatus_t
AGCCtrlGetCfgPreSaw (   pDRXDemodInstance_t demod,
                   pDRXKCfgPreSaw_t    preSaw )
{
   pI2CDeviceAddr_t devAddr = NULL;
   pDRXKData_t      extAttr = NULL;

   /* check arguments */
   if ( preSaw == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   *preSaw = extAttr->presets.preSawCfg;

   return DRX_STS_OK;
}

/* -------------------------------------------------------------------------- */

/**
* \brief Initialize AGC for all standards.
* \param demod instance of demodulator.
* \return DRXStatus_t.
* \retval DRX_STS_OK      AGC algorithms initialized
* \retval DRX_STS_ERROR   failure; I2C error, current standard not supported
*/
static DRXStatus_t
AGCInit ( pDRXDemodInstance_t demod,
          Bool_t              atomic )
{
   pI2CDeviceAddr_t    devAddr            = NULL;
   pDRXCommonAttr_t    commonAttr         = NULL;
   pDRXKData_t         extAttr            = NULL;
   pDRXKPresets_t      presets            = NULL;

   u16_t               ingainTgt          = 0;
   u16_t               ingainTgtMin       = 0;
   u16_t               ingainTgtMax       = 0;
   u16_t               clpCyclen          = 0;
   u16_t               clpSumMin          = 0;
   u16_t               clpDirTo           = 0;
   u16_t               snsSumMin          = 0;
   u16_t               snsSumMax          = 0;
   u16_t               clpSumMax          = 0;
   u16_t               snsDirTo           = 0;
   u16_t               kiInnergainMin     = 0;
   u16_t               ifIaccuHiTgt       = 0;
   u16_t               ifIaccuHiTgtMin    = 0;
   u16_t               ifIaccuHiTgtMax    = 0;
   u16_t               data               = 0;
   u16_t               atvKiChangeTh      = 0;
   u16_t               fastClpCtrlDelay   = 0;
   u16_t               clpCtrlMode        = 0;


   DRXWriteReg16Func_t ScuWr16            = NULL;
   DRXReadReg16Func_t  ScuRr16            = NULL;

   devAddr     = demod -> myI2CDevAddr;
   commonAttr  = (pDRXCommonAttr_t) demod -> myCommonAttr;
   extAttr     = (pDRXKData_t)demod->myExtAttr;
   presets     = &extAttr->presets;

   /* Select appropriate register access */
   if (atomic)
   {
      ScuRr16 = DRXK_DAP_SCU_AtomicReadReg16;
      ScuWr16 = DRXK_DAP_SCU_AtomicWriteReg16;
   }
   else
   {
      ScuRr16 = DRXK_DAP.readReg16Func;
      ScuWr16 = DRXK_DAP.writeReg16Func;
   }

   /* Common settings */
   snsSumMax       = 1023;
   ifIaccuHiTgtMin = 2047;
   clpCyclen       = 500;
   clpSumMax       = 1023;

   /* Standard specific settings */
   switch (extAttr->standard)
   {
      case DRX_STANDARD_DVBT:
         /* AGCInit() not available for DVBT; init done in microcode */
         return ( DRX_STS_ERROR );

      case DRX_STANDARD_ITU_A:
      case DRX_STANDARD_ITU_B:
      case DRX_STANDARD_ITU_C:
         clpSumMin            = 8;
         clpDirTo             = (u16_t) -9;
         clpCtrlMode          = 0;
         snsSumMin            = 8;
         snsDirTo             = (u16_t)-9;
         kiInnergainMin       = (u16_t)-1030;
         break;

      case DRX_STANDARD_FM:
       case DRX_STANDARD_NTSC:
      case DRX_STANDARD_PAL_SECAM_BG:
      case DRX_STANDARD_PAL_SECAM_DK:
      case DRX_STANDARD_PAL_SECAM_I :
         kiInnergainMin = (u16_t) -32768;
         atvKiChangeTh  = 20;
         clpCtrlMode    = 1;
         break;

      case DRX_STANDARD_PAL_SECAM_L :
      case DRX_STANDARD_PAL_SECAM_LP:
         kiInnergainMin = (u16_t) -32768;
         atvKiChangeTh  = 40;
         clpCtrlMode    = 1;
         break;

      default:
         return ( DRX_STS_ERROR );
   }

   /* Preset defined settings */
   ifIaccuHiTgtMax   = presets->rfAgcCfg.top;
   ifIaccuHiTgt      = presets->rfAgcCfg.top;
   ingainTgtMin      = presets->ifAgcCfg.top;
   ingainTgt         = presets->ifAgcCfg.top;
   ingainTgtMax      = presets->agcIngainTgtMax;
   fastClpCtrlDelay  = presets->agcFastClipCtrlDelay;


   /* Write values to registers */
   CHK_ERROR((*ScuWr16)(   devAddr, SCU_RAM_AGC_FAST_CLP_CTRL_DELAY__A,
                           fastClpCtrlDelay, 0 ));

   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CLP_CTRL_MODE__A, clpCtrlMode,
                           0 ) );

   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_INGAIN_TGT__A,     ingainTgt,
                           0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_INGAIN_TGT_MIN__A, ingainTgtMin,
                           0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_INGAIN_TGT_MAX__A, ingainTgtMax,
                           0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_IF_IACCU_HI_TGT_MIN__A,
                           ifIaccuHiTgtMin, 0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_IF_IACCU_HI_TGT_MAX__A,
                           ifIaccuHiTgtMax,  0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_IF_IACCU_HI__A, 0, 0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_IF_IACCU_LO__A, 0, 0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_RF_IACCU_HI__A, 0, 0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_RF_IACCU_LO__A, 0, 0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CLP_SUM_MAX__A, clpSumMax, 0 ) );

   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_SNS_SUM_MAX__A, snsSumMax, 0 ) );


   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_KI_INNERGAIN_MIN__A,
                           kiInnergainMin, 0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_IF_IACCU_HI_TGT__A,
                           ifIaccuHiTgt,  0 ) );
   CHK_ERROR((*ScuWr16)( devAddr,
                            SCU_RAM_AGC_CLP_CYCLEN__A, clpCyclen, 0 ) );

   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_RF_SNS_DEV_MAX__A,  1023, 0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_RF_SNS_DEV_MIN__A,
                                                         (u16_t) -1023, 0 ) );
   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_FAST_SNS_CTRL_DELAY__A, 50,
                                                                        0 ) );

   /* Write QAM specific settings */
   if ( DRXK_ISQAMSTD( extAttr->standard ) )
   {
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_KI_MAXMINGAIN_TH__A,   20, 0));
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CLP_SUM_MIN__A, clpSumMin, 0));
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_SNS_SUM_MIN__A, snsSumMin, 0));
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CLP_DIR_TO__A,  clpDirTo,  0));
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_SNS_DIR_TO__A,  snsDirTo,  0));
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_KI_MINGAIN__A,  0x7fff, 0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_KI_MAXGAIN__A,  0x0,    0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_KI_MIN__A,      0x0117, 0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_KI_MAX__A,      0x0657, 0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CLP_SUM__A,     0,      0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CLP_CYCCNT__A,  0,      0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CLP_DIR_WD__A,  0,      0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CLP_DIR_STP__A, 1,      0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_SNS_SUM__A,     0,      0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_SNS_CYCCNT__A,  0,      0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_SNS_DIR_WD__A,  0,      0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_SNS_DIR_STP__A, 1,      0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_SNS_CYCLEN__A,  500,    0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_KI_CYCLEN__A,   500,    0 ) );
   }

   /* Write ATV specific settings */
   if ( DRXK_ISATVSTD( extAttr->standard ) )
   {
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_ATV_NAGC_KI_MIN__A,  0x0225, 0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_ATV_NAGC_KI_MAX__A,  0x0547, 0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_ATV_PAGC_KI_MIN__A,  0x0445, 0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_ATV_PAGC_KI_MAX__A,  0x0657, 0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_ATV_BPC_KI_MIN__A,   0x0223, 0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_ATV_BPC_KI_MAX__A,   0x0334, 0 ) );
      CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_ATV_KI_CHANGE_TH__A,
                              atvKiChangeTh, 0 ) );
   }

   /* Initialize inner-loop KI gain factors */
   CHK_ERROR((*ScuRr16)( devAddr, SCU_RAM_AGC_KI__A, &data, 0 ) );

   if ( DRXK_ISATVSTD( extAttr->standard ) )
   {
      data &= ~SCU_RAM_AGC_KI_RF__M;
      data |= ( DRXK_KI_RAGC_ATV << SCU_RAM_AGC_KI_RF__B );
      data &= ~SCU_RAM_AGC_KI_IF__M;
      data |= ( DRXK_KI_IAGC_ATV << SCU_RAM_AGC_KI_IF__B );
      data &= ~SCU_RAM_AGC_KI_DGAIN__M;
      data |= ( DRXK_KI_DAGC_ATV << SCU_RAM_AGC_KI_DGAIN__B );
   }
   else if ( DRXK_ISQAMSTD( extAttr->standard ) )
   {
      data = 0x0657;
      data &= ~SCU_RAM_AGC_KI_RF__M;
      data |= ( DRXK_KI_RAGC_QAM << SCU_RAM_AGC_KI_RF__B );
      data &= ~SCU_RAM_AGC_KI_IF__M;
      data |= ( DRXK_KI_IAGC_QAM << SCU_RAM_AGC_KI_IF__B );
   }
   else
   {
      data = 0x0657;
   }

   CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_KI__A, data, 0 ) );

   return DRX_STS_OK;

rw_error:

   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */
/**
* \brief Reset AGC
* \param demod    instance of demodulator.
* \param atomic   bool to indicate if access is atomic
* \return DRXStatus_t.
* \retval DRX_STS_OK      AGC algorithms initialized
* \retval DRX_STS_ERROR   failure; I2C error, current standard not supported
*/
static DRXStatus_t
AGCReset (  pDRXDemodInstance_t  demod,
            Bool_t               atomic )
{

   pI2CDeviceAddr_t    devAddr            = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t         extAttr            = (pDRXKData_t)      NULL;
   pDRXKPresets_t      presets            = (pDRXKPresets_t)   NULL;

   DRXWriteReg16Func_t ScuWr16            = NULL;
   DRXReadReg16Func_t  ScuRr16            = NULL;

   devAddr     = demod->myI2CDevAddr;
   extAttr     = (pDRXKData_t)demod->myExtAttr;
   presets     = &extAttr->presets;

   /* Select appropriate register access */
   if (atomic)
   {
      ScuRr16 = DRXK_DAP_SCU_AtomicReadReg16;
      ScuWr16 = DRXK_DAP_SCU_AtomicWriteReg16;
   }
   else
   {
      ScuRr16 = DRXK_DAP.readReg16Func;
      ScuWr16 = DRXK_DAP.writeReg16Func;
   }

   /* Write values to registers */
   CHK_ERROR((*ScuWr16)(   devAddr,
                           SCU_RAM_AGC_IF_IACCU_HI__A,
                           presets->rfAgcCfg.top,
                           0 ));
   CHK_ERROR((*ScuWr16)(   devAddr,
                           SCU_RAM_AGC_SNS_DIR_STP__A,
                           1,
                           0));
   CHK_ERROR((*ScuWr16)(   devAddr,
                           SCU_RAM_AGC_SNS_SUM__A,
                           0,
                           0));
   CHK_ERROR((*ScuWr16)(   devAddr,
                           SCU_RAM_AGC_SNS_DIR_TO__A,
                           0,
                           0));

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/* -------------------------------------------------------------------------- */
/**
* \brief Configure RF AGC
* \param demod instance of demodulator.
* \param agcSettings AGC configuration structure
* \return DRXStatus_t.
*/
static DRXStatus_t
AGCSetRf ( pDRXDemodInstance_t demod,
           pDRXKCfgAgc_t       agcSettings,
           Bool_t              atomic )
{
   pI2CDeviceAddr_t    devAddr      = NULL;
   pDRXKData_t         extAttr      = NULL;
   pDRXKCfgAgc_t       pAgcSettings = NULL;
   pDRXCommonAttr_t    commonAttr   = NULL;
   DRXWriteReg16Func_t ScuWr16      = NULL;
   DRXReadReg16Func_t  ScuRr16      = NULL;

   commonAttr  = (pDRXCommonAttr_t) demod -> myCommonAttr;
   devAddr  = demod -> myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* Select appropriate register access */
   if (atomic)
   {
      ScuRr16 = DRXK_DAP_SCU_AtomicReadReg16;
      ScuWr16 = DRXK_DAP_SCU_AtomicWriteReg16;
   }
   else
   {
      ScuRr16 = DRXK_DAP.readReg16Func;
      ScuWr16 = DRXK_DAP.writeReg16Func;
   }

   /* Allow to configure AGC only if standard is currently active*/
   if(   DRXK_ISQAMSTD(  extAttr->standard ) ||
         DRXK_ISATVSTD(  extAttr->standard ) ||
         DRXK_ISDVBTSTD( extAttr->standard ) )
   {
      u16_t data     = 0;

      switch ( agcSettings->ctrlMode )
      {
         case  DRXK_AGC_CTRL_AUTO:

            /* Enable RF AGC DAC */
            RR16( devAddr, IQM_AF_STDBY__A , &data );
            data &= ~IQM_AF_STDBY_STDBY_TAGC_RF_STANDBY;
            WR16( devAddr, IQM_AF_STDBY__A, data );

            CHK_ERROR(  (*ScuRr16)
                        ( devAddr, SCU_RAM_AGC_CONFIG__A, &data , 0 ) );

            /* Enable SCU RF AGC loop */
            data &= ~SCU_RAM_AGC_CONFIG_DISABLE_RF_AGC__M;

            /* Polarity */
            if ( commonAttr->tunerRfAgcPol )
            {
               data |= SCU_RAM_AGC_CONFIG_INV_RF_POL__M;
            }
            else
            {
               data &= ~SCU_RAM_AGC_CONFIG_INV_RF_POL__M;
            }
            CHK_ERROR(  (*ScuWr16)
                        ( devAddr, SCU_RAM_AGC_CONFIG__A, data, 0 ) );

            /* Set speed ( using complementary reduction value ) */
            CHK_ERROR(  (*ScuRr16)
                        ( devAddr, SCU_RAM_AGC_KI_RED__A , &data, 0 ) );

            data &= ~SCU_RAM_AGC_KI_RED_RAGC_RED__M;
            data |= ( ~ ( agcSettings->speed << SCU_RAM_AGC_KI_RED_RAGC_RED__B )
                        & SCU_RAM_AGC_KI_RED_RAGC_RED__M );

            CHK_ERROR(  (*ScuWr16)
                        ( devAddr, SCU_RAM_AGC_KI_RED__A , data,  0 ) );

            pAgcSettings = &(extAttr->presets.ifAgcCfg);

            /* Set TOP, only if IF-AGC is in AUTO mode */
            if ( pAgcSettings->ctrlMode == DRXK_AGC_CTRL_AUTO)
            {
               CHK_ERROR((*ScuWr16)( devAddr,
                                     SCU_RAM_AGC_IF_IACCU_HI_TGT_MAX__A,
                                     agcSettings->top,
                                     0 ) );
            }

            /* Cut-Off current */
            CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_RF_IACCU_HI_CO__A,
                                  agcSettings->cutOffCurrent, 0 ) );

            /* Max. output level */
            CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_RF_MAX__A,
                                  agcSettings->maxOutputLevel, 0 ) );

            break;

         case  DRXK_AGC_CTRL_USER:

            /* Enable RF AGC DAC */
            RR16( devAddr, IQM_AF_STDBY__A , &data );
            data &= ~IQM_AF_STDBY_STDBY_TAGC_RF_STANDBY;
            WR16( devAddr, IQM_AF_STDBY__A , data );

            /* Disable SCU RF AGC loop */
            CHK_ERROR(  (*ScuRr16)
                        ( devAddr, SCU_RAM_AGC_CONFIG__A, &data, 0 ) );
            data |= SCU_RAM_AGC_CONFIG_DISABLE_RF_AGC__M;
            if (commonAttr->tunerRfAgcPol)
            {
               data |= SCU_RAM_AGC_CONFIG_INV_RF_POL__M;
            }
            else
            {
               data &= ~SCU_RAM_AGC_CONFIG_INV_RF_POL__M;
            }
            CHK_ERROR((*ScuWr16)(   devAddr,
                                    SCU_RAM_AGC_CONFIG__A,
                                    data, 0 ) );

            /* SCU c.o.c. to 0, enabling full control range */
            CHK_ERROR((*ScuWr16)(   devAddr,
                                    SCU_RAM_AGC_RF_IACCU_HI_CO__A,
                                    0, 0 ) );

            /* Write value to output pin */
            CHK_ERROR((*ScuWr16)(   devAddr,
                                    SCU_RAM_AGC_RF_IACCU_HI__A,
                                    agcSettings->outputLevel, 0 ) );
            break;

         case  DRXK_AGC_CTRL_OFF:
            /* Disable RF AGC DAC */
            RR16( devAddr, IQM_AF_STDBY__A , &data );
            data |= IQM_AF_STDBY_STDBY_TAGC_RF_STANDBY;
            WR16( devAddr, IQM_AF_STDBY__A , data );

            /* Disable SCU RF AGC loop */
            CHK_ERROR((*ScuRr16)( devAddr, SCU_RAM_AGC_CONFIG__A, &data, 0 ) );
            data |= SCU_RAM_AGC_CONFIG_DISABLE_RF_AGC__M;
            CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CONFIG__A, data, 0 ) );
            break;

         default:
            return DRX_STS_INVALID_ARG;

      } /* switch ( agcsettings->ctrlMode ) */

      /* store new RF AGC setting (until a next preset overrides it) */
      extAttr->presets.rfAgcCfg = *agcSettings;
   }
   else
   {
      /* not allowed to configure RF AGC for inactive standard */
      return DRX_STS_INVALID_ARG;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief get configuration of RF AGC
* \param demod instance of demodulator.
* \param agcSettings AGC configuration structure
* \return DRXStatus_t.
*/
static DRXStatus_t
AGCGetRf ( pDRXDemodInstance_t   demod, pDRXKCfgAgc_t agcSettings )
{
   pI2CDeviceAddr_t devAddr  = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t      extAttr  = (pDRXKData_t)      NULL;
   u16_t            agcDacLvl = 0;

   devAddr  = demod -> myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* Return AGC settings only if a standard is active. */
   if( DRXK_ISQAMSTD(  extAttr->standard ) ||
       DRXK_ISATVSTD(  extAttr->standard ) ||
       DRXK_ISDVBTSTD( extAttr->standard ) )
   {
      *agcSettings = extAttr->presets.rfAgcCfg;
      if ( agcSettings->ctrlMode == DRXK_AGC_CTRL_AUTO )
      {
         /* AGC DAC outputLevel */
         RR16( devAddr, IQM_AF_AGC_RF__A, &agcDacLvl );

         agcSettings->outputLevel = 0;
         if ( agcDacLvl > DRXK_AGC_DAC_OFFSET )
         {
            agcSettings->outputLevel = agcDacLvl - DRXK_AGC_DAC_OFFSET;
         }
      }
   }
   else /* No standard active */
   {
      *agcSettings = extAttr->presets.rfAgcCfg;
      agcSettings->standard    = DRX_STANDARD_UNKNOWN;
      agcSettings->outputLevel = 0;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Configure If AGC
* \param demod instance of demodulator.
* \param agcSettings AGC configuration structure
* \return DRXStatus_t.
*/
static DRXStatus_t
AGCSetIf ( pDRXDemodInstance_t demod,
            pDRXKCfgAgc_t       agcSettings,
           Bool_t              atomic )
{
   pI2CDeviceAddr_t    devAddr         = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t         extAttr         = (pDRXKData_t)      NULL;
   pDRXKCfgAgc_t       pAgcSettingsRf  = (pDRXKCfgAgc_t)    NULL;
   pDRXCommonAttr_t    commonAttr      = (pDRXCommonAttr_t) NULL;
   DRXWriteReg16Func_t ScuWr16         = NULL;
   DRXReadReg16Func_t  ScuRr16         = NULL;

   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;
   devAddr    = demod -> myI2CDevAddr;
   extAttr    = (pDRXKData_t)demod->myExtAttr;

   /* Select appropriate register access */
   if (atomic)
   {
      ScuRr16 = DRXK_DAP_SCU_AtomicReadReg16;
      ScuWr16 = DRXK_DAP_SCU_AtomicWriteReg16;
   }
   else
   {
      ScuRr16 = DRXK_DAP.readReg16Func;
      ScuWr16 = DRXK_DAP.writeReg16Func;
   }

   /* Allow to configure AGC only if standard is currently active*/
   if(   DRXK_ISQAMSTD(  extAttr->standard ) ||
         DRXK_ISATVSTD(  extAttr->standard ) ||
         DRXK_ISDVBTSTD( extAttr->standard ) )
   {
      u16_t data        = 0;

      switch ( agcSettings->ctrlMode )
      {
         case  DRXK_AGC_CTRL_AUTO:

            /* Enable IF AGC DAC */
            RR16( devAddr, IQM_AF_STDBY__A , &data );
            data &= ~IQM_AF_STDBY_STDBY_TAGC_IF_STANDBY;
            WR16( devAddr, IQM_AF_STDBY__A , data );

            CHK_ERROR(  (*ScuRr16)
                        ( devAddr, SCU_RAM_AGC_CONFIG__A, &data, 0 ) );

            /* Enable SCU IF AGC loop */
            data &= ~SCU_RAM_AGC_CONFIG_DISABLE_IF_AGC__M;

            /* Polarity */
            if ( commonAttr->tunerIfAgcPol )
            {
               data |= SCU_RAM_AGC_CONFIG_INV_IF_POL__M;
            }
            else
            {
               data &= ~SCU_RAM_AGC_CONFIG_INV_IF_POL__M;
            }
            CHK_ERROR(  (*ScuWr16)
                        ( devAddr, SCU_RAM_AGC_CONFIG__A, data, 0 ) );

            /* Set speed (using complementary reduction value) */
            CHK_ERROR(  (*ScuRr16)
                        ( devAddr, SCU_RAM_AGC_KI_RED__A , &data, 0 ) );
            data &= ~SCU_RAM_AGC_KI_RED_IAGC_RED__M;
            data |= ( ~ ( agcSettings->speed << SCU_RAM_AGC_KI_RED_IAGC_RED__B )
                        & SCU_RAM_AGC_KI_RED_IAGC_RED__M );

            CHK_ERROR(  (*ScuWr16)
                        ( devAddr, SCU_RAM_AGC_KI_RED__A ,
                        data,
               0 ) );

            pAgcSettingsRf = &(extAttr->presets.rfAgcCfg);

            /* Restore TOP */
            CHK_ERROR((*ScuWr16)(   devAddr,
                                    SCU_RAM_AGC_IF_IACCU_HI_TGT_MAX__A,
                                    pAgcSettingsRf->top,
                                    0 ) );


            break;

         case  DRXK_AGC_CTRL_USER:

            /* Enable IF AGC DAC */
            RR16( devAddr, IQM_AF_STDBY__A , &data );
            data &= ~IQM_AF_STDBY_STDBY_TAGC_IF_STANDBY;
            WR16( devAddr, IQM_AF_STDBY__A , data );

            CHK_ERROR(  (*ScuRr16)
                        ( devAddr, SCU_RAM_AGC_CONFIG__A, &data, 0 ) );

            /* Disable SCU IF AGC loop */
            data |= SCU_RAM_AGC_CONFIG_DISABLE_IF_AGC__M;

            /* Polarity */
            if (commonAttr->tunerIfAgcPol)
            {
               data |= SCU_RAM_AGC_CONFIG_INV_IF_POL__M;
            }
            else
            {
               data &= ~SCU_RAM_AGC_CONFIG_INV_IF_POL__M;
            }
            CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CONFIG__A, data, 0 ) );

            /* Write value to output pin */
            CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_IF_IACCU_HI_TGT_MAX__A,
                                    agcSettings->outputLevel,
                                  0 ) );
            break;

         case  DRXK_AGC_CTRL_OFF:

            /* Disable If AGC DAC */
            RR16( devAddr, IQM_AF_STDBY__A , &data );
            data |= IQM_AF_STDBY_STDBY_TAGC_IF_STANDBY;
            WR16( devAddr, IQM_AF_STDBY__A , data );

            /* Disable SCU IF AGC loop */
            CHK_ERROR((*ScuRr16)( devAddr, SCU_RAM_AGC_CONFIG__A, &data, 0 ) );
            data |= SCU_RAM_AGC_CONFIG_DISABLE_IF_AGC__M;
            CHK_ERROR((*ScuWr16)( devAddr, SCU_RAM_AGC_CONFIG__A, data, 0 ) );

            break;

         default:
            return DRX_STS_INVALID_ARG;

      } /* switch ( agcSettingsIf->ctrlMode ) */

      /* always set the top to support configurations without if-loop */
      CHK_ERROR((*ScuWr16)(   devAddr,
                              SCU_RAM_AGC_INGAIN_TGT_MIN__A,
                              agcSettings->top,
                              0 ) );

      /* store new IF AGC setting (until a next preset overrides it) */
      extAttr->presets.ifAgcCfg = *agcSettings;
   }
   else /* if standard not active */
   {
      /* not allowed to configure IF AGC for inactive standard */
      return DRX_STS_INVALID_ARG;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief get configuration of If AGC
* \param demod instance of demodulator.
* \param agcSettings AGC configuration structure
* \return DRXStatus_t.
*/
static DRXStatus_t
AGCGetIf ( pDRXDemodInstance_t   demod, pDRXKCfgAgc_t agcSettings )
{
   pI2CDeviceAddr_t devAddr  = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t      extAttr  = (pDRXKData_t)      NULL;
   u16_t            agcDacLvl = 0;

   devAddr  = demod -> myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* Return AGC settings only if a standard is active. */
   if( DRXK_ISQAMSTD(  extAttr->standard ) ||
       DRXK_ISATVSTD(  extAttr->standard ) ||
       DRXK_ISDVBTSTD( extAttr->standard ) )
   {
      /* Return stored ATV AGC settings */
      *agcSettings = extAttr->presets.ifAgcCfg;

      /* Read output level */
      if ( agcSettings->ctrlMode == DRXK_AGC_CTRL_AUTO )
      {
         /* AGC DAC outputLevel */
         RR16( devAddr, IQM_AF_AGC_IF__A, &agcDacLvl );

         agcSettings->outputLevel = 0;
         if ( agcDacLvl > DRXK_AGC_DAC_OFFSET )
         {
            agcSettings->outputLevel = agcDacLvl - DRXK_AGC_DAC_OFFSET;
         }

          /* Read if top */
         SARR16( devAddr, SCU_RAM_AGC_INGAIN_TGT_MIN__A, &(agcSettings->top) );
      }
   }
   else /* No standard active */
   {
      *agcSettings             = extAttr->presets.ifAgcCfg;
      agcSettings->standard    = DRX_STANDARD_UNKNOWN;
      agcSettings->outputLevel = 0;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Set RF AGC.
* \param demod demod instance
* \param agcSettings rf agc configuration
* \return DRXStatus_t.
*
* Check arguments
* Dispatch handling to standard specific function.
*
*/
static DRXStatus_t
AGCCtrlSetCfgRf ( pDRXDemodInstance_t   demod, pDRXKCfgAgc_t agcSettings )
{
   /* check arguments */
   if ( agcSettings == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   switch ( agcSettings->ctrlMode ) {
      case DRXK_AGC_CTRL_AUTO: /* fallthrough */
      case DRXK_AGC_CTRL_USER: /* fallthrough */
      case DRXK_AGC_CTRL_OFF:
         break;
      default:
         return ( DRX_STS_INVALID_ARG );
   }

   /* Distpatch */
   switch ( agcSettings->standard ) {
      case DRX_STANDARD_DVBT:  /* fallthrough */
      case DRX_STANDARD_ITU_A: /* fallthrough */
      case DRX_STANDARD_ITU_C: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_BG : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP : /* fallthrough */
      case DRX_STANDARD_NTSC:          /* fallthrough */
      case DRX_STANDARD_FM:
         return AGCSetRf ( demod, agcSettings, TRUE);
      case DRX_STANDARD_UNKNOWN:
      default:
         return ( DRX_STS_INVALID_ARG );
   }

   return DRX_STS_OK;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Set IF AGC.
* \param demod demod instance
* \param agcSettings If agc configuration
* \return DRXStatus_t.
*
* Check arguments
* Dispatch handling to standard specific function.
*
*/
static DRXStatus_t
AGCCtrlSetCfgIf ( pDRXDemodInstance_t   demod, pDRXKCfgAgc_t agcSettings )
{
   /* check arguments */
   if ( agcSettings == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   switch ( agcSettings->ctrlMode ) {
      case DRXK_AGC_CTRL_AUTO: /* fallthrough */
      case DRXK_AGC_CTRL_USER: /* fallthrough */
      case DRXK_AGC_CTRL_OFF:  /* fallthrough */
         break;
      default:
         return ( DRX_STS_INVALID_ARG );
   }

   /* Distpatch */
   switch ( agcSettings->standard ) {
      case DRX_STANDARD_DVBT:  /* fallthrough */
      case DRX_STANDARD_ITU_A: /* fallthrough */
      case DRX_STANDARD_ITU_C: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_BG : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP : /* fallthrough */
      case DRX_STANDARD_NTSC:          /* fallthrough */
      case DRX_STANDARD_FM:
         return AGCSetIf ( demod, agcSettings, TRUE);
      case DRX_STANDARD_UNKNOWN:
      default:
         return ( DRX_STS_INVALID_ARG );
   }

   return DRX_STS_OK;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Retrieve RF AGC settings.
* \param demod demod instance
* \param agcSettings Rf agc configuration
* \return DRXStatus_t.
*
* Check arguments
* Dispatch handling to standard specific function.
*
*/
static DRXStatus_t
AGCCtrlGetCfgRf ( pDRXDemodInstance_t   demod, pDRXKCfgAgc_t agcSettings )
{
   /* check arguments */
   if ( agcSettings == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   /* Distpatch */
   switch ( agcSettings->standard ) {
      case DRX_STANDARD_DVBT:  /* fallthrough */
      case DRX_STANDARD_ITU_A: /* fallthrough */
      case DRX_STANDARD_ITU_C: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_BG : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP : /* fallthrough */
      case DRX_STANDARD_NTSC:          /* fallthrough */
      case DRX_STANDARD_FM:
         return AGCGetRf ( demod, agcSettings);
      case DRX_STANDARD_UNKNOWN:
      default:
         return ( DRX_STS_INVALID_ARG );
   }

   return DRX_STS_OK;
   }


/* -------------------------------------------------------------------------- */

/**
* \brief Retrieve IF AGC settings.
* \param demod demod instance
* \param agcSettings If agc configuration
* \return DRXStatus_t.
*
* Check arguments
* Dispatch handling to standard specific function.
*
*/
static DRXStatus_t
AGCCtrlGetCfgIf ( pDRXDemodInstance_t   demod, pDRXKCfgAgc_t agcSettings )
{
   /* check arguments */
   if ( agcSettings == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   /* Distpatch */
   switch ( agcSettings->standard ) {
      case DRX_STANDARD_DVBT:  /* fallthrough */
      case DRX_STANDARD_ITU_A: /* fallthrough */
      case DRX_STANDARD_ITU_C: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_BG : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP : /* fallthrough */
      case DRX_STANDARD_NTSC:          /* fallthrough */
      case DRX_STANDARD_FM:
         return AGCGetIf ( demod, agcSettings);
      case DRX_STANDARD_UNKNOWN:
      default:
         return ( DRX_STS_INVALID_ARG );
   }

   return DRX_STS_OK;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Set frequency shift.
* \param demod instance of demodulator.
* \param channel pointer to channel data.
* \param tunerFreqOffset residual frequency from tuner.
* \return DRXStatus_t.
*/
static DRXStatus_t
SetFrequency ( pDRXDemodInstance_t demod,
               pDRXChannel_t       channel,
               DRXFrequency_t      tunerFreqOffset )
{
   pI2CDeviceAddr_t devAddr         = NULL;
   pDRXCommonAttr_t commonAttr      = NULL;
   DRXFrequency_t samplingFrequency = 0;
   DRXFrequency_t frequencyShift    = 0;
   DRXFrequency_t ifFreqActual      = 0;
   DRXFrequency_t rfFreqResidual    = 0;
   DRXFrequency_t adcFreq           = 0;
   DRXFrequency_t intermediateFreq  = 0;
   u32_t          iqmFsRateOfs      = 0;
   pDRXKData_t    extAttr           = NULL;
   Bool_t         adcFlip           = TRUE;
   Bool_t         selectPosImage    = FALSE;
   Bool_t         rfMirror          = FALSE;
   Bool_t         tunerMirror       = TRUE;
   Bool_t         imageToSelect     = TRUE;
   DRXFrequency_t fmFrequencyShift  = 0;
   DRXFrequency_t lpFrequencyShift  = 0;
   u16_t          lo_incr           = 0;
   DRXFrequency_t f                 = 0;


   devAddr     = demod -> myI2CDevAddr;
   commonAttr  = (pDRXCommonAttr_t) demod -> myCommonAttr;
   extAttr     = (pDRXKData_t)demod->myExtAttr;
   rfFreqResidual = tunerFreqOffset;

   rfMirror = (channel->mirror == DRX_MIRROR_YES)?TRUE:FALSE;
   tunerMirror = demod->myCommonAttr->mirrorFreqSpect?FALSE:TRUE;

   /*
      Program frequency shifter
      No need to account for mirroring on RF
   */
   switch (extAttr->standard)
   {
      case DRX_STANDARD_ITU_A:        /* fallthrough */
      case DRX_STANDARD_ITU_C:        /* fallthrough */
         selectPosImage = TRUE;
         break;
      case DRX_STANDARD_FM:
         /* After IQM FS sound carrier must appear at 4 MHz in spect.
            The extra shift of the tuner should be taken into account here */
         fmFrequencyShift =  4000 + DRX_CARRIER_OFFSET_FM;
      case DRX_STANDARD_ITU_B:         /* fallthrough */
      case DRX_STANDARD_NTSC:          /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_BG:  /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK:  /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I:   /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L:
         selectPosImage = FALSE;
         break;
      case DRX_STANDARD_PAL_SECAM_LP:
         selectPosImage   = TRUE;
         /* L' workaround */
         lpFrequencyShift = -30;
         break;
      default:
         return ( DRX_STS_INVALID_ARG );
   }

   intermediateFreq  = demod->myCommonAttr->intermediateFreq;
   samplingFrequency = demod->myCommonAttr->sysClockFreq/3;

   if ( tunerMirror == TRUE )
   {
      /* tuner doesn't mirror */
      ifFreqActual = intermediateFreq + rfFreqResidual +
                     fmFrequencyShift + lpFrequencyShift;
   } else {
      /* tuner mirrors */
      ifFreqActual = intermediateFreq - rfFreqResidual -
                     fmFrequencyShift - lpFrequencyShift;
  }
   if ( ifFreqActual > samplingFrequency / 2)
   {
      /* adc mirrors */
      adcFreq = samplingFrequency - ifFreqActual;
      adcFlip = TRUE;
   } else {
      /* adc doesn't mirror */
      adcFreq = ifFreqActual;
      adcFlip = FALSE;
   }

   frequencyShift = adcFreq;
   imageToSelect = (Bool_t) ( rfMirror ^ tunerMirror ^
                              adcFlip  ^ selectPosImage );
   iqmFsRateOfs = Frac28( frequencyShift, samplingFrequency );

   if (imageToSelect)
      iqmFsRateOfs = ~iqmFsRateOfs + 1;

   /* Program frequency shifter with tuner offset compensation */
   WR32( devAddr, IQM_FS_RATE_OFS_LO__A , iqmFsRateOfs );
   extAttr->iqmFsRateOfs = iqmFsRateOfs;
   extAttr->posImage = imageToSelect;

   if(extAttr->standard == DRX_STANDARD_FM)
   {
      /* For FM standard used value from PG. */
      WR16( devAddr, IQM_RT_LO_INCR__A     , 2994 );
   }
   else if(DRXK_ISATVSTD(extAttr->standard))
   {
      if ( extAttr->presets.useTgtCarrierIf )
      {
         if ( extAttr->standard == DRX_STANDARD_PAL_SECAM_LP )
         {
            f = intermediateFreq - extAttr->presets.tgtCarrierIf
                  + lpFrequencyShift;
         }
         else
         {
            f = extAttr->presets.tgtCarrierIf - intermediateFreq;
         }
      }
      else
      {
         switch(extAttr->standard)
         {
            case DRX_STANDARD_PAL_SECAM_BG:
               if (channel->bandwidth == DRX_BANDWIDTH_8MHZ)
               {
                  f = DRX_CARRIER_OFFSET_PAL_SECAM_BG_8MHZ;
               }
               else
               {
                  f = DRX_CARRIER_OFFSET_PAL_SECAM_BG_7MHZ;
               }
               break;
            case DRX_STANDARD_PAL_SECAM_DK:
            case DRX_STANDARD_PAL_SECAM_I:
            case DRX_STANDARD_PAL_SECAM_L:
               f = DRX_CARRIER_OFFSET_PAL_SECAM_DKIL;
               break;
            case DRX_STANDARD_PAL_SECAM_LP:
               f = DRX_CARRIER_OFFSET_PAL_SECAM_LP
                  + lpFrequencyShift;
               break;
            case DRX_STANDARD_NTSC:
               f = DRX_CARRIER_OFFSET_PAL_NTSC;
               break;
            default:
               return DRX_STS_ERROR;
               break;
         }
      }

      lo_incr = (u16_t)( ( ( f * 16384L ) + 10125L ) / 20250L );

      WR16( devAddr, IQM_RT_LO_INCR__A     , lo_incr );

   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/*==                   AGC RELATED FUNCTIONS - E N D                        ==*/
/*============================================================================*/



/*============================================================================*/
/*==                 START OF DVBT & QAM JOINT FUNCTIONS                    ==*/
/*============================================================================*/
/**
* \fn DRXStatus_t DTVGetAccPktErr()
* \brief Retrieve packet errors for DVBT And QAM in accumulative mode
* \note Passing NULL will reset the counter
* \param demod Pointer to demod instance
* \param packetErr Pointer to packet error variable
* \return DRXStatus_t.
* \retval DRX_STS_OK sigStrength contains valid data.
* \retval DRX_STS_INVALID_ARG sigStrength is NULL.
* \retval DRX_STS_ERROR Erroneous data, sigStrength contains invalid data.
*/

static DRXStatus_t
DTVGetAccPktErr( pDRXDemodInstance_t demod,
                     pu16_t              packetErr)
{
   pDRXKData_t       extAttr     = NULL;
   pI2CDeviceAddr_t  devAddr     = NULL;

   extAttr  = (pDRXKData_t)demod->myExtAttr;
   devAddr  = demod -> myI2CDevAddr;

   if ( packetErr == NULL)
   {
      WR16( devAddr, SCU_RAM_FEC_ACCUM_PKT_FAILURES__A,   0 );
   }
   else
   {
      RR16( devAddr, SCU_RAM_FEC_ACCUM_PKT_FAILURES__A,   packetErr );
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/*==                 END OF DVBT & QAM JOINT FUNCTIONS                      ==*/
/*============================================================================*/



/*============================================================================*/
/*==                 START OF DVBT FUNCTIONS                                ==*/
/*============================================================================*/

/**
* \brief Enable or disable the OFDM token ring segment
* \param demod Demod instance.
* \param enable If TRUE enable, if FALSE disable OFDM token ring segment
* \return DRXStatus_t.
* \retval DRX_STS_OK      command has been executed.
* \retval DRX_STS_ERROR   failure; I2C error or timeout
*
*/
static DRXStatus_t
DVBTEnableOFDMTokenRing( pDRXDemodInstance_t demod,
                         Bool_t enable )
{
   pI2CDeviceAddr_t devAddr       = (pI2CDeviceAddr_t) NULL;
   DRXStatus_t      sts           = DRX_STS_ERROR;
   u16_t            data          = 0;
   u32_t            startTime     = 0;
   u16_t            desiredCtrl   = SIO_OFDM_SH_OFDM_RING_ENABLE_ON;
   u16_t            desiredStatus = SIO_OFDM_SH_OFDM_RING_STATUS_ENABLED;

   devAddr  = demod -> myI2CDevAddr;

   if ( enable == FALSE )
   {
      desiredCtrl   = SIO_OFDM_SH_OFDM_RING_ENABLE_OFF;
      desiredStatus = SIO_OFDM_SH_OFDM_RING_STATUS_DOWN;
   }

   RR16( devAddr, SIO_OFDM_SH_OFDM_RING_STATUS__A, &data);

   if ( data == desiredStatus )
   {
      /* tokenring already has correct status */
      return DRX_STS_OK;
   }

   /* Disable/enable dvbt tokenring bridge   */
   WR16( devAddr, SIO_OFDM_SH_OFDM_RING_ENABLE__A, desiredCtrl );

   startTime = DRXBSP_HST_Clock();
   do{
      RR16( devAddr, SIO_OFDM_SH_OFDM_RING_STATUS__A, &data);
   }while( ( data != desiredStatus ) &&
       ( (DRXBSP_HST_Clock() - startTime) < 0/*DRXK_OFDM_TR_SHUTDOWN_TIMEOUT*/ ));

   if ( data == desiredStatus )
   {
      sts = DRX_STS_OK;
   }

   return (sts);

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Execute a SC command (DVBT only)
* \param devAddr I2C address.
* \param cmd SC command.
* \return DRXStatus_t.
* \retval DRX_STS_OK      command has been executed.
* \retval DRX_STS_ERROR   failure; I2C error, timeout or SC error (see result)
* \retval DRX_STS_INVALID_ARG failure: cmd==NULL, scu cmd unknown, std unknow
*
* Using devAddr instead of demod because of SCU atomic register access
* interface.
*
*/

static DRXStatus_t
DVBTScCommand( pI2CDeviceAddr_t devAddr, pDRXKOfdmScCmd_t cmd )
{
   u16_t curCmd   = 0;
   u16_t errCode  = 0;
   u16_t scExec   = 0;
   u32_t startTime   = 0;

   /* Check param */
   if ( cmd == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   RR16( devAddr, OFDM_SC_COMM_EXEC__A, &scExec);
   if (scExec != 1)
   {
      /* SC is not running */
      return DRX_STS_ERROR;
   }

   /* Wait until sc is ready to receive command */

   startTime = DRXBSP_HST_Clock();
   do
   {
     // DRXBSP_HST_Sleep(1);
      RR16( devAddr, OFDM_SC_RA_RAM_CMD__A, &curCmd);

   } while  ( ( curCmd!= 0 ) &&
            ( ( startTime - DRXBSP_HST_Clock()) < 0/*DRXK_OFDM_CMD_TIMEOUT*/ ) );
   if ( curCmd != 0 )
   {
      return DRX_STS_ERROR;
   }

   /* Write sub-command */
   switch ( cmd->cmd ) {
      /* All commands using sub-cmd */
      case OFDM_SC_RA_RAM_CMD_PROC_START:
      case OFDM_SC_RA_RAM_CMD_SET_PREF_PARAM:
      case OFDM_SC_RA_RAM_CMD_PROGRAM_PARAM:
         WR16( devAddr, OFDM_SC_RA_RAM_CMD_ADDR__A, cmd->subcmd);
         break;
      default:
         /* Do nothing */
         break;
   } /* switch (cmd->cmd) */

   /* Write needed parameters and the command */
   switch ( cmd->cmd ) {
      /* All commands using 5 parameters */
      /* All commands using 4 parameters */
      /* All commands using 3 parameters */
      /* All commands using 2 parameters */
      case OFDM_SC_RA_RAM_CMD_PROC_START:
      case OFDM_SC_RA_RAM_CMD_SET_PREF_PARAM:
      case OFDM_SC_RA_RAM_CMD_PROGRAM_PARAM:
         WR16( devAddr, OFDM_SC_RA_RAM_PARAM1__A, cmd->param1);
      /* All commands using 1 parameters */
      case OFDM_SC_RA_RAM_CMD_SET_ECHO_TIMING:
      case OFDM_SC_RA_RAM_CMD_USER_IO:
         WR16( devAddr, OFDM_SC_RA_RAM_PARAM0__A, cmd->param0);
      /* All commands using 0 parameters */
      case OFDM_SC_RA_RAM_CMD_GET_OP_PARAM:
      case OFDM_SC_RA_RAM_CMD_NULL:
         /* Write command */
         WR16( devAddr, OFDM_SC_RA_RAM_CMD__A, cmd->cmd);
         break;
      default:
         /* Unknown command */
         return DRX_STS_INVALID_ARG;
         break;
   } /* switch (cmd->cmd) */


   /* Wait until sc is ready processing command */
   startTime = DRXBSP_HST_Clock();
   do
   {
      //DRXBSP_HST_Sleep(1);
      RR16( devAddr, OFDM_SC_RA_RAM_CMD__A, &curCmd);


   } while  ( ( curCmd!= 0 ) &&
            ( ( startTime - DRXBSP_HST_Clock()) < 0/*DRXK_OFDM_CMD_TIMEOUT*/ ) );
   if ( curCmd != 0 )
   {
      return DRX_STS_ERROR;
   }

   /* Check for illegal cmd */
   RR16( devAddr, OFDM_SC_RA_RAM_CMD_ADDR__A, &errCode);
   if ( errCode == 0xFFFF )
   {
      /* illegal command */
      return DRX_STS_INVALID_ARG;
   }

   /* Retreive results parameters from SC */
   switch ( cmd->cmd ) {
      /* All commands yielding 5 results */
      /* All commands yielding 4 results */
      /* All commands yielding 3 results */
      /* All commands yielding 2 results */
      /* All commands yielding 1 result */
      case OFDM_SC_RA_RAM_CMD_USER_IO:
      case OFDM_SC_RA_RAM_CMD_GET_OP_PARAM:
         RR16( devAddr, OFDM_SC_RA_RAM_PARAM0__A, &(cmd->param0));
      /* All commands yielding 0 results */
      case OFDM_SC_RA_RAM_CMD_SET_ECHO_TIMING:
      case OFDM_SC_RA_RAM_CMD_SET_TIMER:
      case OFDM_SC_RA_RAM_CMD_PROC_START:
      case OFDM_SC_RA_RAM_CMD_SET_PREF_PARAM:
      case OFDM_SC_RA_RAM_CMD_PROGRAM_PARAM:
      case OFDM_SC_RA_RAM_CMD_NULL:
         break;
      default:
         /* Unknown command */
         return DRX_STS_INVALID_ARG;
         break;
   } /* switch (cmd->cmd) */

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Power-down DVBT (disable, minimize power usage)
* \param demod instance of demodulator.
* \param setPowerMode Boolean used to decide to call CtrlPowerMode routine
*        or not, to avoid endless recursion.
* \return DRXStatus_t.
*/
static DRXStatus_t
DVBTPowerDown( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t devAddr    = NULL;
   pDRXCommonAttr_t commonAttr = NULL;
   DRXKSCUCmd_t     cmdSCU     = { /* command      */ 0,
                                   /* parameterLen */ 0,
                                   /* resultLen    */ 0,
                                   /* *parameter   */ NULL,
                                   /* *result      */ NULL };
   u16_t            cmdResult  = 0;
   u16_t            data       = 0;

   devAddr    = demod -> myI2CDevAddr;
   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;

   RR16( devAddr, SCU_COMM_EXEC__A, &data );
   if ( data == SCU_COMM_EXEC_ACTIVE )
   {
      /* Send OFDM stop command */
      cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_OFDM
                              | SCU_RAM_COMMAND_CMD_DEMOD_STOP;
      cmdSCU.parameterLen = 0;
      cmdSCU.resultLen    = 1;
      cmdSCU.parameter    = NULL;
      cmdSCU.result       = &cmdResult;
      CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

      /* Send OFDM reset command */
      cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_OFDM |
                            SCU_RAM_COMMAND_CMD_DEMOD_RESET;
      cmdSCU.parameterLen = 0;
      cmdSCU.resultLen    = 1;
      cmdSCU.parameter    = NULL;
      cmdSCU.result       = &cmdResult;
      CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );
   }

   /* Reset datapath for OFDM, processors first */
   WR16( devAddr, OFDM_SC_COMM_EXEC__A, OFDM_SC_COMM_EXEC_STOP );
   WR16( devAddr, OFDM_LC_COMM_EXEC__A, OFDM_LC_COMM_EXEC_STOP );
   WR16( devAddr, IQM_COMM_EXEC__A,     IQM_COMM_EXEC_B_STOP );

   /* powerdown AFE                   */
   CHK_ERROR( IQMSetAf( demod, FALSE ) );

   return DRX_STS_OK;

rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Power-up DVBT
* \param demod instance of demodulator.
* \return DRXStatus_t.
*
* Enable dvbt sysclk, enable tokenring bridge
*
*/
static DRXStatus_t
DVBTPowerUp (pDRXDemodInstance_t   demod )
{
   pI2CDeviceAddr_t  devAddr     = NULL;
   DRXPowerMode_t    powerMode   = DRX_POWER_UP;

   devAddr = demod -> myI2CDevAddr;

   CHK_ERROR( CtrlPowerMode( demod, &powerMode) );

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Enable/Disable DVBT Noise Impulse Cruncher
* \param demod    instance of demodulator.
*        enabled  boolean
* \return DRXStatus_t.
*
* Called in DVBTActivatePresets
*/
static DRXStatus_t
DVBTSetIncEnable ( pDRXDemodInstance_t demod, pBool_t enabled )
{
   pI2CDeviceAddr_t devAddr = NULL;

   /* check arguments */
   if ( enabled == NULL)
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = (pI2CDeviceAddr_t)demod->myI2CDevAddr;

   if ( *enabled == TRUE )
   {
      WR16( devAddr, IQM_CF_BYPASSDET__A,   0 );
   }
   else
   {
      WR16( devAddr, IQM_CF_BYPASSDET__A,   1 );
   }
   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Enable/Disable DVBT Noise Estimation
* \param demod    instance of demodulator.
*        enabled  boolean
* \return DRXStatus_t.
*
* Called in DVBTActivatePresets
*
*/
#define DEFAULT_FR_THRES_8K     6700
static DRXStatus_t
DVBTSetFrEnable ( pDRXDemodInstance_t demod, pBool_t enabled )
{
   pI2CDeviceAddr_t devAddr   = NULL;

   /* check arguments */
   if ( enabled == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = (pI2CDeviceAddr_t)demod->myI2CDevAddr;

   if ( *enabled == TRUE )
   {
      /* write mask to 1 */
      WR16( devAddr, OFDM_SC_RA_RAM_FR_THRES_8K__A,
            DEFAULT_FR_THRES_8K);
   }
   else
   {
      /* write mask to 0 */
      WR16( devAddr, OFDM_SC_RA_RAM_FR_THRES_8K__A, 0);
   }

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
   }

/*============================================================================*/
/**
* \brief Set Echo Threshold
* \param demod    instance of demodulator.
*        DRXConstellation_t  boolean to disable/enable
* \return DRXStatus_t.
*
* Called in DVBTActivatePresets
*
*/
static DRXStatus_t
DVBTSetEchoThreshold ( pDRXDemodInstance_t demod,
                           pDRXKCfgDvbtEchoThres_t echoThres)
{
   u16_t data                 = 0;
   pI2CDeviceAddr_t devAddr   = NULL;

   /* check arguments */
   if(echoThres == NULL)
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = (pI2CDeviceAddr_t)demod->myI2CDevAddr;

   RR16( devAddr, OFDM_SC_RA_RAM_ECHO_THRES__A, &data);

   switch (echoThres->fftMode)
   {
      case DRX_FFTMODE_2K:
         data &= ~ OFDM_SC_RA_RAM_ECHO_THRES_2K__M;
         data |= ((echoThres->threshold << OFDM_SC_RA_RAM_ECHO_THRES_2K__B) &
            (OFDM_SC_RA_RAM_ECHO_THRES_2K__M));
         break;
      case DRX_FFTMODE_8K:
         data &= ~ OFDM_SC_RA_RAM_ECHO_THRES_8K__M;
         data |= ((echoThres->threshold << OFDM_SC_RA_RAM_ECHO_THRES_8K__B) &
            (OFDM_SC_RA_RAM_ECHO_THRES_8K__M));
         break;
      default:
         return DRX_STS_INVALID_ARG;
         break;
   }

   WR16( devAddr, OFDM_SC_RA_RAM_ECHO_THRES__A, data);

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Set DVBT Signal Quality Indicator (SQI) speed
* \param demod    instance of demodulator.
*        speed    instance of DRXKCfgDvbtSqiSpeed
* \return DRXStatus_t.
*
*/
static DRXStatus_t
DVBTCtrlSetCfgSqiSpeed( pDRXDemodInstance_t    demod,
                     pDRXKCfgDvbtSqiSpeed_t speed )
{
   pI2CDeviceAddr_t devAddr   = NULL;
   pDRXKData_t       extAttr     = (pDRXKData_t)NULL;

   /* check arguments */
   if ( speed == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* parameter check */
   switch (*speed)
   {
      case DRXK_DVBT_SQI_SPEED_FAST:
      case DRXK_DVBT_SQI_SPEED_MEDIUM:
      case DRXK_DVBT_SQI_SPEED_SLOW:
         break;
      default:
         return DRX_STS_INVALID_ARG;
   }

   WR16( devAddr, SCU_RAM_FEC_PRE_RS_BER_FILTER_SH__A, (u16_t) *speed );

   extAttr->sqiSpeed = *speed;


   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/**
* \brief Reset DTV BER counter
* \param demod    instance of demodulator.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
DTVCtrlSetCfgResetBer( pDRXDemodInstance_t    demod )

{
   pI2CDeviceAddr_t  devAddr     = NULL;
   u16_t             rsMeasPeriod   = 0;

   devAddr = (pI2CDeviceAddr_t)demod->myI2CDevAddr;

   RR16( devAddr, FEC_RS_MEASUREMENT_PERIOD__A, &rsMeasPeriod );
   WR16( devAddr, FEC_RS_MEASUREMENT_PERIOD__A, 0 );
   WR16( devAddr, FEC_RS_MEASUREMENT_PERIOD__A, rsMeasPeriod );

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get DVBT Signal Quality Indicatir (SQI) speed
* \param demod    instance of demodulator.
*        speed    instance of DRXKCfgDvbtSqiSpeed
* \return DRXStatus_t.
*
*/
static DRXStatus_t
DVBTCtrlGetCfgSqiSpeed( pDRXDemodInstance_t    demod,
                     pDRXKCfgDvbtSqiSpeed_t speed)
{
   u16_t data = 0;
   pI2CDeviceAddr_t devAddr   = NULL;

   /* check arguments */
   if ( speed == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = (pI2CDeviceAddr_t)demod->myI2CDevAddr;

   RR16( devAddr, SCU_RAM_FEC_PRE_RS_BER_FILTER_SH__A, &data );
   /* check if data is valid */
   switch (data)
   {
      case (u16_t) DRXK_DVBT_SQI_SPEED_FAST:
      case (u16_t) DRXK_DVBT_SQI_SPEED_MEDIUM:
      case (u16_t) DRXK_DVBT_SQI_SPEED_SLOW:
         break;
      default:
         *speed = DRXK_DVBT_SQI_SPEED_UNKNOWN;
         return DRX_STS_ERROR;
   }

   *speed = (DRXKCfgDvbtSqiSpeed_t) data;

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Activate DVBT specific presets
* \param demod instance of demodulator.
* \return DRXStatus_t.
*
* Called in DVBTSetStandard
*
*/
static DRXStatus_t
DVBTActivatePresets (pDRXDemodInstance_t   demod )
{

   pI2CDeviceAddr_t  devAddr  = NULL;
   pDRXKData_t       extAttr  = NULL;
   DRXKCfgDvbtEchoThres_t echoThres2k = {0, DRX_FFTMODE_2K};
   DRXKCfgDvbtEchoThres_t echoThres8k = {0, DRX_FFTMODE_8K};

   devAddr = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   echoThres2k.threshold = extAttr->presets.dvbtEchoThres2k;
   echoThres8k.threshold = extAttr->presets.dvbtEchoThres8k;

   CHK_ERROR( DVBTSetIncEnable ( demod,
                              &extAttr->presets.dvbtImpNoiseCrunchEnable));

   CHK_ERROR( DVBTSetFrEnable (  demod,
                              &extAttr->presets.dvbtFrEnable));

   CHK_ERROR( DVBTSetEchoThreshold( demod, &echoThres2k));
   CHK_ERROR( DVBTSetEchoThreshold( demod, &echoThres8k));
   WR16( devAddr, SCU_RAM_AGC_INGAIN_TGT_MAX__A,
                     extAttr->presets.agcIngainTgtMax );

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Initialize channelswitch-independent settings for DVBT.
* \param demod instance of demodulator.
* \return DRXStatus_t.
*
* For ROM code channel filter taps are loaded from the bootloader. For microcode
* the DVB-T taps from the drxk_filters.h are used.
*/

static DRXStatus_t
DVBTSetStandard (pDRXDemodInstance_t   demod )
{
   pI2CDeviceAddr_t  devAddr     = NULL;
   pDRXCommonAttr_t  commonAttr  = NULL;
   pDRXKData_t       extAttr     = (pDRXKData_t)  (NULL);
   u16_t             cmdResult   = 0;
   u16_t             data        = 0;
   DRXKSCUCmd_t      cmdSCU;

   devAddr = demod -> myI2CDevAddr;
   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;
   extAttr = (pDRXKData_t)demod -> myExtAttr;

   DVBTPowerUp(demod);

   /* send OFDM reset command */
   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_OFDM |
                         SCU_RAM_COMMAND_CMD_DEMOD_RESET;
   cmdSCU.parameterLen = 0;
   cmdSCU.resultLen    = 1;
   cmdSCU.parameter    = NULL;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

   /* send OFDM setenv command */
   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_OFDM |
                         SCU_RAM_COMMAND_CMD_DEMOD_SET_ENV;
   cmdSCU.parameterLen = 0;
   cmdSCU.resultLen    = 1;
   cmdSCU.parameter    = NULL;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

   /* reset datapath for OFDM, processors first */
   WR16( devAddr, OFDM_SC_COMM_EXEC__A, OFDM_SC_COMM_EXEC_STOP );
   WR16( devAddr, OFDM_LC_COMM_EXEC__A, OFDM_LC_COMM_EXEC_STOP );
   WR16( devAddr, IQM_COMM_EXEC__A,     IQM_COMM_EXEC_B_STOP   );

   /* IQM setup */
   /* synchronize on ofdm_festart */
   WR16( devAddr, IQM_AF_UPD_SEL__A,      1);
   /* window size for clipping ADC detection */
   WR16( devAddr, IQM_AF_CLP_LEN__A,      0);
   /* window size for for sense pre-SAW detection */
   WR16( devAddr, IQM_AF_SNS_LEN__A,      0);
   /* sense threshold for sense pre-SAW detection */
   WR16( devAddr, IQM_AF_AMUX__A, IQM_AF_AMUX_SIGNAL2ADC);
   CHK_ERROR( IQMSetAf( demod, TRUE ) );

   WR16( devAddr, IQM_AF_AGC_RF__A,      0 );

   /* Impulse noise cruncher setup */
   WR16( devAddr, IQM_AF_INC_LCT__A,     0 ); /* crunch in IQM_CF */
   WR16( devAddr, IQM_CF_DET_LCT__A,     0 ); /* detect in IQM_CF */
   WR16( devAddr, IQM_CF_WND_LEN__A,     3 ); /* peak detector window length */

   WR16( devAddr, IQM_RC_STRETCH__A,    16 );
   WR16( devAddr, IQM_CF_OUT_ENA__A,   0x4 ); /* enable output 2 */
   WR16( devAddr, IQM_CF_DS_ENA__A,    0x4 ); /* decimate output 2 */
   WR16( devAddr, IQM_CF_SCALE__A,    1600 );
   WR16( devAddr, IQM_CF_SCALE_SH__A,    0 );

   /* virtual clipping threshold for clipping ADC detection */
   WR16( devAddr, IQM_AF_CLP_TH__A,    448 );

   WR16( devAddr, IQM_CF_DATATH__A,    495 ); /* crunching threshold */


   if (  (extAttr->presets.channelFilter.dataRe == NULL) ||
         (extAttr->presets.channelFilter.dataIm == NULL) )
   {
      /* default filter coefficients */
      /* upload by boot loader from ROM table */
      CHK_ERROR( BLChainCmd(  devAddr,
                              DRXK_BL_ROM_OFFSET_TAPS_DVBT,
                              DRXK_BLCC_NR_ELEMENTS_TAPS ));
   }
   else
   {
      /* load filters from presets */
      CHK_ERROR ( CtrlLoadFilter( demod, &extAttr->presets.channelFilter ));
   }

   WR16( devAddr, IQM_CF_PKDTH__A,        2 ); /* peak detector threshold */
   WR16( devAddr, IQM_CF_POW_MEAS_LEN__A, 2 );

   /* enable power measurement interrupt */
   WR16( devAddr, IQM_CF_COMM_INT_MSK__A, 1 );
   WR16( devAddr, IQM_COMM_EXEC__A, IQM_COMM_EXEC_B_ACTIVE );


   /* IQM will not be reset from here, sync ADC and update/init AGC */
   CHK_ERROR( ADCSynchronization (demod) );

   extAttr->standard = DRX_STANDARD_DVBT;
   CHK_ERROR( AGCCtrlSetCfgPreSaw( demod, &(extAttr->presets.preSawCfg)) );

   /* Halt SCU to enable safe non-atomic accesses */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_HOLD );

   CHK_ERROR( AGCSetRf( demod, &(extAttr->presets.rfAgcCfg), FALSE ) );
   CHK_ERROR( AGCSetIf( demod, &(extAttr->presets.ifAgcCfg), FALSE ) );

   /* Set Noise Estimation notch width and enable DC fix */
   RR16( devAddr, OFDM_SC_RA_RAM_CONFIG__A, &data);
   data |= OFDM_SC_RA_RAM_CONFIG_NE_FIX_ENABLE__M;
   WR16( devAddr, OFDM_SC_RA_RAM_CONFIG__A, data);


   /* Activate SCU to enable SCU commands */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_ACTIVE );

   if ( !DRXK_A3_ROM_CODE(demod))
   {
      /* AGCInit() is not done for DVBT, so set agcFastClipCtrlDelay  */
      WR16( devAddr, SCU_RAM_AGC_FAST_CLP_CTRL_DELAY__A,
            extAttr->presets.agcFastClipCtrlDelay );

   }

   /* OFDM_SC setup */
#ifdef COMPILE_FOR_NONRT
   WR16( devAddr, OFDM_SC_RA_RAM_BE_OPT_DELAY__A,        1);
   WR16( devAddr, OFDM_SC_RA_RAM_BE_OPT_INIT_DELAY__A,   2);
#endif

   /* FEC setup */
   WR16( devAddr, FEC_DI_INPUT_CTL__A,    1 );     /* OFDM input */

#ifdef COMPILE_FOR_NONRT
   WR16( devAddr, FEC_RS_MEASUREMENT_PERIOD__A   ,  0x400);
#else
   WR16( devAddr, FEC_RS_MEASUREMENT_PERIOD__A   , 0x1000);
#endif
   WR16( devAddr, FEC_RS_MEASUREMENT_PRESCALE__A , 0x0001);

   /* Setup MPEG bus */
   CHK_ERROR( MPEGTSDtoSetup ( demod, DRX_STANDARD_DVBT ) );

   /* Set DVBT Presets */
   CHK_ERROR ( DVBTActivatePresets (demod));

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Parameter check for DVBT set channel.
* \param demod   instance of demodulator.
* \param channel channel parameters to be checked/modified.
* \return DRXStatus_t.
* \retval DRX_STS_OK          channel parameters are valid.
* \retval DRX_STS_INVALID_ARG illegal channel parameters.
*
* Receiver parameter check for dvbt.
* Parameters may be modified to an equivalent setting.
*
*/
static DRXStatus_t
DVBTSetChParamCheck (pDRXDemodInstance_t   demod,
                     pDRXChannel_t         channel )
{
   switch ( channel->bandwidth )
   {
      case DRX_BANDWIDTH_8MHZ    : /* fall through */
      case DRX_BANDWIDTH_7MHZ    : /* fall through */
      case DRX_BANDWIDTH_6MHZ    :
         /* ok */
         break;
      default                    :
         return DRX_STS_INVALID_ARG;
         break;
   }

   switch ( channel->mirror )
   {
      case DRX_MIRROR_YES     : /* fall through */
      case DRX_MIRROR_NO      : /* fall through */
      case DRX_MIRROR_AUTO    :
         /* ok */
         break;
      default                 :
         channel->mirror = DRX_MIRROR_AUTO;
         break;
   }

   switch ( channel->constellation )
   {
      case DRX_CONSTELLATION_AUTO   : /* fall through */
      case DRX_CONSTELLATION_QPSK   : /* fall through */
      case DRX_CONSTELLATION_QAM16  : /* fall through */
      case DRX_CONSTELLATION_QAM64  :
         /* ok */
         break;
      default                       :
         return DRX_STS_INVALID_ARG;
         break;
   }

   switch ( channel->hierarchy )
   {
      case DRX_HIERARCHY_AUTO    : /* fall through */
      case DRX_HIERARCHY_NONE    : /* fall through */
      case DRX_HIERARCHY_ALPHA1  : /* fall through */
      case DRX_HIERARCHY_ALPHA2  : /* fall through */
      case DRX_HIERARCHY_ALPHA4  :
         /* ok */
         break;
      default                    :
         return DRX_STS_INVALID_ARG;
         break;
   }

   switch ( channel->priority)
   {
      case DRX_PRIORITY_LOW     : /* fall through */
      case DRX_PRIORITY_HIGH    :
         /* ok */
         break;
      default                   :
         channel->priority = DRX_PRIORITY_HIGH;
         break;
   }

   switch ( channel->coderate )
   {
      case DRX_CODERATE_AUTO   : /* fall through */
      case DRX_CODERATE_1DIV2  : /* fall through */
      case DRX_CODERATE_2DIV3  : /* fall through */
      case DRX_CODERATE_3DIV4  : /* fall through */
      case DRX_CODERATE_5DIV6  : /* fall through */
      case DRX_CODERATE_7DIV8  :
         /* ok */
         break;
      default              :
         return DRX_STS_INVALID_ARG;
         break;
   }

   switch ( channel->guard )
   {
      case DRX_GUARD_1DIV32  : /* fall through */
      case DRX_GUARD_1DIV16  : /* fall through */
      case DRX_GUARD_1DIV8   : /* fall through */
      case DRX_GUARD_1DIV4   : /* fall through */
      case DRX_GUARD_AUTO    :
         /* ok */
         break;
      default                :
         return DRX_STS_INVALID_ARG;
         break;
   }

   switch ( channel->fftmode)
   {
      case DRX_FFTMODE_2K      : /* fall through */
      case DRX_FFTMODE_8K      : /* fall through */
      case DRX_FFTMODE_AUTO    :
         /* ok */
         break;
      default                  :
         return DRX_STS_INVALID_ARG;
         break;
   }

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Set frequency shift.
* \param demod instance of demodulator.
* \param channel pointer to channel data.
* \param tunerFreqOffset residual frequency from tuner.
* \return DRXStatus_t.
*/
static DRXStatus_t
DVBTSetFrequencyShift ( pDRXDemodInstance_t demod,
                        pDRXChannel_t       channel,
                        DRXFrequency_t      tunerFreqOffset )
{
   pI2CDeviceAddr_t devAddr         = NULL;
   pDRXCommonAttr_t commonAttr      = NULL;
   pDRXKData_t      extAttr           = NULL;
   DRXFrequency_t   samplingFrequency = 0;
   DRXFrequency_t   frequencyShift    = 0;
   DRXFrequency_t   ifFreqActual      = 0;
   DRXFrequency_t   rfFreqResidual    = 0;
   DRXFrequency_t   adcFreq           = 0;
   DRXFrequency_t   intermediateFreq  = 0;
   u32_t            iqmFsRateOfs      = 0;
   Bool_t           adcFlip           = TRUE;
   Bool_t           selectPosImage    = FALSE;
   Bool_t           rfMirror          = FALSE;
   Bool_t           tunerMirror       = TRUE;
   Bool_t           imageToSelect     = TRUE;
   DRXFrequency_t   fmFrequencyShift  = 0;

   devAddr     = demod -> myI2CDevAddr;
   commonAttr  = (pDRXCommonAttr_t) demod -> myCommonAttr;
   extAttr     = (pDRXKData_t)demod->myExtAttr;

   rfFreqResidual = tunerFreqOffset;
   rfMirror       = (channel->mirror == DRX_MIRROR_YES)?TRUE:FALSE;
   tunerMirror    = demod->myCommonAttr->mirrorFreqSpect?FALSE:TRUE;
   /*
      Program frequency shifter
      No need to account for mirroring on RF
   */
   switch (extAttr->standard)
   {
      case DRX_STANDARD_ITU_A:        /* fallthrough */
      case DRX_STANDARD_ITU_C:        /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP: /* fallthrough */
      case DRX_STANDARD_DVBT:
         selectPosImage = TRUE;
         break;
      case DRX_STANDARD_FM:
         /* After IQM FS sound carrier must appear at 4 Mhz in spect.
            Sound carrier is already 3Mhz above centre frequency due
            to tuner setting so now add an extra shift of 1MHz... */
         fmFrequencyShift = 1000;
      case DRX_STANDARD_ITU_B:         /* fallthrough */
      case DRX_STANDARD_NTSC:          /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_BG:  /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK:  /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I:   /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L:
         selectPosImage = FALSE;
         break;
      default:
         return ( DRX_STS_INVALID_ARG );
   }
   intermediateFreq  = demod->myCommonAttr->intermediateFreq;
   samplingFrequency = demod->myCommonAttr->sysClockFreq/3;
   if ( tunerMirror == TRUE )
   {
      /* tuner doesn't mirror */
      ifFreqActual = intermediateFreq + rfFreqResidual + fmFrequencyShift;
   } else {
      /* tuner mirrors */
      ifFreqActual = intermediateFreq - rfFreqResidual - fmFrequencyShift;
  }
   if ( ifFreqActual > samplingFrequency / 2)
   {
      /* adc mirrors */
      adcFreq = samplingFrequency - ifFreqActual;
      adcFlip = TRUE;
   } else {
      /* adc doesn't mirror */
      adcFreq = ifFreqActual;
      adcFlip = FALSE;
  }

   frequencyShift = adcFreq;
   imageToSelect = (Bool_t)(rfMirror ^ tunerMirror ^ adcFlip ^ selectPosImage);
   iqmFsRateOfs = Frac28(frequencyShift,samplingFrequency);

   if (imageToSelect)
      iqmFsRateOfs = ~iqmFsRateOfs + 1;

   /* Program frequency shifter with tuner offset compensation */
   WR32( devAddr, IQM_FS_RATE_OFS_LO__A , iqmFsRateOfs );
   extAttr->iqmFsRateOfs = iqmFsRateOfs;
   extAttr->posImage = imageToSelect;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}



/*============================================================================*/

/**
* \brief Start dvbt demodulating for channel.
* \param demod instance of demodulator.
* \return DRXStatus_t.
*/
static DRXStatus_t
DVBTStart( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t  devAddr     = NULL;
   pDRXKData_t       extAttr     = NULL;
   DRXKOfdmScCmd_t   scCmd;

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t) demod->myExtAttr;

   /* Start correct processes to get in lock */
   /* DRXK: OFDM_SC_RA_RAM_PROC_LOCKTRACK is no longer in mapfile! */
   scCmd.param0 = OFDM_SC_RA_RAM_SW_EVENT_RUN_NMASK__M;
   scCmd.param1 = OFDM_SC_RA_RAM_LOCKTRACK_MIN;
   scCmd.subcmd = 0;
   scCmd.cmd    = OFDM_SC_RA_RAM_CMD_PROC_START;

   CHK_ERROR( DVBTScCommand( devAddr, &scCmd) );

   /* Start FEC OC */
   CHK_ERROR( MPEGTSStart( demod ) );

   WR16( devAddr, FEC_COMM_EXEC__A, FEC_COMM_EXEC_ACTIVE);

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/

/**
* \brief Set up dvbt demodulator for channel.
* \param demod instance of demodulator.
* \return DRXStatus_t.
*/
static DRXStatus_t
DVBTSetChannel ( pDRXDemodInstance_t demod,
                 pDRXChannel_t       channel,
                 DRXFrequency_t      tunerOffset  )
{
   pI2CDeviceAddr_t  devAddr     = NULL;
   pDRXCommonAttr_t  commonAttr  = NULL;
   pDRXKData_t       extAttr     = NULL;
   DRXKOfdmScCmd_t   scCmd;
   u16_t             cmdResult   = 0;
   DRXKSCUCmd_t      cmdSCU;
   u16_t  transmissionParams = 0;
   u16_t  operationMode = 0;
   u32_t  iqmRcRateOfs = 0;
   u32_t  bandwidth = 0;

   devAddr = demod -> myI2CDevAddr;
   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;
   extAttr = (pDRXKData_t) demod->myExtAttr;

   /* send OFDM stop command */
   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_OFDM
                           | SCU_RAM_COMMAND_CMD_DEMOD_STOP;
   cmdSCU.parameterLen = 0;
   cmdSCU.resultLen    = 1;
   cmdSCU.parameter    = NULL;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

   /* Halt SCU to enable safe non-atomic accesses */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_HOLD );

   /* Stop processors */
   WR16( devAddr, OFDM_SC_COMM_EXEC__A, OFDM_SC_COMM_EXEC_STOP);
   WR16( devAddr, OFDM_LC_COMM_EXEC__A, OFDM_LC_COMM_EXEC_STOP);

   /* Mandatory fix, always stop CP, required to set spl offset back to
       hardware default ( is set to 0 by ucode during pilot detection */
   WR16( devAddr, OFDM_CP_COMM_EXEC__A, OFDM_CP_COMM_EXEC_STOP );

   /*== Write channel settings to device =====================================*/

   /* mode */
   switch ( channel->fftmode) {
      case DRX_FFTMODE_AUTO    :
         operationMode |= OFDM_SC_RA_RAM_OP_AUTO_MODE__M;
         /* fall through , try first guess DRX_FFTMODE_8K */
      case DRX_FFTMODE_8K      :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_MODE_8K;
         break;
      case DRX_FFTMODE_2K      :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_MODE_2K;
         break;
      case DRX_FFTMODE_UNKNOWN : /* fall through */
      default:
         return DRX_STS_INVALID_ARG;
         break;
   }

   /* guard */
   switch ( channel->guard ) {
      case DRX_GUARD_AUTO    :
         operationMode |= OFDM_SC_RA_RAM_OP_AUTO_GUARD__M;
         /* fall through , try first guess DRX_GUARD_1DIV4 */
      case DRX_GUARD_1DIV4   :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_GUARD_4;
         break;
      case DRX_GUARD_1DIV32  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_GUARD_32;
         break;
      case DRX_GUARD_1DIV16  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_GUARD_16;
         break;
      case DRX_GUARD_1DIV8   :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_GUARD_8;
         break;
      case DRX_GUARD_UNKNOWN : /* fall through */
      default:
         return DRX_STS_INVALID_ARG;
         break;
   }

   /* hierarchy */
   switch ( channel->hierarchy ) {
      case DRX_HIERARCHY_AUTO    :
         operationMode |= OFDM_SC_RA_RAM_OP_AUTO_HIER__M;
         /* fall through , try first guess SC_RA_RAM_OP_PARAM_HIER_NO */
      case DRX_HIERARCHY_NONE    :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_HIER_NO;
         break;
      case DRX_HIERARCHY_ALPHA1  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_HIER_A1;
         break;
      case DRX_HIERARCHY_ALPHA2  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_HIER_A2;
         break;
      case DRX_HIERARCHY_ALPHA4  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_HIER_A4;
         break;
      case DRX_HIERARCHY_UNKNOWN : /* fall through */
      default:
         return DRX_STS_INVALID_ARG;
         break;
   }


   /* constellation */
   switch ( channel->constellation ) {
      case DRX_CONSTELLATION_AUTO   :
         operationMode |= OFDM_SC_RA_RAM_OP_AUTO_CONST__M;
         /* fall through , try first guess DRX_CONSTELLATION_QAM64 */
      case DRX_CONSTELLATION_QAM64  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_CONST_QAM64;
         break;
      case DRX_CONSTELLATION_QPSK   :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_CONST_QPSK;
         break;
      case DRX_CONSTELLATION_QAM16  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_CONST_QAM16;
         break;
      case DRX_CONSTELLATION_BPSK   : /* fall through */
      case DRX_CONSTELLATION_PSK8   : /* fall through */
      case DRX_CONSTELLATION_QAM32  : /* fall through */
      case DRX_CONSTELLATION_QAM128 : /* fall through */
      case DRX_CONSTELLATION_QAM256 : /* fall through */
      case DRX_CONSTELLATION_QAM512 : /* fall through */
      case DRX_CONSTELLATION_QAM1024: /* fall through */
      case DRX_CONSTELLATION_UNKNOWN: /* fall through */
      default:
         return DRX_STS_INVALID_ARG;
         break;
   }

   /* Priority (only for hierarchical channels) */
   switch ( channel->priority) {
      case DRX_PRIORITY_LOW     :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_PRIO_LO;
         WR16( devAddr, OFDM_EC_SB_PRIOR__A,   OFDM_EC_SB_PRIOR_LO);
         break;
      case DRX_PRIORITY_HIGH    :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_PRIO_HI;
         WR16( devAddr, OFDM_EC_SB_PRIOR__A,   OFDM_EC_SB_PRIOR_HI);
         break;
      case DRX_PRIORITY_UNKNOWN : /* fall through */
      default:
         return DRX_STS_INVALID_ARG;
         break;
   }

   /* coderate */
   switch ( channel->coderate ) {
      case DRX_CODERATE_AUTO   :
         operationMode |= OFDM_SC_RA_RAM_OP_AUTO_RATE__M;
         /* fall through , try first guess DRX_CODERATE_2DIV3 */
      case DRX_CODERATE_2DIV3  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_RATE_2_3;
         break;
      case DRX_CODERATE_1DIV2  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_RATE_1_2;
         break;
      case DRX_CODERATE_3DIV4  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_RATE_3_4;
         break;
      case DRX_CODERATE_5DIV6  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_RATE_5_6;
         break;
      case DRX_CODERATE_7DIV8  :
         transmissionParams |= OFDM_SC_RA_RAM_OP_PARAM_RATE_7_8;
         break;
      case DRX_CODERATE_UNKNOWN: /* fall through */
      default:
         return DRX_STS_INVALID_ARG;
         break;
   }

   /* SAW filter selection: normaly not necesarry, but if wanted
      the application can select a SAW filter via the driver by using UIOs */
   /* First determine real bandwidth (Hz) */
   /* Also set delay for impulse noise cruncher */
   /* Also set parameters for EC_OC fix, note EC_OC_REG_TMD_HIL_MAR is changed
      by SC for fix for some 8K,1/8 guard but is restored by InitEC and ResetEC
      functions */
   switch ( channel->bandwidth ) {
      case DRX_BANDWIDTH_8MHZ    :
         bandwidth = DRXK_BANDWIDTH_8MHZ_IN_HZ;
         WR16( devAddr, OFDM_SC_RA_RAM_SRMM_FIX_FACT_8K__A, 3052);
         /* cochannel protection for PAL 8 MHz */
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_8K_PER_LEFT__A,  7 );
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_8K_PER_RIGHT__A, 7 );
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_2K_PER_LEFT__A,  7 );
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_2K_PER_RIGHT__A, 1 );
         break;
      case DRX_BANDWIDTH_7MHZ    :
         bandwidth = DRXK_BANDWIDTH_7MHZ_IN_HZ;
         WR16( devAddr, OFDM_SC_RA_RAM_SRMM_FIX_FACT_8K__A, 3491);
         /* cochannel protection for PAL 7 MHz */
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_8K_PER_LEFT__A, 8 );
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_8K_PER_RIGHT__A, 8 );
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_2K_PER_LEFT__A,  4 );
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_2K_PER_RIGHT__A, 1 );
         break;
      case DRX_BANDWIDTH_6MHZ    :
         bandwidth = DRXK_BANDWIDTH_6MHZ_IN_HZ;
         WR16( devAddr, OFDM_SC_RA_RAM_SRMM_FIX_FACT_8K__A, 4073);
         /* cochannel protection for NTSC 6 MHz */
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_8K_PER_LEFT__A,  19 );
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_8K_PER_RIGHT__A, 19 );
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_2K_PER_LEFT__A,  14 );
         WR16( devAddr, OFDM_SC_RA_RAM_NI_INIT_2K_PER_RIGHT__A,  1 );
         break;
      case DRX_BANDWIDTH_UNKNOWN : /* fall through */
      default:
         return DRX_STS_INVALID_ARG;
         break;
   }

   if (iqmRcRateOfs == 0)
   {
      /* iqmRcRateOfs not taken from channel cache: calculate from SysClkInHz */
      if (extAttr->sysClkInHz == 0)
      {
         /* SysClkInHz not yet detected, calculate */
         extAttr->sysClkInHz = (commonAttr->sysClockFreq * 1000)/3;
      }
      /* Now compute IQM_RC_RATE_OFS
          ((( SysFreq/BandWidth)/2)/2) -1) * 2^23)
         =>
          ((SysFreq / BandWidth) * (2^21) ) - (2^23)
      */
      /* (SysFreq / BandWidth) * (2^28)  */
      /* assert ( MAX(sysClk)/MIN(bandwidth) < 16 )
         => assert( MAX(sysClk) < 16*MIN(bandwidth) )
         => assert( 109714272 > 48000000 ) = TRUE so Frac 28 can be used  */
      iqmRcRateOfs = Frac28( (u32_t)(extAttr->sysClkInHz), bandwidth );
      /* (SysFreq / BandWidth) * (2^21), rounding before truncating  */
      if ( (iqmRcRateOfs & 0x7fL) >= 0x40 )
      {
         iqmRcRateOfs += 0x80L;
      }
      iqmRcRateOfs = iqmRcRateOfs >> 7 ;
      /* ((SysFreq / BandWidth) * (2^21) ) - (2^23)  */
      iqmRcRateOfs = iqmRcRateOfs - (1<<23);
   }

   iqmRcRateOfs &= (( ((u32_t)IQM_RC_RATE_OFS_HI__M)<<IQM_RC_RATE_OFS_LO__W ) |
                IQM_RC_RATE_OFS_LO__M );
   WR32( devAddr, IQM_RC_RATE_OFS_LO__A, iqmRcRateOfs );
   extAttr->iqmRcRateOfs = iqmRcRateOfs;

   /* Bandwidth setting done */

   CHK_ERROR( DVBTSetFrequencyShift( demod, channel, tunerOffset));


   /*== Start SC, write channel settings to SC ===============================*/

   /* Activate SCU to enable SCU commands */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_ACTIVE );

   /* Enable SC after setting all other parameters */
   WR16( devAddr, OFDM_SC_COMM_STATE__A,    0);
   WR16( devAddr, OFDM_SC_COMM_EXEC__A,     1);

   /* start SCU OFDM microcode */
   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_OFDM
                           | SCU_RAM_COMMAND_CMD_DEMOD_START;
   cmdSCU.parameterLen = 0;
   cmdSCU.resultLen    = 1;
   cmdSCU.parameter    = NULL;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

   /* Write SC parameter registers, set all AUTO flags in operation mode */
   scCmd.param0 = transmissionParams;
   scCmd.param1 = ( OFDM_SC_RA_RAM_OP_AUTO_MODE__M  |
                    OFDM_SC_RA_RAM_OP_AUTO_GUARD__M |
                    OFDM_SC_RA_RAM_OP_AUTO_CONST__M |
                    OFDM_SC_RA_RAM_OP_AUTO_HIER__M  |
                    OFDM_SC_RA_RAM_OP_AUTO_RATE__M  );
   scCmd.subcmd = 0x0000 ;
   scCmd.cmd    = OFDM_SC_RA_RAM_CMD_SET_PREF_PARAM;
   CHK_ERROR( DVBTScCommand( devAddr, &scCmd) );

   if ( !DRXK_A3_ROM_CODE(demod))
   {
      /* set ber averaging for quality indicator */
      CHK_ERROR ( DVBTCtrlSetCfgSqiSpeed( demod, &extAttr->sqiSpeed ));

   }

   /* set scanning function pointer */
   commonAttr->scanFunction = NULL; /* set default scan function for DVBT */

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Retreive lock status .
* \param demod    Pointer to demodulator instance.
* \param lockStat Pointer to lock status structure.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
DVBTLockStatus( pDRXDemodInstance_t demod,
                pDRXLockStatus_t    lockStat )
{
   const u16_t mpeg_lock_mask  = ( OFDM_SC_RA_RAM_LOCK_MPEG__M |
                                   OFDM_SC_RA_RAM_LOCK_FEC__M  );
   const u16_t fec_lock_mask   =  OFDM_SC_RA_RAM_LOCK_FEC__M ;
   const u16_t demod_lock_mask =  OFDM_SC_RA_RAM_LOCK_DEMOD__M ;
   pDRXKData_t       extAttr   = (pDRXKData_t)      (NULL);
   pI2CDeviceAddr_t  devAddr   = (pI2CDeviceAddr_t) (NULL);
   u16_t ScRaRamLock = 0;
   u16_t ScCommExec  = 0;

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t) (demod -> myExtAttr);

   *lockStat = DRX_NOT_LOCKED;

   /* Check if SC is running */
   RR16( devAddr, OFDM_SC_COMM_EXEC__A, &ScCommExec );
   if ( ScCommExec == OFDM_SC_COMM_EXEC_STOP )
   {
      /* SC not active; return DRX_NOT_LOCKED */
      return DRX_STS_OK;
   }

   /* SC running, readout lock status */
   RR16( devAddr, OFDM_SC_RA_RAM_LOCK__A, &ScRaRamLock);

   if ( (ScRaRamLock & mpeg_lock_mask) == mpeg_lock_mask )
   {
      *lockStat = DRX_LOCKED;
   } else if ( (ScRaRamLock & fec_lock_mask) == fec_lock_mask )
   {
      *lockStat = DRXK_FEC_LOCK;
   } else if ( (ScRaRamLock & demod_lock_mask) == demod_lock_mask )
   {
      *lockStat = DRXK_DEMOD_LOCK;
   } else if (ScRaRamLock & OFDM_SC_RA_RAM_LOCK_NODVBT__M)
   {
      *lockStat = DRX_NEVER_LOCK;
   } else
   {
      *lockStat = DRX_NOT_LOCKED;
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Get measured frequency offset ( wrt center frequency)
* \param demod    Pointer to demod instance.
* \param mirrored If TRUE signal had to be mirrored for lock, FALSE otherwise.
* \param offset   Center frequency offset in kHz.
* \return DRXStatus_t.
*/
static DRXStatus_t
DVBTGetCenterFreqOffset ( pDRXDemodInstance_t demod,
                          pBool_t             mirrored,
                          ps32_t              offset )
{
   DRXFrequency_t samplingFrequency = 0;
   s32_t currentFrequency           = 0;
   s32_t nominalFrequency           = 0;
   s32_t carrierFrequencyShift      = 0;
   s32_t sign                       = 1;
   u32_t data64Hi                   = 0;
   u32_t data64Lo                   = 0;
   pI2CDeviceAddr_t devAddr         = NULL;
   pDRXKData_t      extAttr         = (pDRXKData_t) (NULL);

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t) (demod -> myExtAttr);

   samplingFrequency = (demod ->myCommonAttr->sysClockFreq)/3;

   /* Assignment with sign extension */
   if ( ((u32_t)(extAttr->iqmFsRateOfs)) & 0x8000000L )
   {
      nominalFrequency = (s32_t)( (extAttr->iqmFsRateOfs) | 0xF8000000L );
   } else {
      nominalFrequency = (s32_t)(extAttr->iqmFsRateOfs);
   }

   /* Register is sign extended */
   ARR32( devAddr, IQM_FS_RATE_LO__A, (pu32_t) &currentFrequency );

   /* Determine if signal was mirrored after set channel call */
   *mirrored = (Bool_t)( (currentFrequency&0x8000000L) !=
                         (nominalFrequency&0x8000000L) );


   /* Frequency offset computation */
   if ( *mirrored )
   {
      nominalFrequency = (-nominalFrequency);
   }

   if (( (extAttr->posImage) == TRUE )^ (*mirrored) )
   {
      /* Positive image */
      carrierFrequencyShift = currentFrequency - nominalFrequency;
   } else {
      /* Negative image */
      carrierFrequencyShift = nominalFrequency - currentFrequency;
   }

   /* Carrier frequency shift in Hz */
   if (carrierFrequencyShift < 0)
   {
      sign = ((s32_t)-1);
      carrierFrequencyShift *= sign;
   }

   /* offset = carrierFrequencyShift * samplingFrequency / (1 << 28) */
   Mult32 ( carrierFrequencyShift, samplingFrequency, &data64Hi, &data64Lo );
   *offset = (s32_t)((((data64Lo >> 28) & 0xf) | (data64Hi << 4)) * sign);
   /* Rounding */
   if (data64Lo & ( ((u32_t)1) << 27 ) )
   {
      (*offset)+= sign* 1;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Retreive transmission parameters of current channel.
* \param demod   Pointer to demod instance.
* \param channel Pointer to channel data.
* \return DRXStatus_t.
*
* Transmission parameters only valid when device is locked.
* Assuming this device is only called when lock status is "locked"
*
*/
static DRXStatus_t
DVBTGetChannel( pDRXDemodInstance_t   demod,
                pDRXChannel_t         channel )
{
   u16_t            transmissionParams = 0;
   pI2CDeviceAddr_t devAddr            = (pI2CDeviceAddr_t)(NULL);
   pDRXCommonAttr_t commonAttr         = (pDRXCommonAttr_t)(NULL);
   pDRXKData_t      extAttr            = (pDRXKData_t) (NULL);
   DRXFrequency_t   offset             = 0;
   Bool_t           mirrored           = FALSE;
   Bool_t           wasMirrored        = FALSE;

   /*== check arguments ======================================================*/

   if ( channel == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr    = demod -> myI2CDevAddr;
   extAttr    = (pDRXKData_t) (demod -> myExtAttr);
   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;

   /* Bandwidth */
   channel->bandwidth      = extAttr->currentChannel.bandwidth;

   /* Frequency offset correction & mirror detection */
   CHK_ERROR( DVBTGetCenterFreqOffset (demod, &mirrored, &offset ));
   channel->frequency -= offset;

   wasMirrored = (Bool_t)((extAttr->currentChannel.mirror)==DRX_MIRROR_YES);
   if ( mirrored ^ wasMirrored )
   {
      channel->mirror = DRX_MIRROR_YES;
   } else {
      channel->mirror = DRX_MIRROR_NO;
   }

   /* Transmission parameters.  */
   RR16( devAddr, OFDM_SC_RA_RAM_OP_PARAM__A, &transmissionParams);

   /* mode */
   switch ( transmissionParams & OFDM_SC_RA_RAM_OP_PARAM_MODE__M ) {
      case OFDM_SC_RA_RAM_OP_PARAM_MODE_2K:
         channel->fftmode = DRX_FFTMODE_2K;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_MODE_8K:
         channel->fftmode = DRX_FFTMODE_8K;
         break;
      default:
         return DRX_STS_ERROR;
         break;
   }

   /* guard */
   switch ( transmissionParams & OFDM_SC_RA_RAM_OP_PARAM_GUARD__M) {
      case OFDM_SC_RA_RAM_OP_PARAM_GUARD_32:
         channel->guard = DRX_GUARD_1DIV32;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_GUARD_16:
         channel->guard = DRX_GUARD_1DIV16;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_GUARD_8:
         channel->guard = DRX_GUARD_1DIV8;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_GUARD_4:
         channel->guard = DRX_GUARD_1DIV4;
         break;
      default:
         return DRX_STS_ERROR;
         break;
   }

   /* hierarchy */
   switch ( transmissionParams & OFDM_SC_RA_RAM_OP_PARAM_HIER__M ) {
      case OFDM_SC_RA_RAM_OP_PARAM_HIER_NO:
         channel->hierarchy = DRX_HIERARCHY_NONE;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_HIER_A1:
         channel->hierarchy = DRX_HIERARCHY_ALPHA1;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_HIER_A2:
         channel->hierarchy = DRX_HIERARCHY_ALPHA2;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_HIER_A4:
         channel->hierarchy = DRX_HIERARCHY_ALPHA4;
         break;
      default:
         return DRX_STS_ERROR;
         break;
   }

   /* constellation */
   switch ( transmissionParams & OFDM_SC_RA_RAM_OP_PARAM_CONST__M ) {

      case OFDM_SC_RA_RAM_OP_PARAM_CONST_QPSK:
         channel->constellation = DRX_CONSTELLATION_QPSK;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_CONST_QAM16:
         channel->constellation = DRX_CONSTELLATION_QAM16;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_CONST_QAM64:
         channel->constellation = DRX_CONSTELLATION_QAM64;
         break;
      default:
         return DRX_STS_ERROR;
         break;
   }

   /* Priority (only for hierarchical channels) */
   switch ( transmissionParams & OFDM_SC_RA_RAM_OP_PARAM_PRIO__M) {
      case OFDM_SC_RA_RAM_OP_PARAM_PRIO_LO:
         channel->priority = DRX_PRIORITY_LOW;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_PRIO_HI:
         channel->priority = DRX_PRIORITY_HIGH;
         break;
      default:
         return DRX_STS_ERROR;
         break;
   }

   /* coderate */
   switch ( transmissionParams & OFDM_SC_RA_RAM_OP_PARAM_RATE__M) {
      case OFDM_SC_RA_RAM_OP_PARAM_RATE_1_2  :
         channel->coderate = DRX_CODERATE_1DIV2;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_RATE_2_3  :
         channel->coderate = DRX_CODERATE_2DIV3;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_RATE_3_4  :
         channel->coderate = DRX_CODERATE_3DIV4;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_RATE_5_6  :
         channel->coderate = DRX_CODERATE_5DIV6;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_RATE_7_8  :
         channel->coderate = DRX_CODERATE_7DIV8;
         break;
      default:
         return DRX_STS_ERROR;
         break;
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Retreive signal quality from device.
* \param demod Pointer to demodulator instance.
* \param sigQuality Pointer to signal quality data.
* \return DRXStatus_t.
* \retval DRX_STS_OK sigQuality contains valid data.
* \retval DRX_STS_INVALID_ARG sigQuality is NULL.
* \retval DRX_STS_ERROR No lock, erroneous data, sigQuality == NULL.
*
*/
static DRXStatus_t
DVBTSigQuality( pDRXDemodInstance_t demod,
                pDRXSigQuality_t    sigQuality )
{
   /* For MER calculation */
   u16_t regData            = 0;
   u32_t EqRegTdSqrErrI     = 0;
   u32_t EqRegTdSqrErrQ     = 0;
   u16_t EqRegTdSqrErrExp   = 0;
   u16_t EqRegTdTpsPwrOfs   = 0;
   u16_t EqRegTdReqSmbCnt   = 0;
   u32_t tpsCnt             = 0;
   u32_t SqrErrIQ           = 0;
   u32_t a                  = 0;
   u32_t b                  = 0;
   u32_t c                  = 0;
   u32_t iMER               = 0;
   u16_t transmissionParams = 0;
   u16_t hiResBer        = 0;

   /* For SQI calculation */
   u32_t        postViterbiBERHiRes = 0;
   u16_t        idx            = 0;
   s32_t        CNrel          = 0;
   s32_t        berSQI         = 0;
   s32_t        SQI            = 0;
   const s32_t  CNNordigTab[]  = {  51,  69,  79,  89,  97,   /* QPSK  */
                                   108, 131, 146, 156, 160,   /* QAM16 */
                                   165, 187, 202, 216, 225 }; /* QAM64 */

   /* For BER calculation */
   DRXLockStatus_t  lockStatus;

   pDRXKData_t      extAttr = (pDRXKData_t) (NULL);
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t)(NULL);

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t) (demod -> myExtAttr);

   /* Check arguments */
   if ( sigQuality == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   CHK_ERROR( DVBTLockStatus( demod, &lockStatus) );

   if ( lockStatus >= DRXK_FEC_LOCK )
   {
      u16_t EcVdRegErrBitCnt    = 0;
      u16_t EcVdRegInBitCnt     = 0;
      u16_t EcRsRegErrBitCnt    = 0;
      u16_t EcRsRegErrBitCntMan = 0;
      u16_t EcRsRegErrBitCntExp = 0;

      u16_t EcRsPrescale      = 0;
      u16_t EcRsRegInPckCnt   = 0;
      u16_t EcRsRegErrPckCnt  = 0;

      const u32_t rsPacketLength   = 204*8;
      u32_t postViterbiDenominator = 0;
      u32_t postViterbiErrBits     = 0;


      sigQuality->postReedSolomonBER   = 0;

      /* BER measurement is valid if at least FEC lock is achieved */

      /* OFDM_EC_VD_REQ_SMB_CNT__A and/or OFDM_EC_VD_REQ_BIT_CNT can be written
         to set nr of symbols or bits over which
         to measure EC_VD_REG_ERR_BIT_CNT__A . See CtrlSetCfg(). */

      /* Read registers for post/preViterbi BER calculation */
      RR16( devAddr, OFDM_EC_VD_ERR_BIT_CNT__A, &EcVdRegErrBitCnt);
      RR16( devAddr, OFDM_EC_VD_IN_BIT_CNT__A , &EcVdRegInBitCnt );

      /* Number of bit-errors */
      RR16 ( devAddr, FEC_RS_NR_BIT_ERRORS__A, &EcRsRegErrBitCnt );
      EcRsRegErrBitCntMan = EcRsRegErrBitCnt &
                              FEC_RS_NR_BIT_ERRORS_FIXED_MANT__M;
      EcRsRegErrBitCntExp = EcRsRegErrBitCnt >> FEC_RS_NR_BIT_ERRORS_EXP__B;
      /* Normalized mantissa: for exp > 0, bit 11 of mantissa always set.
                              for exp > 4 a u16_t overflows for sure */
      if ( EcRsRegErrBitCntExp <= FEC_RS_NR_BIT_ERRORS_EXP__W )
      {
         EcRsRegErrBitCnt = EcRsRegErrBitCntMan << EcRsRegErrBitCntExp;
      }
      else
      {
         /* Clip at max u16_t */
         EcRsRegErrBitCnt = MAX_U16;
      }

      if ( !DRXK_A3_ROM_CODE(demod))
      {
         RR16 ( devAddr, SCU_RAM_FEC_PRE_RS_BER__A,   &hiResBer);
      }
      else
      {
         /* Hi resolution BER not available in SCU code, just use regular BER */
         hiResBer = EcRsRegErrBitCnt;
      }


      RR16 ( devAddr, FEC_RS_MEASUREMENT_PRESCALE__A, &EcRsPrescale );
      RR16 ( devAddr, FEC_RS_MEASUREMENT_PERIOD__A  , &EcRsRegInPckCnt );

      if ( extAttr->packetErrMode == DRXK_PACKET_ERR_ACCUMULATIVE)
      {
         CHK_ERROR( DTVGetAccPktErr ( demod, &EcRsRegErrPckCnt));
      }
      else
      {
         RR16( devAddr, FEC_RS_NR_FAILURES__A, &EcRsRegErrPckCnt );
      }

      EcRsRegErrBitCnt *= EcRsPrescale;

      /* Check input data for post/preViterbi BER calculation */
      /* Check denominators on zero */
      if ( ( EcVdRegInBitCnt == 0 ) || ( EcRsRegInPckCnt == 0 ) )
      {
         return DRX_STS_ERROR;
      };

      /* Check if denominators are smaller than nominators: if so error */
      postViterbiErrBits = (u32_t) EcRsRegErrBitCnt;
      postViterbiDenominator = ((u32_t)EcRsRegInPckCnt) * rsPacketLength;
      if (  ( EcVdRegInBitCnt < EcVdRegErrBitCnt ) ||
            ( postViterbiDenominator < postViterbiErrBits  ) )
      {
         return DRX_STS_ERROR;
      };

      /* Compute desired results */

      /* Number of errorneous packets */
      sigQuality->packetError  = EcRsRegErrPckCnt;

      /* PreViterbi is computed in steps of 10^(-6) */
      sigQuality->preViterbiBER  = FracTimes1e6( EcVdRegErrBitCnt,
                                                 ((u32_t)EcVdRegInBitCnt) );

      /* PostViterbi is computed in steps of 10^(-6) */
      sigQuality->postViterbiBER  = FracTimes1e6( postViterbiErrBits,
                                                  postViterbiDenominator );

      /* BER values are computed in steps of 10^(-6) */
      sigQuality->scaleFactorBER  = ((u32_t)1000000);

      /* PostViterbi in steps of 10^(-7), for quality indicator */
      postViterbiBERHiRes = FracTimes1e6( (10 * (u32_t)hiResBer),
                                          postViterbiDenominator );
   } else {
      /* No Fec lock... */
      if ( extAttr->packetErrMode == DRXK_PACKET_ERR_ACCUMULATIVE)
      {
         CHK_ERROR( DTVGetAccPktErr ( demod, &sigQuality->packetError ));
      }
      else
      {
         sigQuality->packetError  = (u16_t)(-1);
      }
      /* PreViterbi is computed in steps of 10^(-6) */
      sigQuality->preViterbiBER   = 500000UL;

      /* PostViterbi is computed in steps of 10^(-6) */
      sigQuality->postViterbiBER  = 500000UL;

      /* BER values are computed in steps of 10^(-6) */
      sigQuality->scaleFactorBER  = ((u32_t)1000000);

      /* PostViterbi in steps of 10^(-7), for quality indicator */
      postViterbiBERHiRes         = 5000000UL;
   }

   /* At least demod lock */
   /* Read registers for MER calculation */
   /* TODO ERR_I, ERR_Q and ERR_EXP should be retreived via SC
           otherwise race conditions
   */

   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_PWR_OFS__A , &EqRegTdTpsPwrOfs);
   RR16( devAddr, OFDM_EQ_TOP_TD_REQ_SMB_CNT__A , &EqRegTdReqSmbCnt);
   RR16( devAddr, OFDM_EQ_TOP_TD_SQR_ERR_EXP__A , &EqRegTdSqrErrExp);
   RR16( devAddr, OFDM_EQ_TOP_TD_SQR_ERR_I__A   , &regData );
   /* Extend SQR_ERR_I operational range */
   EqRegTdSqrErrI  = (u32_t) regData;
   if ( ( EqRegTdSqrErrExp > 11 ) && ( EqRegTdSqrErrI < 0x00000FFFUL) )
   {
     EqRegTdSqrErrI += 0x00010000UL;
   }
   RR16( devAddr, OFDM_EQ_TOP_TD_SQR_ERR_Q__A   , &regData );
   /* Extend SQR_ERR_Q operational range */
   EqRegTdSqrErrQ  = (u32_t) regData;
   if ( ( EqRegTdSqrErrExp > 11 ) && ( EqRegTdSqrErrQ < 0x00000FFFUL) )
   {
     EqRegTdSqrErrQ += 0x00010000UL;
   }

   RR16( devAddr, OFDM_SC_RA_RAM_OP_PARAM__A, &transmissionParams);

   /* Check input data for MER */

   /* MER calculation (in 0.1 dB) without math.h */
   if (( EqRegTdTpsPwrOfs == 0 ) || (EqRegTdReqSmbCnt == 0 ))
   {
      iMER = 0;
   }
   else if ( ( EqRegTdSqrErrI + EqRegTdSqrErrQ ) == 0 )
   {
      /* No error at all, this must be the HW reset value
       * Apparently no first measurement yet
       * Set MER to 0.0 */
      iMER = 0;
   }
   else
   {
      SqrErrIQ = (EqRegTdSqrErrI + EqRegTdSqrErrQ) << EqRegTdSqrErrExp;
      if ( (transmissionParams & OFDM_SC_RA_RAM_OP_PARAM_MODE__M)
                                       ==OFDM_SC_RA_RAM_OP_PARAM_MODE_2K)
      {
         tpsCnt = 17;
      } else {
         tpsCnt = 68;
      }

      /* IMER = 100 * log10 ( x )
         where x = (EqRegTdTpsPwrOfs^2 * EqRegTdReqSmbCnt * tpsCnt)/SqrErrIQ

         => IMER = a + b -c
         where a = 100 * log10 (EqRegTdTpsPwrOfs^2)
               b = 100 * log10 (EqRegTdReqSmbCnt * tpsCnt)
               c = 100 * log10 (SqrErrIQ)
      */

      /* log(x) x = 9bits * 9bits -> 18 bits  */
      a = Log10Times100( EqRegTdTpsPwrOfs*EqRegTdTpsPwrOfs );
      /* log(x) x = 16bits * 7bits -> 23 bits  */
      b = Log10Times100( EqRegTdReqSmbCnt*tpsCnt );
      /* log(x) x = (16bits + 16bits) << 15  -> 32 bits  */
      c = Log10Times100( SqrErrIQ );

      iMER = a + b;
      /* No negative MER, clip to zero */
      if ( iMER > c )
      {
         iMER -= c;
      } else {
         iMER = 0;
      }
   }

   sigQuality->MER = (u16_t) iMER;

/*-----------------------------------------------------------------------------/
*
*   Teracom's signal quality indicator calculation
*
*
*   Begin Quote: Teracom Terrestrial Receiver Specification v2.0 d.d.2008-08-18
*
*   4.1.1.7. Requirements for the signal quality indicator (SQI)
*
*   The signal quality indicator (SQI) in [%] shall be calculated according to
*   the following formulas:
*
*      SQI = 0                                 if   C/Nrel   < C/Nstart
*      SQI = ((C/Nrel -3)/10) + 1) * BER_SQI   if   C/Nstart <= C/Nrel < C/Ntop
*      SQI = BER_SQI                           if   C/Nrel   >= C/Ntop
*
*   With:
*
*      C/Nrel   = C/Nrec      - C/NNordigP1
*      C/Nstart = C/NNordigP1 - 7 dB
*      C/Ntop   = C/NNordigP1 + 3 dB
*
*   And:
*
*      BER_SQI  = 0                            if          BER > 10-3
*      BER_SQI  = 20*LOG10(1/BER)-40           if   10-7 < BER <= 10-3
*      BER_SQI  = 100                          if          BER <= 10-7
*
*   End Quote: Teracom Terrestrial Receiver Specification v2.0 d.d.2008-08-18
*
*
*   Probably there's a small error in Teracom's SQI description.
*   Therefore the following was implemented:
*
*      SQI = 0                                 if   C/Nrel <  -7
*      SQI = (((C/Nrel -3)/10) + 1) * BER_SQI  if   -7     <=  C/Nrel < 3
*      SQI = BER_SQI                           if   C/Nrel >=  3
*
*   With:
*
*      C/Nrel = MER - C/NNordigP1
*
*
*   Nordig Unified ver 2.0, Table 3.3
*
*         Modulation | C/NNordigP1
*         ------------+-------------
*         QPSK  1/2 |   5.1
*         QPSK  2/3 |   6.9
*         QPSK  3/4 |   7.9
*         QPSK  5/6 |   8.9
*         QPSK  7/8 |   9.7
*         16QAM  1/2 |  10.8
*         16QAM  2/3 |  13.1
*         16QAM  3/4 |  14.6
*         16QAM  5/6 |  15.6
*         16QAM  7/8 |  16.0
*         64QAM  1/2 |  16.5
*         64QAM  2/3 |  18.7
*         64QAM  3/4 |  20.2
*         64QAM  5/6 |  21.6
*         64QAM  7/8 |  22.5
*
*-----------------------------------------------------------------------------*/
   /* Determine CNNordigTab index from transmission parameters.  */
   /* constellation */
   switch ( transmissionParams & OFDM_SC_RA_RAM_OP_PARAM_CONST__M ) {

      case OFDM_SC_RA_RAM_OP_PARAM_CONST_QPSK:
         idx = 0;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_CONST_QAM16:
         idx = 5;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_CONST_QAM64:
         idx = 10;
         break;
      default:
         idx = 10;
         break;
   }
   /* coderate */
   switch ( transmissionParams & OFDM_SC_RA_RAM_OP_PARAM_RATE__M) {
      case OFDM_SC_RA_RAM_OP_PARAM_RATE_1_2  :
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_RATE_2_3  :
         idx += 1;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_RATE_3_4  :
         idx += 2;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_RATE_5_6  :
         idx += 3;
         break;
      case OFDM_SC_RA_RAM_OP_PARAM_RATE_7_8  :
         idx += 4;
         break;
      default:
         idx += 1;
         break;
   }

   /* Calculate BER_SQI */
   if (postViterbiBERHiRes > 10000)
   {
      berSQI = 0;
   }
   else if (postViterbiBERHiRes > 0)
   {
      /* BER_SQI  = 20*LOG10(1/BER) - 40 */
      berSQI = (s32_t) Log10Times100( (u32_t) postViterbiBERHiRes );
      berSQI -= 700;  /* base 10^-7 */
      berSQI /= (-5);    /* *20/100,  -1 for log(1/BER) */
      berSQI -= 40;
      /* Clipping */
      if (berSQI < 0)
      {
         berSQI = 0;
      }
      if (berSQI > 100)
      {
         berSQI = 100;
      }
   }
   else
   {
      berSQI = 100;
   }

   /* Calculate SQI from C/N and BER_SQI */
   CNrel = iMER - CNNordigTab[idx];
   if ( CNrel < -70 )
   {
      SQI = 0;
   }
   else if ( CNrel < 30 )
   {
      /* (((CNrel-3)/10)+1)*berSQI, for CN unit [dB]
         in calculation below the CN unit is 0.1dB */
      SQI = ((CNrel + 70) * berSQI) / 100;
   }
   else
   {
      SQI = berSQI;
   }
   sigQuality->indicator = (u16_t) SQI;

   /* All done */
   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/

/**
* \brief Retreive symbol rate offset in ppm.
* \param demod Pointer to demodulator instance.
* \param deltaSR Pointer to detected ppm deviaton.
* \return DRXStatus_t.
* \retval DRX_STS_OK sigQuality contains valid data.
* \retval DRX_STS_INVALID_ARG deltaSR is NULL.
* \retval DRX_STS_ERROR I2C error or division by zero.
*
*/
static DRXStatus_t
DVBTGetSymbolrateOffset( pDRXDemodInstance_t demod,
                         ps32_t              deltaSR )
{
   pI2CDeviceAddr_t devAddr      = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t extAttr           = (pDRXKData_t)      NULL;
   u32_t symbolFrequencyRatio    = 0;
   u32_t symbolNomFrequencyRatio = 0;
   u32_t den                     = 0;

   devAddr       = demod->myI2CDevAddr;
   extAttr       = (pDRXKData_t) demod->myExtAttr;

   symbolFrequencyRatio = extAttr->iqmRcRateOfs;
   ARR32( devAddr, IQM_RC_RATE_OFS_LO__A, &symbolNomFrequencyRatio );

   den = ( symbolFrequencyRatio + (1 << 23) );
   if ( den == 0 )
   {
      /* Divide by zero */
      return DRX_STS_ERROR;
   }

   if ( symbolFrequencyRatio > symbolNomFrequencyRatio )
   {
      *deltaSR = (s32_t) FracTimes1e6(
                           ( symbolFrequencyRatio - symbolNomFrequencyRatio ),
                           den );
      *deltaSR *= -1;
   }
   else
   {
      *deltaSR = (s32_t) FracTimes1e6(
                           ( symbolNomFrequencyRatio - symbolFrequencyRatio ),
                           den );
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/*==                 END OF DVBT FUNCTIONS                                  ==*/
/*============================================================================*/



/*============================================================================*/
/*==                 START OF QAM FUNCTIONS                                 ==*/
/*============================================================================*/


/**
* \brief Power-up QAM (power down OFDM)
* \param demod instance of demodulator.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
QAMPowerUp (pDRXDemodInstance_t   demod )
{
   pI2CDeviceAddr_t  devAddr     = NULL;
   DRXPowerMode_t    powerMode   = DRXK_POWER_DOWN_OFDM;

   devAddr = demod->myI2CDevAddr;

   CHK_ERROR( CtrlPowerMode( demod, &powerMode) );

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/**
* \brief Power down QAM related blocks.
* \param demod instance of demodulator.
* \param channel pointer to channel data.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMPowerDown(  pDRXDemodInstance_t demod )
{
   pDRXKData_t      extAttr = (pDRXKData_t)      NULL;
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t) NULL;
   DRXKSCUCmd_t     cmdSCU  = { /* command      */ 0,
                                /* parameterLen */ 0,
                                /* resultLen    */ 0,
                                /* *parameter   */ NULL,
                                /* *result      */ NULL };
   u16_t            cmdResult = 0;
   u16_t            data      = 0;

   devAddr  = demod->myI2CDevAddr;
   extAttr  = (pDRXKData_t) demod->myExtAttr;

   RR16( devAddr, SCU_COMM_EXEC__A, &data);
   if ( data == SCU_COMM_EXEC_ACTIVE )
   {
      /*
         STOP demodulator
                 QAM and HW blocks
      */
      /* stop all comm_exec */
      WR16( devAddr, QAM_COMM_EXEC__A, QAM_COMM_EXEC_STOP );
      cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_QAM |
                            SCU_RAM_COMMAND_CMD_DEMOD_STOP;
      cmdSCU.parameterLen = 0;
      cmdSCU.resultLen    = 1;
      cmdSCU.parameter    = NULL;
      cmdSCU.result       = &cmdResult;
      CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );
   }

   /* powerdown AFE                   */
   CHK_ERROR( IQMSetAf( demod, FALSE ) );

   /* Reset Channel Validator */
   extAttr->qamChannelIsSet = FALSE;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Calculate the bandwith using symbolrate and annex.
* \param  symbolrate.
* \param  standard.
* \param  preset (for bandwidth boundaries)
* \return DRXBandwidth_t.
*/
static DRXBandwidth_t
QAMSymbolrateToBandwidth(  pDRXKPresets_t        presets,
                           u32_t                symbolrate,
                           DRXStandard_t        standard  )
{
   u32_t       bandwidth = 0;
   u32_t       rollOff   = 113; /* ITU_C rollOff */

   /* annex A/C dependend roll off */
   if ( standard == DRX_STANDARD_ITU_A)
   {
      rollOff = 115;
   }

   /* Calculate bandwidth from symbolrate */
   bandwidth = symbolrate * rollOff;
   bandwidth = ( bandwidth + 50 ) / 100;

   if ( bandwidth <= presets->qamBWBound6MHz )
   {
      return (DRX_BANDWIDTH_6MHZ);
   }
   else if ( bandwidth <= presets->qamBWBound7MHz )
   {
      return (DRX_BANDWIDTH_7MHZ);
   }

   return (DRX_BANDWIDTH_8MHZ);
}


/*============================================================================*/

/**
* \brief Setup of the QAM Measurement intervals for signal quality
* \param demod instance of demod.
* \param constellation current constellation.
* \return DRXStatus_t.
*
*  NOTE:
*  Take into account that for certain settings the errorcounters can overflow.
*  The implementation does not check this.
*
*/
static DRXStatus_t
QAMSetMeasurement ( pDRXDemodInstance_t demod,
                    DRXConstellation_t  constellation,
                    u32_t               symbolRate )
{
   pDRXKData_t      extAttr  = (pDRXKData_t)      NULL;
   pI2CDeviceAddr_t devAddr  = (pI2CDeviceAddr_t) NULL;
   u32_t fecBitsDesired   = 0;  /* BER accounting period */
   u32_t fecRsPeriodTotal = 0;  /* Total period */
   u16_t fecRsPrescale    = 0;  /* ReedSolomon Measurement Prescale */
   u16_t fecRsPeriod      = 0;  /* Value for corresponding I2C register */

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t) demod->myExtAttr;

   fecRsPrescale  = 1;

   /* fecBitsDesired = symbolRate [kHz] *
                       FrameLenght [ms] *
                       (constellation + 1) *
                       SyncLoss (== 1) *
                       ViterbiLoss (==1)
   */
   switch ( constellation )
   {
      case DRX_CONSTELLATION_QAM16:
         fecBitsDesired = 4 * symbolRate;
         break;
      case DRX_CONSTELLATION_QAM32:
         fecBitsDesired = 5 * symbolRate;
         break;
      case DRX_CONSTELLATION_QAM64:
         fecBitsDesired = 6 * symbolRate;
         break;
      case DRX_CONSTELLATION_QAM128:
         fecBitsDesired = 7 * symbolRate;
         break;
      case DRX_CONSTELLATION_QAM256:
         fecBitsDesired = 8 * symbolRate;
         break;
      default:
         return DRX_STS_INVALID_ARG;
   }
   fecBitsDesired /= 1000;     /* symbolRate [Hz] -> symbolRate [kHz]  */
   fecBitsDesired *= extAttr->qamBERFrameLength; /* meas. period [ms] */

   /* Annex A/C: bits/RsPeriod = 204 * 8 = 1632 */
   /* fecRsPeriodTotal = fecBitsDesired / 1632 */
   fecRsPeriodTotal = ( fecBitsDesired / 1632UL ) + 1;  /* roughly ceil*/

   /* fecRsPeriodTotal =  fecRsPrescale * fecRsPeriod  */
   fecRsPrescale = 1 + (u16_t) ( fecRsPeriodTotal >> 16 );

   /* avoid devide by zero */
   if ( fecRsPrescale != 0 )
   {
   fecRsPeriod   = ( (u16_t) fecRsPeriodTotal + ( fecRsPrescale >> 1 ) ) /
                   fecRsPrescale;
   /* write corresponding registers */
   WR16( devAddr, FEC_RS_MEASUREMENT_PERIOD__A,   fecRsPeriod );
   WR16( devAddr, FEC_RS_MEASUREMENT_PRESCALE__A, fecRsPrescale );
   WR16( devAddr, FEC_OC_SNC_FAIL_PERIOD__A,      fecRsPeriod );
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief QAM16 specific setup
* \param demod instance of demod.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMSetQAM16 ( pDRXDemodInstance_t   demod )
{
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t) NULL;
   devAddr = demod->myI2CDevAddr;


   /* QAM Equalizer Setup */

   /* Equalizer */
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD0__A,  13517 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD1__A,  13517 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD2__A,  13517 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD3__A,  13517 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD4__A,  13517 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD5__A,  13517 );

   /* Decision Feedback Equalizer */
   WR16 ( devAddr, QAM_DQ_QUAL_FUN0__A,  2 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN1__A,  2 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN2__A,  2 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN3__A,  2 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN4__A,  2 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN5__A,  0 );

   WR16( devAddr, QAM_SY_SYNC_HWM__A, 5);
   WR16( devAddr, QAM_SY_SYNC_AWM__A, 4);
   WR16( devAddr, QAM_SY_SYNC_LWM__A, 3);


   /* QAM Slicer Settings */

   WR16 ( devAddr, SCU_RAM_QAM_SL_SIG_POWER__A, DRXK_QAM_SL_SIG_POWER_QAM16 );


   /* QAM Loop Controller Coeficients */

   WR16 ( devAddr, SCU_RAM_QAM_LC_CA_FINE__A,     15 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CA_COARSE__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_FINE__A,     12 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_MEDIUM__A,   24 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_COARSE__A,   24 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_FINE__A,     12 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_MEDIUM__A,   16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_COARSE__A,   16 );

   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_FINE__A,      5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_MEDIUM__A,   20 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_COARSE__A,   80 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_FINE__A,      5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_MEDIUM__A,   20 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_COARSE__A,   50 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_FINE__A,     16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_MEDIUM__A,   16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_COARSE__A,   32 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_FINE__A,     5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_MEDIUM__A,  10 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_COARSE__A,  10 );


   /* QAM State Machine (FSM) Thresholds */

   WR16 ( devAddr, SCU_RAM_QAM_FSM_RTH__A,       140 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_FTH__A,        50 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_CTH__A,        95 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_PTH__A,       120 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_QTH__A,       230 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_MTH__A,       105 );

   WR16 ( devAddr, SCU_RAM_QAM_FSM_RATE_LIM__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_COUNT_LIM__A,   4 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_FREQ_LIM__A,   24 );


   /* QAM FSM Tracking Parameters */

   WR16 ( devAddr, SCU_RAM_QAM_FSM_MEDIAN_AV_MULT__A,  (u16_t)  16 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_RADIUS_AV_LIMIT__A, (u16_t) 220 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET1__A,   (u16_t)  25 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET2__A,   (u16_t)   6 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET3__A,   (u16_t) -24 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET4__A,   (u16_t) -65 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET5__A,   (u16_t)-127 );

   /* Done */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief QAM32 specific setup
* \param demod instance of demod.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMSetQAM32 ( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t) NULL;
   devAddr = demod->myI2CDevAddr;


   /* QAM Equalizer Setup */

   /* Equalizer */
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD0__A,  6707 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD1__A,  6707 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD2__A,  6707 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD3__A,  6707 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD4__A,  6707 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD5__A,  6707 );

   /* Decision Feedback Equalizer */
   WR16 ( devAddr, QAM_DQ_QUAL_FUN0__A,  3 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN1__A,  3 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN2__A,  3 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN3__A,  3 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN4__A,  3 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN5__A,  0 );

   WR16( devAddr, QAM_SY_SYNC_HWM__A, 6);
   WR16( devAddr, QAM_SY_SYNC_AWM__A, 5);
   WR16( devAddr, QAM_SY_SYNC_LWM__A, 3);


   /* QAM Slicer Settings */

   WR16 ( devAddr, SCU_RAM_QAM_SL_SIG_POWER__A, DRXK_QAM_SL_SIG_POWER_QAM32 );


   /* QAM Loop Controller Coeficients */

   WR16 ( devAddr, SCU_RAM_QAM_LC_CA_FINE__A,     15 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CA_COARSE__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_FINE__A,     12 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_MEDIUM__A,   24 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_COARSE__A,   24 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_FINE__A,     12 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_MEDIUM__A,   16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_COARSE__A,   16 );

   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_FINE__A,      5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_MEDIUM__A,   20 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_COARSE__A,   80 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_FINE__A,      5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_MEDIUM__A,   20 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_COARSE__A,   50 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_FINE__A,     16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_MEDIUM__A,   16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_COARSE__A,   16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_FINE__A,     5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_MEDIUM__A,  10 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_COARSE__A,   0 );


   /* QAM State Machine (FSM) Thresholds */

   WR16 ( devAddr, SCU_RAM_QAM_FSM_RTH__A,        90 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_FTH__A,        50 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_CTH__A,        80 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_PTH__A,       100 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_QTH__A,       170 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_MTH__A,       100 );

   WR16 ( devAddr, SCU_RAM_QAM_FSM_RATE_LIM__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_COUNT_LIM__A,   4 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_FREQ_LIM__A,   10 );


   /* QAM FSM Tracking Parameters */

   WR16 ( devAddr, SCU_RAM_QAM_FSM_MEDIAN_AV_MULT__A,  (u16_t)  12 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_RADIUS_AV_LIMIT__A, (u16_t) 140 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET1__A,   (u16_t)  -8 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET2__A,   (u16_t) -16 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET3__A,   (u16_t) -26 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET4__A,   (u16_t) -56 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET5__A,   (u16_t) -86 );

   /* Done */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief QAM64 specific setup
* \param demod instance of demod.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMSetQAM64 ( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t) NULL;
   devAddr = demod->myI2CDevAddr;


   /* QAM Equalizer Setup */

   /* Equalizer */
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD0__A,  13336 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD1__A,  12618 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD2__A,  11988 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD3__A,  13809 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD4__A,  13809 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD5__A,  15609 );

   /* Decision Feedback Equalizer */
   WR16 ( devAddr, QAM_DQ_QUAL_FUN0__A,  4 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN1__A,  4 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN2__A,  4 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN3__A,  4 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN4__A,  3 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN5__A,  0 );

   WR16( devAddr, QAM_SY_SYNC_HWM__A, 5);
   WR16( devAddr, QAM_SY_SYNC_AWM__A, 4);
   WR16( devAddr, QAM_SY_SYNC_LWM__A, 3);


   /* QAM Slicer Settings */

   WR16 ( devAddr, SCU_RAM_QAM_SL_SIG_POWER__A, DRXK_QAM_SL_SIG_POWER_QAM64 );


   /* QAM Loop Controller Coeficients */

   WR16 ( devAddr, SCU_RAM_QAM_LC_CA_FINE__A,     15 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CA_COARSE__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_FINE__A,     12 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_MEDIUM__A,   24 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_COARSE__A,   24 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_FINE__A,     12 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_MEDIUM__A,   16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_COARSE__A,   16 );

   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_FINE__A,      5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_MEDIUM__A,   30 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_COARSE__A,  100 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_FINE__A,      5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_MEDIUM__A,   30 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_COARSE__A,   50 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_FINE__A,     16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_MEDIUM__A,   25 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_COARSE__A,   48 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_FINE__A,     5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_MEDIUM__A,  10 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_COARSE__A,  10 );


   /* QAM State Machine (FSM) Thresholds */

   WR16 ( devAddr, SCU_RAM_QAM_FSM_RTH__A,       100 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_FTH__A,        60 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_CTH__A,        80 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_PTH__A,       110 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_QTH__A,       200 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_MTH__A,        95 );

   WR16 ( devAddr, SCU_RAM_QAM_FSM_RATE_LIM__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_COUNT_LIM__A,   4 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_FREQ_LIM__A,   15 );


   /* QAM FSM Tracking Parameters */

   WR16 ( devAddr, SCU_RAM_QAM_FSM_MEDIAN_AV_MULT__A,  (u16_t)  12 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_RADIUS_AV_LIMIT__A, (u16_t) 141 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET1__A,   (u16_t)   7 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET2__A,   (u16_t)   0 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET3__A,   (u16_t) -15 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET4__A,   (u16_t) -45 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET5__A,   (u16_t) -80 );

   /* Done */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief QAM128 specific setup
* \param demod: instance of demod.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMSetQAM128( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t) NULL;
   devAddr = demod->myI2CDevAddr;


   /* QAM Equalizer Setup */

   /* Equalizer */
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD0__A,  6564 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD1__A,  6598 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD2__A,  6394 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD3__A,  6409 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD4__A,  6656 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD5__A,  7238 );

   /* Decision Feedback Equalizer */
   WR16 ( devAddr, QAM_DQ_QUAL_FUN0__A,  6 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN1__A,  6 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN2__A,  6 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN3__A,  6 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN4__A,  5 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN5__A,  0 );

   WR16( devAddr, QAM_SY_SYNC_HWM__A, 6);
   WR16( devAddr, QAM_SY_SYNC_AWM__A, 5);
   WR16( devAddr, QAM_SY_SYNC_LWM__A, 3);


   /* QAM Slicer Settings */

   WR16 ( devAddr, SCU_RAM_QAM_SL_SIG_POWER__A,DRXK_QAM_SL_SIG_POWER_QAM128 );


   /* QAM Loop Controller Coeficients */

   WR16 ( devAddr, SCU_RAM_QAM_LC_CA_FINE__A,     15 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CA_COARSE__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_FINE__A,     12 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_MEDIUM__A,   24 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_COARSE__A,   24 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_FINE__A,     12 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_MEDIUM__A,   16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_COARSE__A,   16 );

   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_FINE__A,      5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_MEDIUM__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_COARSE__A,  120 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_FINE__A,      5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_MEDIUM__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_COARSE__A,   60 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_FINE__A,     16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_MEDIUM__A,   25 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_COARSE__A,   64 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_FINE__A,     5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_MEDIUM__A,  10 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_COARSE__A,   0 );


   /* QAM State Machine (FSM) Thresholds */

   WR16 ( devAddr, SCU_RAM_QAM_FSM_RTH__A,        50 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_FTH__A,        60 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_CTH__A,        80 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_PTH__A,       100 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_QTH__A,       140 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_MTH__A,       100 );

   WR16 ( devAddr, SCU_RAM_QAM_FSM_RATE_LIM__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_COUNT_LIM__A,   5 );

   WR16 ( devAddr, SCU_RAM_QAM_FSM_FREQ_LIM__A,   12 );

   /* QAM FSM Tracking Parameters */

   WR16 ( devAddr, SCU_RAM_QAM_FSM_MEDIAN_AV_MULT__A,  (u16_t)   8 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_RADIUS_AV_LIMIT__A, (u16_t)  65 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET1__A,   (u16_t)   5 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET2__A,   (u16_t)   3 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET3__A,   (u16_t)  -1 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET4__A,   (u16_t) -12 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET5__A,   (u16_t) -23 );

   /* Done */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief QAM256 specific setup
* \param demod: instance of demod.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMSetQAM256( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t) NULL;
   devAddr = demod->myI2CDevAddr;


   /* QAM Equalizer Setup */

   /* Equalizer */
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD0__A,  11502 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD1__A,  12084 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD2__A,  12543 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD3__A,  12931 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD4__A,  13629 );
   WR16 ( devAddr, SCU_RAM_QAM_EQ_CMA_RAD5__A,  15385 );

   /* Decision Feedback Equalizer */
   WR16 ( devAddr, QAM_DQ_QUAL_FUN0__A,  8 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN1__A,  8 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN2__A,  8 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN3__A,  8 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN4__A,  6 );
   WR16 ( devAddr, QAM_DQ_QUAL_FUN5__A,  0 );

   WR16( devAddr, QAM_SY_SYNC_HWM__A, 5);
   WR16( devAddr, QAM_SY_SYNC_AWM__A, 4);
   WR16( devAddr, QAM_SY_SYNC_LWM__A, 3);


   /* QAM Slicer Settings */

   WR16 ( devAddr, SCU_RAM_QAM_SL_SIG_POWER__A,DRXK_QAM_SL_SIG_POWER_QAM256 );


   /* QAM Loop Controller Coeficients */

   WR16 ( devAddr, SCU_RAM_QAM_LC_CA_FINE__A,     15 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CA_COARSE__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_FINE__A,     12 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_MEDIUM__A,   24 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EP_COARSE__A,   24 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_FINE__A,     12 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_MEDIUM__A,   16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_EI_COARSE__A,   16 );

   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_FINE__A,      5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_MEDIUM__A,   50 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CP_COARSE__A,  250 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_FINE__A,      5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_MEDIUM__A,   50 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CI_COARSE__A,  125 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_FINE__A,     16 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_MEDIUM__A,   25 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF_COARSE__A,   48 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_FINE__A,     5 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_MEDIUM__A,  10 );
   WR16 ( devAddr, SCU_RAM_QAM_LC_CF1_COARSE__A,  10 );


   /* QAM State Machine (FSM) Thresholds */

   WR16 ( devAddr, SCU_RAM_QAM_FSM_RTH__A,        50 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_FTH__A,        60 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_CTH__A,        80 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_PTH__A,       100 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_QTH__A,       150 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_MTH__A,       110 );

   WR16 ( devAddr, SCU_RAM_QAM_FSM_RATE_LIM__A,   40 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_COUNT_LIM__A,   4 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_FREQ_LIM__A,   12 );


   /* QAM FSM Tracking Parameters */

   WR16 ( devAddr, SCU_RAM_QAM_FSM_MEDIAN_AV_MULT__A,  (u16_t)   8 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_RADIUS_AV_LIMIT__A, (u16_t)  74 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET1__A,   (u16_t)  18 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET2__A,   (u16_t)  13 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET3__A,   (u16_t)   7 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET4__A,   (u16_t)   0 );
   WR16 ( devAddr, SCU_RAM_QAM_FSM_LCAVG_OFFSET5__A,   (u16_t)  -8 );

   /* Done */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/

/**
* \brief Reset QAM block.
* \param demod:   instance of demod.
* \param channel: pointer to channel data.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMResetQAM( pI2CDeviceAddr_t devAddr )
{
   u16_t            cmdResult     = 0;
   DRXKSCUCmd_t     cmdSCU        = { /* command      */ 0,
                                      /* parameterLen */ 0,
                                      /* resultLen    */ 0,
                                      /* parameter    */ NULL,
                                      /* result       */ NULL };

   /* Stop QAM comm_exec */
   WR16( devAddr, QAM_COMM_EXEC__A, QAM_COMM_EXEC_STOP);

   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_QAM |
                         SCU_RAM_COMMAND_CMD_DEMOD_RESET;
   cmdSCU.parameterLen = 0;
   cmdSCU.resultLen    = 1;
   cmdSCU.parameter    = NULL;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

   /* All done, all OK */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Set QAM symbolrate.
* \param demod:   instance of demod.
* \param channel: pointer to channel data.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMSetSymbolrate( pDRXDemodInstance_t demod,
                  u32_t               symbolrate )
{
   pDRXKData_t      extAttr    = (pDRXKData_t)      NULL;
   pI2CDeviceAddr_t devAddr    = (pI2CDeviceAddr_t) NULL;
   pDRXCommonAttr_t commonAttr = (pDRXCommonAttr_t) NULL;
   u32_t            adcFrequency  = 0;
   u32_t            symbFreq      = 0;
   u32_t            iqmRcRate     = 0;
   u16_t            ratesel       = 0;
   u32_t            lcSymbRate    = 0;

   devAddr    = demod->myI2CDevAddr;
   extAttr    = (pDRXKData_t) demod->myExtAttr;
   commonAttr = demod->myCommonAttr;

   /* Select & calculate correct IQM rate */
   adcFrequency = ( commonAttr->sysClockFreq * 1000 ) / 3;
   ratesel = 0;
   if ( symbolrate <= 1188750 )
   {
      ratesel = 3;
   }
   else if ( symbolrate <= 2377500 )
   {
      ratesel = 2;
   }
   else if ( symbolrate <= 4755000 )
   {
      ratesel = 1;
   }
   WR16( devAddr, IQM_FD_RATESEL__A, ratesel );

   /*
       IqmRcRate = ( ( Fadc / (symbolrate * (4<<ratesel) ) ) - 1 ) * (1<<23)
   */
   symbFreq = symbolrate * (1 << ratesel);
   if ( symbFreq == 0)
   {
      /* Divide by zero */
      return DRX_STS_ERROR;
   }
   iqmRcRate = ( adcFrequency / symbFreq ) * ( 1 << 21 ) +
               ( Frac28( ( adcFrequency % symbFreq ), symbFreq ) >> 7 ) -
               ( 1 << 23 );
   WR32( devAddr, IQM_RC_RATE_OFS_LO__A, iqmRcRate );
   extAttr->iqmRcRateOfs = iqmRcRate;
   /*
       LcSymbFreq = round ( .125 *  symbolrate / adcFreq * (1<<15) )
   */
   symbFreq = symbolrate;
   if ( adcFrequency == 0)
   {
      /* Divide by zero */
      return DRX_STS_ERROR;
   }
   lcSymbRate = ( symbFreq / adcFrequency ) * ( 1 << 12 ) +
                ( Frac28( ( symbFreq % adcFrequency ), adcFrequency ) >> 16 );
   if ( lcSymbRate > 511 )
   {
      lcSymbRate = 511;
   }
   WR16 ( devAddr, QAM_LC_SYMBOL_FREQ__A, (u16_t) lcSymbRate );

   /* All done, all OK */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/
/**
* \brief Get QAM lock status.
* \param demod:   instance of demod.
* \param channel: pointer to channel data.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMLockStatus( pDRXDemodInstance_t demod,
               pDRXLockStatus_t    lockStat )
{
   pDRXKData_t  extAttr    = (pDRXKData_t) NULL;

   extAttr    = (pDRXKData_t) demod->myExtAttr;

   CHK_ERROR( SCULockStatus( demod, SCU_RAM_COMMAND_STANDARD_QAM, lockStat ) );

   /* All done, all OK */
   return DRX_STS_OK;

rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Set QAM demod.
* \param demod:   instance of demod.
* \param channel: pointer to channel data.
* \return DRXStatus_t.
*/
#define QAM_MIRROR__M         0x03
#define QAM_MIRROR_NORMAL     0x00
#define QAM_MIRRORED          0x01
#define QAM_MIRROR_AUTO_ON    0x02
#define QAM_LOCKRANGE__M      0x10
#define QAM_LOCKRANGE_NORMAL  0x10


static DRXStatus_t
QAMSetQAM( pDRXDemodInstance_t demod,
           pDRXChannel_t       channel,
           DRXFrequency_t      tunerFreqOffset )
{
   pDRXKData_t      extAttr    = (pDRXKData_t)      NULL;
   pI2CDeviceAddr_t devAddr    = (pI2CDeviceAddr_t) NULL;
   pDRXCommonAttr_t commonAttr = (pDRXCommonAttr_t) NULL;
   u16_t            setEnvParameter       = 0;
   u16_t            setParamParameters[4] = {0};
   u16_t            cmdResult     = 0;
   DRXKSCUCmd_t     cmdSCU        = { /* command      */ 0,
                                      /* parameterLen */ 0,
                                      /* resultLen    */ 0,
                                      /* parameter    */ NULL,
                                      /* result       */ NULL };


   devAddr    = demod->myI2CDevAddr;
   extAttr    = (pDRXKData_t) demod->myExtAttr;
   commonAttr = demod->myCommonAttr;

   /* STEP 1:  reset demodulator
              resets FEC DI and FEC RS
              resets QAM block
              resets SCU variables
   */
   WR16( devAddr, FEC_DI_COMM_EXEC__A, FEC_DI_COMM_EXEC_STOP );
   WR16( devAddr, FEC_RS_COMM_EXEC__A, FEC_RS_COMM_EXEC_STOP );
   CHK_ERROR( QAMResetQAM( devAddr ) );

   /*
    STEP 2: configure demodulator
            -set params; initializes some SCU variables
   */
   if( extAttr->standard == DRX_STANDARD_ITU_A )
   {
      /* compatibility mode for A3 ROM */
      if ( DRXK_A3_ROM_CODE(demod) )
      {
         setEnvParameter     =   QAM_TOP_ANNEX_A;
         cmdSCU.command      =   SCU_RAM_COMMAND_STANDARD_QAM |
                                 SCU_RAM_COMMAND_CMD_DEMOD_SET_ENV;
         cmdSCU.parameterLen =   1;
         cmdSCU.resultLen    =   1;
         cmdSCU.parameter    =   &setEnvParameter;
         cmdSCU.result       =   &cmdResult;
         CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );
      }
      else
      {
         setParamParameters[2]   = QAM_TOP_ANNEX_A;

         if (channel->mirror == DRX_MIRROR_AUTO)
         {
            setParamParameters[3] |= (QAM_MIRROR_AUTO_ON);
         }
         else
         {
            if (channel->mirror == DRX_MIRROR_YES)
   {
               setParamParameters[3] |= QAM_MIRRORED;
            }
         }

         if (extAttr->qamLockRange == DRX_QAM_LOCKRANGE_NORMAL)
         {
            setParamParameters[3] |= QAM_LOCKRANGE_NORMAL;
   }

      }

      setParamParameters[0]   = channel->constellation;
      setParamParameters[1]   = DRX_INTERLEAVEMODE_I12_J17;

   }
   else if( extAttr->standard == DRX_STANDARD_ITU_C )
   {
       /* compatibility mode for A3 ROM */
      if ( DRXK_A3_ROM_CODE(demod) )
      {
         setEnvParameter     =   QAM_TOP_ANNEX_C;
   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_QAM |
                         SCU_RAM_COMMAND_CMD_DEMOD_SET_ENV;
   cmdSCU.parameterLen = 1;
   cmdSCU.resultLen    = 1;
         cmdSCU.parameter    =   &setEnvParameter;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );
      }
      else
      {
         setParamParameters[2]   = QAM_TOP_ANNEX_C;

         if (channel->mirror == DRX_MIRROR_AUTO)
         {
            setParamParameters[3] |= (QAM_MIRROR_AUTO_ON);
         }
         else
         {
            if (channel->mirror == DRX_MIRROR_YES)
            {
               setParamParameters[3] |= QAM_MIRRORED;
            }
         }

         if (extAttr->qamLockRange == DRX_QAM_LOCKRANGE_NORMAL)
         {
            setParamParameters[3] |= QAM_LOCKRANGE_NORMAL;
         }

      }

      setParamParameters[0]   = channel->constellation;
      setParamParameters[1]   = DRX_INTERLEAVEMODE_I12_J17;

   }
   else
   {
      return (DRX_STS_INVALID_ARG);
   }

   CHK_ERROR( QAMSetSymbolrate( demod, channel->symbolrate ) );

   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_QAM |
                         SCU_RAM_COMMAND_CMD_DEMOD_SET_PARAM;

   /* Rom code only needs two parameters */
   if ( DRXK_A3_ROM_CODE(demod) )
   {
   cmdSCU.parameterLen = 2;
   }
   else
   {
      cmdSCU.parameterLen = 4;
   }
   cmdSCU.resultLen    = 1;
   cmdSCU.parameter    = setParamParameters;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );


   /*
      STEP 3: setup constellation independent registers

   */

   /* Setup frequency */
   CHK_ERROR ( SetFrequency ( demod, channel, tunerFreqOffset ) );

   /* Setup BER measurement */
   CHK_ERROR( QAMSetMeasurement ( demod,
                                  channel->constellation,
                                  channel->symbolrate ) );

   /* Reset default values */
   WR16( devAddr, IQM_CF_SCALE_SH__A, IQM_CF_SCALE_SH__PRE);
   WR16( devAddr, QAM_SY_TIMEOUT__A,  QAM_SY_TIMEOUT__PRE);

   /* Reset default LC values */
   WR16( devAddr, QAM_LC_RATE_LIMIT__A,  3);
   WR16( devAddr, QAM_LC_LPF_FACTORP__A, 4);
   WR16( devAddr, QAM_LC_LPF_FACTORI__A, 4);
   WR16( devAddr, QAM_LC_MODE__A,        7);

   WR16( devAddr, QAM_LC_QUAL_TAB0__A,   1);
   WR16( devAddr, QAM_LC_QUAL_TAB1__A,   1);
   WR16( devAddr, QAM_LC_QUAL_TAB2__A,   1);
   WR16( devAddr, QAM_LC_QUAL_TAB3__A,   1);
   WR16( devAddr, QAM_LC_QUAL_TAB4__A,   2);
   WR16( devAddr, QAM_LC_QUAL_TAB5__A,   2);
   WR16( devAddr, QAM_LC_QUAL_TAB6__A,   2);
   WR16( devAddr, QAM_LC_QUAL_TAB8__A,   2);
   WR16( devAddr, QAM_LC_QUAL_TAB9__A,   2);
   WR16( devAddr, QAM_LC_QUAL_TAB10__A,  2);
   WR16( devAddr, QAM_LC_QUAL_TAB12__A,  2);
   WR16( devAddr, QAM_LC_QUAL_TAB15__A,  3);
   WR16( devAddr, QAM_LC_QUAL_TAB16__A,  3);
   WR16( devAddr, QAM_LC_QUAL_TAB20__A,  4);
   WR16( devAddr, QAM_LC_QUAL_TAB25__A,  4);


   /* Mirroring, QAM-block starting point not inverted */
   WR16( devAddr, QAM_SY_SP_INV__A, QAM_SY_SP_INV_SPECTRUM_INV_DIS );

   /* Halt SCU to enable safe non-atomic accesses */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_HOLD );

   /*
      STEP 4: constellation specific setup

   */
   switch ( channel->constellation ) {
      case DRX_CONSTELLATION_QAM16:
         CHK_ERROR( QAMSetQAM16( demod ) );
         break;
      case DRX_CONSTELLATION_QAM32:
         CHK_ERROR( QAMSetQAM32( demod ) );
         break;
      case DRX_CONSTELLATION_QAM64:
         CHK_ERROR( QAMSetQAM64( demod ) );
         break;
      case DRX_CONSTELLATION_QAM128:
         CHK_ERROR( QAMSetQAM128( demod ) );
         break;
      case DRX_CONSTELLATION_QAM256:
         CHK_ERROR( QAMSetQAM256( demod ) );
         break;
      default:
         return DRX_STS_ERROR;
         break;
   } /* switch */

   /* Activate SCU to enable SCU commands */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_ACTIVE );


   /* Re-configure MPEG output, requires knowledge of channel bitrate */
   extAttr->currentChannel.constellation = channel->constellation;
   extAttr->currentChannel.symbolrate    = channel->symbolrate;
   CHK_ERROR( MPEGTSDtoSetup ( demod, (extAttr->standard) ) );


   /* Start processes */
   CHK_ERROR( MPEGTSStart( demod ) );
   WR16( devAddr, FEC_COMM_EXEC__A, FEC_COMM_EXEC_ACTIVE );
   WR16( devAddr, QAM_COMM_EXEC__A, QAM_COMM_EXEC_ACTIVE );
   WR16( devAddr, IQM_COMM_EXEC__A, IQM_COMM_EXEC_B_ACTIVE );


   /*
      STEP 5: start QAM demodulator
   */
   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_QAM |
                         SCU_RAM_COMMAND_CMD_DEMOD_START;
   cmdSCU.parameterLen = 0;
   cmdSCU.resultLen    = 1;
   cmdSCU.parameter    = NULL;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

   /* update DRXK data container */
   extAttr->qamInterleaveMode = DRX_INTERLEAVEMODE_I12_J17;

   /* All done, all OK */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief                Check if constellation array contains a constellation
* \param constArray:    Pointer to constellation array
* \param constSize:     Size constellation array
* \param constFind:     Constellation to be detected
* \param found:         Boolean to return if the constellion was found
*                       in constArray
* \return DRXStatus_t.
* \retval DRX_STS_OK:   All done
*
*
*/
static DRXStatus_t
FindConstellation (  pDRXConstellation_t  constArray,
                     u16_t                constSize,
                     DRXConstellation_t   constFind,
                     pBool_t              found )
{
   int i = 0;
   *found = FALSE;

   for (i = 0; i < constSize; i++ )
   {
      if ( constArray[i] == constFind)
      {
         *found = TRUE;
         return DRX_STS_OK;
      }
   }

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Wait for lock while scanning.
* \param demod:    Pointer to demodulator instance.
* \param lockStat: Pointer to DRXLockStatus_t indicating end result lock status
* \return DRXStatus_t.
* \retval DRX_STS_OK:    Success
* \retval DRX_STS_ERROR: I2C failure or bsp function failure.
*
* Wait until timeout, desired lock or NEVER_LOCK.
* Assume:
* - lock function returns : at least DRX_NOT_LOCKED and a lock state
*   higher then DRX_NOT_LOCKED.
* - BSP has a clock function to retrieve a millisecond ticker value.
* - BSP has a sleep function to enable sleep of n millisecond.
*
* In case DRX_NEVER_LOCK is returned the poll-wait will be aborted.
*
*/
static DRXStatus_t
WaitForLock (  pDRXDemodInstance_t demod,
               pDRXLockStatus_t    lockStatus,
               u32_t               timeoutValue  )
{
   DRXLockStatus_t  desiredLockState = DRX_LOCKED;
   u32_t            startTimeLockStage = 0;
   u32_t            timerValue  = 0;

   startTimeLockStage = DRXBSP_HST_Clock();

   /* Start polling loop, checking for lock & timeout */
   do
   {
      timerValue = DRXBSP_HST_Clock() - startTimeLockStage;
      if ( CtrlLockStatus( demod, lockStatus ) != DRX_STS_OK )
      {
         return DRX_STS_ERROR;
      }

      if (  ( *lockStatus >= desiredLockState )  ||
            ( *lockStatus == DRX_NEVER_LOCK )    )
      {
         /* breakout */
         return DRX_STS_OK;
      }

      /* 10msec relief for the host processor */
     // DRXBSP_HST_Sleep( 10 );

   } while ( timerValue < 0/*timeoutValue*/ );

   /* timeout */
   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief QAM specific scanning function.
*
* \param demod:          Pointer to demodulator instance.
* \param scanCommand:    Scanning command: INIT, NEXT or STOP.
* \param scanChannel:    Channel to check: frequency, symbolrate and
*                        constellation
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
ScanFunctionQam   (  void*               scanContext,
                     DRXScanCommand_t    scanCommand,
                     pDRXChannel_t       scanChannel,
                     pBool_t             getNextChannel  )
{
   DRXStatus_t       status      = DRX_STS_ERROR;
   pDRXCommonAttr_t  commAttr    = (pDRXCommonAttr_t) NULL;
   pDRXKData_t       extAttr     = (pDRXKData_t) NULL;
   DRXLockStatus_t   lockStatus  = DRX_NOT_LOCKED;
   u16_t             i           = 0;
   u16_t             j           = 0;

   pDRXDemodInstance_t demod = NULL;

   Bool_t            channelPresent = FALSE;

   DRXKCfgAgc_t   rfAgcConfig;
   DRXKCfgAgc_t   ifAgcConfig;

   pDRXScanDataQam_t scanData = (pDRXScanDataQam_t) NULL;

   demod = (pDRXDemodInstance_t)scanContext;

   commAttr = (pDRXCommonAttr_t) demod->myCommonAttr;
   extAttr  = (pDRXKData_t)      demod->myExtAttr;

   scanData = (pDRXScanDataQam_t)commAttr->scanParam->extParams;

   if (  ( scanData == NULL                 ) ||
         ( scanData->symbolrate == NULL     ) ||
         ( scanData->symbolrateSize <= 0    ) )

   {
      return DRX_STS_INVALID_ARG;
   }

   if (  extAttr->standard != DRX_STANDARD_ITU_C )
   {
      if(( scanData->constellation == NULL  ) ||
         ( scanData->constellationSize <= 0 ) )
      {
         return DRX_STS_INVALID_ARG;
      }
   }

   if ( scanCommand == DRX_SCAN_COMMAND_INIT)
   {
      extAttr->qamScanConstellation = DRX_CONSTELLATION_QAM256;

      if( scanData->constellationSize == 1)
      {
         extAttr->qamScanConstellation =
            scanData->constellation[0];
      }
      else
      {
         if ( extAttr->standard == DRX_STANDARD_ITU_A )
         {
            /* determine if 128QAM is in the search list,
            store in DRXK extAttr field */
            FindConstellation(   scanData->constellation,
                              scanData->constellationSize,
                              DRX_CONSTELLATION_QAM128,
                              &extAttr->qamScanForQam128 );
         }
      }
      return DRX_STS_OK;

   }

   if ( scanCommand == DRX_SCAN_COMMAND_STOP )
   {
      /* just return OK if not doing "scan next" */
      return DRX_STS_OK;
   }

   *getNextChannel = FALSE;

   /* check if any channel power is present,
      use fixed constellation & symbolrate for power measurement */
   scanChannel->constellation    = DRX_CONSTELLATION_QAM64;
   scanChannel->symbolrate       = 6900000;

   status = CtrlSetChannel( demod, scanChannel );
   if ( status != DRX_STS_OK )
   {
      return (status);
   }

   rfAgcConfig.standard = extAttr->standard;
   status = AGCGetRf( demod, &rfAgcConfig );
   if ( status != DRX_STS_OK )
   {
      return (status);
   }

   ifAgcConfig.standard = extAttr->standard;
   status = AGCGetIf( demod, &ifAgcConfig );
   if ( status != DRX_STS_OK )
   {
      return (status);
   }

   if (  ( rfAgcConfig.outputLevel == rfAgcConfig.cutOffCurrent ) &&
         ( ifAgcConfig.outputLevel <  scanData->ifAgcThreshold  ) )
   {
      /* Nothing here, skip to next. */
      *getNextChannel = TRUE;
      return DRX_STS_BUSY;
   }

   /* some power detected, check for valid QAM signals */
   for ( i = 0;
         i < scanData->symbolrateSize;
         i++ )
   {
      scanChannel->symbolrate = scanData->symbolrate[i];

      if ( extAttr->standard == DRX_STANDARD_ITU_C )
      {
         scanChannel->constellation = DRX_CONSTELLATION_QAM64;

         status = CtrlSetChannel( demod, scanChannel );;
         if ( status != DRX_STS_OK )
         {
            return (status);
         }

         status = WaitForLock(   demod,
                                 &lockStatus,
                                 commAttr->scanDemodLockTimeout);
         if ( status != DRX_STS_OK )
         {
            return (status);
         }
         if (lockStatus == DRX_LOCKED)
         {
            /* found a channel */
            *getNextChannel = TRUE;
            return DRX_STS_OK;
         }
      }
      else /* ITU-A */
      {

         /* check for QAM signals with valid symbolrates,
            use QAM256 as "generic" constellations if scanning
            for multiple constellations */
         status = CtrlSetChannel( demod, scanChannel );
         if ( status != DRX_STS_OK )
         {
            return (status);
         }

         status =  WaitForLock(  demod,
                                 &lockStatus,
                                 commAttr->scanDemodLockTimeout );
         if ( status != DRX_STS_OK )
         {
            return (status);
         }

         if  ( lockStatus == DRX_LOCKED )
         {
            /* all done, found a channel */
            *getNextChannel = TRUE;
            return DRX_STS_OK;
         }
         else if ( lockStatus != DRX_NEVER_LOCK )
         {
            /* QAM channel present;
            symbolrate OK (with some probability)
            frequency OK (with some probability) */
            channelPresent = TRUE;
         }
         else
         {
            if ( extAttr->qamScanForQam128 )
            {
               /* additional check for 128QAM;
               not covered by "generic" QAM256 due to false DRX_NEVER_LOCK */
               scanChannel->constellation    = DRX_CONSTELLATION_QAM128;
               status = CtrlSetChannel( demod, scanChannel );
               if ( status != DRX_STS_OK )
               {
                  return (status);
               }

               status =  WaitForLock(  demod,
                                       &lockStatus,
                                       commAttr->scanDemodLockTimeout );
               if ( lockStatus == DRX_LOCKED)
               {
                  /* all done, found a channel */
                  *getNextChannel = TRUE;
                  return DRX_STS_OK;
               }
            }
         }

         /* QAM signal with symbolrate[i] detected, check different
            constellations for full lock */
         if(channelPresent)
         {
            for ( j = 0;
            j < scanData->constellationSize;
            j++ )
            {
               /* DRX_CONSTELLATION_QAM128 already covered above,
                  don't check again to save some time */
               if ( scanData->constellation[j] != DRX_CONSTELLATION_QAM128)
               {
                  scanChannel->constellation = scanData->constellation[j] ;
                  status = CtrlSetChannel( demod, scanChannel );
                  if ( status != DRX_STS_OK )
                  {
                     return (status);
                  }
                  status =  WaitForLock(  demod,
                                          &lockStatus,
                                          commAttr->scanDemodLockTimeout );
                  if ( status != DRX_STS_OK )
                  {
                     return (status);
                  }
                  if (lockStatus == DRX_LOCKED)
                  {
                     /* found a channel */
                     *getNextChannel = TRUE;
                     return DRX_STS_OK;

                  }
               }
            }
         }
      }
   }

   *getNextChannel = TRUE;
   /* no channel found, continue */
   return DRX_STS_BUSY;
}



/*============================================================================*/

/**
* \brief Set QAM standard.
* \param demod   instance of demodulator.
* \param stadard desired QAM standard.
* \return DRXStatus_t.
*/

static DRXStatus_t
QAMSetStandard( pDRXDemodInstance_t demod ,
                pDRXStandard_t      standard )
{
   pDRXKData_t      extAttr    = (pDRXKData_t)      NULL;
   pDRXCommonAttr_t comAttr   = (pDRXCommonAttr_t) NULL;
   pI2CDeviceAddr_t devAddr    = (pI2CDeviceAddr_t) NULL;

   devAddr    = demod->myI2CDevAddr;
   comAttr    = demod->myCommonAttr;
   extAttr    = (pDRXKData_t) demod->myExtAttr;


   /* Ensure correct power-up mode */
   CHK_ERROR( QAMPowerUp( demod ) );

   /* Reset QAM block */
   CHK_ERROR( QAMResetQAM( devAddr ) );


   /* Setup IQM */

   WR16( devAddr, IQM_COMM_EXEC__A, IQM_COMM_EXEC_B_STOP );
   WR16( devAddr, IQM_AF_AMUX__A, IQM_AF_AMUX_SIGNAL2ADC);


   switch ( *standard ) {
      case DRX_STANDARD_ITU_A:

         if (  (extAttr->presets.channelFilter.dataRe == NULL) ||
               (extAttr->presets.channelFilter.dataIm == NULL) )
         {

            /* default filter coefficients */
            /* upload by boot loader from ROM table */
            CHK_ERROR( BLChainCmd(  devAddr,
                                    DRXK_BL_ROM_OFFSET_TAPS_ITU_A,
                                    DRXK_BLCC_NR_ELEMENTS_TAPS) );
         }
         else
         {
            /* load filters from presets */
            CHK_ERROR ( CtrlLoadFilter( demod,
                        &extAttr->presets.channelFilter ));
   }
         break;
      case DRX_STANDARD_ITU_C:
         if (  (extAttr->presets.channelFilter.dataRe == NULL) ||
               (extAttr->presets.channelFilter.dataIm == NULL) )
         {

            /* default filter coefficients */
            /* upload by boot loader from ROM table */
            /* An index is not available, thus using direct mode to transfer
               the data. Data location derived from rom config file,
               same for A1, A2 and A3 */
            CHK_ERROR( BLDirectCmd(    devAddr,
                                       IQM_CF_TAP_RE0__A,
                                       DRXK_BL_ROM_OFFSET_TAPS_ITU_C,
                                       DRXK_BLDC_NR_ELEMENTS_TAPS) );
            CHK_ERROR( BLDirectCmd(    devAddr,
                                       IQM_CF_TAP_IM0__A,
                                       DRXK_BL_ROM_OFFSET_TAPS_ITU_C,
                                       DRXK_BLDC_NR_ELEMENTS_TAPS) );
         }
         else
         {
             /* load filters from presets */
            CHK_ERROR ( CtrlLoadFilter( demod,
                        &extAttr->presets.channelFilter ));
         }

         break;
      default:
         return DRX_STS_INVALID_ARG;
   }

   WR16( devAddr, IQM_CF_OUT_ENA__A, (1 << IQM_CF_OUT_ENA_QAM__B));
   WR16( devAddr, IQM_CF_SYMMETRIC__A, 0);
   WR16( devAddr, IQM_CF_MIDTAP__A,  ( (1 << IQM_CF_MIDTAP_RE__B) |
                                       (1 << IQM_CF_MIDTAP_IM__B) ));

   WR16( devAddr, IQM_RC_STRETCH__A,       21 );
   WR16( devAddr, IQM_AF_CLP_LEN__A,        0 );
   WR16( devAddr, IQM_AF_CLP_TH__A,       448 );
   WR16( devAddr, IQM_AF_SNS_LEN__A,        0 );
   WR16( devAddr, IQM_CF_POW_MEAS_LEN__A,   0 );

   WR16( devAddr, IQM_FS_ADJ_SEL__A,        1 );
   WR16( devAddr, IQM_RC_ADJ_SEL__A,        1 );
   WR16( devAddr, IQM_CF_ADJ_SEL__A,        1 );
   WR16( devAddr, IQM_AF_UPD_SEL__A,        0 );

   /* IQM Impulse Noise Processing Unit */
   WR16 ( devAddr, IQM_CF_CLP_VAL__A,    500 );
   WR16 ( devAddr, IQM_CF_DATATH__A,    1000 );
   WR16 ( devAddr, IQM_CF_BYPASSDET__A,    1 );
   WR16 ( devAddr, IQM_CF_DET_LCT__A,      0 );
   WR16 ( devAddr, IQM_CF_WND_LEN__A,      1 );
   WR16 ( devAddr, IQM_CF_PKDTH__A,        1 );
   WR16 ( devAddr, IQM_AF_INC_BYPASS__A,   1 );


   /* turn on IQMAF. Must be done before setAgc**() */
   CHK_ERROR( IQMSetAf( demod, TRUE ) );
   WR16(devAddr, IQM_AF_START_LOCK__A, 0x01 );

   /* IQM will not be reset from here, sync ADC and update/init AGC */
   CHK_ERROR( ADCSynchronization (demod) );

   /* Set the FSM step period */
   WR16 ( devAddr, SCU_RAM_QAM_FSM_STEP_PERIOD__A, 2000 );

   /* Halt SCU to enable safe non-atomic accesses */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_HOLD );

   /* No more resets of the IQM, current standard correctly set =>
      now AGCs can be configured. */

   extAttr->standard = *standard;
   CHK_ERROR( AGCInit(  demod, FALSE ) );
   CHK_ERROR( AGCCtrlSetCfgPreSaw( demod, &(extAttr->presets.preSawCfg)) );

   /* Configure AGC's */
   CHK_ERROR( AGCSetRf( demod, &(extAttr->presets.rfAgcCfg), FALSE ) );
   CHK_ERROR( AGCSetIf( demod, &(extAttr->presets.ifAgcCfg), FALSE ) );

   /* Activate SCU to enable SCU commands */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_ACTIVE );

   /* Set scanning function pinter */
   comAttr->scanFunction = ScanFunctionQam;


   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}



/*============================================================================*/

/**
* \brief Parameter check for QAM set channel.
* \param demod   instance of demodulator.
* \param channel channel parameters to be checked/modified.
* \return DRXStatus_t.
* \retval DRX_STS_OK          channel parameters are valid.
* \retval DRX_STS_INVALID_ARG illegal channel parameters.
*/
static DRXStatus_t
QAMSetChParamCheck( pDRXDemodInstance_t demod,
                    pDRXChannel_t       channel )
{
   pDRXKData_t extAttr = (pDRXKData_t) NULL;

   extAttr = (pDRXKData_t) demod->myExtAttr;

   if ( extAttr->standard == DRX_STANDARD_ITU_C )
   {
         /* the only ITU C constellation */
         channel->constellation = DRX_CONSTELLATION_QAM64;
      }

   /* Check parameters */
   switch ( channel->constellation )
   {
      case DRX_CONSTELLATION_QAM16  :
      case DRX_CONSTELLATION_QAM32  :
      case DRX_CONSTELLATION_QAM64  :
      case DRX_CONSTELLATION_QAM128 :
      case DRX_CONSTELLATION_QAM256 :
         /* ok */
         break;
      default :
         return DRX_STS_INVALID_ARG;
   }

   if ( ( channel->symbolrate < DRXK_QAM_SYMBOLRATE_MIN ) ||
        ( channel->symbolrate > DRXK_QAM_SYMBOLRATE_MAX ) )
   {
      return DRX_STS_INVALID_ARG;
   }

   channel->bandwidth = QAMSymbolrateToBandwidth(  &extAttr->presets ,
                                                   channel->symbolrate,
                                                   extAttr->standard  );

   switch ( channel->mirror ) 
{
      case DRX_MIRROR_YES  :
      case DRX_MIRROR_NO   :
      case DRX_MIRROR_AUTO :
         /* ok */
         break;
      default :
         return DRX_STS_INVALID_ARG;
         break;
   }

   /* All done, all OK */
   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Set QAM channel according to the requested constellation.
* \param demod:   instance of demod.
* \param channel: pointer to channel data.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMSetChannel( pDRXDemodInstance_t demod,
               pDRXChannel_t       channel,
               DRXFrequency_t      tunerFreqOffset )
{
   pDRXKData_t extAttr = (pDRXKData_t) NULL;

   extAttr = (pDRXKData_t) demod->myExtAttr;

   /* set QAM channel constellation */
   switch ( channel->constellation ) {
      case DRX_CONSTELLATION_QAM16  :
      case DRX_CONSTELLATION_QAM32  :
      case DRX_CONSTELLATION_QAM64  :
      case DRX_CONSTELLATION_QAM128 :
      case DRX_CONSTELLATION_QAM256 :
         extAttr->currentChannel.constellation = channel->constellation;
         CHK_ERROR ( QAMSetQAM( demod, channel, tunerFreqOffset ) );
         break;
      case DRX_CONSTELLATION_AUTO: /* Not supported */
      default:
         return DRX_STS_INVALID_ARG;
   }

   /* store channel properties */
   extAttr->currentChannel  = *channel;
   extAttr->qamChannelIsSet = TRUE;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Get the frequency offset in QAM mode
* \return Error code
*/
static DRXStatus_t
QAMGetFreqOffset ( pDRXDemodInstance_t demod,
                   pDRXFrequency_t     offsetFreq )
{
   pI2CDeviceAddr_t devAddr         = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t extAttr              = (pDRXKData_t)      NULL;
   pDRXCommonAttr_t commonAttr      = (pDRXCommonAttr_t) NULL;
   DRXFrequency_t samplingFrequency = 0;
   s32_t currentFrequency           = 0;
   s32_t nominalFrequency           = 0;
   s32_t carrierFrequencyShift      = 0;
   s32_t sign                       = 1;
   u32_t data64Hi                   = 0;
   u32_t data64Lo                   = 0;

   devAddr     = demod->myI2CDevAddr;
   extAttr     = (pDRXKData_t) demod->myExtAttr;
   commonAttr  = demod->myCommonAttr;

   if ( extAttr->qamChannelIsSet == FALSE )
   {
       /* Avoid lock ups by returning 0 */
      *offsetFreq = 0;
      return DRX_STS_OK;
   }

   samplingFrequency = commonAttr->sysClockFreq/3;

   /* both registers are sign extended */
   ARR32( devAddr, IQM_FS_RATE_LO__A, (pu32_t) &currentFrequency );
   nominalFrequency = extAttr->iqmFsRateOfs;

   if ( extAttr->posImage == TRUE )
   {
      /* positive image */
      carrierFrequencyShift = currentFrequency - nominalFrequency;
   } else {
      /* negative image */
      carrierFrequencyShift = nominalFrequency - currentFrequency;
   }

   /* carrier Frequency Shift In Hz */
   if (carrierFrequencyShift < 0)
   {
      sign = -1;
      carrierFrequencyShift *= sign;
   }

   /* *offsetFreq = carrierFrequencyShift * 50.625e6 / (1 << 28); */
   Mult32 ( carrierFrequencyShift, samplingFrequency, &data64Hi, &data64Lo );
   *offsetFreq = (DRXFrequency_t) ( ( ( (data64Lo >> 28) & 0xf ) |
                                        (data64Hi << 4 ) ) * sign );
   /* Rounding */
   if (data64Lo & ( ((u32_t)1) << 27 ) )
   {
      (*offsetFreq)+= sign * 1;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Get symbol rate offset in QAM
* \return Error code
*/
static DRXStatus_t
QAMGetSymbolrateOffset( pDRXDemodInstance_t demod,
                        ps32_t              deltaSR )
{
   pI2CDeviceAddr_t devAddr      = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t      extAttr      = (pDRXKData_t)      NULL;
   DRXLockStatus_t  lockStatus   = DRX_NOT_LOCKED;
   u32_t symbolFrequencyRatio    = 0;
   u32_t symbolNomFrequencyRatio = 0;
   u32_t den                     = 0;

   devAddr       = demod->myI2CDevAddr;
   extAttr       = (pDRXKData_t) demod->myExtAttr;

   /* Initialize value to 0 */
   *deltaSR = 0;

   if ( extAttr->qamChannelIsSet == FALSE )
   {
       /* Avoid lock ups by returning 0 */
      return DRX_STS_OK;
   }

   CHK_ERROR( QAMLockStatus( demod, &lockStatus) );

   if (lockStatus >= DRXK_DEMOD_LOCK)
   {
      ARR32( devAddr, IQM_RC_RATE_LO__A, &symbolFrequencyRatio );
      symbolNomFrequencyRatio = extAttr->iqmRcRateOfs;
      den = ( symbolFrequencyRatio + (1 << 23) );
      if ( den == 0 )
      {
         /* Divide by zero */
         return DRX_STS_ERROR;
      }

      if ( symbolFrequencyRatio > symbolNomFrequencyRatio )
      {
         *deltaSR = (s32_t) FracTimes1e6(
                              (  symbolFrequencyRatio -
                                 symbolNomFrequencyRatio ),
                              den );
         *deltaSR *= -1;
      }
      else
      {
         *deltaSR = (s32_t) FracTimes1e6(
                              (  symbolNomFrequencyRatio -
                                 symbolFrequencyRatio ),
                              den );
      }
   } /* if (lockStatus >= DRXK_DEMOD_LOCK) */

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/

/**
* \brief Get QAM channel parameters.
* \param demod:   instance of demod.
* \param channel: pointer to channel data.
* \return DRXStatus_t.
*/
static DRXStatus_t
QAMGetChannel( pDRXDemodInstance_t demod,
               pDRXChannel_t       channel )
{
   pDRXKData_t      extAttr      = (pDRXKData_t)      NULL;
   pI2CDeviceAddr_t devAddr      = (pI2CDeviceAddr_t) NULL;
   pDRXCommonAttr_t commonAttr   = (pDRXCommonAttr_t) NULL;
   u32_t            iqmRcRateLo  = 0;
   u32_t            adcFrequency = 0;
   u16_t            ratesel      = 0;
   u16_t            qamSySpInv   = 0;
   DRXFrequency_t   offsetFreq   = 0;

   devAddr    = demod->myI2CDevAddr;
   extAttr    = (pDRXKData_t) demod->myExtAttr;
   commonAttr = demod->myCommonAttr;

   /* get the channel frequency offset */
   CHK_ERROR( QAMGetFreqOffset ( demod, &offsetFreq ) );
   channel->frequency -= offsetFreq;

   /* get the channel symbol rate */
   /*
       IqmRcRate = ( ( Fadc / (symbolrate * (4<<ratesel) ) ) - 1 ) * (1<<23)
   */
   RR16( devAddr, IQM_FD_RATESEL__A, &ratesel );
   ARR32( devAddr, IQM_RC_RATE_LO__A, &iqmRcRateLo );

   adcFrequency = ( commonAttr->sysClockFreq * 1000 ) / 3;
   if ( iqmRcRateLo == 0)
   {
      /* Divide by zero */
      return DRX_STS_ERROR;
   }
   channel->symbolrate = Frac28( adcFrequency, (iqmRcRateLo + (1<<23)) )
                         >> (7 + ratesel);

   /* calculate bandwidth from symbolrate */
   channel->bandwidth = QAMSymbolrateToBandwidth(  &extAttr->presets,
                                                  channel->symbolrate,
                                                  extAttr->standard );

   /* get constellation */
   switch ( extAttr->currentChannel.constellation )
   {
      case DRX_CONSTELLATION_QAM256:
      case DRX_CONSTELLATION_QAM128:
      case DRX_CONSTELLATION_QAM64:
      case DRX_CONSTELLATION_QAM32:
      case DRX_CONSTELLATION_QAM16:
         channel->constellation = extAttr->currentChannel.constellation;
         break;
      default:
         channel->constellation = DRX_CONSTELLATION_UNKNOWN;
         break;
   }

   /* get mirroring */
   RR16( devAddr, QAM_SY_SP_INV__A, &qamSySpInv );
   switch ( extAttr->currentChannel.mirror )
   {
      case DRX_MIRROR_AUTO:
      case DRX_MIRROR_UNKNOWN:
      case DRX_MIRROR_NO:
         channel->mirror = DRX_MIRROR_NO;
         if ( qamSySpInv == QAM_SY_SP_INV_SPECTRUM_INV_ENA )
         {
            channel->mirror = DRX_MIRROR_YES;
         }
         break;

      case DRX_MIRROR_YES:
         channel->mirror = DRX_MIRROR_YES;
         if ( qamSySpInv == QAM_SY_SP_INV_SPECTRUM_INV_ENA )
         {
            channel->mirror = DRX_MIRROR_NO;
         }
         break;
      default:
         channel->mirror = DRX_MIRROR_UNKNOWN;
         break;
   }


   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Retreive QAM signal quality from device.
* \param demod Pointer to demodulator instance.
* \param sigQuality Pointer to signal quality data.
* \return DRXStatus_t.
* \retval DRX_STS_OK sigQuality contains valid data.
* \retval DRX_STS_INVALID_ARG sigQuality is NULL.
* \retval DRX_STS_ERROR Erroneous data, sigQuality contains invalid data.

*  Pre-condition: Device must be started and locked.
*/
static DRXStatus_t
QAMSigQuality( pDRXDemodInstance_t demod,
               pDRXSigQuality_t    sigQuality )
{
   pDRXKData_t        extAttr  = (pDRXKData_t)      NULL;
   pI2CDeviceAddr_t   devAddr  = (pI2CDeviceAddr_t) NULL;
   DRXConstellation_t constellation  = DRX_CONSTELLATION_UNKNOWN;
   DRXLockStatus_t    lockStatus     = DRX_NOT_LOCKED;

   /* MER calculation */
   u16_t qamSlErrPower = 0; /* accum. error between raw and sliced symbols */
   u32_t qamSlSigPower = 0; /* used for MER, depends of QAM constellation */
   u32_t qamSlMer      = 0; /* QAM MER */

   /* BER calculation */
   u32_t qamPreRSBer   = 500000UL; /* Pre Reed-Solomon BER */
   u32_t qamPostRSBer  = 500000UL; /* Post Reed-Solomon BER */
   u16_t fecOcPeriod   = 0;
   u16_t fecRsPrescale = 0;
   u16_t fecRsPeriod   = 0;
   u16_t nrBitErrors   = 0;
   u16_t nrPktErrs     = (u16_t)-1;
   u16_t nrSncParFails = 0;
   u32_t fecRsBitCnt   = 0;

   /* Intermediate variables */
   u32_t e             = 0; /* exponent value used for QAM BER/SER */
   u32_t m             = 0; /* mantisa value used for QAM BER/SER */


   /* device info */
   devAddr       = demod->myI2CDevAddr;
   extAttr       = (pDRXKData_t) demod->myExtAttr;
   constellation = extAttr->currentChannel.constellation;


   /* ------------------------------ */
   /* MER Calculation                */
   /* ------------------------------ */

   /* MER is good if it is above 27.5 for QAM256 or 21.5 for QAM64 */

   CHK_ERROR( QAMLockStatus( demod, &lockStatus) );

   if (lockStatus >= DRXK_DEMOD_LOCK)
   {
      /* get the register value needed for MER */
      RR16( devAddr, QAM_SL_ERR_POWER__A, &qamSlErrPower );

      /* DRXK_QAM_SL_SIG_POWER_QAMxxx  * 4     */
      switch ( constellation )
      {
         case DRX_CONSTELLATION_QAM16:
            qamSlSigPower = DRXK_QAM_SL_SIG_POWER_QAM16 << 2;
            break;
         case DRX_CONSTELLATION_QAM32:
            qamSlSigPower = DRXK_QAM_SL_SIG_POWER_QAM32 << 2;
            break;
         case DRX_CONSTELLATION_QAM64:
            qamSlSigPower = DRXK_QAM_SL_SIG_POWER_QAM64 << 2;
            break;
         case DRX_CONSTELLATION_QAM128:
            qamSlSigPower = DRXK_QAM_SL_SIG_POWER_QAM128 << 2;
            break;
         case DRX_CONSTELLATION_QAM256:
            qamSlSigPower = DRXK_QAM_SL_SIG_POWER_QAM256 << 2;
            break;
         default:
            return DRX_STS_ERROR;
      }

      /* 10.0*log10(qam_sl_sig_power * 4.0 / qam_sl_err_power); */
      if ( qamSlErrPower == 0 )
      {
         qamSlMer = 0;
      }
      else
      {
         qamSlMer = Log10Times100( qamSlSigPower ) -
                    Log10Times100( ( u32_t ) qamSlErrPower );
      }
   } /* if (lockStatus >= DRXK_DEMOD_LOCK) */


   /* ---------------------------------------- */
   /* pre and post ReedSolomon BER Calculation */
   /* ---------------------------------------- */

   /* pre RS BER is good if it is below 3.5e-4 */

   if (lockStatus >= DRXK_DEMOD_LOCK)
   {
      /* readout data */
      RR16( devAddr, FEC_RS_NR_BIT_ERRORS__A,        &nrBitErrors );

      if ( extAttr->packetErrMode == DRXK_PACKET_ERR_AVERAGE)
      {
         RR16( devAddr, FEC_RS_NR_FAILURES__A,          &nrPktErrs );
      }

      RR16( devAddr, FEC_RS_MEASUREMENT_PERIOD__A ,  &fecRsPeriod );
      RR16( devAddr, FEC_RS_MEASUREMENT_PRESCALE__A, &fecRsPrescale );
      RR16( devAddr, FEC_OC_SNC_FAIL_COUNT__A,       &nrSncParFails );
      RR16( devAddr, FEC_OC_SNC_FAIL_PERIOD__A,      &fecOcPeriod );

      /* Annex A/C: bits/RsPeriod = 204 * 8 = 1632 */
      fecRsBitCnt  = (u32_t) fecRsPeriod * (u32_t) fecRsPrescale * 1632UL;


      /* Pre Reed-Solomon BER calculation */

      /* Extract the Exponent and the Mantisa of the nrBitErrors */
      e = (u32_t) ( ( nrBitErrors & FEC_RS_NR_BIT_ERRORS_EXP__M  ) >>
                      FEC_RS_NR_BIT_ERRORS_EXP__B );
      m = (u32_t) ( ( nrBitErrors & FEC_RS_NR_BIT_ERRORS_FIXED_MANT__M ) >>
                      FEC_RS_NR_BIT_ERRORS_FIXED_MANT__B );

      fecRsBitCnt = fecRsBitCnt >> e;
      if ( m > ( fecRsBitCnt >> 1 ) )
      {
         qamPreRSBer = 500000UL;  /* clip BER 0.5 */
      }
      else if ( fecRsBitCnt != 0 )
      {
         qamPreRSBer= FracTimes1e6( m, fecRsBitCnt );
      }
      else
      {
         /* Divide by zero */
         /* -> communication error or QAM BER measurement not setup yet*/
         return DRX_STS_ERROR;
      }


      /* Post Reed-Solomon BER calculation, assuming 1 bit error per Snc Fail */
      if ( fecOcPeriod == 0 )
      {
         /* Divide by zero */
         /* -> communication error or QAM BER measurement not setup yet*/
         return DRX_STS_ERROR;
      }
      qamPostRSBer = FracTimes1e6( (u32_t) nrSncParFails,
                                   (u32_t) fecOcPeriod * 8UL );
   } /* if (lockStatus >= DRXK_FEC_LOCK) */
   if ( extAttr->packetErrMode == DRXK_PACKET_ERR_ACCUMULATIVE)
   {
      CHK_ERROR( DTVGetAccPktErr ( demod, &nrPktErrs));
   }



   /* Calculate abstract quality indication */
   sigQuality->indicator = 0;
   if ( qamPostRSBer == 0 )
   {
      if ( qamPreRSBer <= 350 )
      {
         sigQuality->indicator = 100 - (u16_t) ( qamPreRSBer / 7 );
      }
      else if ( qamPreRSBer <= 5300 )
      {
         sigQuality->indicator =  53 - (u16_t) ( qamPreRSBer / 100 );
      }
   }
   else if ( qamPostRSBer <= 6000 )
   {
      sigQuality->indicator = 40 - (u16_t) ( qamPostRSBer / 150 );
   }


   /* ---------------------------------------- */
   /* Fill out signal quality structure        */
   /* ---------------------------------------- */

   sigQuality->MER                = (u16_t) qamSlMer;
   sigQuality->preViterbiBER      = qamPreRSBer;
   sigQuality->postViterbiBER     = qamPreRSBer;
   sigQuality->postReedSolomonBER = qamPostRSBer;
   sigQuality->scaleFactorBER     = 1000000UL;
   sigQuality->packetError        = nrPktErrs;

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/*==                 END OF QAM FUNCTIONS                                   ==*/
/*============================================================================*/


#ifndef DRXK_EXCLUDE_AUDIO
/*============================================================================*/
/*==                 START OF AUDIO FUNCTIONS                               ==*/
/*============================================================================*/

/*
* \brief Power up AUD.
* \param demod instance of demodulator
* \return DRXStatus_t.
*
*/
static DRXStatus_t
PowerUpAud( pDRXDemodInstance_t  demod,
            Bool_t               setStandard)
{
   DRXAudStandard_t audStandard  =  DRX_AUD_STANDARD_AUTO;
   pI2CDeviceAddr_t devAddr = NULL;

   devAddr = demod -> myI2CDevAddr;

   WR16( devAddr, AUD_TOP_COMM_EXEC__A, AUD_TOP_COMM_EXEC_ACTIVE);
   /* setup TR interface: R/W mode, fifosize=8 */
   WR16( devAddr, AUD_TOP_TR_MDE__A, 8);
   WR16( devAddr, AUD_COMM_EXEC__A, AUD_COMM_EXEC_ACTIVE);

   if ( setStandard == TRUE )
   {
      CHK_ERROR( AUDCtrlSetStandard ( demod, &audStandard ) );
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Power up AUD.
* \param demod instance of demodulator
* \return DRXStatus_t.
*
*/
static DRXStatus_t
PowerDownAud( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t devAddr = NULL;
   pDRXKData_t       extAttr = NULL;

   devAddr = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   WR16( devAddr, AUD_COMM_EXEC__A, AUD_COMM_EXEC_STOP );

   extAttr->audData.audioIsActive = FALSE;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}
/*============================================================================*/
/**
* \brief Get Modus data from audio RAM
* \param demod instance of demodulator
* \param pointer to modus
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDGetModus (  pDRXDemodInstance_t  demod,
               pu16_t               modus )
{
   pI2CDeviceAddr_t  devAddr        = NULL;
   pDRXKData_t       extAttr        = NULL;

   u16_t             rModus      = 0;
   u16_t             rModusHi    = 0;
   u16_t             rModusLo    = 0;

   if ( modus == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   /* Modus register is combined in to RAM location */
   RR16( devAddr, AUD_DEM_RD_MODUS_HI__A, &rModusHi );
   RR16( devAddr, AUD_DEM_RD_MODUS_LO__A, &rModusLo );

   rModus   = ( (rModusHi << 12 ) & AUD_DEM_RD_MODUS_HI__M)
               | ((( rModusLo & AUD_DEM_RD_MODUS_LO__M) ));

   *modus = rModus;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;

}

/*============================================================================*/
/**
* \brief Get audio RDS dat
* \param demod instance of demodulator
* \param pointer to DRXCfgAudRDS_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCfgRDS (   pDRXDemodInstance_t demod,
                     pDRXCfgAudRDS_t     status )
{
   pI2CDeviceAddr_t  addr           = NULL;
   pDRXKData_t       extAttr        = NULL;

   u16_t             rRDSArrayCntInit  = 0;
   u16_t             rRDSArrayCntCheck = 0;
   u16_t             rRDSData          = 0;
   u16_t             RDSDataCnt        = 0;

   addr     = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;


   if ( status == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   status->valid = FALSE;

   RR16( addr, AUD_DEM_RD_RDS_ARRAY_CNT__A, &rRDSArrayCntInit);

   if ( rRDSArrayCntInit ==
         AUD_DEM_RD_RDS_ARRAY_CNT_RDS_ARRAY_CT_RDS_DATA_NOT_VALID )
   {
      /* invalid data */
      return DRX_STS_OK;
   }

   if ( extAttr->audData.rdsDataCounter == rRDSArrayCntInit)
   {
      /* no new data */
      return DRX_STS_OK;
   }

   /* RDS is detected, as long as FM radio is selected assume
      RDS will be available                                    */
   extAttr->audData.rdsDataPresent = TRUE;

   /* new data */
   /* read the data */
   for ( RDSDataCnt = 0;
         RDSDataCnt < AUD_RDS_ARRAY_SIZE;
         RDSDataCnt++)
   {
      RR16 ( addr, AUD_DEM_RD_RDS_DATA__A, &rRDSData );
      status->data[RDSDataCnt] = rRDSData;
   }

   RR16( addr, AUD_DEM_RD_RDS_ARRAY_CNT__A, &rRDSArrayCntCheck);

   if ( rRDSArrayCntCheck == rRDSArrayCntInit )
   {
      status->valid = TRUE;
      extAttr->audData.rdsDataCounter = rRDSArrayCntCheck;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get the current audio carrier detection status
* \param demod instance of demodulator
* \param pointer to AUDCtrlGetStatus
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCarrierDetectStatus (  pDRXDemodInstance_t  demod,
                                 pDRXAudStatus_t      status )
{
   pDRXKData_t       extAttr  = NULL;
   pI2CDeviceAddr_t devAddr   = NULL;

   u16_t             rData    = 0;

   if ( status == NULL)
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr  = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   /* initialize the variables */
   status->carrierA    = FALSE;
   status->carrierB    = FALSE;
   status->nicamStatus = DRX_AUD_NICAM_NOT_DETECTED;
   status->sap         = FALSE;
   status->stereo      = FALSE;

   /* read stereo sound mode indication */
   RR16( devAddr, AUD_DEM_RD_STATUS__A, &rData );

   /* carrier a detected */
   if ( (rData & AUD_DEM_RD_STATUS_STAT_CARR_A__M ) ==
         AUD_DEM_RD_STATUS_STAT_CARR_A_DETECTED )
   {
      status->carrierA = TRUE;
   }

   /* carrier b detected */
   if ( (rData & AUD_DEM_RD_STATUS_STAT_CARR_B__M ) ==
         AUD_DEM_RD_STATUS_STAT_CARR_B_DETECTED )
   {
      status->carrierB = TRUE;
   }
   /* nicam detected */
   if ( (rData & AUD_DEM_RD_STATUS_STAT_NICAM__M) ==
         AUD_DEM_RD_STATUS_STAT_NICAM_NICAM_DETECTED)
   {
      if ((rData & AUD_DEM_RD_STATUS_BAD_NICAM__M) ==
            AUD_DEM_RD_STATUS_BAD_NICAM_OK)
      {
         status->nicamStatus = DRX_AUD_NICAM_DETECTED;
   }
   else
   {
         status->nicamStatus = DRX_AUD_NICAM_BAD;
      }
   }

   /* audio mode bilingual or SAP detected */
   if ( (rData & AUD_DEM_RD_STATUS_STAT_BIL_OR_SAP__M) ==
         AUD_DEM_RD_STATUS_STAT_BIL_OR_SAP_SAP)
   {
      status->sap = TRUE;
   }

   /* stereo detected */
   if ( (rData & AUD_DEM_RD_STATUS_STAT_STEREO__M) ==
         AUD_DEM_RD_STATUS_STAT_STEREO_STEREO)
   {
      status->stereo = TRUE;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
   }


/*============================================================================*/
/**
* \brief Get the current audio status parameters
* \param demod instance of demodulator
* \param pointer to AUDCtrlGetStatus
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetStatus (   pDRXDemodInstance_t  demod,
                     pDRXAudStatus_t      status )
{
   pDRXKData_t       extAttr  = NULL;
   pI2CDeviceAddr_t  devAddr  = NULL;
   DRXCfgAudRDS_t    rds      = { FALSE, {0} };
   u16_t             rData    = 0;

   if ( status == NULL)
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr  = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* carrier detection */
   CHK_ERROR ( AUDCtrlGetCarrierDetectStatus ( demod, status ) );

   /* rds data */
   status->rds = FALSE;
   CHK_ERROR ( AUDCtrlGetCfgRDS ( demod, &rds ) );
   status->rds = extAttr->audData.rdsDataPresent;

   /* fmIdent */
   RR16( devAddr, AUD_DSP_RD_FM_IDENT_VALUE__A, &rData);
   rData >>= AUD_DSP_RD_FM_IDENT_VALUE_FM_IDENT__B;
   status->fmIdent = (s8_t)rData;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get the current volume settings
* \param demod instance of demodulator
* \param pointer to DRXCfgAudVolume_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCfgVolume (   pDRXDemodInstance_t  demod,
                        pDRXCfgAudVolume_t   volume )
     {
   pI2CDeviceAddr_t  devAddr  = NULL;
   pDRXKData_t       extAttr  = NULL;

   u16_t rVolume        = 0;
   u16_t rAVC           = 0;
   u16_t rStrengthLeft  = 0;
   u16_t rStrengthRight = 0;

   if ( volume == NULL )
        {
      return DRX_STS_INVALID_ARG;
        }

   devAddr  = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
        {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
        }

   /* volume */
   volume->mute = extAttr->audData.volume.mute;
   RR16( devAddr, AUD_DSP_WR_VOLUME__A, &rVolume );
   if ( rVolume == 0 )
   {
      volume->mute   = TRUE;
      volume->volume = extAttr->audData.volume.volume;
     }
     else
     {
      volume->mute      = FALSE;
      volume->volume    =  ( ( rVolume & AUD_DSP_WR_VOLUME_VOL_MAIN__M ) >>
                           AUD_DSP_WR_VOLUME_VOL_MAIN__B ) -
                           AUD_VOLUME_ZERO_DB;
      if ( volume->volume < AUD_VOLUME_DB_MIN )
        {
         volume->volume = AUD_VOLUME_DB_MIN;
        }
      if ( volume->volume > AUD_VOLUME_DB_MAX )
      {
         volume->volume = AUD_VOLUME_DB_MAX;
     }
   }

   /* automatic volume control */
   RR16( devAddr, AUD_DSP_WR_AVC__A, &rAVC );

   if( ( rAVC & AUD_DSP_WR_AVC_AVC_ON__M) ==
         AUD_DSP_WR_AVC_AVC_ON_OFF )

      {
      volume->avcMode = DRX_AUD_AVC_OFF;
      }
   else
   {
      switch ( rAVC & AUD_DSP_WR_AVC_AVC_DECAY__M )
      {
         case AUD_DSP_WR_AVC_AVC_DECAY_20_MSEC:
            volume->avcMode   = DRX_AUD_AVC_DECAYTIME_20MS;
      break;
         case AUD_DSP_WR_AVC_AVC_DECAY_8_SEC:
            volume->avcMode   = DRX_AUD_AVC_DECAYTIME_8S;
            break;
         case AUD_DSP_WR_AVC_AVC_DECAY_4_SEC:
            volume->avcMode   = DRX_AUD_AVC_DECAYTIME_4S;
            break;
         case AUD_DSP_WR_AVC_AVC_DECAY_2_SEC:
            volume->avcMode   = DRX_AUD_AVC_DECAYTIME_2S;
            break;
         default:
            return DRX_STS_ERROR;
            break;
      }
   }

   /* max attenuation */
   switch ( rAVC & AUD_DSP_WR_AVC_AVC_MAX_ATT__M )
      {
      case AUD_DSP_WR_AVC_AVC_MAX_ATT_12DB:
         volume->avcMaxAtten = DRX_AUD_AVC_MAX_ATTEN_12DB;
         break;
      case AUD_DSP_WR_AVC_AVC_MAX_ATT_18DB:
         volume->avcMaxAtten = DRX_AUD_AVC_MAX_ATTEN_18DB;
         break;
      case AUD_DSP_WR_AVC_AVC_MAX_ATT_24DB:
         volume->avcMaxAtten = DRX_AUD_AVC_MAX_ATTEN_24DB;
         break;
      default:
         return DRX_STS_ERROR;
         break;
      }

   /* max gain */
   switch ( rAVC & AUD_DSP_WR_AVC_AVC_MAX_GAIN__M )
   {
      case AUD_DSP_WR_AVC_AVC_MAX_GAIN_0DB:
         volume->avcMaxGain = DRX_AUD_AVC_MAX_GAIN_0DB;
         break;
      case AUD_DSP_WR_AVC_AVC_MAX_GAIN_6DB:
         volume->avcMaxGain = DRX_AUD_AVC_MAX_GAIN_6DB;
         break;
      case AUD_DSP_WR_AVC_AVC_MAX_GAIN_12DB:
         volume->avcMaxGain = DRX_AUD_AVC_MAX_GAIN_12DB;
      break;
   default:
         return DRX_STS_ERROR;
         break;
   }

   /* reference level */
   volume->avcRefLevel = (u16_t)( ( rAVC & AUD_DSP_WR_AVC_AVC_REF_LEV__M) >>
                                    AUD_DSP_WR_AVC_AVC_REF_LEV__B );

   /* read qpeak registers and calculate strength of left and right carrier */
   /* quasi peaks formula: QP(dB) = 20 * log( AUD_DSP_RD_QPEAKx / Q(0dB) */
   /* Q(0dB) represents QP value of 0dB (hex value 0x4000) */
   /* left carrier */

   /* QP vaues */
   /* left carrier */
   RR16 (devAddr, AUD_DSP_RD_QPEAK_L__A, &rStrengthLeft);
   volume->strengthLeft = ( ( (s16_t) Log10Times100 ( rStrengthLeft ) ) -
                              AUD_CARRIER_STRENGTH_QP_0DB_LOG10T100 ) / 5;

   /* right carrier */
   RR16 (devAddr, AUD_DSP_RD_QPEAK_R__A, &rStrengthRight);
   volume->strengthRight = ( ( (s16_t) Log10Times100 ( rStrengthRight ) ) -
                              AUD_CARRIER_STRENGTH_QP_0DB_LOG10T100 ) / 5;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/
/**
* \brief Set the current volume settings
* \param demod instance of demodulator
* \param pointer to DRXCfgAudVolume_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlSetCfgVolume (   pDRXDemodInstance_t  demod,
                        pDRXCfgAudVolume_t   volume )
   {
   pI2CDeviceAddr_t  devAddr  = NULL;
   pDRXKData_t       extAttr  = NULL;

   u16_t wVolume        = 0;
   u16_t wAVC           = 0;


   if ( volume == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr  = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   /* volume */
   /* volume range from -60 to 12 (expressed in dB) */
   if (  ( volume->volume < AUD_VOLUME_DB_MIN ) ||
         ( volume->volume > AUD_VOLUME_DB_MAX ) )
   {
      return DRX_STS_INVALID_ARG;
   }

   RR16( devAddr, AUD_DSP_WR_VOLUME__A, &wVolume );

   /* clear the volume mask */
   wVolume &= (u16_t)~AUD_DSP_WR_VOLUME_VOL_MAIN__M;
   if ( volume->mute == TRUE )
   {
      /* mute */
      /* mute overrules volume */
      wVolume |=  (u16_t) ( 0 );

   }
   else
   {
      wVolume |=  (u16_t) ( ( volume->volume + AUD_VOLUME_ZERO_DB ) <<
                              AUD_DSP_WR_VOLUME_VOL_MAIN__B );
   }

   WR16( devAddr, AUD_DSP_WR_VOLUME__A, wVolume );

   /* automatic volume control */
   RR16( devAddr, AUD_DSP_WR_AVC__A, &wAVC );

   /* clear masks that require writing */
   wAVC &= (u16_t) ~AUD_DSP_WR_AVC_AVC_ON__M;
   wAVC &= (u16_t) ~AUD_DSP_WR_AVC_AVC_DECAY__M;

   if ( volume->avcMode == DRX_AUD_AVC_OFF )
   {
      wAVC |= ( AUD_DSP_WR_AVC_AVC_ON_OFF  );
   }
   else
   {

      wAVC |= ( AUD_DSP_WR_AVC_AVC_ON_ON  );

      /* avc decay */
      switch ( volume->avcMode )
      {
         case DRX_AUD_AVC_DECAYTIME_20MS:
            wAVC |= AUD_DSP_WR_AVC_AVC_DECAY_20_MSEC;
            break;
         case DRX_AUD_AVC_DECAYTIME_8S:
            wAVC |= AUD_DSP_WR_AVC_AVC_DECAY_8_SEC;
            break;
         case DRX_AUD_AVC_DECAYTIME_4S:
            wAVC |= AUD_DSP_WR_AVC_AVC_DECAY_4_SEC;
            break;
         case DRX_AUD_AVC_DECAYTIME_2S:
            wAVC |= AUD_DSP_WR_AVC_AVC_DECAY_2_SEC;
            break;
         default:
            return DRX_STS_INVALID_ARG;
      }
   }

   /* max attenuation */
   wAVC &= (u16_t) ~AUD_DSP_WR_AVC_AVC_MAX_ATT__M;
   switch ( volume->avcMaxAtten )
   {
      case DRX_AUD_AVC_MAX_ATTEN_12DB:
         wAVC |=  AUD_DSP_WR_AVC_AVC_MAX_ATT_12DB;
         break;
      case DRX_AUD_AVC_MAX_ATTEN_18DB:
         wAVC |=  AUD_DSP_WR_AVC_AVC_MAX_ATT_18DB;
         break;
      case DRX_AUD_AVC_MAX_ATTEN_24DB:
         wAVC |=  AUD_DSP_WR_AVC_AVC_MAX_ATT_24DB;
         break;
      default:
         return DRX_STS_INVALID_ARG;
   }

   /* max gain */
   wAVC &= (u16_t) ~AUD_DSP_WR_AVC_AVC_MAX_GAIN__M;
   switch ( volume->avcMaxGain )
   {
   case DRX_AUD_AVC_MAX_GAIN_0DB:
         wAVC |=  AUD_DSP_WR_AVC_AVC_MAX_GAIN_0DB;
         break;
      case DRX_AUD_AVC_MAX_GAIN_6DB:
         wAVC |=  AUD_DSP_WR_AVC_AVC_MAX_GAIN_6DB;
         break;
      case DRX_AUD_AVC_MAX_GAIN_12DB:
         wAVC |=  AUD_DSP_WR_AVC_AVC_MAX_GAIN_12DB;
         break;
      default:
         return DRX_STS_INVALID_ARG;
   }

   /* avc reference level */
   if ( volume->avcRefLevel > AUD_MAX_AVC_REF_LEVEL )
   {
      return DRX_STS_INVALID_ARG;
   }

   wAVC &= (u16_t)~AUD_DSP_WR_AVC_AVC_REF_LEV__M;
   wAVC |= (u16_t)( volume->avcRefLevel << AUD_DSP_WR_AVC_AVC_REF_LEV__B );

   WR16( devAddr, AUD_DSP_WR_AVC__A, wAVC );

   /* all done, store config in data structure */
   extAttr->audData.volume   = *volume;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/
/**
* \brief Get the I2S settings
* \param demod instance of demodulator
* \param pointer to DRXCfgI2SOutput_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCfgOutputI2S (   pDRXDemodInstance_t demod,
                           pDRXCfgI2SOutput_t  output )
{
   pI2CDeviceAddr_t  devAddr  = NULL;
   pDRXKData_t       extAttr  = NULL;

   u16_t wI2SConfig     = 0;
   u16_t rI2SFreq       = 0;

   if ( output == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr  = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   RR16( devAddr, AUD_DEM_RD_I2S_CONFIG2__A, &wI2SConfig );
   RR16( devAddr, AUD_DSP_WR_I2S_OUT_FS__A, &rI2SFreq );

   /* I2S mode */
   switch ( wI2SConfig & AUD_DEM_WR_I2S_CONFIG2_I2S_SLV_MST__M )
   {
      case AUD_DEM_WR_I2S_CONFIG2_I2S_SLV_MST_MASTER:
         output->mode = DRX_I2S_MODE_MASTER;
      break;
      case AUD_DEM_WR_I2S_CONFIG2_I2S_SLV_MST_SLAVE:
         output->mode = DRX_I2S_MODE_SLAVE;
      break;
   default:
         return DRX_STS_ERROR;
   }

   /* I2S format */
   switch ( wI2SConfig & AUD_DEM_WR_I2S_CONFIG2_I2S_WS_MODE__M )
   {
      case AUD_DEM_WR_I2S_CONFIG2_I2S_WS_MODE_DELAY:
         output->format = DRX_I2S_FORMAT_WS_ADVANCED;
      break;
      case AUD_DEM_WR_I2S_CONFIG2_I2S_WS_MODE_NO_DELAY:
         output->format = DRX_I2S_FORMAT_WS_WITH_DATA;
      break;
      default:
         return DRX_STS_ERROR;
   }

   /* I2S word length */
   switch ( wI2SConfig & AUD_DEM_WR_I2S_CONFIG2_I2S_WORD_LEN__M )
   {
      case AUD_DEM_WR_I2S_CONFIG2_I2S_WORD_LEN_BIT_16:
         output->wordLength = DRX_I2S_WORDLENGTH_16;
      break;
      case AUD_DEM_WR_I2S_CONFIG2_I2S_WORD_LEN_BIT_32:
         output->wordLength = DRX_I2S_WORDLENGTH_32;
      break;
      default:
         return DRX_STS_ERROR;
   }

   /* I2S polarity */
   switch ( wI2SConfig & AUD_DEM_WR_I2S_CONFIG2_I2S_WS_POL__M )
   {
      case AUD_DEM_WR_I2S_CONFIG2_I2S_WS_POL_LEFT_HIGH:
         output->polarity = DRX_I2S_POLARITY_LEFT;
         break;
      case AUD_DEM_WR_I2S_CONFIG2_I2S_WS_POL_LEFT_LOW:
         output->polarity = DRX_I2S_POLARITY_RIGHT;
      break;
   default:
         return DRX_STS_ERROR;
   }

   /* I2S output enabled */
   if ( ( wI2SConfig & AUD_DEM_WR_I2S_CONFIG2_I2S_ENABLE__M )
            == AUD_DEM_WR_I2S_CONFIG2_I2S_ENABLE_ENABLE )
   {
      output->outputEnable = TRUE;
   }
   else
   {
      output->outputEnable = FALSE;
   }

   if ( rI2SFreq > 0 )
   {
      output->frequency = 6144UL * 48000 / rI2SFreq;
      if ( output->wordLength == DRX_I2S_WORDLENGTH_16 )
      {
         output->frequency *= 2;
      }
   }
   else
   {
      output->frequency = AUD_I2S_FREQUENCY_MAX;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Set the I2S settings
* \param demod instance of demodulator
* \param pointer to DRXCfgI2SOutput_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlSetCfgOutputI2S (   pDRXDemodInstance_t demod,
                           pDRXCfgI2SOutput_t  output )
{
   pI2CDeviceAddr_t  devAddr  = NULL;
   pDRXKData_t       extAttr  = NULL;

   u16_t wI2SConfig     = 0;
   u16_t wI2SPadsDataDa = 0;
   u16_t wI2SPadsDataCl = 0;
   u16_t wI2SPadsDataWs = 0;
   u32_t wI2SFreq       = 0;

   if ( output == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr  = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   RR16( devAddr, AUD_DEM_RD_I2S_CONFIG2__A, &wI2SConfig );

   /* I2S mode */
   wI2SConfig &= (u16_t)~AUD_DEM_WR_I2S_CONFIG2_I2S_SLV_MST__M;

   switch ( output->mode )
   {
   case DRX_I2S_MODE_MASTER:
      wI2SConfig |= AUD_DEM_WR_I2S_CONFIG2_I2S_SLV_MST_MASTER;
      break;
   case DRX_I2S_MODE_SLAVE:
      wI2SConfig |= AUD_DEM_WR_I2S_CONFIG2_I2S_SLV_MST_SLAVE;
      break;
   default:
         return DRX_STS_INVALID_ARG;
   }

   /* I2S format */
   wI2SConfig &= (u16_t)~AUD_DEM_WR_I2S_CONFIG2_I2S_WS_MODE__M;

   switch ( output->format )
   {
   case DRX_I2S_FORMAT_WS_ADVANCED:
      wI2SConfig |= AUD_DEM_WR_I2S_CONFIG2_I2S_WS_MODE_DELAY;
      break;
   case DRX_I2S_FORMAT_WS_WITH_DATA:
      wI2SConfig |= AUD_DEM_WR_I2S_CONFIG2_I2S_WS_MODE_NO_DELAY;
      break;
   default:
         return DRX_STS_INVALID_ARG;
   }

   /* I2S word length */
   wI2SConfig &= (u16_t)~AUD_DEM_WR_I2S_CONFIG2_I2S_WORD_LEN__M;

   switch ( output->wordLength )
   {
   case DRX_I2S_WORDLENGTH_16:
      wI2SConfig |= AUD_DEM_WR_I2S_CONFIG2_I2S_WORD_LEN_BIT_16;
      break;
   case DRX_I2S_WORDLENGTH_32:
      wI2SConfig |= AUD_DEM_WR_I2S_CONFIG2_I2S_WORD_LEN_BIT_32;
      break;
   default:
         return DRX_STS_INVALID_ARG;
   }

   /* I2S polarity */
   wI2SConfig &= (u16_t)~AUD_DEM_WR_I2S_CONFIG2_I2S_WS_POL__M;
   switch ( output->polarity )
   {
   case DRX_I2S_POLARITY_LEFT:
      wI2SConfig |= AUD_DEM_WR_I2S_CONFIG2_I2S_WS_POL_LEFT_HIGH;
      break;
   case DRX_I2S_POLARITY_RIGHT:
      wI2SConfig |= AUD_DEM_WR_I2S_CONFIG2_I2S_WS_POL_LEFT_LOW;
      break;
   default:
         return DRX_STS_INVALID_ARG;
   }

   /* I2S output enabled */
   wI2SConfig &= (u16_t)~AUD_DEM_WR_I2S_CONFIG2_I2S_ENABLE__M;
   if ( output->outputEnable == TRUE )
   {
      wI2SConfig |= AUD_DEM_WR_I2S_CONFIG2_I2S_ENABLE_ENABLE;
   }
   else
   {
      wI2SConfig |= AUD_DEM_WR_I2S_CONFIG2_I2S_ENABLE_DISABLE;
   }

   /*
      I2S frequency

      wI2SFreq = 6144 * 48000 * nrbits / ( 32 * frequency )

      16bit: 6144 * 48000 / ( 2 * freq ) = ( 6144 * 48000 / freq ) / 2
      32bit: 6144 * 48000 / freq         = ( 6144 * 48000 / freq )
   */
   if ( (   output->frequency > AUD_I2S_FREQUENCY_MAX ) ||
            output->frequency < AUD_I2S_FREQUENCY_MIN )
   {
      return DRX_STS_INVALID_ARG;
   }

   wI2SFreq = (6144UL * 48000UL) + (output->frequency >> 1);
   wI2SFreq /= output->frequency;

   if ( output->wordLength == DRX_I2S_WORDLENGTH_16 )
   {
      wI2SFreq *= 2;
   }

   WR16( devAddr, AUD_DEM_WR_I2S_CONFIG2__A, wI2SConfig );
   WR16( devAddr, AUD_DSP_WR_I2S_OUT_FS__A, (u16_t) wI2SFreq );

   /* configure I2S output pads for master or slave mode */
   WR16( devAddr, SIO_TOP_COMM_KEY__A, SIO_TOP_COMM_KEY_KEY );

   if (output->mode == DRX_I2S_MODE_MASTER)
   {
      wI2SPadsDataDa = SIO_PDR_I2S_DA_CFG_MODE__MASTER |
         SIO_PDR_I2S_DA_CFG_DRIVE__MASTER;
      wI2SPadsDataCl = SIO_PDR_I2S_CL_CFG_MODE__MASTER |
         SIO_PDR_I2S_CL_CFG_DRIVE__MASTER;
      wI2SPadsDataWs = SIO_PDR_I2S_WS_CFG_MODE__MASTER |
         SIO_PDR_I2S_WS_CFG_DRIVE__MASTER;
   }
   else
   {
      wI2SPadsDataDa = SIO_PDR_I2S_DA_CFG_MODE__SLAVE |
         SIO_PDR_I2S_DA_CFG_DRIVE__SLAVE;
      wI2SPadsDataCl = SIO_PDR_I2S_CL_CFG_MODE__SLAVE |
         SIO_PDR_I2S_CL_CFG_DRIVE__SLAVE;
      wI2SPadsDataWs = SIO_PDR_I2S_WS_CFG_MODE__SLAVE |
         SIO_PDR_I2S_WS_CFG_DRIVE__SLAVE;
   }

   WR16( devAddr, SIO_PDR_I2S_DA_CFG__A, wI2SPadsDataDa );
   WR16( devAddr, SIO_PDR_I2S_CL_CFG__A, wI2SPadsDataCl );
   WR16( devAddr, SIO_PDR_I2S_WS_CFG__A, wI2SPadsDataWs );

   WR16( devAddr, SIO_TOP_COMM_KEY__A, SIO_TOP_COMM_KEY__PRE );


   /* all done, store config in data structure */
   extAttr->audData.i2sdata   = *output;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get the Automatic Standard Select (ASS)
*        and Automatic Sound Change (ASC)
* \param demod instance of demodulator
* \param pointer to pDRXAudAutoSound_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCfgAutoSound  (  pDRXDemodInstance_t     demod,
                           pDRXCfgAudAutoSound_t   autoSound )
      {
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t rModus   = 0;

   if ( autoSound == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   CHK_ERROR ( AUDGetModus ( demod, &rModus ));

   switch ( rModus & (  AUD_DEM_WR_MODUS_MOD_ASS__M |
                        AUD_DEM_WR_MODUS_MOD_DIS_STD_CHG__M ) )
   {
      case  AUD_DEM_WR_MODUS_MOD_ASS_OFF |
            AUD_DEM_WR_MODUS_MOD_DIS_STD_CHG_DISABLED:
      case AUD_DEM_WR_MODUS_MOD_ASS_OFF |
            AUD_DEM_WR_MODUS_MOD_DIS_STD_CHG_ENABLED:
         *autoSound = DRX_AUD_AUTO_SOUND_OFF;
         break;
      case  AUD_DEM_WR_MODUS_MOD_ASS_ON |
            AUD_DEM_WR_MODUS_MOD_DIS_STD_CHG_ENABLED:
         *autoSound = DRX_AUD_AUTO_SOUND_SELECT_ON_CHANGE_ON;
         break;
      case  AUD_DEM_WR_MODUS_MOD_ASS_ON |
            AUD_DEM_WR_MODUS_MOD_DIS_STD_CHG_DISABLED:
         *autoSound = DRX_AUD_AUTO_SOUND_SELECT_ON_CHANGE_OFF;
         break;
      default:
         return DRX_STS_ERROR;
   }


   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
      }
/*============================================================================*/
/**
* \brief Set the Automatic Standard Select (ASS)
*        and Automatic Sound Change (ASC)
* \param demod instance of demodulator
* \param pointer to pDRXAudAutoSound_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrSetlCfgAutoSound  (  pDRXDemodInstance_t  demod,
                           pDRXCfgAudAutoSound_t   autoSound )
{
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t rModus   = 0;
   u16_t wModus   = 0;

   if ( autoSound == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
      {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }


   CHK_ERROR ( AUDGetModus ( demod, &rModus ));

   wModus = rModus;
   /* clear ASS & ASC bits */
   wModus &= (u16_t)~AUD_DEM_WR_MODUS_MOD_ASS__M;
   wModus &= (u16_t)~AUD_DEM_WR_MODUS_MOD_DIS_STD_CHG__M;

   switch ( *autoSound )
   {
      case DRX_AUD_AUTO_SOUND_OFF:
         wModus |=  AUD_DEM_WR_MODUS_MOD_ASS_OFF;
         wModus |=  AUD_DEM_WR_MODUS_MOD_DIS_STD_CHG_DISABLED;
         break;
      case DRX_AUD_AUTO_SOUND_SELECT_ON_CHANGE_ON:
         wModus |=  AUD_DEM_WR_MODUS_MOD_ASS_ON;
         wModus |=  AUD_DEM_WR_MODUS_MOD_DIS_STD_CHG_ENABLED;
         break;
      case DRX_AUD_AUTO_SOUND_SELECT_ON_CHANGE_OFF:
         wModus |=  AUD_DEM_WR_MODUS_MOD_ASS_ON;
         wModus |=  AUD_DEM_WR_MODUS_MOD_DIS_STD_CHG_DISABLED;
         break;
      default:
         return DRX_STS_INVALID_ARG;
      }

   if ( wModus != rModus )
      {
      WR16( devAddr, AUD_DEM_WR_MODUS__A, wModus );
   }
   /* copy to data structure */
   extAttr->audData.autoSound = *autoSound;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get the Automatic Standard Select thresholds
* \param demod instance of demodulator
* \param pointer to pDRXAudASSThres_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCfgASSThres  (   pDRXDemodInstance_t  demod,
                           pDRXCfgAudASSThres_t    thres )
{
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t thresA2 = 0;
   u16_t thresBtsc = 0;
   u16_t thresNicam = 0;

   if ( thres == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   RR16( devAddr , AUD_DEM_RD_A2_THRSHLD__A,      &thresA2 );
   RR16( devAddr , AUD_DEM_RD_BTSC_THRSHLD__A,    &thresBtsc );
   RR16( devAddr , AUD_DEM_RD_NICAM_THRSHLD__A,   &thresNicam );

   thres->a2     = thresA2;
   thres->btsc   = thresBtsc;
   thres->nicam  = thresNicam;


   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get the Automatic Standard Select thresholds
* \param demod instance of demodulator
* \param pointer to pDRXAudASSThres_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlSetCfgASSThres  (   pDRXDemodInstance_t     demod,
                           pDRXCfgAudASSThres_t    thres )
{
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   if ( thres == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   WR16( devAddr , AUD_DEM_WR_A2_THRSHLD__A,      thres->a2 );
   WR16( devAddr , AUD_DEM_WR_BTSC_THRSHLD__A,    thres->btsc );
   WR16( devAddr , AUD_DEM_WR_NICAM_THRSHLD__A,   thres->nicam );

   /* update DRXK data structure with hardware values */
   extAttr->audData.assThresholds = *thres;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get Audio Carrier settings
* \param demod instance of demodulator
* \param pointer to pDRXAudCarrier_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCfgCarrier (  pDRXDemodInstance_t     demod,
                        pDRXCfgAudCarriers_t    carriers )
{
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t wModus   = 0;

   u16_t dcoAHi = 0;
   u16_t dcoALo = 0;
   u16_t dcoBHi = 0;
   u16_t dcoBLo = 0;

   u32_t valA = 0;
   u32_t valB = 0;

   u16_t dcLvlA = 0;
   u16_t dcLvlB = 0;

   u16_t cmThesA = 0;
   u16_t cmThesB = 0;

   if ( carriers == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   CHK_ERROR ( AUDGetModus ( demod, &wModus ));

   /* Behaviour of primary audio channel */
   switch ( wModus & ( AUD_DEM_WR_MODUS_MOD_CM_A__M) )
   {
      case AUD_DEM_WR_MODUS_MOD_CM_A_MUTE:
         carriers->a.opt = DRX_NO_CARRIER_MUTE;
         break;
      case AUD_DEM_WR_MODUS_MOD_CM_A_NOISE:
         carriers->a.opt = DRX_NO_CARRIER_NOISE;
         break;
      default:
         return DRX_STS_ERROR;
         break;
   }

   /* Behaviour of secondary audio channel */
   switch ( wModus & ( AUD_DEM_WR_MODUS_MOD_CM_B__M) )
   {
      case AUD_DEM_WR_MODUS_MOD_CM_B_MUTE:
         carriers->b.opt = DRX_NO_CARRIER_MUTE;
         break;
      case AUD_DEM_WR_MODUS_MOD_CM_B_NOISE:
         carriers->b.opt = DRX_NO_CARRIER_NOISE;
         break;
      default:
         return DRX_STS_ERROR;
         break;
   }

   /* frequency adjustment for primary & secondary audio channel */
   RR16( devAddr, AUD_DEM_RD_DCO_A_HI__A, &dcoAHi );
   RR16( devAddr, AUD_DEM_RD_DCO_A_LO__A, &dcoALo );
   RR16( devAddr, AUD_DEM_RD_DCO_B_HI__A, &dcoBHi );
   RR16( devAddr, AUD_DEM_RD_DCO_B_LO__A, &dcoBLo );

   valA = ( ( (u32_t) dcoAHi) << 12 ) | ( (u32_t) dcoALo & 0xFFF );
   valB = ( ( (u32_t) dcoBHi) << 12 ) | ( (u32_t) dcoBLo & 0xFFF );

   /* Multiply by 20250 * 1>>24  ~= 2 / 1657 */
   carriers->a.dco = DRX_S24TODRXFREQ( valA ) * 2L / 1657L;
   carriers->b.dco = DRX_S24TODRXFREQ( valB ) * 2L / 1657L;

   /* DC level of the incoming FM signal on the primary
      & seconday sound channel */
   RR16( devAddr, AUD_DSP_RD_FM_DC_LEVEL_A__A, &dcLvlA );
   RR16( devAddr, AUD_DSP_RD_FM_DC_LEVEL_B__A, &dcLvlB );

   /* offset (kHz) = (dcLvl / 322) */
   carriers->a.shift = ( DRX_U16TODRXFREQ( dcLvlA ) / 322L );
   carriers->b.shift = ( DRX_U16TODRXFREQ( dcLvlB ) / 322L );

   /* Carrier detetcion threshold for primary & secondary channel */
   RR16( devAddr, AUD_DEM_RD_CM_A_THRSHLD__A, &cmThesA);
   RR16( devAddr, AUD_DEM_RD_CM_B_THRSHLD__A, &cmThesB);

   carriers->a.thres = cmThesA;
   carriers->b.thres = cmThesB;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
      }

/*============================================================================*/
/**
* \brief Set Audio Carrier settings
* \param demod instance of demodulator
* \param pointer to pDRXAudCarrier_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlSetCfgCarrier (  pDRXDemodInstance_t     demod,
                        pDRXCfgAudCarriers_t    carriers )
{
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t wModus   = 0;
   u16_t rModus   = 0;

   u16_t dcoAHi = 0;
   u16_t dcoALo = 0;
   u16_t dcoBHi = 0;
   u16_t dcoBLo = 0;

   s32_t valA = 0;
   s32_t valB = 0;

   if ( carriers == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }


   CHK_ERROR ( AUDGetModus ( demod, &rModus ));


   wModus = rModus;
   wModus &= (u16_t)~AUD_DEM_WR_MODUS_MOD_CM_A__M;
   /* Behaviour of primary audio channel */
   switch ( carriers->a.opt )
   {
      case DRX_NO_CARRIER_MUTE:
         wModus |= AUD_DEM_WR_MODUS_MOD_CM_A_MUTE;
      break;
      case DRX_NO_CARRIER_NOISE:
         wModus |= AUD_DEM_WR_MODUS_MOD_CM_A_NOISE;
      break;
      default:
         return DRX_STS_INVALID_ARG;
      break;
   }

   /* Behaviour of secondary audio channel */
   wModus &= (u16_t)~AUD_DEM_WR_MODUS_MOD_CM_B__M;
   switch ( carriers->b.opt )
   {
      case DRX_NO_CARRIER_MUTE:
         wModus |= AUD_DEM_WR_MODUS_MOD_CM_B_MUTE;
      break;
      case DRX_NO_CARRIER_NOISE:
         wModus |= AUD_DEM_WR_MODUS_MOD_CM_B_NOISE;
      break;
      default:
         return DRX_STS_INVALID_ARG;
      break;
   }

   /* now update the modus register */
   if ( wModus != rModus)
   {
      WR16( devAddr, AUD_DEM_WR_MODUS__A, wModus );
   }

   /* frequency adjustment for primary & secondary audio channel */
   valA = (s32_t) ( ( carriers->a.dco ) * 1657L / 2);
   valB = (s32_t) ( ( carriers->b.dco ) * 1657L / 2);

   dcoAHi = (u16_t) ( ( valA >> 12 ) & 0xFFF );
   dcoALo = (u16_t) ( valA & 0xFFF );
   dcoBHi = (u16_t) ( ( valB >> 12 ) & 0xFFF );
   dcoBLo = (u16_t) ( valB & 0xFFF );

   WR16( devAddr, AUD_DEM_WR_DCO_A_HI__A, dcoAHi );
   WR16( devAddr, AUD_DEM_WR_DCO_A_LO__A, dcoALo );
   WR16( devAddr, AUD_DEM_WR_DCO_B_HI__A, dcoBHi );
   WR16( devAddr, AUD_DEM_WR_DCO_B_LO__A, dcoBLo );

   /* Carrier detetcion threshold for primary & secondary channel */
   WR16( devAddr, AUD_DEM_WR_CM_A_THRSHLD__A, carriers->a.thres);
   WR16( devAddr, AUD_DEM_WR_CM_B_THRSHLD__A, carriers->b.thres);

   /* update DRXK data structure */
   extAttr->audData.carriers = *carriers;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get I2S Source, I2S matrix and FM matrix
* \param demod instance of demodulator
* \param pointer to pDRXAudmixer_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCfgMixer ( pDRXDemodInstance_t  demod,
                     pDRXCfgAudMixer_t    mixer )
{
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t srcI2SMatr  = 0;
   u16_t fmMatr      = 0;

   if ( mixer == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   /* Source Selctor */
   RR16( devAddr, AUD_DSP_WR_SRC_I2S_MATR__A, &srcI2SMatr);

   switch ( srcI2SMatr & AUD_DSP_WR_SRC_I2S_MATR_SRC_I2S__M )
   {
      case AUD_DSP_WR_SRC_I2S_MATR_SRC_I2S_MONO:
         mixer->sourceI2S = DRX_AUD_SRC_MONO;
         break;
      case AUD_DSP_WR_SRC_I2S_MATR_SRC_I2S_STEREO_AB:
         mixer->sourceI2S = DRX_AUD_SRC_STEREO_OR_AB;
         break;
      case AUD_DSP_WR_SRC_I2S_MATR_SRC_I2S_STEREO_A:
         mixer->sourceI2S = DRX_AUD_SRC_STEREO_OR_A;
         break;
      case AUD_DSP_WR_SRC_I2S_MATR_SRC_I2S_STEREO_B:
         mixer->sourceI2S = DRX_AUD_SRC_STEREO_OR_B;
      break;
   default:
         return DRX_STS_ERROR;
   }

   /* Matrix */
   switch ( srcI2SMatr & AUD_DSP_WR_SRC_I2S_MATR_MAT_I2S__M )
   {
      case AUD_DSP_WR_SRC_I2S_MATR_MAT_I2S_MONO:
         mixer->matrixI2S = DRX_AUD_I2S_MATRIX_MONO;
      break;
      case AUD_DSP_WR_SRC_I2S_MATR_MAT_I2S_STEREO:
         mixer->matrixI2S = DRX_AUD_I2S_MATRIX_STEREO;
      break;
      case AUD_DSP_WR_SRC_I2S_MATR_MAT_I2S_SOUND_A:
         mixer->matrixI2S = DRX_AUD_I2S_MATRIX_A_MONO;
      break;
      case AUD_DSP_WR_SRC_I2S_MATR_MAT_I2S_SOUND_B:
         mixer->matrixI2S = DRX_AUD_I2S_MATRIX_B_MONO;
      break;
   default:
         return DRX_STS_ERROR;
   }

   /* FM Matrix */
   RR16( devAddr, AUD_DSP_WR_FM_MATRIX__A, &fmMatr );
   switch ( fmMatr & AUD_DSP_WR_FM_MATRIX__M )
   {
      case AUD_DSP_WR_FM_MATRIX_NO_MATRIX:
         mixer->matrixFm = DRX_AUD_FM_MATRIX_NO_MATRIX;
      break;
      case AUD_DSP_WR_FM_MATRIX_GERMAN_MATRIX:
         mixer->matrixFm = DRX_AUD_FM_MATRIX_GERMAN;
      break;
      case AUD_DSP_WR_FM_MATRIX_KOREAN_MATRIX:
         mixer->matrixFm = DRX_AUD_FM_MATRIX_KOREAN;
      break;
      case AUD_DSP_WR_FM_MATRIX_SOUND_A:
         mixer->matrixFm = DRX_AUD_FM_MATRIX_SOUND_A;
         break;
      case AUD_DSP_WR_FM_MATRIX_SOUND_B:
         mixer->matrixFm = DRX_AUD_FM_MATRIX_SOUND_B;
      break;
   default:
         return DRX_STS_ERROR;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
   }

/*============================================================================*/
/**
* \brief Set I2S Source, I2S matrix and FM matrix
* \param demod instance of demodulator
* \param pointer to DRXAudmixer_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlSetCfgMixer (    pDRXDemodInstance_t  demod,
                        pDRXCfgAudMixer_t    mixer )
{
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t srcI2SMatr  = 0;
   u16_t fmMatr      = 0;

   if ( mixer == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
      {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
      }

   /* Source Selctor */
   RR16( devAddr, AUD_DSP_WR_SRC_I2S_MATR__A, &srcI2SMatr);
   srcI2SMatr &= (u16_t)~AUD_DSP_WR_SRC_I2S_MATR_SRC_I2S__M;

   switch (mixer->sourceI2S)
   {
      case DRX_AUD_SRC_MONO:
         srcI2SMatr |= AUD_DSP_WR_SRC_I2S_MATR_SRC_I2S_MONO;
      break;
      case DRX_AUD_SRC_STEREO_OR_AB:
         srcI2SMatr |= AUD_DSP_WR_SRC_I2S_MATR_SRC_I2S_STEREO_AB;
      break;
      case DRX_AUD_SRC_STEREO_OR_A:
         srcI2SMatr |= AUD_DSP_WR_SRC_I2S_MATR_SRC_I2S_STEREO_A;
      break;
      case DRX_AUD_SRC_STEREO_OR_B:
         srcI2SMatr |= AUD_DSP_WR_SRC_I2S_MATR_SRC_I2S_STEREO_B;
      break;
      default:
         return DRX_STS_INVALID_ARG;
   }

   /* Matrix */
   srcI2SMatr &= (u16_t)~AUD_DSP_WR_SRC_I2S_MATR_MAT_I2S__M;
   switch (mixer->matrixI2S)
   {
      case DRX_AUD_I2S_MATRIX_MONO:
         srcI2SMatr |= AUD_DSP_WR_SRC_I2S_MATR_MAT_I2S_MONO;
      break;
      case DRX_AUD_I2S_MATRIX_STEREO:
         srcI2SMatr |= AUD_DSP_WR_SRC_I2S_MATR_MAT_I2S_STEREO ;
      break;
      case DRX_AUD_I2S_MATRIX_A_MONO:
         srcI2SMatr |= AUD_DSP_WR_SRC_I2S_MATR_MAT_I2S_SOUND_A;
      break;
      case DRX_AUD_I2S_MATRIX_B_MONO:
         srcI2SMatr |= AUD_DSP_WR_SRC_I2S_MATR_MAT_I2S_SOUND_B;
      break;
      default:
         return DRX_STS_INVALID_ARG;
   }
   /* write the result */
   WR16( devAddr, AUD_DSP_WR_SRC_I2S_MATR__A, srcI2SMatr);

   /* FM Matrix */
   RR16( devAddr, AUD_DSP_WR_FM_MATRIX__A, &fmMatr );
   fmMatr &= (u16_t)~AUD_DSP_WR_FM_MATRIX__M;
   switch (mixer->matrixFm)
   {
      case DRX_AUD_FM_MATRIX_NO_MATRIX:
         fmMatr |= AUD_DSP_WR_FM_MATRIX_NO_MATRIX;
      break;
      case DRX_AUD_FM_MATRIX_GERMAN:
         fmMatr |= AUD_DSP_WR_FM_MATRIX_GERMAN_MATRIX;
      break;
      case DRX_AUD_FM_MATRIX_KOREAN:
         fmMatr |= AUD_DSP_WR_FM_MATRIX_KOREAN_MATRIX;
      break;
      case DRX_AUD_FM_MATRIX_SOUND_A:
         fmMatr |= AUD_DSP_WR_FM_MATRIX_SOUND_A;
      break;
      case DRX_AUD_FM_MATRIX_SOUND_B:
         fmMatr |= AUD_DSP_WR_FM_MATRIX_SOUND_B;
      break;
   default:
         return DRX_STS_INVALID_ARG;
   }

   /* Only write if ASS is off */
   if ( extAttr->audData.autoSound == DRX_AUD_AUTO_SOUND_OFF )
   {
      WR16( devAddr, AUD_DSP_WR_FM_MATRIX__A, fmMatr );
   }

   /* update the data structure with hardware state */
   extAttr->audData.mixer = *mixer;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Set AV Sync settings
* \param demod instance of demodulator
* \param pointer to DRXICfgAVSync_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlSetCfgAVSync  (  pDRXDemodInstance_t  demod,
                        pDRXCfgAudAVSync_t   avSync )
{
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t wAudVidSync = 0;

   if ( avSync == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   /* audio/video synchronisation */
   RR16( devAddr, AUD_DSP_WR_AV_SYNC__A, &wAudVidSync );

   wAudVidSync &= (u16_t)~AUD_DSP_WR_AV_SYNC_AV_ON__M;

   if ( *avSync == DRX_AUD_AVSYNC_OFF )
   {
      wAudVidSync |= AUD_DSP_WR_AV_SYNC_AV_ON_DISABLE;
   }
   else
   {
      wAudVidSync |= AUD_DSP_WR_AV_SYNC_AV_ON_ENABLE;
   }

   wAudVidSync &= (u16_t)~AUD_DSP_WR_AV_SYNC_AV_STD_SEL__M;

   switch ( *avSync )
   {
      case DRX_AUD_AVSYNC_NTSC:
         wAudVidSync |= AUD_DSP_WR_AV_SYNC_AV_STD_SEL_NTSC;
         break;
      case DRX_AUD_AVSYNC_MONOCHROME:
         wAudVidSync |= AUD_DSP_WR_AV_SYNC_AV_STD_SEL_MONOCHROME;
         break;
      case DRX_AUD_AVSYNC_PAL_SECAM:
         wAudVidSync |= AUD_DSP_WR_AV_SYNC_AV_STD_SEL_PAL_SECAM;
         break;
      case DRX_AUD_AVSYNC_OFF:
         /* OK */
         break;
      default:
         return DRX_STS_INVALID_ARG;
   }

      WR16( devAddr, AUD_DSP_WR_AV_SYNC__A, wAudVidSync );
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
   }

/*============================================================================*/
/**
* \brief Get AV Sync settings
* \param demod instance of demodulator
* \param pointer to DRXICfgAVSync_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCfgAVSync  (  pDRXDemodInstance_t  demod,
                        pDRXCfgAudAVSync_t   avSync )
   {
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t wAudVidSync = 0;

   if ( avSync == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   /* audio/video synchronisation */
   RR16( devAddr, AUD_DSP_WR_AV_SYNC__A, &wAudVidSync );

   if ( ( wAudVidSync & AUD_DSP_WR_AV_SYNC_AV_ON__M ) ==
      AUD_DSP_WR_AV_SYNC_AV_ON_DISABLE )
   {
      *avSync = DRX_AUD_AVSYNC_OFF;
      return DRX_STS_OK;
   }

   switch ( wAudVidSync & AUD_DSP_WR_AV_SYNC_AV_STD_SEL__M )
   {
      case AUD_DSP_WR_AV_SYNC_AV_STD_SEL_NTSC:
         *avSync = DRX_AUD_AVSYNC_NTSC;
         break;
      case AUD_DSP_WR_AV_SYNC_AV_STD_SEL_MONOCHROME:
         *avSync = DRX_AUD_AVSYNC_MONOCHROME;
         break;
      case AUD_DSP_WR_AV_SYNC_AV_STD_SEL_PAL_SECAM:
         *avSync =  DRX_AUD_AVSYNC_PAL_SECAM;
         break;
      default:
         return DRX_STS_ERROR;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get deviation mode
* \param demod instance of demodulator
* \param pointer to DRXCfgAudDeviation_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCfgDev     (  pDRXDemodInstance_t     demod,
                        pDRXCfgAudDeviation_t   dev )
      {
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t rModus   = 0;


   if ( dev == NULL )
   {
      return DRX_STS_INVALID_ARG;
      }

   extAttr = (pDRXKData_t)demod->myExtAttr;
   devAddr = demod->myI2CDevAddr;

   CHK_ERROR ( AUDGetModus ( demod, &rModus ));

   switch ( rModus & AUD_DEM_WR_MODUS_MOD_HDEV_A__M)
      {
      case AUD_DEM_WR_MODUS_MOD_HDEV_A_NORMAL:
         *dev = DRX_AUD_DEVIATION_NORMAL;
         break;
      case AUD_DEM_WR_MODUS_MOD_HDEV_A_HIGH_DEVIATION:
         *dev = DRX_AUD_DEVIATION_HIGH;
         break;
      default:
         return DRX_STS_ERROR;
      }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
   }

/*============================================================================*/
/**
* \brief Get deviation mode
* \param demod instance of demodulator
* \param pointer to DRXCfgAudDeviation_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlSetCfgDev     (  pDRXDemodInstance_t     demod,
                        pDRXCfgAudDeviation_t   dev )
   {
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t wModus   = 0;
   u16_t rModus   = 0;

   if ( dev == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   extAttr = (pDRXKData_t)demod->myExtAttr;
   devAddr = demod->myI2CDevAddr;

   CHK_ERROR ( AUDGetModus ( demod, &rModus ));

   wModus = rModus;

   wModus &= (u16_t)~AUD_DEM_WR_MODUS_MOD_HDEV_A__M;

   switch ( *dev )
   {
      case DRX_AUD_DEVIATION_NORMAL:
            wModus |= AUD_DEM_WR_MODUS_MOD_HDEV_A_NORMAL;
         break;
      case DRX_AUD_DEVIATION_HIGH:
         wModus |= AUD_DEM_WR_MODUS_MOD_HDEV_A_HIGH_DEVIATION;
         break;
      default:
         return DRX_STS_INVALID_ARG;
   }

   /* now update the modus register */
   if ( wModus != rModus)
   {
      WR16( devAddr, AUD_DEM_WR_MODUS__A, wModus );
   }
   /* store in drxk data struct */
   extAttr->audData.deviation = *dev;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get Prescaler settings
* \param demod instance of demodulator
* \param pointer to DRXCfgAudPrescale_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetCfgPrescale(  pDRXDemodInstance_t  demod,
                        pDRXCfgAudPrescale_t presc )
{
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t rMaxFMDeviation   = 0;
   u16_t rNicamPrescaler   = 0;

   if ( presc == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   /* read register data */
   RR16( devAddr, AUD_DSP_WR_NICAM_PRESC__A, &rNicamPrescaler );
   RR16( devAddr, AUD_DSP_WR_FM_PRESC__A,    &rMaxFMDeviation );

   /* calculate max FM deviation */
   rMaxFMDeviation >>= AUD_DSP_WR_FM_PRESC_FM_AM_PRESC__B;
   if ( rMaxFMDeviation > 0 )
      {
      presc->fmDeviation =  3600UL + (rMaxFMDeviation >> 1);
      presc->fmDeviation /= rMaxFMDeviation;
   }
   else
   {
      presc->fmDeviation = 380; /* kHz */
   }

   /* calculate NICAM gain from pre-scaler */
   /*
      nicamGain   = 20 * ( log10( preScaler / 16) )
                  = ( 100log10( preScaler ) - 100log10( 16 ) ) / 5

      because Log10Times100() cannot return negative numbers
                  = ( 100log10( 10 * preScaler ) - 100log10( 10 * 16) ) / 5


      for 0.1dB resolution:

      nicamGain   = 200 * ( log10( preScaler / 16) )
                  = 2 * ( 100log10( 10 * preScaler ) - 100log10( 10 * 16) )
                  = ( 100log10( 10 * preScaler^2 ) - 100log10( 10 * 16^2 ) )


   */
   rNicamPrescaler >>= 8;
   if ( rNicamPrescaler <= 1 )
   {
      presc->nicamGain = -241;
      }
      else
      {

      presc->nicamGain = (s16_t)( ( (s32_t)
                                    ( Log10Times100(  10 * rNicamPrescaler * 
                                                      rNicamPrescaler ) ) -
                                    (s32_t)
                                    ( Log10Times100( 10 * 16 * 16 ) ) ) );
      }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
   }



/*============================================================================*/
/**
* \brief Set Prescaler settings
* \param demod instance of demodulator
* \param pointer to DRXCfgAudPrescale_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlSetCfgPrescale(  pDRXDemodInstance_t  demod,
                        pDRXCfgAudPrescale_t presc )
   {
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t wMaxFMDeviation = 0;
   u16_t nicamPrescaler;

   if ( presc == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   /* setting of max FM deviation */
   wMaxFMDeviation = (u16_t)(Frac (3600UL, presc->fmDeviation, 0));
   wMaxFMDeviation <<= AUD_DSP_WR_FM_PRESC_FM_AM_PRESC__B;
   if ( wMaxFMDeviation >= AUD_DSP_WR_FM_PRESC_FM_AM_PRESC_28_KHZ_FM_DEVIATION )
   {
      wMaxFMDeviation = AUD_DSP_WR_FM_PRESC_FM_AM_PRESC_28_KHZ_FM_DEVIATION;
   }

   /* NICAM Prescaler */
   if( ( presc->nicamGain >= -241) && ( presc->nicamGain <= 180) )
   {
      /* calculation

         prescaler = 16 * 10^( GdB / 20 )

         minval of GdB = -20*log( 16 ) = -24.1dB

         negative numbers not allowed for dB2LinTimes100, so

         prescaler = 16 * 10^( GdB / 20 )
                   = 10^( (GdB / 20) + log10(16) )
                   = 10^( (GdB + 20log10(16)) / 20 )

                   in 0.1dB

                   = 10^( G0.1dB + 200log10(16)) / 200 )

      */
      nicamPrescaler = (u16_t)
                  ( ( dB2LinTimes100( presc->nicamGain + 241UL ) + 50UL ) / 100UL );

      /* clip result */
      if ( nicamPrescaler > 127 )
      {
         nicamPrescaler = 127;
      }

      /* shift before writing to register */
      nicamPrescaler <<= 8;
   }
   else
   {
      return(DRX_STS_INVALID_ARG);
   }
   /* end of setting NICAM Prescaler */

   WR16( devAddr, AUD_DSP_WR_NICAM_PRESC__A, nicamPrescaler );
   WR16( devAddr, AUD_DSP_WR_FM_PRESC__A, wMaxFMDeviation );

   extAttr->presets.prescale = *presc;

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/
/**
* \brief Beep
* \param demod instance of demodulator
* \param pointer to DRXAudBeep_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlBeep (  pDRXDemodInstance_t  demod,
               pDRXAudBeep_t        beep )
{
   pI2CDeviceAddr_t  devAddr = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr = (pDRXKData_t)NULL;

   u16_t theBeep    = 0;
   u16_t volume     = 0;
   u32_t frequency  = 0;


   if ( beep == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr   = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   if (( beep->volume > 0 ) || ( beep->volume < -127 ))
   {
      return DRX_STS_INVALID_ARG;
   }

   if ( beep->frequency > 3000 )
   {
      return DRX_STS_INVALID_ARG;
   }

   volume = (u16_t)beep->volume + 127;
   theBeep |= volume << AUD_DSP_WR_BEEPER_BEEP_VOLUME__B;


   frequency = ( (u32_t) beep->frequency ) * 23 / 500 ;
   if ( frequency > AUD_DSP_WR_BEEPER_BEEP_FREQUENCY__M )
   {
      frequency = AUD_DSP_WR_BEEPER_BEEP_FREQUENCY__M;
   }
   theBeep  |=  (u16_t) frequency;

   if ( beep->mute == TRUE )
   {
      theBeep = 0;
   }

   WR16( devAddr, AUD_DSP_WR_BEEPER__A, theBeep);

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Set an audio standard
* \param demod instance of demodulator
* \param pointer to DRXAudStandard_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlSetStandard ( pDRXDemodInstance_t demod,
                     pDRXAudStandard_t   standard )
{
   pI2CDeviceAddr_t  devAddr     = NULL;
   pDRXKData_t       extAttr     = NULL;
   DRXStandard_t     currentStandard = DRX_STANDARD_UNKNOWN;

   u16_t             wStandard   = 0;
   u16_t             wModus      = 0;
   u16_t             rModus      = 0;

   Bool_t            muteBuffer     = FALSE;
   s16_t             volumeBuffer   = 0;
   u16_t             wVolume        = 0;

   if ( standard == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr  = (pI2CDeviceAddr_t)demod->myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , FALSE ) );
      extAttr->audData.audioIsActive = TRUE;
   }


   /* reset RDS data availability flag */
   extAttr->audData.rdsDataPresent = FALSE;


   /* we need to mute from here to avoid noise during standard switching */
   muteBuffer   = extAttr->audData.volume.mute;
   volumeBuffer = extAttr->audData.volume.volume;

   extAttr->audData.volume.mute = TRUE;
   /* restore data structure from DRX ExtAttr, call volume first to mute */
   CHK_ERROR ( AUDCtrlSetCfgVolume
                  ( demod, &extAttr->audData.volume ) );
   CHK_ERROR ( AUDCtrlSetCfgCarrier
                  ( demod, &extAttr->audData.carriers ) );
   CHK_ERROR ( AUDCtrlSetCfgASSThres
                  ( demod, &extAttr->audData.assThresholds ) );
   CHK_ERROR ( AUDCtrSetlCfgAutoSound
                  ( demod, &extAttr->audData.autoSound ) );
   CHK_ERROR ( AUDCtrlSetCfgMixer
                  ( demod, &extAttr->audData.mixer ) );
   CHK_ERROR ( AUDCtrlSetCfgAVSync
                  ( demod, &extAttr->audData.avSync ) );
   CHK_ERROR ( AUDCtrlSetCfgOutputI2S
                  ( demod, &extAttr->audData.i2sdata ) );


   /* get prescaler from presets */
   CHK_ERROR ( AUDCtrlSetCfgPrescale
                  ( demod, &extAttr->presets.prescale ) );

   CHK_ERROR ( AUDGetModus ( demod, &rModus ));

   wModus = rModus;

   switch ( *standard )
   {
      case DRX_AUD_STANDARD_AUTO:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_AUTO;
            break;
      case DRX_AUD_STANDARD_BTSC:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_BTSC_STEREO;
         if (extAttr->audData.btscDetect == DRX_BTSC_MONO_AND_SAP)
         {
            wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_BTSC_SAP;
         }
            break;
      case DRX_AUD_STANDARD_A2:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_M_KOREA;
            break;
      case DRX_AUD_STANDARD_EIAJ:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_EIA_J;
            break;
      case DRX_AUD_STANDARD_FM_STEREO:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_FM_RADIO;
            break;
      case DRX_AUD_STANDARD_BG_FM:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_BG_FM;
            break;
      case DRX_AUD_STANDARD_D_K1:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_D_K1;
            break;
      case DRX_AUD_STANDARD_D_K2:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_D_K2;
            break;
      case DRX_AUD_STANDARD_D_K3:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_D_K3;
            break;
      case DRX_AUD_STANDARD_BG_NICAM_FM:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_BG_NICAM_FM;
            break;
      case DRX_AUD_STANDARD_L_NICAM_AM:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_L_NICAM_AM;
            break;
      case DRX_AUD_STANDARD_I_NICAM_FM:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_I_NICAM_FM;
            break;
      case DRX_AUD_STANDARD_D_K_NICAM_FM:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_D_K_NICAM_FM;
            break;
      case DRX_AUD_STANDARD_UNKNOWN:
         wStandard = AUD_DEM_WR_STANDARD_SEL_STD_SEL_AUTO;
            break;
         default:
         return DRX_STS_ERROR;
      }

   if ( *standard == DRX_AUD_STANDARD_AUTO )
   {
      /* we need the current standard here */
      currentStandard = extAttr->standard;


      wModus &= (u16_t)~AUD_DEM_WR_MODUS_MOD_6_5MHZ__M;

      if (  ( currentStandard == DRX_STANDARD_PAL_SECAM_L ) ||
            ( currentStandard == DRX_STANDARD_PAL_SECAM_LP ) )
   {
         wModus |= (AUD_DEM_WR_MODUS_MOD_6_5MHZ_SECAM);
   }
   else
   {
         wModus |= (AUD_DEM_WR_MODUS_MOD_6_5MHZ_D_K);
   }

      wModus &= (u16_t)~AUD_DEM_WR_MODUS_MOD_4_5MHZ__M;
      if ( currentStandard == DRX_STANDARD_NTSC )
   {
         wModus |= ( AUD_DEM_WR_MODUS_MOD_4_5MHZ_M_BTSC);

      }
      else /* non USA, ignore standard M to save time */
      {
         wModus |= ( AUD_DEM_WR_MODUS_MOD_4_5MHZ_CHROMA);
   }


   }

   wModus &= (u16_t)~AUD_DEM_WR_MODUS_MOD_FMRADIO__M;

   /* just get hardcoded deemphasis and activate here */
   if ( extAttr->audData.deemph == DRX_AUD_FM_DEEMPH_50US )
   {
      wModus |= ( AUD_DEM_WR_MODUS_MOD_FMRADIO_EU_50U);
   }
   else
   {
      wModus |= ( AUD_DEM_WR_MODUS_MOD_FMRADIO_US_75U);
   }

   wModus &= (u16_t)~AUD_DEM_WR_MODUS_MOD_BTSC__M;
   if( extAttr->audData.btscDetect == DRX_BTSC_STEREO )
   {
      wModus |= ( AUD_DEM_WR_MODUS_MOD_BTSC_BTSC_STEREO);
   }
   else /* DRX_BTSC_MONO_AND_SAP */
      {
      wModus |= ( AUD_DEM_WR_MODUS_MOD_BTSC_BTSC_SAP);
      }

   if ( wModus != rModus)
      {
      WR16( devAddr, AUD_DEM_WR_MODUS__A, wModus );
   }

   WR16( devAddr, AUD_DEM_WR_STANDARD_SEL__A, wStandard );

   /**************************************************************************/
   /* NOT calling AUDCtrlSetCfgVolume to avoid interfering standard          */
   /* detection, need to keep things very minimal here, but keep audio       */
   /* buffers intact                                                         */
   /**************************************************************************/
   extAttr->audData.volume.mute = muteBuffer;
   if ( extAttr->audData.volume.mute == FALSE )
   {
      wVolume |=  (u16_t) ( ( volumeBuffer + AUD_VOLUME_ZERO_DB ) <<
                              AUD_DSP_WR_VOLUME_VOL_MAIN__B );
      WR16( devAddr, AUD_DSP_WR_VOLUME__A, wVolume );
   }

   /* write standard selected */
   extAttr->audData.audioStandard = *standard;

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Get the current audio standard
* \param demod instance of demodulator
* \param pointer to DRXAudStandard_t
* \return DRXStatus_t.
*
*/
static DRXStatus_t
AUDCtrlGetStandard ( pDRXDemodInstance_t demod,
                     pDRXAudStandard_t   standard )
{
   pI2CDeviceAddr_t  devAddr           = NULL;
   pDRXKData_t       extAttr           = NULL;

   u16_t             rData             = 0;

   if ( standard == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   extAttr  = (pDRXKData_t)demod->myExtAttr;
   devAddr  = (pI2CDeviceAddr_t)demod->myI2CDevAddr;

   /* power up */
   if ( extAttr->audData.audioIsActive == FALSE )
   {
      CHK_ERROR ( PowerUpAud( demod , TRUE ) );
      extAttr->audData.audioIsActive = TRUE;
   }

   *standard = DRX_AUD_STANDARD_UNKNOWN;

   RR16( devAddr, AUD_DEM_RD_STANDARD_RES__A, &rData );

   /* return OK if the detection is not ready yet */
   if (  rData >=
         AUD_DEM_RD_STANDARD_RES_STD_RESULT_DETECTION_STILL_ACTIVE )
      {
      *standard = DRX_AUD_STANDARD_NOT_READY;
      return DRX_STS_OK;
      }

   /* detection done, return correct standard */
   switch ( rData )
      {
      /* no standard detected */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_NO_SOUND_STANDARD:
         *standard = DRX_AUD_STANDARD_UNKNOWN;
         break;
      /* standard is KOREA(A2) */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_NTSC_M_DUAL_CARRIER_FM:
         *standard = DRX_AUD_STANDARD_A2;
         break;
      /* standard is EIA-J (Japan) */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_NTSC_EIA_J:
         *standard = DRX_AUD_STANDARD_EIAJ;
         break;
      /* standard is BTSC-stereo */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_BTSC_STEREO:
         *standard = DRX_AUD_STANDARD_BTSC;
         break;
      /* standard is BTSC-mono (SAP) */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_BTSC_MONO_SAP:
         *standard = DRX_AUD_STANDARD_BTSC;
         break;
      /* standard is FM radio */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_FM_RADIO:
         *standard = DRX_AUD_STANDARD_FM_STEREO;
         break;
      /* standard is BG FM */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_B_G_DUAL_CARRIER_FM:
         *standard = DRX_AUD_STANDARD_BG_FM;
         break;
      /* standard is DK-1 FM */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_D_K1_DUAL_CARRIER_FM:
         *standard = DRX_AUD_STANDARD_D_K1;
         break;
      /* standard is DK-2 FM */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_D_K2_DUAL_CARRIER_FM:
         *standard = DRX_AUD_STANDARD_D_K2;
         break;
      /* standard is DK-3 FM */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_D_K3_DUAL_CARRIER_FM:
         *standard = DRX_AUD_STANDARD_D_K3;
         break;
      /* standard is BG-NICAM FM */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_B_G_NICAM_FM:
         *standard = DRX_AUD_STANDARD_BG_NICAM_FM;
         break;
      /* standard is L-NICAM AM */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_L_NICAM_AM:
         *standard = DRX_AUD_STANDARD_L_NICAM_AM;
         break;
      /* standard is I-NICAM FM */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_I_NICAM_FM:
         *standard = DRX_AUD_STANDARD_I_NICAM_FM;
         break;
      /* standard is DK-NICAM FM */
      case AUD_DEM_RD_STANDARD_RES_STD_RESULT_D_K_NICAM_FM:
         *standard = DRX_AUD_STANDARD_D_K_NICAM_FM;
         break;
      default:
         *standard = DRX_AUD_STANDARD_UNKNOWN;
   }

   return DRX_STS_OK;
  rw_error:
   return DRX_STS_ERROR;

}


/*============================================================================*/
/**
* \brief Retreive lock statusin case of FM standard
* \param demod instance of demodulator
* \param pointer to lock status
* \return DRXStatus_t.
*
*/
static DRXStatus_t
FmLockStatus( pDRXDemodInstance_t demod,
              pDRXLockStatus_t    lockStat )
{
   DRXAudStatus_t  status;

   /* Check detection of audio carriers */
   CHK_ERROR( AUDCtrlGetCarrierDetectStatus (  demod, &status ) );

   /* locked if either primary or secondary carrier is detected */
   if (  ( status.carrierA == TRUE ) ||
         ( status.carrierB == TRUE ) )
   {
      *lockStat = DRX_LOCKED;
   } else {
      *lockStat = DRX_NOT_LOCKED;
   }

   return DRX_STS_OK;

   rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief retreive signal quality in case of FM standard
* \param demod instance of demodulator
* \param pointer to signal quality
* \return DRXStatus_t.
*
* Only the quality indicator field is will be supplied.
* This will either be 0% or 100%, nothing in between.
*
*/
static DRXStatus_t
FmSigQuality( pDRXDemodInstance_t demod,
              pDRXSigQuality_t    sigQuality )
{
   DRXLockStatus_t    lockStatus = DRX_NOT_LOCKED;

   CHK_ERROR( FmLockStatus(  demod, &lockStatus ) );
   if ( lockStatus == DRX_LOCKED )
   {
      sigQuality->indicator = 100;
   } else {
      sigQuality->indicator = 0;
   }

   return DRX_STS_OK;

   rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/*==                 END OF AUDIO FUNCTIONS                                 ==*/
/*============================================================================*/
#endif /* DRXK_EXCLUDE_AUDIO */


/*============================================================================*/
/*==                       ATV RELATED FUNCTIONS                            ==*/
/*============================================================================*/

/* -------------------------------------------------------------------------- */

/**
* \brief Flush changes in ATV shadow registers to physical registers.
* \param demod instance of demodulator
* \param forceUpdate don't look at standard or change flags, flush all.
* \return DRXStatus_t.
*
* called by: ATVSetStandard
*            ATVCtrlSetCfgOutput
*            ATVCtrlSetCfgMisc
*            CtrlSetCfgAtvEquCoef
*/
static DRXStatus_t
ATVUpdateConfig( pDRXDemodInstance_t demod,
                 Bool_t              forceUpdate )
{
   pI2CDeviceAddr_t devAddr  = NULL;
   pDRXKData_t      extAttr  = NULL;

   devAddr  = demod -> myI2CDevAddr;
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   if ( forceUpdate ||
        ( extAttr->standard == DRX_STANDARD_NTSC ) ||
        ( extAttr->standard == DRX_STANDARD_PAL_SECAM_BG ) ||
        ( extAttr->standard == DRX_STANDARD_PAL_SECAM_DK ) ||
        ( extAttr->standard == DRX_STANDARD_PAL_SECAM_I  ) ||
        ( extAttr->standard == DRX_STANDARD_PAL_SECAM_L  ) ||
        ( extAttr->standard == DRX_STANDARD_PAL_SECAM_LP ) ||
        ( extAttr->standard == DRX_STANDARD_FM   )    )
   {
      /* equalizer coefficients */
      if ( forceUpdate ||
           ((extAttr->atvCfgChangedFlags & DRXK_ATV_CHANGED_COEF) != 0) )
      {
         WR16( devAddr, ATV_TOP_EQU0__A, extAttr->presets.atvTopEqu[0] );
         WR16( devAddr, ATV_TOP_EQU1__A, extAttr->presets.atvTopEqu[1] );
         WR16( devAddr, ATV_TOP_EQU2__A, extAttr->presets.atvTopEqu[2] );
         WR16( devAddr, ATV_TOP_EQU3__A, extAttr->presets.atvTopEqu[3] );
      }

      /* bypass fast carrier recovery */
      if ( forceUpdate ||
           ((extAttr->atvCfgChangedFlags & DRXK_ATV_CHANGED_PC_BYPASS) != 0) )
      {
         u16_t data=0;

         RR16( devAddr, IQM_RT_ROT_BP__A, &data );
         data &= (~((u16_t)IQM_RT_ROT_BP_ROT_OFF__M));
         if (extAttr->phaseCorrectionBypass)
         {
            data |= IQM_RT_ROT_BP_ROT_OFF_OFF;
         } else {
            data &= ~IQM_RT_ROT_BP_ROT_OFF_OFF;
         }
         WR16( devAddr, IQM_RT_ROT_BP__A, data );
      }

      /* peak filter setting */
      if ( forceUpdate ||
           ((extAttr->atvCfgChangedFlags & DRXK_ATV_CHANGED_PEAK_FLT) != 0) )
      {
         WR16( devAddr, ATV_TOP_VID_PEAK__A, extAttr->presets.peakFilter );
      }

      /* noise filter setting */
      if ( forceUpdate ||
           ((extAttr->atvCfgChangedFlags & DRXK_ATV_CHANGED_NOISE_FLT) != 0) )
      {
         WR16( devAddr, ATV_TOP_NOISE_TH__A, extAttr->presets.atvTopNoiseTh );
      }

      /* SIF attenuation */
      if ( forceUpdate ||
           ((extAttr->atvCfgChangedFlags & DRXK_ATV_CHANGED_SIF_ATT) != 0) )
      {
         u16_t attenuation=0;

         switch( extAttr->sifAttenuation ){
            case DRXK_SIF_ATTENUATION_0DB:
               attenuation = ATV_TOP_AF_SIF_ATT_0DB;
               break;
            case DRXK_SIF_ATTENUATION_3DB:
               attenuation = ATV_TOP_AF_SIF_ATT_M3DB;
               break;
            case DRXK_SIF_ATTENUATION_6DB:
               attenuation = ATV_TOP_AF_SIF_ATT_M6DB;
               break;
            case DRXK_SIF_ATTENUATION_9DB:
               attenuation = ATV_TOP_AF_SIF_ATT_M9DB;
               break;
            default:
               return DRX_STS_ERROR;
               break;
         }
         WR16( devAddr, ATV_TOP_AF_SIF_ATT__A, attenuation );
      }

      /* SIF & CVBS enable */
      if ( forceUpdate ||
           ((extAttr->atvCfgChangedFlags & DRXK_ATV_CHANGED_STDBY) != 0) )
      {
         u16_t data = 0;

         RR16( devAddr, ATV_TOP_STDBY__A, &data );

         if ( extAttr->enableCVBSOutput )
         {
            data &= ~ATV_TOP_STDBY_CVBS_STDBY_STANDBY;
         } else {
            data |= ATV_TOP_STDBY_CVBS_STDBY_STANDBY;
         }


         if ( extAttr->enableSIFOutput )
         {
            data &= (~ATV_TOP_STDBY_SIF_STDBY_STANDBY);
         } else {
            data |= ATV_TOP_STDBY_SIF_STDBY_STANDBY;
         }
         WR16( devAddr, ATV_TOP_STDBY__A, data );
      }

      extAttr->atvCfgChangedFlags=0;
   } /* if */

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Set ATV equalizer coefficients
* \param demod instance of demodulator
* \param coef  the equalizer coefficients
* \return DRXStatus_t.
*
* called by: CtrlSetCfg
*/
static DRXStatus_t
ATVCtrlSetCfgEquCoef( pDRXDemodInstance_t demod ,
                      pDRXKCfgAtvEquCoef_t coef)
{
   pDRXKData_t      extAttr  = NULL;

   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* current standard needs to be an ATV standard */
   if (!DRXK_ISATVSTD(extAttr->standard ))
   {
      return DRX_STS_ERROR;
   }

   /* Check arguments */
   if ( ( coef == NULL )                               ||
        ( coef->coef0 > (ATV_TOP_EQU0_EQU_C0__M / 2) ) ||
        ( coef->coef1 > (ATV_TOP_EQU1_EQU_C1__M / 2) ) ||
        ( coef->coef2 > (ATV_TOP_EQU2_EQU_C2__M / 2) ) ||
        ( coef->coef3 > (ATV_TOP_EQU3_EQU_C3__M / 2) ) ||
        ( coef->coef0 < ((s16_t)~(ATV_TOP_EQU0_EQU_C0__M >> 1)) ) ||
        ( coef->coef1 < ((s16_t)~(ATV_TOP_EQU1_EQU_C1__M >> 1)) ) ||
        ( coef->coef2 < ((s16_t)~(ATV_TOP_EQU2_EQU_C2__M >> 1)) ) ||
        ( coef->coef3 < ((s16_t)~(ATV_TOP_EQU3_EQU_C3__M >> 1)) ) )
   {
      return DRX_STS_INVALID_ARG;
   }

   /* change presets (until overridden) */
   extAttr->presets.atvTopEqu[0] = coef->coef0;
   extAttr->presets.atvTopEqu[1] = coef->coef1;
   extAttr->presets.atvTopEqu[2] = coef->coef2;
   extAttr->presets.atvTopEqu[3] = coef->coef3;
   extAttr->atvCfgChangedFlags |= DRXK_ATV_CHANGED_COEF;

   CHK_ERROR( ATVUpdateConfig( demod, FALSE ) );

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Get ATV equ coef settings
* \param demod instance of demodulator
* \param coef The ATV equ coefficients
* \return DRXStatus_t.
*
* The values are read from the shadow registers maintained by the drxdriver
* If registers are manipulated outside of the drxdriver scope the reported
* settings will not reflect these changes because of the use of shadow
* regitsers.
*
* called by: CtrlGetCfg
*/
static DRXStatus_t
ATVCtrlGetCfgEquCoef( pDRXDemodInstance_t demod ,
                      pDRXKCfgAtvEquCoef_t coef)
{
   pDRXKData_t      extAttr  = NULL;

   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* current standard needs to be an ATV standard */
   if (!DRXK_ISATVSTD(extAttr->standard ))
   {
      return DRX_STS_ERROR;
   }

   /* Check arguments */
   if ( coef == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   coef->coef0 = extAttr->presets.atvTopEqu[0];
   coef->coef1 = extAttr->presets.atvTopEqu[1];
   coef->coef2 = extAttr->presets.atvTopEqu[2];
   coef->coef3 = extAttr->presets.atvTopEqu[3];

   return DRX_STS_OK;
}

/* -------------------------------------------------------------------------- */

/**
* \brief Set misc. settings for ATV.
* \param demod instance of demodulator
* \param
* \return DRXStatus_t.
*
* called by: CtrlSetCfg
*/
static DRXStatus_t
ATVCtrlSetCfgMisc( pDRXDemodInstance_t    demod ,
                   pDRXKCfgAtvMisc_t      settings )
{
   pDRXKData_t      extAttr  = NULL;

   /* Check arguments */
   if ( ( settings == NULL )                    ||
        ((settings->peakFilter) < (s16_t)(-8) ) ||
        ((settings->peakFilter) > (s16_t)(15) ) ||
        ((settings->noiseFilter) > 15 )       )
   {
      return DRX_STS_INVALID_ARG;
   } /* if */

   switch( settings->imbalanceCtrlMode ){
      case DRXK_ATV_IMBALANCE_OFF   : /* fallthrough */
      case DRXK_ATV_IMBALANCE_MODE_A: /* fallthrough */
      case DRXK_ATV_IMBALANCE_MODE_B: /* fallthrough */
      case DRXK_ATV_IMBALANCE_FREEZE:
         /*Do nothing */
         break;
      default:
         return DRX_STS_INVALID_ARG;
         break;
   } /* switch */

   extAttr  = (pDRXKData_t)demod->myExtAttr;

   if ( settings->imbalanceCtrlMode != extAttr->atvImbCtrlMode )
   {
      extAttr->atvImbCtrlMode = settings->imbalanceCtrlMode;
      extAttr->atvCfgChangedFlags |= DRXK_ATV_CHANGED_IMB_CTRL;
   }

   if ( settings->phaseCorrectionBypass != extAttr->phaseCorrectionBypass )
   {
      extAttr->phaseCorrectionBypass = settings->phaseCorrectionBypass;
      extAttr->atvCfgChangedFlags |= DRXK_ATV_CHANGED_PC_BYPASS;
   }

   if ( settings->peakFilter != extAttr->presets.peakFilter )
   {
      extAttr->presets.peakFilter = settings->peakFilter;
      extAttr->atvCfgChangedFlags |= DRXK_ATV_CHANGED_PEAK_FLT;
   }

   if ( settings->noiseFilter != extAttr->presets.atvTopNoiseTh )
   {
      extAttr->presets.atvTopNoiseTh = settings->noiseFilter;
      extAttr->atvCfgChangedFlags |= DRXK_ATV_CHANGED_NOISE_FLT;
   }

   CHK_ERROR( ATVUpdateConfig( demod, FALSE ) );

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Get misc settings of ATV.
* \param demod instance of demodulator
* \param settings misc. ATV settings
* \return DRXStatus_t.
*
* The values are read from the shadow registers maintained by the drxdriver
* If registers are manipulated outside of the drxdriver scope the reported
* settings will not reflect these changes because of the use of shadow
* regitsers.
*
* called by: CtrlGetCfg
*/
static DRXStatus_t
ATVCtrlGetCfgMisc( pDRXDemodInstance_t    demod ,
                   pDRXKCfgAtvMisc_t      settings )
{
   pDRXKData_t      extAttr  = NULL;

   /* Check arguments */
   if ( settings == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   extAttr  = (pDRXKData_t)demod->myExtAttr;

   settings->imbalanceCtrlMode     = extAttr->atvImbCtrlMode       ;
   settings->phaseCorrectionBypass = extAttr->phaseCorrectionBypass;
   settings->peakFilter            = extAttr->presets.peakFilter   ;
   settings->noiseFilter           = extAttr->presets.atvTopNoiseTh;

   return DRX_STS_OK;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Configure ATV ouputs
* \param demod instance of demodulator
* \param outputCfg output configuaration
* \return DRXStatus_t.
*
* called by: CtrlSetCfg
*/
static DRXStatus_t
ATVCtrlSetCfgOutput( pDRXDemodInstance_t demod ,
                     pDRXKCfgAtvOutput_t outputCfg )
{
   pDRXKData_t      extAttr  = NULL;

   /* copy settings to shadow data */
   extAttr  = (pDRXKData_t)demod->myExtAttr;

   /* Check arguments */
   if ( outputCfg == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   if ( outputCfg->enableSIFOutput )
   {
   switch( outputCfg->sifAttenuation ){
      case DRXK_SIF_ATTENUATION_0DB: /* fallthrough */
      case DRXK_SIF_ATTENUATION_3DB: /* fallthrough */
      case DRXK_SIF_ATTENUATION_6DB: /* fallthrough */
      case DRXK_SIF_ATTENUATION_9DB:
         /* Do nothing */
         break;
      default:
         return DRX_STS_INVALID_ARG;
         break;
   }

   if(extAttr->sifAttenuation != outputCfg->sifAttenuation )
   {
      extAttr->sifAttenuation = outputCfg->sifAttenuation;
      extAttr->atvCfgChangedFlags |= DRXK_ATV_CHANGED_SIF_ATT;
   }
   }

   if ( extAttr->enableCVBSOutput != outputCfg->enableCVBSOutput )
   {
      extAttr->enableCVBSOutput = outputCfg->enableCVBSOutput;
      extAttr->atvCfgChangedFlags |= DRXK_ATV_CHANGED_STDBY;
   }

   if ( extAttr->enableSIFOutput != outputCfg->enableSIFOutput )
   {
      extAttr->enableSIFOutput = outputCfg->enableSIFOutput;
      extAttr->atvCfgChangedFlags |= DRXK_ATV_CHANGED_STDBY;
   }

   CHK_ERROR( ATVUpdateConfig(demod, FALSE) );

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief
* \param demod instance of demodulator
* \param outputCfg output configuaration
* \return DRXStatus_t.
*
* called by: CtrlGetCfg
*/
static DRXStatus_t
ATVCtrlGetCfgOutput( pDRXDemodInstance_t demod ,
                     pDRXKCfgAtvOutput_t outputCfg )
{
   pDRXKData_t      extAttr  = NULL;
   pI2CDeviceAddr_t devAddr  = NULL;
   u16_t            data     = 0;

   /* Check arguments */
   if ( outputCfg == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }
   extAttr = (pDRXKData_t)demod->myExtAttr;
   devAddr = demod -> myI2CDevAddr;

   /* Return the status of HW*/
   RR16( devAddr, ATV_TOP_STDBY__A, &data );
   data &= ATV_TOP_STDBY_CVBS_STDBY__M;

   if ( data == ATV_TOP_STDBY_CVBS_STDBY_STANDBY )
   {
      outputCfg->enableCVBSOutput = FALSE;
   }
   else
   {
      outputCfg->enableCVBSOutput = TRUE;
   }

   RR16( devAddr, ATV_TOP_STDBY__A, &data );
   data &= ATV_TOP_STDBY_SIF_STDBY__M;

   if ( data == ATV_TOP_STDBY_SIF_STDBY_STANDBY )
   {
      outputCfg->enableSIFOutput = FALSE;
   }
   else
   {
      outputCfg->enableSIFOutput = TRUE;
      RR16(demod->myI2CDevAddr, ATV_TOP_AF_SIF_ATT__A, &data);
      outputCfg->sifAttenuation = (DRXKSIFAttenuation_t) data;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief
* \param demod instance of demodulator
* \param agcStatus agc status
* \return DRXStatus_t.
*
* called by: CtrlGetCfg
*/
static DRXStatus_t
ATVCtrlGetCfgAgcStatus( pDRXDemodInstance_t    demod ,
                        pDRXKCfgAtvAgcStatus_t agcStatus )
{
   pI2CDeviceAddr_t devAddr  = NULL;
   pDRXKData_t      extAttr  = NULL;
   u16_t            data     = 0;
   u32_t            tmp      = 0;

   /* Check arguments */
   if ( agcStatus == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /*
      RFgain = (IQM_AF_AGC_RF__A * 26.75)/1000 (uA)
             = ((IQM_AF_AGC_RF__A * 27) - (0.25*IQM_AF_AGC_RF__A))/1000

             IQM_AF_AGC_RF__A * 27 is 20 bits worst case.
   */
   RR16( devAddr, IQM_AF_AGC_RF__A, &data );
   tmp = ((u32_t)data) * 27 - ((u32_t)(data>>2)); /* nA */
   agcStatus->rfAgcGain = (u16_t)(tmp/1000) ; /* uA */
   /* rounding */
   if ( tmp%1000 >= 500 )
   {
      (agcStatus->rfAgcGain)++;
   }

   /*
      IFgain = (IQM_AF_AGC_IF__A * 26.75)/1000 (uA)
             = ((IQM_AF_AGC_IF__A * 27) - (0.25*IQM_AF_AGC_IF__A))/1000

             IQM_AF_AGC_IF__A * 27 is 20 bits worst case.
   */
   RR16( devAddr, IQM_AF_AGC_IF__A, &data );
   tmp = ((u32_t)data) * 27 - ((u32_t)(data>>2)); /* nA */
   agcStatus->ifAgcGain = (u16_t)(tmp/1000) ; /* uA */
   /* rounding */
   if ( tmp%1000 >= 500 )
   {
      (agcStatus->ifAgcGain)++;
   }

   /*
      videoGain = (ATV_TOP_SFR_VID_GAIN__A/16 -150)* 0.05 (dB)
                = (ATV_TOP_SFR_VID_GAIN__A/16 -150)/20 (dB)
                = 10*(ATV_TOP_SFR_VID_GAIN__A/16 -150)/20 (in 0.1 dB)
                = (ATV_TOP_SFR_VID_GAIN__A/16 -150)/2 (in 0.1 dB)
                = (ATV_TOP_SFR_VID_GAIN__A/32) - 75 (in 0.1 dB)
   */

   SARR16( devAddr, SCU_RAM_ATV_VID_GAIN_HI__A , &data );
   /* dividing by 32 inclusive rounding */
   data >>=4;
   if ((data & 1) !=0 )
   {
      data++;
   }
   data >>= 1;
   agcStatus->videoAgcGain = ((s16_t)data)-75; /* 0.1 dB */

   /*
      audioGain = (SCU_RAM_ATV_SIF_GAIN__A -8)* 0.05 (dB)
                = (SCU_RAM_ATV_SIF_GAIN__A -8)/20 (dB)
                = 10*(SCU_RAM_ATV_SIF_GAIN__A -8)/20 (in 0.1 dB)
                = (SCU_RAM_ATV_SIF_GAIN__A -8)/2 (in 0.1 dB)
                = (SCU_RAM_ATV_SIF_GAIN__A/2) - 4 (in 0.1 dB)
   */

   SARR16( devAddr, SCU_RAM_ATV_SIF_GAIN__A, &data );
   data &= SCU_RAM_ATV_SIF_GAIN__M;
   /* dividing by 2 inclusive rounding */
   if ((data & 1) !=0 )
   {
      data++;
   }
   data >>= 1;
   agcStatus->audioAgcGain = ((s16_t)data)-4; /* 0.1 dB */

   /* Loop gain's */
   SARR16( devAddr, SCU_RAM_AGC_KI__A, &data );
   agcStatus->videoAgcLoopGain =
   ( (data & SCU_RAM_AGC_KI_DGAIN__M)>>SCU_RAM_AGC_KI_DGAIN__B) ;
   agcStatus->rfAgcLoopGain =
   ( (data & SCU_RAM_AGC_KI_RF__M)>>SCU_RAM_AGC_KI_RF__B) ;
   agcStatus->ifAgcLoopGain =
   ( (data & SCU_RAM_AGC_KI_IF__M)>>SCU_RAM_AGC_KI_IF__B) ;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Start up ATV.
* \param demod instance of demodulator
* \return DRXStatus_t.
*
* Starts ATV and IQM
*
*/
static DRXStatus_t
ATVStart( pDRXDemodInstance_t   demod )
{
   pI2CDeviceAddr_t devAddr  = NULL;
   pDRXKData_t extAttr = NULL;


   devAddr  = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* ATV */
   WR16( devAddr, ATV_COMM_EXEC__A, ATV_COMM_EXEC_ACTIVE );

   /* turn on IQM_AF */
   CHK_ERROR( IQMSetAf( demod, TRUE ) );

   WR16(devAddr, IQM_AF_START_LOCK__A, 0x01 );
   WR16( devAddr, IQM_COMM_EXEC__A, IQM_COMM_EXEC_B_ACTIVE );

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Power-up ATV (power down OFDM)
* \param demod instance of demodulator.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
ATVPowerUp (pDRXDemodInstance_t   demod )
{
   pI2CDeviceAddr_t  devAddr     = NULL;
   DRXPowerMode_t    powerMode   = DRXK_POWER_DOWN_OFDM;

   devAddr = demod->myI2CDevAddr;

   CHK_ERROR( CtrlPowerMode( demod, &powerMode) );

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/**
* \brief Power down ATV.
* \param demod instance of demodulator
* \param standard relateed to current ATV mode
* \return DRXStatus_t.
*
*  Stops and thus resets ATV and IQM block
*  SIF and CVBS ADC are powered down
*  Calls audio power down
*
*/
static DRXStatus_t
ATVPowerDown( pDRXDemodInstance_t   demod)
{
   pI2CDeviceAddr_t devAddr  = NULL;
   DRXKSCUCmd_t     cmdSCU   = { /* command      */ 0,
                                 /* parameterLen */ 0,
                                 /* resultLen    */ 0,
                                 /* *parameter   */ NULL,
                                 /* *result      */ NULL };
   u16_t cmdResult           = 0;
   u16_t data                = 0;
   pDRXKData_t extAttr       = NULL;

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t) demod->myExtAttr;

   RR16( devAddr, SCU_COMM_EXEC__A, &data);
   if ( data == SCU_COMM_EXEC_ACTIVE )
   {
      /* Stop ATV SCU (will reset ATV and IQM hardware */
      cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_ATV |
                            SCU_RAM_COMMAND_CMD_DEMOD_STOP;
      cmdSCU.parameterLen = 0;
      cmdSCU.resultLen    = 1;
      cmdSCU.parameter    = NULL;
      cmdSCU.result       = &cmdResult;
      CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );
   }

   /* Disable ATV outputs (ATV reset enables CVBS, undo this) */
   WR16( devAddr, ATV_TOP_STDBY__A, ( ATV_TOP_STDBY_SIF_STDBY_STANDBY |
                                      ATV_TOP_STDBY_CVBS_STDBY_STANDBY ));
   WR16( devAddr, ATV_COMM_EXEC__A, ATV_COMM_EXEC_STOP);
   WR16(devAddr, IQM_COMM_EXEC__A, IQM_COMM_EXEC_B_STOP );
   CHK_ERROR( IQMSetAf( demod, FALSE ) );

#ifndef DRXK_EXCLUDE_AUDIO
   /* Audio */
   CHK_ERROR( PowerDownAud(demod));
   extAttr->audData.audioIsActive = FALSE;
#endif

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/* -------------------------------------------------------------------------- */

/**
* \brief Enable/Disable ATV IIR workaroung.
* \param demod instance of demodulator
* \param standard for which to set the workaround
* \return DRXStatus_t.
*
* Init the standard dependent registers.
* Assuming that the current standard is available
*
* called by: ATVSetStandard
*/
/* -------------------------------------------------------------------------- */
static DRXStatus_t
ATVSetIIRWorkAround( pDRXDemodInstance_t demod, pDRXStandard_t standard)
{
   pI2CDeviceAddr_t   devAddr  = NULL;
   pDRXKData_t        extAttr  = NULL;
   u16_t scuRamAtvIirCrit      = 0;

   devAddr    = demod -> myI2CDevAddr;
   extAttr    = (pDRXKData_t)demod->myExtAttr;

   /* Enable the workaround for A2 patch */
   /* Disable the workaround for A3+ */
   if ( DRXK_A1_PATCH_CODE( demod ) ||
        DRXK_A2_PATCH_CODE( demod ))
   {
      switch(*standard)
      {
         case DRX_STANDARD_NTSC:
            scuRamAtvIirCrit = 225;
            break;
         case DRX_STANDARD_PAL_SECAM_BG:
            scuRamAtvIirCrit = 275;
            break;
         case DRX_STANDARD_PAL_SECAM_DK:
         case DRX_STANDARD_PAL_SECAM_L:
         case DRX_STANDARD_PAL_SECAM_LP:
             scuRamAtvIirCrit = 325;
            break;
         case DRX_STANDARD_PAL_SECAM_I:
            scuRamAtvIirCrit = 300;
            break;
         default  :
            scuRamAtvIirCrit = 0;
      }
      WR16( devAddr, SCU_RAM_ATV_IIR_CRIT__A , scuRamAtvIirCrit);
      WR16( devAddr, SCU_RAM_ATV_ENABLE_IIR_WA__A, 1 );
   }
   else if ( !DRXK_A1_PATCH_CODE (demod) &&
               !DRXK_A2_PATCH_CODE (demod ) )
   {
      /* Disable IIR workaround for A3+ */
      WR16( devAddr, SCU_RAM_ATV_ENABLE_IIR_WA__A, 0 );
   }
   else
   {
      /* Do nothing */
   }
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/**
* \brief Configure ATV mode (lock-in or locked).
* \param demod     instance of demodulator
* \param atvMode   ATV mode (scan or lock)
* \return DRXStatus_t.
*/
static DRXStatus_t
ATVCtrlSetCfgAtvMode(   pDRXDemodInstance_t demod,
                        DRXKAtvMode_t       atvMode )
{
   pI2CDeviceAddr_t   devAddr  = NULL;
   pDRXKData_t        extAttr  = NULL;
   u16_t              atvCrAmpThres = 0;
   u16_t              atvCrOvmThres = 0;

   devAddr    = demod -> myI2CDevAddr;
   extAttr    = (pDRXKData_t)demod->myExtAttr;

   if ( atvMode == DRXK_ATV_MODE_LOCK )
   {
      atvCrAmpThres = extAttr->presets.atvCrAmpThres;
      atvCrOvmThres = extAttr->presets.atvCrOvmThres;
   }
   else
   {
      atvCrAmpThres = 0;
      atvCrOvmThres = extAttr->presets.atvCrOvmThres;
   }

   WR16( devAddr, ATV_TOP_CR_AMP_TH__A, atvCrAmpThres );
   WR16( devAddr, ATV_TOP_CR_OVM_TH__A, atvCrOvmThres );

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/
/**
* \brief    Detects presence of audio carrier by means of checking the ratio of
*           digital audio and video amplification inside the DRXK
* \param    demod          pointer to demod instance
* \param    svrThreshold   threshold of S/V ratio in 0.1dB
* \param    audioIsPresent pointer to boolean result
*                          TRUE meaning audio is present
                           FALSE meaning no audio is present
* \return DRXStatus_t
* \retval DRX_STS_OK Succes
* \retval DRX_STS_ERROR Timeout, I2C error, illegal bank
**/
DRXStatus_t
AGCDetectAudioPower(    pDRXDemodInstance_t demod,
                        s16_t               svrThreshold,
                        pBool_t             audioIsPresent )
{
   DRXKCfgAtvAgcStatus_t atvAGCs = {0};

   if ( audioIsPresent == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   *audioIsPresent = TRUE;

   CHK_ERROR( ATVCtrlGetCfgAgcStatus(demod, &atvAGCs));

   /* s16_t videoAgcGain    ;  -75 .. 1972 in 0.1 dB steps */
   /* s16_t audioAgcGain    ;   -4 .. 1020 in 0.1 dB steps */

   if( (atvAGCs.audioAgcGain - atvAGCs.videoAgcGain) > svrThreshold )
   {
      /* High ratio SAGC/VAGC; probably no audio carrier power */
      *audioIsPresent = FALSE;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/
/**
* \brief Autodetect ATV standard using the audio power information and the
*        audio block (if available)
* \param standard    pointer to a DRXStandard_t to stote detected standard;
*                    - DRX_STANDARD_UNKNOWN is returned in case no audio
*                      carriers were present
*                    - In case no audio block is available, the active ATV
*                      standard is returned in case audio power is present
* \return DRXStatus_t
* \retval DRX_STS_OK Succes
* \retval DRX_STS_ERROR Timeout, I2C error, illegal bank
**/
DRXStatus_t ATVCtrlGetCfgStandard ( pDRXDemodInstance_t  demod,
                                    pDRXStandard_t       standard )
{
   pDRXKData_t       extAttr           = (pDRXKData_t) NULL;
   DRXLockStatus_t   lockStatus        = DRX_NOT_LOCKED;
   DRXChannel_t      channel;
   const u16_t       timeOut           = 300; /* ms */
   Bool_t            audioPowerPresent = FALSE;

#ifndef DRXK_EXCLUDE_AUDIO
   u32_t             startTime         = 0;
   DRXAudStandard_t  audioStandard     = DRX_AUD_STANDARD_AUTO;
#endif

   if (  ( standard == NULL ) ||
         ( demod == NULL ) )
   {
      return DRX_STS_INVALID_ARG;
   }
   extAttr  = (pDRXKData_t) demod->myExtAttr;

   /* intially set standard to unknown */
   *standard = DRX_STANDARD_UNKNOWN;


   /*-------------------------------------------------------*/
   /*                                                       */
   /* ensure proper channel alignment for audio detection   */
   /*                                                       */
   /*-------------------------------------------------------*/
   CHK_ERROR( WaitForLock( demod, &lockStatus, timeOut) );
   if ( lockStatus != DRX_LOCKED )
   {
      /* Not locked, nothing there */
      return DRX_STS_OK;
   }
   CHK_ERROR( CtrlGetChannel ( demod, &channel ) );
   CHK_ERROR( CtrlSetChannel ( demod, &channel ) );
   CHK_ERROR( WaitForLock( demod, &lockStatus, timeOut) );
   if ( lockStatus != DRX_LOCKED)
   {
      /* Not locked, nothing there */
      return DRX_STS_OK;
   }
   /* some settling time before sound detection */
   DRXBSP_HST_Sleep ( 50 );


   /*-------------------------------------------------------*/
   /*                                                       */
   /*    quick check for sound carrier power                */
   /*                                                       */
   /*-------------------------------------------------------*/
   CHK_ERROR( AGCDetectAudioPower ( demod,
                                    extAttr->atvSVRThreshold,
                                    &audioPowerPresent ) );
   if ( audioPowerPresent == FALSE )
   {
      /* No audio power; return DRX_STANDARD_UNKNOWN */
      return DRX_STS_OK;
   }
   /* some audio power; first guess is the selected standard */
   *standard = extAttr->standard;


#ifndef DRXK_EXCLUDE_AUDIO
   /*-------------------------------------------------------*/
   /*                                                       */
   /* ATV standard detection using the audio block          */
   /*                                                       */
   /*-------------------------------------------------------*/
   if( AUDCtrlSetStandard ( demod, &audioStandard)  != DRX_STS_OK )
   {
      /* cannot set standard, just return DRX_STANDARD_UNKNOWN */
      return DRX_STS_OK;
   }
   audioStandard = DRX_AUD_STANDARD_NOT_READY;

   /* try reading the detected standard */
   startTime = DRXBSP_HST_Clock();
   do
   {
      DRXBSP_HST_Sleep ( 10 );
      CHK_ERROR ( AUDCtrlGetStandard ( demod, &audioStandard) );
   } while
      ( audioStandard == DRX_AUD_STANDARD_NOT_READY &&
         ( ( DRXBSP_HST_Clock() - startTime ) < 0/*timeOut*/ ) );

   /* convert the audio standard to a DRXStandard_t type */
   switch ( audioStandard )
   {
      case DRX_AUD_STANDARD_BG_FM:
      case DRX_AUD_STANDARD_BG_NICAM_FM:
         *standard = DRX_STANDARD_PAL_SECAM_BG;
         break;
      case DRX_AUD_STANDARD_D_K1:
      case DRX_AUD_STANDARD_D_K2:
      case DRX_AUD_STANDARD_D_K3:
      case DRX_AUD_STANDARD_D_K_MONO:
      case DRX_AUD_STANDARD_D_K_NICAM_FM:
         *standard = DRX_STANDARD_PAL_SECAM_DK;
         break;
      case DRX_AUD_STANDARD_I_NICAM_FM:
         *standard = DRX_STANDARD_PAL_SECAM_I;
         break;
      case DRX_AUD_STANDARD_FM_STEREO:
         *standard = DRX_STANDARD_FM;
         break;
      case DRX_AUD_STANDARD_L_NICAM_AM:
         *standard = DRX_STANDARD_PAL_SECAM_L;
         break;
      default:
         *standard = DRX_STANDARD_UNKNOWN;
         break;
   }
#endif

   /* All done, all OK */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}



/*============================================================================*/
/**
* \brief ATV specific scanning function.
*
* \param demod:          Pointer to demodulator instance.
* \param scanCommand:    Scanning command: INIT, NEXT or STOP.
* \param scanChannel:    Channel to check: frequency
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
ScanFunctionATV    ( void*               scanContext,
                     DRXScanCommand_t    scanCommand,
                     pDRXChannel_t       scanChannel,
                     pBool_t             getNextChannel  )
{
   DRXStatus_t          status            = DRX_STS_ERROR;
   DRXLockStatus_t      lockStatus        = DRX_NOT_LOCKED;
   DRXStandard_t        detectedStandard  = DRX_STANDARD_UNKNOWN;

   pDRXDemodInstance_t  demod             = (pDRXDemodInstance_t) NULL;
   pDRXCommonAttr_t     commAttr          = (pDRXCommonAttr_t) NULL;
   pDRXKData_t          extAttr           = (pDRXKData_t) NULL;

   pDRXScanDataAtv_t    atvScanData       = {0};

   demod = (pDRXDemodInstance_t) scanContext;
   commAttr = (pDRXCommonAttr_t) demod->myCommonAttr;
   extAttr  = (pDRXKData_t) demod->myExtAttr;

   atvScanData = (pDRXScanDataAtv_t) commAttr->scanParam->extParams;

   if ( scanCommand == DRX_SCAN_COMMAND_INIT )
   {
      commAttr->scanDemodLockTimeout = 500;

      /* set scanning mode for ATV lock indication */
      status = ATVCtrlSetCfgAtvMode ( demod, DRXK_ATV_MODE_SCAN );
      if ( status != DRX_STS_OK )
      {
         return (status);
      }

      return DRX_STS_OK;
   }

   if ( scanCommand == DRX_SCAN_COMMAND_STOP )
   {
      commAttr->scanDemodLockTimeout = 3000;

      /* set "normal operation mode" for lock ATV lock indication */
      status = ATVCtrlSetCfgAtvMode ( demod, DRXK_ATV_MODE_LOCK );
      if ( status != DRX_STS_OK )
      {
         return (status);
      }

      return DRX_STS_OK;
   }

   *getNextChannel = FALSE;

   status = CtrlSetChannel( demod, scanChannel );
   if ( status != DRX_STS_OK )
   {
      return (status);
   }

   status =  WaitForLock(  demod,
                           &lockStatus,
                           commAttr->scanDemodLockTimeout );
   if ( status != DRX_STS_OK )
   {
      return (status);
   }

   /* done with this channel, move to next one */
   *getNextChannel = TRUE;
   if ( lockStatus != DRX_LOCKED)
   {
      /* Not locked, continue with next frequency */
      return DRX_STS_BUSY;
   }

   /* Check for a lock glitch */
   status = CtrlLockStatus(demod, &lockStatus);
   if ( status != DRX_STS_OK )
   {
      return (status);
   }
   if (lockStatus != DRX_LOCKED)
   {
      /* it was just a glitch */
      return DRX_STS_BUSY;
   }

   /* Try detecting the standard using audio */
   /* Note: without usage of DRXK audio blocks the detected
            standard will be equal to the set standard   */
   extAttr->atvSVRThreshold = atvScanData->svrThreshold;

   status = ATVCtrlGetCfgStandard(  demod,
                                    &detectedStandard );
   if ( status != DRX_STS_OK )
   {
      return (status);
   }

   if ( detectedStandard == DRX_STANDARD_UNKNOWN)
   {
      /* Can't detect audio standard, continue with next frequency */
      return DRX_STS_BUSY;
   }

   /* sneak in the standard for the next DRX_CTRL_GET_STANDARD */
   extAttr->standard = detectedStandard;

   /* channel found */
   return DRX_STS_OK;
}

/*============================================================================*/
/**
* \brief    Detects presence of audio carrier by means of checking the ratio of
*           digital audio and video amplification inside the DRXK
*
* \return DRXStatus_t
* \retval DRX_STS_OK Succes
* \retval DRX_STS_ERROR Timeout, I2C error, illegal bank
**/

static DRXStatus_t
ATVLockStatus (   pDRXDemodInstance_t   demod,
                  pDRXLockStatus_t      lockStat )
{
   pDRXCommonAttr_t  comAttr  = (pDRXCommonAttr_t)NULL;
   pI2CDeviceAddr_t  devAddr  = (pI2CDeviceAddr_t)NULL;
   pDRXKData_t       extAttr  = (pDRXKData_t)NULL;

   u16_t             rOffset        = 0;
   s16_t             measuredOffset = 0;

   comAttr = demod->myCommonAttr;
   devAddr    = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* get the lockstatus from scu */
   CHK_ERROR( SCULockStatus(  demod, SCU_RAM_COMMAND_STANDARD_ATV,
                              lockStat ) );

   /* check if the frequencyoffset is in range*/
   if ( *lockStat == DRX_LOCKED )
   {
      RR16(devAddr, ATV_TOP_CR_FREQ__A, &rOffset);

      measuredOffset = (s16_t)rOffset;

      /* Signed 8 bit register => sign extension needed */
      if ( (measuredOffset & 0x0080) != 0)
      {
         /* sign extension */
         measuredOffset |= 0xFF80;
      }
      measuredOffset *= 10;
   }

   /* outside this range the lock is not reliable */
   if (  ( measuredOffset < extAttr->presets.lockRangeMin ) ||
         ( measuredOffset > extAttr->presets.lockRangeMax) )
   {
      *lockStat = DRX_NOT_LOCKED;
   }

   if( !comAttr->scanActive )
   {
      if ( *lockStat == DRX_LOCKED )
      {
         /* Carrier-recovery to locked mode */
         CHK_ERROR( ATVCtrlSetCfgAtvMode( demod, DRXK_ATV_MODE_LOCK ) );
      }
      else
      {
         /* Carrier-recovery to lock-in mode */
         CHK_ERROR( ATVCtrlSetCfgAtvMode( demod, DRXK_ATV_MODE_SCAN ) );
      }
   }
   /* All done, all OK */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/*============================================================================*/
/**
* \brief Set up ATV demodulator.
* \param demod instance of demodulator
* \param standard    ATV standard
* \return DRXStatus_t.
*
* Init all channel independent registers.
* Assuming that IQM, ATV and AUD blocks have been reset and are in STOP mode
*
*/
static DRXStatus_t
ATVSetStandard( pDRXDemodInstance_t   demod , pDRXStandard_t standard )
{
   pI2CDeviceAddr_t  devAddr        = (pI2CDeviceAddr_t)NULL;

   DRXStandard_t     prevStandard   = DRX_STANDARD_UNKNOWN;
   DRXKSCUCmd_t      cmdSCU         = {   /* command      */ 0,
                                          /* parameterLen */ 0,
                                          /* resultLen    */ 0,
                                          /* *parameter   */ NULL,
                                          /* *result      */ NULL };
   u16_t            cmdResult  = 0;
   u16_t            cmdParam   = 0;


   /* ATV standard specific parameters */
   u16_t             romOffset            = 0;
   u16_t            scuRamAtvAgcMode = 0;
   u16_t             atvTopCrCont         = 0;
   u16_t             atvTopStd            = 0;
   u16_t             atvTopVidAmp         = 0;
   u16_t             scuRamAtvVidGainHi   = 0;
   u16_t             scuRamAtvVidGainLo   = 0;

   pDRXKData_t      extAttr    = (pDRXKData_t) NULL;
   pDRXCommonAttr_t comAttr    = (pDRXCommonAttr_t) NULL;

   extAttr        = (pDRXKData_t)demod->myExtAttr;
   devAddr        = demod -> myI2CDevAddr;
   comAttr        = demod->myCommonAttr;

   prevStandard   = extAttr->standard;

   /* Ensure correct power-up mode */
   CHK_ERROR( ATVPowerUp( demod ) );

   /* different standard, maybe we should start with substandard main */
   if ( *standard != prevStandard )
   {
      extAttr->substandard = DRX_SUBSTANDARD_MAIN;
   }

   WR16( devAddr, ATV_COMM_EXEC__A, ATV_COMM_EXEC_STOP );
   WR16( devAddr, IQM_COMM_EXEC__A, IQM_COMM_EXEC_B_STOP );

   /* Reset ATV SCU */
   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_ATV |
                         SCU_RAM_COMMAND_CMD_DEMOD_RESET;
   cmdSCU.parameterLen = 0;
   cmdSCU.resultLen    = 1;
   cmdSCU.parameter    = NULL;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

   /* Halt SCU to enable safe non-atomic accesses */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_HOLD );

   /* initialize variables with standard specific data */
   switch ( *standard )
   {
      case DRX_STANDARD_NTSC:
         /* NTSC */
         cmdParam    = (u16_t) SCU_RAM_ATV_STANDARD_STANDARD_MN;
         romOffset   = (u16_t) DRXK_BL_ROM_OFFSET_TAPS_NTSC;

         atvTopStd = (u16_t)( ATV_TOP_STD_MODE_MN |
                              ATV_TOP_STD_VID_POL_MN);

         atvTopCrCont = (u16_t)( ATV_TOP_CR_CONT_CR_P_MN |
                              ATV_TOP_CR_CONT_CR_D_MN |
                                 ATV_TOP_CR_CONT_CR_I_MN );

         atvTopVidAmp = (u16_t)( ATV_TOP_VID_AMP_MN );


         scuRamAtvAgcMode = (u16_t)
                          ( SCU_RAM_ATV_AGC_MODE_SIF_STD_SIF_AGC_FM |
                            SCU_RAM_ATV_AGC_MODE_FAST_VAGC_EN_FAGC_ENABLE );

         scuRamAtvVidGainHi = (u16_t) 0x1000;
         scuRamAtvVidGainLo = (u16_t) 0x0000;

         extAttr->phaseCorrectionBypass = FALSE;
         break;
      case DRX_STANDARD_FM:
         cmdParam    = (u16_t)SCU_RAM_ATV_STANDARD_STANDARD_FM;
         romOffset   = (u16_t)DRXK_BL_ROM_OFFSET_TAPS_FM;

         atvTopStd = (u16_t)( ATV_TOP_STD_MODE_FM |
                              ATV_TOP_STD_VID_POL_FM ) ;
         scuRamAtvAgcMode = (u16_t)
                            ( SCU_RAM_ATV_AGC_MODE_VAGC_VEL_AGC_SLOW |
                              SCU_RAM_ATV_AGC_MODE_SIF_STD_SIF_AGC_FM );


         extAttr->phaseCorrectionBypass = TRUE;
         break;
      case DRX_STANDARD_PAL_SECAM_BG:
         /* PAL/SECAM B/G */
         cmdParam    = (u16_t) SCU_RAM_ATV_STANDARD_STANDARD_B;
         romOffset   = (u16_t) DRXK_BL_ROM_OFFSET_TAPS_BG;


         atvTopStd      = (u16_t)(  ATV_TOP_STD_MODE_BG |
                                    ATV_TOP_STD_VID_POL_BG );
         atvTopCrCont   = (u16_t)(  ATV_TOP_CR_CONT_CR_P_BG |
                              ATV_TOP_CR_CONT_CR_D_BG |
                                    ATV_TOP_CR_CONT_CR_I_BG );
         atvTopVidAmp   = (u16_t)   DRXK_ATV_TOP_VID_AMP_BG_ADAPTED;

         scuRamAtvAgcMode  = (u16_t)
                          ( SCU_RAM_ATV_AGC_MODE_SIF_STD_SIF_AGC_FM |
                            SCU_RAM_ATV_AGC_MODE_FAST_VAGC_EN_FAGC_ENABLE );
         scuRamAtvVidGainHi = (u16_t) 0x1000;
         scuRamAtvVidGainLo = (u16_t) 0x0000;
         extAttr->phaseCorrectionBypass = FALSE;
         break;

      case DRX_STANDARD_PAL_SECAM_DK:
         /* PAL/SECAM D/K */
         cmdParam    = (u16_t)SCU_RAM_ATV_STANDARD_STANDARD_DK;
         romOffset      = (u16_t) DRXK_BL_ROM_OFFSET_TAPS_DKILLP;
         atvTopVidAmp   = (u16_t)   DRXK_ATV_TOP_VID_AMP_DK_ADAPTED;
         atvTopCrCont   = (u16_t)(  ATV_TOP_CR_CONT_CR_P_DK |
                              ATV_TOP_CR_CONT_CR_D_DK |
                                    ATV_TOP_CR_CONT_CR_I_DK );
         atvTopStd      = (u16_t)(  ATV_TOP_STD_MODE_DK |
                                    ATV_TOP_STD_VID_POL_DK );
         scuRamAtvAgcMode = (u16_t)
                          ( SCU_RAM_ATV_AGC_MODE_SIF_STD_SIF_AGC_FM |
                            SCU_RAM_ATV_AGC_MODE_FAST_VAGC_EN_FAGC_ENABLE );

         scuRamAtvVidGainHi = (u16_t) 0x1000;
         scuRamAtvVidGainLo = (u16_t) 0x0000;
         extAttr->phaseCorrectionBypass = FALSE;
         break;

      case DRX_STANDARD_PAL_SECAM_I:
         /* PAL/SECAM I   */
         cmdParam    = (u16_t)SCU_RAM_ATV_STANDARD_STANDARD_I;
         romOffset   = (u16_t) DRXK_BL_ROM_OFFSET_TAPS_DKILLP;

         atvTopVidAmp   = (u16_t)  DRXK_ATV_TOP_VID_AMP_I_ADAPTED;
         atvTopCrCont   = (u16_t)(  ATV_TOP_CR_CONT_CR_P_I |
                              ATV_TOP_CR_CONT_CR_D_I |
                                    ATV_TOP_CR_CONT_CR_I_I );
         atvTopStd      = (u16_t)(  ATV_TOP_STD_MODE_I |
                                    ATV_TOP_STD_VID_POL_I );
         scuRamAtvAgcMode = (u16_t)
                          ( SCU_RAM_ATV_AGC_MODE_SIF_STD_SIF_AGC_FM |
                            SCU_RAM_ATV_AGC_MODE_FAST_VAGC_EN_FAGC_ENABLE );
         scuRamAtvVidGainHi = (u16_t) 0x1000;
         scuRamAtvVidGainLo = (u16_t) 0x0000;
         extAttr->phaseCorrectionBypass = FALSE;
         break;

      case DRX_STANDARD_PAL_SECAM_L:
         /* PAL/SECAM L with negative modulation */
         cmdParam    = (u16_t)SCU_RAM_ATV_STANDARD_STANDARD_L;
         romOffset   = (u16_t) DRXK_BL_ROM_OFFSET_TAPS_DKILLP;

         atvTopVidAmp   = (u16_t) ATV_TOP_VID_AMP_L;

         atvTopCrCont   = (u16_t)(  0x3 | /* less reactive carrier recovery */
                              ATV_TOP_CR_CONT_CR_D_L |
                                    ATV_TOP_CR_CONT_CR_I_L );

         atvTopStd      = (u16_t)(  ATV_TOP_STD_MODE_L |
                                    ATV_TOP_STD_VID_POL_L );
         scuRamAtvAgcMode = (u16_t)
                          ( SCU_RAM_ATV_AGC_MODE_SIF_STD_SIF_AGC_AM |
                            SCU_RAM_ATV_AGC_MODE_BP_EN_BPC_ENABLE   |
                            SCU_RAM_ATV_AGC_MODE_VAGC_VEL_AGC_SLOW );

         scuRamAtvVidGainHi = (u16_t) 0x1000;
         scuRamAtvVidGainLo = (u16_t) 0x0000;
         extAttr->phaseCorrectionBypass = FALSE;
         break;

      case DRX_STANDARD_PAL_SECAM_LP:
         /* PAL/SECAM L with positive modulation */
         cmdParam = SCU_RAM_ATV_STANDARD_STANDARD_LP;
         romOffset   = (u16_t) DRXK_BL_ROM_OFFSET_TAPS_DKILLP;

         atvTopVidAmp   = (u16_t) ATV_TOP_VID_AMP_LP;

         atvTopCrCont   = (u16_t)(  ATV_TOP_CR_CONT_CR_P_LP |
                              ATV_TOP_CR_CONT_CR_D_LP |
                                    ATV_TOP_CR_CONT_CR_I_LP );
         atvTopStd      = (u16_t)(  ATV_TOP_STD_MODE_LP |
                                    ATV_TOP_STD_VID_POL_LP );
         scuRamAtvAgcMode = (u16_t)
                          ( SCU_RAM_ATV_AGC_MODE_SIF_STD_SIF_AGC_AM |
                            SCU_RAM_ATV_AGC_MODE_BP_EN_BPC_ENABLE   |
                            SCU_RAM_ATV_AGC_MODE_VAGC_VEL_AGC_SLOW );

         scuRamAtvVidGainHi = (u16_t) 0x1000;
         scuRamAtvVidGainLo = (u16_t) 0x0000;
         extAttr->phaseCorrectionBypass = FALSE;
         break;
      default:
         return DRX_STS_ERROR;
         break;
   }

   /* write standard specific IQM regsiters */
   WR16( devAddr, IQM_CF_MIDTAP__A,    IQM_CF_MIDTAP_RE__M );
   if ( *standard == DRX_STANDARD_FM )
   {
      WR16( devAddr, IQM_RT_ROT_BP__A, IQM_RT_ROT_BP_ROT_OFF_OFF );
   }

   /* write ATV TOP registers */
   WR16( devAddr, ATV_TOP_STD__A,      atvTopStd );
   WR16( devAddr, ATV_TOP_CR_CONT__A,  atvTopCrCont );

   if ( *standard != DRX_STANDARD_FM )
   {
      WR16( devAddr, ATV_TOP_VID_AMP__A,           atvTopVidAmp );

      /* write SCU_RAM_ATV registers */
      WR16( devAddr, SCU_RAM_ATV_VID_GAIN_HI__A,   scuRamAtvVidGainHi );
      WR16( devAddr, SCU_RAM_ATV_VID_GAIN_LO__A,   scuRamAtvVidGainLo );
   }

   /* default filter coefficients */
   /* upload by boot loader from ROM table */
   if (  (extAttr->presets.channelFilter.dataRe == NULL) ||
         (extAttr->presets.channelFilter.dataIm == NULL) )
   {
      CHK_ERROR( BLChainCmd(  devAddr,
                              romOffset,
                              DRXK_BLCC_NR_ELEMENTS_TAPS));
   }
   else
   {
      /* load filters from presets */
      CHK_ERROR ( CtrlLoadFilter( demod, &extAttr->presets.channelFilter ));
      WR16( devAddr, IQM_CF_SCALE__A, extAttr->presets.cfScale);
   }


   /* imbalance control: disabled for Secam L/Lp and FM, enabled otherwise */
   if (  ( *standard == DRX_STANDARD_PAL_SECAM_L  ) ||
         ( *standard == DRX_STANDARD_PAL_SECAM_LP ) ||
         ( *standard == DRX_STANDARD_FM ) )
   {
      scuRamAtvAgcMode |= SCU_RAM_ATV_AGC_MODE_MOD_WA_BP_MWA_DISABLE;
      WR16( devAddr, ATV_TOP_MOD_CONTROL__A, 0 );
      WR16( devAddr, ATV_TOP_MOD_ACCU__A,    0 );
   }
   else
   {
      scuRamAtvAgcMode |= SCU_RAM_ATV_AGC_MODE_MOD_WA_BP_MWA_ENABLE;
      WR16( devAddr, ATV_TOP_MOD_CONTROL__A, ATV_TOP_MOD_CONTROL__PRE );
   }

   WR16( devAddr, SCU_RAM_ATV_AGC_MODE__A ,     scuRamAtvAgcMode );

   /* Now standard is set */
   extAttr->standard = *standard;

   /* standard dependency handled by presets */
   WR16( devAddr, SCU_RAM_ATV_AMS_MAX_REF__A,
                                 extAttr->presets.atvCvbsRefLvl);
   /* write SCU_RAM_ATV_BP_RELY__A to zero, directly after setting
      SCU_RAM_ATV_AMS_MAX_REF__A. This will make CVBS adaptions possible
      for positive and negative modulation*/
   WR16( devAddr, SCU_RAM_ATV_BP_RELY__A    , 0    );

   /* Common initializations FM & NTSC & B/G & D/K & I & L & LP */
   WR16( devAddr, IQM_AF_AMUX__A, IQM_AF_AMUX_SIGNAL2LOWPASS);

#define IQM_AF_CLP_LEN_ATV  0
#define IQM_AF_CLP_TH_ATV   448
#define IQM_AF_SNS_LEN_ATV  0
#define IQM_RC_STRETCH_ATV  15

   WR16( devAddr, IQM_AF_CLP_LEN__A       , IQM_AF_CLP_LEN_ATV );
   WR16( devAddr, IQM_AF_CLP_TH__A        , IQM_AF_CLP_TH_ATV );
   WR16( devAddr, IQM_AF_SNS_LEN__A       , IQM_AF_SNS_LEN_ATV );
   CHK_ERROR( AGCCtrlSetCfgPreSaw( demod, &(extAttr->presets.preSawCfg)) );
   WR16( devAddr, IQM_AF_AGC_IF__A        , 10248 );

   WR32( devAddr, IQM_RC_RATE_OFS_LO__A   , 0x00200000L );
   WR16( devAddr, IQM_RC_ADJ_SEL__A       , IQM_RC_ADJ_SEL_B_OFF );
   WR16( devAddr, IQM_RC_STRETCH__A       , IQM_RC_STRETCH_ATV );

   WR16( devAddr, IQM_RT_ACTIVE__A        , IQM_RT_ACTIVE_ACTIVE_RT_ATV_FCR_ON |
                                            IQM_RT_ACTIVE_ACTIVE_CR_ATV_CR_ON );

   WR16( devAddr, IQM_CF_OUT_ENA__A       , IQM_CF_OUT_ENA_ATV__M );
   WR16( devAddr, IQM_CF_SYMMETRIC__A     , IQM_CF_SYMMETRIC_IM__M );
   /* default: SIF in standby */
   WR16( devAddr, ATV_TOP_SYNC_SLICE__A   , ATV_TOP_SYNC_SLICE_MN );
   WR16( devAddr, ATV_TOP_MOD_ACCU__A     , ATV_TOP_MOD_ACCU__PRE );

   WR16( devAddr, SCU_RAM_ATV_SIF_GAIN__A     , 0x080 );
   WR16( devAddr, SCU_RAM_ATV_FAGC_TH_RED__A  , 10    );
   WR16( devAddr, SCU_RAM_ATV_AAGC_CNT__A     , 7     );
   WR16( devAddr, SCU_RAM_ATV_NAGC_KI_MIN__A  , 535   );
   WR16( devAddr, SCU_RAM_ATV_LOCK__A         , 0     );

   WR16( devAddr, IQM_RT_DELAY__A           , IQM_RT_DELAY__PRE );
   WR16( devAddr, SCU_RAM_ATV_BPC_KI_MIN__A , 531  );
   WR16( devAddr, SCU_RAM_ATV_BP_REF_MIN__A , 100  );
   WR16( devAddr, SCU_RAM_ATV_BP_REF_MAX__A , 260  );
   WR16( devAddr, SCU_RAM_ATV_BP_LVL__A     , 0    );
   WR16( devAddr, SCU_RAM_ATV_AMS_MAX__A    , 0    );
   WR16( devAddr, SCU_RAM_ATV_AMS_MIN__A    , 2047 );


   /* When the Modulation mode is FM then turn off CVBS output*/
   if ( extAttr->standard == DRX_STANDARD_FM)
   {
      extAttr->enableCVBSOutput = FALSE;
   }
   else
   {
      extAttr->enableCVBSOutput = TRUE;
   }

   /* Override reset values with current shadow settings */
   CHK_ERROR( ATVUpdateConfig( demod, TRUE) );

   /* IQM will not be reset from here, sync ADC and update/init AGC */
   CHK_ERROR( ADCSynchronization (demod) );

   /* Configure/restore AGC settings */
   CHK_ERROR( AGCInit(  demod, FALSE ) );
   CHK_ERROR( AGCCtrlSetCfgPreSaw( demod, &(extAttr->presets.preSawCfg)) );
   CHK_ERROR( AGCSetRf( demod, &(extAttr->presets.rfAgcCfg), FALSE ) );
   CHK_ERROR( AGCSetIf( demod, &(extAttr->presets.ifAgcCfg), FALSE ) );

   /* Set IIR workaround according to ROM content */
   CHK_ERROR ( ATVSetIIRWorkAround(demod, standard));

   /* Activate SCU to enable SCU commands */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_ACTIVE );

   /* Set SCU ATV standard, assuming this doesn't require running ATV block */
   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_ATV |
                         SCU_RAM_COMMAND_CMD_DEMOD_SET_ENV;
   cmdSCU.parameterLen = 1;
   cmdSCU.resultLen    = 1;
   cmdSCU.parameter    = &cmdParam;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

   /* Start SCU */
   CHK_ERROR( ATVStart( demod));

   CHK_ERROR( ADCCompensationRestoreParams(demod));

   /* set scanning function pointer */
   comAttr->scanFunction = ScanFunctionATV;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/**
* \brief Parameter check for ATV set channel.
* \param demod   instance of demodulator.
* \param channel channel parameters to be checked/modified.
* \return DRXStatus_t.
* \retval DRX_STS_OK          channel parameters are valid.
* \retval DRX_STS_INVALID_ARG illegal channel parameters.
*
* Receiver parameter check for dvbt.
* Parameters may be modified to an equivalent setting.
*
*/
static DRXStatus_t
ATVSetChParamCheck (pDRXDemodInstance_t   demod,
                     pDRXChannel_t         channel )
{
   pDRXKData_t        extAttr           = NULL;
   DRXStandard_t      standard          = DRX_STANDARD_UNKNOWN;

   extAttr    = (pDRXKData_t)demod -> myExtAttr;
   standard   = extAttr->standard;

   /* check bandwidth PAL/SECAM DK/I/L/LP  */
   if ( ( standard == DRX_STANDARD_PAL_SECAM_DK ) ||
        ( standard == DRX_STANDARD_PAL_SECAM_I  ) ||
        ( standard == DRX_STANDARD_PAL_SECAM_L  ) ||
        ( standard == DRX_STANDARD_PAL_SECAM_LP ) )
   {
            channel->bandwidth = DRX_BANDWIDTH_8MHZ;
   }

   /* check bandwidth PAL/SECAM BG*/
   if ( standard == DRX_STANDARD_PAL_SECAM_BG )
   {
      switch ( channel->bandwidth )
      {
         case DRX_BANDWIDTH_7MHZ    :
            channel->bandwidth = DRX_BANDWIDTH_7MHZ;
            break;
         case DRX_BANDWIDTH_8MHZ    :
            channel->bandwidth = DRX_BANDWIDTH_8MHZ;
            break;
         default                    :
            return DRX_STS_INVALID_ARG;
      }
   }

   /* check bandwidth NTSC */
   if( standard == DRX_STANDARD_NTSC  )
   {
      channel->bandwidth = DRX_BANDWIDTH_6MHZ;
   }
   return DRX_STS_OK;
}

/* -------------------------------------------------------------------------- */

/**
* \brief Set ATV channel.
* \param demod:   instance of demod.
* \return DRXStatus_t.
*
* Not much needs to be done here, only start the SCU for NTSC/FM.
* Mirrored channels are not expected in the RF domain, so IQM FS setting
* doesn't need to be remembered.
* The channel->mirror parameter is therefor ignored.
*
*/
static DRXStatus_t
ATVSetChannel( pDRXDemodInstance_t demod,
               DRXFrequency_t      tunerFreqOffset,
               pDRXChannel_t       channel,
               DRXStandard_t       standard )
{
   DRXKSCUCmd_t       cmdSCU    = {/* command      */ 0,
                                   /* parameterLen */ 0,
                                   /* resultLen    */ 0,
                                   /* parameter    */ NULL,
                                   /* result       */ NULL };
   u16_t              cmdResult = 0;
   pDRXKData_t        extAttr   = (pDRXKData_t) NULL;
   pDRXCommonAttr_t   comAttr   = (pDRXCommonAttr_t) NULL;
   pI2CDeviceAddr_t   devAddr   = NULL;

   devAddr = demod -> myI2CDevAddr;
   comAttr = (pDRXCommonAttr_t)  demod->myCommonAttr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /*
      Program frequency shifter
      No need to account for mirroring on RF
   */
   CHK_ERROR ( SetFrequency ( demod, channel, tunerFreqOffset ) );

   WR16(devAddr, ATV_TOP_CR_FREQ__A, ATV_TOP_CR_FREQ__PRE);
   WR16(devAddr, SCU_RAM_ATV_NAGC_KI_MIN__A, extAttr->presets.atvNAgcKiMin );

   if ( !comAttr->scanActive)
   {
   /* Carrier-recovery to lock-in mode */
   CHK_ERROR( ATVCtrlSetCfgAtvMode( demod, DRXK_ATV_MODE_SCAN ) );
   }
   /* Start ATV SCU */
   cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_ATV |
                         SCU_RAM_COMMAND_CMD_DEMOD_START;
   cmdSCU.parameterLen = 0;
   cmdSCU.resultLen    = 1;
   cmdSCU.parameter    = NULL;
   cmdSCU.result       = &cmdResult;
   CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );

   (extAttr->currentChannel) = *channel;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/* -------------------------------------------------------------------------- */

/**
* \brief Set ATV channel.
* \param demod:   instance of demod.
* \param channel: pointer to channel data.
* \param standard: NTSC or FM.
* \return DRXStatus_t.
*
* Covers NTSC, PAL/SECAM - B/G, D/K, I, L, LP and FM.
* Computes the frequency offset in te RF domain and adds it to
* channel->frequency. Determines the value for channel->bandwidth.
*
*/
static DRXStatus_t
ATVGetChannel( pDRXDemodInstance_t   demod,
               pDRXChannel_t         channel,
               DRXStandard_t         standard )
{
   DRXFrequency_t   offset    = 0;
   pI2CDeviceAddr_t devAddr   = NULL;
   pDRXKData_t      extAttr   = NULL;

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* Getting channel information. */
   *channel = (extAttr->currentChannel);

   switch ( standard )
   {
      case DRX_STANDARD_NTSC:
      case DRX_STANDARD_PAL_SECAM_BG:
      case DRX_STANDARD_PAL_SECAM_DK:
      case DRX_STANDARD_PAL_SECAM_I:
      case DRX_STANDARD_PAL_SECAM_L:
      {
            u16_t measuredOffset=0;

            /* get measured frequency offset */
            RR16(devAddr, ATV_TOP_CR_FREQ__A, &measuredOffset);
            /* Signed 8 bit register => sign extension needed */
            if ( (measuredOffset & 0x0080) != 0)
            {
               /* sign extension */
               measuredOffset |= 0xFF80;
            }
            offset+= (DRXFrequency_t)( ((s16_t)measuredOffset)*10);
            break;
      }
      case DRX_STANDARD_PAL_SECAM_LP:
         {
            u16_t measuredOffset=0;

            /* get measured frequency offset */
            RR16(devAddr, ATV_TOP_CR_FREQ__A, &measuredOffset);
            /* Signed 8 bit register => sign extension needed */
            if ( (measuredOffset & 0x0080) != 0)
            {
               /* sign extension */
               measuredOffset |= 0xFF80;
            }
            offset-= (DRXFrequency_t)( ((s16_t)measuredOffset)*10);
         }
         break;
      case DRX_STANDARD_FM:
         /* No bandwidth know for FM */
         channel->bandwidth = DRX_BANDWIDTH_UNKNOWN;
         break;
      default:
         return ( DRX_STS_ERROR );
   }

   channel->frequency -= offset;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/**
* \brief Get current IF frequency from tuner implementation.
* \param demod:   instance of demod.
* \param channel: pointer to channel data.
* \param ifFrequency: pointer to frequency
* \return DRXStatus_t.
**/
DRXStatus_t ATVGetIfFrequency (  pDRXDemodInstance_t  demod,
                                 pDRXChannel_t        channel,
                                 pDRXFrequency_t      ifFrequency )
{
   DRXStandard_t     standard;
   TUNERMode_t       tunerMode   = 0;
   DRXFrequency_t    dummy       = 0;
   Bool_t            tunerSlowMode  = FALSE;

   CHK_ERROR( CtrlGetStandard( demod, &standard));

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

   if ( demod->myTuner != NULL )
   {
      CHK_ERROR( DRXBSP_TUNER_GetFrequency(  demod->myTuner,
                                             tunerMode,
                                             &dummy,
                                             ifFrequency) );
   }
   else
   {
      *ifFrequency = 0;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;

}


/**
* \brief Calculates tuner frequency
* \param standard     Current ATV standard.
* \param frequency    Current frequency of channel.
* \param tunerSetFreq Frequency of tuner.
* \return DRXStatus_t.
* \retval DRX_STS_OK sigStrength contains valid data.
* \retval DRX_STS_ERROR Erroneous data.
*/
static DRXStatus_t
ATVTunerFreq(
               pDRXDemodInstance_t  demod,
               DRXStandard_t        standard,
               pDRXChannel_t        channel,
               pDRXFrequency_t      tunerSetFreq )
{
   pDRXKData_t      extAttr   = NULL;
   s32_t            delta = 0;
   DRXFrequency_t    ifFrequency = 0;

   extAttr = (pDRXKData_t)demod->myExtAttr;

   CHK_ERROR( ATVGetIfFrequency( demod,
                                 channel,
                                 &ifFrequency ));

   if (  (extAttr->presets.useTgtCarrierIf) )
   {
      switch ( standard )
      {
         case DRX_STANDARD_NTSC:
            delta = extAttr->presets.tgtCarrierIf
               - ifFrequency
               - DRX_CARRIER_OFFSET_PAL_NTSC;
            break;
         case DRX_STANDARD_PAL_SECAM_BG:
               if (channel->bandwidth == DRX_BANDWIDTH_7MHZ)
               {
                  delta = extAttr->presets.tgtCarrierIf
                     - ifFrequency
                     - DRX_CARRIER_OFFSET_PAL_SECAM_BG_7MHZ;
               }
               else
               {
                  delta = extAttr->presets.tgtCarrierIf
                     - ifFrequency
                     - DRX_CARRIER_OFFSET_PAL_SECAM_BG_8MHZ;
               }
            break;
         case DRX_STANDARD_PAL_SECAM_DK: /* fallthrough */
         case DRX_STANDARD_PAL_SECAM_I:  /* fallthrough */
         case DRX_STANDARD_PAL_SECAM_L:  /* fallthrough */
               delta = extAttr->presets.tgtCarrierIf
               - ifFrequency
                  - DRX_CARRIER_OFFSET_PAL_SECAM_DKIL;
            break;
         case DRX_STANDARD_PAL_SECAM_LP:
               delta = extAttr->presets.tgtCarrierIf
               - ifFrequency
                  - DRX_CARRIER_OFFSET_PAL_SECAM_LP;

            break;
         case DRX_STANDARD_FM:
            /* center frequency (equals sound carrier) as input,
               tune to edge of SAW */
               delta = DRX_CARRIER_OFFSET_FM;

            break;
         case DRX_STANDARD_UNKNOWN:
         default:
            return DRX_STS_ERROR;
      } /* switch(standard) */
   }
   else
   {
      delta = 0;
   }

   *tunerSetFreq = channel->frequency + delta;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/* -------------------------------------------------------------------------- */

/**
* \brief Retrieve signal quality indication for ATV.
* \param demod Pointer to demodulator instance.
* \param sigQuality Pointer to signal quality structure.
* \return DRXStatus_t.
* \retval DRX_STS_OK sigQuality contains valid data.
* \retval DRX_STS_ERROR Erroneous data, sigQuality indicator equals 0.
*
*
*/
static DRXStatus_t
ATVSigQuality( pDRXDemodInstance_t demod,
               pDRXSigQuality_t    sigQuality )
{
   pI2CDeviceAddr_t   devAddr          = NULL;
   u16_t              qualityIndicator = 0;

   devAddr = demod -> myI2CDevAddr;

   /* defined values for fields not used */
   sigQuality->MER                = 0;
   sigQuality->preViterbiBER      = 0;
   sigQuality->postViterbiBER     = 0;
   sigQuality->scaleFactorBER     = 1;
   sigQuality->packetError        = 0;
   sigQuality->postReedSolomonBER = 0;


   /*
      Mapping:
      0x000..0x080: strong signal  => 80% .. 100%
      0x080..0x700: weak signal    => 30% .. 80%
      0x700..0x7ff: no signal      => 0%  .. 30%
   */

   SARR16( devAddr, SCU_RAM_ATV_CR_LOCK__A, &qualityIndicator );
   qualityIndicator &= SCU_RAM_ATV_CR_LOCK_CR_LOCK__M;
   if ( qualityIndicator <= 0x80 )
   {
      sigQuality->indicator = 80 + ( (20*(0x80-qualityIndicator))/0x80);
   } else if ( qualityIndicator <= 0x700 )
   {
      sigQuality->indicator = 30 +
                           ( (50*(0x700-qualityIndicator))/(0x700-0x81));
   } else {
      sigQuality->indicator =
                           (30*(0x7FF-qualityIndicator))/(0x7FF-0x701);
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/*==                   ATV RELATED FUNCTIONS - E N D                        ==*/
/*============================================================================*/

static DRXStatus_t CtrlUIOWrite( pDRXDemodInstance_t, pDRXUIOData_t );

/**
* \fn DRXStatus_t CtrlSetUIOCfg()
* \brief Configure modus oprandi UIO.
* \param demod Pointer to demodulator instance.
* \param UIOCfg Pointer to a configuration setting for a certain UIO.
* \return DRXStatus_t.
*/
static DRXStatus_t
CtrlSetUIOCfg( pDRXDemodInstance_t demod, pDRXUIOCfg_t UIOCfg )
{
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t      extAttr = (pDRXKData_t)      NULL;

   if (( UIOCfg == NULL ) || ( demod == NULL ))
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t)demod -> myExtAttr;

   /* stop lock indicator process */
   WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_DISABLE);

   switch ( UIOCfg->uio ) {
      /*====================================================================*/
      case DRX_UIO1 :
         /* DRX_UIO1: SAW SW, UIO-1 */
         if (extAttr->hasSAWSW != TRUE)
            return DRX_STS_ERROR;
         switch ( UIOCfg->mode )
         {
            case DRXK_UIO_MODE_FIRMWARE_SAW:            /* falltrough */
            case DRX_UIO_MODE_READWRITE:
               extAttr->uioSawSwMode = UIOCfg->mode;
               break;
            case DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR:
               extAttr->uioSawSwMode = UIOCfg->mode;
               {
                  DRXUIOData_t uio1 = { DRX_UIO1, FALSE };
                  CHK_ERROR( CtrlUIOWrite( demod, &uio1 ) );
               }
               break;
            case DRX_UIO_MODE_DISABLE:
               extAttr->uioSawSwMode = UIOCfg->mode;
               /* pad configuration register is set 0 - input mode */
               WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A,
                     SIO_TOP_COMM_KEY_KEY );
               WR16( demod->myI2CDevAddr, SIO_PDR_SMA_TX_CFG__A, 0 );
               WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, 0x0000 );
               break;
            default:
               return DRX_STS_INVALID_ARG;
           } /* switch ( UIOCfg->mode ) */
        break;
      /*====================================================================*/
      case DRX_UIO2 :
         /* DRX_UIO2: GPIO2 UIO-2 */
         if (extAttr->hasGPIO2 != TRUE)
            return DRX_STS_ERROR;
         switch ( UIOCfg->mode )
         {
            case DRX_UIO_MODE_FIRMWARE0: /* falltrough */
            case DRX_UIO_MODE_READWRITE:
              extAttr->uioGPIO2Mode = UIOCfg->mode;
              break;
            case DRX_UIO_MODE_DISABLE:
               extAttr->uioGPIO2Mode = UIOCfg->mode;
               /* pad configuration register is set 0 - input mode */
               WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A,
                     SIO_TOP_COMM_KEY_KEY );
               WR16( demod->myI2CDevAddr, SIO_PDR_SMA_RX_CFG__A, 0 );
               WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, 0x0000 );
              break;
            default:
               return DRX_STS_INVALID_ARG;
               break;
         } /* switch ( UIOCfg->mode ) */
         break;
      /*====================================================================*/
      case DRX_UIO3 :
         /* DRX_UIO3: GPIO1 UIO-3 */
         if (extAttr->hasGPIO1 != TRUE)
            return DRX_STS_ERROR;
         switch ( UIOCfg->mode )
         {
            case DRX_UIO_MODE_FIRMWARE0: /* falltrough */
            case DRX_UIO_MODE_READWRITE:
              extAttr->uioGPIO1Mode = UIOCfg->mode;
              break;
            case DRX_UIO_MODE_DISABLE:
              extAttr->uioGPIO1Mode = UIOCfg->mode;
              /* pad configuration register is set 0 - input mode */
               WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A,
                     SIO_TOP_COMM_KEY_KEY );
              WR16( demod->myI2CDevAddr, SIO_PDR_GPIO_CFG__A, 0 );
              WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, 0x0000 );
              break;
            default:
               return DRX_STS_INVALID_ARG;
               break;
         } /* switch ( UIOCfg->mode ) */
       break;
      /*====================================================================*/
      default:
         return DRX_STS_INVALID_ARG;
   } /* switch ( UIOCfg->uio ) */

   if ( extAttr->uioSawSwMode ==
         DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR )
   {
      /* restart lock indicator process */
      WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_ENABLE );
   }

   return DRX_STS_OK;


 rw_error:
   return DRX_STS_ERROR;
}
/*============================================================================*/
/**
* \fn DRXStatus_t CtrlGetUIOCfg()
* \brief Get modus oprandi UIO.
* \param demod Pointer to demodulator instance.
* \param UIOCfg Pointer to a configuration setting for a certain UIO.
* \return DRXStatus_t.
*/
static DRXStatus_t
CtrlGetUIOCfg( pDRXDemodInstance_t demod, pDRXUIOCfg_t UIOCfg )
{

   pDRXKData_t    extAttr           = (pDRXKData_t)      NULL;
   pDRXUIOMode_t  UIOMode[3]        = {NULL};
   pBool_t        UIOAvailable[3]   = {NULL};

   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* UIO1: SawSw */
   /* UIO2: GPIO2 */
   /* UIO3: GPIO1 */

   UIOMode[DRX_UIO1] = &extAttr->uioSawSwMode;
   UIOMode[DRX_UIO2] = &extAttr->uioGPIO2Mode;
   UIOMode[DRX_UIO3] = &extAttr->uioGPIO1Mode;

   UIOAvailable[DRX_UIO1] = &extAttr->hasSAWSW;
   UIOAvailable[DRX_UIO2] = &extAttr->hasGPIO2;
   UIOAvailable[DRX_UIO3] = &extAttr->hasGPIO1;

   if ( UIOCfg == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   if (  ( UIOCfg->uio > DRX_UIO3 ) ||
         ( UIOCfg->uio < DRX_UIO1 ) )
   {
      return DRX_STS_INVALID_ARG;
   }

   if( *UIOAvailable[UIOCfg->uio] == FALSE )
   {
      return DRX_STS_ERROR;
   }

   UIOCfg->mode = *UIOMode[ UIOCfg->uio ];

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Write to a UIO.
* \param demod Pointer to demodulator instance.
* \param UIOData Pointer to data container for a certain UIO.
* \return DRXStatus_t.
*/
static DRXStatus_t
CtrlUIOWrite( pDRXDemodInstance_t demod,
              pDRXUIOData_t       UIOData)
{
   pI2CDeviceAddr_t devAddr     = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t      extAttr     = (pDRXKData_t)(NULL);
   u16_t            pinCfgValue = 0;
   u16_t            value       = 0;

   if (( UIOData == NULL ) || ( demod == NULL ))
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t)demod -> myExtAttr;

   /* stop lock indicator process */
   WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_DISABLE );

   switch ( UIOData->uio ) {
      /*====================================================================*/
      case DRX_UIO1:
      /* DRX_UIO1: SAW SW UIO-1 */
         if (extAttr->hasSAWSW != TRUE)
            return DRX_STS_ERROR;
         if ( ( extAttr->uioSawSwMode != DRX_UIO_MODE_READWRITE )
            && ( extAttr->uioSawSwMode != DRXK_UIO_MODE_FIRMWARE_SAW )
            && ( extAttr->uioSawSwMode
            != DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR ))
         {
            return DRX_STS_ERROR;
         }
         pinCfgValue = 0;
         /* io_pad_cfg register (8 bit reg.) MSB bit is 1 (default value) */
         pinCfgValue |= 0x0113;
         /* io_pad_cfg_mode output mode is drive always */
         /* io_pad_cfg_drive is set to power 2 (23 mA) */

         /*  Write magic word to enable pdr reg write */
         WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, SIO_TOP_COMM_KEY_KEY );

         /* write to io pad configuration register - output mode */
         WR16( demod->myI2CDevAddr, SIO_PDR_SMA_TX_CFG__A, pinCfgValue );

         /* use corresponding bit in io data output registar */
         RR16( demod->myI2CDevAddr, SIO_PDR_UIO_OUT_LO__A, &value );
         if (UIOData->value == FALSE)
         {
            value &= 0x7FFF;            /* write zero to 15th bit - 1st UIO */
         } else {
            value |= 0x8000;            /* write one to 15th bit - 1st UIO */
         }
         /* write back to io data output register */
         WR16( demod->myI2CDevAddr, SIO_PDR_UIO_OUT_LO__A, value );

         /*  Write magic word to disable pdr reg write */
         WR16 ( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, 0x0000 );
         break;
   /*======================================================================*/
      case DRX_UIO2:
      /* DRX_UIO2: GPIO2 UIO-2 */
         if (extAttr->hasGPIO2 != TRUE)
            return DRX_STS_ERROR;
         if ( extAttr->uioGPIO2Mode != DRX_UIO_MODE_READWRITE )
         {
            return DRX_STS_ERROR;
         }
         pinCfgValue = 0;
         /* io_pad_cfg register (8 bit reg.) MSB bit is 1 (default value) */
         pinCfgValue |= 0x0113;
         /* io_pad_cfg_mode output mode is drive always */
         /* io_pad_cfg_drive is set to power 2 (23 mA) */

         /*  Write magic word to enable pdr reg write */
         WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, SIO_TOP_COMM_KEY_KEY );

         /* write to io pad configuration register - output mode */
         WR16( demod->myI2CDevAddr, SIO_PDR_SMA_RX_CFG__A, pinCfgValue );

         /* use corresponding bit in io data output registar */
         RR16( demod->myI2CDevAddr, SIO_PDR_UIO_OUT_LO__A, &value );
         if (UIOData->value == FALSE)
         {
            value &= 0xBFFF;        /* write zero to 14th bit - 2nd UIO */
         } else {
            value |= 0x4000;        /* write one to 14th bit - 2nd UIO */
         }
         /* write back to io data output register */
         WR16( demod->myI2CDevAddr, SIO_PDR_UIO_OUT_LO__A, value );

         /*  Write magic word to disable pdr reg write */
         WR16 ( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, 0x0000 );
         break;
   /*====================================================================*/
      case DRX_UIO3:
      /* DRX_UIO3: GPIO1 UIO-3 */
         if (extAttr->hasGPIO1 != TRUE)
            return DRX_STS_ERROR;
         if ( extAttr->uioGPIO1Mode != DRX_UIO_MODE_READWRITE )
         {
            return DRX_STS_ERROR;
         }
         pinCfgValue = 0;
         /* io_pad_cfg register (8 bit reg.) MSB bit is 1 (default value) */
         pinCfgValue |= 0x0113;
         /* io_pad_cfg_mode output mode is drive always */
         /* io_pad_cfg_drive is set to power 2 (23 mA) */

         /*  Write magic word to enable pdr reg write */
         WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, SIO_TOP_COMM_KEY_KEY );

         /* write to io pad configuration register - output mode */
         WR16( demod->myI2CDevAddr, SIO_PDR_GPIO_CFG__A, pinCfgValue );

         /* use corresponding bit in io data output registar */
         RR16( demod->myI2CDevAddr, SIO_PDR_UIO_OUT_HI__A, &value );
         if (UIOData->value == FALSE)
         {
            value &= 0xFFFB;            /* write zero to 2nd bit - 3rd UIO */
         } else {
            value |= 0x0004;            /* write one to 2nd bit - 3rd UIO */
         }
         /* write back to io data output register */
         WR16( demod->myI2CDevAddr, SIO_PDR_UIO_OUT_HI__A, value );

         /*  Write magic word to disable pdr reg write */
         WR16 ( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, 0x0000 );
         break;
      /*=====================================================================*/
      default:
         return DRX_STS_INVALID_ARG;
   } /* switch ( UIOData->uio ) */

   if ( extAttr->uioSawSwMode ==
        DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR )
   {
      /* restart lock indicator process */
      WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_ENABLE );
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
*\brief Read from a UIO.
* \param demod Pointer to demodulator instance.
* \param UIOData Pointer to data container for a certain UIO.
* \return DRXStatus_t.
*/
static DRXStatus_t
CtrlUIORead( pDRXDemodInstance_t demod,
             pDRXUIOData_t       UIOData)
{
   pI2CDeviceAddr_t devAddr     = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t      extAttr     = (pDRXKData_t)(NULL);
   u16_t            pinCfgValue = 0;
   u16_t            value       = 0;

   if (( UIOData == NULL ) || ( demod == NULL ))
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t)demod -> myExtAttr;

   /* stop lock indicator process */
   WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_DISABLE );

   switch ( UIOData->uio ) {
      /*====================================================================*/
      case DRX_UIO1:
      /* DRX_UIO1: SAW SW UIO-1 */
         if (extAttr->hasSAWSW != TRUE)
            return DRX_STS_ERROR;

         if ( extAttr->uioSawSwMode != DRX_UIO_MODE_READWRITE )
         {
            return DRX_STS_ERROR;
         }
         pinCfgValue = 0;
         /* io_pad_cfg register (8 bit reg.) MSB bit is 1 (default value) */
         pinCfgValue |= 0x0110;
         /* io_pad_cfg_mode output mode is drive always */
         /* io_pad_cfg_drive is set to power 2 (23 mA) */

         /* write to io pad configuration register - input mode */
         WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, SIO_TOP_COMM_KEY_KEY );
         WR16( demod->myI2CDevAddr, SIO_PDR_SMA_TX_CFG__A, pinCfgValue );
         /* read io input data register */
         RR16( demod->myI2CDevAddr, SIO_PDR_UIO_IN_LO__A, &value );
         WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, 0x0000 );

         if ( (value & 0x8000) != 0 )         /* check 15th bit - 1st UIO */
         {
            UIOData->value = TRUE;
         } else {
            UIOData->value = FALSE;
         }
         break;
   /*======================================================================*/
      case DRX_UIO2:
      /* DRX_UIO2: GPIO2 UIO-2 */
         if (extAttr->hasGPIO2 != TRUE)
            return DRX_STS_ERROR;

         if ( extAttr->uioGPIO2Mode != DRX_UIO_MODE_READWRITE )
         {
            return DRX_STS_ERROR;
         }
         pinCfgValue = 0;
         /* io_pad_cfg register (8 bit reg.) MSB bit is 1 (default value) */
         pinCfgValue |= 0x0110;
         /* io_pad_cfg_mode output mode is drive always */
         /* io_pad_cfg_drive is set to power 2 (23 mA) */

         /* write to io pad configuration register - input mode */
         WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, SIO_TOP_COMM_KEY_KEY );
         WR16( demod->myI2CDevAddr, SIO_PDR_SMA_RX_CFG__A, pinCfgValue );
         /* read io input data register */
         RR16( demod->myI2CDevAddr, SIO_PDR_UIO_IN_LO__A, &value );
         WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, 0x0000 );

         if ( (value & 0x4000) != 0 )         /* check 14th bit - 2nd UIO */
         {
            UIOData->value = TRUE;
         } else {
            UIOData->value = FALSE;
         }
         break;
   /*=====================================================================*/
      case DRX_UIO3:
      /* DRX_UIO3: GPIO1 UIO-3 */
         if (extAttr->hasGPIO1 != TRUE)
            return DRX_STS_ERROR;

         if ( extAttr->uioGPIO1Mode != DRX_UIO_MODE_READWRITE )
         {
            return DRX_STS_ERROR;
         }
         pinCfgValue = 0;
         /* io_pad_cfg register (8 bit reg.) MSB bit is 1 (default value) */
         pinCfgValue |= 0x0110;
         /* io_pad_cfg_mode output mode is drive always */
         /* io_pad_cfg_drive is set to power 2 (23 mA) */

         /* write to io pad configuration register - input mode */
         WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, SIO_TOP_COMM_KEY_KEY );
         WR16( demod->myI2CDevAddr, SIO_PDR_GPIO_CFG__A, pinCfgValue );
         /* read io input data register */
         RR16( demod->myI2CDevAddr, SIO_PDR_UIO_IN_HI__A, &value );
         WR16( demod->myI2CDevAddr, SIO_TOP_COMM_KEY__A, 0x0000 );

         if ( (value & 0x0004) != 0 )            /* check 2nd bit - 3rd UIO */
         {
            UIOData->value = TRUE;
         } else {
            UIOData->value = FALSE;
         }
         break;
      /*====================================================================*/
      default:
         return DRX_STS_INVALID_ARG;
   } /* switch ( UIOData->uio ) */

   if ( extAttr->uioSawSwMode ==
        DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR )
   {
      /* restart lock indicator process */
      WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_ENABLE );
   }

   return DRX_STS_OK;
 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Report version of microcode and if possible version of device
* \param demod Pointer to demodulator instance.
* \param versionList Pointer to pointer of linked list of versions.
* \return DRXStatus_t.
*
* Using static structures so no allocation of memory is needed.
* Filling in all the fields each time, cause you don't know if they are
* changed by the application.
*
* For device:
* Major version number will be last two digits of family number.
* Minor number will be full respin number
* Patch will be metal fix number+1
* Examples:
* DRX3913K A2 => number: 13.1.2 text: "DRX3913K:A2"
* DRX3921K B1 => number: 21.2.1 text: "DRX3921K:B1"
*
*/
static DRXStatus_t
CtrlVersion( pDRXDemodInstance_t demod,
             pDRXVersionList_t   *versionList )
{
   pDRXKData_t  extAttr       = (pDRXKData_t)  (NULL);
   pI2CDeviceAddr_t devAddr   = (pI2CDeviceAddr_t) (NULL);
   pDRXCommonAttr_t  commonAttr   = (pDRXCommonAttr_t)(NULL);
   u16_t ucodeMajorMinor      = 0; /* BCD Ma:Ma:Ma:Mi */
   u16_t ucodePatch           = 0; /* BCD Pa:Pa:Pa:Pa */
   u16_t major    = 0;
   u16_t minor    = 0;
   u16_t patch    = 0;
   u16_t idx      = 0;
   u32_t jtag     = 0;
   u16_t subtype  = 0;
   u16_t mfx      = 0;
   u16_t bid      = 0;
   u16_t key      = 0;
   u32_t ROMVer   = 0;

   static char ucodeName[]   = "Microcode";
   static char deviceName[]  = "Device";

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t)demod -> myExtAttr;
   commonAttr = (pDRXCommonAttr_t)demod->myCommonAttr;

   /* stop lock indicator process */
   WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_DISABLE );

   /* Microcode version ****************************************/
   extAttr->vVersion[0].moduleType = DRX_MODULE_MICROCODE;
   extAttr->vVersion[0].moduleName = ucodeName;
   extAttr->vVersion[0].vString = extAttr->vText[0];

   if ( commonAttr->isOpened == TRUE )
   {
   SARR16( devAddr, SCU_RAM_VERSION_HI__A, &ucodeMajorMinor );
   SARR16( devAddr, SCU_RAM_VERSION_LO__A, &ucodePatch );

   /* Translate BCD to numbers and string */
   /* According to Software Version Systematics v0.2 */
   minor           =   (ucodeMajorMinor & 0xF);
   ucodeMajorMinor >>= 4;
   major           =   (ucodeMajorMinor & 0xF);
   ucodeMajorMinor >>= 4;
   major           +=  (10* (ucodeMajorMinor & 0xF));
   ucodeMajorMinor >>= 4;
   major           +=  (100* (ucodeMajorMinor & 0xF));
   patch           =   (ucodePatch & 0xF);
   ucodePatch      >>= 4;
   patch           +=  (10*(ucodePatch & 0xF));
   ucodePatch      >>= 4;
   patch           +=  (100*(ucodePatch & 0xF));
   ucodePatch      >>= 4;
   patch           +=  (1000*(ucodePatch & 0xF));
   }
   else
   {
      /* Rom version ****************************************/
      CHK_ERROR (BL_ReadRomVersion (demod, &ROMVer));

      /* Translate BCD to numbers and string */
      /* According to Software Version Systematics v0.2 */
      patch           =   (u16_t)(ROMVer & 0xF);
      ROMVer      >>= 4;
      patch           +=  (u16_t)(10*(ROMVer & 0xF));
      ROMVer      >>= 4;
      patch           +=  (u16_t)(100*(ROMVer & 0xF));
      ROMVer      >>= 4;
      patch           +=  (u16_t)(1000*(ROMVer & 0xF));
      ROMVer      >>= 4;
      minor           =   (u16_t)(ROMVer & 0xF);
      ROMVer      >>= 4;
      major           =   (u16_t)(ROMVer & 0xF);
      ROMVer      >>= 4;
      major           +=  (u16_t)(10* (ROMVer & 0xF));
      ROMVer      >>= 4;
      major           +=  (u16_t)(100* (ROMVer & 0xF));
   }

   extAttr->vVersion[0].vMajor     =  major;
   extAttr->vVersion[0].vMinor     =  minor;
   extAttr->vVersion[0].vPatch     =  patch;

   if ( major/100 != 0 )
   {
      extAttr->vVersion[0].vString[idx++] = ((char)(major/100))+'0';
      major %= 100;
   }
   if ( major/10 != 0 )
   {
      extAttr->vVersion[0].vString[idx++] = ((char)(major/10))+'0';
      major %= 10;
   }
   extAttr->vVersion[0].vString[idx++] = ((char)major)+'0';
   extAttr->vVersion[0].vString[idx++] = '.';
   extAttr->vVersion[0].vString[idx++] = ((char)minor)+'0';
   extAttr->vVersion[0].vString[idx++] = '.';
   if ( patch/1000 != 0 )
   {
      extAttr->vVersion[0].vString[idx++] = ((char)(patch/1000))+'0';
      patch %= 1000;
   }
   if ( patch/100 != 0 )
   {
      extAttr->vVersion[0].vString[idx++] = ((char)(patch/100))+'0';
      patch %= 100;
   }
   if ( patch/10 != 0 )
   {
      extAttr->vVersion[0].vString[idx++] = ((char)(patch/10))+'0';
      patch %= 10;
   }
   extAttr->vVersion[0].vString[idx++] = ((char)patch)+'0';
   extAttr->vVersion[0].vString[idx] = '\0';

   extAttr->vListElements[0].version = &(extAttr->vVersion[0]);
   extAttr->vListElements[0].next = &(extAttr->vListElements[1]);


   /* Device version ****************************************/

   extAttr->vVersion[1].moduleType = DRX_MODULE_DEVICE;
   extAttr->vVersion[1].moduleName = deviceName;
   extAttr->vVersion[1].vString = extAttr->vText[1];
   extAttr->vVersion[1].vString[0]  = 'D';
   extAttr->vVersion[1].vString[1]  = 'R';
   extAttr->vVersion[1].vString[2]  = 'X';
   extAttr->vVersion[1].vString[3]  = '3';
   extAttr->vVersion[1].vString[4]  = '9';
   extAttr->vVersion[1].vString[7]  = 'K';
   extAttr->vVersion[1].vString[8]  = ':';
   extAttr->vVersion[1].vString[11] = '\0';

   /* Check device id */
   RR16( devAddr, SIO_TOP_COMM_KEY__A   , &key);
   WR16( devAddr, SIO_TOP_COMM_KEY__A   , SIO_TOP_COMM_KEY_KEY );
   RR32( devAddr, SIO_TOP_JTAGID_LO__A  , &jtag );
   RR16( devAddr, SIO_PDR_UIO_IN_HI__A  , &bid);
   WR16( devAddr, SIO_TOP_COMM_KEY__A   , key);

   /* DRX39xxK type Ax */
   subtype = (u16_t)((jtag>>12)&0xFF);
   mfx = (u16_t)(jtag>>29);
   extAttr->vVersion[1].vMinor     = 1;
   extAttr->vVersion[1].vPatch     = mfx+1;
   extAttr->vVersion[1].vString[6] = ((char)(subtype&0xF))+'0';
   extAttr->vVersion[1].vMajor     = (subtype & 0x0F);
   subtype>>=4;
   extAttr->vVersion[1].vString[5] = ((char)(subtype&0xF))+'0';
   extAttr->vVersion[1].vMajor     += 10*subtype;
   extAttr->vVersion[1].vString[9] = 'A';
   switch (mfx)
   {
      case 0:
         extAttr->vVersion[1].vString[10] = '1';
         break;
      case 2:
         extAttr->vVersion[1].vString[10] = '2';
         break;
      case 3:
         extAttr->vVersion[1].vString[10] = '3';
         break;
      default:
         /* unknown */
         extAttr->vVersion[1].vString[10] = 'x';
         break;
   }

   extAttr->vListElements[1].version = &(extAttr->vVersion[1]);
   extAttr->vListElements[1].next = (pDRXVersionList_t)(NULL);

   *versionList = &(extAttr->vListElements[0]);

   if ( extAttr->uioSawSwMode ==
        DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR )
   {
      /* restart lock indicator process */
      WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_ENABLE );
   }

   return DRX_STS_OK;

 rw_error:
   *versionList = (pDRXVersionList_t)(NULL);
   return DRX_STS_ERROR;

}

/*============================================================================*/

/**
* \brief Set the power mode of the device to the specified power mode
* \param demod Pointer to demodulator instance.
* \param mode  Pointer to new power mode.
* \return DRXStatus_t.
* \retval DRX_STS_OK          Success
* \retval DRX_STS_ERROR       I2C error or other failure
* \retval DRX_STS_INVALID_ARG Invalid mode argument.
*
*
*/
static DRXStatus_t
CtrlPowerMode( pDRXDemodInstance_t demod,
               pDRXPowerMode_t     mode )
{
   pDRXCommonAttr_t commonAttr   = (pDRXCommonAttr_t)NULL;
   pDRXKData_t      extAttr      = (pDRXKData_t)NULL;
   pI2CDeviceAddr_t devAddr      = (pI2CDeviceAddr_t)NULL;
   u16_t            sioCcPwdMode = 0;

   commonAttr = (pDRXCommonAttr_t)demod -> myCommonAttr;
   extAttr    = (pDRXKData_t)demod->myExtAttr;
   devAddr    = demod -> myI2CDevAddr;

   /* Check arguments */
   if ( mode == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   switch ( *mode )
   {
      case DRX_POWER_UP:
         sioCcPwdMode = SIO_CC_PWD_MODE_LEVEL_NONE;
         break;
      case DRXK_POWER_DOWN_OFDM:
         sioCcPwdMode = SIO_CC_PWD_MODE_LEVEL_OFDM;
         break;
      case DRXK_POWER_DOWN_CORE:
         sioCcPwdMode = SIO_CC_PWD_MODE_LEVEL_CLOCK;
         break;
      case DRXK_POWER_DOWN_PLL:
         sioCcPwdMode = SIO_CC_PWD_MODE_LEVEL_PLL;
         break;
      case DRX_POWER_DOWN:
         sioCcPwdMode = SIO_CC_PWD_MODE_LEVEL_OSC;
         break;
      default:
         /* Unknow sleep mode */
         return DRX_STS_INVALID_ARG;
         break;
   }

   /* If already in requested power mode, do nothing */
   if ( commonAttr->currentPowerMode == *mode )
   {
      return DRX_STS_OK;
   }

   /* For next steps make sure to start from DRX_POWER_UP mode */
   if ( commonAttr->currentPowerMode != DRX_POWER_UP )
   {
      CHK_ERROR( WakeUpDevice(demod) );
      CHK_ERROR( DVBTEnableOFDMTokenRing(demod, TRUE) );
   }

   if ( *mode == DRX_POWER_UP )
   {
      /* Restore analog & pin configuartion */
   } else {
      /* Power down to requested mode */
      /* Backup some register settings */
      /* Set pins with possible pull-ups connected to them in input mode */
      /* Analog power down */
      /* ADC power down */
      /* Power down device */
      /* stop all comm_exec */
      /*
         Stop and power down previous standard
      */
      if ( !DRXK_ISPOWERUPMODE( *mode ) )
      {
         CHK_ERROR( CtrlStopStandard( demod ) );
      }

      CHK_ERROR( DVBTEnableOFDMTokenRing(demod, FALSE) );

      WR16( devAddr, SIO_CC_PWD_MODE__A, sioCcPwdMode);
      WR16( devAddr, SIO_CC_UPDATE__A  , SIO_CC_UPDATE_KEY);

      if  ( !DRXK_ISPOWERUPMODE( *mode ) )
      {
         extAttr -> HICfgCtrl |= SIO_HI_RA_RAM_PAR_5_CFG_SLEEP_ZZZ;
         HICfgCommand( demod );  /* note: ignore error here */
      }
   }

   commonAttr->currentPowerMode = *mode;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Probe device, check if it is present
* \param demod Pointer to demodulator instance.
* \return DRXStatus_t.
* \retval DRX_STS_OK    a drx39xxK device has been detected.
* \retval DRX_STS_ERROR no drx39xxK device detected.
*
* This funtion can be caled before open() and after close().
*
*/
static DRXStatus_t
CtrlProbeDevice( pDRXDemodInstance_t demod )
{
   DRXPowerMode_t    orgPowerMode = DRX_POWER_UP;
   pDRXCommonAttr_t  commonAttr   = (pDRXCommonAttr_t)(NULL);

   commonAttr = (pDRXCommonAttr_t)demod ->myCommonAttr;

   if ( commonAttr->isOpened == FALSE )
   {
      pI2CDeviceAddr_t  devAddr     = NULL;
      DRXPowerMode_t    powerMode   = DRX_POWER_UP;

      devAddr    = demod ->myI2CDevAddr;

      /* Store original power mode */
      orgPowerMode = commonAttr->currentPowerMode;

      if(demod->myCommonAttr->isOpened == FALSE)
      {
         CHK_ERROR(WakeUpDevice(demod));
      }
      else
      {
         /* Wake-up device, feedback from device */
         CHK_ERROR( CtrlPowerMode( demod, &powerMode ));
      }

      /* Initialize HI, wakeup key especially */
      CHK_ERROR(InitHI(demod) );

      /* Check device id */
      CHK_ERROR( GetDeviceCapabilities( demod ) );

      /* Device was not opened, return to orginal powermode,
         feedback from device */
      CHK_ERROR( CtrlPowerMode( demod, &orgPowerMode ));
   }
   /* else device is already opened, just return OK */

   return DRX_STS_OK;

 rw_error:
   commonAttr->currentPowerMode = orgPowerMode;
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Get modulation standard currently used to demodulate.
* \param standard Modulation standard.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
CtrlGetStandard( pDRXDemodInstance_t demod,
                 pDRXStandard_t      standard )
{
   pDRXKData_t extAttr = NULL;

   /* check arguments */
   if ( standard == NULL )
   {
      return ( DRX_STS_INVALID_ARG );
   }

   extAttr  = ( pDRXKData_t )demod->myExtAttr;
   ( *standard ) = extAttr->standard;

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Deactivate active standard.
* \param demod Demodulator instance.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
CtrlStopStandard( pDRXDemodInstance_t demod )
{
   pDRXKData_t      extAttr     = (pDRXKData_t)(NULL);

   extAttr = (pDRXKData_t)demod -> myExtAttr;

   if ( extAttr->standardActive == TRUE )
   {
      switch ( extAttr->standard )
   {
      case DRX_STANDARD_DVBT:
         CHK_ERROR( MPEGTSStop( demod ) );
            CHK_ERROR( DVBTPowerDown( demod ) );
         break;
      case DRX_STANDARD_ITU_B:
         return ( DRX_STS_ERROR );
         break;
      case DRX_STANDARD_ITU_A: /* fallthrough */
      case DRX_STANDARD_ITU_C:
         CHK_ERROR( MPEGTSStop( demod ) );
         CHK_ERROR( QAMPowerDown(demod) );
         break;
      case DRX_STANDARD_NTSC:         /* fallthrough */
      case DRX_STANDARD_FM:           /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_BG: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I:  /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L:  /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP:
         CHK_ERROR( ATVPowerDown( demod ));
         break;
      case DRX_STANDARD_UNKNOWN:
         /* Do nothing */
         break;
      default:
         return ( DRX_STS_ERROR );
   }
   }
   /* clear appropriate extAttr fields  */
   extAttr->standard                      = DRX_STANDARD_UNKNOWN;
   extAttr->standardActive                = FALSE;
   extAttr->channelActive                 = FALSE;
   extAttr->currentChannel.frequency      = 0L;
   extAttr->currentChannel.symbolrate     = 0UL;
   extAttr->currentChannel.bandwidth      = DRX_BANDWIDTH_UNKNOWN;
   extAttr->currentChannel.priority       = DRX_PRIORITY_UNKNOWN;
   extAttr->currentChannel.mirror         = DRX_MIRROR_UNKNOWN;
   extAttr->currentChannel.hierarchy      = DRX_HIERARCHY_UNKNOWN;
   extAttr->currentChannel.coderate       = DRX_CODERATE_UNKNOWN;
   extAttr->currentChannel.guard          = DRX_GUARD_UNKNOWN;
   extAttr->currentChannel.fftmode        = DRX_FFTMODE_UNKNOWN;
   extAttr->currentChannel.classification = DRX_CLASSIFICATION_UNKNOWN;
   extAttr->currentChannel.constellation  = DRX_CONSTELLATION_UNKNOWN;

   return DRX_STS_OK;

rw_error:
   /* Don't now what the standard is now  */
   extAttr->standard = DRX_STANDARD_UNKNOWN;
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Set modulation standard to be used.
* \param demod Demodulator instance.
* \param standard Modulation standard.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
CtrlSetStandard( pDRXDemodInstance_t demod, pDRXStandard_t standard )
{
   DRXStandard_t    prevStandard;
   pI2CDeviceAddr_t devAddr            = (pI2CDeviceAddr_t) NULL;
   pDRXKData_t      extAttr            = (pDRXKData_t)(NULL);

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t)demod->myExtAttr;

   /* check arguments */
   if ( standard == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   extAttr = (pDRXKData_t)demod->myExtAttr;
   prevStandard = extAttr->standard;

   /* check if standard is already active and device is powered up */
   if (  ( *standard                == prevStandard ) &&
         ( extAttr->standardActive  == TRUE ) )
   {
      /* standard already active, no need to do anything */
      return DRX_STS_OK;
   }

   /* disable HW lock indicator */
   WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_DISABLE );

   /*
      Stop and power down previous standard
   */
   CHK_ERROR( CtrlStopStandard( demod ) );

   /*
      Activate preset at standard level
   */
   /* temporarily set extAttr->standard for preset activation */
   extAttr->standard = *standard;
   DRXK_Preset( demod, DRXK_PRESET_LEVEL_STANDARD, &extAttr->presets, NULL );
   /* restore to previous */
   extAttr->standard = prevStandard;

   /*
      Power up new standard
   */
   switch ( *standard )
   {
      case DRX_STANDARD_DVBT:
         CHK_ERROR( DVBTSetStandard( demod ));
         CHK_ERROR( MPEGTSConfigureHW( demod, *standard ) );
         break;
      case DRX_STANDARD_ITU_B:
         return ( DRX_STS_ERROR );
         break;
      case DRX_STANDARD_ITU_A:        /* fallthrough */
      case DRX_STANDARD_ITU_C:
         CHK_ERROR( QAMSetStandard( demod , standard ) );
         CHK_ERROR( MPEGTSConfigureHW( demod, *standard ) );
         break;
      case DRX_STANDARD_NTSC:         /* fallthrough */
      case DRX_STANDARD_FM:           /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_BG: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I:  /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L:  /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP:
         CHK_ERROR( ATVSetStandard( demod, standard ));
         break;
      default:
         return ( DRX_STS_ERROR );
   }
   extAttr->standard = *standard;
   extAttr->standardActive = TRUE;

   /* set/restore standard specific tuner submode */
   if ( demod->myTuner != NULL )
   {
      demod->myTuner->myCommonAttr->subMode = extAttr->presets.subMode;
   }

   return DRX_STS_OK;

rw_error:
   /* Don't now what the standard is now ... try again */
   extAttr->standard=DRX_STANDARD_UNKNOWN;
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Set modulation substandard to be used.
* \param demod Demodulator instance.
* \param substandard Modulation standard.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
CtrlSetCfgSubStandard( pDRXDemodInstance_t demod,
                       pDRXSubstandard_t   substandard )
{
   pDRXKData_t      extAttr     = (pDRXKData_t)(NULL);

   extAttr = (pDRXKData_t)demod -> myExtAttr;

   /* check arguments */
   if ( substandard == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   /* change substandard */
   extAttr->substandard = *substandard;

   /* activate change in case standard is known */
   if ( extAttr->standard != DRX_STANDARD_UNKNOWN )
   {
      extAttr->standardActive = FALSE;
      return CtrlSetStandard( demod, &extAttr->standard );
   }

   /* all done, all OK */
   return DRX_STS_OK;
}


/*============================================================================*/

/**
* \brief Get modulation substandard to be used.
* \param demod Demodulator instance.
* \param substandard Modulation standard.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
CtrlGetCfgSubStandard( pDRXDemodInstance_t demod,
                       pDRXSubstandard_t   substandard )
{
   pDRXKData_t      extAttr     = (pDRXKData_t)(NULL);

   /* check arguments */
   if ( substandard == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   extAttr = (pDRXKData_t)demod -> myExtAttr;
   *substandard = extAttr->substandard;

   /* all done, all OK */
   return DRX_STS_OK;
}


/*============================================================================*/

/**
* \brief Retreive lock status .
* \param devAddr Pointer to demodulator device address.
* \param lockStat Pointer to lock status structure.
* \return DRXStatus_t.
*
*/
static DRXStatus_t
CtrlLockStatus( pDRXDemodInstance_t demod,
                pDRXLockStatus_t    lockStat )
{
   DRXStandard_t     standard  = DRX_STANDARD_UNKNOWN;




   /* check arguments */
   if ( lockStat == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   standard = ((pDRXKData_t)demod -> myExtAttr)->standard;
   *lockStat = DRX_NOT_LOCKED;

   /* define the SCU command code */
   switch ( standard ) {
      case DRX_STANDARD_DVBT:
         return DVBTLockStatus(demod, lockStat);
         break;
      case DRX_STANDARD_ITU_A:  /* fallthrough */
      case DRX_STANDARD_ITU_C:
         return QAMLockStatus( demod, lockStat );
         break;
      case DRX_STANDARD_PAL_SECAM_BG: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP: /* fallthrough */
      case DRX_STANDARD_NTSC        :
         return ATVLockStatus( demod, lockStat );
         break;
#ifndef DRXK_EXCLUDE_AUDIO
      case DRX_STANDARD_FM:
         return FmLockStatus( demod, lockStat );
         break;
#endif
      case DRX_STANDARD_UNKNOWN: /* fallthrough */
      default:
         return DRX_STS_ERROR;
   }

   /* This point is only reached in case ATV lockstatus went OK */
   return DRX_STS_OK;
}

/**
* \fn DRXStatus_t CtrlGetConstel()
* \brief Retreive constellation point via I2C.
* \param demod Pointer to demodulator instance.
* \param complexNr Pointer to the structure in which to store the
                  constellation point.
* \return DRXStatus_t.
*/
/* remove it after mc release the new register map */
#ifndef SCU_RAM_COMMAND_CMD_DEMOD_CONSTEL
#define SCU_RAM_COMMAND_CMD_DEMOD_CONSTEL 0xA
#endif

static DRXStatus_t
CtrlGetConstel(   pDRXDemodInstance_t demod,
                  pDRXComplex_t       complexNr )
{
   DRXKSCUCmd_t   cmdSCU    = {  /* command      */ 0,
                                 /* parameterLen */ 0,
                                 /* resultLen    */ 0,
                                 /* *parameter   */ NULL,
                                 /* *result      */ NULL };
   u16_t cmdResult[3] = { 0, 0, 0 };
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t)(NULL);

   /* check arguments */
   if ( complexNr == NULL )
   {
      return (DRX_STS_INVALID_ARG);
   }

   devAddr = demod->myI2CDevAddr;

   /* define the SCU command code */
   switch ( ((pDRXKData_t)demod->myExtAttr)->standard )
   {
      case DRX_STANDARD_DVBT:
         /* define the SCU command paramters and execute the command */
         cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_OFDM |
                              SCU_RAM_COMMAND_CMD_DEMOD_CONSTEL;
         cmdSCU.parameterLen = 0;
         cmdSCU.resultLen    = 3;
         cmdSCU.parameter    = NULL;
         cmdSCU.result       = cmdResult;
         CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );
         complexNr->re = cmdSCU.result[1];
         complexNr->im = cmdSCU.result[2];
         break;
      case DRX_STANDARD_ITU_A:  /* fallthrough */
      case DRX_STANDARD_ITU_C:
         /* define the SCU command paramters and execute the command */
         cmdSCU.command      = SCU_RAM_COMMAND_STANDARD_QAM |
                              SCU_RAM_COMMAND_CMD_DEMOD_CONSTEL;
         cmdSCU.parameterLen = 0;
         cmdSCU.resultLen    = 3;
         cmdSCU.parameter    = NULL;
         cmdSCU.result       = cmdResult;
         CHK_ERROR( SCUCommand( devAddr, &cmdSCU ) );
         complexNr->re = cmdSCU.result[1];
         complexNr->im = cmdSCU.result[2];
      default:
         break;
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/* =============================================================================
   ===== CtrlTPSInfo() =========================================================
   ===========================================================================*/

/**
* \fn DRXStatus_t CtrlTPSInfo()
* \brief Retreive TPS information .
* \param demod Pointer to demodulator instance.
* \param TPSInfo Pointer to TPSInfo structure.
* \return DRXStatus_t.
*
*/

static DRXStatus_t
CtrlTPSInfo( pDRXDemodInstance_t    demod,
             pDRXTPSInfo_t          TPSInfo )
{
   pI2CDeviceAddr_t devAddr = (pI2CDeviceAddr_t)(NULL);
   pDRXKData_t      extAttr = (pDRXKData_t) (NULL);
   u16_t            value   = 0;

   /* check arguments */
   if( TPSInfo == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t)demod -> myExtAttr;

   /* fft mode */
   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_TR_MODE__A, &value);
   switch( value & OFDM_EQ_TOP_TD_TPS_TR_MODE__M ) {
      case OFDM_EQ_TOP_TD_TPS_TR_MODE_2K:
         TPSInfo->fftmode = DRX_FFTMODE_2K;
         break;
      case OFDM_EQ_TOP_TD_TPS_TR_MODE_8K:
         TPSInfo->fftmode = DRX_FFTMODE_8K;
         break;
      default:
         TPSInfo->fftmode = DRX_FFTMODE_UNKNOWN;
         break;
   } /* switch ( ... ) */

   /* guard */
   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_GUARD__A, &value);
   switch( value & OFDM_EQ_TOP_TD_TPS_GUARD__M ) {
      case OFDM_EQ_TOP_TD_TPS_GUARD_32:
         TPSInfo->guard = DRX_GUARD_1DIV32;
         break;
      case OFDM_EQ_TOP_TD_TPS_GUARD_16:
         TPSInfo->guard = DRX_GUARD_1DIV16;
         break;
      case OFDM_EQ_TOP_TD_TPS_GUARD_08:
         TPSInfo->guard = DRX_GUARD_1DIV8;
         break;
      case OFDM_EQ_TOP_TD_TPS_GUARD_04:
         TPSInfo->guard = DRX_GUARD_1DIV4;
         break;
      default:
         TPSInfo->guard = DRX_GUARD_UNKNOWN;
         break;
   } /* switch ( ... ) */

   /* constellation */
   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_CONST__A, &value);
   switch( value & OFDM_EQ_TOP_TD_TPS_CONST__M ) {
      case OFDM_EQ_TOP_TD_TPS_CONST_QPSK:
         TPSInfo->constellation = DRX_CONSTELLATION_QPSK;
         break;
      case OFDM_EQ_TOP_TD_TPS_CONST_16QAM:
         TPSInfo->constellation = DRX_CONSTELLATION_QAM16;
         break;
      case OFDM_EQ_TOP_TD_TPS_CONST_64QAM:
         TPSInfo->constellation = DRX_CONSTELLATION_QAM64;
         break;
      default:
         TPSInfo->constellation = DRX_CONSTELLATION_UNKNOWN;
         break;
   } /* switch ( ... ) */

   /* hierarchy */
   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_HINFO__A, &value);
   switch( value & OFDM_EQ_TOP_TD_TPS_HINFO__M ) {
      case OFDM_EQ_TOP_TD_TPS_HINFO_NH:
         TPSInfo->hierarchy = DRX_HIERARCHY_NONE;
         break;
      case OFDM_EQ_TOP_TD_TPS_HINFO_H1:
         TPSInfo->hierarchy = DRX_HIERARCHY_ALPHA1;
         break;
      case OFDM_EQ_TOP_TD_TPS_HINFO_H2:
         TPSInfo->hierarchy = DRX_HIERARCHY_ALPHA2;
         break;
      case OFDM_EQ_TOP_TD_TPS_HINFO_H4:
         TPSInfo->hierarchy = DRX_HIERARCHY_ALPHA4;
         break;
      default:
         TPSInfo->hierarchy = DRX_HIERARCHY_UNKNOWN;
         break;
   } /* switch ( ... ) */

   /* high priority coderate */
   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_CODE_HP__A, &value);
   switch( value & OFDM_EQ_TOP_TD_TPS_CODE_HP__M ) {
      case OFDM_EQ_TOP_TD_TPS_CODE_HP_1_2:
         TPSInfo->highCoderate = DRX_CODERATE_1DIV2;
         break;
      case OFDM_EQ_TOP_TD_TPS_CODE_HP_2_3:
         TPSInfo->highCoderate = DRX_CODERATE_2DIV3;
         break;
      case OFDM_EQ_TOP_TD_TPS_CODE_HP_3_4:
         TPSInfo->highCoderate = DRX_CODERATE_3DIV4;
         break;
      case OFDM_EQ_TOP_TD_TPS_CODE_HP_5_6:
         TPSInfo->highCoderate = DRX_CODERATE_5DIV6;
         break;
      case OFDM_EQ_TOP_TD_TPS_CODE_HP_7_8:
         TPSInfo->highCoderate = DRX_CODERATE_7DIV8;
         break;
      default:
         TPSInfo->highCoderate = DRX_CODERATE_UNKNOWN;
         break;
   } /* switch ( ... ) */

   /* low priority coderate */
   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_CODE_LP__A, &value);
   switch( value & OFDM_EQ_TOP_TD_TPS_CODE_LP__M ) {
      case OFDM_EQ_TOP_TD_TPS_CODE_LP_1_2:
         TPSInfo->lowCoderate = DRX_CODERATE_1DIV2;
         break;
      case OFDM_EQ_TOP_TD_TPS_CODE_LP_2_3:
         TPSInfo->lowCoderate = DRX_CODERATE_2DIV3;
         break;
      case OFDM_EQ_TOP_TD_TPS_CODE_LP_3_4:
         TPSInfo->lowCoderate = DRX_CODERATE_3DIV4;
         break;
      case OFDM_EQ_TOP_TD_TPS_CODE_LP_5_6:
         TPSInfo->lowCoderate = DRX_CODERATE_5DIV6;
         break;
      case OFDM_EQ_TOP_TD_TPS_CODE_LP_7_8:
         TPSInfo->lowCoderate = DRX_CODERATE_7DIV8;
         break;
      default:
         TPSInfo->lowCoderate = DRX_CODERATE_UNKNOWN;
         break;
   } /* switch ( ... ) */

   /* frame */
   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_FRM_NMB__A, &value);
   switch( value & OFDM_EQ_TOP_TD_TPS_FRM_NMB__M ) {
      case OFDM_EQ_TOP_TD_TPS_FRM_NMB_1:
         TPSInfo->frame = DRX_TPS_FRAME1;
         break;
      case OFDM_EQ_TOP_TD_TPS_FRM_NMB_2:
         TPSInfo->frame = DRX_TPS_FRAME2;
         break;
      case OFDM_EQ_TOP_TD_TPS_FRM_NMB_3:
         TPSInfo->frame = DRX_TPS_FRAME3;
         break;
      case OFDM_EQ_TOP_TD_TPS_FRM_NMB_4:
         TPSInfo->frame = DRX_TPS_FRAME4;
         break;
      default:
         TPSInfo->frame = DRX_TPS_FRAME_UNKNOWN;
         break;
   } /* switch ( ... ) */

   /* length */
   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_LEN__A, &value);
   TPSInfo->length = (u8_t) (value & OFDM_EQ_TOP_TD_TPS_LEN__M);

   /* cell identifiers */
   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_CELL_ID_LO__A, &(TPSInfo->cellId));
   TPSInfo->cellId &= OFDM_EQ_TOP_TD_TPS_CELL_ID_LO__M;
   RR16( devAddr, OFDM_EQ_TOP_TD_TPS_CELL_ID_HI__A, &value);
   value &= OFDM_EQ_TOP_TD_TPS_CELL_ID_HI__M;
   TPSInfo->cellId |= (value << 8);

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}
/*============================================================================*/

/**
* \brief    Activate channel specific presets
* \param    Pointer to demodulator instance.
* \return   DRXStatus_t
* \note     Assumption is made SCU could be running when calling this function
*/
static DRXStatus_t ActivateChannelPresets(pDRXDemodInstance_t demod)
{
   pDRXKData_t      extAttr = NULL;
   pI2CDeviceAddr_t devAddr = NULL;

   devAddr = demod->myI2CDevAddr;
   extAttr = (pDRXKData_t) demod->myExtAttr;

   /* activate the AGC with presets, assume SCU could be running: atomic */
   CHK_ERROR ( AGCCtrlSetCfgPreSaw ( demod, &extAttr->presets.preSawCfg ) );
   CHK_ERROR ( AGCSetIf         ( demod, &extAttr->presets.ifAgcCfg, TRUE) );
   CHK_ERROR ( AGCSetRf         ( demod, &extAttr->presets.rfAgcCfg, TRUE) );

   if ( DRXK_ISATVSTD( extAttr->standard ) )
   {
      /* Vid. peak filter */
      WR16( devAddr, ATV_TOP_VID_PEAK__A, extAttr->presets.peakFilter );
      /* equalizer coefficients */
      WR16( devAddr, ATV_TOP_EQU0__A,     extAttr->presets.atvTopEqu[0] );
      WR16( devAddr, ATV_TOP_EQU1__A,     extAttr->presets.atvTopEqu[1] );
      WR16( devAddr, ATV_TOP_EQU2__A,     extAttr->presets.atvTopEqu[2] );
      WR16( devAddr, ATV_TOP_EQU3__A,     extAttr->presets.atvTopEqu[3] );
   }

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Select a new receiver channel.
* \param demod instance of demod.
* \param channel Pointer to channel data.
* \return DRXStatus_t.
*
* In case the tuner module is not used and in case of NTSC/FM the pogrammer
* must tune the tuner to the center frequency of the NTSC/FM channel.
*
*/
static DRXStatus_t
CtrlSetChannel( pDRXDemodInstance_t   demod,
                pDRXChannel_t         channel )
{

   pDRXKData_t             extAttr           = NULL;
   pI2CDeviceAddr_t        devAddr           = NULL;
   pDRXCommonAttr_t        commonAttr        = NULL;
   DRXStandard_t           standard          = DRX_STANDARD_UNKNOWN;
   Bool_t                  rfAgcIsAuto       = FALSE;
   DRXFrequency_t          tunerFreqOffset   = 0;
   DRXChannel_t            tunerChannel      = {DRX_UNKNOWN};
   DRXStatus_t             paramStatus       = DRX_STS_OK;

   /*== Check arguments ======================================================*/
   if ( channel == NULL )
   {
      return DRX_STS_FUNC_NOT_AVAILABLE;
   }

   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;
   devAddr    = demod -> myI2CDevAddr;
   extAttr    = (pDRXKData_t)demod -> myExtAttr;
   standard   = extAttr->standard;


   /* Channel dependent presets */
   if (  (extAttr->presets.hasChannelPresets == TRUE) && (channel != NULL))
   {
      DRXK_Preset( demod, DRXK_PRESET_LEVEL_CHANNEL,
                     &extAttr->presets, channel );

      /* activate the presets */
      CHK_ERROR (ActivateChannelPresets(demod));
   }

   /* Check channel parameter constraints per standard */
   if ( standard == DRX_STANDARD_DVBT )
   {
      paramStatus = DVBTSetChParamCheck( demod, channel );
      if ( paramStatus != DRX_STS_OK )
      {
         return paramStatus;
      }
      CHK_ERROR( MPEGTSStop( demod ) );
   }
   else if ( DRXK_ISQAMSTD( standard ) )
   {
      paramStatus = QAMSetChParamCheck( demod, channel );
      if ( paramStatus != DRX_STS_OK )
      {
         return paramStatus;
      }
      CHK_ERROR( MPEGTSStop( demod ) );
   }
   else if ( DRXK_ISATVSTD( standard ) )
   {
      paramStatus = ATVSetChParamCheck( demod, channel ) ;
      if ( paramStatus != DRX_STS_OK )
      {
         return paramStatus;
      }
   }
   else
   {
      /* Unsupported current standard */
      return DRX_STS_INVALID_ARG;
   }

   /* disable HW lock indicator */
   WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_DISABLE );

   /* SAW SW, (UIO 1) */
   if ( ( extAttr->hasSAWSW == TRUE ) &&
        ( extAttr->uioSawSwMode == DRXK_UIO_MODE_FIRMWARE_SAW ) )
   {
      DRXUIOData_t uio1 = { DRX_UIO1, FALSE };

      switch ( channel->bandwidth )
      {
         case DRX_BANDWIDTH_8MHZ:
            uio1.value = TRUE;
            break;
         case DRX_BANDWIDTH_7MHZ:
            uio1.value = FALSE;
            break;
         case DRX_BANDWIDTH_6MHZ:
            uio1.value = FALSE;
            break;
         case DRX_BANDWIDTH_UNKNOWN:
         default:
            return DRX_STS_INVALID_ARG;
      }

      CHK_ERROR( CtrlUIOWrite( demod, &uio1 ) );
   }
   if( ( extAttr->hasSAWSW == TRUE ) &&
       ( extAttr->uioSawSwMode == DRXK_UIO_MODE_FIRMWARE_LOCK_INDICATOR ) )
   {
      DRXUIOData_t uio1 = { DRX_UIO1, FALSE };

      /* set pin to output, value low */
      CHK_ERROR( CtrlUIOWrite( demod, &uio1 ) );

      /* enable HW lock indicator */
      WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_ENABLE );
   }

   /* Programming RF AGC switch ( UIO2 ) */
   if ( ( extAttr->hasGPIO2 == TRUE ) &&
        ( extAttr->uioGPIO2Mode == DRX_UIO_MODE_FIRMWARE ) )
   {
      DRXUIOData_t uio2Data = { DRX_UIO2, FALSE };
      DRXStatus_t  sts      = DRX_STS_ERROR;

      uio2Data.value = extAttr->presets.rfAgcSwitch;

      /* dirty trick for code reuse ... */
      extAttr->uioGPIO2Mode = DRX_UIO_MODE_READWRITE;
      sts = CtrlUIOWrite( demod,  &uio2Data );
      extAttr->uioGPIO2Mode = DRX_UIO_MODE_FIRMWARE;

      if ( sts != DRX_STS_OK)
      {
         return (sts);
      }
   }

   if ( demod->myTuner != NULL )
   {
      tunerChannel = *channel;
      commonAttr->tunerSlowMode = FALSE;
      CHK_ERROR ( DRX_Ctrl ( demod, DRX_CTRL_PROGRAM_TUNER, &tunerChannel ));

      /* calculate the offset introduced by the tuner */
      tunerFreqOffset = tunerChannel.frequency - extAttr->tunerSetFreq;
   }
   else
   {
      tunerFreqOffset = 0;
   }

   /*== Setup demod for specific standard ====================================*/

   switch ( standard ) {
      case DRX_STANDARD_DVBT:
            CHK_ERROR ( DVBTSetChannel( demod,
                                        channel,
                                        tunerFreqOffset ) );
            break;
      case DRX_STANDARD_ITU_A:
      case DRX_STANDARD_ITU_C:
            CHK_ERROR ( QAMSetChannel( demod,
                                       channel,
                                       tunerFreqOffset ) );
            break;
      case DRX_STANDARD_PAL_SECAM_BG: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP: /* fallthrough */
      case DRX_STANDARD_NTSC        : /* fallthrough */
      case DRX_STANDARD_FM          :
            CHK_ERROR ( ATVSetChannel ( demod,
                                        tunerFreqOffset,
                                        channel,
                                        standard) );
            break;
      case DRX_STANDARD_UNKNOWN: /* fallthrough */
      default:
         return DRX_STS_INVALID_ARG;
   } /* switch ( standard ) */

   rfAgcIsAuto = (Bool_t) ( extAttr->presets.rfAgcCfg.ctrlMode ==
                              DRXK_AGC_CTRL_AUTO );

   if ( ( demod->myTuner != NULL ) &&
            rfAgcIsAuto == FALSE )
   {
      tunerChannel = *channel;
      /* tune again, but in slow mode */
      commonAttr->tunerSlowMode = TRUE;
      CHK_ERROR ( DRX_Ctrl ( demod, DRX_CTRL_PROGRAM_TUNER, &tunerChannel ));
   }


   /*== For DVBT, start demodulating here
         (potentially glitches on signal might reduce locking speed) =========*/
   if( standard == DRX_STANDARD_DVBT)
   {
      CHK_ERROR ( DVBTStart( demod ) );
   }

   if ( DRXK_ISATVSTD ( standard ) )
   {
      /* reduce AGC history in ATV to avoid AGC sticking */
      CHK_ERROR ( AGCReset ( demod, TRUE ));
   }

   if( extAttr->packetErrMode == DRXK_PACKET_ERR_ACCUMULATIVE)
   {
      if(   ( standard == DRX_STANDARD_DVBT )   ||
            ( standard == DRX_STANDARD_ITU_A )  ||
            ( standard == DRX_STANDARD_ITU_C )
         )
      {
         /* reset the counter */
         CHK_ERROR (DTVGetAccPktErr( demod, NULL ));
      }
   }


   /* All done, all OK */
   extAttr->channelActive  = TRUE;
   extAttr->currentChannel = *channel;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Retreive parameters of current channel.
* \param demod   Pointer to demod instance.
* \param channel Pointer to channel data.
* \return DRXStatus_t.
*/
static DRXStatus_t
CtrlGetChannel( pDRXDemodInstance_t   demod,
                pDRXChannel_t         channel )
{
   pI2CDeviceAddr_t devAddr    = NULL;
   pDRXKData_t      extAttr    = NULL;
   pDRXCommonAttr_t commonAttr =(pDRXCommonAttr_t)(NULL);
   DRXLockStatus_t  lockStatus = DRX_NOT_LOCKED;
   DRXStandard_t    standard   = DRX_STANDARD_UNKNOWN;

   /* check arguments */
   if ( channel == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr    = demod -> myI2CDevAddr;
   extAttr    = (pDRXKData_t)demod -> myExtAttr;
   standard   = extAttr->standard;
   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;

   /* initialize channel fields */
   channel->frequency         = extAttr->currentChannel.frequency;
   channel->bandwidth         = extAttr->currentChannel.bandwidth;
   channel->priority          = extAttr->currentChannel.priority;
   channel->symbolrate        = extAttr->currentChannel.symbolrate;
   channel->mirror            = DRX_MIRROR_UNKNOWN;
   channel->hierarchy         = DRX_HIERARCHY_UNKNOWN;
   channel->coderate          = DRX_CODERATE_UNKNOWN;
   channel->guard             = DRX_GUARD_UNKNOWN;
   channel->fftmode           = DRX_FFTMODE_UNKNOWN;
   channel->classification    = DRX_CLASSIFICATION_UNKNOWN;
   channel->constellation     = DRX_CONSTELLATION_UNKNOWN;
   channel->interleavemode    = DRX_INTERLEAVEMODE_I12_J17;

   /* Get tuner frequency */
   if ( demod->myTuner != NULL )
   {
      DRXFrequency_t dummy = 0 ;
      DRXFrequency_t tunerFreqOffset = 0 ;
      Bool_t         tunerMirror = FALSE;

      tunerMirror = (commonAttr->mirrorFreqSpect)?FALSE:TRUE;

      /* Get frequency from tuner */
      CHK_ERROR( DRXBSP_TUNER_GetFrequency(  demod->myTuner,
                                             0,
                                             &(channel->frequency),
                                             &dummy ) );

      tunerFreqOffset = channel->frequency - extAttr->currentChannel.frequency;
      if ( tunerMirror == TRUE )
      {
         /* negative image */
         channel->frequency -= tunerFreqOffset;
      } else {
         /* positive image */
         channel->frequency += tunerFreqOffset;
      }
   }

   /* Check lock status and get the rest of channel parameters */
   CHK_ERROR( CtrlLockStatus( demod, &lockStatus) );
   if ( DRXK_ISATVSTD ( standard ) )
   {
      if ( lockStatus == DRX_LOCKED )
      {
         return ATVGetChannel( demod, channel, standard );
      }
   }
   else if ( DRXK_ISQAMSTD ( standard ) )
   {
      if ( lockStatus == DRX_LOCKED )
      {
         return QAMGetChannel( demod, channel );
      }
   }
   else if ( DRXK_ISDVBTSTD ( standard ) )
   {
      if ( ( lockStatus != DRX_NOT_LOCKED ) &&
           ( lockStatus != DRX_NEVER_LOCK ) )
      {
         return DVBTGetChannel( demod, channel );
      }
   }
   else /* invalid standard */
   {
      return DRX_STS_ERROR;
   }

   /* standard OK, but no channel info available */
   return DRX_STS_OK;

rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Retreive signal quality form device.
* \param demod Pointer to demodulator instance.
* \param sigQuality Pointer to signal quality data.
* \return DRXStatus_t.
* \retval DRX_STS_OK sigQuality contains valid data.
* \retval DRX_STS_INVALID_ARG sigQuality is NULL.
* \retval DRX_STS_ERROR Erroneous data, sigQuality contains invalid data.

*/
static DRXStatus_t
CtrlSigQuality( pDRXDemodInstance_t demod,
                pDRXSigQuality_t    sigQuality )
{
   DRXStandard_t      standard   = DRX_STANDARD_UNKNOWN;

   if ( sigQuality == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   standard = ((pDRXKData_t)demod->myExtAttr)->standard;

   switch ( standard ) {
      case DRX_STANDARD_DVBT:
         return DVBTSigQuality ( demod, sigQuality );
         break;
      case DRX_STANDARD_ITU_A: /* fallthrough */
      case DRX_STANDARD_ITU_C:
         return QAMSigQuality ( demod, sigQuality );
         break;
      case DRX_STANDARD_PAL_SECAM_BG: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK: /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I:  /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L:  /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP: /* fallthrough */
      case DRX_STANDARD_NTSC:
         return ATVSigQuality ( demod, sigQuality );
         break;
#ifndef DRXK_EXCLUDE_AUDIO
      case DRX_STANDARD_FM:
         return FmSigQuality ( demod, sigQuality );
         break;
#endif
      default:
         /* Standard not supported */
         return DRX_STS_ERROR;
   }

   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Retrieve signal strength.
* \param demod Pointer to demodulator instance.
* \param sigStrength Pointer to signal strength data; range 0, .. , 100.
* \return DRXStatus_t.
* \retval DRX_STS_OK sigStrength contains valid data.
* \retval DRX_STS_INVALID_ARG sigStrength is NULL.
* \retval DRX_STS_ERROR Erroneous data, sigStrength contains invalid data.

*/
static DRXStatus_t
CtrlSigStrength( pDRXDemodInstance_t    demod,
                 pu16_t                 sigStrength )
{
   pDRXKData_t    extAttr  = (pDRXKData_t)    NULL;
   DRXStandard_t  standard = DRX_STANDARD_UNKNOWN;
   DRXKCfgAgc_t   rfAgc;
   DRXKCfgAgc_t   ifAgc;
   u32_t          totalGain  = 0;
   u32_t          atten      = 0;
   u32_t          agcRange   = 0;

   extAttr = (pDRXKData_t) demod->myExtAttr;


   /* Check arguments */
   if ( sigStrength == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   standard = ((pDRXKData_t) demod->myExtAttr)->standard;
   *sigStrength = 0;

   /* Signal strength indication */
   switch ( standard )
   {
      case DRX_STANDARD_DVBT  :        /* fallthrough */
      case DRX_STANDARD_ITU_A :        /* fallthrough */
      case DRX_STANDARD_ITU_C :        /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_BG : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_DK : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_I  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_L  : /* fallthrough */
      case DRX_STANDARD_PAL_SECAM_LP : /* fallthrough */
      case DRX_STANDARD_NTSC:          /* fallthrough */
      case DRX_STANDARD_FM:
         break;
      case DRX_STANDARD_UNKNOWN: /* fallthrough */
      default:
         /* No active standard; just return 0 */
         return DRX_STS_OK;
   }

   /*  get AGC data */
   rfAgc.standard = standard;
   ifAgc.standard = standard;
   CHK_ERROR( AGCGetRf ( demod, &rfAgc ) );
   CHK_ERROR( AGCGetIf ( demod, &ifAgc ) );

   if ( rfAgc.ctrlMode == DRXK_AGC_CTRL_AUTO )
   {
      /* Take RF gain into account */
      totalGain += (u32_t) extAttr->presets.tunerRfGain;

      /* clip output value */
      if ( rfAgc.outputLevel < rfAgc.minOutputLevel )
      {
         rfAgc.outputLevel = rfAgc.minOutputLevel;
      }
      if ( rfAgc.outputLevel > rfAgc.maxOutputLevel )
      {
         rfAgc.outputLevel = rfAgc.maxOutputLevel;
      }

      agcRange  = (u32_t) (rfAgc.maxOutputLevel - rfAgc.minOutputLevel);
      if ( agcRange > 0 )
      {
         atten +=
               100UL *
               ( (u32_t) (extAttr->presets.tunerRfGain) ) *
               ( (u32_t) (rfAgc.outputLevel - rfAgc.minOutputLevel) ) /
               agcRange;
      }
   }

   if ( ifAgc.ctrlMode == DRXK_AGC_CTRL_AUTO )
   {
      /* Take IF gain into account */
      totalGain += (u32_t) extAttr->presets.tunerIfGain;

      /* clip output value */
      if ( ifAgc.outputLevel < ifAgc.minOutputLevel )
      {
         ifAgc.outputLevel = ifAgc.minOutputLevel;
      }
      if ( ifAgc.outputLevel > ifAgc.maxOutputLevel )
      {
         ifAgc.outputLevel = ifAgc.maxOutputLevel;
      }

      agcRange  = (u32_t) (ifAgc.maxOutputLevel - ifAgc.minOutputLevel);
      if ( agcRange > 0 )
      {
         atten +=
               100UL *
               ( (u32_t) (extAttr->presets.tunerIfGain) ) *
               ( (u32_t) (ifAgc.outputLevel - ifAgc.minOutputLevel) ) /
               agcRange;
      }
   }

   /* Convert to 0..100 scale */
   if ( totalGain > 0 )
   {
      *sigStrength = (u16_t) (atten / totalGain);
   }
   if ( *sigStrength > 100 )
   {
      *sigStrength = 100;
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Set MPEG output configuration of the device.
* \param demod  Pointer to demodulator instance.
* \param cfgData Pointer to mpeg output configuaration.
* \return DRXStatus_t.
* \retval DRX_STS_OK MPEG TS ouput is configured.
* \retval DRX_STS_ERROR I2C error; current constellation or standard not
*                       supported
* \retval DRX_STS_INVALID_ARG cfgData == NULL.
*
*  Configure MPEG output parameters.
*
*/
static DRXStatus_t
MPEGCtrlSetCfgOutput(   pDRXDemodInstance_t   demod,
                      pDRXCfgMPEGOutput_t   cfgData )
{
   pDRXKData_t extAttr =(pDRXKData_t) (NULL);
   DRXStatus_t sts     = DRX_STS_OK;

   /* check arguments */
   if ( cfgData == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   extAttr = (pDRXKData_t) demod->myExtAttr;

   /* store wanted MPEG config in demod data structure */
   MPEGTSCopyCfg( &(demod->myCommonAttr->mpegCfg), cfgData );

   switch ( extAttr->standard )
   {
      case DRX_STANDARD_DVBT:    /* fallthrough */
      case DRX_STANDARD_ITU_A:   /* fallthrough */
      case DRX_STANDARD_ITU_B:   /* fallthrough */
      case DRX_STANDARD_ITU_C:
         /* digital standards */
         sts = MPEGTSConfigureHW( demod, (extAttr->standard) );
         break;
      default:
         /* non-digital standard: do nothing */
         break;
   } /* switch ( extAttr->standard ) */

   return (sts);
}

/*============================================================================*/

/**
* \brief Get MPEG output configuration of the device.
* \param demod  Pointer to demodulator instance.
* \param cfgData Pointer to MPEG output configuaration struct.
* \return DRXStatus_t.
*
*  Retrieve MPEG output configuartion.
*
*/
static DRXStatus_t
CtrlGetCfgMPEGOutput( pDRXDemodInstance_t   demod,
                      pDRXCfgMPEGOutput_t   cfgData )
{
   pDRXKData_t      extAttr    = (pDRXKData_t)(     NULL);
   pI2CDeviceAddr_t devAddr    = (pI2CDeviceAddr_t) (NULL);
   pDRXCommonAttr_t commonAttr = (pDRXCommonAttr_t) (NULL);

   /* check arguments */
   if ( cfgData == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   devAddr    = (pI2CDeviceAddr_t) demod -> myI2CDevAddr;
   extAttr    = (pDRXKData_t)      demod -> myExtAttr;
   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;

   MPEGTSCopyCfg( cfgData, &(commonAttr->mpegCfg) );

   if ( commonAttr->mpegCfg.staticCLK == TRUE )   /* Static mode */
   {
      u16_t fecOcDtoPeriod = 0;

      RR16 ( devAddr, FEC_OC_DTO_PERIOD__A, &fecOcDtoPeriod );
      cfgData->bitrate = ( commonAttr->sysClockFreq * 1000 ) /
                         ( fecOcDtoPeriod + 2 );
   }
   else
   {
      u32_t dynRate = 0;
      u32_t dataHi  = 0;
      u32_t dataLo  = 0;

      ARR32 ( devAddr, FEC_OC_RCN_DYN_RATE_LO__A, &dynRate);
      /*
         bitrate = 2^-25 * dynRate * Fsys
      */
      Mult32 ( dynRate,
               (commonAttr->sysClockFreq * 1000),
               &dataHi,
               &dataLo );
      cfgData->bitrate = (dataHi << 7) | (dataLo >> 25);
   }

   return DRX_STS_OK;

 rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Get symbol clock offset in ppm.
* \param demod  Pointer to demodulator instance.
* \param  Pointer to deviation in ppm.
* \return DRXStatus_t.
*
* Deviation between transmitter clock and cristal/osc on board.
*
*/
static DRXStatus_t
DTVCtrlGetCfgSymbolClockOffset(  pDRXDemodInstance_t demod,
                             ps32_t              ppm )
{
   pDRXKData_t extAttr = (pDRXKData_t) (NULL);

   /* check arguments */
   if ( ppm == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   extAttr = (pDRXKData_t)  demod -> myExtAttr;

   switch ( extAttr->standard ) {
      case DRX_STANDARD_ITU_A: /* fallthrough */
      case DRX_STANDARD_ITU_C:
         return QAMGetSymbolrateOffset( demod, ppm );
         break;
      case DRX_STANDARD_DVBT:
         return DVBTGetSymbolrateOffset( demod, ppm );
         break;
      default:
         return (DRX_STS_FUNC_NOT_AVAILABLE);
   }

   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Get packet error mode
* \param demod  Pointer to demodulator instance.
* \param  Pointer to DRXKCfgPacketErr_t
* \return DRXStatus_t.
*
* Retrieve current packet error mode.
*
*/
static DRXStatus_t
DTVCtrlGetCfgPacketErrMode ( pDRXDemodInstance_t demod,
                           pDRXKCfgPacketErrMode_t mode )
{
   pDRXKData_t extAttr = (pDRXKData_t) (NULL);
   extAttr = (pDRXKData_t) demod -> myExtAttr;

   /* check arguments */
   if ( mode == NULL)
   {
      return DRX_STS_INVALID_ARG;
   }

   *mode = extAttr->packetErrMode;

   return DRX_STS_OK;
}
/*============================================================================*/

/**
* \brief Set packet error mode
* \param demod  Pointer to demodulator instance.
* \param  Pointer to DRXKCfgPacketErr_t
* \return DRXStatus_t.
*
* Set packet error mode and rest the counter
*
*/
static DRXStatus_t
DTVCtrlSetCfgPacketErrMode ( pDRXDemodInstance_t demod,
                           pDRXKCfgPacketErrMode_t mode )
{
   pDRXKData_t extAttr = (pDRXKData_t) (NULL);
   extAttr = (pDRXKData_t) demod -> myExtAttr;

   /* check arguments */
   if ( mode == NULL)
   {
      return DRX_STS_INVALID_ARG;
   }

   /* call DTVGetAccPktErr with NULL to reset counter */
   CHK_ERROR( DTVGetAccPktErr(demod, NULL) );

   extAttr->packetErrMode = *mode;

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/

/**
* \brief Set 'some' configuration of the device.
* \param demod Pointer to demodulator instance.
* \param config Pointer to configuration parameters (type and data).
* \return DRXStatus_t.

*/
static DRXStatus_t
CtrlSetCfg( pDRXDemodInstance_t   demod,
            pDRXCfg_t             config )
{
   /* check arguments */
   if ( config == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   switch ( config->cfgType )
   {
      case DRX_CFG_MPEG_OUTPUT:
         return MPEGCtrlSetCfgOutput(  demod,
                                      (pDRXCfgMPEGOutput_t) config->cfgData );
         break;
      case DRX_CFG_PINS_SAFE_MODE:
         return CtrlSetCfgPdrSafeMode( demod,
                                       (pBool_t) config->cfgData );
         break;
      case DRXK_CFG_AGC_RF:
         return AGCCtrlSetCfgRf (     demod,
                                      (pDRXKCfgAgc_t) config->cfgData );
      case DRXK_CFG_AGC_IF:
         return AGCCtrlSetCfgIf (     demod,
                                      (pDRXKCfgAgc_t) config->cfgData );
      case DRXK_CFG_PRE_SAW:
         return AGCCtrlSetCfgPreSaw(   demod,
                                      (pDRXKCfgPreSaw_t) config->cfgData );
      case DRXK_CFG_ATV_MISC:
         return ATVCtrlSetCfgMisc(    demod,
                                      (pDRXKCfgAtvMisc_t) config->cfgData );
      case DRXK_CFG_ATV_EQU_COEF:
         return ATVCtrlSetCfgEquCoef( demod,
                                      (pDRXKCfgAtvEquCoef_t) config->cfgData );
      case DRXK_CFG_ATV_OUTPUT:
         return ATVCtrlSetCfgOutput(  demod,
                                      (pDRXKCfgAtvOutput_t) config->cfgData );
      case DRX_CFG_SUBSTANDARD:
         return CtrlSetCfgSubStandard( demod,
                                       (pDRXSubstandard_t) config->cfgData );
      case DRXK_CFG_PACKET_ERR_MODE:
         return DTVCtrlSetCfgPacketErrMode(    demod,
                                                   (pDRXKCfgPacketErrMode_t)
                                                config->cfgData );
      case DRXK_CFG_DVBT_SQI_SPEED:
         return DVBTCtrlSetCfgSqiSpeed(   demod,
                                       (pDRXKCfgDvbtSqiSpeed_t)
                                       config->cfgData);
      case DRXK_CFG_RESET_BER:
         return DTVCtrlSetCfgResetBer(          demod );

#ifndef DRXK_EXCLUDE_AUDIO
      case DRX_CFG_AUD_VOLUME:
         return AUDCtrlSetCfgVolume(      demod,
                                          (pDRXCfgAudVolume_t)config->cfgData );
      case DRX_CFG_I2S_OUTPUT:
         return AUDCtrlSetCfgOutputI2S(   demod,
                                          (pDRXCfgI2SOutput_t)config->cfgData );
      case DRX_CFG_AUD_AUTOSOUND:
         return AUDCtrSetlCfgAutoSound(   demod,
                                          (pDRXCfgAudAutoSound_t)
                                             config->cfgData);
      case DRX_CFG_AUD_ASS_THRES:
         return AUDCtrlSetCfgASSThres(    demod,
                                          (pDRXCfgAudASSThres_t)
                                             config->cfgData);
      case DRX_CFG_AUD_CARRIER:
         return AUDCtrlSetCfgCarrier(  demod,
                                       (pDRXCfgAudCarriers_t)config->cfgData);
      case DRX_CFG_AUD_DEVIATION:
         return AUDCtrlSetCfgDev(      demod,
                                       (pDRXCfgAudDeviation_t)config->cfgData);
      case DRX_CFG_AUD_PRESCALE:
         return AUDCtrlSetCfgPrescale( demod,
                                       (pDRXCfgAudPrescale_t)config->cfgData);
      case DRX_CFG_AUD_MIXER:
         return AUDCtrlSetCfgMixer(    demod,
                                       (pDRXCfgAudMixer_t)config->cfgData);
      case DRX_CFG_AUD_AVSYNC:
         return AUDCtrlSetCfgAVSync(   demod,
                                       (pDRXCfgAudAVSync_t)config->cfgData);

#endif
      /* ACCES (XS) functions from here */
      case DRX_XS_CFG_PRESET:
         ((pDRXKData_t)demod->myExtAttr)->presetName =
               *((char **)config->cfgData);
         return DRX_STS_OK;
      case DRX_XS_CFG_AUD_BTSC_DETECT:
         ((pDRXKData_t)demod->myExtAttr)->audData.btscDetect =
               *((pDRXAudBtscDetect_t)config->cfgData);
         return DRX_STS_OK;
      case DRX_XS_CFG_QAM_LOCKRANGE:
         ((pDRXKData_t)demod->myExtAttr)->qamLockRange =
               *((pDRXQamLockRange_t)config->cfgData);
         return DRX_STS_OK;
      default:
         return (DRX_STS_FUNC_NOT_AVAILABLE);
   }

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Get 'some' configuration of the device.
* \param demod Pointer to demodulator instance.
* \param config Pointer to configuration parameters (type and data).
* \return DRXStatus_t.
*/

static DRXStatus_t
CtrlGetCfg( pDRXDemodInstance_t   demod,
            pDRXCfg_t             config )
{

   if ( config == NULL )
   {
      return DRX_STS_INVALID_ARG;
   }

   switch ( config->cfgType )
   {
      case DRX_CFG_MPEG_OUTPUT:
         return CtrlGetCfgMPEGOutput( demod,
                                      (pDRXCfgMPEGOutput_t) config->cfgData );
      case DRX_CFG_PINS_SAFE_MODE:
         return CtrlGetCfgPdrSafeMode( demod,
                                       (pBool_t) config->cfgData );
      case DRXK_CFG_AGC_RF:
         return AGCCtrlGetCfgRf(       demod,
                                       (pDRXKCfgAgc_t) config->cfgData );
      case DRXK_CFG_AGC_IF:
         return AGCCtrlGetCfgIf(       demod,
                                       (pDRXKCfgAgc_t) config->cfgData );
      case    DRXK_CFG_PRE_SAW:
         return AGCCtrlGetCfgPreSaw(   demod,
                                       (pDRXKCfgPreSaw_t) config->cfgData );
      case DRXK_CFG_ATV_EQU_COEF:
         return ATVCtrlGetCfgEquCoef ( demod,
                                       (pDRXKCfgAtvEquCoef_t) config->cfgData );
      case DRX_CFG_ATV_STANDARD:

         return ATVCtrlGetCfgStandard (   demod,
                                          (pDRXStandard_t) config->cfgData );
      case DRXK_CFG_ATV_MISC:
         return ATVCtrlGetCfgMisc ( demod,
                                    (pDRXKCfgAtvMisc_t) config->cfgData );
      case DRXK_CFG_ATV_OUTPUT:
         return ATVCtrlGetCfgOutput  ( demod,
                                       (pDRXKCfgAtvOutput_t) config->cfgData );
      case DRXK_CFG_ATV_AGC_STATUS:
         return ATVCtrlGetCfgAgcStatus( demod,
                                        (pDRXKCfgAtvAgcStatus_t)
                                        config->cfgData );
      case DRXK_CFG_SYMBOL_CLK_OFFSET:
         return DTVCtrlGetCfgSymbolClockOffset(   demod,
                                                      (ps32_t)
                                                         config->cfgData );
      case DRX_CFG_SUBSTANDARD:
         return CtrlGetCfgSubStandard( demod,
                                       (pDRXSubstandard_t) config->cfgData );
      case DRXK_CFG_PACKET_ERR_MODE:
         return DTVCtrlGetCfgPacketErrMode( demod,
                                          (pDRXKCfgPacketErrMode_t)
                                          config->cfgData );
      case DRXK_CFG_DVBT_SQI_SPEED:
         return DVBTCtrlGetCfgSqiSpeed(   demod,
                                       (pDRXKCfgDvbtSqiSpeed_t)
                                       config->cfgData);
#ifndef DRXK_EXCLUDE_AUDIO
      case DRX_CFG_AUD_VOLUME:
         return AUDCtrlGetCfgVolume(      demod,
                                          (pDRXCfgAudVolume_t)config->cfgData );
      case DRX_CFG_I2S_OUTPUT:
         return AUDCtrlGetCfgOutputI2S(   demod,
                                          (pDRXCfgI2SOutput_t)config->cfgData );

      case DRX_CFG_AUD_RDS:
         return  AUDCtrlGetCfgRDS(        demod,
                                          (pDRXCfgAudRDS_t)config->cfgData );
      case DRX_CFG_AUD_AUTOSOUND:
         return AUDCtrlGetCfgAutoSound(   demod,
                                          (pDRXCfgAudAutoSound_t)
                                             config->cfgData);
      case DRX_CFG_AUD_ASS_THRES:
         return AUDCtrlGetCfgASSThres(    demod,
                                          (pDRXCfgAudASSThres_t)
                                             config->cfgData);
      case DRX_CFG_AUD_CARRIER:
         return AUDCtrlGetCfgCarrier(     demod,
                                          (pDRXCfgAudCarriers_t)
                                             config->cfgData);
      case DRX_CFG_AUD_DEVIATION:
         return AUDCtrlGetCfgDev(         demod,
                                          (pDRXCfgAudDeviation_t)
                                             config->cfgData);
      case DRX_CFG_AUD_PRESCALE:
         return AUDCtrlGetCfgPrescale(    demod,
                                          (pDRXCfgAudPrescale_t)
                                             config->cfgData);
      case DRX_CFG_AUD_MIXER:
         return AUDCtrlGetCfgMixer(       demod,
                                          (pDRXCfgAudMixer_t)config->cfgData);
      case DRX_CFG_AUD_AVSYNC:
         return AUDCtrlGetCfgAVSync(      demod,
                                          (pDRXCfgAudAVSync_t)config->cfgData);
#endif
      /* ACCES (XS) functions from here */
      case DRX_XS_CFG_PRESET:
         *((char **)config->cfgData) =
               ((pDRXKData_t)demod->myExtAttr)->presetName;
         return DRX_STS_OK;
      case DRX_XS_CFG_AUD_BTSC_DETECT:
         *((pDRXAudBtscDetect_t)config->cfgData) =
               ((pDRXKData_t)demod->myExtAttr)->audData.btscDetect;
         return DRX_STS_OK;
      case DRX_XS_CFG_QAM_LOCKRANGE:
         *((pDRXQamLockRange_t)config->cfgData) =
               ((pDRXKData_t)demod->myExtAttr)->qamLockRange;
         return DRX_STS_OK;
      default:
         return (DRX_STS_FUNC_NOT_AVAILABLE);
   }

   return DRX_STS_OK;
}

/*============================================================================*/

/**
* \brief Read ROM version number
* \param demod        Demodulator to read ROM address from.
* \param version      Address to store version number.
* \return DRXStatus_t.
* \retval DRX_STS_OK    Success
* \retval DRX_STS_ERROR Failure: I2C error
*
* DRXK does not have a version entry in the ROM.
* Therefore, to find the ROM version number, we will select one based on the
* metal fix of the detected device.
*/

static DRXStatus_t
BL_ReadRomVersion (pDRXDemodInstance_t demod, pu32_t version)
{
   /* Fake ROM version number read: DRXK ROM does not have an easy way
      to find it.
      We use the metal fix ID to "guess" the ROM version number. */
   u32_t mfx;
   DRX_GET_MFX (demod, mfx);
   switch (mfx)
   {
   case 0:
      *version = 0x00060014;   /* 0.6.14 */
      break;
   case 2:
      *version = 0x00070001;   /* 0.7.1 */
      break;
   case 3:
      *version = 0x00090000;   /* 0.9.0 */
      break;
      default:
      *version = 0xFFFFFFFF;   /* Unknown */
      break;
   }
   return DRX_STS_OK;
}


/**
* \brief Validate microcode instance prior to loading.
* \param demod     Pointer to demodulator instance.
* \return DRXStatus_t.
* \retval DRX_STS_OK            Microcode is valid for usage
* \retval DRX_STS_INVALID_ARG   Wrong device ID or meant for different ROM
* \retval DRX_STS_ERROR         Error in the validation process
*
*/
static DRXStatus_t
CtrlValidateUCode( pDRXDemodInstance_t demod )
{
   u32_t prodID, mcDev, mcPatch, ROMVer;
   u16_t mcVerType;

   /* Check device.
      Disallow microcode if:
      -  MC has version record AND
      -  device ID in version record is not 0 AND
      -  product ID in version record's device ID does not
         match product ID of device
   */
   DRX_GET_MCVERTYPE (demod, mcVerType);
   DRX_GET_MCDEV     (demod, mcDev);
   DRX_GET_MCPATCH   (demod, mcPatch);
   DRX_GET_PRODUCTID (demod, prodID);
   //printf("mcDev:0x%X,prodID:0x%X\n",mcDev,prodID);
   if (DRX_ISMCVERTYPE (mcVerType))
   {
		if ((mcDev != 0) && (((mcDev >> 16) & 0xFFF) != (prodID & 0xFFF)))
		{
			/* Microcode is marked for another device - error */
			//printf("%s error line %d,mcDev:0x%X,prodID:%d\n",mcDev,prodID,__FUNCTION__,__LINE__);
			return DRX_STS_INVALID_ARG;
		}
		else
		{
			/* Valid device ID - now check the version numbers. */
			/* Error if microcode is patch and ROM version does not match */
			if (mcPatch != 0)
			{
				/* Check whether this patch can be used */
				/* Read ROM version number */
				CHK_ERROR (BL_ReadRomVersion (demod, &ROMVer));
				if (ROMVer != mcPatch)
				{
				   /* Patch error - patch is not meant for this ROM */
				   //printf("%s error line %d \n",__FUNCTION__,__LINE__);
				   return DRX_STS_INVALID_ARG;
				}
			}
		}
   }

   /* Everything else: OK */
   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}


/**
* \brief Handle ucode upload (raising a flag only)
* \param demod     Pointer to demodulator instance.
* \return DRXStatus_t.
* \retval DRX_STS_FUNC_NOT_AVAILABLE
*
* Using this hook to the device dependent code only to flag upload of ucode.
* Returning DRX_STS_FUNC_NOT_AVAILABLE to trigger usage of shared
* implementation of ucode upload.
* Flag is used to distinguish between usage of ROM ucode and uploaded
* (patched) ucode.
*
*/
static DRXStatus_t
CtrlLoadUCode( pDRXDemodInstance_t demod )
{
   pDRXKData_t extAttr = NULL;

   extAttr = (pDRXKData_t) demod ->myExtAttr;

   extAttr->ucodeUploaded = TRUE;

   return ( DRX_STS_FUNC_NOT_AVAILABLE);
}

/*===========================================================================
=============================================================================
===== EXPORTED FUNCTIONS ====================================================
=============================================================================
===========================================================================*/

/**
* \brief Open the demod instance, configure device, configure drxdriver
* \return Status_t Return status.
*
* DRX_Open() can be called with a NULL ucode image => no ucode upload.
* This means that DRX_Open() must NOT contain SCU commands or, in general,
* rely on SCU or SC ucode to be present.
*
*/
DRXStatus_t
DRXK_Open( pDRXDemodInstance_t demod )
{

   pI2CDeviceAddr_t   devAddr       = NULL;
   pDRXKData_t        extAttr       = NULL;
   pDRXCommonAttr_t   commonAttr    = NULL;
   u16_t              driverVersion = 0;
   DRXPowerMode_t     powerMode     = DRXK_POWER_DOWN_OFDM;
   DRXUCodeInfo_t     ucodeInfo     = {NULL, 0};

   /* Check arguments */
   if (demod -> myExtAttr == NULL )
   {
      return ( DRX_STS_INVALID_ARG);
   }

   devAddr    = demod->myI2CDevAddr;
   extAttr    = (pDRXKData_t)demod->myExtAttr;
   commonAttr = (pDRXCommonAttr_t)demod->myCommonAttr;

   DRXK_Preset( demod, DRXK_PRESET_LEVEL_BASELINE, &extAttr->presets, NULL );

   if ( demod->myTuner != NULL )
   {
      /* select presets based on tuner name */
      extAttr->presetName = NULL;
      if ( demod->myTuner->myCommonAttr != NULL )
      {
         extAttr->presetName = demod->myTuner->myCommonAttr->name;
      }
   }
   DRXK_Preset( demod, DRXK_PRESET_LEVEL_OPEN, &extAttr->presets, NULL );

   /* Copying:
      - IF AGC polarity
      - tunerPortNr
      - tunerMirrors
      from presets to existing commonAttr field for sake of consistancy with
      other DRX-es, and preventing hazard of uninitialized value */
   commonAttr->tunerIfAgcPol = extAttr->presets.tunerIfAgcPol;
   commonAttr->tunerPortNr   = extAttr->presets.tunerPortNr;
   commonAttr->mirrorFreqSpect   = extAttr->presets.tunerMirrors;

   /* Make sure device is not in power saving mode */
   CHK_ERROR( WakeUpDevice( demod ) );

   /* Soft reset of OFDM-, sys- and osc-clockdomain */
   WR16( devAddr, SIO_CC_SOFT_RST__A, ( SIO_CC_SOFT_RST_OFDM__M |
                                        SIO_CC_SOFT_RST_SYS__M  |
                                        SIO_CC_SOFT_RST_OSC__M  ) );
   WR16( devAddr, SIO_CC_UPDATE__A, SIO_CC_UPDATE_KEY);
   CHK_ERROR( DRXBSP_HST_Sleep(1) );

   CHK_ERROR( GetDeviceCapabilities( demod ) );
   CHK_ERROR( InitHI( demod ) );

   /* disable various processes */
   WR16( devAddr, SCU_RAM_GPIO__A, SCU_RAM_GPIO_HW_LOCK_IND_DISABLE );

   CHK_ERROR( MPEGTSDisable( demod ) );
   WR16( devAddr, AUD_COMM_EXEC__A, AUD_COMM_EXEC_STOP );
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_STOP );

   /* Inform Audio it will need to do all setting */
   extAttr->audData.audioIsActive = FALSE;

   /* Enable token-ring bus through OFDM block for possible ucode upload */
   WR16( devAddr, SIO_OFDM_SH_OFDM_RING_ENABLE__A,
                  SIO_OFDM_SH_OFDM_RING_ENABLE_ON );
   /* Handle ucode upload via bootloader */
   if ( extAttr->useBootloader == TRUE )
   {
      WR16( devAddr, SIO_BL_COMM_EXEC__A, SIO_BL_COMM_EXEC_ACTIVE);
      CHK_ERROR( BLChainCmd(  devAddr,
                              DRXK_BL_ROM_OFFSET_UCODE,
                              DRXK_BLCC_NR_ELEMENTS_UCODE ));
   }
   /* Upload/patch microcode */
   extAttr->ucodeUploaded = FALSE;
   if ( commonAttr->microcode != NULL )
   {
		/* Dirty trick to use common ucode upload & verify,
		pretend device is already open */
		commonAttr->isOpened = TRUE;
		ucodeInfo.mcData = commonAttr->microcode;
		ucodeInfo.mcSize = commonAttr->microcodeSize;
		//printf("mcSize:%d\n",ucodeInfo.mcSize);
		CHK_ERROR( DRX_Ctrl( demod, DRX_CTRL_LOAD_UCODE, &ucodeInfo) );
		if ( commonAttr->verifyMicrocode == TRUE )
		{
			CHK_ERROR( DRX_Ctrl ( demod, DRX_CTRL_VERIFY_UCODE, &ucodeInfo) );
		}
		commonAttr->isOpened = FALSE;
   }
   /* disable token-ring bus through OFDM block */
   WR16( devAddr, SIO_OFDM_SH_OFDM_RING_ENABLE__A,
                  SIO_OFDM_SH_OFDM_RING_ENABLE_OFF );

   /* Activate SCU */
   WR16( devAddr, SCU_COMM_EXEC__A, SCU_COMM_EXEC_ACTIVE);

   /* SCU alive check */
   {
      /* Execute NOP command.                                            */
      /* Successful execution means that the SCU is processing commands. */
      /* The function does not take arguments and does not return them.  */
      DRXKSCUCmd_t cmdSCU;
      cmdSCU.command =
         SCU_RAM_COMMAND_STANDARD_TOP + SCU_RAM_COMMAND_CMD_ADMIN_NOP;
      cmdSCU.parameterLen = 0;
      cmdSCU.resultLen = 0;
      CHK_ERROR (SCUCommand (devAddr, &cmdSCU));
   }

   /* Open tuner instance */
   if (  (demod->myTuner != NULL) &&
         (demod->myTuner->myCommonAttr != NULL))
   {
      demod->myTuner->myCommonAttr->myUserData  = (void *)demod;

      if ( commonAttr->tunerPortNr == 1 )
      {
         /* close tuner bridge */
         Bool_t bridgeClosed = TRUE;
         CHK_ERROR( CtrlI2CBridge( demod, &bridgeClosed ) );
         /* set tuner frequency */
      }

      CHK_ERROR( DRXBSP_TUNER_Open( demod -> myTuner ) );

      if ( commonAttr->tunerPortNr == 1 )
      {
         /* open tuner bridge */
         Bool_t bridgeClosed = FALSE;
         CHK_ERROR( CtrlI2CBridge( demod, &bridgeClosed ) );
      }

      commonAttr->tunerMinFreqRF = ((demod->myTuner)->myCommonAttr->minFreqRF);
      commonAttr->tunerMaxFreqRF = ((demod->myTuner)->myCommonAttr->maxFreqRF);
   }

   /* ADCCompensation */
   if ( extAttr->hasATV == TRUE)
   {
      /* If the device has ATV, do the ADCCompensation once */
      CHK_ERROR( ATVStart( demod ));
      CHK_ERROR( ADCCompensation( demod) );
      CHK_ERROR( ATVPowerDown( demod ));
   }

   /* Power down the OFDM part */
   powerMode = DRXK_POWER_DOWN_OFDM;
   CHK_ERROR( CtrlPowerMode( demod, &powerMode) );

   /* Initialize scan timeout */
   commonAttr -> scanDemodLockTimeout = DRXK_SCAN_TIMEOUT;
   commonAttr -> scanDesiredLock = DRX_LOCKED;

   extAttr->standard = DRX_STANDARD_UNKNOWN;
   extAttr->standardActive = FALSE;

   /* Stamp driver version number in SCU data RAM in BCD code
      Done to enable field application engineers to retreive drxdriver version
      via I2C from SCU RAM.
      Not using SCU command interface for SCU register access since no
      microcode may be present.
   */
   driverVersion = (((VERSION_MAJOR/100) % 10) << 12) +
                   (((VERSION_MAJOR/10)  % 10) <<  8) +
                   ( (VERSION_MAJOR%10)        <<  4) +
                     (VERSION_MINOR%10);
   WR16(devAddr, SCU_RAM_DRIVER_VER_HI__A, driverVersion );
   driverVersion = (((VERSION_PATCH/1000) % 10) << 12) +
                   (((VERSION_PATCH/100)  % 10) <<  8) +
                   (((VERSION_PATCH/10)   % 10) <<  4) +
                     (VERSION_PATCH%10);
   WR16(devAddr, SCU_RAM_DRIVER_VER_LO__A, driverVersion );

   /* Reset driver debug flags to 0 */
   WR16(devAddr, SCU_RAM_DRIVER_DEBUG__A, 0);


   /* Setup FEC OC:
      NOTE: No more full FEC resets allowed afterwards!! */
   WR16( devAddr, FEC_COMM_EXEC__A, FEC_COMM_EXEC_STOP );
   CHK_ERROR( MPEGTSDtoInit( devAddr ) );
   CHK_ERROR( MPEGTSStop( demod ) );
   CHK_ERROR(
      MPEGTSConfigurePins( demod,
                           demod->myCommonAttr->mpegCfg.enableMPEGOutput ) );

   /* refresh the audio data structure with default */
   extAttr->audData = DRXKDefaultAudData_g;

   return DRX_STS_OK;

rw_error:
   commonAttr->isOpened = FALSE;
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief Close the demod instance, power down the device
* \return Status_t Return status.
*
*/
DRXStatus_t
DRXK_Close( pDRXDemodInstance_t demod )
{
   pI2CDeviceAddr_t devAddr    = NULL;
   pDRXKData_t      extAttr    = NULL;
   pDRXCommonAttr_t commonAttr = NULL;
   DRXPowerMode_t   powerMode  = DRX_POWER_UP;

   commonAttr = (pDRXCommonAttr_t) demod -> myCommonAttr;
   devAddr = demod -> myI2CDevAddr;
   extAttr = (pDRXKData_t) demod -> myExtAttr;

   /* Close tuner instance */
   if ( demod->myTuner != NULL )
   {
      /* Check if bridge is used */
      if ( commonAttr->tunerPortNr == 1 )
      {
         Bool_t bridgeClosed = TRUE;
         CHK_ERROR( CtrlI2CBridge( demod, &bridgeClosed ) );
      }
      CHK_ERROR( DRXBSP_TUNER_Close( demod -> myTuner ) );
      if ( commonAttr->tunerPortNr == 1 )
      {
         Bool_t bridgeClosed = FALSE;
         CHK_ERROR( CtrlI2CBridge( demod, &bridgeClosed ) );
      }
   };

   /* Put device in max power saving mode */
   powerMode  =DRX_POWER_DOWN;
   CHK_ERROR( CtrlPowerMode( demod, &powerMode ));

   return DRX_STS_OK;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief DRXK specific tuner control function
* \return Status_t Return status.
*/

static DRXStatus_t
CtrlProgramTuner(    pDRXDemodInstance_t  demod,
                     pDRXChannel_t        channel )
{
   pDRXKData_t extAttr = (pDRXKData_t)NULL;
   DRXFrequency_t  freqSet = 0;

   extAttr = (pDRXKData_t)demod->myExtAttr;


   /* special handling for ATV, conversion for target carrier */
   if ( DRXK_ISATVSTD( extAttr->standard ) )
   {
      CHK_ERROR (ATVTunerFreq(   demod,
                                 extAttr->standard,
                                 channel,
                                 &freqSet));

      extAttr->tunerSetFreq = freqSet;
      channel->frequency    = freqSet;
   }
   else
   {
      extAttr->tunerSetFreq = channel->frequency;
   }


   return DRX_STS_FUNC_NOT_AVAILABLE;
rw_error:
   return DRX_STS_ERROR;
}

/*============================================================================*/
/**
* \brief DRXK specific control function
* \return Status_t Return status.
*/
DRXStatus_t
DRXK_Ctrl( pDRXDemodInstance_t demod,
           DRXCtrlIndex_t      ctrl,
           void                *ctrlData )
{

   switch ( ctrl ) {
      /*======================================================================*/
      case DRX_CTRL_SET_CHANNEL:
         {
            return CtrlSetChannel( demod, (pDRXChannel_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_GET_CHANNEL:
         {
            return CtrlGetChannel( demod, (pDRXChannel_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_SIG_QUALITY:
         {
         return CtrlSigQuality ( demod, (pDRXSigQuality_t) ctrlData);
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_SIG_STRENGTH:
         {
            return CtrlSigStrength ( demod, (pu16_t) ctrlData);
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_SET_CFG:
         {
            return CtrlSetCfg( demod, (pDRXCfg_t)ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_GET_CFG:
         {
            return CtrlGetCfg ( demod, (pDRXCfg_t) ctrlData);
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_I2C_BRIDGE:
         {
            return CtrlI2CBridge( demod, (pBool_t)ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_LOCK_STATUS:
         {
            return CtrlLockStatus( demod, (pDRXLockStatus_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_SET_STANDARD:
         {
            return CtrlSetStandard( demod, ( pDRXStandard_t )ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_GET_STANDARD:
         {
            return CtrlGetStandard( demod, (pDRXStandard_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_POWER_MODE:
         {
            return CtrlPowerMode( demod, (pDRXPowerMode_t)ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_VERSION:
         {
            return CtrlVersion( demod, (pDRXVersionList_t *) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_PROBE_DEVICE:
         {
            return CtrlProbeDevice( demod );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_SET_UIO_CFG:
         {
            return CtrlSetUIOCfg( demod, (pDRXUIOCfg_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_GET_UIO_CFG:
         {
            return CtrlGetUIOCfg( demod, (pDRXUIOCfg_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_UIO_READ:
         {
            return CtrlUIORead( demod, (pDRXUIOData_t)ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_UIO_WRITE:
         {
            return CtrlUIOWrite( demod, (pDRXUIOData_t)ctrlData );
         }
         break;
#ifndef DRXK_EXCLUDE_AUDIO
      /*======================================================================*/
      case DRX_CTRL_AUD_SET_STANDARD:
         {
            return AUDCtrlSetStandard( demod, (pDRXAudStandard_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_AUD_GET_STANDARD:
         {
            return AUDCtrlGetStandard( demod, (pDRXAudStandard_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_AUD_GET_STATUS:
         {
            return AUDCtrlGetStatus( demod, (pDRXAudStatus_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_AUD_BEEP:
         {
            return AUDCtrlBeep( demod, (pDRXAudBeep_t) ctrlData );
         }
         break;
      /*======================================================================*/

#endif
      case DRX_CTRL_TPS_INFO:
         {
            return CtrlTPSInfo( demod, (pDRXTPSInfo_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_LOAD_UCODE:
         {
            return CtrlLoadUCode( demod);
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_VALIDATE_UCODE:
         {
            return CtrlValidateUCode( demod);
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_LOAD_FILTER:
      {
            return CtrlLoadFilter(  demod,
                                    (pDRXFilterInfo_t)ctrlData );
      }
         break;
      /*======================================================================*/
      case DRX_CTRL_PROGRAM_TUNER:
            {
            return CtrlProgramTuner ( demod,
                                       (pDRXChannel_t) ctrlData );
         }
         break;
      /*======================================================================*/
      case DRX_CTRL_CONSTEL:
         {
            return CtrlGetConstel ( demod,
                                    (pDRXComplex_t) ctrlData );
         }
         break;

         break;
      /*======================================================================*/
      default:
         return (DRX_STS_FUNC_NOT_AVAILABLE);
      }
   return DRX_STS_OK;
   }
/* END OF FILE */



