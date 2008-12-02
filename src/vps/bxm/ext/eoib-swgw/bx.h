
/* Copyright (c) 2008  VirtualPlane Systems, Inc. */

#include <infiniband/verbs.h>
#include <infiniband/umad.h>


/* The information required  to send multicast messages.
 */

struct mcast_data_t {
	union ibv_gid	port_gid;       /* the port's gid */
        uint16_t	pkey;           /* the port's PKEY in index DEF_PKEY_IDX */
	uint16_t	mlid;           /* the multicast group's  mlid */
        int             mcast_state;    /* indicates whether join and attach were done */
};

/* The global structure book-keeping all the information about
 * infiniband environment/context.
 */

struct bx_context {
        struct ibv_context *context;
        struct ibv_pd      *pd;
        struct ibv_mr      *send_mr;
	struct ibv_mr      *recv_mr;
        struct ibv_cq      *cq;
        struct ibv_qp      *qp;
	struct ibv_qp	   *mqp;
        int                 mr_size;
        int                 max_recv;
        char               *send_buf;
	char               *recv_buf;
	struct ibv_port_attr port_attr;      /* IB port attributes */
        struct ibv_ah      *ah;
	struct ibv_device  *ib_dev;
	struct mcast_data_t mcast_data;
	int		   port; 
};


/* This structure contains the crucial information for the two communicating parties 
 * to maintain about each other.
 */
struct bx_dest {
        int lid;
        int qpn;
        int psn;
};

/* The information to be filled by bx_init() */
struct init_info_packet {
        uint16_t  slid;   /* Source local id */
        uint8_t   sgid[16];    /*  Source global id */
        uint32_t sqp;   /* Source Qpair */
        uint16_t pkey;
};


/* Infiniband Header Formats */

/* Local Routing Header */
struct lrh {
	uint16_t vl_lv_sl;		/* Miscellaneous fields: VL, L-Ver, SL, LNH */
	uint16_t dlid;			/* Destination LID */
	uint16_t length;		/* Packet length. Lower 11 bits */
	uint16_t slid;			/* Source LID */
};

/* Global Routing Header */
struct grh {
	/* Miscellaneous fields: IP version, Traffic class, Flow label */
	uint32_t misc_1;
	uint16_t payload_len;		
	uint8_t  next_header;
	uint8_t  hop_limit;
        union ibv_gid   src_gid;
        union ibv_gid   dest_gid;
};

/* Base Transport Header */
struct bth {
	uint16_t misc_1;	/* Misc. fields: Opcode, SE, M, PC, THV */
	uint16_t pkey;		/* Partition key */
	uint32_t dqp;		/* Destination Queue Pair. LSB 24-bits */
	uint32_t misc_2;	/* Bit 31:Ack. Request, LSB 24 bits PSN */
};

/* Datagram Extended Transport Header */
struct deth {
	uint32_t qkey;		/* Queue Key. For EoIB: 0x8002 0000 */
	uint32_t sqp;		/* Source Queue Pair. LSB 24 bits */
};

/* Infiniband header formats */
struct ib_header {
        struct lrh lrh;         /* Local Routing Header */
        struct grh grh;         /* Optional Global Routing Header */
        struct bth bth;         /* Base Transport Header */
        struct deth deth;       /* Datagram Extended Transport Header */
        uint32_t version_reserved;      /* Version and Reserved field */
};

/* according to Table 195 in the IB spec 1.2.1 */
struct sa_mad_packet_t {
        u_int8_t                mad_header_buf[24];
        u_int8_t                rmpp_header_buf[12];
        u_int64_t               SM_Key;
        u_int16_t               AttributeOffset;
        u_int16_t               Reserved1;
        u_int64_t               ComponentMask;
        u_int8_t                SubnetAdminData[200];
}__attribute__((packed));


/* according to Table 187 in the IB spec 1.2.1 */
enum subn_adm_method_t {
        SUBN_ADM_METHOD_SET    = 0x2,
        SUBN_ADM_METHOD_DELETE = 0x15
};


/* Set this flag to get debug messages */
#define ENIB_DBG_FLAG

#ifdef ENIB_DBG_FLAG
#define ENIB_DBG(x) x;
#else
#define ENIB_DBG(x) do {} while(0);
#endif

/* Various Debug logging levels. Comment out whichever is not applicable */
#define ENIB_DEBUG_0 // Debug  Messages
#define ENIB_DEBUG_1 // Info Messages
#define ENIB_DEBUG_2 // Error Messages

#ifdef ENIB_DEBUG_0
#define enib_debug(string, arg...) \
        { printf("BX_API:%s(): Line %d:" \
          string "\n" , __FUNCTION__ , __LINE__ , ##arg); }
#else
#define enib_debug(string, arg...) do{ }while(0);
#endif

#ifdef ENIB_DEBUG_1
#define enib_printf(string, arg...) \
                 { printf("BX_API:" string "\n", ##arg); }
#else
#define enib_printf(string, arg...) do{ }while(0);
#endif


#ifdef ENIB_DEBUG_2
#define enib_error(string, arg...) \
        { printf("BX_API: ERROR :%s(): Line %d:" \
          string "\n" , __FUNCTION__ , __LINE__ , ##arg); }
#else
#define enib_error(string, arg...) do{ }while(0);
#endif

/* Important values of a few fields */
#define MELLANOX_QKEY       0x80020002
#define MULTICAST_QPN       0xFFFFFF
#define MAD_SIZE            256        /* the size of a MAD is 256 bytes */
#define DEF_PKEY_IDX        0
#define DEF_TRANS_ID        0x12345678 /* TransactionID */
#define MANAGMENT_CLASS_SUBN_ADM       0x03
#define GRH_SIZE	   40

/* generate a bit mask S bits width */
#define MASK32(S)  ( ((u_int32_t) ~0L) >> (32-(S)) )

/* generate a bit mask with bits O+S..O set (assumes 32 bit integer).
numbering bits as following:    31.........76543210 */
#define BITS32(O,S) ( MASK32(S) << (O) )

/* extract S bits from (u_int32_t)W with offset O and shifts them O places 
to the right (right justifies the field extracted).*/
#define EXTRACT32(W,O,S) ( ((W)>>(O)) & MASK32(S) )

/* insert S bits with offset O from field F into word W (u_int32_t) */
#define INSERT32(W,F,O,S) (/*(W)=*/ ( ((W) & (~BITS32(O,S)) ) | (((F) & MASK32(S))<<(O)) ))

#ifndef INSERTF
#  define INSERTF(W,O1,F,O2,S) (INSERT32(W, EXTRACT32(F, O2, S), O1, S) )
#endif

