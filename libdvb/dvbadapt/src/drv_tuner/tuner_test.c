/******************************************************************************

                  ∞Ê»®À˘”– (C), 2001-2011, ±±æ©º”Œ¨Õ®—∂µÁ◊”ºº ı”–œﬁπ´Àæ

 ******************************************************************************
  Œƒ º˛ √˚   : tuner_test.c
  ∞Ê ±æ ∫≈   : ≥ı∏Â
  ◊˜    ’ﬂ   : zhwu
  …˙≥…»’∆⁄   : 2012ƒÍ6‘¬12»’
  ◊ÓΩ¸–ﬁ∏ƒ   :
  π¶ƒ‹√Ë ˆ   : tuner≤‚ ‘π¶ƒ‹∫Ø ˝
  ∫Ø ˝¡–±Ì   :
  –ﬁ∏ƒ¿˙ ∑   :
  1.»’    ∆⁄   : 2012ƒÍ6‘¬12»’
    ◊˜    ’ﬂ   : zhwu
    –ﬁ∏ƒƒ⁄»›   : ¥¥Ω®Œƒº˛

******************************************************************************/

/*----------------------------------------------*
 * ∞¸∫¨Õ∑Œƒº˛                                   *
 *----------------------------------------------*/

#define AM_DEBUG_LEVEL 5
#include "am_debug.h"
#include "am_av.h"
#include "am_dmx.h"
#include "am_vout.h"
#include "string.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#ifdef ANDROID
#include <sys/socket.h>
#endif
#include <arpa/inet.h>
#include "am_misc.h"
#include "am_aout.h"
#include "AVL_tuner.h"
#include "am_dvr.h"
int errno = 0;
 #define AM_TRY(_func) \
	do {\
	AM_ErrorCode_t _ret;\
	printf("info --- %s---%d\n", __FUNCTION__, __LINE__);\
	if ((_ret=(_func))!=AM_SUCCESS)\
		printf("error=%x  \n", _ret);\
	} while(0)

/*----------------------------------------------*
 * Õ‚≤ø±‰¡øÀµ√˜                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * Õ‚≤ø∫Ø ˝‘≠–ÕÀµ√˜                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ƒ⁄≤ø∫Ø ˝‘≠–ÕÀµ√˜                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * »´æ÷±‰¡ø                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ƒ£øÈº∂±‰¡ø                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ≥£¡ø∂®“Â                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ∫Í∂®“Â                                       *
 *----------------------------------------------*/
 
#define FEND_DEV_NO 0
#define DMX_DEV_NO  0
#define AV_DEV_NO   0
#define OSD_DEV_NO  0
#define AOUT_DEV_NO 0
#define VOUT_DEV_NO 0

#ifdef ENABLE_JPEG_TEST
#define OSD_FORMAT  AM_OSD_FMT_COLOR_8888
#define OSD_W       1280
#define OSD_H       720
#endif

/****************************************************************************
 * Functions
 ***************************************************************************/

/***************************************************************************
 *  Copyright C 2009 by Amlogic, Inc. All Rights Reserved.
 */
/**\file
 * \brief DVRÊµãËØïÁ®ãÂ∫è
 *
 * \author Xia Lei Peng <leipeng.xia@amlogic.com>
 * \date 2010-12-10: create the document
 ***************************************************************************/

/****************************************************************************
 * Macro definitions
 ***************************************************************************/
#ifdef CHIP_8226H
#define DVR_DEV_COUNT      (2)
#elif defined(CHIP_8226M) || defined(CHIP_8626X)
#define DVR_DEV_COUNT      (3)
#else
#define DVR_DEV_COUNT      (2)
#endif

#define	EPERM		 1	/* Operation not permitted */
#define	ENOENT		 2	/* No such file or directory */
#define	ESRCH		 3	/* No such process */
#define	EINTR		 4	/* Interrupted system call */
#define	EIO		 5	/* I/O error */
#define	ENXIO		 6	/* No such device or address */
#define	E2BIG		 7	/* Argument list too long */
#define	ENOEXEC		 8	/* Exec format error */
#define	EBADF		 9	/* Bad file number */
#define	ECHILD		10	/* No child processes */
#define	EAGAIN		11	/* Try again */
#define	ENOMEM		12	/* Out of memory */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
#define	ENOTBLK		15	/* Block device required */
#define	EBUSY		16	/* Device or resource busy */
#define	EEXIST		17	/* File exists */
#define	EXDEV		18	/* Cross-device link */
#define	ENODEV		19	/* No such device */
#define	ENOTDIR		20	/* Not a directory */
#define	EISDIR		21	/* Is a directory */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* File table overflow */
#define	EMFILE		24	/* Too many open files */
#define	ENOTTY		25	/* Not a typewriter */
#define	ETXTBSY		26	/* Text file busy */
#define	EFBIG		27	/* File too large */
#define	ENOSPC		28	/* No space left on device */
#define	ESPIPE		29	/* Illegal seek */
#define	EROFS		30	/* Read-only file system */
#define	EMLINK		31	/* Too many links */
#define	EPIPE		32	/* Broken pipe */
#define	EDOM		33	/* Math argument out of domain of func */
#define	ERANGE		34	/* Math result not representable */


#define FEND_DEV_NO 0
#define AV_DEV_NO 0
#define PLAY_DMX_DEV_NO 1

typedef struct
{
	int id;
	char file_name[256];
	pthread_t thread;
	int running;
	int fd;
}DVRData;

static int pat_fid, sdt_fid;
static DVRData data_threads[DVR_DEV_COUNT];

