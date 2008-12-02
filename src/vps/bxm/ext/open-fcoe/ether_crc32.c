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

#include <sys/types.h>
#include "ether_crc32.h"

/*
 * Ethernet CRC32 polynomials (big- and little-endian verions).
 * These endiannesses refer to the bit order being transmitted, not the
 * host byte order.
 */
#define ETHER_CRC_POLY_LE   0xedb88320
#define ETHER_CRC_POLY_BE   0x04c11db6

u_int32_t
ether_crc32_le_partial(const u_int8_t *buf, size_t len, u_int32_t crc)
{
        static const u_int32_t crctab[] = {
                0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
                0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
                0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
                0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
        };
        size_t i;

        for (i = 0; i < len; i++) {
                crc ^= buf[i];
                crc = (crc >> 4) ^ crctab[crc & 0xf];
                crc = (crc >> 4) ^ crctab[crc & 0xf];
        }

        return (crc);
}

u_int32_t
ether_crc32_le(const u_int8_t *buf, size_t len)
{
        return (ether_crc32_le_partial(buf, len, ETHER_CRC32_SEED));
}

u_int32_t
ether_crc32_be_partial(const u_int8_t *buf, size_t len, u_int32_t crc)
{
        size_t i;
        u_int32_t carry;
        int bit;
        u_int8_t data;

        for (i = 0; i < len; i++) {
                for (data = *buf++, bit = 0; bit < 8; bit++, data >>= 1) {
                        carry = ((crc & 0x80000000) ? 1 : 0) ^ (data & 0x01);
                        crc <<= 1;
                        if (carry) {
                                crc = (crc ^ ETHER_CRC_POLY_BE) | carry;
                        }
                }
        }

        return (crc);
}

u_int32_t
ether_crc32_be(const u_int8_t *buf, size_t len)
{
        return (ether_crc32_be_partial(buf, len, ETHER_CRC32_SEED));
}
