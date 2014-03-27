

#include "sqlite3.h"


#include "npdatabase.h"
#include "log/anpdebug.h"



static sqlite3 *MainDB;
static sqlite3_stmt *stmt = NULL;

static char *db_sql_str = NULL;
static char *db_ret_buffer = NULL;



static int db_bug = 1;
void database_debug(char *str_temp)
{
    if(db_bug == 0)
        return;
    DebugMessage("===   PXWANG	database_debug	CallBy Function:%s", str_temp);
}

int NpDatabase_Debug(int flag)
{
    db_bug = flag;
	return 0;
}


int NpDatabase_Open(char *db_path)
{
    int ret = 0;

	database_debug(__FUNCTION__);

    if(db_path == NULL)
    {
        DebugMessage("	pxwang	====		NpDatabase_Open:	db_path== NULL");
		return 1;
    }

    DebugMessage("	pxwang	====		Open_DB:	%s", db_path);
    ret = sqlite3_open( db_path, &MainDB );
    if( ( SQLITE_OK != ret ) || ( NULL == MainDB ) )
    {
        return ret;
    }
    return SQLITE_OK;
}

int NpDatabase_Close(void)
{
    int ret = 0;
	
	database_debug(__FUNCTION__);
    ret = sqlite3_close(MainDB);
    if(SQLITE_OK != ret )
    {
        return ret;
    }
    return SQLITE_OK;
}

int NpDatabase_Exe(char *com_str)
{
    int ret = 0;

	database_debug(__FUNCTION__);
    NpDatabase_Sqlstr(com_str);

    DebugMessage("		=============			NpDatabase_Exe : [%s] ", db_sql_str);
    ret = sqlite3_exec( MainDB, db_sql_str, NULL, NULL, NULL );
    if(SQLITE_OK != ret )
    {
       return ret;
    }

    return SQLITE_OK;
}


int NpDatabase_Prepare(char *com_str)
{
    int ret = 0;

	database_debug(__FUNCTION__);
    /*  防止上次的 stmt 没有释放 ，丢内存 */
    NpDatabase_Finalize();

    NpDatabase_Sqlstr(com_str);

    DebugMessage("		=============			NpDatabase_Prepare		[%s]", db_sql_str);

    ret = sqlite3_prepare( MainDB, db_sql_str, -1, &stmt, NULL );
    if ((ret != SQLITE_OK) && (ret != SQLITE_ROW) && (ret != SQLITE_DONE))
    {
        DebugMessage(("Step Error: %s\n", sqlite3_errmsg(MainDB)));
        NpDatabase_Finalize();

        return ret;
    }

    DebugMessage("		=============			NpDatabase_Prepare		%x", stmt);

    return 0;
}

int NpDatabase_Step(void)
{
    int ret = 0;
	
    database_debug(__FUNCTION__);
    ret = sqlite3_step(stmt);
    if ((ret != SQLITE_OK) && (ret != SQLITE_ROW) && (ret != SQLITE_DONE))
    {
        DebugMessage(("Step Error: %s\n", sqlite3_errmsg(MainDB)));
        NpDatabase_Finalize();
        return ret;
    }


    return 0;
}

int NpDatabase_Finalize(void)
{
    int ret = 0;

	database_debug(__FUNCTION__);
    if (stmt)
    {
        ret = sqlite3_finalize(stmt);
        stmt = NULL;
        if(SQLITE_OK != ret )
        {
            DebugMessage("Finallize Error: %s\n", sqlite3_errmsg(MainDB));
            return ret;
        }
    }

    return 0;
}

int NpDatabase_Column_Int(int index)
{
    int ret = 0;

    database_debug(__FUNCTION__);
    ret = sqlite3_column_int(stmt, index);

    DebugMessage("NpDatabase_Column_Int: %d  %d\n", index, ret);


    return ret;

}

double NpDatabase_Column_Double(int index)
{
    double ret = 0;

    ret = sqlite3_column_double(stmt, index);
    return ret;
}

char *NpDatabase_Column_Text(int index)
{
    char *ret = NULL;
    int ustrlen;

    database_debug(__FUNCTION__);
    ret = sqlite3_column_text( stmt, index);
    NpDatabase_Retstr_Free();
    ustrlen = strlen(ret);
    if(ustrlen > 0)
    {
        db_ret_buffer = (char *)malloc(ustrlen + 10);
        if(db_ret_buffer == NULL)
        {
            return db_ret_buffer;
        }
        sprintf( db_ret_buffer, "%s", ret);
    }
    return ret;
}


void NpDatabase_Sqlstr(char *sql_str)
{
    int ustrlen;

    if(db_sql_str != NULL)
    {
        free(db_sql_str);
        db_sql_str = NULL;
    }

    ustrlen = strlen(sql_str);
    if(ustrlen > 0)
    {
        db_sql_str = (char *)malloc(ustrlen + 10);
        if(db_sql_str == NULL)
        {
            return db_sql_str;
        }
        sprintf( db_sql_str, "%s", sql_str);
    }
}

void NpDatabase_Retstr_Free(void)
{
    if(db_ret_buffer != NULL)
    {
        free(db_ret_buffer);
        db_ret_buffer = NULL;
    }
}

void NpDatabase_Sqlstr_Free(void)
{
    if(db_sql_str != NULL)
    {
        free(db_sql_str);
        db_sql_str = NULL;
    }
}