static void section_cbf(int dev_no, int fid, const uint8_t *data, int len, void *user_data)
{
	int i;
	
	printf("section:\n");
	for(i=0;i<8;i++)
	{
		printf("%02x ", data[i]);
		if(((i+1)%16)==0) printf("\n");
	}
	
	if((i%16)!=0) printf("\n");
}

static void pes_cbf(int dev_no, int fid, const uint8_t *data, int len, void *user_data)
{
	int i;

#if 0	
	printf("pes:\n");
	for(i=0;i<len;i++)
	{
		printf("%02x ", data[i]);
		if(((i+1)%16)==0) printf("\n");
	}
	
	if((i%16)!=0) printf("\n");
#endif
}

static void display_usage(void)
{
	printf("usages:\n");
	printf("\tsetsrc\t[dvr_no async_fifo_no]\n");
	printf("\tstart\t[dvr_no FILE pid_cnt pids]\n");
	printf("\tstop\t[dvr_no]\n");
	printf("\thelp\n");
	printf("\tquit\n");
}

static void start_si(void)
{
#if 0
	struct dmx_sct_filter_params param;

#define SET_FILTER(f, p, t)\
	AM_DMX_AllocateFilter(DMX_DEV_NO, &(f));\
	AM_DMX_SetCallback(DMX_DEV_NO, f, section_cbf, NULL);\
	memset(&param, 0, sizeof(param));\
	param.pid = p;\
	param.filter.filter[0] = t;\
	param.filter.mask[0] = 0xff;\
	param.flags = DMX_CHECK_CRC;\
	AM_DMX_SetSecFilter(DMX_DEV_NO, f, &param);\
	AM_DMX_SetBufferSize(DMX_DEV_NO, f, 32*1024);\
	AM_DMX_StartFilter(DMX_DEV_NO, f);
		
	if (pat_fid == -1)
	{
		SET_FILTER(pat_fid, 0, 0)
	}
	if (sdt_fid == -1)
	{
		SET_FILTER(sdt_fid, 0x11, 0x42)
	}
#endif
}

static void stop_si(void)
{
#if 0
#define FREE_FILTER(f)\
	AM_DMX_StopFilter(DMX_DEV_NO, f);\
	AM_DMX_FreeFilter(DMX_DEV_NO, f);
	
	if (pat_fid != -1)
	{
		FREE_FILTER(pat_fid)
		pat_fid = -1;
	}

	if (sdt_fid != -1)
	{
		FREE_FILTER(sdt_fid)
		sdt_fid = -1;
	}
#endif
}

static int dvr_data_write(int fd, uint8_t *buf, int size)
{
	int ret;
	int left = size;
	uint8_t *p = buf;

	while (left > 0)
	{
		ret = write(fd, p, left);
		if (ret == -1)
		{
			if (errno != EINTR)
			{
				printf(0, "Write DVR data failed: %s", strerror(errno));
				break;
			}
			ret = 0;
		}
		
		left -= ret;
		p += ret;
	}

	return (size - left);
}
static void* dvr_data_thread(void *arg)
{
	DVRData *dd = (DVRData*)arg;
	int cnt;
	uint8_t buf[256*1024];

	printf( "Data thread for DVR%d start ,record file will save to '%s'", dd->id, dd->file_name);
	
	while (dd->running)
	{
		cnt = AM_DVR_Read(dd->id, buf, sizeof(buf), 1000);
		if (cnt <= 0)
		{
			printf( "No data available from DVR%d\n", dd->id);
			usleep(200*1000);
			continue;
		}
		//printf( "read from DVR%d return %d bytes", dd->id, cnt);
		if (dd->fd != -1)
		{
			dvr_data_write(dd->fd, buf, cnt);
		}
	}

	if (dd->fd != -1)
	{
		close(dd->fd);
		dd->fd = -1;
	}
	printf( "Data thread for DVR%d now exit", dd->id);

	return NULL;
}
	
static void start_data_thread(int dev_no)
{
	DVRData *dd = &data_threads[dev_no];
	
	if (dd->running)
		return;
	dd->fd = open(dd->file_name, O_TRUNC | O_WRONLY | O_CREAT, 0666);
	if (dd->fd == -1)
	{
		printf( "Cannot open record file '%s' for DVR%d", dd->file_name, dd->id);
		return;
	}
	dd->running = 1;
	pthread_create(&dd->thread, NULL, dvr_data_thread, dd);
}

static void stop_data_thread(int dev_no)
{
	DVRData *dd = &data_threads[dev_no];

	if (! dd->running)
		return;
	dd->running = 0;
	pthread_join(dd->thread, NULL);
	printf( "Data thread for DVR%d has exit", dd->id);
}

