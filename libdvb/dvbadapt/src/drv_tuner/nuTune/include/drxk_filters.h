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
*/

/*------------------------------------------------------------------------------
INCLUDES
------------------------------------------------------------------------------*/

#include "drx_driver.h"


/*------------------------------------------------------------------------------
TAPSETS
------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
DEFAULT TAPSETS
------------------------------------------------------------------------------*/

/* default DVBT taps */
#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_DVBT
#define  DRXK_TAPS_DVBT
#endif
#endif

#ifdef   DRXK_TAPS_DVBT
#ifndef  DRXK_TAPS_DVBT_SELECTED
#define  DRXK_TAPS_DVBT_SELECTED

#define  DRXK_TAPS_DVBT_RE drxk_taps_dvbt_g
#define  DRXK_TAPS_DVBT_IM drxk_taps_dvbt_g

static const u8_t drxk_taps_dvbt_g[]= {
   DRX_16TO8(  -10 ),  /* re0  */
   DRX_16TO8(    0 ),  /* re1  */
   DRX_16TO8(    8 ),  /* re2  */
   DRX_16TO8(    7 ),  /* re3  */
   DRX_16TO8(   -4 ),  /* re4  */
   DRX_16TO8(  -11 ),  /* re5  */
   DRX_16TO8(    0 ),  /* re6  */
   DRX_16TO8(   16 ),  /* re7  */
   DRX_16TO8(   12 ),  /* re8  */
   DRX_16TO8(  -13 ),  /* re9  */
   DRX_16TO8(  -22 ),  /* re10 */
   DRX_16TO8(    3 ),  /* re11 */
   DRX_16TO8(   31 ),  /* re12 */
   DRX_16TO8(   17 ),  /* re13 */
   DRX_16TO8(  -29 ),  /* re14 */
   DRX_16TO8(  -39 ),  /* re15 */
   DRX_16TO8(   13 ),  /* re16 */
   DRX_16TO8(   59 ),  /* re17 */
   DRX_16TO8(   21 ),  /* re18 */
   DRX_16TO8(  -63 ),  /* re19 */
   DRX_16TO8(  -68 ),  /* re20 */
   DRX_16TO8(   42 ),  /* re21 */
   DRX_16TO8(  124 ),  /* re22 */
   DRX_16TO8(   24 ),  /* re23 */
   DRX_16TO8( -175 ),  /* re24 */
   DRX_16TO8( -172 ),  /* re25 */
   DRX_16TO8(  212 ),  /* re26 */
   DRX_16TO8(  762 )   /* re27 */
};

#endif   /* DRXK_TAPS_DVBT_SELECTED */
#endif   /* DRXK_DVBT_TAPS */



/* default ITU-A taps */
#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_ITU_A
#define  DRXK_TAPS_ITU_A
#endif
#endif

#ifdef   DRXK_TAPS_ITU_A
#ifndef  DRXK_TAPS_ITU_A_SELECTED
#define  DRXK_TAPS_ITU_A_SELECTED

#define  DRXK_TAPS_ITU_A_RE drxk_taps_itu_a_g
#define  DRXK_TAPS_ITU_A_IM drxk_taps_itu_a_g

static const u8_t drxk_taps_itu_a_g[]= {
   DRX_16TO8(  -1 ),  /* re0  */
   DRX_16TO8(   1 ),  /* re1  */
   DRX_16TO8(   1 ),  /* re2  */
   DRX_16TO8(  -1 ),  /* re3  */
   DRX_16TO8(  -1 ),  /* re4  */
   DRX_16TO8(   2 ),  /* re5  */
   DRX_16TO8(   1 ),  /* re6  */
   DRX_16TO8(  -2 ),  /* re7  */
   DRX_16TO8(   0 ),  /* re8  */
   DRX_16TO8(   3 ),  /* re9  */
   DRX_16TO8(  -1 ),  /* re10 */
   DRX_16TO8(  -3 ),  /* re11 */
   DRX_16TO8(   4 ),  /* re12 */
   DRX_16TO8(   1 ),  /* re13 */
   DRX_16TO8(  -8 ),  /* re14 */
   DRX_16TO8(   4 ),  /* re15 */
   DRX_16TO8(  13 ),  /* re16 */
   DRX_16TO8( -13 ),  /* re17 */
   DRX_16TO8( -19 ),  /* re18 */
   DRX_16TO8(  28 ),  /* re19 */
   DRX_16TO8(  25 ),  /* re20 */
   DRX_16TO8( -53 ),  /* re21 */
   DRX_16TO8( -31 ),  /* re22 */
   DRX_16TO8(  96 ),  /* re23 */
   DRX_16TO8(  37 ),  /* re24 */
   DRX_16TO8(-190 ),  /* re25 */
   DRX_16TO8( -40 ),  /* re26 */
   DRX_16TO8( 619 )   /* re27 */
};

#endif /* DRXK_TAPS_ITU_A_SELECTED */
#endif /* DRXK_TAPS_ITU_A */



/* default ITU-C taps */
#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_ITU_C
#define  DRXK_TAPS_ITU_C
#endif
#endif

#ifdef   DRXK_TAPS_ITU_C
#ifndef  DRXK_TAPS_ITU_C_SELECTED
#define  DRXK_TAPS_ITU_C_SELECTED

#define  DRXK_TAPS_ITU_C_RE drxk_taps_itu_c_g
#define  DRXK_TAPS_ITU_C_IM drxk_taps_itu_c_g

static const u8_t drxk_taps_itu_c_g[]= {
   DRX_16TO8(  -3 ),  /* re0  */
   DRX_16TO8(   3 ),  /* re1  */
   DRX_16TO8(   2 ),  /* re2  */
   DRX_16TO8(  -4 ),  /* re3  */
   DRX_16TO8(   0 ),  /* re4  */
   DRX_16TO8(   4 ),  /* re5  */
   DRX_16TO8(  -1 ),  /* re6  */
   DRX_16TO8(  -4 ),  /* re7  */
   DRX_16TO8(   3 ),  /* re8  */
   DRX_16TO8(   3 ),  /* re9  */
   DRX_16TO8(  -5 ),  /* re10 */
   DRX_16TO8(   0 ),  /* re11 */
   DRX_16TO8(   9 ),  /* re12 */
   DRX_16TO8(  -4 ),  /* re13 */
   DRX_16TO8( -12 ),  /* re14 */
   DRX_16TO8(  10 ),  /* re15 */
   DRX_16TO8(  16 ),  /* re16 */
   DRX_16TO8( -21 ),  /* re17 */
   DRX_16TO8( -20 ),  /* re18 */
   DRX_16TO8(  37 ),  /* re19 */
   DRX_16TO8(  25 ),  /* re20 */
   DRX_16TO8( -62 ),  /* re21 */
   DRX_16TO8( -28 ),  /* re22 */
   DRX_16TO8( 105 ),  /* re23 */
   DRX_16TO8(  31 ),  /* re24 */
   DRX_16TO8(-197 ),  /* re25 */
   DRX_16TO8( -33 ),  /* re26 */
   DRX_16TO8( 626 )   /* re27 */
};

#endif /* DRXK_TAPS_ITU_C_SELECTED */
#endif /* DRXK_TAPS_ITU_C */



/* default NTSC taps */
#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_NTSC
#define  DRXK_TAPS_NTSC
#endif
#endif

#ifdef   DRXK_TAPS_NTSC
#ifndef  DRXK_TAPS_NTSC_SELECTED
#define  DRXK_TAPS_NTSC_SELECTED

#define  DRXK_TAPS_NTSC_RE drxk_taps_ntsc_re_g
#define  DRXK_TAPS_NTSC_IM drxk_taps_ntsc_im_g

static const u8_t drxk_taps_ntsc_re_g[]= {
   DRX_16TO8(  -16 ),  /* re0  */
   DRX_16TO8(   -1 ),  /* re1  */
   DRX_16TO8(   22 ),  /* re2  */
   DRX_16TO8(    7 ),  /* re3  */
   DRX_16TO8(  -24 ),  /* re4  */
   DRX_16TO8(  -13 ),  /* re5  */
   DRX_16TO8(   26 ),  /* re6  */
   DRX_16TO8(   20 ),  /* re7  */
   DRX_16TO8(  -26 ),  /* re8  */
   DRX_16TO8(  -30 ),  /* re9  */
   DRX_16TO8(   22 ),  /* re10 */
   DRX_16TO8(   41 ),  /* re11 */
   DRX_16TO8(  -11 ),  /* re12 */
   DRX_16TO8(  -43 ),  /* re13 */
   DRX_16TO8(    0 ),  /* re14 */
   DRX_16TO8(   38 ),  /* re15 */
   DRX_16TO8(    3 ),  /* re16 */
   DRX_16TO8(  -34 ),  /* re17 */
   DRX_16TO8(   -3 ),  /* re18 */
   DRX_16TO8(   25 ),  /* re19 */
   DRX_16TO8(  -19 ),  /* re20 */
   DRX_16TO8(  -39 ),  /* re21 */
   DRX_16TO8(   43 ),  /* re22 */
   DRX_16TO8(   79 ),  /* re23 */
   DRX_16TO8(  -74 ),  /* re24 */
   DRX_16TO8( -193 ),  /* re25 */
   DRX_16TO8(   96 ),  /* re26 */
   DRX_16TO8(  701 )   /* re27 */
};

