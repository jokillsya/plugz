#include <zdb.h>
#include <stdlib.h>
#include <string.h>
#include "../include/plugz.h"
#include "../include/plugz_db.h"

static URL_T url;
static ConnectionPool_T pool;

P_INT init_db() {

    url = URL_new(SQLITE_STR);
    pool = ConnectionPool_new(url);

    if (pool == NULL) return (EXIT_FAILURE);

    ConnectionPool_start(pool);
    ConnectionPool_setMaxConnections(pool, POOL_SIZE);
    
    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T stmt = Connection_prepareStatement(con, PLUGZ_TABLE_INIT);
    
    TRY
    {
        PreparedStatement_execute(stmt);
    }
    CATCH(SQLException)
    {
        printf("Unable to initialize Database!\n");
        exit(-1);
    }
    FINALLY
    {
        Connection_close(con);
    }
    END_TRY;

    return EXIT_SUCCESS;

}

int db_get_prod_count()
{
    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T get_prod_stmt = Connection_prepareStatement(con, "   ");

    ResultSet_T result = PreparedStatement_executeQuery(get_prod_stmt);

    int count = 0;

    if (ResultSet_next(result))
    {

        count = ResultSet_getInt(result, 1);

    }

    Connection_close(con);

    return count;

}

P_BOOL get_plug(P_STRING_C proc_code, P_STRING *buffer) {

    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T stmt = Connection_prepareStatement(con, PLUGZ_GET);

    PreparedStatement_setString(stmt, 1, proc_code);
    ResultSet_T result = PreparedStatement_executeQuery(stmt);

    P_STRING_C res;

    if (ResultSet_next(result)) {

        res = ResultSet_getString(result, 1);
        *buffer = malloc(strlen(res));
        strcpy(*buffer, res);

    }

    Connection_close(con);

    if (res == NULL) {

        return FALSE;

    }

    return TRUE;


}

//plug * plug ->> hehehe...
P_BOOL set_plug(plug_t * plug_t) {

    P_BOOL r_val;

    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T stmt = Connection_prepareStatement(con, PLUGZ_SET);

    PreparedStatement_setString(stmt, 1, plug_t->code);
    PreparedStatement_setInt(stmt, 2, plug_t->type);
    PreparedStatement_setString(stmt, 3, plug_t->con_str);

    TRY
    {

        PreparedStatement_execute(stmt);
        r_val = TRUE;

    }

    CATCH(SQLException) {

        r_val = FALSE;

    }
    FINALLY
    {

        Connection_close(con);

    }
    END_TRY;

    return r_val;

}

