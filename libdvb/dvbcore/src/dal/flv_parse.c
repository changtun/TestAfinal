#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "flv_parse.h"



//#define FLV_DEBUG(_x_)		{ printf("                               [FLV_DEBUG] %s:  ",__FUNCTION__); printf _x_ ; }
#define FLV_DEBUG(_x_)		

#define FLV_ERROR(_x_)		{ printf("                    ???????????[FLV_ERROR] %s:  ",__FUNCTION__); printf _x_ ; }



#define MAX_FLV_PARSE_WORK_SPACE		1024


#define FLV_HEADER_LEN				9
#define FLV_PRE_TAG_SIZE			4
#define FLV_TAG_HEADER_LEN			11


#define FLV_AUDIO_TAG				0x8
#define FLV_VIDEO_TAG				0x9
#define FLV_SCRIPT_TAG				0x12

#define FLV_FIRST_AMF_LEN			13
#define FLV_SECOND_AMF_HEADER_LEN		5


static flv_parse_input_t 	g_flv_parse_input= NULL;






double double_convert( unsigned char* buf);




void flv_parse_set_input(flv_parse_input_t input_func)
{
	g_flv_parse_input = input_func;
}

int flv_parse_perform( flv_info_t* p_info)
{
	unsigned int 		cur_off = 0;
	unsigned int		read_size = 0;
	unsigned int		tag_data_len = 0;
	unsigned int		second_amf_item_count = 0;
	unsigned int		item_name_len = 0;
	unsigned int		item_data_len = 0;
	int					i = 0;
	unsigned char		work_buf[MAX_FLV_PARSE_WORK_SPACE];
	char				item_name[256];
	double				item_data_double;

	if( p_info == NULL || g_flv_parse_input == NULL)
	{
		return -1;
	}

	memset(work_buf,0,sizeof(work_buf));
	read_size = g_flv_parse_input(cur_off, work_buf, FLV_HEADER_LEN);
	if( read_size != FLV_HEADER_LEN)
	{	
		FLV_ERROR(("read data failed with off: %d  size: %d\n", cur_off, FLV_HEADER_LEN));
		return -1;
	}
	cur_off += FLV_HEADER_LEN;


	if( work_buf[0] != 'F' || work_buf[1] != 'L' || work_buf[2] != 'V')
	{
		FLV_ERROR(("flv signature error\n"));
		return -1;
	}

	cur_off += FLV_PRE_TAG_SIZE;

	while(1)
	{
		memset(work_buf,0,sizeof(work_buf));
		read_size = g_flv_parse_input(cur_off, work_buf, FLV_TAG_HEADER_LEN);
		if( read_size != FLV_TAG_HEADER_LEN)
		{	
			FLV_ERROR(("read data failed with off: %d  size: %d\n", cur_off, FLV_TAG_HEADER_LEN));
			return -1;
		}
		cur_off += FLV_TAG_HEADER_LEN;

		tag_data_len = ((work_buf[1]&0xff)<<16) + ((work_buf[2]&0xff)<<8) + (work_buf[3]&0xff);
		if( FLV_SCRIPT_TAG != work_buf[0])
		{
			/*不是metadata 数据块，则跳过该数据块*/
			cur_off += tag_data_len;
			continue;
		}
		p_info->header_metadata_len = FLV_HEADER_LEN + FLV_PRE_TAG_SIZE + FLV_TAG_HEADER_LEN + tag_data_len + FLV_PRE_TAG_SIZE;
		FLV_DEBUG(("header_metadata_len: %d  0x%x\n", p_info->header_metadata_len, p_info->header_metadata_len));

		FLV_DEBUG(("find metadata!!!\n"));

		/*跳过第一个AMF 块*/
		cur_off += FLV_FIRST_AMF_LEN;

		memset(work_buf,0,sizeof(work_buf));
		read_size = g_flv_parse_input(cur_off, work_buf, FLV_SECOND_AMF_HEADER_LEN);
		if( read_size != FLV_SECOND_AMF_HEADER_LEN)
		{	
			FLV_ERROR(("read data failed with off: %d  size: %d\n", cur_off, FLV_SECOND_AMF_HEADER_LEN));
			return -1;
		}
		cur_off += FLV_SECOND_AMF_HEADER_LEN;

		if( 0x8 != work_buf[0])
		{
			FLV_ERROR(("second AMF is not type 0x08\n"));
			return -1;
		}
		
		second_amf_item_count = (work_buf[1]<<24) + (work_buf[2]<<16) + (work_buf[3]<< 8) + work_buf[4];

		for( i =0; i< second_amf_item_count; i++)
		{
			memset(work_buf,0,sizeof(work_buf));
			read_size = g_flv_parse_input(cur_off, work_buf, 2);
			if( read_size != 2)
			{	
				FLV_ERROR(("read data failed with off: %d  size: %d\n", cur_off, 2));
				return -1;
			}
			cur_off += 2;

			item_name_len = work_buf[1];

			memset(work_buf,0,sizeof(work_buf));
			read_size = g_flv_parse_input(cur_off, work_buf, item_name_len);
			if( read_size != item_name_len)
			{	
				FLV_ERROR(("read data failed with off: %d  size: %d\n", cur_off, item_name_len));
				return -1;
			}
			cur_off += item_name_len;

			//FLV_DEBUG(("item name: %s\n", work_buf));

			if( strcmp((char*)work_buf, "duration") == 0 || strcmp((char*)work_buf, "width") == 0 
				|| strcmp((char*)work_buf, "height") == 0 || strcmp((char*)work_buf, "framerate") == 0)
			{
				memset(item_name,0,sizeof(item_name));
				strncpy((char*)item_name, (char*)work_buf, item_name_len);
				
				memset(work_buf,0,sizeof(work_buf));
				read_size = g_flv_parse_input(cur_off, work_buf, 9);
				if( read_size != 9)
				{	
					FLV_ERROR(("read data failed with off: %d  size: %d\n", cur_off, 9));
					return -1;
				}
				cur_off += 9;

				item_data_double = double_convert(&work_buf[1]);
				if( strcmp(item_name, "duration") == 0)
				{
					p_info->duration = item_data_double;
					FLV_DEBUG(("duration: %f\n", p_info->duration));
				}
				else if( strcmp(item_name, "width") == 0)
				{
					p_info->width = item_data_double;
					FLV_DEBUG(("width: %f\n", p_info->width));
				}
				else if( strcmp(item_name, "height") == 0)
				{
					p_info->height = item_data_double;
					FLV_DEBUG(("height: %f\n", p_info->height));
				}
				else if( strcmp(item_name, "framerate") == 0)
				{
					p_info->framerate = item_data_double;
					FLV_DEBUG(("framerate: %f\n", p_info->framerate));
				}				
			}
			else
			{
				/*获取数据类型*/
				memset(work_buf,0,sizeof(work_buf));
				read_size = g_flv_parse_input(cur_off, work_buf, 1);
				if( read_size != 1)
				{	
					FLV_ERROR(("read data failed with off: %d  size: %d\n", cur_off, 1));
					return -1;
				}
				cur_off += 1;
				
				if( work_buf[0] == 0x00)/*data type is double*/
				{
					item_data_len = 8;
				}
				else if( work_buf[0] == 0x01)/*data type is boolean*/
				{
					item_data_len = 1;
				}
				else if( work_buf[0] == 0x02)/*data type is string*/
				{
					/*获取字符串长度*/
					memset(work_buf,0,sizeof(work_buf));
					read_size = g_flv_parse_input(cur_off, work_buf, 2);
					if( read_size != 2)
					{	
						FLV_ERROR(("read data failed with off: %d  size: %d\n", cur_off, 2));
						return -1;
					}
					cur_off += 2;	
					item_data_len = (work_buf[0]<<8) + work_buf[1];
				}
				else if( work_buf[0] == 0x03)/*data type is object*/
				{
					FLV_DEBUG(("wo don't to parse type SCRIPTDATAOBJECT!!!\n"));
					return 0;
				}

				/*跳过当前item_data 数据*/
				cur_off += item_data_len;
			}
		}
	}

	return 0;
}

double double_convert( unsigned char* buf)
{
	unsigned char		double_value[8];
	double*				p_double = (double*)double_value;

	double_value[0] = buf[7];
	double_value[1] = buf[6];
	double_value[2] = buf[5];
	double_value[3] = buf[4];
	double_value[4] = buf[3];
	double_value[5] = buf[2];
	double_value[6] = buf[1];
	double_value[7] = buf[0];
	return *p_double;
}

int test_flv_parse_input(unsigned int offset, unsigned char* p_buf, unsigned int size)
{
	static FILE*		fp = NULL;

	if( fp == NULL)
	{
		fp = fopen("0.flv", "rb");
		if( fp == NULL)
		{
			return -1;
		}
	}

	fseek( fp, offset, SEEK_SET);
	return fread( p_buf, 1, size, fp);
}

void FLV_PARSE_TEST(void)
{
	int				iret = 0;
	flv_info_t		info;

	flv_parse_set_input(test_flv_parse_input);

	iret = flv_parse_perform(&info);
	if(iret != 0)
	{
		printf("flv_parse_perform failed\n");
		return ;
	}
}