static const u8_t drxk_taps_ntsc_im_g[]= {
   DRX_16TO8(   12 ),  /* im0  */
   DRX_16TO8(   -5 ),  /* im1  */
   DRX_16TO8(  -12 ),  /* im2  */
   DRX_16TO8(    7 ),  /* im3  */
   DRX_16TO8(   17 ),  /* im4  */
   DRX_16TO8(    5 ),  /* im5  */
   DRX_16TO8(    6 ),  /* im6  */
   DRX_16TO8(   19 ),  /* im7  */
   DRX_16TO8(    7 ),  /* im8  */
   DRX_16TO8(  -13 ),  /* im9  */
   DRX_16TO8(   17 ),  /* im10 */
   DRX_16TO8(   69 ),  /* im11 */
   DRX_16TO8(   44 ),  /* im12 */
   DRX_16TO8(  -37 ),  /* im13 */
   DRX_16TO8(  -24 ),  /* im14 */
   DRX_16TO8(  103 ),  /* im15 */
   DRX_16TO8(  149 ),  /* im16 */
   DRX_16TO8(   14 ),  /* im17 */
   DRX_16TO8(  -93 ),  /* im18 */
   DRX_16TO8(   37 ),  /* im19 */
   DRX_16TO8(  243 ),  /* im20 */
   DRX_16TO8(  209 ),  /* im21 */
   DRX_16TO8(  -26 ),  /* im22 */
   DRX_16TO8(  -84 ),  /* im23 */
   DRX_16TO8(  199 ),  /* im24 */
   DRX_16TO8(  540 ),  /* im25 */
   DRX_16TO8(  600 ),  /* im26 */
   DRX_16TO8(  362 )   /* im27 */
};

#endif /* DRXK_TAPS_NTSC_SELECTED */
#endif /* DRXK_TAPS_NTSC */



/* default BG taps */
#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_BG
#define  DRXK_TAPS_BG
#endif
#endif

#ifdef   DRXK_TAPS_BG
#ifndef  DRXK_TAPS_BG_SELECTED
#define  DRXK_TAPS_BG_SELECTED

#define  DRXK_TAPS_BG_RE drxk_taps_bg_re_g
#define  DRXK_TAPS_BG_IM drxk_taps_bg_im_g

static const u8_t drxk_taps_bg_re_g[]= {
   DRX_16TO8(   -3 ),  /* re0  */
   DRX_16TO8(    0 ),  /* re1  */
   DRX_16TO8(   10 ),  /* re2  */
   DRX_16TO8(   16 ),  /* re3  */
   DRX_16TO8(  -15 ),  /* re4  */
   DRX_16TO8(  -15 ),  /* re5  */
   DRX_16TO8(   30 ),  /* re6  */
   DRX_16TO8(   15 ),  /* re7  */
   DRX_16TO8(  -34 ),  /* re8  */
   DRX_16TO8(  -10 ),  /* re9  */
   DRX_16TO8(   24 ),  /* re10 */
   DRX_16TO8(    6 ),  /* re11 */
   DRX_16TO8(  -11 ),  /* re12 */
   DRX_16TO8(  -30 ),  /* re13 */
   DRX_16TO8(  -17 ),  /* re14 */
   DRX_16TO8(   65 ),  /* re15 */
   DRX_16TO8(   22 ),  /* re16 */
   DRX_16TO8( -133 ),  /* re17 */
   DRX_16TO8(  -11 ),  /* re18 */
   DRX_16TO8(  201 ),  /* re19 */
   DRX_16TO8(  -39 ),  /* re20 */
   DRX_16TO8( -268 ),  /* re21 */
   DRX_16TO8(  112 ),  /* re22 */
   DRX_16TO8(  320 ),  /* re23 */
   DRX_16TO8( -191 ),  /* re24 */
   DRX_16TO8( -389 ),  /* re25 */
   DRX_16TO8(  258 ),  /* re26 */
   DRX_16TO8(  873 )   /* re27 */
};

static const u8_t drxk_taps_bg_im_g[]= {
   DRX_16TO8(   15 ),  /* im0  */
   DRX_16TO8(   12 ),  /* im1  */
   DRX_16TO8(  -22 ),  /* im2  */
   DRX_16TO8(  -19 ),  /* im3  */
   DRX_16TO8(   10 ),  /* im4  */
   DRX_16TO8(   -2 ),  /* im5  */
   DRX_16TO8(  -23 ),  /* im6  */
   DRX_16TO8(  -25 ),  /* im7  */
   DRX_16TO8(  -25 ),  /* im8  */
   DRX_16TO8(   -6 ),  /* im9  */
   DRX_16TO8(   -6 ),  /* im10 */
   DRX_16TO8(  -67 ),  /* im11 */
   DRX_16TO8(  -57 ),  /* im12 */
   DRX_16TO8(   36 ),  /* im13 */
   DRX_16TO8(  -12 ),  /* im14 */
   DRX_16TO8( -118 ),  /* im15 */
   DRX_16TO8(  -12 ),  /* im16 */
   DRX_16TO8(   81 ),  /* im17 */
   DRX_16TO8(  -50 ),  /* im18 */
   DRX_16TO8(  -63 ),  /* im19 */
   DRX_16TO8(   90 ),  /* im20 */
   DRX_16TO8(   57 ),  /* im21 */
   DRX_16TO8(   25 ),  /* im22 */
   DRX_16TO8(  172 ),  /* im23 */
   DRX_16TO8(  111 ),  /* im24 */
   DRX_16TO8(  103 ),  /* im25 */
   DRX_16TO8(  710 ),  /* im26 */
   DRX_16TO8(  994 )   /* im27 */
};

#endif /* DRXK_TAPS_BG_SELECTED */
#endif /* DRXK_TAPS_BG */



/* default DK, I, L and L' taps */
#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_DKILLP
#define  DRXK_TAPS_DKILLP
#endif
#endif

#ifdef   DRXK_TAPS_DKILLP
#ifndef  DRXK_TAPS_DKILLP_SELECTED
#define  DRXK_TAPS_DKILLP_SELECTED

#define  DRXK_TAPS_DKILLP_RE drxk_taps_dkillp_re_g
#define  DRXK_TAPS_DKILLP_IM drxk_taps_dkillp_im_g

static const u8_t drxk_taps_dkillp_re_g[]= {
   DRX_16TO8(  -23 ),  /* re0  */
   DRX_16TO8(    9 ),  /* re1  */
   DRX_16TO8(   16 ),  /* re2  */
   DRX_16TO8(  -26 ),  /* re3  */
   DRX_16TO8(   -3 ),  /* re4  */
   DRX_16TO8(   13 ),  /* re5  */
   DRX_16TO8(  -19 ),  /* re6  */
   DRX_16TO8(   -3 ),  /* re7  */
   DRX_16TO8(   13 ),  /* re8  */
   DRX_16TO8(  -26 ),  /* re9  */
   DRX_16TO8(   -4 ),  /* re10 */
   DRX_16TO8(   28 ),  /* re11 */
   DRX_16TO8(  -15 ),  /* re12 */
   DRX_16TO8(  -14 ),  /* re13 */
   DRX_16TO8(   10 ),  /* re14 */
   DRX_16TO8(    1 ),  /* re15 */
   DRX_16TO8(   39 ),  /* re16 */
   DRX_16TO8(  -18 ),  /* re17 */
   DRX_16TO8(  -90 ),  /* re18 */
   DRX_16TO8(  109 ),  /* re19 */
   DRX_16TO8(  113 ),  /* re20 */
   DRX_16TO8( -235 ),  /* re21 */
   DRX_16TO8(  -49 ),  /* re22 */
   DRX_16TO8(  359 ),  /* re23 */
   DRX_16TO8(  -79 ),  /* re24 */
   DRX_16TO8( -459 ),  /* re25 */
   DRX_16TO8(  206 ),  /* re26 */
   DRX_16TO8(  894 )   /* re27 */
};

static const u8_t drxk_taps_dkillp_im_g[]= {
   DRX_16TO8(   -8 ),  /* im0  */
   DRX_16TO8(  -20 ),  /* im1  */
   DRX_16TO8(   17 ),  /* im2  */
   DRX_16TO8(  -14 ),  /* im3  */
   DRX_16TO8(  -52 ),  /* im4  */
   DRX_16TO8(    4 ),  /* im5  */
   DRX_16TO8(    9 ),  /* im6  */
   DRX_16TO8(  -62 ),  /* im7  */
   DRX_16TO8(  -47 ),  /* im8  */
   DRX_16TO8(    0 ),  /* im9  */
   DRX_16TO8(  -20 ),  /* im10 */
   DRX_16TO8(  -48 ),  /* im11 */
   DRX_16TO8(  -65 ),  /* im12 */
   DRX_16TO8(  -23 ),  /* im13 */
   DRX_16TO8(   44 ),  /* im14 */
   DRX_16TO8(  -60 ),  /* im15 */
   DRX_16TO8( -113 ),  /* im16 */
   DRX_16TO8(   92 ),  /* im17 */
   DRX_16TO8(   81 ),  /* im18 */
   DRX_16TO8( -125 ),  /* im19 */
   DRX_16TO8(   28 ),  /* im20 */
   DRX_16TO8(  182 ),  /* im21 */
   DRX_16TO8(   35 ),  /* im22 */
   DRX_16TO8(   94 ),  /* im23 */
   DRX_16TO8(  180 ),  /* im24 */
   DRX_16TO8(  134 ),  /* im25 */
   DRX_16TO8(  657 ),  /* im26 */
   DRX_16TO8( 1023 )   /* im27 */
};

#endif /* DRXK_TAPS_DKILLP_SELECTED */
#endif /* DRXK_TAPS_DKILLP */



/* default DK China taps */
#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_DK_CN
#define  DRXK_TAPS_DK_CN
#endif
#endif

#ifdef   DRXK_TAPS_DK_CN
#ifndef  DRXK_TAPS_DK_CN_SELECTED
#define  DRXK_TAPS_DK_CN_SELECTED

#define  DRXK_TAPS_DK_CN_RE  drxk_taps_dk_cn_re_g
#define  DRXK_TAPS_DK_CN_IM  drxk_taps_dk_cn_im_g

