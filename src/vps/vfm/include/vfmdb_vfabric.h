#ifndef INCLUDED_vfmdb_vfabric
#define INCLUDED_vfmdb_vfabric


int
process_vfabric(void *data, int num_cols, uint8_t **values, char **cols);


vps_error
populate_vfabric_information(vfm_vfabric_attr_t *attr,
                vfm_vfabric_attr_bitmask_t *bitmask, res_packet *rsc);


#endif /* INCLUDED_vfmdb_vfabric */
