/* 
 * File:   plugz_db.h
 * Author: johnathan
 *
 * Created on 17 April 2012, 11:30 PM
 */

#ifndef PLUGZ_DB_H
#define	PLUGZ_DB_H

#define SQLITE_STR      "sqlite:///etc/plugz/plugz.db?synchronous=normal&heap_limit=4096&foreign_keys=on;"
#define POOL_SIZE       25

/**
 * We'll hard code the type values for now, create a types table later.
 * 
 * 1 = IPC
 * 2 = TPC/IP
 */
#define PLUGZ_TABLE_INIT       "CREATE TABLE IF NOT EXISTS plugz         \
                                        (       code CHAR(8),            \
                                                type INT,                \
                                                con_str TEXT,            \
                                                port INT,                \
                                                PRIMARY KEY (code)       \
                                        );                               \
                                "

#define PLUGZ_SET              "INSERT INTO plugz        \
                                        (                \
                                                code,    \
                                                type,    \
                                                con_str, \
                                                port     \
                                        )                \
                                 VALUES(?, ?, ?, ?);     \
                                "

#define PLUGZ_GET              "SELECT type, con_str, port FROM plugz WHER code = ?"

typedef struct plug_t {
    const char *code;
    int         type;
    const char *con_str;
    int         port;
} plug_t;

gboolean init_db();
gboolean get_plug(const char *proc_code, plug_t *buffer);
gboolean set_plug(plug_t *plug_t);

#endif	/* PLUGZ_DB_H */

