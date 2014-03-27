
#ifndef _NPSYSTEM_H_
#define _NPSYSTEM_H_


#ifdef __cplusplus
extern "C"
{
#endif

    int Code_Init(void);
    int Code_Set_Default(int set_val);
    char *Code_ToChar(int CodeType, char *str, int strLen );


#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* #ifndef _NPSYSTEM_H_ */




