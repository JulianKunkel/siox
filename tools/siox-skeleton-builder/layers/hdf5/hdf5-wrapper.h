#include "hdf5-helper.h"

//@component "HDF5" "Generic"

//@createInitializerForLibrary

//implVersion "" int major, minor, relnum; char implVersion[151]; int v_ret = H5get_libversion(& major, & minor, & relnum); if (v_ret >= 0) snprintf(implVersion, 150, "%d.%d", major, minor); else sprintf(implVersion, "0");

//@register_attribute fileName "HDF5" "descriptor/filename" SIOX_STORAGE_STRING
//@register_attribute fileHandle "HDF5" "descriptor/hid" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute fileOpenFlags "HDF5" "hints/openFlags" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute fileMountChild "HDF5" "descriptor/hidChild" SIOX_STORAGE_32_BIT_UINTEGER
//@register_attribute fileMountGroup "HDF5" "descriptor/group" SIOX_STORAGE_STRING
//@register_attribute fileDataset "HDF5" "descriptor/datasetName" SIOX_STORAGE_STRING
//@register_attribute fileDatasetID "HDF5" "descriptor/datasetID" SIOX_STORAGE_32_BIT_UINTEGER

//@horizontal_map_create_int
//@horizontal_map_create_int MapName=activityHashTable_dtypeMaps


//////////////////////////////////////////////////////////////////////////////////////////

// HDF5 API prefixes:
// P: Property
// O: Object
// F: File
// I: Identifier
// T: Type
// L: Link
// G: Group
// E: Error
// D: Dataset
// A: Attribute

//@activity
//@activity_attribute_pointer fileName filename
//@activity_attribute_u32 fileHandle ret
//@splice_before uint32_t translatedFlags = translateHDFOpenFlagsToSIOX(flags);
//@activity_attribute fileOpenFlags translatedFlags
//@horizontal_map_put_int ret
//@error ''ret<0'' ret
hid_t H5Fcreate( const char * filename, unsigned flags, hid_t create_plist, hid_t access_plist );

// hid_t H5Fget_access_plist(hid_t file_id);

//@activity
//@activity_attribute_pointer fileName filename
//@activity_attribute_u32 fileHandle ret
//@splice_before uint32_t translatedFlags = translateHDFOpenFlagsToSIOX(flags);
//@activity_attribute fileOpenFlags translatedFlags
//@horizontal_map_put_int ret
//@error ''ret<0'' ret
hid_t H5Fopen( const char * filename, unsigned flags, hid_t access_plist );

//@activity
//@activity_attribute_u32 fileHandle file_id
//@activity_lookup_ID_int file_id ActivityID=ParentID
//@horizontal_map_put_int_ID ret ActivityID=ParentID
//@error ''ret<0'' ret
hid_t H5Freopen( hid_t file_id );

//@activity
//@activity_attribute_u32 fileHandle object_id
//@error ''ret<0'' ret
herr_t H5Fflush( hid_t object_id, H5F_scope_t scope );

//@activity
//@activity_attribute_u32 fileHandle file_id
//@activity_link_int file_id
//@horizontal_map_remove_int file_id
//@error ''ret<0'' ret
herr_t H5Fclose( hid_t file_id );


//@activity
//@activity_attribute_pointer fileName filename
//@error ''ret<0'' ret
htri_t H5Fis_hdf5( const char * filename );

//@activity
//@activity_attribute_u32 fileHandle loc
//@activity_attribute_pointer fileMountGroup name
//@activity_attribute_u32 fileMountChild child
//@error ''ret<0'' ret
herr_t H5Fmount( hid_t loc, const char * name, hid_t child, hid_t plist );

//@activity
//@activity_attribute_u32 fileHandle loc
//@activity_attribute_pointer fileMountGroup name
//@error ''ret<0'' ret
herr_t H5Funmount( hid_t loc, const char * name );

//@activity
//@activity_attribute_u32 fileHandle file_id
//@error ''ret<0'' ret
hssize_t H5Fget_freespace( hid_t file_id );

//@activity
//@activity_attribute_u32 fileHandle file_id
//@error ''ret<0'' ret
herr_t H5Fget_filesize( hid_t file_id, hsize_t * size );


//@activity
//@activity_attribute_u32 fileHandle loc_id
//@activity_attribute_pointer fileDataset name
//@activity_attribute_u32 fileDatasetID ret
//@activity_link_int loc_id
//@horizontal_map_put_int ret MapName=activityHashTable_dtypeMaps
//@error ''ret<0'' ret
hid_t H5Dcreate1( hid_t loc_id, const char * name, hid_t type_id, hid_t space_id, hid_t dcpl_id );

//@activity
//@activity_attribute_u32 fileHandle loc_id
//@activity_attribute_pointer fileDataset name
//@activity_attribute_u32 fileDatasetID ret
//@activity_link_int loc_id
//@horizontal_map_put_int ret MapName=activityHashTable_dtypeMaps
//@error ''ret<0'' ret
hid_t H5Dcreate2( hid_t loc_id, const char * name, hid_t dtype_id, hid_t space_id, hid_t lcpl_id, hid_t dcpl_id, hid_t dapl_id );

//@activity
//@activity_attribute_u32 fileHandle loc_id
//@activity_attribute_pointer fileDataset name
//@activity_attribute_u32 fileDatasetID ret
//@horizontal_map_put_int ret MapName=activityHashTable_dtypeMaps
//@error ''ret<0'' ret
hid_t H5Dopen1( hid_t loc_id, const char * name );

//@activity
//@activity_attribute_u32 fileHandle loc_id
//@activity_attribute_pointer fileDataset name
//@activity_attribute_u32 fileDatasetID ret
//@horizontal_map_put_int ret MapName=activityHashTable_dtypeMaps
//@error ''ret<0'' ret
hid_t H5Dopen2( hid_t loc_id, const char * name, hid_t dapl_id );

//@activity
//@activity_link_int dset_id MapName=activityHashTable_dtypeMaps
//@activity_attribute_u32 fileDatasetID dset_id
//@error ''ret<0'' ret
herr_t H5Dread( hid_t dset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t plist_id, void * buf );

//@activity
//@activity_link_int dset_id MapName=activityHashTable_dtypeMaps
//@activity_attribute_u32 fileDatasetID dset_id
//@error ''ret<0'' ret
herr_t H5Dwrite( hid_t dset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t plist_id, const void * buf );

