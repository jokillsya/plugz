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
 * File:   plugz.h
 * Author: johnathan
 *
 * Created on 31 March 2012, 12:37 PM
 */

#ifndef PLUGZ_H
#define	PLUGZ_H

#define EXISTS(path, statstruct)        (stat(path, &statstruct) == 0)
#define MKDIR(dirpath, mode)            (!(mkdir(dirpath, mode) == -1))
#define MKFLE(filepath, mode)           (!(creat(filepath, mode) == -1))

#define P_APP_DIR       "plugz"

/**
 * Bit Utility macros... And so it begins...
 * 
 * The devil is in the details - but we'll just pretend he's asleep for now...
 */
#define I_B_CALC(type) (sizeof(type) * CHAR_BIT - 1)
#define B2_POW(x) ((x == 0) ? 1 : 2 << (x - 1))
#define PL_MIN(x, y) (y + ((x - y) & ((x - y) >> I_B_CALC(x))))
#define PL_MAX(x, y) (x - ((x - y) & ((x - y) >> I_B_CALC(x))))
#define BIT_EXTR(bl_1, bl_2, d) ((d & (PL_MAX(B2_POW(bl_1), B2_POW(bl_2)) - PL_MIN(B2_POW(bl_1), B2_POW(bl_2))) >> bl_1))

#endif	/* PLUGZ_H */