static const u8_t drxk_taps_dk_cn_re_g[]= {
   DRX_16TO8(   10 ),  /* re0  */
   DRX_16TO8(    0 ),  /* re1  */
   DRX_16TO8(  -22 ),  /* re2  */
   DRX_16TO8(   33 ),  /* re3  */
   DRX_16TO8(    0 ),  /* re4  */
   DRX_16TO8(  -32 ),  /* re5  */
   DRX_16TO8(   21 ),  /* re6  */
   DRX_16TO8(   16 ),  /* re7  */
   DRX_16TO8(  -21 ),  /* re8  */
   DRX_16TO8(   -2 ),  /* re9  */
   DRX_16TO8(   -2 ),  /* re10 */
   DRX_16TO8(   17 ),  /* re11 */
   DRX_16TO8(    0 ),  /* re12 */
   DRX_16TO8(  -38 ),  /* re13 */
   DRX_16TO8(   27 ),  /* re14 */
   DRX_16TO8(   15 ),  /* re15 */
   DRX_16TO8(  -36 ),  /* re16 */
   DRX_16TO8(   37 ),  /* re17 */
   DRX_16TO8(  -38 ),  /* re18 */
   DRX_16TO8(  -30 ),  /* re19 */
   DRX_16TO8(  149 ),  /* re20 */
   DRX_16TO8( -102 ),  /* re21 */
   DRX_16TO8( -181 ),  /* re22 */
   DRX_16TO8(  310 ),  /* re23 */
   DRX_16TO8(   61 ),  /* re24 */
   DRX_16TO8( -480 ),  /* re25 */
   DRX_16TO8(  134 ),  /* re26 */
   DRX_16TO8(  900 )   /* re27 */
};

static const u8_t drxk_taps_dk_cn_im_g[]= {
   DRX_16TO8(  -18 ),  /* im0  */
   DRX_16TO8(   17 ),  /* im1  */
   DRX_16TO8(  -24 ),  /* im2  */
   DRX_16TO8(  -17 ),  /* im3  */
   DRX_16TO8(    7 ),  /* im4  */
   DRX_16TO8(  -36 ),  /* im5  */
   DRX_16TO8(  -24 ),  /* im6  */
   DRX_16TO8(    7 ),  /* im7  */
   DRX_16TO8(  -44 ),  /* im8  */
   DRX_16TO8(  -48 ),  /* im9  */
   DRX_16TO8(    2 ),  /* im10 */
   DRX_16TO8(  -22 ),  /* im11 */
   DRX_16TO8(  -51 ),  /* im12 */
   DRX_16TO8(  -56 ),  /* im13 */
   DRX_16TO8(    9 ),  /* im14 */
   DRX_16TO8(   34 ),  /* im15 */
   DRX_16TO8( -120 ),  /* im16 */
   DRX_16TO8(  -39 ),  /* im17 */
   DRX_16TO8(  171 ),  /* im18 */
   DRX_16TO8(  -59 ),  /* im19 */
   DRX_16TO8( -119 ),  /* im20 */
   DRX_16TO8(  207 ),  /* im21 */
   DRX_16TO8(  114 ),  /* im22 */
   DRX_16TO8(   -2 ),  /* im23 */
   DRX_16TO8(  179 ),  /* im24 */
   DRX_16TO8(  170 ),  /* im25 */
   DRX_16TO8(  587 ),  /* im26 */
   DRX_16TO8( 1018 )   /* im27 */
};

#endif /* DRXK_TAPS_DK_CN_SELECTED */
#endif /* DRXK_TAPS_DK_CN */



/* default FM taps */
#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_FM
#define  DRXK_TAPS_FM
#endif
#endif

#ifdef   DRXK_TAPS_FM
#ifndef  DRXK_TAPS_FM_SELECTED
#define  DRXK_TAPS_FM_SELECTED

#define  DRXK_TAPS_FM_RE drxk_taps_fm_re_g
#define  DRXK_TAPS_FM_IM drxk_taps_fm_im_g

static const u8_t drxk_taps_fm_re_g[]= {
   DRX_16TO8( 0 ),  /* re0  */
   DRX_16TO8( 0 ),  /* re1  */
   DRX_16TO8( 0 ),  /* re2  */
   DRX_16TO8( 0 ),  /* re3  */
   DRX_16TO8( 0 ),  /* re4  */
   DRX_16TO8( 0 ),  /* re5  */
   DRX_16TO8( 0 ),  /* re6  */
   DRX_16TO8( 0 ),  /* re7  */
   DRX_16TO8( 0 ),  /* re8  */
   DRX_16TO8( 0 ),  /* re9  */
   DRX_16TO8( 0 ),  /* re10 */
   DRX_16TO8( 0 ),  /* re11 */
   DRX_16TO8( 0 ),  /* re12 */
   DRX_16TO8( 0 ),  /* re13 */
   DRX_16TO8( 0 ),  /* re14 */
   DRX_16TO8( 0 ),  /* re15 */
   DRX_16TO8( 0 ),  /* re16 */
   DRX_16TO8( 0 ),  /* re17 */
   DRX_16TO8( 0 ),  /* re18 */
   DRX_16TO8( 0 ),  /* re19 */
   DRX_16TO8( 0 ),  /* re20 */
   DRX_16TO8( 0 ),  /* re21 */
   DRX_16TO8( 0 ),  /* re22 */
   DRX_16TO8( 0 ),  /* re23 */
   DRX_16TO8( 0 ),  /* re24 */
   DRX_16TO8( 0 ),  /* re25 */
   DRX_16TO8( 0 ),  /* re26 */
   DRX_16TO8( 0 )   /* re27 */
};

static const u8_t drxk_taps_fm_im_g[]= {
   DRX_16TO8(   -6 ),  /* im0  */
   DRX_16TO8(    2 ),  /* im1  */
   DRX_16TO8(   14 ),  /* im2  */
   DRX_16TO8(  -38 ),  /* im3  */
   DRX_16TO8(   58 ),  /* im4  */
   DRX_16TO8(  -62 ),  /* im5  */
   DRX_16TO8(   42 ),  /* im6  */
   DRX_16TO8(    0 ),  /* im7  */
   DRX_16TO8(  -45 ),  /* im8  */
   DRX_16TO8(   73 ),  /* im9  */
   DRX_16TO8(  -65 ),  /* im10 */
   DRX_16TO8(   23 ),  /* im11 */
   DRX_16TO8(   34 ),  /* im12 */
   DRX_16TO8(  -77 ),  /* im13 */
   DRX_16TO8(   80 ),  /* im14 */
   DRX_16TO8(  -39 ),  /* im15 */
   DRX_16TO8(  -25 ),  /* im16 */
   DRX_16TO8(   78 ),  /* im17 */
   DRX_16TO8(  -90 ),  /* im18 */
   DRX_16TO8(   52 ),  /* im19 */
   DRX_16TO8(   16 ),  /* im20 */
   DRX_16TO8(  -77 ),  /* im21 */
   DRX_16TO8(   97 ),  /* im22 */
   DRX_16TO8(  -62 ),  /* im23 */
   DRX_16TO8(   -8 ),  /* im24 */
   DRX_16TO8(   75 ),  /* im25 */
   DRX_16TO8( -100 ),  /* im26 */
   DRX_16TO8(   70 )   /* im27 */
};

#endif /* DRXK_TAPS_FM_SELECTED */
#endif /* DRXK_TAPS_FM */



/*------------------------------------------------------------------------------
HS TAPSETS
------------------------------------------------------------------------------*/

#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_BG_HS
#define  DRXK_TAPS_BG_HS
#endif
#endif

#ifdef   DRXK_TAPS_BG_HS
#ifndef  DRXK_TAPS_BG_HS_SELECTED
#define  DRXK_TAPS_BG_HS_SELECTED
#define  DRXK_TAPS_BG_HS_CFSCALE    (1200)

#define  DRXK_TAPS_BG_HS_RE drxk_taps_bg_hs_re_g
#define  DRXK_TAPS_BG_HS_IM drxk_taps_bg_hs_im_g

static const u8_t drxk_taps_bg_hs_re_g[]= {
   DRX_16TO8(   -2 ),  /* re00 */
   DRX_16TO8(   -1 ),  /* re01 */
   DRX_16TO8(    6 ),  /* re02 */
   DRX_16TO8(    2 ),  /* re03 */
   DRX_16TO8(  -11 ),  /* re04 */
   DRX_16TO8(    0 ),  /* re05 */
   DRX_16TO8(   19 ),  /* re06 */
   DRX_16TO8(   -4 ),  /* re07 */
   DRX_16TO8(  -29 ),  /* re08 */
   DRX_16TO8(   12 ),  /* re09 */
   DRX_16TO8(   38 ),  /* re10 */
   DRX_16TO8(  -25 ),  /* re11 */
   DRX_16TO8(  -45 ),  /* re12 */
   DRX_16TO8(   40 ),  /* re13 */
   DRX_16TO8(   41 ),  /* re14 */
   DRX_16TO8(  -58 ),  /* re15 */
   DRX_16TO8(  -30 ),  /* re16 */
   DRX_16TO8(   67 ),  /* re17 */
   DRX_16TO8(    3 ),  /* re18 */
   DRX_16TO8(  -65 ),  /* re19 */
   DRX_16TO8(   33 ),  /* re20 */
   DRX_16TO8(   44 ),  /* re21 */
   DRX_16TO8(  -70 ),  /* re22 */
   DRX_16TO8(   24 ),  /* re23 */
   DRX_16TO8(  137 ),  /* re24 */
   DRX_16TO8( -103 ),  /* re25 */
   DRX_16TO8( -144 ),  /* re26 */
   DRX_16TO8(  527 )   /* re27 */
};

static const u8_t drxk_taps_bg_hs_im_g[]= {
   DRX_16TO8(    1 ),  /* im00 */
   DRX_16TO8(   -2 ),  /* im01 */
   DRX_16TO8(   -3 ),  /* im02 */
   DRX_16TO8(    2 ),  /* im03 */
   DRX_16TO8(   -2 ),  /* im04 */
   DRX_16TO8(  -10 ),  /* im05 */
   DRX_16TO8(   -2 ),  /* im06 */
   DRX_16TO8(    5 ),  /* im07 */
   DRX_16TO8(  -19 ),  /* im08 */
   DRX_16TO8(  -24 ),  /* im09 */
   DRX_16TO8(   18 ),  /* im10 */
   DRX_16TO8(    4 ),  /* im11 */
   DRX_16TO8(  -63 ),  /* im12 */
   DRX_16TO8(  -21 ),  /* im13 */
   DRX_16TO8(   70 ),  /* im14 */
   DRX_16TO8(  -16 ),  /* im15 */
   DRX_16TO8( -121 ),  /* im16 */
   DRX_16TO8(   31 ),  /* im17 */
   DRX_16TO8(  152 ),  /* im18 */
   DRX_16TO8(  -58 ),  /* im19 */
   DRX_16TO8( -153 ),  /* im20 */
   DRX_16TO8(  150 ),  /* im21 */
   DRX_16TO8(  259 ),  /* im22 */
   DRX_16TO8(  -67 ),  /* im23 */
   DRX_16TO8( -117 ),  /* im24 */
   DRX_16TO8(  330 ),  /* im25 */
   DRX_16TO8(  573 ),  /* im26 */
   DRX_16TO8(  342 )   /* im27 */
};

