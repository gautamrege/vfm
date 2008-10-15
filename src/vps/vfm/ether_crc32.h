/*
 * Copyright(c) 2007 Intel Corporation. All rights reserved.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 * Maintained at www.Open-FCoE.org
 */

#ifndef _LIBSA_TPS_ETHER_CRC32_H_
#define _LIBSA_TPS_ETHER_CRC32_H_

u_int32_t ether_crc32_be(const u_int8_t *, size_t);
u_int32_t ether_crc32_le(const u_int8_t *, size_t);

/*
 * These variants are for doing a CRC on a scattered buffer.  Start with
 * the seed of 0xffffffff, and feed the partial CRC value from the first
 * segment into the next, etc.
 */
#define ETHER_CRC32_SEED    0xffffffffUL

u_int32_t ether_crc32_be_partial(const u_int8_t *, size_t, u_int32_t seed);
u_int32_t ether_crc32_le_partial(const u_int8_t *, size_t, u_int32_t seed);

#endif /* _LIBSA_TPS_ETHER_CRC32_H_ */
