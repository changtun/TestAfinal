/******************************************************************************
* <crc.h>
*
* 2005-4-5
*****************************************************************************/

#ifndef __CRC_H_
#define __CRC_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define CRC32_MAX_COEFFICIENTS	256
#define CRC16_MAX_COEFFICIENTS	256

void CRC_crc32Calculate(unsigned char *buffer, unsigned int size, unsigned int *CRC32);
void CRC_crc16Calculate(unsigned char *buffer, unsigned int size, unsigned short *CRC16);
unsigned int pv_crc32(void *buff, unsigned int length, int block_len);

#ifdef __cplusplus
}
#endif

#endif	/* __CRC_H_ */