#endif /* DRXK_TAPS_BG_HS_SELECTED */
#endif /* DRXK_TAPS_BG_HS */


#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_I_HS
#define  DRXK_TAPS_I_HS
#endif
#endif

#ifdef   DRXK_TAPS_I_HS
#ifndef  DRXK_TAPS_I_HS_SELECTED
#define  DRXK_TAPS_I_HS_SELECTED
#define  DRXK_TAPS_I_HS_CFSCALE    (1320)

#define  DRXK_TAPS_I_HS_RE drxk_taps_i_hs_re_g
#define  DRXK_TAPS_I_HS_IM drxk_taps_i_hs_im_g

static const u8_t drxk_taps_i_hs_re_g[]= {
   DRX_16TO8(    0 ),  /* re00 */
   DRX_16TO8(    2 ),  /* re01 */
   DRX_16TO8(   -2 ),  /* re02 */
   DRX_16TO8(   -4 ),  /* re03 */
   DRX_16TO8(    8 ),  /* re04 */
   DRX_16TO8(    2 ),  /* re05 */
   DRX_16TO8(  -17 ),  /* re06 */
   DRX_16TO8(    6 ),  /* re07 */
   DRX_16TO8(   24 ),  /* re08 */
   DRX_16TO8(  -25 ),  /* re09 */
   DRX_16TO8(  -24 ),  /* re10 */
   DRX_16TO8(   48 ),  /* re11 */
   DRX_16TO8(    7 ),  /* re12 */
   DRX_16TO8(  -68 ),  /* re13 */
   DRX_16TO8(   26 ),  /* re14 */
   DRX_16TO8(   66 ),  /* re15 */
   DRX_16TO8(  -64 ),  /* re16 */
   DRX_16TO8(  -37 ),  /* re17 */
   DRX_16TO8(   83 ),  /* re18 */
   DRX_16TO8(  -11 ),  /* re19 */
   DRX_16TO8(  -61 ),  /* re20 */
   DRX_16TO8(   54 ),  /* re21 */
   DRX_16TO8(   -1 ),  /* re22 */
   DRX_16TO8(  -39 ),  /* re23 */
   DRX_16TO8(  114 ),  /* re24 */
   DRX_16TO8(  -29 ),  /* re25 */
   DRX_16TO8( -165 ),  /* re26 */
   DRX_16TO8(  474 )   /* re27 */
};

static const u8_t drxk_taps_i_hs_im_g[]= {
   DRX_16TO8(   -2 ),  /* im00 */
   DRX_16TO8(    0 ),  /* im01 */
   DRX_16TO8(    5 ),  /* im02 */
   DRX_16TO8(   -1 ),  /* im03 */
   DRX_16TO8(   -6 ),  /* im04 */
   DRX_16TO8(    7 ),  /* im05 */
   DRX_16TO8(    3 ),  /* im06 */
   DRX_16TO8(  -15 ),  /* im07 */
   DRX_16TO8(    2 ),  /* im08 */
   DRX_16TO8(    8 ),  /* im09 */
   DRX_16TO8(  -30 ),  /* im10 */
   DRX_16TO8(  -12 ),  /* im11 */
   DRX_16TO8(   22 ),  /* im12 */
   DRX_16TO8(  -47 ),  /* im13 */
   DRX_16TO8(  -53 ),  /* im14 */
   DRX_16TO8(   54 ),  /* im15 */
   DRX_16TO8(  -18 ),  /* im16 */
   DRX_16TO8( -132 ),  /* im17 */
   DRX_16TO8(   46 ),  /* im18 */
   DRX_16TO8(  111 ),  /* im19 */
   DRX_16TO8( -147 ),  /* im20 */
   DRX_16TO8(  -73 ),  /* im21 */
   DRX_16TO8(  257 ),  /* im22 */
   DRX_16TO8(   74 ),  /* im23 */
   DRX_16TO8( -182 ),  /* im24 */
   DRX_16TO8(  202 ),  /* im25 */
   DRX_16TO8(  576 ),  /* im26 */
   DRX_16TO8(  377 )   /* im27 */
};

#endif /* DRXK_TAPS_I_HS_SELECTED */
#endif /* DRXK_TAPS_I_HS */





#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_NTSC_HS
#define  DRXK_TAPS_NTSC_HS
#endif
#endif

#ifdef   DRXK_TAPS_NTSC_HS
#ifndef  DRXK_TAPS_NTSC_HS_SELECTED
#define  DRXK_TAPS_NTSC_HS_SELECTED
#define  DRXK_TAPS_NTSC_HS_CFSCALE    (690)

#define  DRXK_TAPS_NTSC_HS_RE drxk_taps_ntsc_hs_re_g
#define  DRXK_TAPS_NTSC_HS_IM drxk_taps_ntsc_hs_im_g

static const u8_t drxk_taps_ntsc_hs_re_g[]= {
   DRX_16TO8(    1 ),  /* re00 */
   DRX_16TO8(    2 ),  /* re01 */
   DRX_16TO8(    0 ),  /* re02 */
   DRX_16TO8(   -5 ),  /* re03 */
   DRX_16TO8(   -3 ),  /* re04 */
   DRX_16TO8(    8 ),  /* re05 */
   DRX_16TO8(   13 ),  /* re06 */
   DRX_16TO8(   -4 ),  /* re07 */
   DRX_16TO8(  -22 ),  /* re08 */
   DRX_16TO8(   -5 ),  /* re09 */
   DRX_16TO8(   32 ),  /* re10 */
   DRX_16TO8(   29 ),  /* re11 */
   DRX_16TO8(  -25 ),  /* re12 */
   DRX_16TO8(  -45 ),  /* re13 */
   DRX_16TO8(   12 ),  /* re14 */
   DRX_16TO8(   60 ),  /* re15 */
   DRX_16TO8(   17 ),  /* re16 */
   DRX_16TO8(  -45 ),  /* re17 */
   DRX_16TO8(  -21 ),  /* re18 */
   DRX_16TO8(   36 ),  /* re19 */
   DRX_16TO8(   17 ),  /* re20 */
   DRX_16TO8(  -18 ),  /* re21 */
   DRX_16TO8(   44 ),  /* re22 */
   DRX_16TO8(   95 ),  /* re23 */
   DRX_16TO8(  -40 ),  /* re24 */
   DRX_16TO8( -176 ),  /* re25 */
   DRX_16TO8(   87 ),  /* re26 */
   DRX_16TO8(  693 )   /* re27 */
};

static const u8_t drxk_taps_ntsc_hs_im_g[]= {
   DRX_16TO8(   -1 ),  /* im00 */
   DRX_16TO8(   -3 ),  /* im01 */
   DRX_16TO8(   -2 ),  /* im02 */
   DRX_16TO8(   -1 ),  /* im03 */
   DRX_16TO8(   -2 ),  /* im04 */
   DRX_16TO8(   -5 ),  /* im05 */
   DRX_16TO8(   -4 ),  /* im06 */
   DRX_16TO8(   -5 ),  /* im07 */
   DRX_16TO8(  -11 ),  /* im08 */
   DRX_16TO8(  -13 ),  /* im09 */
   DRX_16TO8(    5 ),  /* im10 */
   DRX_16TO8(   18 ),  /* im11 */
   DRX_16TO8(  -12 ),  /* im12 */
   DRX_16TO8(  -53 ),  /* im13 */
   DRX_16TO8(  -18 ),  /* im14 */
   DRX_16TO8(   77 ),  /* im15 */
   DRX_16TO8(   81 ),  /* im16 */
   DRX_16TO8(  -50 ),  /* im17 */
   DRX_16TO8( -116 ),  /* im18 */
   DRX_16TO8(   47 ),  /* im19 */
   DRX_16TO8(  246 ),  /* im20 */
   DRX_16TO8(  172 ),  /* im21 */
   DRX_16TO8(  -90 ),  /* im22 */
   DRX_16TO8( -121 ),  /* im23 */
   DRX_16TO8(  212 ),  /* im24 */
   DRX_16TO8(  548 ),  /* im25 */
   DRX_16TO8(  547 ),  /* im26 */
   DRX_16TO8(  290 )   /* im27 */
};

#endif /* DRXK_TAPS_NTSC_HS_SELECTED */
#endif /* DRXK_TAPS_NTSC_HS */


#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_DKLLP_HS
#define  DRXK_TAPS_DKLLP_HS
#endif
#endif

#ifdef   DRXK_TAPS_DKLLP_HS
#ifndef  DRXK_TAPS_DKLLP_HS_SELECTED
#define  DRXK_TAPS_DKLLP_HS_SELECTED
#define  DRXK_TAPS_DKLLP_HS_CFSCALE    (1240)

#define  DRXK_TAPS_DKLLP_HS_RE drxk_taps_dkllp_hs_re_g
#define  DRXK_TAPS_DKLLP_HS_IM drxk_taps_dkllp_hs_im_g

