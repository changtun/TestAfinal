#ifndef _NPHLS_H_
#define _NPHLS_H_


#ifdef __cplusplus
extern "C"
{
#endif

	typedef void(*nphls_Notify_pfn)(int eNotifyType, char *str_buffer);
	int NpHls_Register(nphls_Notify_pfn result_cb);
	int NpHLS_Debug(int flag);
    int NpHLS_Init(void);
    int NpHLS_Term(void);
    int NpHLS_Start(int play_type,char *Play_url);
	int NpHLS_Seek_Next(void);
    int NpHLS_Stop(void);	
	void NpHLS_CallBack(int type);
	int NpHLS_List_Add(char *Play_url);
	
JNIEXPORT jint JNICALL Java_com_pbi_dvb_nppvware_NpHls_HlsRegisterCallback
(JNIEnv *env, jobject obj, jobject objData);


#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* #ifndef _NPHLS_H_ */