int start_dvr_test(void)
{
	AM_DVR_StartRecPara_t spara;
	AM_Bool_t go = AM_TRUE;
	char buf[256];
	int pid, apid;

	display_usage();
	pat_fid = sdt_fid = -1;
	
	while (go)
	{
		if (gets(buf))
		{
			if(!strncmp(buf, "quit", 4))
			{
				go = AM_FALSE;
				continue;
			}
			else if (!strncmp(buf, "start", 5))
			{
				int dev_no, pid_cnt, i;
				int pids[8];
				char name[256];

				/**‰ªÖÊµãËØïÊúÄÂ§ö8‰∏™PID*/
				sscanf(buf + 5, "%d %s %d %d %d %d %d %d %d %d %d", &dev_no, name, &pid_cnt, &pids[0], &pids[1],&pids[2],&pids[3],
						&pids[4],&pids[5],&pids[6],&pids[7]);

				if (dev_no < 0 || dev_no >= DVR_DEV_COUNT)
				{
					printf( "Invalid DVR dev no, must [%d-%d]", 0, DVR_DEV_COUNT-1);
					continue;
				}
				if (pid_cnt > 8)
					pid_cnt = 8;
					
				strcpy(data_threads[dev_no].file_name, name);
				spara.pid_count = pid_cnt;
				memcpy(&spara.pids, pids, sizeof(pids));
				
				if (AM_DVR_StartRecord(dev_no, &spara) == AM_SUCCESS)
				{
					printf("AM_DVR_StartRecord ----\n");
					start_data_thread(dev_no);
				}
			}
			else if (!strncmp(buf, "stop", 4))
			{
				int dev_no;
				
				sscanf(buf + 4, "%d", &dev_no);
				if (dev_no < 0 || dev_no >= DVR_DEV_COUNT)
				{
					printf( "Invalid DVR dev no, must [%d-%d]", 0, DVR_DEV_COUNT-1);
					continue;
				}
				printf( "stop record for %d", dev_no);
				AM_DVR_StopRecord(dev_no);
				printf( "stop data thread for %d", dev_no);
				stop_data_thread(dev_no);
			}
			else if (!strncmp(buf, "help", 4))
			{
				display_usage();
			}
			else if (!strncmp(buf, "sistart", 7))
			{
				start_si();
			}
			else if (!strncmp(buf, "sistop", 6))
			{
				stop_si();
			}
			else if (!strncmp(buf, "setsrc", 6))
			{
				int dev_no, fifo_id;
				
				sscanf(buf + 6, "%d %d", &dev_no, &fifo_id);

				printf("fifo_id = %d\n",fifo_id);
				AM_DVR_SetSource(dev_no, fifo_id);
			}
			else
			{
				printf("Unkown command: %s\n", buf);
				display_usage();
			}
		}
	}
	
	return 0;
}

int dvr_main(int argc, char **argv)
{
//	AM_FEND_OpenPara_t fpara;
	AM_DMX_OpenPara_t para;
	AM_DVR_OpenPara_t dpara;
	//struct dvb_frontend_parameters p;
//	fe_status_t status;
	int freq = 0;
	int i;

	
	for (i=0; i< DVR_DEV_COUNT; i++)
	{
		printf( "Openning DMX%d...", i);
		memset(&para, 0, sizeof(para));
		AM_TRY(AM_DMX_Open(i, &para));
		AM_TRY(AM_DMX_SetSource(i, 2));
		printf( "Openning DVR%d...", i);
		memset(&dpara, 0, sizeof(dpara));
		AM_TRY(AM_DVR_Open(i, &dpara));

		data_threads[i].id = i;
		data_threads[i].fd = -1;
		data_threads[i].running = 0;
	}
	
	start_dvr_test();

	for (i=0; i< DVR_DEV_COUNT; i++)
	{
		printf( "Closing DVR%d...", i);
		if (data_threads[i].running)
			stop_data_thread(i);
		AM_DVR_Close(i);
		printf( "Closing DMX%d...", i);
		AM_DMX_Close(i);
	}
	
//	AM_FEND_Close(FEND_DEV_NO);

	return 0;
}



static void usage(void)
{
	printf("usage: am_av_test INPUT\n");
	printf("    INPUT: FILE [loop] [POSITION]\n");
	printf("    INPUT: dvb vpid apid vfmt afmt tssrc frequency\n");
	printf("    INPUT: aes FILE\n");
	printf("    INPUT: ves FILE\n");
#ifdef ENABLE_JPEG_TEST	
	printf("    INPUT: jpeg FILE\n");
#endif
	printf("    INPUT: inject IPADDR PORT vpid apid vfmt afmt\n");
	exit(0);
}

static void normal_help(void)
{
	printf("* blackout\n");
	printf("* noblackout\n");
	printf("* enablev\n");
	printf("* disablev\n");
	printf("* window X Y W H\n");
	printf("* contrast VAL\n");
	printf("* saturation VAL\n");
	printf("* brightness VAL\n");
	printf("* fullscreen\n");
	printf("* normaldisp\n");
#ifdef ENABLE_JPEG_TEST		
	printf("* snapshot\n");
#endif
	printf("* ltrack\n");
	printf("* rtrack\n");
	printf("* swaptrack\n");
	printf("* stereo\n");
	printf("* mute\n");
	printf("* unmute\n");
	printf("* vol VAL\n");
	printf("* aout\n");
	printf("* 16x9\n");
	printf("* 4x3\n");
}