static const u8_t drxk_taps_dkllp_hs_re_g[]= {
   DRX_16TO8(    1 ),  /* re00 */
   DRX_16TO8(    0 ),  /* re01 */
   DRX_16TO8(   -3 ),  /* re02 */
   DRX_16TO8(    3 ),  /* re03 */
   DRX_16TO8(    3 ),  /* re04 */
   DRX_16TO8(  -10 ),  /* re05 */
   DRX_16TO8(    1 ),  /* re06 */
   DRX_16TO8(   15 ),  /* re07 */
   DRX_16TO8(   -9 ),  /* re08 */
   DRX_16TO8(  -13 ),  /* re09 */
   DRX_16TO8(   16 ),  /* re10 */
   DRX_16TO8(    8 ),  /* re11 */
   DRX_16TO8(   -9 ),  /* re12 */
   DRX_16TO8(  -10 ),  /* re13 */
   DRX_16TO8(    2 ),  /* re14 */
   DRX_16TO8(   34 ),  /* re15 */
   DRX_16TO8(  -11 ),  /* re16 */
   DRX_16TO8(  -56 ),  /* re17 */
   DRX_16TO8(   53 ),  /* re18 */
   DRX_16TO8(   49 ),  /* re19 */
   DRX_16TO8(  -90 ),  /* re20 */
   DRX_16TO8(    6 ),  /* re21 */
   DRX_16TO8(   75 ),  /* re22 */
   DRX_16TO8(  -51 ),  /* re23 */
   DRX_16TO8(   22 ),  /* re24 */
   DRX_16TO8(  -10 ),  /* re25 */
   DRX_16TO8( -142 ),  /* re26 */
   DRX_16TO8(  454 )   /* re27 */
};

static const u8_t drxk_taps_dkllp_hs_im_g[]= {
   DRX_16TO8(   -1 ),  /* im00 */
   DRX_16TO8(    2 ),  /* im01 */
   DRX_16TO8(    0 ),  /* im02 */
   DRX_16TO8(   -5 ),  /* im03 */
   DRX_16TO8(    1 ),  /* im04 */
   DRX_16TO8(    1 ),  /* im05 */
   DRX_16TO8(  -10 ),  /* im06 */
   DRX_16TO8(   -1 ),  /* im07 */
   DRX_16TO8(   -1 ),  /* im08 */
   DRX_16TO8(  -18 ),  /* im09 */
   DRX_16TO8(   -1 ),  /* im10 */
   DRX_16TO8(    3 ),  /* im11 */
   DRX_16TO8(  -28 ),  /* im12 */
   DRX_16TO8(  -12 ),  /* im13 */
   DRX_16TO8(    7 ),  /* im14 */
   DRX_16TO8(  -10 ),  /* im15 */
   DRX_16TO8(  -12 ),  /* im16 */
   DRX_16TO8(  -27 ),  /* im17 */
   DRX_16TO8(   17 ),  /* im18 */
   DRX_16TO8(   78 ),  /* im19 */
   DRX_16TO8(  -61 ),  /* im20 */
   DRX_16TO8(  -56 ),  /* im21 */
   DRX_16TO8(  236 ),  /* im22 */
   DRX_16TO8(   98 ),  /* im23 */
   DRX_16TO8( -189 ),  /* im24 */
   DRX_16TO8(  244 ),  /* im25 */
   DRX_16TO8(  633 ),  /* im26 */
   DRX_16TO8(  327 )   /* im27 */
};

#endif /* DRXK_TAPS_DKLLP_HS_SELECTED */
#endif /* DRXK_TAPS_DKLLP_HS */



/*------------------------------------------------------------------------------
SSK TAPSETS
------------------------------------------------------------------------------*/

#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_SSK_BG
#define  DRXK_TAPS_SSK_BG
#endif
#endif

#ifdef   DRXK_TAPS_SSK_BG
#ifndef  DRXK_TAPS_SSK_BG_SELECTED
#define  DRXK_TAPS_SSK_BG_SELECTED

#define  DRXK_TAPS_SSK_BG_CFSCALE     (1200)

#define  DRXK_TAPS_SSK_BG_RE drxk_taps_ssk_bg_re_g
#define  DRXK_TAPS_SSK_BG_IM drxk_taps_ssk_bg_im_g

static const u8_t drxk_taps_ssk_bg_re_g[]= {
   DRX_16TO8(   -6 ),  /* re00 */
   DRX_16TO8(   -4 ),  /* re01 */
   DRX_16TO8(    5 ),  /* re02 */
   DRX_16TO8(    8 ),  /* re03 */
   DRX_16TO8(  -12 ),  /* re04 */
   DRX_16TO8(   -6 ),  /* re05 */
   DRX_16TO8(   23 ),  /* re06 */
   DRX_16TO8(    1 ),  /* re07 */
   DRX_16TO8(  -38 ),  /* re08 */
   DRX_16TO8(    3 ),  /* re09 */
   DRX_16TO8(   40 ),  /* re10 */
   DRX_16TO8(  -29 ),  /* re11 */
   DRX_16TO8(  -58 ),  /* re12 */
   DRX_16TO8(   36 ),  /* re13 */
   DRX_16TO8(   45 ),  /* re14 */
   DRX_16TO8(  -63 ),  /* re15 */
   DRX_16TO8(  -40 ),  /* re16 */
   DRX_16TO8(   71 ),  /* re17 */
   DRX_16TO8(   19 ),  /* re18 */
   DRX_16TO8(  -65 ),  /* re19 */
   DRX_16TO8(   15 ),  /* re20 */
   DRX_16TO8(   50 ),  /* re21 */
   DRX_16TO8(  -32 ),  /* re22 */
   DRX_16TO8(   34 ),  /* re23 */
   DRX_16TO8(  108 ),  /* re24 */
   DRX_16TO8(  -86 ),  /* re25 */
   DRX_16TO8(  -64 ),  /* re26 */
   DRX_16TO8(  573 )   /* re27 */
};

static const u8_t drxk_taps_ssk_bg_im_g[]= {
   DRX_16TO8(   -4 ),  /* im00 */
   DRX_16TO8(   -2 ),  /* im01 */
   DRX_16TO8(   -1 ),  /* im02 */
   DRX_16TO8(    6 ),  /* im03 */
   DRX_16TO8(    3 ),  /* im04 */
   DRX_16TO8(  -18 ),  /* im05 */
   DRX_16TO8(   -9 ),  /* im06 */
   DRX_16TO8(   10 ),  /* im07 */
   DRX_16TO8(  -23 ),  /* im08 */
   DRX_16TO8(  -47 ),  /* im09 */
   DRX_16TO8(    5 ),  /* im10 */
   DRX_16TO8(   13 ),  /* im11 */
   DRX_16TO8(  -66 ),  /* im12 */
   DRX_16TO8(  -49 ),  /* im13 */
   DRX_16TO8(   52 ),  /* im14 */
   DRX_16TO8(  -10 ),  /* im15 */
   DRX_16TO8( -123 ),  /* im16 */
   DRX_16TO8(   -1 ),  /* im17 */
   DRX_16TO8(  118 ),  /* im18 */
   DRX_16TO8(  -58 ),  /* im19 */
   DRX_16TO8( -148 ),  /* im20 */
   DRX_16TO8(  121 ),  /* im21 */
   DRX_16TO8(  228 ),  /* im22 */
   DRX_16TO8(  -53 ),  /* im23 */
   DRX_16TO8(  -96 ),  /* im24 */
   DRX_16TO8(  321 ),  /* im25 */
   DRX_16TO8(  579 ),  /* im26 */
   DRX_16TO8(  376 )   /* im27 */
};

#endif /* DRXK_TAPS_SSK_BG_SELECTED */
#endif /* DRXK_TAPS_SSK_BG */




#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_SSK_DKIL
#define  DRXK_TAPS_SSK_DKIL
#endif
#endif

#ifdef   DRXK_TAPS_SSK_DKIL
#ifndef  DRXK_TAPS_SSK_DKIL_SELECTED
#define  DRXK_TAPS_SSK_DKIL_SELECTED

#define  DRXK_TAPS_SSK_DKIL_CFSCALE     (1240)

#define  DRXK_TAPS_SSK_DKIL_RE drxk_taps_ssk_dkil_re_g
#define  DRXK_TAPS_SSK_DKIL_IM drxk_taps_ssk_dkil_im_g

static const u8_t drxk_taps_ssk_dkil_re_g[]= {
   DRX_16TO8(    1 ),  /* re00 */
   DRX_16TO8(    0 ),  /* re01 */
   DRX_16TO8(   -1 ),  /* re02 */
   DRX_16TO8(    7 ),  /* re03 */
   DRX_16TO8(   -1 ),  /* re04 */
   DRX_16TO8(  -12 ),  /* re05 */
   DRX_16TO8(   11 ),  /* re06 */
   DRX_16TO8(   10 ),  /* re07 */
   DRX_16TO8(  -21 ),  /* re08 */
   DRX_16TO8(   -6 ),  /* re09 */
   DRX_16TO8(    9 ),  /* re10 */
   DRX_16TO8(   -2 ),  /* re11 */
   DRX_16TO8(    1 ),  /* re12 */
   DRX_16TO8(  -22 ),  /* re13 */
   DRX_16TO8(   -4 ),  /* re14 */
   DRX_16TO8(   53 ),  /* re15 */
   DRX_16TO8(  -42 ),  /* re16 */
   DRX_16TO8(  -75 ),  /* re17 */
   DRX_16TO8(  104 ),  /* re18 */
   DRX_16TO8(   29 ),  /* re19 */
   DRX_16TO8( -138 ),  /* re20 */
   DRX_16TO8(   77 ),  /* re21 */
   DRX_16TO8(  106 ),  /* re22 */
   DRX_16TO8( -116 ),  /* re23 */
   DRX_16TO8(   82 ),  /* re24 */
   DRX_16TO8(  111 ),  /* re25 */
   DRX_16TO8( -167 ),  /* re26 */
   DRX_16TO8(  398 )   /* re27 */
};