//@activity
//@activity_link_int dset_id MapName=activityHashTable_dtypeMaps
//@activity_attribute_u32 fileDatasetID dset_id
//@error ''ret<0'' ret
herr_t H5Dset_extent( hid_t dset_id, const hsize_t size[] );

//@activity
//@activity_attribute_u32 fileDatasetID dataset_id
//@activity_link_int dataset_id MapName=activityHashTable_dtypeMaps
//@horizontal_map_remove_int dataset_id MapName=activityHashTable_dtypeMaps
//@error ''ret<0'' ret
herr_t H5Dclose( hid_t dataset_id );

// H5FDpublic.h: Instrumentation of HDF5 VFL function depends on the module, that means name differ, thus we cannot instrument it here.


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Potential optimizations
// http://www.hdfgroup.org/HDF5/doc/RM/RM_H5.html#Library-SetFreeListLimits
herr_t H5set_free_list_limits( int reg_global_lim, int reg_list_lim, int arr_global_lim, int arr_list_lim, int blk_global_lim, int blk_list_lim );

herr_t H5Fget_mdc_config( hid_t file_id, H5AC_cache_config_t * config_ptr );
herr_t H5Fset_mdc_config( hid_t file_id, H5AC_cache_config_t * config_ptr );
herr_t H5Fget_mdc_hit_rate( hid_t file_id, double * hit_rate_ptr );
herr_t H5Fget_mdc_size( hid_t file_id, size_t * max_size_ptr, size_t * min_clean_size_ptr, size_t * cur_size_ptr, int * cur_num_entries_ptr );
herr_t H5Freset_mdc_hit_rate_stats( hid_t file_id );

// Would be useful to instrument, but is not always available.

// herr_t H5DOwrite_chunk( hid_t dset_id, hid_t dxpl_id, uint32_t filter_mask, hsize_t *offset, size_t data_size, const void *buf );

// Measure overhead:
herr_t H5garbage_collect( void );

herr_t H5Fget_mpi_atomicity( hid_t file_id, hbool_t * flag );

herr_t H5Fset_mpi_atomicity( hid_t file_id, hbool_t flag );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Potentially relevant:
//herr_t H5Fget_vfd_handle(hid_t file_id, hid_t fapl, void **file_handle);

// herr_t H5Fclear_elink_file_cache( hid_t file_id );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Irrelevant functions:

// Library init / close:
//herr_t H5open(void);
//herr_t H5close(void);
//herr_t H5check_version(unsigned majnum, unsigned minnum, unsigned relnum);

hid_t H5Fget_create_plist( hid_t file_id );
herr_t H5Fget_info( hid_t obj_id, H5F_info_t * bh_info );
herr_t H5Fget_intent( hid_t file_id, unsigned * intent );
ssize_t H5Fget_name( hid_t obj_id, char * name, size_t size );