static int normal_cmd(const char *cmd)
{
	printf("cmd:%s\n", cmd);
	
	if(!strncmp(cmd, "blackout", 8))
	{
		AM_AV_EnableVideoBlackout(AV_DEV_NO);
	}
	else if(!strncmp(cmd, "noblackout", 10))
	{
		AM_AV_DisableVideoBlackout(AV_DEV_NO);
	}
	else if(!strncmp(cmd, "enablev", 7))
	{
		AM_AV_EnableVideo(AV_DEV_NO);
	}
	else if(!strncmp(cmd, "disablev", 8))
	{
		AM_AV_DisableVideo(AV_DEV_NO);
	}
	else if(!strncmp(cmd, "window", 6))
	{
		int x, y, w, h;
		
		sscanf(cmd+6, "%d %d %d %d", &x, &y, &w, &h);
		AM_AV_SetVideoWindow(AV_DEV_NO, x, y, w, h);
	}
	else if(!strncmp(cmd, "contrast", 8))
	{
		int v;
		
		sscanf(cmd+8, "%d", &v);
		AM_AV_SetVideoContrast(AV_DEV_NO, v);
	}
	else if(!strncmp(cmd, "saturation", 10))
	{
		int v;
		
		sscanf(cmd+10, "%d", &v);
		AM_AV_SetVideoSaturation(AV_DEV_NO, v);
	}
	else if(!strncmp(cmd, "brightness", 10))
	{
		int v;
		
		sscanf(cmd+10, "%d", &v);
		AM_AV_SetVideoBrightness(AV_DEV_NO, v);
	}
	else if(!strncmp(cmd, "fullscreen", 10))
	{
		AM_AV_SetVideoDisplayMode(AV_DEV_NO, AM_AV_VIDEO_DISPLAY_FULL_SCREEN);
	}
	else if(!strncmp(cmd, "normaldisp", 10))
	{
		AM_AV_SetVideoDisplayMode(AV_DEV_NO, AM_AV_VIDEO_DISPLAY_NORMAL);
	}
	else if(!strncmp(cmd, "snapshot", 8))
	{
#ifndef ANDROID
#ifdef ENABLE_JPEG_TEST
		AM_OSD_Surface_t *s;
		AM_AV_SurfacePara_t para;
		AM_ErrorCode_t ret = AM_SUCCESS;
		
		para.option = 0;
		para.angle  = 0;
		para.width  = 0;
		para.height = 0;
		
		ret = AM_AV_GetVideoFrame(AV_DEV_NO, &para, &s);
		
		if(ret==AM_SUCCESS)
		{
			AM_OSD_Rect_t sr, dr;
			AM_OSD_BlitPara_t bp;
			AM_OSD_Surface_t *disp;
			
			sr.x = 0;
			sr.y = 0;
			sr.w = s->width;
			sr.h = s->height;
			dr.x = 200;
			dr.y = 200;
			dr.w = 200;
			dr.h = 200;
			memset(&bp, 0, sizeof(bp));
			
			AM_OSD_GetSurface(OSD_DEV_NO, &disp);
			AM_OSD_Blit(s, &sr, disp, &dr, &bp);
			AM_OSD_Update(OSD_DEV_NO, NULL);
			AM_OSD_DestroySurface(s);
		}
#endif		
#endif
	}
	else if(!strncmp(cmd, "ltrack", 6))
	{
		printf("ltrack\n");
		AM_AOUT_SetOutputMode(AOUT_DEV_NO, AM_AOUT_OUTPUT_DUAL_LEFT);
	}	
	else if(!strncmp(cmd, "rtrack", 6))
	{
		printf("rtrack\n");
		AM_AOUT_SetOutputMode(AOUT_DEV_NO, AM_AOUT_OUTPUT_DUAL_RIGHT);
	}	
	else if(!strncmp(cmd, "swaptrack", 9))
	{
		printf("strack\n");
		AM_AOUT_SetOutputMode(AOUT_DEV_NO, AM_AOUT_OUTPUT_SWAP);
	}	
	else if(!strncmp(cmd, "stereo", 6))
	{
		printf("stereo\n");
		AM_AOUT_SetOutputMode(AOUT_DEV_NO, AM_AOUT_OUTPUT_STEREO);
	}	
	else if(!strncmp(cmd, "mute", 4))
	{
		printf("mute\n");
		AM_AOUT_SetMute(AOUT_DEV_NO, AM_TRUE);
	}	
	else if(!strncmp(cmd, "unmute", 6))
	{
		printf("unmute\n");
		AM_AOUT_SetMute(AOUT_DEV_NO, AM_FALSE);
	}	
	else if(!strncmp(cmd, "vol", 3))
	{
		int v;
		
		sscanf(cmd+3, "%d", &v);
		AM_AOUT_SetVolume(AOUT_DEV_NO, v);
	}	
	else if(!strncmp(cmd, "aout", 4))
	{
		AM_Bool_t mute;
		AM_AOUT_OutputMode_t out;
		int vol;
		
		AM_AOUT_GetMute(AOUT_DEV_NO, &mute);
		AM_AOUT_GetOutputMode(AOUT_DEV_NO, &out);
		AM_AOUT_GetVolume(AOUT_DEV_NO, &vol);

		printf("Audio Out Info:\n");
		printf("Mute: %s\n", mute?"true":"false");
		printf("Mode: %s\n", (out==AM_AOUT_OUTPUT_STEREO)?"STEREO":
							(out==AM_AOUT_OUTPUT_DUAL_LEFT)?"DUALLEFT":
							(out==AM_AOUT_OUTPUT_DUAL_RIGHT)?"DUALRIGHT":
							(out==AM_AOUT_OUTPUT_SWAP)?"SWAP":"???");
		printf("Volume: %d\n", vol);
	}
	else if(!strncmp(cmd, "vout", 4))
	{
		AM_AV_VideoAspectRatio_t as;
		AM_AV_VideoAspectMatchMode_t am;
		
		AM_AV_GetVideoAspectRatio(AV_DEV_NO, &as);
		AM_AV_GetVideoAspectMatchMode(AV_DEV_NO, &am);

		printf("Video Out Info:\n");
		printf("AspectRatio: %s\n", (as==AM_AV_VIDEO_ASPECT_AUTO)?"AUTO":
							(as==AM_AV_VIDEO_ASPECT_16_9)?"16x9":
							(as==AM_AV_VIDEO_ASPECT_4_3)?"4x3":"???");
		printf("AspectMatch: %s\n", (am==AM_AV_VIDEO_ASPECT_MATCH_IGNORE)?"IGNORE":
							(am==AM_AV_VIDEO_ASPECT_MATCH_LETTER_BOX)?"LETTERBOX":
							(am==AM_AV_VIDEO_ASPECT_MATCH_PAN_SCAN)?"PANSCAN":
							(am==AM_AV_VIDEO_ASPECT_MATCH_COMBINED)?"COMBINED":"???");
	}
	else if(!strncmp(cmd, "auto", 4))
	{
		AM_AV_SetVideoAspectRatio(AV_DEV_NO, AM_AV_VIDEO_ASPECT_AUTO);
	}	
	else if(!strncmp(cmd, "16x9", 4))
	{
		AM_AV_SetVideoAspectRatio(AV_DEV_NO, AM_AV_VIDEO_ASPECT_16_9);
	}
	else if(!strncmp(cmd, "4x3", 3))
	{
		AM_AV_SetVideoAspectRatio(AV_DEV_NO, AM_AV_VIDEO_ASPECT_4_3);
	}
	else if(!strncmp(cmd, "igno", 4))
	{
		AM_AV_SetVideoAspectMatchMode(AV_DEV_NO, AM_AV_VIDEO_ASPECT_MATCH_IGNORE);
	}
	else if(!strncmp(cmd, "lett", 4))
	{
		AM_AV_SetVideoAspectMatchMode(AV_DEV_NO, AM_AV_VIDEO_ASPECT_MATCH_LETTER_BOX);
	}
	else if(!strncmp(cmd, "pans", 4))
	{
		AM_AV_SetVideoAspectMatchMode(AV_DEV_NO, AM_AV_VIDEO_ASPECT_MATCH_PAN_SCAN);
	}
	else if(!strncmp(cmd, "comb", 4))
	{
		AM_AV_SetVideoAspectMatchMode(AV_DEV_NO, AM_AV_VIDEO_ASPECT_MATCH_COMBINED);
	}
	else
	{
		return 0;
	}
	
	return 1;
}