static const u8_t drxk_taps_ssk_dkil_im_g[]= {
   DRX_16TO8(    0 ),  /* im00 */
   DRX_16TO8(    1 ),  /* im01 */
   DRX_16TO8(   -4 ),  /* im02 */
   DRX_16TO8(   -3 ),  /* im03 */
   DRX_16TO8(    8 ),  /* im04 */
   DRX_16TO8(   -6 ),  /* im05 */
   DRX_16TO8(  -16 ),  /* im06 */
   DRX_16TO8(    4 ),  /* im07 */
   DRX_16TO8(   -9 ),  /* im08 */
   DRX_16TO8(  -25 ),  /* im09 */
   DRX_16TO8(    0 ),  /* im10 */
   DRX_16TO8(  -12 ),  /* im11 */
   DRX_16TO8(  -28 ),  /* im12 */
   DRX_16TO8(  -10 ),  /* im13 */
   DRX_16TO8(  -35 ),  /* im14 */
   DRX_16TO8(  -17 ),  /* im15 */
   DRX_16TO8(    9 ),  /* im16 */
   DRX_16TO8(  -80 ),  /* im17 */
   DRX_16TO8(   -8 ),  /* im18 */
   DRX_16TO8(  121 ),  /* im19 */
   DRX_16TO8( -102 ),  /* im20 */
   DRX_16TO8( -105 ),  /* im21 */
   DRX_16TO8(  286 ),  /* im22 */
   DRX_16TO8(   89 ),  /* im23 */
   DRX_16TO8( -251 ),  /* im24 */
   DRX_16TO8(  269 ),  /* im25 */
   DRX_16TO8(  601 ),  /* im26 */
   DRX_16TO8(  182 )   /* im27 */
};

#endif /* DRXK_TAPS_SSK_DKIL_SELECTED */
#endif /* DRXK_TAPS_SSK_DKIL */



#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_SSK_LP
#define  DRXK_TAPS_SSK_LP
#endif
#endif

#ifdef   DRXK_TAPS_SSK_LP
#ifndef  DRXK_TAPS_SSK_LP_SELECTED
#define  DRXK_TAPS_SSK_LP_SELECTED

#define  DRXK_TAPS_SSK_LP_CFSCALE     (1240)

#define  DRXK_TAPS_SSK_LP_RE drxk_taps_ssk_lp_re_g
#define  DRXK_TAPS_SSK_LP_IM drxk_taps_ssk_lp_im_g

static const u8_t drxk_taps_ssk_lp_re_g[]= {
   DRX_16TO8(    3 ),  /* re00 */
   DRX_16TO8(    6 ),  /* re01 */
   DRX_16TO8(   -3 ),  /* re02 */
   DRX_16TO8(    1 ),  /* re03 */
   DRX_16TO8(    3 ),  /* re04 */
   DRX_16TO8(    3 ),  /* re05 */
   DRX_16TO8(   -4 ),  /* re06 */
   DRX_16TO8(    1 ),  /* re07 */
   DRX_16TO8(   13 ),  /* re08 */
   DRX_16TO8(  -12 ),  /* re09 */
   DRX_16TO8(  -22 ),  /* re10 */
   DRX_16TO8(   35 ),  /* re11 */
   DRX_16TO8(   11 ),  /* re12 */
   DRX_16TO8(  -65 ),  /* re13 */
   DRX_16TO8(   27 ),  /* re14 */
   DRX_16TO8(   85 ),  /* re15 */
   DRX_16TO8(  -78 ),  /* re16 */
   DRX_16TO8(  -63 ),  /* re17 */
   DRX_16TO8(  123 ),  /* re18 */
   DRX_16TO8(    6 ),  /* re19 */
   DRX_16TO8( -131 ),  /* re20 */
   DRX_16TO8(   46 ),  /* re21 */
   DRX_16TO8(   64 ),  /* re22 */
   DRX_16TO8(  -60 ),  /* re23 */
   DRX_16TO8(   47 ),  /* re24 */
   DRX_16TO8(  -22 ),  /* re25 */
   DRX_16TO8( -135 ),  /* re26 */
   DRX_16TO8(  479 )   /* re27 */
};

static const u8_t drxk_taps_ssk_lp_im_g[]= {
   DRX_16TO8(   -8 ),  /* im00 */
   DRX_16TO8(   -2 ),  /* im01 */
   DRX_16TO8(   -9 ),  /* im02 */
   DRX_16TO8(  -11 ),  /* im03 */
   DRX_16TO8(   -9 ),  /* im04 */
   DRX_16TO8(   -5 ),  /* im05 */
   DRX_16TO8(   -8 ),  /* im06 */
   DRX_16TO8(  -29 ),  /* im07 */
   DRX_16TO8(  -16 ),  /* im08 */
   DRX_16TO8(    8 ),  /* im09 */
   DRX_16TO8(  -27 ),  /* im10 */
   DRX_16TO8(  -39 ),  /* im11 */
   DRX_16TO8(   23 ),  /* im12 */
   DRX_16TO8(   -2 ),  /* im13 */
   DRX_16TO8(  -59 ),  /* im14 */
   DRX_16TO8(   27 ),  /* im15 */
   DRX_16TO8(   30 ),  /* im16 */
   DRX_16TO8(  -84 ),  /* im17 */
   DRX_16TO8(   36 ),  /* im18 */
   DRX_16TO8(  124 ),  /* im19 */
   DRX_16TO8( -101 ),  /* im20 */
   DRX_16TO8(  -32 ),  /* im21 */
   DRX_16TO8(  294 ),  /* im22 */
   DRX_16TO8(   81 ),  /* im23 */
   DRX_16TO8( -180 ),  /* im24 */
   DRX_16TO8(  273 ),  /* im25 */
   DRX_16TO8(  631 ),  /* im26 */
   DRX_16TO8(  345 )   /* im27 */
};

#endif /* DRXK_TAPS_SSK_LP_SELECTED */
#endif /* DRXK_TAPS_SSK_LP */



#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_SSK_NTSC
#define  DRXK_TAPS_SSK_NTSC
#endif
#endif

#ifdef   DRXK_TAPS_SSK_NTSC
#ifndef  DRXK_TAPS_SSK_NTSC_SELECTED
#define  DRXK_TAPS_SSK_NTSC_SELECTED

#define  DRXK_TAPS_SSK_NTSC_CFSCALE   (690)

#define  DRXK_TAPS_SSK_NTSC_RE drxk_taps_ssk_ntsc_re_g
#define  DRXK_TAPS_SSK_NTSC_IM drxk_taps_ssk_ntsc_im_g

static const u8_t drxk_taps_ssk_ntsc_re_g[]= {
   DRX_16TO8(  -12 ),  /* re00 */
   DRX_16TO8(   10 ),  /* re01 */
   DRX_16TO8(   20 ),  /* re02 */
   DRX_16TO8(  -10 ),  /* re03 */
   DRX_16TO8(  -28 ),  /* re04 */
   DRX_16TO8(   11 ),  /* re05 */
   DRX_16TO8(   44 ),  /* re06 */
   DRX_16TO8(   -2 ),  /* re07 */
   DRX_16TO8(  -65 ),  /* re08 */
   DRX_16TO8(  -33 ),  /* re09 */
   DRX_16TO8(   54 ),  /* re10 */
   DRX_16TO8(   57 ),  /* re11 */
   DRX_16TO8(  -29 ),  /* re12 */
   DRX_16TO8(  -56 ),  /* re13 */
   DRX_16TO8(   21 ),  /* re14 */
   DRX_16TO8(   65 ),  /* re15 */
   DRX_16TO8(   -2 ),  /* re16 */
   DRX_16TO8(  -58 ),  /* re17 */
   DRX_16TO8(   -8 ),  /* re18 */
   DRX_16TO8(   42 ),  /* re19 */
   DRX_16TO8(   -6 ),  /* re20 */
   DRX_16TO8(  -41 ),  /* re21 */
   DRX_16TO8(   44 ),  /* re22 */
   DRX_16TO8(   84 ),  /* re23 */
   DRX_16TO8(  -92 ),  /* re24 */
   DRX_16TO8( -244 ),  /* re25 */
   DRX_16TO8(   44 ),  /* re26 */
   DRX_16TO8(  681 )   /* re27 */
};

static const u8_t drxk_taps_ssk_ntsc_im_g[]= {
   DRX_16TO8(  -21 ),  /* im00 */
   DRX_16TO8(   -9 ),  /* im01 */
   DRX_16TO8(   -1 ),  /* im02 */
   DRX_16TO8(  -10 ),  /* im03 */
   DRX_16TO8(  -36 ),  /* im04 */
   DRX_16TO8(  -36 ),  /* im05 */
   DRX_16TO8(    2 ),  /* im06 */
   DRX_16TO8(   15 ),  /* im07 */
   DRX_16TO8(  -37 ),  /* im08 */
   DRX_16TO8(  -72 ),  /* im09 */
   DRX_16TO8(   -8 ),  /* im10 */
   DRX_16TO8(   65 ),  /* im11 */
   DRX_16TO8(    8 ),  /* im12 */
   DRX_16TO8( -115 ),  /* im13 */
   DRX_16TO8(  -95 ),  /* im14 */
   DRX_16TO8(   68 ),  /* im15 */
   DRX_16TO8(  123 ),  /* im16 */
   DRX_16TO8(  -36 ),  /* im17 */
   DRX_16TO8( -152 ),  /* im18 */
   DRX_16TO8(    2 ),  /* im19 */
   DRX_16TO8(  233 ),  /* im20 */
   DRX_16TO8(  191 ),  /* im21 */
   DRX_16TO8(  -72 ),  /* im22 */
   DRX_16TO8( -145 ),  /* im23 */
   DRX_16TO8(  151 ),  /* im24 */
   DRX_16TO8(  519 ),  /* im25 */
   DRX_16TO8(  602 ),  /* im26 */
   DRX_16TO8(  370 )   /* im27 */
};

#endif /* DRXK_TAPS_SSK_NTSC_SELECTED */
#endif /* DRXK_TAPS_SSK_NTSC */



/*------------------------------------------------------------------------------
SSR111 TAPSETS
------------------------------------------------------------------------------*/

#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_SSR111_BG
#define  DRXK_TAPS_SSR111_BG
#endif
#endif

#ifdef   DRXK_TAPS_SSR111_BG
#ifndef  DRXK_TAPS_SSR111_BG_SELECTED
#define  DRXK_TAPS_SSR111_BG_SELECTED

