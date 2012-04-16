
#include <fcntl.h>
#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

/**
 *File and stat stuff... 
 */
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>

#include "../include/plugz.h"
#include "../include/lock_util.h"

pid_t get_locking_pid() {

    P_CHAR dirpath[PATH_MAX];
    P_CHAR filepath[PATH_MAX];

    snprintf(dirpath, PATH_MAX, "%s/%s", "/etc", P_APP_DIR);
    snprintf(filepath, PATH_MAX, "%s/%s", dirpath, P_LCK_FLE);

    P_INT fd;
    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = 0;
    lock.l_len = 0;

    fd = open(filepath, O_RDONLY);
    fcntl(fd, F_GETLK, &lock);

    if (lock.l_type == F_UNLCK) {

        return (0);

    }

    return (lock.l_pid);

}

P_BOOL lock(pid_t pid) {

    P_CHAR dirpath[PATH_MAX];
    P_CHAR filepath[PATH_MAX];

    snprintf(dirpath, PATH_MAX, "%s/%s", "/etc/", P_APP_DIR);
    snprintf(filepath, PATH_MAX, "%s/%s", dirpath, P_LCK_FLE);

    P_INT fd;
    struct flock fl;
    fd = open(filepath, O_RDWR);
    if (fd == -1)
        return FALSE;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = pid;
    if (fcntl(fd, F_SETLK, &fl) == -1)
        return FALSE;
    return TRUE;

}