static void file_play(const char *name, int loop, int pos)
{
	int running = 1;
	char buf[256];
	int paused = 0;
	AM_DMX_OpenPara_t para;
	
	memset(&para, 0, sizeof(para));
	
	AM_DMX_Open(DMX_DEV_NO, &para);
	AM_DMX_SetSource(DMX_DEV_NO, AM_DMX_SRC_HIU);
	AM_AV_SetTSSource(AV_DEV_NO, AM_AV_TS_SRC_HIU);
	AM_AV_StartFile(AV_DEV_NO, name, loop, pos);
	
	printf("play \"%s\" loop:%s from:%d\n", name, loop?"y":"n", pos);
	
	while(running)
	{
		printf("********************\n");
		printf("* commands:\n");
		printf("* quit\n");
		printf("* pause\n");
		printf("* resume\n");
		printf("* ff SPEED\n");
		printf("* fb SPEED\n");
		printf("* seek POS\n");
		printf("* status\n");
		printf("* info\n");
		normal_help();
		printf("********************\n");
		
		if(gets(buf))
		{
			if(!strncmp(buf, "quit", 4))
			{
				running = 0;
			}
			else if(!strncmp(buf, "pause", 5))
			{
				AM_AV_PauseFile(AV_DEV_NO);
				paused = 1;
			}
			else if(!strncmp(buf, "resume", 6))
			{
				AM_AV_ResumeFile(AV_DEV_NO);
				paused = 0;
			}
			else if(!strncmp(buf, "ff", 2))
			{
				int speed = 0;
				
				sscanf(buf+2, "%d", &speed);
				AM_AV_FastForwardFile(AV_DEV_NO, speed);
			}
			else if(!strncmp(buf, "fb", 2))
			{
				int speed = 0;
				
				sscanf(buf+2, "%d", &speed);
				AM_AV_FastBackwardFile(AV_DEV_NO, speed);
			}
			else if(!strncmp(buf, "seek", 4))
			{
				int pos = 0;
				
				sscanf(buf+4, "%d", &pos);
				AM_AV_SeekFile(AV_DEV_NO, pos, !paused);
			}
			else if(!strncmp(buf, "status", 6))
			{
				AM_AV_PlayStatus_t status;
				
				AM_AV_GetPlayStatus(AV_DEV_NO, &status);
				printf("stats: duration:%d current:%d\n", status.duration, status.position);
			}
			else if(!strncmp(buf, "info", 4))
			{
				AM_AV_FileInfo_t info;
				
				AM_AV_GetCurrFileInfo(AV_DEV_NO, &info);
				printf("info: duration:%d size:%lld\n", info.duration, info.size);
			}
			else
			{
				normal_cmd(buf);
			}
		}
	}
	
	AM_DMX_Close(DMX_DEV_NO);
}


