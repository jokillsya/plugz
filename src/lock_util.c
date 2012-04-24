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

    char dirpath[PATH_MAX];
    char filepath[PATH_MAX];

    snprintf(dirpath, PATH_MAX, "%s/%s", "/etc", P_APP_DIR);
    snprintf(filepath, PATH_MAX, "%s/%s", dirpath, P_LCK_FLE);

    int fd;
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

    char dirpath[PATH_MAX];
    char filepath[PATH_MAX];

    snprintf(&dirpath[0], PATH_MAX, "%s/%s", "/etc/", P_APP_DIR);
    snprintf(&filepath[0], PATH_MAX, "%s/%s", dirpath, P_LCK_FLE);

    int fd;
    struct flock fl;
    fd = open(&filepath[0], O_RDWR);
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
