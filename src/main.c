/**
 * This file is part of plugz
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */


/* 
 * File:   main.c
 * Author: johnathan
 *
 * Created on 31 March 2012, 12:16 PM
 */
#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <argtable2.h>

/**
 *File and stat stuff... 
 */
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <string.h>

//#include "../include/serv_zmq_layer.h"
#include "../include/server.h"
#include "../include/plugz.h"
#include "../include/plugz_db.h"
#include "../include/lock_util.h"

static void register_module(const char *code, const char *ipc, const char *s_port) {

    plug_t plug;
    plug.code = code;
    plug.con_str = ipc;
    //I know it's dodgy - leave me alone - I'll fix it later...
    plug.port = atoi(s_port);
    
    gboolean l_valid = FALSE;

    if(strstr(ipc, "ipc://") != NULL) {
        
        plug.type = 1;
        l_valid = TRUE;
        
    } else if (strstr(ipc, "tcp://") != NULL) {
        
        plug.type = 2;
        l_valid = TRUE;
        
    } else {
        
        l_valid = FALSE;
        printf("Invalid Module Location\n");
        
    }

    if(l_valid == TRUE) set_plug(&plug);

}

static gboolean is_root() {
    if (geteuid() != 0) {
        return FALSE;
    }
    return TRUE;
}

static gboolean dir_init() {
    gchar dirpath[PATH_MAX];
    gchar filepath[PATH_MAX];
    snprintf(dirpath, PATH_MAX, "%s/%s", "/etc", P_APP_DIR);
    struct stat st;
    if (!EXISTS(dirpath, st)) {
        if (!MKDIR(dirpath, O_CREAT)) {
            return FALSE;
        }
    }
    snprintf(filepath, PATH_MAX, "%s/%s", dirpath, P_LCK_FLE);
    if (!EXISTS(filepath, st)) {
        if (!MKFLE(filepath, O_CREAT)) {
            return FALSE;
        }
    }
    return TRUE;
}

static void start(gint s, gint k, gint r, const gchar **strings, gint nstrings) {
    printf("\n");
    if ((nstrings > 0) && !r) {
        printf("module strings specified without -r option, ignoring module strings...\n");
    }
    if (is_root()) {
        init_db();
        if (r) {
            if (nstrings != 3) {
                printf("invalid options specified with the -r flag, could not register module.");
            } else {
                register_module(strings[0], strings[1], strings[2]);
            }
        }
        if (s || k) {

            if (s && !k) {
                if (!dir_init()) {
                    printf("unable to initialize directories!\n");
                    exit(EXIT_FAILURE);
                }
                printf("starting up plugz service...\n");
                start_server();
                printf("\n");
            }
            if (k && !s) {
                printf("killing plugz...\n");
                stop_server();
            }
            if (s && k) {
                printf("ignored -s and -k flags: -s and -k options were used together, why would you want to do that!?\n");
            }
        }
    } else {
        printf("the plugz service says: \"My heart belongs to root and root alone.\"\n");
    }
    printf("\n");
}

int main(gint argc, gchar **argv) {

    struct arg_lit *plugz_start_opt = arg_lit0("s", "start", "Start the plugz server if it isn't already started.");
    struct arg_lit *plugz_stop_opt = arg_lit0("k", "kill", "Kill the plugz server if it is started.");
    struct arg_lit *plugs_reg_mod_opt = arg_lit0("r", "register", "Register a module.");
    struct arg_str *strs = arg_strn(NULL, NULL, "STRING", 0, 3, "<Module code> <Module location(ipc/tcp)> <port>");
    struct arg_lit *help = arg_lit0(NULL, "help", "print this help and exit");
    struct arg_end *end = arg_end(20);
    gpointer argtable[] = {plugz_start_opt, plugz_stop_opt, help, plugs_reg_mod_opt, strs, end};
    const char *progname = "plugz";
    int nerrors;
    /* verify the argtable[] entries were allocated sucessfully */
    if (arg_nullcheck(argtable) != 0) {
        /* NULL entries were detected, some allocations must have failed */
        printf("%s: insufficient memory\n", progname);
        return EXIT_FAILURE;
    }

    /* Parse the command line as defined by argtable[] */
    nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0) {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Echo the STRINGs to standard output.\n\n");
        arg_print_glossary(stdout, argtable, "  %-10s %s\n");
        printf("\nPlugz usage: plugz <options> \n\n"
                "  Options include:\n"
                "  -s or --start: Start the plugz service\n"
                "  -k or --kill: Kill the plugz service\n"
                "  -r or --register: Register a module with plugz, example: -r <module code> <module location>\n"
                "Report bugs to <jokillsya@gmail.com>.\n");
        return EXIT_SUCCESS;
    }

    /* If the parser returned any errors then display them and exit */
    if (nerrors > 0) {
        /* Display the error details contained in the arg_end struct.*/
        arg_print_errors(stdout, end, progname);
        printf("Try '%s --help' for more information.\n", progname);
        return EXIT_FAILURE;
    }

    start(plugz_start_opt->count, plugz_stop_opt->count, plugs_reg_mod_opt->count, strs->sval, strs->count);

    return EXIT_SUCCESS;

}

