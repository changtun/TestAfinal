
#ifndef _NPDVB_H_
#define _NPDVB_H_


#ifdef __cplusplus
extern "C"
{
#endif



    int NpDvb_Debug(int flag);
	int NpDvb_Init(void);
	int NpDvb_term(void);
	int NpDvb_Play_c(int f_v, int s_v, int m_v, int se_v, int p_v);
	int NpDvb_Play_t(int f_v, int b_v, int se_v, int p_v);
	int NpDvb_Stop(void);
	int NpDvb_volume(int volume_v);
	int NpDvb_Area(int x, int y, int width, int height);
	int NpDvb_DbPath(char *path_str);
	void NpDvb_str(char *chname_str);
	void NpDvb_db_str(char *path_str);


#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* #ifndef _NPDVB_H_ */
