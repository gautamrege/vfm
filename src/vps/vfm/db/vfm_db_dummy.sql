BEGIN TRANSACTION;
CREATE TABLE User_Account (
                user varchar(20),
                password varchar(20)
);
CREATE TABLE vfm_bridge_device (
    guid char(8) NOT NULL,          /* Bridge GUID */

    vfm_guid char(8),               /* Guid of VFM managing this device */
    name varchar(64),
    `desc` varchar(64),
    firmware_version varchar(64),

    primary key(guid)
);
INSERT INTO "vfm_bridge_device" VALUES('1234', '5678', 'Bridge1', 'Dummy Bridge', '1.0');
CREATE TABLE vfm_gateway_attr(
    gw_id int NOT NULL,             /* Unique GW_ID in the Gateway cluster */

    vfm_guid char(8) NOT NULL,      /* VFM guid */
    physical_index varchar(64),    /* user defined name */
    `desc` varchar(64),             /* user defined description */
    int_protocol int,
    ext_protocol int,

    int_port int NOT NULL,          /* internal port */
    ext_port int NOT NULL,          /* external port */

    ingress_learning int,
    ingress_secure int,
    egress_secure int,
    flood int,
    checksum_offload int,
    component_mask int,

    foreign key(int_port) references vfm_port_attr(id),
    foreign key(ext_port) references vfm_port_attr(id),
    primary key (gw_id)
);
INSERT INTO "vfm_gateway_attr" VALUES(11, '5678', NULL, NULL, NULL, NULL, 1, 2, NULL, NULL, NULL, NULL, NULL, NULL);
CREATE TABLE vfm_gw_module_attr (
    gw_module_id int NOT NULL,      /* gateway module id */    
    vfm_bridge_guid char(8) NOT NULL,   /* bridge device this gateway is on */

    foreign key(vfm_bridge_guid) references vfm_bridge_device(guid),
    primary key(gw_module_id)
);
INSERT INTO "vfm_gw_module_attr" VALUES(1, '1234');
CREATE TABLE vfm_io_module_attr(
    id int,                         /* interface id (unique???) */
    name varchar(64),               /* user defined name */

    `type` int,                     /* EN / IB module */
    mac char(6),                    /* Only for EN module */
    guid char(8),                   /* Only for IB module */ 
    num_vhba int,
    num_vnic int,
    slot int,
    port int,
    supported_speed int,
    primary key(id)
);
CREATE TABLE vfm_port_attr (
    id INTEGER PRIMARY KEY autoincrement,
    gw_module_id int NOT NULL,      /* Gateway module this port is with */
    port_id int,

    name varchar(64),               /* Name of the port */
    `type` int NOT NULL,            /* Internal or External */
    protocol int NOT NULL,          /* EN, IB, FC */
    mtu int,                        /* Max transmission unit */
    supported_speed int,            /* Supported speed */

    foreign key (gw_module_id) references vfm_gw_module_attr(gw_module_id)
);
INSERT INTO "vfm_port_attr" VALUES(1, 1, NULL, NULL, 1, 1, NULL, NULL);
INSERT INTO "vfm_port_attr" VALUES(2, 1, NULL, NULL, 2, 1, NULL, NULL);
DELETE FROM sqlite_sequence;
INSERT INTO "sqlite_sequence" VALUES('vfm_port_attr', 2);
CREATE TABLE vfm_port_en_attr (
    vfm_port_id int NOT NULL,       /* the port id database reference */
    mac char(6),                    /* MAC address of the port */
    foreign key (vfm_port_id) references vfm_port_attr(id)
);
CREATE TABLE vfm_port_fc_attr (
    vfm_port_id int NOT NULL,       /* the port id database reference */
    wwnn char(8),
    wwpn char(8),
    fcid char(3),
    foreign key (vfm_port_id) references vfm_port_attr(id)
);
CREATE TABLE vfm_port_ib_attr (
    vfm_port_id int NOT NULL,       /* the port id database reference */
    ib_guid char(8),                /* IB guid */
    foreign key (vfm_port_id) references vfm_port_attr(id)
);
CREATE TABLE vfm_vadapter_attr (
    id int NOT NULL,                /* VFM asssigned unique id */
    io_module_id int,               /* CNA on which this vadapter resides */
    vfabric_id int,                      /* The gateway id */
    status char(10),
    name varchar(64),
    `desc` varchar(64),
    init_type char(10),                  /* host / network initiated */
    protocol varchar(10),                   /* vNIC or vHBA */
    assignment_type int,			/* How to assign protocol */
    component_mask int,
    foreign key(io_module_id) references vfm_io_module_attr(id),
    foreign key(vfabric_id) references vfm_vfabric_attr(id),
    primary key (id)
);
CREATE TABLE vfm_vadapter_en_attr (
    vadapter_id int NOT NULL,       /* database adaptor id */

    mac char(6),                    /* mac address of the vadapter */
    promiscuous char(10),
    silent_listener char(10),
    vlan varchar(64),               /* Comma separated vland ids */
    component_mask int,
    foreign key(vadapter_id) references vfm_vadapter_attr(id)
);
CREATE TABLE vfm_vadapter_fc_attr (
    vadapter_id int NOT NULL,       /* database adaptor id */

    wwnn char(8),
    wwpn char(8),
    fc_id char(8),
    spma int,
    fmpa int,
    component_mask int,
    foreign key(vadapter_id) references vfm_vadapter_attr(id)
);
CREATE TABLE vfm_vfabric_attr (
    id INTEGER PRIMARY KEY autoincrement,
    primary_gw_id int,
    backup_gw_id int,
    name varchar(64),               /* user defined name */
    `desc` varchar(64),             /* user defined description */
    `type` int,            /* type of fabric: EN / FC */
    ctx_table_id int,               /* The context table of the vHUB/vNPIV-Set. */
    protocol int,
    auto_failover int,
    auto_failback int,
    component_mask int,

    foreign key(primary_gw_id) references vfm_gateway_attr(gw_id)
);
CREATE TABLE vfm_vfabric_en_attr (
    vfabric_id int NOT NULL,    /* vfabric id */
    vlan int,
    mac char(6),                /* MAC address assigned to a gateway port */
    component_mask int,
    foreign key(vfabric_id) references vfm_vfabric_attr(id)
);
CREATE TABLE vfm_vfabric_fc_attr (
    vfabric_id int NOT NULL,    /* vfabric id */
    fcid int,                   /* 24 bit FCID of Gateway external FC port.*/
    wwnn char(8),
    wwpn char(8),
    component_mask int,
    foreign key(vfabric_id) references vfm_vfabric_attr(id)
);
CREATE VIEW `v_vfm_bridge_attr` AS
select  guid, name, `desc`, count(vfm_gw_module_attr.vfm_bridge_guid) as num_gw_module, firmware_version, vfm_guid from vfm_bridge_device left outer join vfm_gw_module_attr on 
vfm_bridge_device.guid = vfm_gw_module_attr.vfm_bridge_guid group by vfm_bridge_device.guid;
CREATE VIEW `v_vfm_gateway_attr` AS
select  gw_id, physical_index, `desc`, int_port, ext_port, ingress_learning, ingress_secure from vfm_gateway_attr;
CREATE VIEW `v_vfm_port_attr_by_bridge` AS
select  port_id, type, protocol, mtu, supported_speed, vfm_bridge_device.guid as guid from vfm_port_attr join vfm_gw_module_attr on vfm_port_attr.gw_module_id = vfm_gw_module_attr.gw_module_id join vfm_bridge_device on vfm_gw_module_attr.vfm_bridge_guid = vfm_bridge_device.guidorder by vfm_bridge_device.guid;
CREATE VIEW `v_vfm_vadapter_attr` AS
select  id, name, `desc`, init_type, protocol, status, io_module_id,vfabric_id, wwnn, wwpn, fc_id, spma, fmpa, mac, promiscuous, silent_listener, vlan from vfm_vadapter_attr
left outer join vfm_vadapter_fc_attr on vfm_vadapter_fc_attr.vadapter_id = vfm_vadapter_attr.id
left outer join vfm_vadapter_en_attr on vfm_vadapter_en_attr.vadapter_id = vfm_vadapter_attr.id;
CREATE VIEW `v_vfm_vfabric_attr` AS
select  id, name, `desc`, ctx_table_id, primary_gw_id, backup_gw_id, protocol,type,auto_failover,auto_failback from vfm_vfabric_attr;
COMMIT;
