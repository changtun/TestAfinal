
#ifndef _NP_DATABASE_H_
#define _NP_DATABASE_H_


#ifdef __cplusplus
extern "C"
{
#endif

    int NpDatabase_Debug(int flag);
    int NpDatabase_Open(char *db_path);
    int NpDatabase_Close(void);
    int NpDatabase_Exe(char *com_str);
    int NpDatabase_Prepare(char *com_str);
    int NpDatabase_Step(void);
    int NpDatabase_Finalize(void);
    int NpDatabase_Column_Int(int index);
    double NpDatabase_Column_Double(int index);
    char *NpDatabase_Column_Text(int index);
    void NpDatabase_Sqlstr(char *sql_str);
    void NpDatabase_Retstr_Free(void);
    void NpDatabase_Sqlstr_Free(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* #ifndef _NP_DATABASE_H_ */