#define  DRXK_TAPS_SSR111_BG_RE drxk_taps_ssr111_bg_re_g
#define  DRXK_TAPS_SSR111_BG_IM drxk_taps_ssr111_bg_im_g

static const u8_t drxk_taps_ssr111_bg_re_g[]= {
   DRX_16TO8(   -5 ),  /* re00 */
   DRX_16TO8(   -1 ),  /* re01 */
   DRX_16TO8(   10 ),  /* re02 */
   DRX_16TO8(   -4 ),  /* re03 */
   DRX_16TO8(  -13 ),  /* re04 */
   DRX_16TO8(   12 ),  /* re05 */
   DRX_16TO8(   13 ),  /* re06 */
   DRX_16TO8(  -19 ),  /* re07 */
   DRX_16TO8(   -7 ),  /* re08 */
   DRX_16TO8(   18 ),  /* re09 */
   DRX_16TO8(    3 ),  /* re10 */
   DRX_16TO8(   -6 ),  /* re11 */
   DRX_16TO8(   -9 ),  /* re12 */
   DRX_16TO8(   -8 ),  /* re13 */
   DRX_16TO8(   35 ),  /* re14 */
   DRX_16TO8(   12 ),  /* re15 */
   DRX_16TO8(  -71 ),  /* re16 */
   DRX_16TO8(   15 ),  /* re17 */
   DRX_16TO8(   97 ),  /* re18 */
   DRX_16TO8(  -63 ),  /* re19 */
   DRX_16TO8(  -76 ),  /* re20 */
   DRX_16TO8(  116 ),  /* re21 */
   DRX_16TO8(    9 ),  /* re22 */
   DRX_16TO8( -103 ),  /* re23 */
   DRX_16TO8(  118 ),  /* re24 */
   DRX_16TO8(   18 ),  /* re25 */
   DRX_16TO8( -199 ),  /* re26 */
   DRX_16TO8(  431 )   /* re27 */
};

static const u8_t drxk_taps_ssr111_bg_im_g[]= {
   DRX_16TO8(    3 ),  /* im00 */
   DRX_16TO8(   -5 ),  /* im01 */
   DRX_16TO8(    1 ),  /* im02 */
   DRX_16TO8(   11 ),  /* im03 */
   DRX_16TO8(   -4 ),  /* im04 */
   DRX_16TO8(   -5 ),  /* im05 */
   DRX_16TO8(   19 ),  /* im06 */
   DRX_16TO8(    5 ),  /* im07 */
   DRX_16TO8(  -14 ),  /* im08 */
   DRX_16TO8(   19 ),  /* im09 */
   DRX_16TO8(   26 ),  /* im10 */
   DRX_16TO8(   -7 ),  /* im11 */
   DRX_16TO8(    6 ),  /* im12 */
   DRX_16TO8(   27 ),  /* im13 */
   DRX_16TO8(   26 ),  /* im14 */
   DRX_16TO8(   37 ),  /* im15 */
   DRX_16TO8(   -4 ),  /* im16 */
   DRX_16TO8(   -5 ),  /* im17 */
   DRX_16TO8(  133 ),  /* im18 */
   DRX_16TO8(   82 ),  /* im19 */
   DRX_16TO8( -128 ),  /* im20 */
   DRX_16TO8(   80 ),  /* im21 */
   DRX_16TO8(  345 ),  /* im22 */
   DRX_16TO8(  -15 ),  /* im23 */
   DRX_16TO8( -191 ),  /* im24 */
   DRX_16TO8(  382 ),  /* im25 */
   DRX_16TO8(  606 ),  /* im26 */
   DRX_16TO8(  191 )   /* im27 */
};

#endif /* DRXK_TAPS_SSR111_BG_SELECTED */
#endif /* DRXK_TAPS_SSR111_BG */

#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_SSR111_DKI
#define  DRXK_TAPS_SSR111_DKI
#endif
#endif

#ifdef   DRXK_TAPS_SSR111_DKI
#ifndef  DRXK_TAPS_SSR111_DKI_SELECTED
#define  DRXK_TAPS_SSR111_DKI_SELECTED

#define  DRXK_TAPS_SSR111_DKI_RE drxk_taps_ssr111_dki_re_g
#define  DRXK_TAPS_SSR111_DKI_IM drxk_taps_ssr111_dki_im_g

static const u8_t drxk_taps_ssr111_dki_re_g[]= {
   DRX_16TO8(   -4 ),  /* re00 */
   DRX_16TO8(   -1 ),  /* re01 */
   DRX_16TO8(    8 ),  /* re02 */
   DRX_16TO8(   -3 ),  /* re03 */
   DRX_16TO8(  -11 ),  /* re04 */
   DRX_16TO8(   10 ),  /* re05 */
   DRX_16TO8(    8 ),  /* re06 */
   DRX_16TO8(  -15 ),  /* re07 */
   DRX_16TO8(    0 ),  /* re08 */
   DRX_16TO8(   13 ),  /* re09 */
   DRX_16TO8(   -8 ),  /* re10 */
   DRX_16TO8(   -1 ),  /* re11 */
   DRX_16TO8(    6 ),  /* re12 */
   DRX_16TO8(  -13 ),  /* re13 */
   DRX_16TO8(   14 ),  /* re14 */
   DRX_16TO8(   17 ),  /* re15 */
   DRX_16TO8(  -43 ),  /* re16 */
   DRX_16TO8(    9 ),  /* re17 */
   DRX_16TO8(   62 ),  /* re18 */
   DRX_16TO8(  -55 ),  /* re19 */
   DRX_16TO8(  -37 ),  /* re20 */
   DRX_16TO8(  107 ),  /* re21 */
   DRX_16TO8(  -31 ),  /* re22 */
   DRX_16TO8(  -98 ),  /* re23 */
   DRX_16TO8(  154 ),  /* re24 */
   DRX_16TO8(   16 ),  /* re25 */
   DRX_16TO8( -232 ),  /* re26 */
   DRX_16TO8(  414 )   /* re27 */
};

static const u8_t drxk_taps_ssr111_dki_im_g[]= {
   DRX_16TO8(    2 ),  /* im00 */
   DRX_16TO8(   -6 ),  /* im01 */
   DRX_16TO8(    1 ),  /* im02 */
   DRX_16TO8(   11 ),  /* im03 */
   DRX_16TO8(   -7 ),  /* im04 */
   DRX_16TO8(   -9 ),  /* im05 */
   DRX_16TO8(   18 ),  /* im06 */
   DRX_16TO8(    9 ),  /* im07 */
   DRX_16TO8(  -19 ),  /* im08 */
   DRX_16TO8(    7 ),  /* im09 */
   DRX_16TO8(   26 ),  /* im10 */
   DRX_16TO8(    2 ),  /* im11 */
   DRX_16TO8(   -3 ),  /* im12 */
   DRX_16TO8(    5 ),  /* im13 */
   DRX_16TO8(   26 ),  /* im14 */
   DRX_16TO8(   53 ),  /* im15 */
   DRX_16TO8(  -13 ),  /* im16 */
   DRX_16TO8(  -37 ),  /* im17 */
   DRX_16TO8(  128 ),  /* im18 */
   DRX_16TO8(  108 ),  /* im19 */
   DRX_16TO8( -131 ),  /* im20 */
   DRX_16TO8(   36 ),  /* im21 */
   DRX_16TO8(  330 ),  /* im22 */
   DRX_16TO8(   24 ),  /* im23 */
   DRX_16TO8( -185 ),  /* im24 */
   DRX_16TO8(  322 ),  /* im25 */
   DRX_16TO8(  580 ),  /* im26 */
   DRX_16TO8(  226 )   /* im27 */
};

#endif /* DRXK_TAPS_SSR111_DKI_SELECTED */
#endif /* DRXK_TAPS_SSR111_DKI */

#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_SSR111_LLP
#define  DRXK_TAPS_SSR111_LLP
#endif
#endif

#ifdef   DRXK_TAPS_SSR111_LLP
#ifndef  DRXK_TAPS_SSR111_LLP_SELECTED
#define  DRXK_TAPS_SSR111_LLP_SELECTED

#define  DRXK_TAPS_SSR111_LLP_RE drxk_taps_ssr111_llp_re_g
#define  DRXK_TAPS_SSR111_LLP_IM drxk_taps_ssr111_llp_im_g

static const u8_t drxk_taps_ssr111_llp_re_g[]= {
   DRX_16TO8(   -4 ),  /* re00 */
   DRX_16TO8(   -1 ),  /* re01 */
   DRX_16TO8(    7 ),  /* re02 */
   DRX_16TO8(   -4 ),  /* re03 */
   DRX_16TO8(   -8 ),  /* re04 */
   DRX_16TO8(   11 ),  /* re05 */
   DRX_16TO8(    5 ),  /* re06 */
   DRX_16TO8(  -17 ),  /* re07 */
   DRX_16TO8(    5 ),  /* re08 */
   DRX_16TO8(   16 ),  /* re09 */
   DRX_16TO8(  -15 ),  /* re10 */
   DRX_16TO8(   -5 ),  /* re11 */
   DRX_16TO8(   16 ),  /* re12 */
   DRX_16TO8(   -8 ),  /* re13 */
   DRX_16TO8(    1 ),  /* re14 */
   DRX_16TO8(   11 ),  /* re15 */
   DRX_16TO8(  -25 ),  /* re16 */
   DRX_16TO8(   14 ),  /* re17 */
   DRX_16TO8(   40 ),  /* re18 */
   DRX_16TO8(  -59 ),  /* re19 */
   DRX_16TO8(  -13 ),  /* re20 */
   DRX_16TO8(  109 ),  /* re21 */
   DRX_16TO8(  -57 ),  /* re22 */
   DRX_16TO8( -100 ),  /* re23 */
   DRX_16TO8(  178 ),  /* re24 */
   DRX_16TO8(   20 ),  /* re25 */
   DRX_16TO8( -253 ),  /* re26 */
   DRX_16TO8(  401 )   /* re27 */
};

