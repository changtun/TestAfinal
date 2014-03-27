/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2011/8/24 : Initial Version
******************************************************************************/
#ifndef __DAL_MMS_H
#define __DAL_MMS_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/
#define MMS_MAX_URL_LEN			256
/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
typedef enum mms_event_e
{
	MMS_DOWNLOAD_START,
	MMS_FRIST_DATA_BLOCK_OK,
	MMS_DATA_RECEIVE_TIMEOUT,
	MMS_DATA_RECEIVE_RESUME,
}mms_event_t;

typedef void (*mms_download_event_func)(mms_event_t event, void* param); 

typedef struct mms_init_param_e
{
	mms_download_event_func		dl_notify;
	int							auto_timeshift;
}mms_init_param_t;

/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/

/******************************************************************************
* Function : 
* parameters :		 
* Return :	
* Description :
* Author : zyliu
******************************************************************************/

int dal_mms_init( mms_init_param_t* param);
int dal_mms_connect(const char* url);

int dal_mms_timeshift(void);

int dal_mms_close(void);
int dal_mms_interrupt(void);



/*
U32  STSTREAMER_MMS_FileOpen (STSTREAM_Handle_t Handle, U8 * FileName, STSTREAM_FSflags_t Flags);
U32   STSTREAMER_MMS_FileClose  (STSTREAM_Handle_t Handle, U32 FileHandle);
U32   STSTREAMER_MMS_FileRead   (STSTREAM_Handle_t Handle, U32 FileHandle, U8  * Buffer, U32 SizeInBytes);
U32   STSTREAMER_MMS_FileWrite  (STSTREAM_Handle_t Handle, U32 FileHandle, U8  * Buffer, U32 SizeInBytes);
U32   STSTREAMER_MMS_FileSeek   (STSTREAM_Handle_t Handle, U32 FileHandle, S64     ByteOffset, STSTREAM_FSflags_t Flags);
U32   STSTREAMER_MMS_FileSeeko  (STSTREAM_Handle_t Handle, U32 FileHandle, off_t ByteOffset, STSTREAM_FSflags_t Flags);
int   STSTREAMER_MMS_FileGetc   (STSTREAM_Handle_t Handle, U32 FileHandle);
off_t STSTREAMER_MMS_FileTello  (STSTREAM_Handle_t Handle, U32 FileHandle);
int   STSTREAMER_MMS_FileEof    (STSTREAM_Handle_t Handle, U32 FileHandle);
int   STSTREAMER_MMS_FileError  (STSTREAM_Handle_t Handle, U32 FileHandle);
*/

//void* ststream_mms_open();
int ststream_mms_read(unsigned char* buffer, unsigned int size);
int ststream_mms_seek(unsigned int offset);
//void* ststream_mms_close();

unsigned int ststream_mms_tell( void);
int ststream_mms_found_data_reference(unsigned int data_reference);
int ststream_mms_set_file_offset_ptr(unsigned int* p_ststeam_file_offset);


#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __DAL_MMS_H */
/* End of XXXXXXXX.h  --------------------------------------------------------- */

