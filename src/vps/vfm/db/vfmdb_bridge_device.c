
/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <vfmapi_common.h>


/*
 * This routine is called ONCE for each record from the result set - Bridge
 */
int
process_bridge(void *data, int num_cols, char **values, char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        vfm_bd_attr_t *bridge;
        uint32_t i;
       
        vps_trace(VPS_ENTRYEXIT, "Entering process_bridge. Count: %d",
                        rsc->count);

        /* Read the existing resrouce count for re-allocation */
        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(vfm_bd_attr_t) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems .Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the bridge array offset */
        bridge = rsc->data + (sizeof(vfm_bd_attr_t) * rsc->count);
        memset(bridge, 0 , sizeof(vfm_bd_attr_t));
        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "guid") == 0)
                        memcpy(&bridge->_bd_guid , values[i], sizeof(uint64_t));
                else if (strcmp(cols[i], "bxm_guid") == 0)
                        memcpy(&bridge->_vfm_guid, values[i], sizeof(uint64_t));
                else if (strcmp(cols[i], "desc") == 0) {
                        memcpy(bridge->desc, values[i], 64);
                }
                else if (strcmp(cols[i], "firmware_version") == 0) {
                        memcpy(bridge->_firmware_version, values[i], 64);
                }
        }

        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;
        vps_trace(VPS_INFO, "Bridge successfully read");
        vps_trace(VPS_ENTRYEXIT, "Leaving process_bridge");
        return 0; /* Callback must return 0 on success */
}