static void dvb_play(int vpid, int apid, int vfmt, int afmt, int freq, int ts)
{
	int running = 1;
	char buf[256];
	AM_DMX_OpenPara_t para;
	int error = 0;
	printf("DVB: V[%d:%d], A[%d:%d], TS[%d:%d]\n", 	vpid, vfmt, apid, afmt, ts, freq);

	memset(&para, 0, sizeof(para));
	AM_TRY(AM_DMX_Open(DMX_DEV_NO, &para));

	AM_TRY(AM_DMX_SetSource(DMX_DEV_NO, ts));
	AM_TRY(AM_AV_SetTSSource(AV_DEV_NO, ts));


	AM_TRY(AM_VOUT_SetFormat(VOUT_DEV_NO,AM_VOUT_FORMAT_1080I));
	AM_TRY(AM_VOUT_Enable(VOUT_DEV_NO));	

//	AM_TRY(AM_AV_SetVideoWindow(AV_DEV_NO, 0,0,300,400));
//	AM_TRY(AM_AV_SetVideoAspectRatio(AV_DEV_NO, AM_AV_VIDEO_ASPECT_4_3));

	
//	AM_TRY(AM_AOUT_SetOutputMode(AOUT_DEV_NO,AM_AOUT_OUTPUT_STEREO));
//	AM_TRY(AM_AOUT_SetVolume(AOUT_DEV_NO,16));

	//if(freq>0)
	{
	//	AM_FEND_OpenPara_t para;
	//	struct dvb_frontend_parameters p;
	//	fe_status_t status;
		
	//	memset(&para, 0, sizeof(para));
	//	AM_FEND_Open(FEND_DEV_NO, &para);
		
	//	p.frequency = freq;
#if 1	
	//	p.inversion = INVERSION_AUTO;
	//	p.u.ofdm.bandwidth = BANDWIDTH_8_MHZ;
	//	p.u.ofdm.code_rate_HP = FEC_AUTO;
	//	p.u.ofdm.code_rate_LP = FEC_AUTO;
	//	p.u.ofdm.constellation = QAM_AUTO;
	//	p.u.ofdm.guard_interval = GUARD_INTERVAL_AUTO;
	//	p.u.ofdm.hierarchy_information = HIERARCHY_AUTO;
	//	p.u.ofdm.transmission_mode = TRANSMISSION_MODE_AUTO;
#else		
		p.u.qam.symbol_rate = 6875000;
		p.u.qam.fec_inner = FEC_AUTO;
		p.u.qam.modulation = QAM_64;
#endif		
	//	AM_FEND_Lock(FEND_DEV_NO, &p, &status);
		
	//	printf("lock status: 0x%x\n", status);
	}

	//AM_TRY(AM_AV_EnableVideo(AV_DEV_NO));
	
	AM_TRY(AM_AV_StartTS(AV_DEV_NO, vpid, apid, vfmt, afmt));
	
	while(running)
	{
		printf("********************\n");
		printf("* commands:\n");
		printf("* quit\n");
		normal_help();
		printf("********************\n");
		
		if(gets(buf))
		{
			if(!strncmp(buf, "quit", 4))
			{
				running = 0;
			}
			else
			{
				normal_cmd(buf);
			}
		}
	}
	
//	if(freq>0)
//	{
//		AM_FEND_Close(FEND_DEV_NO);
//	}
	
	AM_DMX_Close(DMX_DEV_NO);
}

 
void video_play(int argc,char* argv[])
{
	AM_AV_OpenPara_t para;
#ifdef ENABLE_JPEG_TEST	
	AM_OSD_OpenPara_t osd_p;
#endif
	int apid=-1, vpid=-1, vfmt=0, afmt=0, freq=0, ts=0;
	int i, v, loop=0, pos=0, port=1234;
	struct in_addr addr;
	AM_AOUT_OpenPara_t aout_para;
	AM_VOUT_OpenPara_t vout_para;
	if(argc<2)
		usage();

	system("echo 1 > /sys/class/graphics/fb0/blank");

	memset(&para, 0, sizeof(para));
	AM_TRY(AM_AV_Open(AV_DEV_NO, &para));
	
#ifdef ENABLE_JPEG_TEST
	memset(&osd_p, 0, sizeof(osd_p));
	osd_p.format = AM_OSD_FMT_COLOR_8888;
	osd_p.width  = 1280;
	osd_p.height = 720;
	osd_p.output_width  = 1280;
	osd_p.output_height = 720;
	osd_p.enable_double_buffer = AM_FALSE;
#ifndef ANDROID
	AM_TRY(AM_OSD_Open(OSD_DEV_NO, &osd_p));
#endif
#endif

	memset(&aout_para, 0, sizeof(aout_para));
	AM_TRY(AM_AOUT_Open(AOUT_DEV_NO, &aout_para));

	memset(&vout_para,0,sizeof(vout_para));
	AM_TRY(AM_VOUT_Open(VOUT_DEV_NO, &vout_para));
	
	if(strcmp(argv[1], "dvb")==0)
	{
		if(argc<3)
			usage();
		
		vpid = strtol(argv[2], NULL, 0);
		
		if(argc>3)
			apid = strtol(argv[3], NULL, 0);
		
		if(argc>4)
			vfmt = strtol(argv[4], NULL, 0);
		
		if(argc>5)
			afmt = strtol(argv[5], NULL, 0);

		if(argc>6)
			ts = strtol(argv[6], NULL, 0);
		
		if(argc>7)
			freq = strtol(argv[7], NULL, 0);
		if(argc>8)
			freq = strtol(argv[7], NULL, 0);
	
		dvb_play(vpid, apid, vfmt, afmt, freq, ts);
	}
	else if(strcmp(argv[1], "ves")==0)
	{
		char *name;
		int vfmt = 0;
		
		if(argc<3)
			usage();
		
		name = argv[2];
		
		if(argc>3)
			vfmt = strtol(argv[3], NULL, 0);
		
	//	ves_play(name, vfmt);
	}
	else if(strcmp(argv[1], "aes")==0)
	{
		char *name;
		int afmt = 0;
		int times = 1;
		
		if(argc<3)
			usage();
		
		name = argv[2];
		
		if(argc>3)
			afmt = strtol(argv[3], NULL, 0);
		if(argc>4)
			times = strtol(argv[4], NULL, 0);
		
	//	aes_play(name, afmt, times);
	}
#ifdef ENABLE_JPEG_TEST	
	else if(strcmp(argv[1], "jpeg")==0)
	{
		if(argc<3)
			usage();
		
	//	jpeg_show(argv[2]);
	}
#endif	
	else if(strcmp(argv[1], "inject")==0)
	{
		if(argc<5)
			usage();
		
		inet_pton(AF_INET, argv[2], &addr);
		port = strtol(argv[3], NULL, 0);
		vpid = strtol(argv[4], NULL, 0);
		
		if(argc>5)
			apid = strtol(argv[5], NULL, 0);
		
		if(argc>6)
			vfmt = strtol(argv[6], NULL, 0);
		
		if(argc>7)
			afmt = strtol(argv[7], NULL, 0);
		
	//	inject_play(&addr, port, vpid, apid, vfmt, afmt);
	}
	else
	{
		for(i=2; i<argc; i++)
		{
			if(!strcmp(argv[i], "loop"))
				loop = 1;
			else if(sscanf(argv[i], "%d", &v)==1)
				pos = v;
		}
		
		file_play(argv[1], loop, pos);
	}
	
#ifndef ANDROID
#ifdef ENABLE_JPEG_TEST
	AM_OSD_Close(OSD_DEV_NO);
#endif
#endif
	AM_AV_Close(AV_DEV_NO);
	AM_AOUT_Close(AOUT_DEV_NO);
	
	return 0;	
}
extern int dmx_test(int argc, char **argv);
extern void OS_API_Init(void);
//#define  PMT_TEST
#define  PAT_TEST
//#define  EIT_TEST

