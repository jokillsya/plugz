#include <glib-2.0/glib.h>
#include <zdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/plugz.h"
#include "../include/plugz_db.h"

static URL_T url;
static ConnectionPool_T pool;

gboolean init_db() {

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

    return TRUE;

}

gboolean get_plug(const gchar *proc_code, plug_t *buffer) {
    
    gboolean ret = FALSE;

    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T stmt = Connection_prepareStatement(con, PLUGZ_GET);

    PreparedStatement_setString(stmt, 1, proc_code);
    ResultSet_T result = PreparedStatement_executeQuery(stmt);

    //TODO: Handle Exceptions...
    if (ResultSet_next(result)) {
        
        buffer->code = proc_code;
        buffer->type = ResultSet_getInt(result, 1);
        strcpy((char *)buffer->con_str, ResultSet_getString(result, 2));
        buffer->port = ResultSet_getInt(result, 3);
        
        ret = TRUE;

    }

    Connection_close(con);

    return ret;

}

//plug * plug ->> hehehe...
gboolean set_plug(plug_t *plug_t) {

    gboolean r_val;

    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T stmt = Connection_prepareStatement(con, PLUGZ_SET);

    PreparedStatement_setString(stmt, 1, plug_t->code);
    PreparedStatement_setInt(stmt, 2, plug_t->type);
    PreparedStatement_setString(stmt, 3, plug_t->con_str);
    PreparedStatement_setInt(stmt, 4, plug_t->port);

    TRY
    {

        PreparedStatement_execute(stmt);
        r_val = TRUE;

    }

    CATCH(SQLException) {
        
        printf("Error: %s\n", Connection_getLastError(con));

        r_val = FALSE;

    }
    FINALLY
    {

        Connection_close(con);

    }
    END_TRY;

    return r_val;

}

