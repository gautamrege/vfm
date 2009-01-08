/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef __MLX_FIP_H__
#define __MLX_FIP_H__

/*
 * ---- Mellanox Plug-in for VFM ----
 * This header file contains all Mellanox specific headers.
 */

/*
 * Mellanox Gateway
 */
typedef struct __mlx_gateway {
        uint8_t  ext_ports;
        uint8_t  flags;
        uint16_t gw_id;        /* Gateway id :LSBs 12 bits */
        /* TODO:External port status: 30 bytes=15 ports * 2 bits */
        uint32_t ports_stats;
}mlx_gateway;

/*
 * --- Mellanox Specific BridgeX Adverisment  (Ref: Table 11) ---
 */
typedef struct __mlx_bridgeX {
        uint16_t gw_num;        /* Gateway number */
        uint16_t db_id;         /* Database ID :Initially 0x0 */
        /*
         * ref: Table5, mac is allocated 2 DWORDS Hence we need a
         * reserved field too
         */
        uint16_t reserved;
        uint8_t last_bc_mac[6]; /* Last associated BC mac address*/
        /*
         * Pointer to the gateway array. Number of elements is decided
         * by gw_num
         */
        mlx_gateway *gw_arr;
}mlx_bridgeX;

/*
 * --- Mellanox Specific BridgeX Adverisment  (Ref: Table 15) ---
 */
typedef struct __mlx_adv_res{
        uint16_t gwid;          /* Gateway id: Comes in Bx Advertisement */
        uint16_t db_id;         /* Database ID: VFM sets this for new Bx */
        uint8_t cluster_mac[6];          /* TODO: Where does it come from */
        uint8_t gateway_mac[6];          /* TODO: Where does it come from */
}mlx_adv_res;

#endif /* __MLX_FIP_H__ */
