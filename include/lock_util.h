/* 
 * File:   lock_util.h
 * Author: johnathan
 *
 * Created on 31 March 2012, 12:34 PM
 */

#ifndef LOCK_UTIL_H
#define	LOCK_UTIL_H

#define P_LCK_FLE "lock"

pid_t get_locking_pid();
P_BOOL lock(pid_t pid);

#endif	/* LOCK_UTIL_H */

