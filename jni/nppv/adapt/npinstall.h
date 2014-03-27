
#ifndef _NPINSTALL_H_
#define _NPINSTALL_H_


#ifdef __cplusplus
extern "C"
{
#endif

    typedef void(*npsearch_Notify_pfn)(int eNotifyType, char *str_buffer);

    int NpInstallation_Init(void);
    int NpInstallation_Register(npsearch_Notify_pfn result_cb);
    int NpInstallation_Deinit(void);
    int NpInstallation_Start(char *params_str);
    int NpInstallation_Stop(void);
    int NpInstallation_Get_SearchRet(void);
    int NpInstallation_Free_SearchRet(void);
    int NpInstallation_Get_Tptotal(void);
    int NpInstallation_Get_Chtotal(void);
    int NpInstallation_Debug(int flag);
    void NpInstallation_Retstr_Free(void);


#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* #ifndef _NPINSTALL_H_ */