static void dump_bytes(int dev_no, int fid, const uint8_t *data, int len, void *user_data)
{
	FILE *fp;
	int did = data[8];
	int sec = data[6];
	
	
#if 0
	if(did!=3)
		return;
	
	if(sections[sec].got)
	{
		if((sections[sec].len!=len) || memcmp(sections[sec].data, data, len))
		{
			printf( "SECTION %d MISMATCH LEN: %d", sec, len);
		}
	}
	else
	{
		sections[sec].len = len;
		sections[sec].got = 1;
		memcpy(sections[sec].data, data, len);
		printf( "GET SECTION %d LEN: %d", sec, len);
	}
#endif
#if 0
	int did = data[8];
	int bid = (data[9]<<24)|(data[10]<<16)|(data[11]<<8)|data[12];
	int sec = data[6];
	
	if(did!=3)
		return;
	if(sec_mask[sec])
		return;
	
	sec_mask[sec] = 1;
	printf( "get section %d", sec);
	
	fseek(fp, bid*4030, SEEK_SET);
	fwrite(data+13, 1, len-17, fp);
#endif
#if 1
	int i;
	
	printf("section: = %d:%d:%d\n",dev_no,fid,len);
	for(i=0;i<len;i++)
	{
		printf("%02x ", data[i]);
		if(((i+1)%16)==0) printf("\n");
	}
	
	if((i%16)!=0) printf("\n");
#endif
}

static int get_section(int dmx)
{
#ifdef PAT_TEST
	int fid;
#endif

#ifdef PMT_TEST
	int fid_pmt;
#endif

#ifdef EIT_TEST
	int fid_eit_pf, fid_eit_opf;
#endif

	struct dmx_sct_filter_params param;
	struct dmx_pes_filter_params pparam;
#ifdef PAT_TEST
	AM_TRY(AM_DMX_AllocateFilter(dmx, &fid));
	AM_TRY(AM_DMX_SetCallback(dmx, fid, dump_bytes, NULL));
#endif

#ifdef PMT_TEST
	AM_TRY(AM_DMX_AllocateFilter(dmx, &fid_pmt));
	AM_TRY(AM_DMX_SetCallback(dmx, fid_pmt, dump_bytes, NULL));
#endif

#ifdef EIT_TEST
	AM_TRY(AM_DMX_AllocateFilter(dmx, &fid_eit_pf));
	AM_TRY(AM_DMX_SetCallback(dmx, fid_eit_pf, dump_bytes, NULL));
	AM_TRY(AM_DMX_AllocateFilter(dmx, &fid_eit_opf));
	AM_TRY(AM_DMX_SetCallback(dmx, fid_eit_opf, dump_bytes, NULL));
#endif

#ifdef PAT_TEST
	memset(&param, 0, sizeof(param));
	param.pid = 0;
	param.filter.filter[0] = 0;
	param.filter.mask[0] = 0xff;
	//param.filter.filter[2] = 0x08;
	//param.filter.mask[2] = 0xff;
	param.timeout = 50000;

	param.flags = DMX_CHECK_CRC;
	
	AM_TRY(AM_DMX_SetSecFilter(dmx, fid, &param));
#endif
	//pmt
#ifdef PMT_TEST
	memset(&param, 0, sizeof(param));
	param.pid = 0x3f2;
	param.filter.filter[0] = 0x2;
	param.filter.mask[0] = 0xff;
	param.filter.filter[1] = 0x0;
	param.filter.mask[1] = 0xff;
	param.filter.filter[2] = 0x65;
	param.filter.mask[2] = 0xff;
	param.flags = DMX_CHECK_CRC;
	
	AM_TRY(AM_DMX_SetSecFilter(dmx, fid_pmt, &param));
#endif

#ifdef EIT_TEST
	memset(&param, 0, sizeof(param));
	param.pid = 0x12;
	param.filter.filter[0] = 0x4E;
	param.filter.mask[0] = 0xff;
	param.flags = DMX_CHECK_CRC;
	AM_TRY(AM_DMX_SetSecFilter(dmx, fid_eit_pf, &param));
	
	memset(&param, 0, sizeof(param));
	param.pid = 0x12;
	param.filter.filter[0] = 0x4F;
	param.filter.mask[0] = 0xff;
	param.flags = DMX_CHECK_CRC;
	AM_TRY(AM_DMX_SetSecFilter(dmx, fid_eit_opf, &param));
#endif

#ifdef PAT_TEST
	AM_TRY(AM_DMX_SetBufferSize(dmx, fid, 32*1024));
	AM_TRY(AM_DMX_StartFilter(dmx, fid));
#endif
#ifdef PMT_TEST
	AM_TRY(AM_DMX_SetBufferSize(dmx, fid_pmt, 32*1024));
	AM_TRY(AM_DMX_StartFilter(dmx, fid_pmt));
#endif
#ifdef EIT_TEST
	AM_TRY(AM_DMX_SetBufferSize(dmx, fid_eit_pf, 32*1024));
	AM_TRY(AM_DMX_StartFilter(dmx, fid_eit_pf));
	AM_TRY(AM_DMX_SetBufferSize(dmx, fid_eit_opf, 32*1024));
	//AM_TRY(AM_DMX_StartFilter(dmx, fid_eit_opf));
#endif
	
	sleep(1000);
#ifdef PAT_TEST
	AM_TRY(AM_DMX_StopFilter(dmx, fid));
	AM_TRY(AM_DMX_FreeFilter(dmx, fid));
#endif	
#ifdef PMT_TEST
	AM_TRY(AM_DMX_StopFilter(dmx, fid_pmt));
	AM_TRY(AM_DMX_FreeFilter(dmx, fid_pmt));
#endif
#ifdef EIT_TEST
	AM_TRY(AM_DMX_StopFilter(dmx, fid_eit_pf));
	AM_TRY(AM_DMX_FreeFilter(dmx, fid_eit_pf));
	AM_TRY(AM_DMX_StopFilter(dmx, fid_eit_opf));
	AM_TRY(AM_DMX_FreeFilter(dmx, fid_eit_opf));
#endif
	//fclose(fp);
	
	return 0;
}

