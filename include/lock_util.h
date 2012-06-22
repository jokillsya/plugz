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
 * File:   lock_util.h
 * Author: johnathan
 *
 * Created on 31 March 2012, 12:34 PM
 */

#ifndef LOCK_UTIL_H
#define	LOCK_UTIL_H

#define P_LCK_FLE "lock"

pid_t get_locking_pid();
gboolean lock(pid_t pid);

#endif	/* LOCK_UTIL_H */