hid_t H5Iregister( H5I_type_t type, const void * object );
void * H5Iobject_verify( hid_t id, H5I_type_t id_type );
void * H5Iremove_verify( hid_t id, H5I_type_t id_type );
H5I_type_t H5Iget_type( hid_t id );
hid_t H5Iget_file_id( hid_t id );
ssize_t H5Iget_name( hid_t id, char * name , size_t size );
int H5Iinc_ref( hid_t id );
int H5Idec_ref( hid_t id );
int H5Iget_ref( hid_t id );
H5I_type_t H5Iregister_type( size_t hash_size, unsigned reserved, H5I_free_t free_func );
herr_t H5Iclear_type( H5I_type_t type, hbool_t force );
herr_t H5Idestroy_type( H5I_type_t type );
int H5Iinc_type_ref( H5I_type_t type );
int H5Idec_type_ref( H5I_type_t type );
int H5Iget_type_ref( H5I_type_t type );
void * H5Isearch( H5I_type_t type, H5I_search_func_t func, void * key );
herr_t H5Inmembers( H5I_type_t type, hsize_t * num_members );
htri_t H5Itype_exists( H5I_type_t type );
htri_t H5Iis_valid( hid_t id );
hid_t H5Tcreate( H5T_class_t type, size_t size );
hid_t H5Tcopy( hid_t type_id );
herr_t H5Tclose( hid_t type_id );
htri_t H5Tequal( hid_t type1_id, hid_t type2_id );
herr_t H5Tlock( hid_t type_id );
herr_t H5Tcommit2( hid_t loc_id, const char * name, hid_t type_id, hid_t lcpl_id, hid_t tcpl_id, hid_t tapl_id );
hid_t H5Topen2( hid_t loc_id, const char * name, hid_t tapl_id );
herr_t H5Tcommit_anon( hid_t loc_id, hid_t type_id, hid_t tcpl_id, hid_t tapl_id );
hid_t H5Tget_create_plist( hid_t type_id );
htri_t H5Tcommitted( hid_t type_id );
herr_t H5Tencode( hid_t obj_id, void * buf, size_t * nalloc );
hid_t H5Tdecode( const void * buf );
herr_t H5Tinsert( hid_t parent_id, const char * name, size_t offset, hid_t member_id );
herr_t H5Tpack( hid_t type_id );
hid_t H5Tenum_create( hid_t base_id );
herr_t H5Tenum_insert( hid_t type, const char * name, const void * value );
herr_t H5Tenum_nameof( hid_t type, const void * value, char * name , size_t size );
herr_t H5Tenum_valueof( hid_t type, const char * name, void * value );
hid_t H5Tvlen_create( hid_t base_id );
hid_t H5Tarray_create2( hid_t base_id, unsigned ndims, const hsize_t dim[ ] );
int H5Tget_array_ndims( hid_t type_id );
int H5Tget_array_dims2( hid_t type_id, hsize_t dims[] );
herr_t H5Tset_tag( hid_t type, const char * tag );
char * H5Tget_tag( hid_t type );
hid_t H5Tget_super( hid_t type );
H5T_class_t H5Tget_class( hid_t type_id );
htri_t H5Tdetect_class( hid_t type_id, H5T_class_t cls );
size_t H5Tget_size( hid_t type_id );
H5T_order_t H5Tget_order( hid_t type_id );
size_t H5Tget_precision( hid_t type_id );
int H5Tget_offset( hid_t type_id );
herr_t H5Tget_pad( hid_t type_id, H5T_pad_t * lsb , H5T_pad_t * msb );
H5T_sign_t H5Tget_sign( hid_t type_id );
herr_t H5Tget_fields( hid_t type_id, size_t * spos , size_t * epos , size_t * esize , size_t * mpos , size_t * msize );
size_t H5Tget_ebias( hid_t type_id );
H5T_norm_t H5Tget_norm( hid_t type_id );
H5T_pad_t H5Tget_inpad( hid_t type_id );
H5T_str_t H5Tget_strpad( hid_t type_id );
int H5Tget_nmembers( hid_t type_id );
char * H5Tget_member_name( hid_t type_id, unsigned membno );
int H5Tget_member_index( hid_t type_id, const char * name );
size_t H5Tget_member_offset( hid_t type_id, unsigned membno );
H5T_class_t H5Tget_member_class( hid_t type_id, unsigned membno );
hid_t H5Tget_member_type( hid_t type_id, unsigned membno );
herr_t H5Tget_member_value( hid_t type_id, unsigned membno, void * value );
H5T_cset_t H5Tget_cset( hid_t type_id );
htri_t H5Tis_variable_str( hid_t type_id );
hid_t H5Tget_native_type( hid_t type_id, H5T_direction_t direction );
herr_t H5Tset_size( hid_t type_id, size_t size );
herr_t H5Tset_order( hid_t type_id, H5T_order_t order );
herr_t H5Tset_precision( hid_t type_id, size_t prec );
herr_t H5Tset_offset( hid_t type_id, size_t offset );
herr_t H5Tset_pad( hid_t type_id, H5T_pad_t lsb, H5T_pad_t msb );
herr_t H5Tset_sign( hid_t type_id, H5T_sign_t sign );
herr_t H5Tset_fields( hid_t type_id, size_t spos, size_t epos, size_t esize, size_t mpos, size_t msize );
herr_t H5Tset_ebias( hid_t type_id, size_t ebias );
herr_t H5Tset_norm( hid_t type_id, H5T_norm_t norm );
herr_t H5Tset_inpad( hid_t type_id, H5T_pad_t pad );
herr_t H5Tset_cset( hid_t type_id, H5T_cset_t cset );
herr_t H5Tset_strpad( hid_t type_id, H5T_str_t strpad );
herr_t H5Tregister( H5T_pers_t pers, const char * name, hid_t src_id, hid_t dst_id, H5T_conv_t func );
herr_t H5Tunregister( H5T_pers_t pers, const char * name, hid_t src_id, hid_t dst_id, H5T_conv_t func );
H5T_conv_t H5Tfind( hid_t src_id, hid_t dst_id, H5T_cdata_t ** pcdata );
htri_t H5Tcompiler_conv( hid_t src_id, hid_t dst_id );
herr_t H5Tconvert( hid_t src_id, hid_t dst_id, size_t nelmts, void * buf, void * background, hid_t plist_id );
herr_t H5Tcommit1( hid_t loc_id, const char * name, hid_t type_id );
hid_t H5Topen1( hid_t loc_id, const char * name );
hid_t H5Tarray_create1( hid_t base_id, int ndims, const hsize_t dim[ ], const int perm[ ] );
int H5Tget_array_dims1( hid_t type_id, hsize_t dims[], int perm[] );
herr_t H5Lmove( hid_t src_loc, const char * src_name, hid_t dst_loc, const char * dst_name, hid_t lcpl_id, hid_t lapl_id );
herr_t H5Lcopy( hid_t src_loc, const char * src_name, hid_t dst_loc, const char * dst_name, hid_t lcpl_id, hid_t lapl_id );
herr_t H5Lcreate_hard( hid_t cur_loc, const char * cur_name, hid_t dst_loc, const char * dst_name, hid_t lcpl_id, hid_t lapl_id );
herr_t H5Lcreate_soft( const char * link_target, hid_t link_loc_id, const char * link_name, hid_t lcpl_id, hid_t lapl_id );
herr_t H5Ldelete( hid_t loc_id, const char * name, hid_t lapl_id );
herr_t H5Ldelete_by_idx( hid_t loc_id, const char * group_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, hid_t lapl_id );
herr_t H5Lget_val( hid_t loc_id, const char * name, void * buf , size_t size, hid_t lapl_id );
herr_t H5Lget_val_by_idx( hid_t loc_id, const char * group_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, void * buf , size_t size, hid_t lapl_id );
htri_t H5Lexists( hid_t loc_id, const char * name, hid_t lapl_id );
herr_t H5Lget_info( hid_t loc_id, const char * name, H5L_info_t * linfo , hid_t lapl_id );
herr_t H5Lget_info_by_idx( hid_t loc_id, const char * group_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, H5L_info_t * linfo , hid_t lapl_id );
ssize_t H5Lget_name_by_idx( hid_t loc_id, const char * group_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, char * name , size_t size, hid_t lapl_id );
herr_t H5Literate( hid_t grp_id, H5_index_t idx_type, H5_iter_order_t order, hsize_t * idx, H5L_iterate_t op, void * op_data );
herr_t H5Literate_by_name( hid_t loc_id, const char * group_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t * idx, H5L_iterate_t op, void * op_data, hid_t lapl_id );
herr_t H5Lvisit( hid_t grp_id, H5_index_t idx_type, H5_iter_order_t order, H5L_iterate_t op, void * op_data );
herr_t H5Lvisit_by_name( hid_t loc_id, const char * group_name, H5_index_t idx_type, H5_iter_order_t order, H5L_iterate_t op, void * op_data, hid_t lapl_id );
herr_t H5Lcreate_ud( hid_t link_loc_id, const char * link_name, H5L_type_t link_type, const void * udata, size_t udata_size, hid_t lcpl_id, hid_t lapl_id );
herr_t H5Lregister( const H5L_class_t * cls );
herr_t H5Lunregister( H5L_type_t id );
htri_t H5Lis_registered( H5L_type_t id );
herr_t H5Lunpack_elink_val( const void * ext_linkval , size_t link_size, unsigned * flags, const char ** filename , const char ** obj_path );
herr_t H5Lcreate_external( const char * file_name, const char * obj_name, hid_t link_loc_id, const char * link_name, hid_t lcpl_id, hid_t lapl_id );
hid_t H5Oopen( hid_t loc_id, const char * name, hid_t lapl_id );
hid_t H5Oopen_by_addr( hid_t loc_id, haddr_t addr );
hid_t H5Oopen_by_idx( hid_t loc_id, const char * group_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, hid_t lapl_id );
herr_t H5Oget_info( hid_t loc_id, H5O_info_t * oinfo );
herr_t H5Oget_info_by_name( hid_t loc_id, const char * name, H5O_info_t * oinfo, hid_t lapl_id );
herr_t H5Oget_info_by_idx( hid_t loc_id, const char * group_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, H5O_info_t * oinfo, hid_t lapl_id );
herr_t H5Olink( hid_t obj_id, hid_t new_loc_id, const char * new_name, hid_t lcpl_id, hid_t lapl_id );
herr_t H5Oincr_refcount( hid_t object_id );
herr_t H5Odecr_refcount( hid_t object_id );
herr_t H5Ocopy( hid_t src_loc_id, const char * src_name, hid_t dst_loc_id, const char * dst_name, hid_t ocpypl_id, hid_t lcpl_id );
herr_t H5Oset_comment( hid_t obj_id, const char * comment );
herr_t H5Oset_comment_by_name( hid_t loc_id, const char * name, const char * comment, hid_t lapl_id );
ssize_t H5Oget_comment( hid_t obj_id, char * comment, size_t bufsize );
ssize_t H5Oget_comment_by_name( hid_t loc_id, const char * name, char * comment, size_t bufsize, hid_t lapl_id );
herr_t H5Ovisit( hid_t obj_id, H5_index_t idx_type, H5_iter_order_t order, H5O_iterate_t op, void * op_data );
herr_t H5Ovisit_by_name( hid_t loc_id, const char * obj_name, H5_index_t idx_type, H5_iter_order_t order, H5O_iterate_t op, void * op_data, hid_t lapl_id );
herr_t H5Oclose( hid_t object_id );
hid_t H5Acreate2( hid_t loc_id, const char * attr_name, hid_t type_id, hid_t space_id, hid_t acpl_id, hid_t aapl_id );
hid_t H5Acreate_by_name( hid_t loc_id, const char * obj_name, const char * attr_name, hid_t type_id, hid_t space_id, hid_t acpl_id, hid_t aapl_id, hid_t lapl_id );
hid_t H5Aopen( hid_t obj_id, const char * attr_name, hid_t aapl_id );
hid_t H5Aopen_by_name( hid_t loc_id, const char * obj_name, const char * attr_name, hid_t aapl_id, hid_t lapl_id );
hid_t H5Aopen_by_idx( hid_t loc_id, const char * obj_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, hid_t aapl_id, hid_t lapl_id );
herr_t H5Awrite( hid_t attr_id, hid_t type_id, const void * buf );
herr_t H5Aread( hid_t attr_id, hid_t type_id, void * buf );
herr_t H5Aclose( hid_t attr_id );
hid_t H5Aget_space( hid_t attr_id );
hid_t H5Aget_type( hid_t attr_id );
hid_t H5Aget_create_plist( hid_t attr_id );
ssize_t H5Aget_name( hid_t attr_id, size_t buf_size, char * buf );
ssize_t H5Aget_name_by_idx( hid_t loc_id, const char * obj_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, char * name , size_t size, hid_t lapl_id );
hsize_t H5Aget_storage_size( hid_t attr_id );
herr_t H5Aget_info( hid_t attr_id, H5A_info_t * ainfo );
herr_t H5Aget_info_by_name( hid_t loc_id, const char * obj_name, const char * attr_name, H5A_info_t * ainfo , hid_t lapl_id );
herr_t H5Aget_info_by_idx( hid_t loc_id, const char * obj_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, H5A_info_t * ainfo , hid_t lapl_id );
herr_t H5Arename( hid_t loc_id, const char * old_name, const char * new_name );
herr_t H5Arename_by_name( hid_t loc_id, const char * obj_name, const char * old_attr_name, const char * new_attr_name, hid_t lapl_id );
herr_t H5Aiterate2( hid_t loc_id, H5_index_t idx_type, H5_iter_order_t order, hsize_t * idx, H5A_operator2_t op, void * op_data );
herr_t H5Aiterate_by_name( hid_t loc_id, const char * obj_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t * idx, H5A_operator2_t op, void * op_data, hid_t lapd_id );
herr_t H5Adelete( hid_t loc_id, const char * name );
herr_t H5Adelete_by_name( hid_t loc_id, const char * obj_name, const char * attr_name, hid_t lapl_id );
herr_t H5Adelete_by_idx( hid_t loc_id, const char * obj_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, hid_t lapl_id );
htri_t H5Aexists( hid_t obj_id, const char * attr_name );
htri_t H5Aexists_by_name( hid_t obj_id, const char * obj_name, const char * attr_name, hid_t lapl_id );
hid_t H5Acreate1( hid_t loc_id, const char * name, hid_t type_id, hid_t space_id, hid_t acpl_id );
hid_t H5Aopen_name( hid_t loc_id, const char * name );
hid_t H5Aopen_idx( hid_t loc_id, unsigned idx );
int H5Aget_num_attrs( hid_t loc_id );
herr_t H5Aiterate1( hid_t loc_id, unsigned * attr_num, H5A_operator1_t op, void * op_data );
hid_t H5Dcreate2( hid_t loc_id, const char * name, hid_t type_id, hid_t space_id, hid_t lcpl_id, hid_t dcpl_id, hid_t dapl_id );
hid_t H5Dcreate_anon( hid_t file_id, hid_t type_id, hid_t space_id, hid_t plist_id, hid_t dapl_id );
hid_t H5Dopen2( hid_t file_id, const char * name, hid_t dapl_id );
herr_t H5Dclose( hid_t dset_id );
hid_t H5Dget_space( hid_t dset_id );
herr_t H5Dget_space_status( hid_t dset_id, H5D_space_status_t * allocation );
hid_t H5Dget_type( hid_t dset_id );
hid_t H5Dget_create_plist( hid_t dset_id );
hid_t H5Dget_access_plist( hid_t dset_id );
hsize_t H5Dget_storage_size( hid_t dset_id );
haddr_t H5Dget_offset( hid_t dset_id );
herr_t H5Dset_extent( hid_t dset_id, const hsize_t size[] );
herr_t H5Ddebug( hid_t dset_id );
hid_t H5Dcreate1( hid_t file_id, const char * name, hid_t type_id, hid_t space_id, hid_t dcpl_id );
hid_t H5Dopen1( hid_t file_id, const char * name );
herr_t H5Dextend( hid_t dset_id, const hsize_t size[] );
hid_t H5Eregister_class( const char * cls_name, const char * lib_name, const char * version );
herr_t H5Eunregister_class( hid_t class_id );
herr_t H5Eclose_msg( hid_t err_id );
hid_t H5Ecreate_msg( hid_t cls, H5E_type_t msg_type, const char * msg );
hid_t H5Ecreate_stack( void );
hid_t H5Eget_current_stack( void );
herr_t H5Eclose_stack( hid_t stack_id );
ssize_t H5Eget_class_name( hid_t class_id, char * name, size_t size );
herr_t H5Eset_current_stack( hid_t err_stack_id );
herr_t H5Epush2( hid_t err_stack, const char * file, const char * func, unsigned line, hid_t cls_id, hid_t maj_id, hid_t min_id, const char * msg, ... );
herr_t H5Epop( hid_t err_stack, size_t count );
herr_t H5Eprint2( hid_t err_stack, FILE * stream );
herr_t H5Ewalk2( hid_t err_stack, H5E_direction_t direction, H5E_walk2_t func, void * client_data );
herr_t H5Eget_auto2( hid_t estack_id, H5E_auto2_t * func, void ** client_data );
herr_t H5Eset_auto2( hid_t estack_id, H5E_auto2_t func, void * client_data );
herr_t H5Eclear2( hid_t err_stack );
herr_t H5Eauto_is_v2( hid_t err_stack, unsigned * is_stack );
ssize_t H5Eget_msg( hid_t msg_id, H5E_type_t * type, char * msg, size_t size );
ssize_t H5Eget_num( hid_t error_stack_id );
herr_t H5Eclear1( void );
herr_t H5Eget_auto1( H5E_auto1_t * func, void ** client_data );
herr_t H5Epush1( const char * file, const char * func, unsigned line, H5E_major_t maj, H5E_minor_t min, const char * str );
herr_t H5Eprint1( FILE * stream );
herr_t H5Eset_auto1( H5E_auto1_t func, void * client_data );
herr_t H5Ewalk1( H5E_direction_t direction, H5E_walk1_t func, void * client_data );
char * H5Eget_major( H5E_major_t maj );
char * H5Eget_minor( H5E_minor_t min );
hid_t H5Gcreate2( hid_t loc_id, const char * name, hid_t lcpl_id, hid_t gcpl_id, hid_t gapl_id );
hid_t H5Gcreate_anon( hid_t loc_id, hid_t gcpl_id, hid_t gapl_id );
hid_t H5Gopen2( hid_t loc_id, const char * name, hid_t gapl_id );
hid_t H5Gget_create_plist( hid_t group_id );
herr_t H5Gget_info( hid_t loc_id, H5G_info_t * ginfo );
herr_t H5Gget_info_by_name( hid_t loc_id, const char * name, H5G_info_t * ginfo, hid_t lapl_id );
herr_t H5Gget_info_by_idx( hid_t loc_id, const char * group_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, H5G_info_t * ginfo, hid_t lapl_id );
herr_t H5Gclose( hid_t group_id );
hid_t H5Gcreate1( hid_t loc_id, const char * name, size_t size_hint );
hid_t H5Gopen1( hid_t loc_id, const char * name );
herr_t H5Glink( hid_t cur_loc_id, H5L_type_t type, const char * cur_name, const char * new_name );
herr_t H5Glink2( hid_t cur_loc_id, const char * cur_name, H5L_type_t type, hid_t new_loc_id, const char * new_name );
herr_t H5Gmove( hid_t src_loc_id, const char * src_name, const char * dst_name );
herr_t H5Gmove2( hid_t src_loc_id, const char * src_name, hid_t dst_loc_id, const char * dst_name );
herr_t H5Gunlink( hid_t loc_id, const char * name );
herr_t H5Gget_linkval( hid_t loc_id, const char * name, size_t size, char * buf );
herr_t H5Gset_comment( hid_t loc_id, const char * name, const char * comment );
int H5Gget_comment( hid_t loc_id, const char * name, size_t bufsize, char * buf );
herr_t H5Giterate( hid_t loc_id, const char * name, int * idx, H5G_iterate_t op, void * op_data );
herr_t H5Gget_num_objs( hid_t loc_id, hsize_t * num_objs );
herr_t H5Gget_objinfo( hid_t loc_id, const char * name, hbool_t follow_link, H5G_stat_t * statbuf );
ssize_t H5Gget_objname_by_idx( hid_t loc_id, hsize_t idx, char * name, size_t size );
H5G_obj_t H5Gget_objtype_by_idx( hid_t loc_id, hsize_t idx );
herr_t H5Zregister( const void * cls );
herr_t H5Zunregister( H5Z_filter_t id );
htri_t H5Zfilter_avail( H5Z_filter_t id );
herr_t H5Zget_filter_info( H5Z_filter_t filter, unsigned int * filter_config_flags );
hid_t H5Pcreate_class( hid_t parent, const char * name, H5P_cls_create_func_t cls_create, void * create_data, H5P_cls_copy_func_t cls_copy, void * copy_data, H5P_cls_close_func_t cls_close, void * close_data );
char * H5Pget_class_name( hid_t pclass_id );
hid_t H5Pcreate( hid_t cls_id );
herr_t H5Pregister2( hid_t cls_id, const char * name, size_t size, void * def_value, H5P_prp_create_func_t prp_create, H5P_prp_set_func_t prp_set, H5P_prp_get_func_t prp_get, H5P_prp_delete_func_t prp_del, H5P_prp_copy_func_t prp_copy, H5P_prp_compare_func_t prp_cmp, H5P_prp_close_func_t prp_close );
herr_t H5Pinsert2( hid_t plist_id, const char * name, size_t size, void * value, H5P_prp_set_func_t prp_set, H5P_prp_get_func_t prp_get, H5P_prp_delete_func_t prp_delete, H5P_prp_copy_func_t prp_copy, H5P_prp_compare_func_t prp_cmp, H5P_prp_close_func_t prp_close );
herr_t H5Pset( hid_t plist_id, const char * name, void * value );
htri_t H5Pexist( hid_t plist_id, const char * name );
herr_t H5Pget_size( hid_t id, const char * name, size_t * size );
herr_t H5Pget_nprops( hid_t id, size_t * nprops );
hid_t H5Pget_class( hid_t plist_id );
hid_t H5Pget_class_parent( hid_t pclass_id );
herr_t H5Pget( hid_t plist_id, const char * name, void * value );
htri_t H5Pequal( hid_t id1, hid_t id2 );
htri_t H5Pisa_class( hid_t plist_id, hid_t pclass_id );
int H5Piterate( hid_t id, int * idx, H5P_iterate_t iter_func, void * iter_data );
herr_t H5Pcopy_prop( hid_t dst_id, hid_t src_id, const char * name );
herr_t H5Premove( hid_t plist_id, const char * name );
herr_t H5Punregister( hid_t pclass_id, const char * name );
herr_t H5Pclose_class( hid_t plist_id );
herr_t H5Pclose( hid_t plist_id );
hid_t H5Pcopy( hid_t plist_id );
herr_t H5Pset_attr_phase_change( hid_t plist_id, unsigned max_compact, unsigned min_dense );
herr_t H5Pget_attr_phase_change( hid_t plist_id, unsigned * max_compact, unsigned * min_dense );
herr_t H5Pset_attr_creation_order( hid_t plist_id, unsigned crt_order_flags );
herr_t H5Pget_attr_creation_order( hid_t plist_id, unsigned * crt_order_flags );
herr_t H5Pset_obj_track_times( hid_t plist_id, hbool_t track_times );
herr_t H5Pget_obj_track_times( hid_t plist_id, hbool_t * track_times );
herr_t H5Pget_version( hid_t plist_id, unsigned * boot , unsigned * freelist , unsigned * stab , unsigned * shhdr );
herr_t H5Pset_userblock( hid_t plist_id, hsize_t size );
herr_t H5Pget_userblock( hid_t plist_id, hsize_t * size );
herr_t H5Pset_sizes( hid_t plist_id, size_t sizeof_addr, size_t sizeof_size );
herr_t H5Pget_sizes( hid_t plist_id, size_t * sizeof_addr , size_t * sizeof_size );
herr_t H5Pset_sym_k( hid_t plist_id, unsigned ik, unsigned lk );
herr_t H5Pget_sym_k( hid_t plist_id, unsigned * ik , unsigned * lk );
herr_t H5Pset_istore_k( hid_t plist_id, unsigned ik );
herr_t H5Pget_istore_k( hid_t plist_id, unsigned * ik );
herr_t H5Pset_shared_mesg_nindexes( hid_t plist_id, unsigned nindexes );
herr_t H5Pget_shared_mesg_nindexes( hid_t plist_id, unsigned * nindexes );
herr_t H5Pset_shared_mesg_index( hid_t plist_id, unsigned index_num, unsigned mesg_type_flags, unsigned min_mesg_size );
herr_t H5Pget_shared_mesg_index( hid_t plist_id, unsigned index_num, unsigned * mesg_type_flags, unsigned * min_mesg_size );
herr_t H5Pset_shared_mesg_phase_change( hid_t plist_id, unsigned max_list, unsigned min_btree );
herr_t H5Pget_shared_mesg_phase_change( hid_t plist_id, unsigned * max_list, unsigned * min_btree );
herr_t H5Pset_alignment( hid_t fapl_id, hsize_t threshold, hsize_t alignment );
herr_t H5Pget_alignment( hid_t fapl_id, hsize_t * threshold , hsize_t * alignment );
herr_t H5Pset_driver( hid_t plist_id, hid_t driver_id, const void * driver_info );
hid_t H5Pget_driver( hid_t plist_id );
void * H5Pget_driver_info( hid_t plist_id );
herr_t H5Pset_family_offset( hid_t fapl_id, hsize_t offset );
herr_t H5Pget_family_offset( hid_t fapl_id, hsize_t * offset );
herr_t H5Pset_multi_type( hid_t fapl_id, H5FD_mem_t type );
herr_t H5Pget_multi_type( hid_t fapl_id, H5FD_mem_t * type );
herr_t H5Pset_cache( hid_t plist_id, int mdc_nelmts, size_t rdcc_nslots, size_t rdcc_nbytes, double rdcc_w0 );
herr_t H5Pget_cache( hid_t plist_id, int * mdc_nelmts, size_t * rdcc_nslots , size_t * rdcc_nbytes , double * rdcc_w0 );
herr_t H5Pset_mdc_config( hid_t plist_id, H5AC_cache_config_t * config_ptr );
herr_t H5Pget_mdc_config( hid_t plist_id, H5AC_cache_config_t * config_ptr );
herr_t H5Pset_gc_references( hid_t fapl_id, unsigned gc_ref );
herr_t H5Pget_gc_references( hid_t fapl_id, unsigned * gc_ref );
herr_t H5Pset_fclose_degree( hid_t fapl_id, H5F_close_degree_t degree );
herr_t H5Pget_fclose_degree( hid_t fapl_id, H5F_close_degree_t * degree );
herr_t H5Pset_meta_block_size( hid_t fapl_id, hsize_t size );
herr_t H5Pget_meta_block_size( hid_t fapl_id, hsize_t * size );
herr_t H5Pset_sieve_buf_size( hid_t fapl_id, size_t size );
herr_t H5Pget_sieve_buf_size( hid_t fapl_id, size_t * size );
herr_t H5Pset_small_data_block_size( hid_t fapl_id, hsize_t size );
herr_t H5Pget_small_data_block_size( hid_t fapl_id, hsize_t * size );
herr_t H5Pset_libver_bounds( hid_t plist_id, H5F_libver_t low, H5F_libver_t high );
herr_t H5Pget_libver_bounds( hid_t plist_id, H5F_libver_t * low, H5F_libver_t * high );
herr_t H5Pset_layout( hid_t plist_id, H5D_layout_t layout );
H5D_layout_t H5Pget_layout( hid_t plist_id );
herr_t H5Pset_chunk( hid_t plist_id, int ndims, const hsize_t dim[ ] );
int H5Pget_chunk( hid_t plist_id, int max_ndims, hsize_t dim[] );
herr_t H5Pset_external( hid_t plist_id, const char * name, off_t offset, hsize_t size );
int H5Pget_external_count( hid_t plist_id );
herr_t H5Pget_external( hid_t plist_id, unsigned idx, size_t name_size, char * name , off_t * offset , hsize_t * size );
herr_t H5Pmodify_filter( hid_t plist_id, H5Z_filter_t filter, unsigned int flags, size_t cd_nelmts, const unsigned int cd_values[ ] );
herr_t H5Pset_filter( hid_t plist_id, H5Z_filter_t filter, unsigned int flags, size_t cd_nelmts, const unsigned int c_values[] );
int H5Pget_nfilters( hid_t plist_id );
H5Z_filter_t H5Pget_filter2( hid_t plist_id, unsigned filter, unsigned int * flags , size_t * cd_nelmts , unsigned cd_values[] , size_t namelen, char name[], unsigned * filter_config );
herr_t H5Pget_filter_by_id2( hid_t plist_id, H5Z_filter_t id, unsigned int * flags , size_t * cd_nelmts , unsigned cd_values[] , size_t namelen, char name[] , unsigned * filter_config );
htri_t H5Pall_filters_avail( hid_t plist_id );
herr_t H5Premove_filter( hid_t plist_id, H5Z_filter_t filter );
herr_t H5Pset_deflate( hid_t plist_id, unsigned aggression );
herr_t H5Pset_szip( hid_t plist_id, unsigned options_mask, unsigned pixels_per_block );
herr_t H5Pset_shuffle( hid_t plist_id );
herr_t H5Pset_nbit( hid_t plist_id );
herr_t H5Pset_scaleoffset( hid_t plist_id, H5Z_SO_scale_type_t scale_type, int scale_factor );
herr_t H5Pset_fletcher32( hid_t plist_id );
herr_t H5Pset_fill_value( hid_t plist_id, hid_t type_id, const void * value );
herr_t H5Pget_fill_value( hid_t plist_id, hid_t type_id, void * value );
herr_t H5Pfill_value_defined( hid_t plist, H5D_fill_value_t * status );
herr_t H5Pset_alloc_time( hid_t plist_id, H5D_alloc_time_t alloc_time );
herr_t H5Pget_alloc_time( hid_t plist_id, H5D_alloc_time_t * alloc_time );
herr_t H5Pset_fill_time( hid_t plist_id, H5D_fill_time_t fill_time );
herr_t H5Pget_fill_time( hid_t plist_id, H5D_fill_time_t * fill_time );
herr_t H5Pset_chunk_cache( hid_t dapl_id, size_t rdcc_nslots, size_t rdcc_nbytes, double rdcc_w0 );
herr_t H5Pget_chunk_cache( hid_t dapl_id, size_t * rdcc_nslots , size_t * rdcc_nbytes , double * rdcc_w0 );
herr_t H5Pset_data_transform( hid_t plist_id, const char * expression );
ssize_t H5Pget_data_transform( hid_t plist_id, char * expression , size_t size );
herr_t H5Pset_buffer( hid_t plist_id, size_t size, void * tconv, void * bkg );
size_t H5Pget_buffer( hid_t plist_id, void ** tconv , void ** bkg );
herr_t H5Pset_preserve( hid_t plist_id, hbool_t status );
int H5Pget_preserve( hid_t plist_id );
herr_t H5Pset_edc_check( hid_t plist_id, H5Z_EDC_t check );
H5Z_EDC_t H5Pget_edc_check( hid_t plist_id );
herr_t H5Pset_filter_callback( hid_t plist_id, H5Z_filter_func_t func, void * op_data );
herr_t H5Pset_btree_ratios( hid_t plist_id, double left, double middle, double right );
herr_t H5Pget_btree_ratios( hid_t plist_id, double * left , double * middle , double * right );
herr_t H5Pset_vlen_mem_manager( hid_t plist_id, H5MM_allocate_t alloc_func, void * alloc_info, H5MM_free_t free_func, void * free_info );
herr_t H5Pget_vlen_mem_manager( hid_t plist_id, H5MM_allocate_t * alloc_func, void ** alloc_info, H5MM_free_t * free_func, void ** free_info );
herr_t H5Pset_hyper_vector_size( hid_t fapl_id, size_t size );
herr_t H5Pget_hyper_vector_size( hid_t fapl_id, size_t * size );
herr_t H5Pset_type_conv_cb( hid_t dxpl_id, H5T_conv_except_func_t op, void * operate_data );
herr_t H5Pget_type_conv_cb( hid_t dxpl_id, H5T_conv_except_func_t * op, void ** operate_data );
herr_t H5Pset_create_intermediate_group( hid_t plist_id, unsigned crt_intmd );
herr_t H5Pget_create_intermediate_group( hid_t plist_id, unsigned * crt_intmd );
herr_t H5Pset_local_heap_size_hint( hid_t plist_id, size_t size_hint );
herr_t H5Pget_local_heap_size_hint( hid_t plist_id, size_t * size_hint );
herr_t H5Pset_link_phase_change( hid_t plist_id, unsigned max_compact, unsigned min_dense );
herr_t H5Pget_link_phase_change( hid_t plist_id, unsigned * max_compact , unsigned * min_dense );
herr_t H5Pset_est_link_info( hid_t plist_id, unsigned est_num_entries, unsigned est_name_len );
herr_t H5Pget_est_link_info( hid_t plist_id, unsigned * est_num_entries , unsigned * est_name_len );
herr_t H5Pset_link_creation_order( hid_t plist_id, unsigned crt_order_flags );
herr_t H5Pget_link_creation_order( hid_t plist_id, unsigned * crt_order_flags );
herr_t H5Pset_char_encoding( hid_t plist_id, H5T_cset_t encoding );
herr_t H5Pget_char_encoding( hid_t plist_id, H5T_cset_t * encoding );
herr_t H5Pset_nlinks( hid_t plist_id, size_t nlinks );
herr_t H5Pget_nlinks( hid_t plist_id, size_t * nlinks );
herr_t H5Pset_elink_prefix( hid_t plist_id, const char * prefix );
ssize_t H5Pget_elink_prefix( hid_t plist_id, char * prefix, size_t size );
hid_t H5Pget_elink_fapl( hid_t lapl_id );
herr_t H5Pset_elink_fapl( hid_t lapl_id, hid_t fapl_id );
herr_t H5Pset_elink_acc_flags( hid_t lapl_id, unsigned flags );
herr_t H5Pget_elink_acc_flags( hid_t lapl_id, unsigned * flags );
herr_t H5Pset_elink_cb( hid_t lapl_id, H5L_elink_traverse_t func, void * op_data );
herr_t H5Pget_elink_cb( hid_t lapl_id, H5L_elink_traverse_t * func, void ** op_data );
herr_t H5Pset_copy_object( hid_t plist_id, unsigned crt_intmd );
herr_t H5Pget_copy_object( hid_t plist_id, unsigned * crt_intmd );
herr_t H5Pregister1( hid_t cls_id, const char * name, size_t size, void * def_value, H5P_prp_create_func_t prp_create, H5P_prp_set_func_t prp_set, H5P_prp_get_func_t prp_get, H5P_prp_delete_func_t prp_del, H5P_prp_copy_func_t prp_copy, H5P_prp_close_func_t prp_close );
herr_t H5Pinsert1( hid_t plist_id, const char * name, size_t size, void * value, H5P_prp_set_func_t prp_set, H5P_prp_get_func_t prp_get, H5P_prp_delete_func_t prp_delete, H5P_prp_copy_func_t prp_copy, H5P_prp_close_func_t prp_close );
H5Z_filter_t H5Pget_filter1( hid_t plist_id, unsigned filter, unsigned int * flags , size_t * cd_nelmts , unsigned cd_values[] , size_t namelen, char name[] );
herr_t H5Pget_filter_by_id1( hid_t plist_id, H5Z_filter_t id, unsigned int * flags , size_t * cd_nelmts , unsigned cd_values[] , size_t namelen, char name[] );
herr_t H5Rcreate( void * ref, hid_t loc_id, const char * name, H5R_type_t ref_type, hid_t space_id );
hid_t H5Rdereference( hid_t dataset, H5R_type_t ref_type, const void * ref );
hid_t H5Rget_region( hid_t dataset, H5R_type_t ref_type, const void * ref );
herr_t H5Rget_obj_type2( hid_t id, H5R_type_t ref_type, const void * _ref, H5O_type_t * obj_type );
ssize_t H5Rget_name( hid_t loc_id, H5R_type_t ref_type, const void * ref, char * name , size_t size );
H5G_obj_t H5Rget_obj_type1( hid_t id, H5R_type_t ref_type, const void * _ref );
hid_t H5Screate( H5S_class_t type );
hid_t H5Screate_simple( int rank, const hsize_t dims[], const hsize_t maxdims[] );
herr_t H5Sset_extent_simple( hid_t space_id, int rank, const hsize_t dims[], const hsize_t max[] );
hid_t H5Scopy( hid_t space_id );
herr_t H5Sclose( hid_t space_id );
herr_t H5Sencode( hid_t obj_id, void * buf, size_t * nalloc );
hid_t H5Sdecode( const void * buf );
hssize_t H5Sget_simple_extent_npoints( hid_t space_id );
int H5Sget_simple_extent_ndims( hid_t space_id );
int H5Sget_simple_extent_dims( hid_t space_id, hsize_t dims[], hsize_t maxdims[] );
htri_t H5Sis_simple( hid_t space_id );
hssize_t H5Sget_select_npoints( hid_t spaceid );
herr_t H5Sselect_hyperslab( hid_t space_id, H5S_seloper_t op, const hsize_t start[], const hsize_t _stride[], const hsize_t count[], const hsize_t _block[] );
herr_t H5Sselect_elements( hid_t space_id, H5S_seloper_t op, size_t num_elem, const hsize_t * coord );
H5S_class_t H5Sget_simple_extent_type( hid_t space_id );
herr_t H5Sset_extent_none( hid_t space_id );
herr_t H5Sextent_copy( hid_t dst_id, hid_t src_id );
htri_t H5Sextent_equal( hid_t sid1, hid_t sid2 );
herr_t H5Sselect_all( hid_t spaceid );
herr_t H5Sselect_none( hid_t spaceid );
herr_t H5Soffset_simple( hid_t space_id, const hssize_t * offset );
htri_t H5Sselect_valid( hid_t spaceid );
hssize_t H5Sget_select_hyper_nblocks( hid_t spaceid );
hssize_t H5Sget_select_elem_npoints( hid_t spaceid );
herr_t H5Sget_select_hyper_blocklist( hid_t spaceid, hsize_t startblock, hsize_t numblocks, hsize_t buf[ ] );
herr_t H5Sget_select_elem_pointlist( hid_t spaceid, hsize_t startpoint, hsize_t numpoints, hsize_t buf[ ] );
herr_t H5Sget_select_bounds( hid_t spaceid, hsize_t start[], hsize_t end[] );
H5S_sel_type H5Sget_select_type( hid_t spaceid );
hid_t H5FD_core_init( void );
void H5FD_core_term( void );
herr_t H5Pset_fapl_core( hid_t fapl_id, size_t increment, hbool_t backing_store );
herr_t H5Pget_fapl_core( hid_t fapl_id, size_t * increment , hbool_t * backing_store );
hid_t H5FD_family_init( void );
void H5FD_family_term( void );
herr_t H5Pset_fapl_family( hid_t fapl_id, hsize_t memb_size, hid_t memb_fapl_id );
herr_t H5Pget_fapl_family( hid_t fapl_id, hsize_t * memb_size , hid_t * memb_fapl_id );
hid_t H5FD_log_init( void );
void H5FD_log_term( void );
herr_t H5Pset_fapl_log( hid_t fapl_id, const char * logfile, unsigned flags, size_t buf_size );
hid_t H5FD_multi_init( void );
void H5FD_multi_term( void );
herr_t H5Pset_fapl_multi( hid_t fapl_id, const H5FD_mem_t * memb_map, const hid_t * memb_fapl, const char * const * memb_name, const haddr_t * memb_addr, hbool_t relax );
herr_t H5Pget_fapl_multi( hid_t fapl_id, H5FD_mem_t * memb_map , hid_t * memb_fapl , char ** memb_name , haddr_t * memb_addr , hbool_t * relax );
herr_t H5Pset_dxpl_multi( hid_t dxpl_id, const hid_t * memb_dxpl );
herr_t H5Pget_dxpl_multi( hid_t dxpl_id, hid_t * memb_dxpl );
herr_t H5Pset_fapl_split( hid_t fapl, const char * meta_ext, hid_t meta_plist_id, const char * raw_ext, hid_t raw_plist_id );
hid_t H5FD_sec2_init( void );
void H5FD_sec2_term( void );
herr_t H5Pset_fapl_sec2( hid_t fapl_id );
hid_t H5FD_stdio_init( void );
void H5FD_stdio_term( void );
herr_t H5Pset_fapl_stdio( hid_t fapl_id );
