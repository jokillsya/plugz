/* 
 * File:   plugz.h
 * Author: johnathan
 *
 * Created on 31 March 2012, 12:37 PM
 */

#ifndef PLUGZ_H
#define	PLUGZ_H

#define P_CHAR          char
#define P_CHAR_C         const P_CHAR
#define P_STRING        P_CHAR *
#define P_STRING_C       const P_STRING
#define P_INT           int
#define P_BOOL          char
#define TRUE    (char)1
#define FALSE   (char)0

/**
 *File and stat stuff... 
 */
#include <sys/stat.h>
#include <limits.h>

#define EXISTS(path, statstruct)        (stat(path, &statstruct) == 0)
#define MKDIR(dirpath, mode)            (!(mkdir(dirpath, mode) == -1))
#define MKFLE(filepath, mode)           (!(creat(filepath, mode) == -1))

#define P_APP_DIR       "plugz"

#endif	/* PLUGZ_H */

