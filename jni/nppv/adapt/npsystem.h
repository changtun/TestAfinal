#ifndef _NPSYSTEM_H_
#define _NPSYSTEM_H_


#ifdef __cplusplus
extern "C"
{
#endif

    int NpSystem_Init(void);
	int NpSystem_set_charset(int charset_id);
	int NpSystem_set_debug(int debug_val);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* #ifndef _NPSYSTEM_H_ */