void dmx_test1(int argc,char** argv)
{
	int dmx = 0;
	int ts = 0;
	AM_DMX_OpenPara_t para;
	printf("argc = %d\n",argc);
	if(argc>2)
	{
		dmx = strtol(argv[1], NULL, 0);
		ts =  strtol(argv[2], NULL, 0);
	}	
	dmx = 0;
	ts = 2;
	AM_TRY(AM_DMX_Open(dmx, &para));
	AM_TRY(AM_DMX_SetSource(dmx, ts));

	get_section(dmx);

}
int  main(int argc,char* argv[])
{
	printf("-----------AVL63X1_TestTuner start ----------\n");
	#if  1
	AVL63X1_ErrorCode ret = AVL63X1_EC_OK;	
	
	AVL_TUNER_PARM_STRU ptSrcInfo;
	AVL63X1_LockCode Status = 0;
	AVL_uint16 strength=0;
	AVL_uint32 quality=0;
	AVL_uint32 puiSignalBer = 0;
	float puiSignalSnr = 0;
	AVL_int32 i = 0;
	OS_API_Init();
	int log = 0;
#if 1
	/* ≥ı ºªØtuner */
	ret = AVL63X1_Tuner_Initialize();


	/* À¯∆µ */
	ptSrcInfo.m_demod_mode = AVL_DEMOD_MODE_DVBC;
	ptSrcInfo.m_uiFrequency_Hz = 698000000;//427000000;
	ptSrcInfo.m_uiSymbolRate_Hz = 6875000;//6952000;
	if(argc>7)
	{
		ptSrcInfo.m_uiFrequency_Hz  = strtol(argv[7], NULL, 0);
		ptSrcInfo.m_uiFrequency_Hz*= 1000000;
		ptSrcInfo.m_uiSymbolRate_Hz = strtol(argv[8], NULL, 0);
		ptSrcInfo.m_uiSymbolRate_Hz*=1000;
		log = strtol(argv[9], NULL, 0);
		if(log==1)
		{
			system("echo 7 > /proc/sys/kernel/printk");
		}
		else
		{
			system("echo 1 > /proc/sys/kernel/printk");
		}

	}
	
	printf("set frequency = %d\n",ptSrcInfo.m_uiFrequency_Hz);
	ret |= AVL63X1_Tuner_SetFrequency(0, &ptSrcInfo, 0);
	/* À¯∆µ◊¥Ã¨ */
	while( i < 50 )
	{
		Status = AVL63X1_Tuner_GetLockStatus(0);
		if ( Status == 1 )
		{
		    	break;
		}
		i++;
		usleep(100);
	}

	if(Status!=1)
	{
		printf("---------- Lock status %d    i = %d-------\n",Status, i );	
		return 0;
	}
	else
	{
		printf("---------- Lock status %d    i = %d-------\n",Status, i );		
	}

	/* –≈∫≈«ø∂» */
//	ret |= AVL63X1_Tuner_GetTunInfoStrengthPercent( 0, &strength);
	
	/* –≈∫≈÷ ¡ø */
//	ret |= AVL63X1_Tuner_GetTunInfoQualityPercent( 0, &quality );

//	printf("strength %d%, quality %d%\n",strength,quality);

	/* –≈∫≈ŒÛ¬Î¬  */
//	ret |= AVL63X1_Tuner_GetTunInfoBer(0, &puiSignalBer );

//	printf("puiSignalBer %e.\n",puiSignalBer);
#endif	
	usleep(10);
	if(argc<3)
	{
		dmx_test1(argc,argv);
	}
	if((strcmp(argv[1], "dvb")==0))
	{
		video_play(argc,argv);
	}
	else
	{
		dvr_main(argc,argv);
	}
	
	while(1)
	{
	usleep(10000);
	/* ªÒ»°–≈‘Î±» */
	ret |= AVL63X1_Tuner_GetTunInfoSnr(0, &puiSignalSnr);

	printf("puiSignalSnr %.2f.\n",puiSignalSnr);
		
	}
#else
	demo_test_main();
#endif /* #if 0 */
	
	printf("-----------AVL63X1_TestTuner end ----------\n");


	return AVL63X1_EC_OK;//ret;
}






