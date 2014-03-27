#include "pvddefs.h"

U8 Parse_TDT(U32 *UTC_time_m, U32 *UTC_time_l, U8 *data_buffer)
{
    *UTC_time_m = (data_buffer[3] << 8) + data_buffer[4];
    *UTC_time_l = (data_buffer[5] << 16) + (data_buffer[6] << 8) + data_buffer[7];

    return 0;
}


U8 Parse_TOT(U32 *UTC_time_m, U32 *UTC_time_l, U8 *data_buffer)
{
    *UTC_time_m = (data_buffer[3] << 8) + data_buffer[4];
    *UTC_time_l = (data_buffer[5] << 16) + (data_buffer[6] << 8) + data_buffer[7];

    return 0;
}



