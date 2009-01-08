#ifndef INCLUDED_vfmdb_vadapter
#define INCLUDED_vfmdb_vadapter


/* callback for editing a EN vadapter */
int
process_vadapter_en(void *data, int num_cols, uint8_t **values, char **cols);

int
process_vadapter(void *data, int num_cols, uint8_t **values, char **cols);

#endif /* INCLUDED_vfmdb_vadapter */