static const u8_t drxk_taps_ssr111_llp_im_g[]= {
   DRX_16TO8(    2 ),  /* im00 */
   DRX_16TO8(   -6 ),  /* im01 */
   DRX_16TO8(    1 ),  /* im02 */
   DRX_16TO8(   13 ),  /* im03 */
   DRX_16TO8(   -4 ),  /* im04 */
   DRX_16TO8(  -10 ),  /* im05 */
   DRX_16TO8(   18 ),  /* im06 */
   DRX_16TO8(   14 ),  /* im07 */
   DRX_16TO8(  -13 ),  /* im08 */
   DRX_16TO8(    3 ),  /* im09 */
   DRX_16TO8(   25 ),  /* im10 */
   DRX_16TO8(   14 ),  /* im11 */
   DRX_16TO8(    6 ),  /* im12 */
   DRX_16TO8(   -3 ),  /* im13 */
   DRX_16TO8(   24 ),  /* im14 */
   DRX_16TO8(   71 ),  /* im15 */
   DRX_16TO8(   -3 ),  /* im16 */
   DRX_16TO8(  -50 ),  /* im17 */
   DRX_16TO8(  123 ),  /* im18 */
   DRX_16TO8(  130 ),  /* im19 */
   DRX_16TO8( -118 ),  /* im20 */
   DRX_16TO8(   13 ),  /* im21 */
   DRX_16TO8(  319 ),  /* im22 */
   DRX_16TO8(   54 ),  /* im23 */
   DRX_16TO8( -173 ),  /* im24 */
   DRX_16TO8(  287 ),  /* im25 */
   DRX_16TO8(  563 ),  /* im26 */
   DRX_16TO8(  249 )   /* im27 */
};

#endif /* DRXK_TAPS_SSR111_LLP_SELECTED */
#endif /* DRXK_TAPS_SSR111_LLP */



/*------------------------------------------------------------------------------
MISC TAPSETS
------------------------------------------------------------------------------*/

#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_BG_IS38
#define  DRXK_TAPS_BG_IS38
#endif
#endif

#ifdef   DRXK_TAPS_BG_IS38
#ifndef  DRXK_TAPS_BG_IS38_SELECTED
#define  DRXK_TAPS_BG_IS38_SELECTED

#define  DRXK_TAPS_BG_IS38_RE drxk_taps_bg_is38_re_g
#define  DRXK_TAPS_BG_IS38_IM drxk_taps_bg_is38_im_g

static const u8_t drxk_taps_bg_is38_re_g[]= {
   DRX_16TO8(  -11 ),  /* re00 */
   DRX_16TO8(   23 ),  /* re01 */
   DRX_16TO8(    2 ),  /* re02 */
   DRX_16TO8(  -12 ),  /* re03 */
   DRX_16TO8(  -16 ),  /* re04 */
   DRX_16TO8(    1 ),  /* re05 */
   DRX_16TO8(   29 ),  /* re06 */
   DRX_16TO8(   -1 ),  /* re07 */
   DRX_16TO8(  -41 ),  /* re08 */
   DRX_16TO8(   10 ),  /* re09 */
   DRX_16TO8(   55 ),  /* re10 */
   DRX_16TO8(  -26 ),  /* re11 */
   DRX_16TO8(  -60 ),  /* re12 */
   DRX_16TO8(   44 ),  /* re13 */
   DRX_16TO8(   53 ),  /* re14 */
   DRX_16TO8(  -63 ),  /* re15 */
   DRX_16TO8(  -40 ),  /* re16 */
   DRX_16TO8(   69 ),  /* re17 */
   DRX_16TO8(   14 ),  /* re18 */
   DRX_16TO8(  -66 ),  /* re19 */
   DRX_16TO8(   13 ),  /* re20 */
   DRX_16TO8(   32 ),  /* re21 */
   DRX_16TO8(  -51 ),  /* re22 */
   DRX_16TO8(   27 ),  /* re23 */
   DRX_16TO8(  101 ),  /* re24 */
   DRX_16TO8(  -96 ),  /* re25 */
   DRX_16TO8(  -68 ),  /* re26 */
   DRX_16TO8(  573 )   /* re27 */
};

static const u8_t drxk_taps_bg_is38_im_g[]= {
   DRX_16TO8(  -22 ),  /* im00 */
   DRX_16TO8(   -5 ),  /* im01 */
   DRX_16TO8(   18 ),  /* im02 */
   DRX_16TO8(   -6 ),  /* im03 */
   DRX_16TO8(  -24 ),  /* im04 */
   DRX_16TO8(  -25 ),  /* im05 */
   DRX_16TO8(   -6 ),  /* im06 */
   DRX_16TO8(   10 ),  /* im07 */
   DRX_16TO8(  -35 ),  /* im08 */
   DRX_16TO8(  -57 ),  /* im09 */
   DRX_16TO8(   15 ),  /* im10 */
   DRX_16TO8(   19 ),  /* im11 */
   DRX_16TO8(  -85 ),  /* im12 */
   DRX_16TO8(  -61 ),  /* im13 */
   DRX_16TO8(   65 ),  /* im14 */
   DRX_16TO8(   -3 ),  /* im15 */
   DRX_16TO8( -136 ),  /* im16 */
   DRX_16TO8(   -1 ),  /* im17 */
   DRX_16TO8(  140 ),  /* im18 */
   DRX_16TO8(  -40 ),  /* im19 */
   DRX_16TO8( -143 ),  /* im20 */
   DRX_16TO8(  124 ),  /* im21 */
   DRX_16TO8(  248 ),  /* im22 */
   DRX_16TO8(  -19 ),  /* im23 */
   DRX_16TO8(  -76 ),  /* im24 */
   DRX_16TO8(  321 ),  /* im25 */
   DRX_16TO8(  590 ),  /* im26 */
   DRX_16TO8(  402 )   /* im27 */
};

#endif /* DRXK_TAPS_BG_IS38_SELECTED */
#endif /* DRXK_TAPS_BG_IS38 */



#ifdef   DRXK_TAPS_ALL
#ifndef  DRXK_TAPS_SCAN_ATV
#define  DRXK_TAPS_SCAN_ATV
#endif
#endif

#ifdef   DRXK_TAPS_SCAN_ATV
#ifndef  DRXK_TAPS_SCAN_ATV_SELECTED
#define  DRXK_TAPS_SCAN_ATV_SELECTED

#define  DRXK_TAPS_SCAN_ATV_RE drxk_taps_scan_atv_re_g
#define  DRXK_TAPS_SCAN_ATV_IM drxk_taps_scan_atv_im_g

static const u8_t drxk_taps_scan_atv_re_g[]= {
   DRX_16TO8(   -2 ),  /* re00 */
   DRX_16TO8(    2 ),  /* re01 */
   DRX_16TO8(    1 ),  /* re02 */
   DRX_16TO8(   -3 ),  /* re03 */
   DRX_16TO8(    0 ),  /* re04 */
   DRX_16TO8(    1 ),  /* re05 */
   DRX_16TO8(    1 ),  /* re06 */
   DRX_16TO8(    2 ),  /* re07 */
   DRX_16TO8(  -10 ),  /* re08 */
   DRX_16TO8(    3 ),  /* re09 */
   DRX_16TO8(   19 ),  /* re10 */
   DRX_16TO8(  -19 ),  /* re11 */
   DRX_16TO8(  -13 ),  /* re12 */
   DRX_16TO8(   42 ),  /* re13 */
   DRX_16TO8(   11 ),  /* re14 */
   DRX_16TO8(  -11 ),  /* re15 */
   DRX_16TO8(    2 ),  /* re16 */
   DRX_16TO8(   -1 ),  /* re17 */
   DRX_16TO8(   59 ),  /* re18 */
   DRX_16TO8(   10 ),  /* re19 */
   DRX_16TO8( -103 ),  /* re20 */
   DRX_16TO8(   65 ),  /* re21 */
   DRX_16TO8(  103 ),  /* re22 */
   DRX_16TO8( -124 ),  /* re23 */
   DRX_16TO8(   18 ),  /* re24 */
   DRX_16TO8(   69 ),  /* re25 */
   DRX_16TO8( -173 ),  /* re26 */
   DRX_16TO8(  394 )   /* re27 */
};

static const u8_t drxk_taps_scan_atv_im_g[]= {
   DRX_16TO8(   -1 ),  /* im00 */
   DRX_16TO8(   -2 ),  /* im01 */
   DRX_16TO8(    3 ),  /* im02 */
   DRX_16TO8(    1 ),  /* im03 */
   DRX_16TO8(   -3 ),  /* im04 */
   DRX_16TO8(    0 ),  /* im05 */
   DRX_16TO8(   -1 ),  /* im06 */
   DRX_16TO8(    5 ),  /* im07 */
   DRX_16TO8(    2 ),  /* im08 */
   DRX_16TO8(  -15 ),  /* im09 */
   DRX_16TO8(   10 ),  /* im10 */
   DRX_16TO8(   18 ),  /* im11 */
   DRX_16TO8(  -28 ),  /* im12 */
   DRX_16TO8(    5 ),  /* im13 */
   DRX_16TO8(   34 ),  /* im14 */
   DRX_16TO8(  -18 ),  /* im15 */
   DRX_16TO8(   -2 ),  /* im16 */
   DRX_16TO8(   28 ),  /* im17 */
   DRX_16TO8(   28 ),  /* im18 */
   DRX_16TO8(   52 ),  /* im19 */
   DRX_16TO8(  -40 ),  /* im20 */
   DRX_16TO8(    3 ),  /* im21 */
   DRX_16TO8(  263 ),  /* im22 */
   DRX_16TO8(   41 ),  /* im23 */
   DRX_16TO8( -218 ),  /* im24 */
   DRX_16TO8(  353 ),  /* im25 */
   DRX_16TO8(  645 ),  /* im26 */
   DRX_16TO8(  152 )   /* im27 */
};

#endif /* DRXK_TAPS_SCAN_ATV_SELECTED */
#endif /* DRXK_TAPS_SCAN_ATV */



/* END OF FILE */
