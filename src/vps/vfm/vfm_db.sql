create table bridges (
    mac char(6) NOT NULL,                   /* MAC: 48-bits */
    node_name char(8),                      /* 64-bits */

    db_id char(2),                          /* 16-bits */
    last_bc_mac char(6),                    /* MAC: 48-bits */
    max_recv int,

    vendor varchar(64),                     /* eg. Mellanox */
    model_number varchar(64) default '',
    fw_version varchar(32) default '',

    primary key (mac)
);
    

/* bridges has_many gateways
 * Number of records for a gw_id determine how many gateways there are 
 * for a bridge
 */
create table gateways (
    bridge_id char(6) NOT NULL, /* MAC address */
    gw_id char(2) NOT NULL,     /* 12-bits only */

    flag_ep int,                /* External Protocol: EN=0, FC=1 */
    flag_sp int,                /* Ingored */
    flag_se int,                /* Ingored */
    flag_f int,                 /* Flood Unknown */
    flag_es int,                /* Egress secure mode */
    flag_is int,                /* Ingress secure mode */
    flag_l int,                 /* L2 Ethernet address lookup */
    flag_extra int DEFAULT 0,   /* Any extra flags */

    ips int,                    /* Internal port status. */
                                /* Every Gateway has ONLY 1 internal port */
    heartbeat timestamp,        /* The heart-beat is updated when an adv is recevied from the bridge */
    conn_speed char(4),          /* Connection speed */

    primary key (gw_id)
    /* mysql REF: foreign key (bridge_id) references mlx_bridges(mac) */
);

/* one gateway has 15 external ports. */
create table external_ports (
    gw_id char(2) NOT NULL,              /* 12-bits */
    id int,                              /* 2 to 16 */
    max_speed varchar(16),               /* Max connection speed */
    current_speed varchar(16),           /* Current connection speed */
    status int                           /* 0,1,2 or 3 only. */
    /* mysql REF: foreign key (gw_id) references gateways(gw_id) */
);

create table hosts(
    name char(8),                            /* 64-bits */

    /* CNA heart-beat takes precedence over host heartbeat. This means that if we 
     * receive a CNA heartbeat, it implies the host heart-beat. This record may
     * be updated with the latest CNA heartbeat associated with the host. 
     * This field will be important for CNA which cannot sent solicitation and rely
     * on a host-agent plugin to send them!
     */
    heartbeat timestamp,                     

    primary key (name)
);

/* Converged Network Adaptors */
create table cna (
    mac char(6) NOT NULL,                    /* MAC: 48-bits */
    host_id char(8),                         /* host id */
    max_recv int,

    /* This record gets updated when we receive an adv from the cna */
    heartbeat timestamp,               

    primary key (mac)
);

create table vhba (
    fcf_mac char(6) NOT NULL,        /* MAC: 48-bits */
    name char(8),                    /* 64-bits */
    port char(3),                    /* 24-bit port id, fc_id */
    priority int,                    /* 16-bit priority */
    fc_map int,             
    switch_name char(8),
    fabric_name char(8),
    freq_keep_alive int, 
    heartbeat timestamp,             /* keep-alive */

    status  int,                     /* initializing, connected, disconnected */

    /* mysql REF: foreign key(mac) references hosts(mac), */
    primary key(fcf_mac)
);

/** This is the context table entry.
 * type = 0 implies it is a vHBA entry and the fields 'fc_id' and 'host_mac'
 * are valid. A context table will have 1 entry of this kind per vHBA.
 *
 * type = 1 implies the Gateway internal address. 'gateway_mac' field applies
 * for this type. A context table will only have 1 entry of this type.
 */
create table vhba_fabric (
    valid int,              /* flag for determining host or gateway */
    fc_id char(3),
    vhba_mac char(6),
    gw_id char(2)
    /* mysql REF: foreign key(vhba_mac) references hosts(mac), */
    /* mysql REF: foreign key(gw_id) references gateways(gw_id), */
);
