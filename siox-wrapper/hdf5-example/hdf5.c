#include  <stdio.h>
#include  <stdlib.h>
#include <hdf5.h>


herr_t __real_H5open(void  );
herr_t __real_H5close(void  );
herr_t __real_H5dont_atexit(void  );
herr_t __real_H5garbage_collect(void  );
herr_t __real_H5set_free_list_limits(int   reg_global_lim, int   reg_list_lim, int   arr_global_lim, int   arr_list_lim, int   blk_global_lim, int   blk_list_lim);
herr_t __real_H5get_libversion(unsigned *  majnum, unsigned *  minnum, unsigned *  relnum);
herr_t __real_H5check_version(unsigned   majnum, unsigned   minnum, unsigned   relnum);
hid_t __real_H5Iregister(H5I_type_t   type, const void *  object);
void * __real_H5Iobject_verify(hid_t   id, H5I_type_t   id_type);
void * __real_H5Iremove_verify(hid_t   id, H5I_type_t   id_type);
H5I_type_t __real_H5Iget_type(hid_t   id);
hid_t __real_H5Iget_file_id(hid_t   id);
ssize_t __real_H5Iget_name(hid_t   id, char *  name, size_t   size);
int __real_H5Iinc_ref(hid_t   id);
int __real_H5Idec_ref(hid_t   id);
int __real_H5Iget_ref(hid_t   id);
H5I_type_t __real_H5Iregister_type(size_t   hash_size, unsigned   reserved, H5I_free_t   free_func);
herr_t __real_H5Iclear_type(H5I_type_t   type, hbool_t   force);
herr_t __real_H5Idestroy_type(H5I_type_t   type);
int __real_H5Iinc_type_ref(H5I_type_t   type);
int __real_H5Idec_type_ref(H5I_type_t   type);
int __real_H5Iget_type_ref(H5I_type_t   type);
void * __real_H5Isearch(H5I_type_t   type, H5I_search_func_t   func, void *  key);
herr_t __real_H5Inmembers(H5I_type_t   type, hsize_t *  num_members);
htri_t __real_H5Itype_exists(H5I_type_t   type);
htri_t __real_H5Iis_valid(hid_t   id);
hid_t __real_H5Tcreate(H5T_class_t   type, size_t   size);
hid_t __real_H5Tcopy(hid_t   type_id);
herr_t __real_H5Tclose(hid_t   type_id);
htri_t __real_H5Tequal(hid_t   type1_id, hid_t   type2_id);
herr_t __real_H5Tlock(hid_t   type_id);
herr_t __real_H5Tcommit2(hid_t   loc_id, const char *  name, hid_t   type_id, hid_t   lcpl_id, hid_t   tcpl_id, hid_t   tapl_id);
hid_t __real_H5Topen2(hid_t   loc_id, const char *  name, hid_t   tapl_id);
herr_t __real_H5Tcommit_anon(hid_t   loc_id, hid_t   type_id, hid_t   tcpl_id, hid_t   tapl_id);
hid_t __real_H5Tget_create_plist(hid_t   type_id);
htri_t __real_H5Tcommitted(hid_t   type_id);
herr_t __real_H5Tencode(hid_t   obj_id, void *  buf, size_t *  nalloc);
hid_t __real_H5Tdecode(const void *  buf);
herr_t __real_H5Tinsert(hid_t   parent_id, const char *  name, size_t   offset, hid_t   member_id);
herr_t __real_H5Tpack(hid_t   type_id);
hid_t __real_H5Tenum_create(hid_t   base_id);
herr_t __real_H5Tenum_insert(hid_t   type, const char *  name, const void *  value);
herr_t __real_H5Tenum_nameof(hid_t   type, const void *  value, char *  name, size_t   size);
herr_t __real_H5Tenum_valueof(hid_t   type, const char *  name, void *  value);
hid_t __real_H5Tvlen_create(hid_t   base_id);
hid_t __real_H5Tarray_create2(hid_t   base_id, unsigned   ndims, const hsize_t *  dim);
int __real_H5Tget_array_ndims(hid_t   type_id);
int __real_H5Tget_array_dims2(hid_t   type_id, hsize_t *  dims);
herr_t __real_H5Tset_tag(hid_t   type, const char *  tag);
char * __real_H5Tget_tag(hid_t   type);
hid_t __real_H5Tget_super(hid_t   type);
H5T_class_t __real_H5Tget_class(hid_t   type_id);
htri_t __real_H5Tdetect_class(hid_t   type_id, H5T_class_t   cls);
size_t __real_H5Tget_size(hid_t   type_id);
H5T_order_t __real_H5Tget_order(hid_t   type_id);
size_t __real_H5Tget_precision(hid_t   type_id);
int __real_H5Tget_offset(hid_t   type_id);
herr_t __real_H5Tget_pad(hid_t   type_id, H5T_pad_t *  lsb, H5T_pad_t *  msb);
H5T_sign_t __real_H5Tget_sign(hid_t   type_id);
herr_t __real_H5Tget_fields(hid_t   type_id, size_t *  spos, size_t *  epos, size_t *  esize, size_t *  mpos, size_t *  msize);
size_t __real_H5Tget_ebias(hid_t   type_id);
H5T_norm_t __real_H5Tget_norm(hid_t   type_id);
H5T_pad_t __real_H5Tget_inpad(hid_t   type_id);
H5T_str_t __real_H5Tget_strpad(hid_t   type_id);
int __real_H5Tget_nmembers(hid_t   type_id);
char * __real_H5Tget_member_name(hid_t   type_id, unsigned   membno);
int __real_H5Tget_member_index(hid_t   type_id, const char *  name);
size_t __real_H5Tget_member_offset(hid_t   type_id, unsigned   membno);
H5T_class_t __real_H5Tget_member_class(hid_t   type_id, unsigned   membno);
hid_t __real_H5Tget_member_type(hid_t   type_id, unsigned   membno);
herr_t __real_H5Tget_member_value(hid_t   type_id, unsigned   membno, void *  value);
H5T_cset_t __real_H5Tget_cset(hid_t   type_id);
htri_t __real_H5Tis_variable_str(hid_t   type_id);
hid_t __real_H5Tget_native_type(hid_t   type_id, H5T_direction_t   direction);
herr_t __real_H5Tset_size(hid_t   type_id, size_t   size);
herr_t __real_H5Tset_order(hid_t   type_id, H5T_order_t   order);
herr_t __real_H5Tset_precision(hid_t   type_id, size_t   prec);
herr_t __real_H5Tset_offset(hid_t   type_id, size_t   offset);
herr_t __real_H5Tset_pad(hid_t   type_id, H5T_pad_t   lsb, H5T_pad_t   msb);
herr_t __real_H5Tset_sign(hid_t   type_id, H5T_sign_t   sign);
herr_t __real_H5Tset_fields(hid_t   type_id, size_t   spos, size_t   epos, size_t   esize, size_t   mpos, size_t   msize);
herr_t __real_H5Tset_ebias(hid_t   type_id, size_t   ebias);
herr_t __real_H5Tset_norm(hid_t   type_id, H5T_norm_t   norm);
herr_t __real_H5Tset_inpad(hid_t   type_id, H5T_pad_t   pad);
herr_t __real_H5Tset_cset(hid_t   type_id, H5T_cset_t   cset);
herr_t __real_H5Tset_strpad(hid_t   type_id, H5T_str_t   strpad);
herr_t __real_H5Tregister(H5T_pers_t   pers, const char *  name, hid_t   src_id, hid_t   dst_id, H5T_conv_t   func);
herr_t __real_H5Tunregister(H5T_pers_t   pers, const char *  name, hid_t   src_id, hid_t   dst_id, H5T_conv_t   func);
H5T_conv_t __real_H5Tfind(hid_t   src_id, hid_t   dst_id, H5T_cdata_t **  pcdata);
htri_t __real_H5Tcompiler_conv(hid_t   src_id, hid_t   dst_id);
herr_t __real_H5Tconvert(hid_t   src_id, hid_t   dst_id, size_t   nelmts, void *  buf, void *  background, hid_t   plist_id);
herr_t __real_H5Tcommit1(hid_t   loc_id, const char *  name, hid_t   type_id);
hid_t __real_H5Topen1(hid_t   loc_id, const char *  name);
hid_t __real_H5Tarray_create1(hid_t   base_id, int   ndims, const hsize_t *  dim, const int *  perm);
int __real_H5Tget_array_dims1(hid_t   type_id, hsize_t *  dims, int *  perm);
herr_t __real_H5Lmove(hid_t   src_loc, const char *  src_name, hid_t   dst_loc, const char *  dst_name, hid_t   lcpl_id, hid_t   lapl_id);
herr_t __real_H5Lcopy(hid_t   src_loc, const char *  src_name, hid_t   dst_loc, const char *  dst_name, hid_t   lcpl_id, hid_t   lapl_id);
herr_t __real_H5Lcreate_hard(hid_t   cur_loc, const char *  cur_name, hid_t   dst_loc, const char *  dst_name, hid_t   lcpl_id, hid_t   lapl_id);
herr_t __real_H5Lcreate_soft(const char *  link_target, hid_t   link_loc_id, const char *  link_name, hid_t   lcpl_id, hid_t   lapl_id);
herr_t __real_H5Ldelete(hid_t   loc_id, const char *  name, hid_t   lapl_id);
herr_t __real_H5Ldelete_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, hid_t   lapl_id);
herr_t __real_H5Lget_val(hid_t   loc_id, const char *  name, void *  buf, size_t   size, hid_t   lapl_id);
herr_t __real_H5Lget_val_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, void *  buf, size_t   size, hid_t   lapl_id);
htri_t __real_H5Lexists(hid_t   loc_id, const char *  name, hid_t   lapl_id);
herr_t __real_H5Lget_info(hid_t   loc_id, const char *  name, H5L_info_t *  linfo, hid_t   lapl_id);
herr_t __real_H5Lget_info_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, H5L_info_t *  linfo, hid_t   lapl_id);
ssize_t __real_H5Lget_name_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, char *  name, size_t   size, hid_t   lapl_id);
herr_t __real_H5Literate(hid_t   grp_id, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t *  idx, H5L_iterate_t   op, void *  op_data);
herr_t __real_H5Literate_by_name(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t *  idx, H5L_iterate_t   op, void *  op_data, hid_t   lapl_id);
herr_t __real_H5Lvisit(hid_t   grp_id, H5_index_t   idx_type, H5_iter_order_t   order, H5L_iterate_t   op, void *  op_data);
herr_t __real_H5Lvisit_by_name(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, H5L_iterate_t   op, void *  op_data, hid_t   lapl_id);
herr_t __real_H5Lcreate_ud(hid_t   link_loc_id, const char *  link_name, H5L_type_t   link_type, const void *  udata, size_t   udata_size, hid_t   lcpl_id, hid_t   lapl_id);
herr_t __real_H5Lregister(const H5L_class_t *  cls);
herr_t __real_H5Lunregister(H5L_type_t   id);
htri_t __real_H5Lis_registered(H5L_type_t   id);
herr_t __real_H5Lunpack_elink_val(const void *  ext_linkval, size_t   link_size, unsigned *  flags, const char **  filename, const char **  obj_path);
herr_t __real_H5Lcreate_external(const char *  file_name, const char *  obj_name, hid_t   link_loc_id, const char *  link_name, hid_t   lcpl_id, hid_t   lapl_id);
hid_t __real_H5Oopen(hid_t   loc_id, const char *  name, hid_t   lapl_id);
hid_t __real_H5Oopen_by_addr(hid_t   loc_id, haddr_t   addr);
hid_t __real_H5Oopen_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, hid_t   lapl_id);
herr_t __real_H5Oget_info(hid_t   loc_id, H5O_info_t *  oinfo);
herr_t __real_H5Oget_info_by_name(hid_t   loc_id, const char *  name, H5O_info_t *  oinfo, hid_t   lapl_id);
herr_t __real_H5Oget_info_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, H5O_info_t *  oinfo, hid_t   lapl_id);
herr_t __real_H5Olink(hid_t   obj_id, hid_t   new_loc_id, const char *  new_name, hid_t   lcpl_id, hid_t   lapl_id);
herr_t __real_H5Oincr_refcount(hid_t   object_id);
herr_t __real_H5Odecr_refcount(hid_t   object_id);
herr_t __real_H5Ocopy(hid_t   src_loc_id, const char *  src_name, hid_t   dst_loc_id, const char *  dst_name, hid_t   ocpypl_id, hid_t   lcpl_id);
herr_t __real_H5Oset_comment(hid_t   obj_id, const char *  comment);
herr_t __real_H5Oset_comment_by_name(hid_t   loc_id, const char *  name, const char *  comment, hid_t   lapl_id);
ssize_t __real_H5Oget_comment(hid_t   obj_id, char *  comment, size_t   bufsize);
ssize_t __real_H5Oget_comment_by_name(hid_t   loc_id, const char *  name, char *  comment, size_t   bufsize, hid_t   lapl_id);
herr_t __real_H5Ovisit(hid_t   obj_id, H5_index_t   idx_type, H5_iter_order_t   order, H5O_iterate_t   op, void *  op_data);
herr_t __real_H5Ovisit_by_name(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, H5O_iterate_t   op, void *  op_data, hid_t   lapl_id);
herr_t __real_H5Oclose(hid_t   object_id);
hid_t __real_H5Acreate2(hid_t   loc_id, const char *  attr_name, hid_t   type_id, hid_t   space_id, hid_t   acpl_id, hid_t   aapl_id);
hid_t __real_H5Acreate_by_name(hid_t   loc_id, const char *  obj_name, const char *  attr_name, hid_t   type_id, hid_t   space_id, hid_t   acpl_id, hid_t   aapl_id, hid_t   lapl_id);
hid_t __real_H5Aopen(hid_t   obj_id, const char *  attr_name, hid_t   aapl_id);
hid_t __real_H5Aopen_by_name(hid_t   loc_id, const char *  obj_name, const char *  attr_name, hid_t   aapl_id, hid_t   lapl_id);
hid_t __real_H5Aopen_by_idx(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, hid_t   aapl_id, hid_t   lapl_id);
herr_t __real_H5Awrite(hid_t   attr_id, hid_t   type_id, const void *  buf);
herr_t __real_H5Aread(hid_t   attr_id, hid_t   type_id, void *  buf);
herr_t __real_H5Aclose(hid_t   attr_id);
hid_t __real_H5Aget_space(hid_t   attr_id);
hid_t __real_H5Aget_type(hid_t   attr_id);
hid_t __real_H5Aget_create_plist(hid_t   attr_id);
ssize_t __real_H5Aget_name(hid_t   attr_id, size_t   buf_size, char *  buf);
ssize_t __real_H5Aget_name_by_idx(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, char *  name, size_t   size, hid_t   lapl_id);
hsize_t __real_H5Aget_storage_size(hid_t   attr_id);
herr_t __real_H5Aget_info(hid_t   attr_id, H5A_info_t *  ainfo);
herr_t __real_H5Aget_info_by_name(hid_t   loc_id, const char *  obj_name, const char *  attr_name, H5A_info_t *  ainfo, hid_t   lapl_id);
herr_t __real_H5Aget_info_by_idx(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, H5A_info_t *  ainfo, hid_t   lapl_id);
herr_t __real_H5Arename(hid_t   loc_id, const char *  old_name, const char *  new_name);
herr_t __real_H5Arename_by_name(hid_t   loc_id, const char *  obj_name, const char *  old_attr_name, const char *  new_attr_name, hid_t   lapl_id);
herr_t __real_H5Aiterate2(hid_t   loc_id, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t *  idx, H5A_operator2_t   op, void *  op_data);
herr_t __real_H5Aiterate_by_name(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t *  idx, H5A_operator2_t   op, void *  op_data, hid_t   lapd_id);
herr_t __real_H5Adelete(hid_t   loc_id, const char *  name);
herr_t __real_H5Adelete_by_name(hid_t   loc_id, const char *  obj_name, const char *  attr_name, hid_t   lapl_id);
herr_t __real_H5Adelete_by_idx(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, hid_t   lapl_id);
htri_t __real_H5Aexists(hid_t   obj_id, const char *  attr_name);
htri_t __real_H5Aexists_by_name(hid_t   obj_id, const char *  obj_name, const char *  attr_name, hid_t   lapl_id);
hid_t __real_H5Acreate1(hid_t   loc_id, const char *  name, hid_t   type_id, hid_t   space_id, hid_t   acpl_id);
hid_t __real_H5Aopen_name(hid_t   loc_id, const char *  name);
hid_t __real_H5Aopen_idx(hid_t   loc_id, unsigned   idx);
int __real_H5Aget_num_attrs(hid_t   loc_id);
herr_t __real_H5Aiterate1(hid_t   loc_id, unsigned *  attr_num, H5A_operator1_t   op, void *  op_data);
hid_t __real_H5Dcreate2(hid_t   loc_id, const char *  name, hid_t   type_id, hid_t   space_id, hid_t   lcpl_id, hid_t   dcpl_id, hid_t   dapl_id);
hid_t __real_H5Dcreate_anon(hid_t   file_id, hid_t   type_id, hid_t   space_id, hid_t   plist_id, hid_t   dapl_id);
hid_t __real_H5Dopen2(hid_t   file_id, const char *  name, hid_t   dapl_id);
herr_t __real_H5Dclose(hid_t   dset_id);
hid_t __real_H5Dget_space(hid_t   dset_id);
herr_t __real_H5Dget_space_status(hid_t   dset_id, H5D_space_status_t *  allocation);
hid_t __real_H5Dget_type(hid_t   dset_id);
hid_t __real_H5Dget_create_plist(hid_t   dset_id);
hid_t __real_H5Dget_access_plist(hid_t   dset_id);
hsize_t __real_H5Dget_storage_size(hid_t   dset_id);
haddr_t __real_H5Dget_offset(hid_t   dset_id);
herr_t __real_H5Dread(hid_t   dset_id, hid_t   mem_type_id, hid_t   mem_space_id, hid_t   file_space_id, hid_t   plist_id, void *  buf);
herr_t __real_H5Dwrite(hid_t   dset_id, hid_t   mem_type_id, hid_t   mem_space_id, hid_t   file_space_id, hid_t   plist_id, const void *  buf);
herr_t __real_H5Diterate(void *  buf, hid_t   type_id, hid_t   space_id, H5D_operator_t   op, void *  operator_data);
herr_t __real_H5Dvlen_reclaim(hid_t   type_id, hid_t   space_id, hid_t   plist_id, void *  buf);
herr_t __real_H5Dvlen_get_buf_size(hid_t   dataset_id, hid_t   type_id, hid_t   space_id, hsize_t *  size);
herr_t __real_H5Dfill(const void *  fill, hid_t   fill_type, void *  buf, hid_t   buf_type, hid_t   space);
herr_t __real_H5Dset_extent(hid_t   dset_id, const hsize_t *  size);
herr_t __real_H5Ddebug(hid_t   dset_id);
hid_t __real_H5Dcreate1(hid_t   file_id, const char *  name, hid_t   type_id, hid_t   space_id, hid_t   dcpl_id);
hid_t __real_H5Dopen1(hid_t   file_id, const char *  name);
herr_t __real_H5Dextend(hid_t   dset_id, const hsize_t *  size);
hid_t __real_H5Eregister_class(const char *  cls_name, const char *  lib_name, const char *  version);
herr_t __real_H5Eunregister_class(hid_t   class_id);
herr_t __real_H5Eclose_msg(hid_t   err_id);
hid_t __real_H5Ecreate_msg(hid_t   cls, H5E_type_t   msg_type, const char *  msg);
hid_t __real_H5Ecreate_stack(void  );
hid_t __real_H5Eget_current_stack(void  );
herr_t __real_H5Eclose_stack(hid_t   stack_id);
ssize_t __real_H5Eget_class_name(hid_t   class_id, char *  name, size_t   size);
herr_t __real_H5Eset_current_stack(hid_t   err_stack_id);
herr_t __real_H5Epop(hid_t   err_stack, size_t   count);
herr_t __real_H5Eprint2(hid_t   err_stack, FILE *  stream);
herr_t __real_H5Ewalk2(hid_t   err_stack, H5E_direction_t   direction, H5E_walk2_t   func, void *  client_data);
herr_t __real_H5Eget_auto2(hid_t   estack_id, H5E_auto2_t *  func, void **  client_data);
herr_t __real_H5Eset_auto2(hid_t   estack_id, H5E_auto2_t   func, void *  client_data);
herr_t __real_H5Eclear2(hid_t   err_stack);
herr_t __real_H5Eauto_is_v2(hid_t   err_stack, unsigned *  is_stack);
ssize_t __real_H5Eget_msg(hid_t   msg_id, H5E_type_t *  type, char *  msg, size_t   size);
ssize_t __real_H5Eget_num(hid_t   error_stack_id);
herr_t __real_H5Eclear1(void  );
herr_t __real_H5Eget_auto1(H5E_auto1_t *  func, void **  client_data);
herr_t __real_H5Epush1(const char *  file, const char *  func, unsigned   line, H5E_major_t   maj, H5E_minor_t   min, const char *  str);
herr_t __real_H5Eprint1(FILE *  stream);
herr_t __real_H5Eset_auto1(H5E_auto1_t   func, void *  client_data);
herr_t __real_H5Ewalk1(H5E_direction_t   direction, H5E_walk1_t   func, void *  client_data);
char * __real_H5Eget_major(H5E_major_t   maj);
char * __real_H5Eget_minor(H5E_minor_t   min);
htri_t __real_H5Fis_hdf5(const char *  filename);
hid_t __real_H5Fcreate(const char *  filename, unsigned   flags, hid_t   create_plist, hid_t   access_plist);
hid_t __real_H5Fopen(const char *  filename, unsigned   flags, hid_t   access_plist);
hid_t __real_H5Freopen(hid_t   file_id);
herr_t __real_H5Fflush(hid_t   object_id, H5F_scope_t   scope);
herr_t __real_H5Fclose(hid_t   file_id);
hid_t __real_H5Fget_create_plist(hid_t   file_id);
hid_t __real_H5Fget_access_plist(hid_t   file_id);
herr_t __real_H5Fget_intent(hid_t   file_id, unsigned *   intent);
ssize_t __real_H5Fget_obj_count(hid_t   file_id, unsigned   types);
ssize_t __real_H5Fget_obj_ids(hid_t   file_id, unsigned   types, size_t   max_objs, hid_t *  obj_id_list);
herr_t __real_H5Fget_vfd_handle(hid_t   file_id, hid_t   fapl, void **  file_handle);
herr_t __real_H5Fmount(hid_t   loc, const char *  name, hid_t   child, hid_t   plist);
herr_t __real_H5Funmount(hid_t   loc, const char *  name);
hssize_t __real_H5Fget_freespace(hid_t   file_id);
herr_t __real_H5Fget_filesize(hid_t   file_id, hsize_t *  size);
herr_t __real_H5Fget_mdc_config(hid_t   file_id, H5AC_cache_config_t *   config_ptr);
herr_t __real_H5Fset_mdc_config(hid_t   file_id, H5AC_cache_config_t *   config_ptr);
herr_t __real_H5Fget_mdc_hit_rate(hid_t   file_id, double *   hit_rate_ptr);
herr_t __real_H5Fget_mdc_size(hid_t   file_id, size_t *   max_size_ptr, size_t *   min_clean_size_ptr, size_t *   cur_size_ptr, int *   cur_num_entries_ptr);
herr_t __real_H5Freset_mdc_hit_rate_stats(hid_t   file_id);
ssize_t __real_H5Fget_name(hid_t   obj_id, char *  name, size_t   size);
herr_t __real_H5Fget_info(hid_t   obj_id, H5F_info_t *  bh_info);
hid_t __real_H5FDregister(const H5FD_class_t *  cls);
herr_t __real_H5FDunregister(hid_t   driver_id);
H5FD_t * __real_H5FDopen(const char *  name, unsigned   flags, hid_t   fapl_id, haddr_t   maxaddr);
herr_t __real_H5FDclose(H5FD_t *  file);
int __real_H5FDcmp(const H5FD_t *  f1, const H5FD_t *  f2);
int __real_H5FDquery(const H5FD_t *  f, unsigned long *  flags);
haddr_t __real_H5FDalloc(H5FD_t *  file, H5FD_mem_t   type, hid_t   dxpl_id, hsize_t   size);
herr_t __real_H5FDfree(H5FD_t *  file, H5FD_mem_t   type, hid_t   dxpl_id, haddr_t   addr, hsize_t   size);
haddr_t __real_H5FDget_eoa(H5FD_t *  file, H5FD_mem_t   type);
herr_t __real_H5FDset_eoa(H5FD_t *  file, H5FD_mem_t   type, haddr_t   eoa);
haddr_t __real_H5FDget_eof(H5FD_t *  file);
herr_t __real_H5FDget_vfd_handle(H5FD_t *  file, hid_t   fapl, void**  file_handle);
herr_t __real_H5FDread(H5FD_t *  file, H5FD_mem_t   type, hid_t   dxpl_id, haddr_t   addr, size_t   size, void *  buf);
herr_t __real_H5FDwrite(H5FD_t *  file, H5FD_mem_t   type, hid_t   dxpl_id, haddr_t   addr, size_t   size, const void *  buf);
herr_t __real_H5FDflush(H5FD_t *  file, hid_t   dxpl_id, unsigned   closing);
herr_t __real_H5FDtruncate(H5FD_t *  file, hid_t   dxpl_id, hbool_t   closing);
hid_t __real_H5Gcreate2(hid_t   loc_id, const char *  name, hid_t   lcpl_id, hid_t   gcpl_id, hid_t   gapl_id);
hid_t __real_H5Gcreate_anon(hid_t   loc_id, hid_t   gcpl_id, hid_t   gapl_id);
hid_t __real_H5Gopen2(hid_t   loc_id, const char *  name, hid_t   gapl_id);
hid_t __real_H5Gget_create_plist(hid_t   group_id);
herr_t __real_H5Gget_info(hid_t   loc_id, H5G_info_t *  ginfo);
herr_t __real_H5Gget_info_by_name(hid_t   loc_id, const char *  name, H5G_info_t *  ginfo, hid_t   lapl_id);
herr_t __real_H5Gget_info_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, H5G_info_t *  ginfo, hid_t   lapl_id);
herr_t __real_H5Gclose(hid_t   group_id);
hid_t __real_H5Gcreate1(hid_t   loc_id, const char *  name, size_t   size_hint);
hid_t __real_H5Gopen1(hid_t   loc_id, const char *  name);
herr_t __real_H5Glink(hid_t   cur_loc_id, H5L_type_t   type, const char *  cur_name, const char *  new_name);
herr_t __real_H5Glink2(hid_t   cur_loc_id, const char *  cur_name, H5L_type_t   type, hid_t   new_loc_id, const char *  new_name);
herr_t __real_H5Gmove(hid_t   src_loc_id, const char *  src_name, const char *  dst_name);
herr_t __real_H5Gmove2(hid_t   src_loc_id, const char *  src_name, hid_t   dst_loc_id, const char *  dst_name);
herr_t __real_H5Gunlink(hid_t   loc_id, const char *  name);
herr_t __real_H5Gget_linkval(hid_t   loc_id, const char *  name, size_t   size, char *  buf);
herr_t __real_H5Gset_comment(hid_t   loc_id, const char *  name, const char *  comment);
int __real_H5Gget_comment(hid_t   loc_id, const char *  name, size_t   bufsize, char *  buf);
herr_t __real_H5Giterate(hid_t   loc_id, const char *  name, int *  idx, H5G_iterate_t   op, void *  op_data);
herr_t __real_H5Gget_num_objs(hid_t   loc_id, hsize_t *  num_objs);
herr_t __real_H5Gget_objinfo(hid_t   loc_id, const char *  name, hbool_t   follow_link, H5G_stat_t *  statbuf);
ssize_t __real_H5Gget_objname_by_idx(hid_t   loc_id, hsize_t   idx, char*   name, size_t   size);
H5G_obj_t __real_H5Gget_objtype_by_idx(hid_t   loc_id, hsize_t   idx);
herr_t __real_H5Zregister(const void *  cls);
herr_t __real_H5Zunregister(H5Z_filter_t   id);
htri_t __real_H5Zfilter_avail(H5Z_filter_t   id);
herr_t __real_H5Zget_filter_info(H5Z_filter_t   filter, unsigned int *  filter_config_flags);
hid_t __real_H5Pcreate_class(hid_t   parent, const char *  name, H5P_cls_create_func_t   cls_create, void *  create_data, H5P_cls_copy_func_t   cls_copy, void *  copy_data, H5P_cls_close_func_t   cls_close, void *  close_data);
char * __real_H5Pget_class_name(hid_t   pclass_id);
hid_t __real_H5Pcreate(hid_t   cls_id);
herr_t __real_H5Pregister2(hid_t   cls_id, const char *  name, size_t   size, void *  def_value, H5P_prp_create_func_t   prp_create, H5P_prp_set_func_t   prp_set, H5P_prp_get_func_t   prp_get, H5P_prp_delete_func_t   prp_del, H5P_prp_copy_func_t   prp_copy, H5P_prp_compare_func_t   prp_cmp, H5P_prp_close_func_t   prp_close);
herr_t __real_H5Pinsert2(hid_t   plist_id, const char *  name, size_t   size, void *  value, H5P_prp_set_func_t   prp_set, H5P_prp_get_func_t   prp_get, H5P_prp_delete_func_t   prp_delete, H5P_prp_copy_func_t   prp_copy, H5P_prp_compare_func_t   prp_cmp, H5P_prp_close_func_t   prp_close);
herr_t __real_H5Pset(hid_t   plist_id, const char *  name, void *  value);
htri_t __real_H5Pexist(hid_t   plist_id, const char *  name);
herr_t __real_H5Pget_size(hid_t   id, const char *  name, size_t *  size);
herr_t __real_H5Pget_nprops(hid_t   id, size_t *  nprops);
hid_t __real_H5Pget_class(hid_t   plist_id);
hid_t __real_H5Pget_class_parent(hid_t   pclass_id);
herr_t __real_H5Pget(hid_t   plist_id, const char *  name, void *   value);
htri_t __real_H5Pequal(hid_t   id1, hid_t   id2);
htri_t __real_H5Pisa_class(hid_t   plist_id, hid_t   pclass_id);
int __real_H5Piterate(hid_t   id, int *  idx, H5P_iterate_t   iter_func, void *  iter_data);
herr_t __real_H5Pcopy_prop(hid_t   dst_id, hid_t   src_id, const char *  name);
herr_t __real_H5Premove(hid_t   plist_id, const char *  name);
herr_t __real_H5Punregister(hid_t   pclass_id, const char *  name);
herr_t __real_H5Pclose_class(hid_t   plist_id);
herr_t __real_H5Pclose(hid_t   plist_id);
hid_t __real_H5Pcopy(hid_t   plist_id);
herr_t __real_H5Pset_attr_phase_change(hid_t   plist_id, unsigned   max_compact, unsigned   min_dense);
herr_t __real_H5Pget_attr_phase_change(hid_t   plist_id, unsigned *  max_compact, unsigned *  min_dense);
herr_t __real_H5Pset_attr_creation_order(hid_t   plist_id, unsigned   crt_order_flags);
herr_t __real_H5Pget_attr_creation_order(hid_t   plist_id, unsigned *  crt_order_flags);
herr_t __real_H5Pset_obj_track_times(hid_t   plist_id, hbool_t   track_times);
herr_t __real_H5Pget_obj_track_times(hid_t   plist_id, hbool_t *  track_times);
herr_t __real_H5Pget_version(hid_t   plist_id, unsigned *  boot, unsigned *  freelist, unsigned *  stab, unsigned *  shhdr);
herr_t __real_H5Pset_userblock(hid_t   plist_id, hsize_t   size);
herr_t __real_H5Pget_userblock(hid_t   plist_id, hsize_t *  size);
herr_t __real_H5Pset_sizes(hid_t   plist_id, size_t   sizeof_addr, size_t   sizeof_size);
herr_t __real_H5Pget_sizes(hid_t   plist_id, size_t *  sizeof_addr, size_t *  sizeof_size);
herr_t __real_H5Pset_sym_k(hid_t   plist_id, unsigned   ik, unsigned   lk);
herr_t __real_H5Pget_sym_k(hid_t   plist_id, unsigned *  ik, unsigned *  lk);
herr_t __real_H5Pset_istore_k(hid_t   plist_id, unsigned   ik);
herr_t __real_H5Pget_istore_k(hid_t   plist_id, unsigned *  ik);
herr_t __real_H5Pset_shared_mesg_nindexes(hid_t   plist_id, unsigned   nindexes);
herr_t __real_H5Pget_shared_mesg_nindexes(hid_t   plist_id, unsigned *  nindexes);
herr_t __real_H5Pset_shared_mesg_index(hid_t   plist_id, unsigned   index_num, unsigned   mesg_type_flags, unsigned   min_mesg_size);
herr_t __real_H5Pget_shared_mesg_index(hid_t   plist_id, unsigned   index_num, unsigned *  mesg_type_flags, unsigned *  min_mesg_size);
herr_t __real_H5Pset_shared_mesg_phase_change(hid_t   plist_id, unsigned   max_list, unsigned   min_btree);
herr_t __real_H5Pget_shared_mesg_phase_change(hid_t   plist_id, unsigned *  max_list, unsigned *  min_btree);
herr_t __real_H5Pset_alignment(hid_t   fapl_id, hsize_t   threshold, hsize_t   alignment);
herr_t __real_H5Pget_alignment(hid_t   fapl_id, hsize_t *  threshold, hsize_t *  alignment);
herr_t __real_H5Pset_driver(hid_t   plist_id, hid_t   driver_id, const void *  driver_info);
hid_t __real_H5Pget_driver(hid_t   plist_id);
void * __real_H5Pget_driver_info(hid_t   plist_id);
herr_t __real_H5Pset_family_offset(hid_t   fapl_id, hsize_t   offset);
herr_t __real_H5Pget_family_offset(hid_t   fapl_id, hsize_t *  offset);
herr_t __real_H5Pset_multi_type(hid_t   fapl_id, H5FD_mem_t   type);
herr_t __real_H5Pget_multi_type(hid_t   fapl_id, H5FD_mem_t *  type);
herr_t __real_H5Pset_cache(hid_t   plist_id, int   mdc_nelmts, size_t   rdcc_nslots, size_t   rdcc_nbytes, double   rdcc_w0);
herr_t __real_H5Pget_cache(hid_t   plist_id, int *  mdc_nelmts, size_t *  rdcc_nslots, size_t *  rdcc_nbytes, double *  rdcc_w0);
herr_t __real_H5Pset_mdc_config(hid_t   plist_id, H5AC_cache_config_t *   config_ptr);
herr_t __real_H5Pget_mdc_config(hid_t   plist_id, H5AC_cache_config_t *   config_ptr);
herr_t __real_H5Pset_gc_references(hid_t   fapl_id, unsigned   gc_ref);
herr_t __real_H5Pget_gc_references(hid_t   fapl_id, unsigned *  gc_ref);
herr_t __real_H5Pset_fclose_degree(hid_t   fapl_id, H5F_close_degree_t   degree);
herr_t __real_H5Pget_fclose_degree(hid_t   fapl_id, H5F_close_degree_t *  degree);
herr_t __real_H5Pset_meta_block_size(hid_t   fapl_id, hsize_t   size);
herr_t __real_H5Pget_meta_block_size(hid_t   fapl_id, hsize_t *  size);
herr_t __real_H5Pset_sieve_buf_size(hid_t   fapl_id, size_t   size);
herr_t __real_H5Pget_sieve_buf_size(hid_t   fapl_id, size_t *  size);
herr_t __real_H5Pset_small_data_block_size(hid_t   fapl_id, hsize_t   size);
herr_t __real_H5Pget_small_data_block_size(hid_t   fapl_id, hsize_t *  size);
herr_t __real_H5Pset_libver_bounds(hid_t   plist_id, H5F_libver_t   low, H5F_libver_t   high);
herr_t __real_H5Pget_libver_bounds(hid_t   plist_id, H5F_libver_t *  low, H5F_libver_t *  high);
herr_t __real_H5Pset_layout(hid_t   plist_id, H5D_layout_t   layout);
H5D_layout_t __real_H5Pget_layout(hid_t   plist_id);
herr_t __real_H5Pset_chunk(hid_t   plist_id, int   ndims, const hsize_t *  dim);
int __real_H5Pget_chunk(hid_t   plist_id, int   max_ndims, hsize_t *  dim);
herr_t __real_H5Pset_external(hid_t   plist_id, const char *  name, off_t   offset, hsize_t   size);
int __real_H5Pget_external_count(hid_t   plist_id);
herr_t __real_H5Pget_external(hid_t   plist_id, unsigned   idx, size_t   name_size, char *  name, off_t *  offset, hsize_t *  size);
herr_t __real_H5Pmodify_filter(hid_t   plist_id, H5Z_filter_t   filter, unsigned int   flags, size_t   cd_nelmts, const unsigned int *  cd_values);
herr_t __real_H5Pset_filter(hid_t   plist_id, H5Z_filter_t   filter, unsigned int   flags, size_t   cd_nelmts, const unsigned int *  c_values);
int __real_H5Pget_nfilters(hid_t   plist_id);
H5Z_filter_t __real_H5Pget_filter2(hid_t   plist_id, unsigned   filter, unsigned int *  flags, size_t *  cd_nelmts, unsigned *  cd_values, size_t   namelen, char *  name, unsigned *  filter_config);
herr_t __real_H5Pget_filter_by_id2(hid_t   plist_id, H5Z_filter_t   id, unsigned int *  flags, size_t *  cd_nelmts, unsigned *  cd_values, size_t   namelen, char *  name, unsigned *  filter_config);
htri_t __real_H5Pall_filters_avail(hid_t   plist_id);
herr_t __real_H5Premove_filter(hid_t   plist_id, H5Z_filter_t   filter);
herr_t __real_H5Pset_deflate(hid_t   plist_id, unsigned   aggression);
herr_t __real_H5Pset_szip(hid_t   plist_id, unsigned   options_mask, unsigned   pixels_per_block);
herr_t __real_H5Pset_shuffle(hid_t   plist_id);
herr_t __real_H5Pset_nbit(hid_t   plist_id);
herr_t __real_H5Pset_scaleoffset(hid_t   plist_id, H5Z_SO_scale_type_t   scale_type, int   scale_factor);
herr_t __real_H5Pset_fletcher32(hid_t   plist_id);
herr_t __real_H5Pset_fill_value(hid_t   plist_id, hid_t   type_id, const void *  value);
herr_t __real_H5Pget_fill_value(hid_t   plist_id, hid_t   type_id, void *  value);
herr_t __real_H5Pfill_value_defined(hid_t   plist, H5D_fill_value_t *  status);
herr_t __real_H5Pset_alloc_time(hid_t   plist_id, H5D_alloc_time_t   alloc_time);
herr_t __real_H5Pget_alloc_time(hid_t   plist_id, H5D_alloc_time_t *  alloc_time);
herr_t __real_H5Pset_fill_time(hid_t   plist_id, H5D_fill_time_t   fill_time);
herr_t __real_H5Pget_fill_time(hid_t   plist_id, H5D_fill_time_t *  fill_time);
herr_t __real_H5Pset_chunk_cache(hid_t   dapl_id, size_t   rdcc_nslots, size_t   rdcc_nbytes, double   rdcc_w0);
herr_t __real_H5Pget_chunk_cache(hid_t   dapl_id, size_t *  rdcc_nslots, size_t *  rdcc_nbytes, double *  rdcc_w0);
herr_t __real_H5Pset_data_transform(hid_t   plist_id, const char*   expression);
ssize_t __real_H5Pget_data_transform(hid_t   plist_id, char*   expression, size_t   size);
herr_t __real_H5Pset_buffer(hid_t   plist_id, size_t   size, void *  tconv, void *  bkg);
size_t __real_H5Pget_buffer(hid_t   plist_id, void **  tconv, void **  bkg);
herr_t __real_H5Pset_preserve(hid_t   plist_id, hbool_t   status);
int __real_H5Pget_preserve(hid_t   plist_id);
herr_t __real_H5Pset_edc_check(hid_t   plist_id, H5Z_EDC_t   check);
H5Z_EDC_t __real_H5Pget_edc_check(hid_t   plist_id);
herr_t __real_H5Pset_filter_callback(hid_t   plist_id, H5Z_filter_func_t   func, void*   op_data);
herr_t __real_H5Pset_btree_ratios(hid_t   plist_id, double   left, double   middle, double   right);
herr_t __real_H5Pget_btree_ratios(hid_t   plist_id, double *  left, double *  middle, double *  right);
herr_t __real_H5Pset_vlen_mem_manager(hid_t   plist_id, H5MM_allocate_t   alloc_func, void *  alloc_info, H5MM_free_t   free_func, void *  free_info);
herr_t __real_H5Pget_vlen_mem_manager(hid_t   plist_id, H5MM_allocate_t *  alloc_func, void **  alloc_info, H5MM_free_t *  free_func, void **  free_info);
herr_t __real_H5Pset_hyper_vector_size(hid_t   fapl_id, size_t   size);
herr_t __real_H5Pget_hyper_vector_size(hid_t   fapl_id, size_t *  size);
herr_t __real_H5Pset_type_conv_cb(hid_t   dxpl_id, H5T_conv_except_func_t   op, void*   operate_data);
herr_t __real_H5Pget_type_conv_cb(hid_t   dxpl_id, H5T_conv_except_func_t *  op, void**   operate_data);
herr_t __real_H5Pset_create_intermediate_group(hid_t   plist_id, unsigned   crt_intmd);
herr_t __real_H5Pget_create_intermediate_group(hid_t   plist_id, unsigned *  crt_intmd);
herr_t __real_H5Pset_local_heap_size_hint(hid_t   plist_id, size_t   size_hint);
herr_t __real_H5Pget_local_heap_size_hint(hid_t   plist_id, size_t *  size_hint);
herr_t __real_H5Pset_link_phase_change(hid_t   plist_id, unsigned   max_compact, unsigned   min_dense);
herr_t __real_H5Pget_link_phase_change(hid_t   plist_id, unsigned *  max_compact, unsigned *  min_dense);
herr_t __real_H5Pset_est_link_info(hid_t   plist_id, unsigned   est_num_entries, unsigned   est_name_len);
herr_t __real_H5Pget_est_link_info(hid_t   plist_id, unsigned *  est_num_entries, unsigned *  est_name_len);
herr_t __real_H5Pset_link_creation_order(hid_t   plist_id, unsigned   crt_order_flags);
herr_t __real_H5Pget_link_creation_order(hid_t   plist_id, unsigned *  crt_order_flags);
herr_t __real_H5Pset_char_encoding(hid_t   plist_id, H5T_cset_t   encoding);
herr_t __real_H5Pget_char_encoding(hid_t   plist_id, H5T_cset_t *  encoding);
herr_t __real_H5Pset_nlinks(hid_t   plist_id, size_t   nlinks);
herr_t __real_H5Pget_nlinks(hid_t   plist_id, size_t *  nlinks);
herr_t __real_H5Pset_elink_prefix(hid_t   plist_id, const char *  prefix);
ssize_t __real_H5Pget_elink_prefix(hid_t   plist_id, char *  prefix, size_t   size);
hid_t __real_H5Pget_elink_fapl(hid_t   lapl_id);
herr_t __real_H5Pset_elink_fapl(hid_t   lapl_id, hid_t   fapl_id);
herr_t __real_H5Pset_elink_acc_flags(hid_t   lapl_id, unsigned   flags);
herr_t __real_H5Pget_elink_acc_flags(hid_t   lapl_id, unsigned *  flags);
herr_t __real_H5Pset_elink_cb(hid_t   lapl_id, H5L_elink_traverse_t   func, void *  op_data);
herr_t __real_H5Pget_elink_cb(hid_t   lapl_id, H5L_elink_traverse_t *  func, void **  op_data);
herr_t __real_H5Pset_copy_object(hid_t   plist_id, unsigned   crt_intmd);
herr_t __real_H5Pget_copy_object(hid_t   plist_id, unsigned *  crt_intmd);
herr_t __real_H5Pregister1(hid_t   cls_id, const char *  name, size_t   size, void *  def_value, H5P_prp_create_func_t   prp_create, H5P_prp_set_func_t   prp_set, H5P_prp_get_func_t   prp_get, H5P_prp_delete_func_t   prp_del, H5P_prp_copy_func_t   prp_copy, H5P_prp_close_func_t   prp_close);
herr_t __real_H5Pinsert1(hid_t   plist_id, const char *  name, size_t   size, void *  value, H5P_prp_set_func_t   prp_set, H5P_prp_get_func_t   prp_get, H5P_prp_delete_func_t   prp_delete, H5P_prp_copy_func_t   prp_copy, H5P_prp_close_func_t   prp_close);
H5Z_filter_t __real_H5Pget_filter1(hid_t   plist_id, unsigned   filter, unsigned int *  flags, size_t *  cd_nelmts, unsigned *  cd_values, size_t   namelen, char *  name);
herr_t __real_H5Pget_filter_by_id1(hid_t   plist_id, H5Z_filter_t   id, unsigned int *  flags, size_t *  cd_nelmts, unsigned *  cd_values, size_t   namelen, char *  name);
herr_t __real_H5Rcreate(void *  ref, hid_t   loc_id, const char *  name, H5R_type_t   ref_type, hid_t   space_id);
hid_t __real_H5Rdereference(hid_t   dataset, H5R_type_t   ref_type, const void *  ref);
hid_t __real_H5Rget_region(hid_t   dataset, H5R_type_t   ref_type, const void *  ref);
herr_t __real_H5Rget_obj_type2(hid_t   id, H5R_type_t   ref_type, const void *  _ref, H5O_type_t *  obj_type);
ssize_t __real_H5Rget_name(hid_t   loc_id, H5R_type_t   ref_type, const void *  ref, char *  name, size_t   size);
H5G_obj_t __real_H5Rget_obj_type1(hid_t   id, H5R_type_t   ref_type, const void *  _ref);
hid_t __real_H5Screate(H5S_class_t   type);
hid_t __real_H5Screate_simple(int   rank, const hsize_t *  dims, const hsize_t *  maxdims);
herr_t __real_H5Sset_extent_simple(hid_t   space_id, int   rank, const hsize_t *  dims, const hsize_t *  max);
hid_t __real_H5Scopy(hid_t   space_id);
herr_t __real_H5Sclose(hid_t   space_id);
herr_t __real_H5Sencode(hid_t   obj_id, void *  buf, size_t *  nalloc);
hid_t __real_H5Sdecode(const void *  buf);
hssize_t __real_H5Sget_simple_extent_npoints(hid_t   space_id);
int __real_H5Sget_simple_extent_ndims(hid_t   space_id);
int __real_H5Sget_simple_extent_dims(hid_t   space_id, hsize_t *  dims, hsize_t *  maxdims);
htri_t __real_H5Sis_simple(hid_t   space_id);
hssize_t __real_H5Sget_select_npoints(hid_t   spaceid);
herr_t __real_H5Sselect_hyperslab(hid_t   space_id, H5S_seloper_t   op, const hsize_t *  start, const hsize_t *  _stride, const hsize_t *  count, const hsize_t *  _block);
herr_t __real_H5Sselect_elements(hid_t   space_id, H5S_seloper_t   op, size_t   num_elem, const hsize_t *  coord);
H5S_class_t __real_H5Sget_simple_extent_type(hid_t   space_id);
herr_t __real_H5Sset_extent_none(hid_t   space_id);
herr_t __real_H5Sextent_copy(hid_t   dst_id, hid_t   src_id);
htri_t __real_H5Sextent_equal(hid_t   sid1, hid_t   sid2);
herr_t __real_H5Sselect_all(hid_t   spaceid);
herr_t __real_H5Sselect_none(hid_t   spaceid);
herr_t __real_H5Soffset_simple(hid_t   space_id, const hssize_t *  offset);
htri_t __real_H5Sselect_valid(hid_t   spaceid);
hssize_t __real_H5Sget_select_hyper_nblocks(hid_t   spaceid);
hssize_t __real_H5Sget_select_elem_npoints(hid_t   spaceid);
herr_t __real_H5Sget_select_hyper_blocklist(hid_t   spaceid, hsize_t   startblock, hsize_t   numblocks, hsize_t *  buf);
herr_t __real_H5Sget_select_elem_pointlist(hid_t   spaceid, hsize_t   startpoint, hsize_t   numpoints, hsize_t *  buf);
herr_t __real_H5Sget_select_bounds(hid_t   spaceid, hsize_t *  start, hsize_t *  end);
H5S_sel_type __real_H5Sget_select_type(hid_t   spaceid);
hid_t __real_H5FD_core_init(void  );
void __real_H5FD_core_term(void  );
herr_t __real_H5Pset_fapl_core(hid_t   fapl_id, size_t   increment, hbool_t   backing_store);
herr_t __real_H5Pget_fapl_core(hid_t   fapl_id, size_t *  increment, hbool_t *  backing_store);
hid_t __real_H5FD_family_init(void  );
void __real_H5FD_family_term(void  );
herr_t __real_H5Pset_fapl_family(hid_t   fapl_id, hsize_t   memb_size, hid_t   memb_fapl_id);
herr_t __real_H5Pget_fapl_family(hid_t   fapl_id, hsize_t *  memb_size, hid_t *  memb_fapl_id);
hid_t __real_H5FD_log_init(void  );
void __real_H5FD_log_term(void  );
herr_t __real_H5Pset_fapl_log(hid_t   fapl_id, const char *  logfile, unsigned   flags, size_t   buf_size);
hid_t __real_H5FD_multi_init(void  );
void __real_H5FD_multi_term(void  );
herr_t __real_H5Pset_fapl_multi(hid_t   fapl_id, const H5FD_mem_t *  memb_map, const hid_t *  memb_fapl, const char * const *  memb_name, const haddr_t *  memb_addr, hbool_t   relax);
herr_t __real_H5Pget_fapl_multi(hid_t   fapl_id, H5FD_mem_t *  memb_map, hid_t *  memb_fapl, char **  memb_name, haddr_t *  memb_addr, hbool_t *  relax);
herr_t __real_H5Pset_dxpl_multi(hid_t   dxpl_id, const hid_t *  memb_dxpl);
herr_t __real_H5Pget_dxpl_multi(hid_t   dxpl_id, hid_t *  memb_dxpl);
herr_t __real_H5Pset_fapl_split(hid_t   fapl, const char *  meta_ext, hid_t   meta_plist_id, const char *  raw_ext, hid_t   raw_plist_id);
hid_t __real_H5FD_sec2_init(void  );
void __real_H5FD_sec2_term(void  );
herr_t __real_H5Pset_fapl_sec2(hid_t   fapl_id);
hid_t __real_H5FD_stdio_init(void  );
void __real_H5FD_stdio_term(void  );
herr_t __real_H5Pset_fapl_stdio(hid_t   fapl_id);

herr_t __wrap_H5open(void  )
{
	herr_t ret;
	ret = __real_H5open();
	return ret;
}

herr_t __wrap_H5close(void  )
{
	herr_t ret;
	ret = __real_H5close();
	return ret;
}

herr_t __wrap_H5dont_atexit(void  )
{
	herr_t ret;
	ret = __real_H5dont_atexit();
	return ret;
}

herr_t __wrap_H5garbage_collect(void  )
{
	herr_t ret;
	ret = __real_H5garbage_collect();
	return ret;
}

herr_t __wrap_H5set_free_list_limits(int   reg_global_lim, int   reg_list_lim, int   arr_global_lim, int   arr_list_lim, int   blk_global_lim, int   blk_list_lim)
{
	herr_t ret;
	ret = __real_H5set_free_list_limits(reg_global_lim, reg_list_lim, arr_global_lim, arr_list_lim, blk_global_lim, blk_list_lim);
	return ret;
}

herr_t __wrap_H5get_libversion(unsigned *  majnum, unsigned *  minnum, unsigned *  relnum)
{
	herr_t ret;
	ret = __real_H5get_libversion(majnum, minnum, relnum);
	return ret;
}

herr_t __wrap_H5check_version(unsigned   majnum, unsigned   minnum, unsigned   relnum)
{
	herr_t ret;
	ret = __real_H5check_version(majnum, minnum, relnum);
	return ret;
}

hid_t __wrap_H5Iregister(H5I_type_t   type, const void *  object)
{
	hid_t ret;
	ret = __real_H5Iregister(type, object);
	return ret;
}

void * __wrap_H5Iobject_verify(hid_t   id, H5I_type_t   id_type)
{
	void * ret;
	ret = __real_H5Iobject_verify(id, id_type);
	return ret;
}

void * __wrap_H5Iremove_verify(hid_t   id, H5I_type_t   id_type)
{
	void * ret;
	ret = __real_H5Iremove_verify(id, id_type);
	return ret;
}

H5I_type_t __wrap_H5Iget_type(hid_t   id)
{
	H5I_type_t ret;
	ret = __real_H5Iget_type(id);
	return ret;
}

hid_t __wrap_H5Iget_file_id(hid_t   id)
{
	hid_t ret;
	ret = __real_H5Iget_file_id(id);
	return ret;
}

ssize_t __wrap_H5Iget_name(hid_t   id, char *  name, size_t   size)
{
	ssize_t ret;
	ret = __real_H5Iget_name(id, name, size);
	return ret;
}

int __wrap_H5Iinc_ref(hid_t   id)
{
	int ret;
	ret = __real_H5Iinc_ref(id);
	return ret;
}

int __wrap_H5Idec_ref(hid_t   id)
{
	int ret;
	ret = __real_H5Idec_ref(id);
	return ret;
}

int __wrap_H5Iget_ref(hid_t   id)
{
	int ret;
	ret = __real_H5Iget_ref(id);
	return ret;
}

H5I_type_t __wrap_H5Iregister_type(size_t   hash_size, unsigned   reserved, H5I_free_t   free_func)
{
	H5I_type_t ret;
	ret = __real_H5Iregister_type(hash_size, reserved, free_func);
	return ret;
}

herr_t __wrap_H5Iclear_type(H5I_type_t   type, hbool_t   force)
{
	herr_t ret;
	ret = __real_H5Iclear_type(type, force);
	return ret;
}

herr_t __wrap_H5Idestroy_type(H5I_type_t   type)
{
	herr_t ret;
	ret = __real_H5Idestroy_type(type);
	return ret;
}

int __wrap_H5Iinc_type_ref(H5I_type_t   type)
{
	int ret;
	ret = __real_H5Iinc_type_ref(type);
	return ret;
}

int __wrap_H5Idec_type_ref(H5I_type_t   type)
{
	int ret;
	ret = __real_H5Idec_type_ref(type);
	return ret;
}

int __wrap_H5Iget_type_ref(H5I_type_t   type)
{
	int ret;
	ret = __real_H5Iget_type_ref(type);
	return ret;
}

void * __wrap_H5Isearch(H5I_type_t   type, H5I_search_func_t   func, void *  key)
{
	void * ret;
	ret = __real_H5Isearch(type, func, key);
	return ret;
}

herr_t __wrap_H5Inmembers(H5I_type_t   type, hsize_t *  num_members)
{
	herr_t ret;
	ret = __real_H5Inmembers(type, num_members);
	return ret;
}

htri_t __wrap_H5Itype_exists(H5I_type_t   type)
{
	htri_t ret;
	ret = __real_H5Itype_exists(type);
	return ret;
}

htri_t __wrap_H5Iis_valid(hid_t   id)
{
	htri_t ret;
	ret = __real_H5Iis_valid(id);
	return ret;
}

hid_t __wrap_H5Tcreate(H5T_class_t   type, size_t   size)
{
	hid_t ret;
	ret = __real_H5Tcreate(type, size);
	return ret;
}

hid_t __wrap_H5Tcopy(hid_t   type_id)
{
	hid_t ret;
	ret = __real_H5Tcopy(type_id);
	return ret;
}

herr_t __wrap_H5Tclose(hid_t   type_id)
{
	herr_t ret;
	ret = __real_H5Tclose(type_id);
	return ret;
}

htri_t __wrap_H5Tequal(hid_t   type1_id, hid_t   type2_id)
{
	htri_t ret;
	ret = __real_H5Tequal(type1_id, type2_id);
	return ret;
}

herr_t __wrap_H5Tlock(hid_t   type_id)
{
	herr_t ret;
	ret = __real_H5Tlock(type_id);
	return ret;
}

herr_t __wrap_H5Tcommit2(hid_t   loc_id, const char *  name, hid_t   type_id, hid_t   lcpl_id, hid_t   tcpl_id, hid_t   tapl_id)
{
	herr_t ret;
	ret = __real_H5Tcommit2(loc_id, name, type_id, lcpl_id, tcpl_id, tapl_id);
	return ret;
}

hid_t __wrap_H5Topen2(hid_t   loc_id, const char *  name, hid_t   tapl_id)
{
	hid_t ret;
	ret = __real_H5Topen2(loc_id, name, tapl_id);
	return ret;
}

herr_t __wrap_H5Tcommit_anon(hid_t   loc_id, hid_t   type_id, hid_t   tcpl_id, hid_t   tapl_id)
{
	herr_t ret;
	ret = __real_H5Tcommit_anon(loc_id, type_id, tcpl_id, tapl_id);
	return ret;
}

hid_t __wrap_H5Tget_create_plist(hid_t   type_id)
{
	hid_t ret;
	ret = __real_H5Tget_create_plist(type_id);
	return ret;
}

htri_t __wrap_H5Tcommitted(hid_t   type_id)
{
	htri_t ret;
	ret = __real_H5Tcommitted(type_id);
	return ret;
}

herr_t __wrap_H5Tencode(hid_t   obj_id, void *  buf, size_t *  nalloc)
{
	herr_t ret;
	ret = __real_H5Tencode(obj_id, buf, nalloc);
	return ret;
}

hid_t __wrap_H5Tdecode(const void *  buf)
{
	hid_t ret;
	ret = __real_H5Tdecode(buf);
	return ret;
}

herr_t __wrap_H5Tinsert(hid_t   parent_id, const char *  name, size_t   offset, hid_t   member_id)
{
	herr_t ret;
	ret = __real_H5Tinsert(parent_id, name, offset, member_id);
	return ret;
}

herr_t __wrap_H5Tpack(hid_t   type_id)
{
	herr_t ret;
	ret = __real_H5Tpack(type_id);
	return ret;
}

hid_t __wrap_H5Tenum_create(hid_t   base_id)
{
	hid_t ret;
	ret = __real_H5Tenum_create(base_id);
	return ret;
}

herr_t __wrap_H5Tenum_insert(hid_t   type, const char *  name, const void *  value)
{
	herr_t ret;
	ret = __real_H5Tenum_insert(type, name, value);
	return ret;
}

herr_t __wrap_H5Tenum_nameof(hid_t   type, const void *  value, char *  name, size_t   size)
{
	herr_t ret;
	ret = __real_H5Tenum_nameof(type, value, name, size);
	return ret;
}

herr_t __wrap_H5Tenum_valueof(hid_t   type, const char *  name, void *  value)
{
	herr_t ret;
	ret = __real_H5Tenum_valueof(type, name, value);
	return ret;
}

hid_t __wrap_H5Tvlen_create(hid_t   base_id)
{
	hid_t ret;
	ret = __real_H5Tvlen_create(base_id);
	return ret;
}

hid_t __wrap_H5Tarray_create2(hid_t   base_id, unsigned   ndims, const hsize_t *  dim)
{
	hid_t ret;
	ret = __real_H5Tarray_create2(base_id, ndims, dim);
	return ret;
}

int __wrap_H5Tget_array_ndims(hid_t   type_id)
{
	int ret;
	ret = __real_H5Tget_array_ndims(type_id);
	return ret;
}

int __wrap_H5Tget_array_dims2(hid_t   type_id, hsize_t *  dims)
{
	int ret;
	ret = __real_H5Tget_array_dims2(type_id, dims);
	return ret;
}

herr_t __wrap_H5Tset_tag(hid_t   type, const char *  tag)
{
	herr_t ret;
	ret = __real_H5Tset_tag(type, tag);
	return ret;
}

char * __wrap_H5Tget_tag(hid_t   type)
{
	char * ret;
	ret = __real_H5Tget_tag(type);
	return ret;
}

hid_t __wrap_H5Tget_super(hid_t   type)
{
	hid_t ret;
	ret = __real_H5Tget_super(type);
	return ret;
}

H5T_class_t __wrap_H5Tget_class(hid_t   type_id)
{
	H5T_class_t ret;
	ret = __real_H5Tget_class(type_id);
	return ret;
}

htri_t __wrap_H5Tdetect_class(hid_t   type_id, H5T_class_t   cls)
{
	htri_t ret;
	ret = __real_H5Tdetect_class(type_id, cls);
	return ret;
}

size_t __wrap_H5Tget_size(hid_t   type_id)
{
	size_t ret;
	ret = __real_H5Tget_size(type_id);
	return ret;
}

H5T_order_t __wrap_H5Tget_order(hid_t   type_id)
{
	H5T_order_t ret;
	ret = __real_H5Tget_order(type_id);
	return ret;
}

size_t __wrap_H5Tget_precision(hid_t   type_id)
{
	size_t ret;
	ret = __real_H5Tget_precision(type_id);
	return ret;
}

int __wrap_H5Tget_offset(hid_t   type_id)
{
	int ret;
	ret = __real_H5Tget_offset(type_id);
	return ret;
}

herr_t __wrap_H5Tget_pad(hid_t   type_id, H5T_pad_t *  lsb, H5T_pad_t *  msb)
{
	herr_t ret;
	ret = __real_H5Tget_pad(type_id, lsb, msb);
	return ret;
}

H5T_sign_t __wrap_H5Tget_sign(hid_t   type_id)
{
	H5T_sign_t ret;
	ret = __real_H5Tget_sign(type_id);
	return ret;
}

herr_t __wrap_H5Tget_fields(hid_t   type_id, size_t *  spos, size_t *  epos, size_t *  esize, size_t *  mpos, size_t *  msize)
{
	herr_t ret;
	ret = __real_H5Tget_fields(type_id, spos, epos, esize, mpos, msize);
	return ret;
}

size_t __wrap_H5Tget_ebias(hid_t   type_id)
{
	size_t ret;
	ret = __real_H5Tget_ebias(type_id);
	return ret;
}

H5T_norm_t __wrap_H5Tget_norm(hid_t   type_id)
{
	H5T_norm_t ret;
	ret = __real_H5Tget_norm(type_id);
	return ret;
}

H5T_pad_t __wrap_H5Tget_inpad(hid_t   type_id)
{
	H5T_pad_t ret;
	ret = __real_H5Tget_inpad(type_id);
	return ret;
}

H5T_str_t __wrap_H5Tget_strpad(hid_t   type_id)
{
	H5T_str_t ret;
	ret = __real_H5Tget_strpad(type_id);
	return ret;
}

int __wrap_H5Tget_nmembers(hid_t   type_id)
{
	int ret;
	ret = __real_H5Tget_nmembers(type_id);
	return ret;
}

char * __wrap_H5Tget_member_name(hid_t   type_id, unsigned   membno)
{
	char * ret;
	ret = __real_H5Tget_member_name(type_id, membno);
	return ret;
}

int __wrap_H5Tget_member_index(hid_t   type_id, const char *  name)
{
	int ret;
	ret = __real_H5Tget_member_index(type_id, name);
	return ret;
}

size_t __wrap_H5Tget_member_offset(hid_t   type_id, unsigned   membno)
{
	size_t ret;
	ret = __real_H5Tget_member_offset(type_id, membno);
	return ret;
}

H5T_class_t __wrap_H5Tget_member_class(hid_t   type_id, unsigned   membno)
{
	H5T_class_t ret;
	ret = __real_H5Tget_member_class(type_id, membno);
	return ret;
}

hid_t __wrap_H5Tget_member_type(hid_t   type_id, unsigned   membno)
{
	hid_t ret;
	ret = __real_H5Tget_member_type(type_id, membno);
	return ret;
}

herr_t __wrap_H5Tget_member_value(hid_t   type_id, unsigned   membno, void *  value)
{
	herr_t ret;
	ret = __real_H5Tget_member_value(type_id, membno, value);
	return ret;
}

H5T_cset_t __wrap_H5Tget_cset(hid_t   type_id)
{
	H5T_cset_t ret;
	ret = __real_H5Tget_cset(type_id);
	return ret;
}

htri_t __wrap_H5Tis_variable_str(hid_t   type_id)
{
	htri_t ret;
	ret = __real_H5Tis_variable_str(type_id);
	return ret;
}

hid_t __wrap_H5Tget_native_type(hid_t   type_id, H5T_direction_t   direction)
{
	hid_t ret;
	ret = __real_H5Tget_native_type(type_id, direction);
	return ret;
}

herr_t __wrap_H5Tset_size(hid_t   type_id, size_t   size)
{
	herr_t ret;
	ret = __real_H5Tset_size(type_id, size);
	return ret;
}

herr_t __wrap_H5Tset_order(hid_t   type_id, H5T_order_t   order)
{
	herr_t ret;
	ret = __real_H5Tset_order(type_id, order);
	return ret;
}

herr_t __wrap_H5Tset_precision(hid_t   type_id, size_t   prec)
{
	herr_t ret;
	ret = __real_H5Tset_precision(type_id, prec);
	return ret;
}

herr_t __wrap_H5Tset_offset(hid_t   type_id, size_t   offset)
{
	herr_t ret;
	ret = __real_H5Tset_offset(type_id, offset);
	return ret;
}

herr_t __wrap_H5Tset_pad(hid_t   type_id, H5T_pad_t   lsb, H5T_pad_t   msb)
{
	herr_t ret;
	ret = __real_H5Tset_pad(type_id, lsb, msb);
	return ret;
}

herr_t __wrap_H5Tset_sign(hid_t   type_id, H5T_sign_t   sign)
{
	herr_t ret;
	ret = __real_H5Tset_sign(type_id, sign);
	return ret;
}

herr_t __wrap_H5Tset_fields(hid_t   type_id, size_t   spos, size_t   epos, size_t   esize, size_t   mpos, size_t   msize)
{
	herr_t ret;
	ret = __real_H5Tset_fields(type_id, spos, epos, esize, mpos, msize);
	return ret;
}

herr_t __wrap_H5Tset_ebias(hid_t   type_id, size_t   ebias)
{
	herr_t ret;
	ret = __real_H5Tset_ebias(type_id, ebias);
	return ret;
}

herr_t __wrap_H5Tset_norm(hid_t   type_id, H5T_norm_t   norm)
{
	herr_t ret;
	ret = __real_H5Tset_norm(type_id, norm);
	return ret;
}

herr_t __wrap_H5Tset_inpad(hid_t   type_id, H5T_pad_t   pad)
{
	herr_t ret;
	ret = __real_H5Tset_inpad(type_id, pad);
	return ret;
}

herr_t __wrap_H5Tset_cset(hid_t   type_id, H5T_cset_t   cset)
{
	herr_t ret;
	ret = __real_H5Tset_cset(type_id, cset);
	return ret;
}

herr_t __wrap_H5Tset_strpad(hid_t   type_id, H5T_str_t   strpad)
{
	herr_t ret;
	ret = __real_H5Tset_strpad(type_id, strpad);
	return ret;
}

herr_t __wrap_H5Tregister(H5T_pers_t   pers, const char *  name, hid_t   src_id, hid_t   dst_id, H5T_conv_t   func)
{
	herr_t ret;
	ret = __real_H5Tregister(pers, name, src_id, dst_id, func);
	return ret;
}

herr_t __wrap_H5Tunregister(H5T_pers_t   pers, const char *  name, hid_t   src_id, hid_t   dst_id, H5T_conv_t   func)
{
	herr_t ret;
	ret = __real_H5Tunregister(pers, name, src_id, dst_id, func);
	return ret;
}

H5T_conv_t __wrap_H5Tfind(hid_t   src_id, hid_t   dst_id, H5T_cdata_t **  pcdata)
{
	H5T_conv_t ret;
	ret = __real_H5Tfind(src_id, dst_id, pcdata);
	return ret;
}

htri_t __wrap_H5Tcompiler_conv(hid_t   src_id, hid_t   dst_id)
{
	htri_t ret;
	ret = __real_H5Tcompiler_conv(src_id, dst_id);
	return ret;
}

herr_t __wrap_H5Tconvert(hid_t   src_id, hid_t   dst_id, size_t   nelmts, void *  buf, void *  background, hid_t   plist_id)
{
	herr_t ret;
	ret = __real_H5Tconvert(src_id, dst_id, nelmts, buf, background, plist_id);
	return ret;
}

herr_t __wrap_H5Tcommit1(hid_t   loc_id, const char *  name, hid_t   type_id)
{
	herr_t ret;
	ret = __real_H5Tcommit1(loc_id, name, type_id);
	return ret;
}

hid_t __wrap_H5Topen1(hid_t   loc_id, const char *  name)
{
	hid_t ret;
	ret = __real_H5Topen1(loc_id, name);
	return ret;
}

hid_t __wrap_H5Tarray_create1(hid_t   base_id, int   ndims, const hsize_t *  dim, const int *  perm)
{
	hid_t ret;
	ret = __real_H5Tarray_create1(base_id, ndims, dim, perm);
	return ret;
}

int __wrap_H5Tget_array_dims1(hid_t   type_id, hsize_t *  dims, int *  perm)
{
	int ret;
	ret = __real_H5Tget_array_dims1(type_id, dims, perm);
	return ret;
}

herr_t __wrap_H5Lmove(hid_t   src_loc, const char *  src_name, hid_t   dst_loc, const char *  dst_name, hid_t   lcpl_id, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lmove(src_loc, src_name, dst_loc, dst_name, lcpl_id, lapl_id);
	return ret;
}

herr_t __wrap_H5Lcopy(hid_t   src_loc, const char *  src_name, hid_t   dst_loc, const char *  dst_name, hid_t   lcpl_id, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lcopy(src_loc, src_name, dst_loc, dst_name, lcpl_id, lapl_id);
	return ret;
}

herr_t __wrap_H5Lcreate_hard(hid_t   cur_loc, const char *  cur_name, hid_t   dst_loc, const char *  dst_name, hid_t   lcpl_id, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lcreate_hard(cur_loc, cur_name, dst_loc, dst_name, lcpl_id, lapl_id);
	return ret;
}

herr_t __wrap_H5Lcreate_soft(const char *  link_target, hid_t   link_loc_id, const char *  link_name, hid_t   lcpl_id, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lcreate_soft(link_target, link_loc_id, link_name, lcpl_id, lapl_id);
	return ret;
}

herr_t __wrap_H5Ldelete(hid_t   loc_id, const char *  name, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Ldelete(loc_id, name, lapl_id);
	return ret;
}

herr_t __wrap_H5Ldelete_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Ldelete_by_idx(loc_id, group_name, idx_type, order, n, lapl_id);
	return ret;
}

herr_t __wrap_H5Lget_val(hid_t   loc_id, const char *  name, void *  buf, size_t   size, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lget_val(loc_id, name, buf, size, lapl_id);
	return ret;
}

herr_t __wrap_H5Lget_val_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, void *  buf, size_t   size, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lget_val_by_idx(loc_id, group_name, idx_type, order, n, buf, size, lapl_id);
	return ret;
}

htri_t __wrap_H5Lexists(hid_t   loc_id, const char *  name, hid_t   lapl_id)
{
	htri_t ret;
	ret = __real_H5Lexists(loc_id, name, lapl_id);
	return ret;
}

herr_t __wrap_H5Lget_info(hid_t   loc_id, const char *  name, H5L_info_t *  linfo, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lget_info(loc_id, name, linfo, lapl_id);
	return ret;
}

herr_t __wrap_H5Lget_info_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, H5L_info_t *  linfo, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lget_info_by_idx(loc_id, group_name, idx_type, order, n, linfo, lapl_id);
	return ret;
}

ssize_t __wrap_H5Lget_name_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, char *  name, size_t   size, hid_t   lapl_id)
{
	ssize_t ret;
	ret = __real_H5Lget_name_by_idx(loc_id, group_name, idx_type, order, n, name, size, lapl_id);
	return ret;
}

herr_t __wrap_H5Literate(hid_t   grp_id, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t *  idx, H5L_iterate_t   op, void *  op_data)
{
	herr_t ret;
	ret = __real_H5Literate(grp_id, idx_type, order, idx, op, op_data);
	return ret;
}

herr_t __wrap_H5Literate_by_name(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t *  idx, H5L_iterate_t   op, void *  op_data, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Literate_by_name(loc_id, group_name, idx_type, order, idx, op, op_data, lapl_id);
	return ret;
}

herr_t __wrap_H5Lvisit(hid_t   grp_id, H5_index_t   idx_type, H5_iter_order_t   order, H5L_iterate_t   op, void *  op_data)
{
	herr_t ret;
	ret = __real_H5Lvisit(grp_id, idx_type, order, op, op_data);
	return ret;
}

herr_t __wrap_H5Lvisit_by_name(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, H5L_iterate_t   op, void *  op_data, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lvisit_by_name(loc_id, group_name, idx_type, order, op, op_data, lapl_id);
	return ret;
}

herr_t __wrap_H5Lcreate_ud(hid_t   link_loc_id, const char *  link_name, H5L_type_t   link_type, const void *  udata, size_t   udata_size, hid_t   lcpl_id, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lcreate_ud(link_loc_id, link_name, link_type, udata, udata_size, lcpl_id, lapl_id);
	return ret;
}

herr_t __wrap_H5Lregister(const H5L_class_t *  cls)
{
	herr_t ret;
	ret = __real_H5Lregister(cls);
	return ret;
}

herr_t __wrap_H5Lunregister(H5L_type_t   id)
{
	herr_t ret;
	ret = __real_H5Lunregister(id);
	return ret;
}

htri_t __wrap_H5Lis_registered(H5L_type_t   id)
{
	htri_t ret;
	ret = __real_H5Lis_registered(id);
	return ret;
}

herr_t __wrap_H5Lunpack_elink_val(const void *  ext_linkval, size_t   link_size, unsigned *  flags, const char **  filename, const char **  obj_path)
{
	herr_t ret;
	ret = __real_H5Lunpack_elink_val(ext_linkval, link_size, flags, filename, obj_path);
	return ret;
}

herr_t __wrap_H5Lcreate_external(const char *  file_name, const char *  obj_name, hid_t   link_loc_id, const char *  link_name, hid_t   lcpl_id, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Lcreate_external(file_name, obj_name, link_loc_id, link_name, lcpl_id, lapl_id);
	return ret;
}

hid_t __wrap_H5Oopen(hid_t   loc_id, const char *  name, hid_t   lapl_id)
{
	hid_t ret;
	ret = __real_H5Oopen(loc_id, name, lapl_id);
	return ret;
}

hid_t __wrap_H5Oopen_by_addr(hid_t   loc_id, haddr_t   addr)
{
	hid_t ret;
	ret = __real_H5Oopen_by_addr(loc_id, addr);
	return ret;
}

hid_t __wrap_H5Oopen_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, hid_t   lapl_id)
{
	hid_t ret;
	ret = __real_H5Oopen_by_idx(loc_id, group_name, idx_type, order, n, lapl_id);
	return ret;
}

herr_t __wrap_H5Oget_info(hid_t   loc_id, H5O_info_t *  oinfo)
{
	herr_t ret;
	ret = __real_H5Oget_info(loc_id, oinfo);
	return ret;
}

herr_t __wrap_H5Oget_info_by_name(hid_t   loc_id, const char *  name, H5O_info_t *  oinfo, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Oget_info_by_name(loc_id, name, oinfo, lapl_id);
	return ret;
}

herr_t __wrap_H5Oget_info_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, H5O_info_t *  oinfo, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Oget_info_by_idx(loc_id, group_name, idx_type, order, n, oinfo, lapl_id);
	return ret;
}

herr_t __wrap_H5Olink(hid_t   obj_id, hid_t   new_loc_id, const char *  new_name, hid_t   lcpl_id, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Olink(obj_id, new_loc_id, new_name, lcpl_id, lapl_id);
	return ret;
}

herr_t __wrap_H5Oincr_refcount(hid_t   object_id)
{
	herr_t ret;
	ret = __real_H5Oincr_refcount(object_id);
	return ret;
}

herr_t __wrap_H5Odecr_refcount(hid_t   object_id)
{
	herr_t ret;
	ret = __real_H5Odecr_refcount(object_id);
	return ret;
}

herr_t __wrap_H5Ocopy(hid_t   src_loc_id, const char *  src_name, hid_t   dst_loc_id, const char *  dst_name, hid_t   ocpypl_id, hid_t   lcpl_id)
{
	herr_t ret;
	ret = __real_H5Ocopy(src_loc_id, src_name, dst_loc_id, dst_name, ocpypl_id, lcpl_id);
	return ret;
}

herr_t __wrap_H5Oset_comment(hid_t   obj_id, const char *  comment)
{
	herr_t ret;
	ret = __real_H5Oset_comment(obj_id, comment);
	return ret;
}

herr_t __wrap_H5Oset_comment_by_name(hid_t   loc_id, const char *  name, const char *  comment, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Oset_comment_by_name(loc_id, name, comment, lapl_id);
	return ret;
}

ssize_t __wrap_H5Oget_comment(hid_t   obj_id, char *  comment, size_t   bufsize)
{
	ssize_t ret;
	ret = __real_H5Oget_comment(obj_id, comment, bufsize);
	return ret;
}

ssize_t __wrap_H5Oget_comment_by_name(hid_t   loc_id, const char *  name, char *  comment, size_t   bufsize, hid_t   lapl_id)
{
	ssize_t ret;
	ret = __real_H5Oget_comment_by_name(loc_id, name, comment, bufsize, lapl_id);
	return ret;
}

herr_t __wrap_H5Ovisit(hid_t   obj_id, H5_index_t   idx_type, H5_iter_order_t   order, H5O_iterate_t   op, void *  op_data)
{
	herr_t ret;
	ret = __real_H5Ovisit(obj_id, idx_type, order, op, op_data);
	return ret;
}

herr_t __wrap_H5Ovisit_by_name(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, H5O_iterate_t   op, void *  op_data, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Ovisit_by_name(loc_id, obj_name, idx_type, order, op, op_data, lapl_id);
	return ret;
}

herr_t __wrap_H5Oclose(hid_t   object_id)
{
	herr_t ret;
	ret = __real_H5Oclose(object_id);
	return ret;
}

hid_t __wrap_H5Acreate2(hid_t   loc_id, const char *  attr_name, hid_t   type_id, hid_t   space_id, hid_t   acpl_id, hid_t   aapl_id)
{
	hid_t ret;
	ret = __real_H5Acreate2(loc_id, attr_name, type_id, space_id, acpl_id, aapl_id);
	return ret;
}

hid_t __wrap_H5Acreate_by_name(hid_t   loc_id, const char *  obj_name, const char *  attr_name, hid_t   type_id, hid_t   space_id, hid_t   acpl_id, hid_t   aapl_id, hid_t   lapl_id)
{
	hid_t ret;
	ret = __real_H5Acreate_by_name(loc_id, obj_name, attr_name, type_id, space_id, acpl_id, aapl_id, lapl_id);
	return ret;
}

hid_t __wrap_H5Aopen(hid_t   obj_id, const char *  attr_name, hid_t   aapl_id)
{
	hid_t ret;
	ret = __real_H5Aopen(obj_id, attr_name, aapl_id);
	return ret;
}

hid_t __wrap_H5Aopen_by_name(hid_t   loc_id, const char *  obj_name, const char *  attr_name, hid_t   aapl_id, hid_t   lapl_id)
{
	hid_t ret;
	ret = __real_H5Aopen_by_name(loc_id, obj_name, attr_name, aapl_id, lapl_id);
	return ret;
}

hid_t __wrap_H5Aopen_by_idx(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, hid_t   aapl_id, hid_t   lapl_id)
{
	hid_t ret;
	ret = __real_H5Aopen_by_idx(loc_id, obj_name, idx_type, order, n, aapl_id, lapl_id);
	return ret;
}

herr_t __wrap_H5Awrite(hid_t   attr_id, hid_t   type_id, const void *  buf)
{
	herr_t ret;
	ret = __real_H5Awrite(attr_id, type_id, buf);
	return ret;
}

herr_t __wrap_H5Aread(hid_t   attr_id, hid_t   type_id, void *  buf)
{
	herr_t ret;
	ret = __real_H5Aread(attr_id, type_id, buf);
	return ret;
}

herr_t __wrap_H5Aclose(hid_t   attr_id)
{
	herr_t ret;
	ret = __real_H5Aclose(attr_id);
	return ret;
}

hid_t __wrap_H5Aget_space(hid_t   attr_id)
{
	hid_t ret;
	ret = __real_H5Aget_space(attr_id);
	return ret;
}

hid_t __wrap_H5Aget_type(hid_t   attr_id)
{
	hid_t ret;
	ret = __real_H5Aget_type(attr_id);
	return ret;
}

hid_t __wrap_H5Aget_create_plist(hid_t   attr_id)
{
	hid_t ret;
	ret = __real_H5Aget_create_plist(attr_id);
	return ret;
}

ssize_t __wrap_H5Aget_name(hid_t   attr_id, size_t   buf_size, char *  buf)
{
	ssize_t ret;
	ret = __real_H5Aget_name(attr_id, buf_size, buf);
	return ret;
}

ssize_t __wrap_H5Aget_name_by_idx(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, char *  name, size_t   size, hid_t   lapl_id)
{
	ssize_t ret;
	ret = __real_H5Aget_name_by_idx(loc_id, obj_name, idx_type, order, n, name, size, lapl_id);
	return ret;
}

hsize_t __wrap_H5Aget_storage_size(hid_t   attr_id)
{
	hsize_t ret;
	ret = __real_H5Aget_storage_size(attr_id);
	return ret;
}

herr_t __wrap_H5Aget_info(hid_t   attr_id, H5A_info_t *  ainfo)
{
	herr_t ret;
	ret = __real_H5Aget_info(attr_id, ainfo);
	return ret;
}

herr_t __wrap_H5Aget_info_by_name(hid_t   loc_id, const char *  obj_name, const char *  attr_name, H5A_info_t *  ainfo, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Aget_info_by_name(loc_id, obj_name, attr_name, ainfo, lapl_id);
	return ret;
}

herr_t __wrap_H5Aget_info_by_idx(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, H5A_info_t *  ainfo, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Aget_info_by_idx(loc_id, obj_name, idx_type, order, n, ainfo, lapl_id);
	return ret;
}

herr_t __wrap_H5Arename(hid_t   loc_id, const char *  old_name, const char *  new_name)
{
	herr_t ret;
	ret = __real_H5Arename(loc_id, old_name, new_name);
	return ret;
}

herr_t __wrap_H5Arename_by_name(hid_t   loc_id, const char *  obj_name, const char *  old_attr_name, const char *  new_attr_name, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Arename_by_name(loc_id, obj_name, old_attr_name, new_attr_name, lapl_id);
	return ret;
}

herr_t __wrap_H5Aiterate2(hid_t   loc_id, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t *  idx, H5A_operator2_t   op, void *  op_data)
{
	herr_t ret;
	ret = __real_H5Aiterate2(loc_id, idx_type, order, idx, op, op_data);
	return ret;
}

herr_t __wrap_H5Aiterate_by_name(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t *  idx, H5A_operator2_t   op, void *  op_data, hid_t   lapd_id)
{
	herr_t ret;
	ret = __real_H5Aiterate_by_name(loc_id, obj_name, idx_type, order, idx, op, op_data, lapd_id);
	return ret;
}

herr_t __wrap_H5Adelete(hid_t   loc_id, const char *  name)
{
	herr_t ret;
	ret = __real_H5Adelete(loc_id, name);
	return ret;
}

herr_t __wrap_H5Adelete_by_name(hid_t   loc_id, const char *  obj_name, const char *  attr_name, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Adelete_by_name(loc_id, obj_name, attr_name, lapl_id);
	return ret;
}

herr_t __wrap_H5Adelete_by_idx(hid_t   loc_id, const char *  obj_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Adelete_by_idx(loc_id, obj_name, idx_type, order, n, lapl_id);
	return ret;
}

htri_t __wrap_H5Aexists(hid_t   obj_id, const char *  attr_name)
{
	htri_t ret;
	ret = __real_H5Aexists(obj_id, attr_name);
	return ret;
}

htri_t __wrap_H5Aexists_by_name(hid_t   obj_id, const char *  obj_name, const char *  attr_name, hid_t   lapl_id)
{
	htri_t ret;
	ret = __real_H5Aexists_by_name(obj_id, obj_name, attr_name, lapl_id);
	return ret;
}

hid_t __wrap_H5Acreate1(hid_t   loc_id, const char *  name, hid_t   type_id, hid_t   space_id, hid_t   acpl_id)
{
	hid_t ret;
	ret = __real_H5Acreate1(loc_id, name, type_id, space_id, acpl_id);
	return ret;
}

hid_t __wrap_H5Aopen_name(hid_t   loc_id, const char *  name)
{
	hid_t ret;
	ret = __real_H5Aopen_name(loc_id, name);
	return ret;
}

hid_t __wrap_H5Aopen_idx(hid_t   loc_id, unsigned   idx)
{
	hid_t ret;
	ret = __real_H5Aopen_idx(loc_id, idx);
	return ret;
}

int __wrap_H5Aget_num_attrs(hid_t   loc_id)
{
	int ret;
	ret = __real_H5Aget_num_attrs(loc_id);
	return ret;
}

herr_t __wrap_H5Aiterate1(hid_t   loc_id, unsigned *  attr_num, H5A_operator1_t   op, void *  op_data)
{
	herr_t ret;
	ret = __real_H5Aiterate1(loc_id, attr_num, op, op_data);
	return ret;
}

hid_t __wrap_H5Dcreate2(hid_t   loc_id, const char *  name, hid_t   type_id, hid_t   space_id, hid_t   lcpl_id, hid_t   dcpl_id, hid_t   dapl_id)
{
	hid_t ret;
	ret = __real_H5Dcreate2(loc_id, name, type_id, space_id, lcpl_id, dcpl_id, dapl_id);
	return ret;
}

hid_t __wrap_H5Dcreate_anon(hid_t   file_id, hid_t   type_id, hid_t   space_id, hid_t   plist_id, hid_t   dapl_id)
{
	hid_t ret;
	ret = __real_H5Dcreate_anon(file_id, type_id, space_id, plist_id, dapl_id);
	return ret;
}

hid_t __wrap_H5Dopen2(hid_t   file_id, const char *  name, hid_t   dapl_id)
{
	hid_t ret;
	ret = __real_H5Dopen2(file_id, name, dapl_id);
	return ret;
}

herr_t __wrap_H5Dclose(hid_t   dset_id)
{
	herr_t ret;
	ret = __real_H5Dclose(dset_id);
	return ret;
}

hid_t __wrap_H5Dget_space(hid_t   dset_id)
{
	hid_t ret;
	ret = __real_H5Dget_space(dset_id);
	return ret;
}

herr_t __wrap_H5Dget_space_status(hid_t   dset_id, H5D_space_status_t *  allocation)
{
	herr_t ret;
	ret = __real_H5Dget_space_status(dset_id, allocation);
	return ret;
}

hid_t __wrap_H5Dget_type(hid_t   dset_id)
{
	hid_t ret;
	ret = __real_H5Dget_type(dset_id);
	return ret;
}

hid_t __wrap_H5Dget_create_plist(hid_t   dset_id)
{
	hid_t ret;
	ret = __real_H5Dget_create_plist(dset_id);
	return ret;
}

hid_t __wrap_H5Dget_access_plist(hid_t   dset_id)
{
	hid_t ret;
	ret = __real_H5Dget_access_plist(dset_id);
	return ret;
}

hsize_t __wrap_H5Dget_storage_size(hid_t   dset_id)
{
	hsize_t ret;
	ret = __real_H5Dget_storage_size(dset_id);
	return ret;
}

haddr_t __wrap_H5Dget_offset(hid_t   dset_id)
{
	haddr_t ret;
	ret = __real_H5Dget_offset(dset_id);
	return ret;
}

herr_t __wrap_H5Dread(hid_t   dset_id, hid_t   mem_type_id, hid_t   mem_space_id, hid_t   file_space_id, hid_t   plist_id, void *  buf)
{
	herr_t ret;
	ret = __real_H5Dread(dset_id, mem_type_id, mem_space_id, file_space_id, plist_id, buf);
	return ret;
}

herr_t __wrap_H5Dwrite(hid_t   dset_id, hid_t   mem_type_id, hid_t   mem_space_id, hid_t   file_space_id, hid_t   plist_id, const void *  buf)
{
	herr_t ret;
	ret = __real_H5Dwrite(dset_id, mem_type_id, mem_space_id, file_space_id, plist_id, buf);
	return ret;
}

herr_t __wrap_H5Diterate(void *  buf, hid_t   type_id, hid_t   space_id, H5D_operator_t   op, void *  operator_data)
{
	herr_t ret;
	ret = __real_H5Diterate(buf, type_id, space_id, op, operator_data);
	return ret;
}

herr_t __wrap_H5Dvlen_reclaim(hid_t   type_id, hid_t   space_id, hid_t   plist_id, void *  buf)
{
	herr_t ret;
	ret = __real_H5Dvlen_reclaim(type_id, space_id, plist_id, buf);
	return ret;
}

herr_t __wrap_H5Dvlen_get_buf_size(hid_t   dataset_id, hid_t   type_id, hid_t   space_id, hsize_t *  size)
{
	herr_t ret;
	ret = __real_H5Dvlen_get_buf_size(dataset_id, type_id, space_id, size);
	return ret;
}

herr_t __wrap_H5Dfill(const void *  fill, hid_t   fill_type, void *  buf, hid_t   buf_type, hid_t   space)
{
	herr_t ret;
	ret = __real_H5Dfill(fill, fill_type, buf, buf_type, space);
	return ret;
}

herr_t __wrap_H5Dset_extent(hid_t   dset_id, const hsize_t *  size)
{
	herr_t ret;
	ret = __real_H5Dset_extent(dset_id, size);
	return ret;
}

herr_t __wrap_H5Ddebug(hid_t   dset_id)
{
	herr_t ret;
	ret = __real_H5Ddebug(dset_id);
	return ret;
}

hid_t __wrap_H5Dcreate1(hid_t   file_id, const char *  name, hid_t   type_id, hid_t   space_id, hid_t   dcpl_id)
{
	hid_t ret;
	ret = __real_H5Dcreate1(file_id, name, type_id, space_id, dcpl_id);
	return ret;
}

hid_t __wrap_H5Dopen1(hid_t   file_id, const char *  name)
{
	hid_t ret;
	ret = __real_H5Dopen1(file_id, name);
	return ret;
}

herr_t __wrap_H5Dextend(hid_t   dset_id, const hsize_t *  size)
{
	herr_t ret;
	ret = __real_H5Dextend(dset_id, size);
	return ret;
}

hid_t __wrap_H5Eregister_class(const char *  cls_name, const char *  lib_name, const char *  version)
{
	hid_t ret;
	ret = __real_H5Eregister_class(cls_name, lib_name, version);
	return ret;
}

herr_t __wrap_H5Eunregister_class(hid_t   class_id)
{
	herr_t ret;
	ret = __real_H5Eunregister_class(class_id);
	return ret;
}

herr_t __wrap_H5Eclose_msg(hid_t   err_id)
{
	herr_t ret;
	ret = __real_H5Eclose_msg(err_id);
	return ret;
}

hid_t __wrap_H5Ecreate_msg(hid_t   cls, H5E_type_t   msg_type, const char *  msg)
{
	hid_t ret;
	ret = __real_H5Ecreate_msg(cls, msg_type, msg);
	return ret;
}

hid_t __wrap_H5Ecreate_stack(void  )
{
	hid_t ret;
	ret = __real_H5Ecreate_stack();
	return ret;
}

hid_t __wrap_H5Eget_current_stack(void  )
{
	hid_t ret;
	ret = __real_H5Eget_current_stack();
	return ret;
}

herr_t __wrap_H5Eclose_stack(hid_t   stack_id)
{
	herr_t ret;
	ret = __real_H5Eclose_stack(stack_id);
	return ret;
}

ssize_t __wrap_H5Eget_class_name(hid_t   class_id, char *  name, size_t   size)
{
	ssize_t ret;
	ret = __real_H5Eget_class_name(class_id, name, size);
	return ret;
}

herr_t __wrap_H5Eset_current_stack(hid_t   err_stack_id)
{
	herr_t ret;
	ret = __real_H5Eset_current_stack(err_stack_id);
	return ret;
}

herr_t __wrap_H5Epop(hid_t   err_stack, size_t   count)
{
	herr_t ret;
	ret = __real_H5Epop(err_stack, count);
	return ret;
}

herr_t __wrap_H5Eprint2(hid_t   err_stack, FILE *  stream)
{
	herr_t ret;
	ret = __real_H5Eprint2(err_stack, stream);
	return ret;
}

herr_t __wrap_H5Ewalk2(hid_t   err_stack, H5E_direction_t   direction, H5E_walk2_t   func, void *  client_data)
{
	herr_t ret;
	ret = __real_H5Ewalk2(err_stack, direction, func, client_data);
	return ret;
}

herr_t __wrap_H5Eget_auto2(hid_t   estack_id, H5E_auto2_t *  func, void **  client_data)
{
	herr_t ret;
	ret = __real_H5Eget_auto2(estack_id, func, client_data);
	return ret;
}

herr_t __wrap_H5Eset_auto2(hid_t   estack_id, H5E_auto2_t   func, void *  client_data)
{
	herr_t ret;
	ret = __real_H5Eset_auto2(estack_id, func, client_data);
	return ret;
}

herr_t __wrap_H5Eclear2(hid_t   err_stack)
{
	herr_t ret;
	ret = __real_H5Eclear2(err_stack);
	return ret;
}

herr_t __wrap_H5Eauto_is_v2(hid_t   err_stack, unsigned *  is_stack)
{
	herr_t ret;
	ret = __real_H5Eauto_is_v2(err_stack, is_stack);
	return ret;
}

ssize_t __wrap_H5Eget_msg(hid_t   msg_id, H5E_type_t *  type, char *  msg, size_t   size)
{
	ssize_t ret;
	ret = __real_H5Eget_msg(msg_id, type, msg, size);
	return ret;
}

ssize_t __wrap_H5Eget_num(hid_t   error_stack_id)
{
	ssize_t ret;
	ret = __real_H5Eget_num(error_stack_id);
	return ret;
}

herr_t __wrap_H5Eclear1(void  )
{
	herr_t ret;
	ret = __real_H5Eclear1();
	return ret;
}

herr_t __wrap_H5Eget_auto1(H5E_auto1_t *  func, void **  client_data)
{
	herr_t ret;
	ret = __real_H5Eget_auto1(func, client_data);
	return ret;
}

herr_t __wrap_H5Epush1(const char *  file, const char *  func, unsigned   line, H5E_major_t   maj, H5E_minor_t   min, const char *  str)
{
	herr_t ret;
	ret = __real_H5Epush1(file, func, line, maj, min, str);
	return ret;
}

herr_t __wrap_H5Eprint1(FILE *  stream)
{
	herr_t ret;
	ret = __real_H5Eprint1(stream);
	return ret;
}

herr_t __wrap_H5Eset_auto1(H5E_auto1_t   func, void *  client_data)
{
	herr_t ret;
	ret = __real_H5Eset_auto1(func, client_data);
	return ret;
}

herr_t __wrap_H5Ewalk1(H5E_direction_t   direction, H5E_walk1_t   func, void *  client_data)
{
	herr_t ret;
	ret = __real_H5Ewalk1(direction, func, client_data);
	return ret;
}

char * __wrap_H5Eget_major(H5E_major_t   maj)
{
	char * ret;
	ret = __real_H5Eget_major(maj);
	return ret;
}

char * __wrap_H5Eget_minor(H5E_minor_t   min)
{
	char * ret;
	ret = __real_H5Eget_minor(min);
	return ret;
}

htri_t __wrap_H5Fis_hdf5(const char *  filename)
{
	htri_t ret;
	ret = __real_H5Fis_hdf5(filename);
	return ret;
}

hid_t __wrap_H5Fcreate(const char *  filename, unsigned   flags, hid_t   create_plist, hid_t   access_plist)
{
	hid_t ret;
	printf("Creating file: %s (SIOX TRACE CALL)\n", filename);
	ret = __real_H5Fcreate(filename, flags, create_plist, access_plist);
	return ret;
}

hid_t __wrap_H5Fopen(const char *  filename, unsigned   flags, hid_t   access_plist)
{
	hid_t ret;
	ret = __real_H5Fopen(filename, flags, access_plist);
	return ret;
}

hid_t __wrap_H5Freopen(hid_t   file_id)
{
	hid_t ret;
	ret = __real_H5Freopen(file_id);
	return ret;
}

herr_t __wrap_H5Fflush(hid_t   object_id, H5F_scope_t   scope)
{
	herr_t ret;
	ret = __real_H5Fflush(object_id, scope);
	return ret;
}

herr_t __wrap_H5Fclose(hid_t   file_id)
{
	herr_t ret;
	ret = __real_H5Fclose(file_id);
	return ret;
}

hid_t __wrap_H5Fget_create_plist(hid_t   file_id)
{
	hid_t ret;
	ret = __real_H5Fget_create_plist(file_id);
	return ret;
}

hid_t __wrap_H5Fget_access_plist(hid_t   file_id)
{
	hid_t ret;
	ret = __real_H5Fget_access_plist(file_id);
	return ret;
}

herr_t __wrap_H5Fget_intent(hid_t   file_id, unsigned *   intent)
{
	herr_t ret;
	ret = __real_H5Fget_intent(file_id, intent);
	return ret;
}

ssize_t __wrap_H5Fget_obj_count(hid_t   file_id, unsigned   types)
{
	ssize_t ret;
	ret = __real_H5Fget_obj_count(file_id, types);
	return ret;
}

ssize_t __wrap_H5Fget_obj_ids(hid_t   file_id, unsigned   types, size_t   max_objs, hid_t *  obj_id_list)
{
	ssize_t ret;
	ret = __real_H5Fget_obj_ids(file_id, types, max_objs, obj_id_list);
	return ret;
}

herr_t __wrap_H5Fget_vfd_handle(hid_t   file_id, hid_t   fapl, void **  file_handle)
{
	herr_t ret;
	ret = __real_H5Fget_vfd_handle(file_id, fapl, file_handle);
	return ret;
}

herr_t __wrap_H5Fmount(hid_t   loc, const char *  name, hid_t   child, hid_t   plist)
{
	herr_t ret;
	ret = __real_H5Fmount(loc, name, child, plist);
	return ret;
}

herr_t __wrap_H5Funmount(hid_t   loc, const char *  name)
{
	herr_t ret;
	ret = __real_H5Funmount(loc, name);
	return ret;
}

hssize_t __wrap_H5Fget_freespace(hid_t   file_id)
{
	hssize_t ret;
	ret = __real_H5Fget_freespace(file_id);
	return ret;
}

herr_t __wrap_H5Fget_filesize(hid_t   file_id, hsize_t *  size)
{
	herr_t ret;
	ret = __real_H5Fget_filesize(file_id, size);
	return ret;
}

herr_t __wrap_H5Fget_mdc_config(hid_t   file_id, H5AC_cache_config_t *   config_ptr)
{
	herr_t ret;
	ret = __real_H5Fget_mdc_config(file_id, config_ptr);
	return ret;
}

herr_t __wrap_H5Fset_mdc_config(hid_t   file_id, H5AC_cache_config_t *   config_ptr)
{
	herr_t ret;
	ret = __real_H5Fset_mdc_config(file_id, config_ptr);
	return ret;
}

herr_t __wrap_H5Fget_mdc_hit_rate(hid_t   file_id, double *   hit_rate_ptr)
{
	herr_t ret;
	ret = __real_H5Fget_mdc_hit_rate(file_id, hit_rate_ptr);
	return ret;
}

herr_t __wrap_H5Fget_mdc_size(hid_t   file_id, size_t *   max_size_ptr, size_t *   min_clean_size_ptr, size_t *   cur_size_ptr, int *   cur_num_entries_ptr)
{
	herr_t ret;
	ret = __real_H5Fget_mdc_size(file_id, max_size_ptr, min_clean_size_ptr, cur_size_ptr, cur_num_entries_ptr);
	return ret;
}

herr_t __wrap_H5Freset_mdc_hit_rate_stats(hid_t   file_id)
{
	herr_t ret;
	ret = __real_H5Freset_mdc_hit_rate_stats(file_id);
	return ret;
}

ssize_t __wrap_H5Fget_name(hid_t   obj_id, char *  name, size_t   size)
{
	ssize_t ret;
	ret = __real_H5Fget_name(obj_id, name, size);
	return ret;
}

herr_t __wrap_H5Fget_info(hid_t   obj_id, H5F_info_t *  bh_info)
{
	herr_t ret;
	ret = __real_H5Fget_info(obj_id, bh_info);
	return ret;
}

hid_t __wrap_H5FDregister(const H5FD_class_t *  cls)
{
	hid_t ret;
	ret = __real_H5FDregister(cls);
	return ret;
}

herr_t __wrap_H5FDunregister(hid_t   driver_id)
{
	herr_t ret;
	ret = __real_H5FDunregister(driver_id);
	return ret;
}

H5FD_t * __wrap_H5FDopen(const char *  name, unsigned   flags, hid_t   fapl_id, haddr_t   maxaddr)
{
	H5FD_t * ret;
	ret = __real_H5FDopen(name, flags, fapl_id, maxaddr);
	return ret;
}

herr_t __wrap_H5FDclose(H5FD_t *  file)
{
	herr_t ret;
	ret = __real_H5FDclose(file);
	return ret;
}

int __wrap_H5FDcmp(const H5FD_t *  f1, const H5FD_t *  f2)
{
	int ret;
	ret = __real_H5FDcmp(f1, f2);
	return ret;
}

int __wrap_H5FDquery(const H5FD_t *  f, unsigned long *  flags)
{
	int ret;
	ret = __real_H5FDquery(f, flags);
	return ret;
}

haddr_t __wrap_H5FDalloc(H5FD_t *  file, H5FD_mem_t   type, hid_t   dxpl_id, hsize_t   size)
{
	haddr_t ret;
	ret = __real_H5FDalloc(file, type, dxpl_id, size);
	return ret;
}

herr_t __wrap_H5FDfree(H5FD_t *  file, H5FD_mem_t   type, hid_t   dxpl_id, haddr_t   addr, hsize_t   size)
{
	herr_t ret;
	ret = __real_H5FDfree(file, type, dxpl_id, addr, size);
	return ret;
}

haddr_t __wrap_H5FDget_eoa(H5FD_t *  file, H5FD_mem_t   type)
{
	haddr_t ret;
	ret = __real_H5FDget_eoa(file, type);
	return ret;
}

herr_t __wrap_H5FDset_eoa(H5FD_t *  file, H5FD_mem_t   type, haddr_t   eoa)
{
	herr_t ret;
	ret = __real_H5FDset_eoa(file, type, eoa);
	return ret;
}

haddr_t __wrap_H5FDget_eof(H5FD_t *  file)
{
	haddr_t ret;
	ret = __real_H5FDget_eof(file);
	return ret;
}

herr_t __wrap_H5FDget_vfd_handle(H5FD_t *  file, hid_t   fapl, void**  file_handle)
{
	herr_t ret;
	ret = __real_H5FDget_vfd_handle(file, fapl, file_handle);
	return ret;
}

herr_t __wrap_H5FDread(H5FD_t *  file, H5FD_mem_t   type, hid_t   dxpl_id, haddr_t   addr, size_t   size, void *  buf)
{
	herr_t ret;
	ret = __real_H5FDread(file, type, dxpl_id, addr, size, buf);
	return ret;
}

herr_t __wrap_H5FDwrite(H5FD_t *  file, H5FD_mem_t   type, hid_t   dxpl_id, haddr_t   addr, size_t   size, const void *  buf)
{
	herr_t ret;
	ret = __real_H5FDwrite(file, type, dxpl_id, addr, size, buf);
	return ret;
}

herr_t __wrap_H5FDflush(H5FD_t *  file, hid_t   dxpl_id, unsigned   closing)
{
	herr_t ret;
	ret = __real_H5FDflush(file, dxpl_id, closing);
	return ret;
}

herr_t __wrap_H5FDtruncate(H5FD_t *  file, hid_t   dxpl_id, hbool_t   closing)
{
	herr_t ret;
	ret = __real_H5FDtruncate(file, dxpl_id, closing);
	return ret;
}

hid_t __wrap_H5Gcreate2(hid_t   loc_id, const char *  name, hid_t   lcpl_id, hid_t   gcpl_id, hid_t   gapl_id)
{
	hid_t ret;
	ret = __real_H5Gcreate2(loc_id, name, lcpl_id, gcpl_id, gapl_id);
	return ret;
}

hid_t __wrap_H5Gcreate_anon(hid_t   loc_id, hid_t   gcpl_id, hid_t   gapl_id)
{
	hid_t ret;
	ret = __real_H5Gcreate_anon(loc_id, gcpl_id, gapl_id);
	return ret;
}

hid_t __wrap_H5Gopen2(hid_t   loc_id, const char *  name, hid_t   gapl_id)
{
	hid_t ret;
	ret = __real_H5Gopen2(loc_id, name, gapl_id);
	return ret;
}

hid_t __wrap_H5Gget_create_plist(hid_t   group_id)
{
	hid_t ret;
	ret = __real_H5Gget_create_plist(group_id);
	return ret;
}

herr_t __wrap_H5Gget_info(hid_t   loc_id, H5G_info_t *  ginfo)
{
	herr_t ret;
	ret = __real_H5Gget_info(loc_id, ginfo);
	return ret;
}

herr_t __wrap_H5Gget_info_by_name(hid_t   loc_id, const char *  name, H5G_info_t *  ginfo, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Gget_info_by_name(loc_id, name, ginfo, lapl_id);
	return ret;
}

herr_t __wrap_H5Gget_info_by_idx(hid_t   loc_id, const char *  group_name, H5_index_t   idx_type, H5_iter_order_t   order, hsize_t   n, H5G_info_t *  ginfo, hid_t   lapl_id)
{
	herr_t ret;
	ret = __real_H5Gget_info_by_idx(loc_id, group_name, idx_type, order, n, ginfo, lapl_id);
	return ret;
}

herr_t __wrap_H5Gclose(hid_t   group_id)
{
	herr_t ret;
	ret = __real_H5Gclose(group_id);
	return ret;
}

hid_t __wrap_H5Gcreate1(hid_t   loc_id, const char *  name, size_t   size_hint)
{
	hid_t ret;
	ret = __real_H5Gcreate1(loc_id, name, size_hint);
	return ret;
}

hid_t __wrap_H5Gopen1(hid_t   loc_id, const char *  name)
{
	hid_t ret;
	ret = __real_H5Gopen1(loc_id, name);
	return ret;
}

herr_t __wrap_H5Glink(hid_t   cur_loc_id, H5L_type_t   type, const char *  cur_name, const char *  new_name)
{
	herr_t ret;
	ret = __real_H5Glink(cur_loc_id, type, cur_name, new_name);
	return ret;
}

herr_t __wrap_H5Glink2(hid_t   cur_loc_id, const char *  cur_name, H5L_type_t   type, hid_t   new_loc_id, const char *  new_name)
{
	herr_t ret;
	ret = __real_H5Glink2(cur_loc_id, cur_name, type, new_loc_id, new_name);
	return ret;
}

herr_t __wrap_H5Gmove(hid_t   src_loc_id, const char *  src_name, const char *  dst_name)
{
	herr_t ret;
	ret = __real_H5Gmove(src_loc_id, src_name, dst_name);
	return ret;
}

herr_t __wrap_H5Gmove2(hid_t   src_loc_id, const char *  src_name, hid_t   dst_loc_id, const char *  dst_name)
{
	herr_t ret;
	ret = __real_H5Gmove2(src_loc_id, src_name, dst_loc_id, dst_name);
	return ret;
}

herr_t __wrap_H5Gunlink(hid_t   loc_id, const char *  name)
{
	herr_t ret;
	ret = __real_H5Gunlink(loc_id, name);
	return ret;
}

herr_t __wrap_H5Gget_linkval(hid_t   loc_id, const char *  name, size_t   size, char *  buf)
{
	herr_t ret;
	ret = __real_H5Gget_linkval(loc_id, name, size, buf);
	return ret;
}

herr_t __wrap_H5Gset_comment(hid_t   loc_id, const char *  name, const char *  comment)
{
	herr_t ret;
	ret = __real_H5Gset_comment(loc_id, name, comment);
	return ret;
}

int __wrap_H5Gget_comment(hid_t   loc_id, const char *  name, size_t   bufsize, char *  buf)
{
	int ret;
	ret = __real_H5Gget_comment(loc_id, name, bufsize, buf);
	return ret;
}

herr_t __wrap_H5Giterate(hid_t   loc_id, const char *  name, int *  idx, H5G_iterate_t   op, void *  op_data)
{
	herr_t ret;
	ret = __real_H5Giterate(loc_id, name, idx, op, op_data);
	return ret;
}

herr_t __wrap_H5Gget_num_objs(hid_t   loc_id, hsize_t *  num_objs)
{
	herr_t ret;
	ret = __real_H5Gget_num_objs(loc_id, num_objs);
	return ret;
}

herr_t __wrap_H5Gget_objinfo(hid_t   loc_id, const char *  name, hbool_t   follow_link, H5G_stat_t *  statbuf)
{
	herr_t ret;
	ret = __real_H5Gget_objinfo(loc_id, name, follow_link, statbuf);
	return ret;
}

ssize_t __wrap_H5Gget_objname_by_idx(hid_t   loc_id, hsize_t   idx, char*   name, size_t   size)
{
	ssize_t ret;
	ret = __real_H5Gget_objname_by_idx(loc_id, idx, name, size);
	return ret;
}

H5G_obj_t __wrap_H5Gget_objtype_by_idx(hid_t   loc_id, hsize_t   idx)
{
	H5G_obj_t ret;
	ret = __real_H5Gget_objtype_by_idx(loc_id, idx);
	return ret;
}

herr_t __wrap_H5Zregister(const void *  cls)
{
	herr_t ret;
	ret = __real_H5Zregister(cls);
	return ret;
}

herr_t __wrap_H5Zunregister(H5Z_filter_t   id)
{
	herr_t ret;
	ret = __real_H5Zunregister(id);
	return ret;
}

htri_t __wrap_H5Zfilter_avail(H5Z_filter_t   id)
{
	htri_t ret;
	ret = __real_H5Zfilter_avail(id);
	return ret;
}

herr_t __wrap_H5Zget_filter_info(H5Z_filter_t   filter, unsigned int *  filter_config_flags)
{
	herr_t ret;
	ret = __real_H5Zget_filter_info(filter, filter_config_flags);
	return ret;
}

hid_t __wrap_H5Pcreate_class(hid_t   parent, const char *  name, H5P_cls_create_func_t   cls_create, void *  create_data, H5P_cls_copy_func_t   cls_copy, void *  copy_data, H5P_cls_close_func_t   cls_close, void *  close_data)
{
	hid_t ret;
	ret = __real_H5Pcreate_class(parent, name, cls_create, create_data, cls_copy, copy_data, cls_close, close_data);
	return ret;
}

char * __wrap_H5Pget_class_name(hid_t   pclass_id)
{
	char * ret;
	ret = __real_H5Pget_class_name(pclass_id);
	return ret;
}

hid_t __wrap_H5Pcreate(hid_t   cls_id)
{
	hid_t ret;
	ret = __real_H5Pcreate(cls_id);
	return ret;
}

herr_t __wrap_H5Pregister2(hid_t   cls_id, const char *  name, size_t   size, void *  def_value, H5P_prp_create_func_t   prp_create, H5P_prp_set_func_t   prp_set, H5P_prp_get_func_t   prp_get, H5P_prp_delete_func_t   prp_del, H5P_prp_copy_func_t   prp_copy, H5P_prp_compare_func_t   prp_cmp, H5P_prp_close_func_t   prp_close)
{
	herr_t ret;
	ret = __real_H5Pregister2(cls_id, name, size, def_value, prp_create, prp_set, prp_get, prp_del, prp_copy, prp_cmp, prp_close);
	return ret;
}

herr_t __wrap_H5Pinsert2(hid_t   plist_id, const char *  name, size_t   size, void *  value, H5P_prp_set_func_t   prp_set, H5P_prp_get_func_t   prp_get, H5P_prp_delete_func_t   prp_delete, H5P_prp_copy_func_t   prp_copy, H5P_prp_compare_func_t   prp_cmp, H5P_prp_close_func_t   prp_close)
{
	herr_t ret;
	ret = __real_H5Pinsert2(plist_id, name, size, value, prp_set, prp_get, prp_delete, prp_copy, prp_cmp, prp_close);
	return ret;
}

herr_t __wrap_H5Pset(hid_t   plist_id, const char *  name, void *  value)
{
	herr_t ret;
	ret = __real_H5Pset(plist_id, name, value);
	return ret;
}

htri_t __wrap_H5Pexist(hid_t   plist_id, const char *  name)
{
	htri_t ret;
	ret = __real_H5Pexist(plist_id, name);
	return ret;
}

herr_t __wrap_H5Pget_size(hid_t   id, const char *  name, size_t *  size)
{
	herr_t ret;
	ret = __real_H5Pget_size(id, name, size);
	return ret;
}

herr_t __wrap_H5Pget_nprops(hid_t   id, size_t *  nprops)
{
	herr_t ret;
	ret = __real_H5Pget_nprops(id, nprops);
	return ret;
}

hid_t __wrap_H5Pget_class(hid_t   plist_id)
{
	hid_t ret;
	ret = __real_H5Pget_class(plist_id);
	return ret;
}

hid_t __wrap_H5Pget_class_parent(hid_t   pclass_id)
{
	hid_t ret;
	ret = __real_H5Pget_class_parent(pclass_id);
	return ret;
}

herr_t __wrap_H5Pget(hid_t   plist_id, const char *  name, void *   value)
{
	herr_t ret;
	ret = __real_H5Pget(plist_id, name, value);
	return ret;
}

htri_t __wrap_H5Pequal(hid_t   id1, hid_t   id2)
{
	htri_t ret;
	ret = __real_H5Pequal(id1, id2);
	return ret;
}

htri_t __wrap_H5Pisa_class(hid_t   plist_id, hid_t   pclass_id)
{
	htri_t ret;
	ret = __real_H5Pisa_class(plist_id, pclass_id);
	return ret;
}

int __wrap_H5Piterate(hid_t   id, int *  idx, H5P_iterate_t   iter_func, void *  iter_data)
{
	int ret;
	ret = __real_H5Piterate(id, idx, iter_func, iter_data);
	return ret;
}

herr_t __wrap_H5Pcopy_prop(hid_t   dst_id, hid_t   src_id, const char *  name)
{
	herr_t ret;
	ret = __real_H5Pcopy_prop(dst_id, src_id, name);
	return ret;
}

herr_t __wrap_H5Premove(hid_t   plist_id, const char *  name)
{
	herr_t ret;
	ret = __real_H5Premove(plist_id, name);
	return ret;
}

herr_t __wrap_H5Punregister(hid_t   pclass_id, const char *  name)
{
	herr_t ret;
	ret = __real_H5Punregister(pclass_id, name);
	return ret;
}

herr_t __wrap_H5Pclose_class(hid_t   plist_id)
{
	herr_t ret;
	ret = __real_H5Pclose_class(plist_id);
	return ret;
}

herr_t __wrap_H5Pclose(hid_t   plist_id)
{
	herr_t ret;
	ret = __real_H5Pclose(plist_id);
	return ret;
}

hid_t __wrap_H5Pcopy(hid_t   plist_id)
{
	hid_t ret;
	ret = __real_H5Pcopy(plist_id);
	return ret;
}

herr_t __wrap_H5Pset_attr_phase_change(hid_t   plist_id, unsigned   max_compact, unsigned   min_dense)
{
	herr_t ret;
	ret = __real_H5Pset_attr_phase_change(plist_id, max_compact, min_dense);
	return ret;
}

herr_t __wrap_H5Pget_attr_phase_change(hid_t   plist_id, unsigned *  max_compact, unsigned *  min_dense)
{
	herr_t ret;
	ret = __real_H5Pget_attr_phase_change(plist_id, max_compact, min_dense);
	return ret;
}

herr_t __wrap_H5Pset_attr_creation_order(hid_t   plist_id, unsigned   crt_order_flags)
{
	herr_t ret;
	ret = __real_H5Pset_attr_creation_order(plist_id, crt_order_flags);
	return ret;
}

herr_t __wrap_H5Pget_attr_creation_order(hid_t   plist_id, unsigned *  crt_order_flags)
{
	herr_t ret;
	ret = __real_H5Pget_attr_creation_order(plist_id, crt_order_flags);
	return ret;
}

herr_t __wrap_H5Pset_obj_track_times(hid_t   plist_id, hbool_t   track_times)
{
	herr_t ret;
	ret = __real_H5Pset_obj_track_times(plist_id, track_times);
	return ret;
}

herr_t __wrap_H5Pget_obj_track_times(hid_t   plist_id, hbool_t *  track_times)
{
	herr_t ret;
	ret = __real_H5Pget_obj_track_times(plist_id, track_times);
	return ret;
}

herr_t __wrap_H5Pget_version(hid_t   plist_id, unsigned *  boot, unsigned *  freelist, unsigned *  stab, unsigned *  shhdr)
{
	herr_t ret;
	ret = __real_H5Pget_version(plist_id, boot, freelist, stab, shhdr);
	return ret;
}

herr_t __wrap_H5Pset_userblock(hid_t   plist_id, hsize_t   size)
{
	herr_t ret;
	ret = __real_H5Pset_userblock(plist_id, size);
	return ret;
}

herr_t __wrap_H5Pget_userblock(hid_t   plist_id, hsize_t *  size)
{
	herr_t ret;
	ret = __real_H5Pget_userblock(plist_id, size);
	return ret;
}

herr_t __wrap_H5Pset_sizes(hid_t   plist_id, size_t   sizeof_addr, size_t   sizeof_size)
{
	herr_t ret;
	ret = __real_H5Pset_sizes(plist_id, sizeof_addr, sizeof_size);
	return ret;
}

herr_t __wrap_H5Pget_sizes(hid_t   plist_id, size_t *  sizeof_addr, size_t *  sizeof_size)
{
	herr_t ret;
	ret = __real_H5Pget_sizes(plist_id, sizeof_addr, sizeof_size);
	return ret;
}

herr_t __wrap_H5Pset_sym_k(hid_t   plist_id, unsigned   ik, unsigned   lk)
{
	herr_t ret;
	ret = __real_H5Pset_sym_k(plist_id, ik, lk);
	return ret;
}

herr_t __wrap_H5Pget_sym_k(hid_t   plist_id, unsigned *  ik, unsigned *  lk)
{
	herr_t ret;
	ret = __real_H5Pget_sym_k(plist_id, ik, lk);
	return ret;
}

herr_t __wrap_H5Pset_istore_k(hid_t   plist_id, unsigned   ik)
{
	herr_t ret;
	ret = __real_H5Pset_istore_k(plist_id, ik);
	return ret;
}

herr_t __wrap_H5Pget_istore_k(hid_t   plist_id, unsigned *  ik)
{
	herr_t ret;
	ret = __real_H5Pget_istore_k(plist_id, ik);
	return ret;
}

herr_t __wrap_H5Pset_shared_mesg_nindexes(hid_t   plist_id, unsigned   nindexes)
{
	herr_t ret;
	ret = __real_H5Pset_shared_mesg_nindexes(plist_id, nindexes);
	return ret;
}

herr_t __wrap_H5Pget_shared_mesg_nindexes(hid_t   plist_id, unsigned *  nindexes)
{
	herr_t ret;
	ret = __real_H5Pget_shared_mesg_nindexes(plist_id, nindexes);
	return ret;
}

herr_t __wrap_H5Pset_shared_mesg_index(hid_t   plist_id, unsigned   index_num, unsigned   mesg_type_flags, unsigned   min_mesg_size)
{
	herr_t ret;
	ret = __real_H5Pset_shared_mesg_index(plist_id, index_num, mesg_type_flags, min_mesg_size);
	return ret;
}

herr_t __wrap_H5Pget_shared_mesg_index(hid_t   plist_id, unsigned   index_num, unsigned *  mesg_type_flags, unsigned *  min_mesg_size)
{
	herr_t ret;
	ret = __real_H5Pget_shared_mesg_index(plist_id, index_num, mesg_type_flags, min_mesg_size);
	return ret;
}

herr_t __wrap_H5Pset_shared_mesg_phase_change(hid_t   plist_id, unsigned   max_list, unsigned   min_btree)
{
	herr_t ret;
	ret = __real_H5Pset_shared_mesg_phase_change(plist_id, max_list, min_btree);
	return ret;
}

herr_t __wrap_H5Pget_shared_mesg_phase_change(hid_t   plist_id, unsigned *  max_list, unsigned *  min_btree)
{
	herr_t ret;
	ret = __real_H5Pget_shared_mesg_phase_change(plist_id, max_list, min_btree);
	return ret;
}

herr_t __wrap_H5Pset_alignment(hid_t   fapl_id, hsize_t   threshold, hsize_t   alignment)
{
	herr_t ret;
	ret = __real_H5Pset_alignment(fapl_id, threshold, alignment);
	return ret;
}

herr_t __wrap_H5Pget_alignment(hid_t   fapl_id, hsize_t *  threshold, hsize_t *  alignment)
{
	herr_t ret;
	ret = __real_H5Pget_alignment(fapl_id, threshold, alignment);
	return ret;
}

herr_t __wrap_H5Pset_driver(hid_t   plist_id, hid_t   driver_id, const void *  driver_info)
{
	herr_t ret;
	ret = __real_H5Pset_driver(plist_id, driver_id, driver_info);
	return ret;
}

hid_t __wrap_H5Pget_driver(hid_t   plist_id)
{
	hid_t ret;
	ret = __real_H5Pget_driver(plist_id);
	return ret;
}

void * __wrap_H5Pget_driver_info(hid_t   plist_id)
{
	void * ret;
	ret = __real_H5Pget_driver_info(plist_id);
	return ret;
}

herr_t __wrap_H5Pset_family_offset(hid_t   fapl_id, hsize_t   offset)
{
	herr_t ret;
	ret = __real_H5Pset_family_offset(fapl_id, offset);
	return ret;
}

herr_t __wrap_H5Pget_family_offset(hid_t   fapl_id, hsize_t *  offset)
{
	herr_t ret;
	ret = __real_H5Pget_family_offset(fapl_id, offset);
	return ret;
}

herr_t __wrap_H5Pset_multi_type(hid_t   fapl_id, H5FD_mem_t   type)
{
	herr_t ret;
	ret = __real_H5Pset_multi_type(fapl_id, type);
	return ret;
}

herr_t __wrap_H5Pget_multi_type(hid_t   fapl_id, H5FD_mem_t *  type)
{
	herr_t ret;
	ret = __real_H5Pget_multi_type(fapl_id, type);
	return ret;
}

herr_t __wrap_H5Pset_cache(hid_t   plist_id, int   mdc_nelmts, size_t   rdcc_nslots, size_t   rdcc_nbytes, double   rdcc_w0)
{
	herr_t ret;
	ret = __real_H5Pset_cache(plist_id, mdc_nelmts, rdcc_nslots, rdcc_nbytes, rdcc_w0);
	return ret;
}

herr_t __wrap_H5Pget_cache(hid_t   plist_id, int *  mdc_nelmts, size_t *  rdcc_nslots, size_t *  rdcc_nbytes, double *  rdcc_w0)
{
	herr_t ret;
	ret = __real_H5Pget_cache(plist_id, mdc_nelmts, rdcc_nslots, rdcc_nbytes, rdcc_w0);
	return ret;
}

herr_t __wrap_H5Pset_mdc_config(hid_t   plist_id, H5AC_cache_config_t *   config_ptr)
{
	herr_t ret;
	ret = __real_H5Pset_mdc_config(plist_id, config_ptr);
	return ret;
}

herr_t __wrap_H5Pget_mdc_config(hid_t   plist_id, H5AC_cache_config_t *   config_ptr)
{
	herr_t ret;
	ret = __real_H5Pget_mdc_config(plist_id, config_ptr);
	return ret;
}

herr_t __wrap_H5Pset_gc_references(hid_t   fapl_id, unsigned   gc_ref)
{
	herr_t ret;
	ret = __real_H5Pset_gc_references(fapl_id, gc_ref);
	return ret;
}

herr_t __wrap_H5Pget_gc_references(hid_t   fapl_id, unsigned *  gc_ref)
{
	herr_t ret;
	ret = __real_H5Pget_gc_references(fapl_id, gc_ref);
	return ret;
}

herr_t __wrap_H5Pset_fclose_degree(hid_t   fapl_id, H5F_close_degree_t   degree)
{
	herr_t ret;
	ret = __real_H5Pset_fclose_degree(fapl_id, degree);
	return ret;
}

herr_t __wrap_H5Pget_fclose_degree(hid_t   fapl_id, H5F_close_degree_t *  degree)
{
	herr_t ret;
	ret = __real_H5Pget_fclose_degree(fapl_id, degree);
	return ret;
}

herr_t __wrap_H5Pset_meta_block_size(hid_t   fapl_id, hsize_t   size)
{
	herr_t ret;
	ret = __real_H5Pset_meta_block_size(fapl_id, size);
	return ret;
}

herr_t __wrap_H5Pget_meta_block_size(hid_t   fapl_id, hsize_t *  size)
{
	herr_t ret;
	ret = __real_H5Pget_meta_block_size(fapl_id, size);
	return ret;
}

herr_t __wrap_H5Pset_sieve_buf_size(hid_t   fapl_id, size_t   size)
{
	herr_t ret;
	ret = __real_H5Pset_sieve_buf_size(fapl_id, size);
	return ret;
}

herr_t __wrap_H5Pget_sieve_buf_size(hid_t   fapl_id, size_t *  size)
{
	herr_t ret;
	ret = __real_H5Pget_sieve_buf_size(fapl_id, size);
	return ret;
}

herr_t __wrap_H5Pset_small_data_block_size(hid_t   fapl_id, hsize_t   size)
{
	herr_t ret;
	ret = __real_H5Pset_small_data_block_size(fapl_id, size);
	return ret;
}

herr_t __wrap_H5Pget_small_data_block_size(hid_t   fapl_id, hsize_t *  size)
{
	herr_t ret;
	ret = __real_H5Pget_small_data_block_size(fapl_id, size);
	return ret;
}

herr_t __wrap_H5Pset_libver_bounds(hid_t   plist_id, H5F_libver_t   low, H5F_libver_t   high)
{
	herr_t ret;
	ret = __real_H5Pset_libver_bounds(plist_id, low, high);
	return ret;
}

herr_t __wrap_H5Pget_libver_bounds(hid_t   plist_id, H5F_libver_t *  low, H5F_libver_t *  high)
{
	herr_t ret;
	ret = __real_H5Pget_libver_bounds(plist_id, low, high);
	return ret;
}

herr_t __wrap_H5Pset_layout(hid_t   plist_id, H5D_layout_t   layout)
{
	herr_t ret;
	ret = __real_H5Pset_layout(plist_id, layout);
	return ret;
}

H5D_layout_t __wrap_H5Pget_layout(hid_t   plist_id)
{
	H5D_layout_t ret;
	ret = __real_H5Pget_layout(plist_id);
	return ret;
}

herr_t __wrap_H5Pset_chunk(hid_t   plist_id, int   ndims, const hsize_t *  dim)
{
	herr_t ret;
	ret = __real_H5Pset_chunk(plist_id, ndims, dim);
	return ret;
}

int __wrap_H5Pget_chunk(hid_t   plist_id, int   max_ndims, hsize_t *  dim)
{
	int ret;
	ret = __real_H5Pget_chunk(plist_id, max_ndims, dim);
	return ret;
}

herr_t __wrap_H5Pset_external(hid_t   plist_id, const char *  name, off_t   offset, hsize_t   size)
{
	herr_t ret;
	ret = __real_H5Pset_external(plist_id, name, offset, size);
	return ret;
}

int __wrap_H5Pget_external_count(hid_t   plist_id)
{
	int ret;
	ret = __real_H5Pget_external_count(plist_id);
	return ret;
}

herr_t __wrap_H5Pget_external(hid_t   plist_id, unsigned   idx, size_t   name_size, char *  name, off_t *  offset, hsize_t *  size)
{
	herr_t ret;
	ret = __real_H5Pget_external(plist_id, idx, name_size, name, offset, size);
	return ret;
}

herr_t __wrap_H5Pmodify_filter(hid_t   plist_id, H5Z_filter_t   filter, unsigned int   flags, size_t   cd_nelmts, const unsigned int *  cd_values)
{
	herr_t ret;
	ret = __real_H5Pmodify_filter(plist_id, filter, flags, cd_nelmts, cd_values);
	return ret;
}

herr_t __wrap_H5Pset_filter(hid_t   plist_id, H5Z_filter_t   filter, unsigned int   flags, size_t   cd_nelmts, const unsigned int *  c_values)
{
	herr_t ret;
	ret = __real_H5Pset_filter(plist_id, filter, flags, cd_nelmts, c_values);
	return ret;
}

int __wrap_H5Pget_nfilters(hid_t   plist_id)
{
	int ret;
	ret = __real_H5Pget_nfilters(plist_id);
	return ret;
}

H5Z_filter_t __wrap_H5Pget_filter2(hid_t   plist_id, unsigned   filter, unsigned int *  flags, size_t *  cd_nelmts, unsigned *  cd_values, size_t   namelen, char *  name, unsigned *  filter_config)
{
	H5Z_filter_t ret;
	ret = __real_H5Pget_filter2(plist_id, filter, flags, cd_nelmts, cd_values, namelen, name, filter_config);
	return ret;
}

herr_t __wrap_H5Pget_filter_by_id2(hid_t   plist_id, H5Z_filter_t   id, unsigned int *  flags, size_t *  cd_nelmts, unsigned *  cd_values, size_t   namelen, char *  name, unsigned *  filter_config)
{
	herr_t ret;
	ret = __real_H5Pget_filter_by_id2(plist_id, id, flags, cd_nelmts, cd_values, namelen, name, filter_config);
	return ret;
}

htri_t __wrap_H5Pall_filters_avail(hid_t   plist_id)
{
	htri_t ret;
	ret = __real_H5Pall_filters_avail(plist_id);
	return ret;
}

herr_t __wrap_H5Premove_filter(hid_t   plist_id, H5Z_filter_t   filter)
{
	herr_t ret;
	ret = __real_H5Premove_filter(plist_id, filter);
	return ret;
}

herr_t __wrap_H5Pset_deflate(hid_t   plist_id, unsigned   aggression)
{
	herr_t ret;
	ret = __real_H5Pset_deflate(plist_id, aggression);
	return ret;
}

herr_t __wrap_H5Pset_szip(hid_t   plist_id, unsigned   options_mask, unsigned   pixels_per_block)
{
	herr_t ret;
	ret = __real_H5Pset_szip(plist_id, options_mask, pixels_per_block);
	return ret;
}

herr_t __wrap_H5Pset_shuffle(hid_t   plist_id)
{
	herr_t ret;
	ret = __real_H5Pset_shuffle(plist_id);
	return ret;
}

herr_t __wrap_H5Pset_nbit(hid_t   plist_id)
{
	herr_t ret;
	ret = __real_H5Pset_nbit(plist_id);
	return ret;
}

herr_t __wrap_H5Pset_scaleoffset(hid_t   plist_id, H5Z_SO_scale_type_t   scale_type, int   scale_factor)
{
	herr_t ret;
	ret = __real_H5Pset_scaleoffset(plist_id, scale_type, scale_factor);
	return ret;
}

herr_t __wrap_H5Pset_fletcher32(hid_t   plist_id)
{
	herr_t ret;
	ret = __real_H5Pset_fletcher32(plist_id);
	return ret;
}

herr_t __wrap_H5Pset_fill_value(hid_t   plist_id, hid_t   type_id, const void *  value)
{
	herr_t ret;
	ret = __real_H5Pset_fill_value(plist_id, type_id, value);
	return ret;
}

herr_t __wrap_H5Pget_fill_value(hid_t   plist_id, hid_t   type_id, void *  value)
{
	herr_t ret;
	ret = __real_H5Pget_fill_value(plist_id, type_id, value);
	return ret;
}

herr_t __wrap_H5Pfill_value_defined(hid_t   plist, H5D_fill_value_t *  status)
{
	herr_t ret;
	ret = __real_H5Pfill_value_defined(plist, status);
	return ret;
}

herr_t __wrap_H5Pset_alloc_time(hid_t   plist_id, H5D_alloc_time_t   alloc_time)
{
	herr_t ret;
	ret = __real_H5Pset_alloc_time(plist_id, alloc_time);
	return ret;
}

herr_t __wrap_H5Pget_alloc_time(hid_t   plist_id, H5D_alloc_time_t *  alloc_time)
{
	herr_t ret;
	ret = __real_H5Pget_alloc_time(plist_id, alloc_time);
	return ret;
}

herr_t __wrap_H5Pset_fill_time(hid_t   plist_id, H5D_fill_time_t   fill_time)
{
	herr_t ret;
	ret = __real_H5Pset_fill_time(plist_id, fill_time);
	return ret;
}

herr_t __wrap_H5Pget_fill_time(hid_t   plist_id, H5D_fill_time_t *  fill_time)
{
	herr_t ret;
	ret = __real_H5Pget_fill_time(plist_id, fill_time);
	return ret;
}

herr_t __wrap_H5Pset_chunk_cache(hid_t   dapl_id, size_t   rdcc_nslots, size_t   rdcc_nbytes, double   rdcc_w0)
{
	herr_t ret;
	ret = __real_H5Pset_chunk_cache(dapl_id, rdcc_nslots, rdcc_nbytes, rdcc_w0);
	return ret;
}

herr_t __wrap_H5Pget_chunk_cache(hid_t   dapl_id, size_t *  rdcc_nslots, size_t *  rdcc_nbytes, double *  rdcc_w0)
{
	herr_t ret;
	ret = __real_H5Pget_chunk_cache(dapl_id, rdcc_nslots, rdcc_nbytes, rdcc_w0);
	return ret;
}

herr_t __wrap_H5Pset_data_transform(hid_t   plist_id, const char*   expression)
{
	herr_t ret;
	ret = __real_H5Pset_data_transform(plist_id, expression);
	return ret;
}

ssize_t __wrap_H5Pget_data_transform(hid_t   plist_id, char*   expression, size_t   size)
{
	ssize_t ret;
	ret = __real_H5Pget_data_transform(plist_id, expression, size);
	return ret;
}

herr_t __wrap_H5Pset_buffer(hid_t   plist_id, size_t   size, void *  tconv, void *  bkg)
{
	herr_t ret;
	ret = __real_H5Pset_buffer(plist_id, size, tconv, bkg);
	return ret;
}

size_t __wrap_H5Pget_buffer(hid_t   plist_id, void **  tconv, void **  bkg)
{
	size_t ret;
	ret = __real_H5Pget_buffer(plist_id, tconv, bkg);
	return ret;
}

herr_t __wrap_H5Pset_preserve(hid_t   plist_id, hbool_t   status)
{
	herr_t ret;
	ret = __real_H5Pset_preserve(plist_id, status);
	return ret;
}

int __wrap_H5Pget_preserve(hid_t   plist_id)
{
	int ret;
	ret = __real_H5Pget_preserve(plist_id);
	return ret;
}

herr_t __wrap_H5Pset_edc_check(hid_t   plist_id, H5Z_EDC_t   check)
{
	herr_t ret;
	ret = __real_H5Pset_edc_check(plist_id, check);
	return ret;
}

H5Z_EDC_t __wrap_H5Pget_edc_check(hid_t   plist_id)
{
	H5Z_EDC_t ret;
	ret = __real_H5Pget_edc_check(plist_id);
	return ret;
}

herr_t __wrap_H5Pset_filter_callback(hid_t   plist_id, H5Z_filter_func_t   func, void*   op_data)
{
	herr_t ret;
	ret = __real_H5Pset_filter_callback(plist_id, func, op_data);
	return ret;
}

herr_t __wrap_H5Pset_btree_ratios(hid_t   plist_id, double   left, double   middle, double   right)
{
	herr_t ret;
	ret = __real_H5Pset_btree_ratios(plist_id, left, middle, right);
	return ret;
}

herr_t __wrap_H5Pget_btree_ratios(hid_t   plist_id, double *  left, double *  middle, double *  right)
{
	herr_t ret;
	ret = __real_H5Pget_btree_ratios(plist_id, left, middle, right);
	return ret;
}

herr_t __wrap_H5Pset_vlen_mem_manager(hid_t   plist_id, H5MM_allocate_t   alloc_func, void *  alloc_info, H5MM_free_t   free_func, void *  free_info)
{
	herr_t ret;
	ret = __real_H5Pset_vlen_mem_manager(plist_id, alloc_func, alloc_info, free_func, free_info);
	return ret;
}

herr_t __wrap_H5Pget_vlen_mem_manager(hid_t   plist_id, H5MM_allocate_t *  alloc_func, void **  alloc_info, H5MM_free_t *  free_func, void **  free_info)
{
	herr_t ret;
	ret = __real_H5Pget_vlen_mem_manager(plist_id, alloc_func, alloc_info, free_func, free_info);
	return ret;
}

herr_t __wrap_H5Pset_hyper_vector_size(hid_t   fapl_id, size_t   size)
{
	herr_t ret;
	ret = __real_H5Pset_hyper_vector_size(fapl_id, size);
	return ret;
}

herr_t __wrap_H5Pget_hyper_vector_size(hid_t   fapl_id, size_t *  size)
{
	herr_t ret;
	ret = __real_H5Pget_hyper_vector_size(fapl_id, size);
	return ret;
}

herr_t __wrap_H5Pset_type_conv_cb(hid_t   dxpl_id, H5T_conv_except_func_t   op, void*   operate_data)
{
	herr_t ret;
	ret = __real_H5Pset_type_conv_cb(dxpl_id, op, operate_data);
	return ret;
}

herr_t __wrap_H5Pget_type_conv_cb(hid_t   dxpl_id, H5T_conv_except_func_t *  op, void**   operate_data)
{
	herr_t ret;
	ret = __real_H5Pget_type_conv_cb(dxpl_id, op, operate_data);
	return ret;
}

herr_t __wrap_H5Pset_create_intermediate_group(hid_t   plist_id, unsigned   crt_intmd)
{
	herr_t ret;
	ret = __real_H5Pset_create_intermediate_group(plist_id, crt_intmd);
	return ret;
}

herr_t __wrap_H5Pget_create_intermediate_group(hid_t   plist_id, unsigned *  crt_intmd)
{
	herr_t ret;
	ret = __real_H5Pget_create_intermediate_group(plist_id, crt_intmd);
	return ret;
}

herr_t __wrap_H5Pset_local_heap_size_hint(hid_t   plist_id, size_t   size_hint)
{
	herr_t ret;
	ret = __real_H5Pset_local_heap_size_hint(plist_id, size_hint);
	return ret;
}

herr_t __wrap_H5Pget_local_heap_size_hint(hid_t   plist_id, size_t *  size_hint)
{
	herr_t ret;
	ret = __real_H5Pget_local_heap_size_hint(plist_id, size_hint);
	return ret;
}

herr_t __wrap_H5Pset_link_phase_change(hid_t   plist_id, unsigned   max_compact, unsigned   min_dense)
{
	herr_t ret;
	ret = __real_H5Pset_link_phase_change(plist_id, max_compact, min_dense);
	return ret;
}

herr_t __wrap_H5Pget_link_phase_change(hid_t   plist_id, unsigned *  max_compact, unsigned *  min_dense)
{
	herr_t ret;
	ret = __real_H5Pget_link_phase_change(plist_id, max_compact, min_dense);
	return ret;
}

herr_t __wrap_H5Pset_est_link_info(hid_t   plist_id, unsigned   est_num_entries, unsigned   est_name_len)
{
	herr_t ret;
	ret = __real_H5Pset_est_link_info(plist_id, est_num_entries, est_name_len);
	return ret;
}

herr_t __wrap_H5Pget_est_link_info(hid_t   plist_id, unsigned *  est_num_entries, unsigned *  est_name_len)
{
	herr_t ret;
	ret = __real_H5Pget_est_link_info(plist_id, est_num_entries, est_name_len);
	return ret;
}

herr_t __wrap_H5Pset_link_creation_order(hid_t   plist_id, unsigned   crt_order_flags)
{
	herr_t ret;
	ret = __real_H5Pset_link_creation_order(plist_id, crt_order_flags);
	return ret;
}

herr_t __wrap_H5Pget_link_creation_order(hid_t   plist_id, unsigned *  crt_order_flags)
{
	herr_t ret;
	ret = __real_H5Pget_link_creation_order(plist_id, crt_order_flags);
	return ret;
}

herr_t __wrap_H5Pset_char_encoding(hid_t   plist_id, H5T_cset_t   encoding)
{
	herr_t ret;
	ret = __real_H5Pset_char_encoding(plist_id, encoding);
	return ret;
}

herr_t __wrap_H5Pget_char_encoding(hid_t   plist_id, H5T_cset_t *  encoding)
{
	herr_t ret;
	ret = __real_H5Pget_char_encoding(plist_id, encoding);
	return ret;
}

herr_t __wrap_H5Pset_nlinks(hid_t   plist_id, size_t   nlinks)
{
	herr_t ret;
	ret = __real_H5Pset_nlinks(plist_id, nlinks);
	return ret;
}

herr_t __wrap_H5Pget_nlinks(hid_t   plist_id, size_t *  nlinks)
{
	herr_t ret;
	ret = __real_H5Pget_nlinks(plist_id, nlinks);
	return ret;
}

herr_t __wrap_H5Pset_elink_prefix(hid_t   plist_id, const char *  prefix)
{
	herr_t ret;
	ret = __real_H5Pset_elink_prefix(plist_id, prefix);
	return ret;
}

ssize_t __wrap_H5Pget_elink_prefix(hid_t   plist_id, char *  prefix, size_t   size)
{
	ssize_t ret;
	ret = __real_H5Pget_elink_prefix(plist_id, prefix, size);
	return ret;
}

hid_t __wrap_H5Pget_elink_fapl(hid_t   lapl_id)
{
	hid_t ret;
	ret = __real_H5Pget_elink_fapl(lapl_id);
	return ret;
}

herr_t __wrap_H5Pset_elink_fapl(hid_t   lapl_id, hid_t   fapl_id)
{
	herr_t ret;
	ret = __real_H5Pset_elink_fapl(lapl_id, fapl_id);
	return ret;
}

herr_t __wrap_H5Pset_elink_acc_flags(hid_t   lapl_id, unsigned   flags)
{
	herr_t ret;
	ret = __real_H5Pset_elink_acc_flags(lapl_id, flags);
	return ret;
}

herr_t __wrap_H5Pget_elink_acc_flags(hid_t   lapl_id, unsigned *  flags)
{
	herr_t ret;
	ret = __real_H5Pget_elink_acc_flags(lapl_id, flags);
	return ret;
}

herr_t __wrap_H5Pset_elink_cb(hid_t   lapl_id, H5L_elink_traverse_t   func, void *  op_data)
{
	herr_t ret;
	ret = __real_H5Pset_elink_cb(lapl_id, func, op_data);
	return ret;
}

herr_t __wrap_H5Pget_elink_cb(hid_t   lapl_id, H5L_elink_traverse_t *  func, void **  op_data)
{
	herr_t ret;
	ret = __real_H5Pget_elink_cb(lapl_id, func, op_data);
	return ret;
}

herr_t __wrap_H5Pset_copy_object(hid_t   plist_id, unsigned   crt_intmd)
{
	herr_t ret;
	ret = __real_H5Pset_copy_object(plist_id, crt_intmd);
	return ret;
}

herr_t __wrap_H5Pget_copy_object(hid_t   plist_id, unsigned *  crt_intmd)
{
	herr_t ret;
	ret = __real_H5Pget_copy_object(plist_id, crt_intmd);
	return ret;
}

herr_t __wrap_H5Pregister1(hid_t   cls_id, const char *  name, size_t   size, void *  def_value, H5P_prp_create_func_t   prp_create, H5P_prp_set_func_t   prp_set, H5P_prp_get_func_t   prp_get, H5P_prp_delete_func_t   prp_del, H5P_prp_copy_func_t   prp_copy, H5P_prp_close_func_t   prp_close)
{
	herr_t ret;
	ret = __real_H5Pregister1(cls_id, name, size, def_value, prp_create, prp_set, prp_get, prp_del, prp_copy, prp_close);
	return ret;
}

herr_t __wrap_H5Pinsert1(hid_t   plist_id, const char *  name, size_t   size, void *  value, H5P_prp_set_func_t   prp_set, H5P_prp_get_func_t   prp_get, H5P_prp_delete_func_t   prp_delete, H5P_prp_copy_func_t   prp_copy, H5P_prp_close_func_t   prp_close)
{
	herr_t ret;
	ret = __real_H5Pinsert1(plist_id, name, size, value, prp_set, prp_get, prp_delete, prp_copy, prp_close);
	return ret;
}

H5Z_filter_t __wrap_H5Pget_filter1(hid_t   plist_id, unsigned   filter, unsigned int *  flags, size_t *  cd_nelmts, unsigned *  cd_values, size_t   namelen, char *  name)
{
	H5Z_filter_t ret;
	ret = __real_H5Pget_filter1(plist_id, filter, flags, cd_nelmts, cd_values, namelen, name);
	return ret;
}

herr_t __wrap_H5Pget_filter_by_id1(hid_t   plist_id, H5Z_filter_t   id, unsigned int *  flags, size_t *  cd_nelmts, unsigned *  cd_values, size_t   namelen, char *  name)
{
	herr_t ret;
	ret = __real_H5Pget_filter_by_id1(plist_id, id, flags, cd_nelmts, cd_values, namelen, name);
	return ret;
}

herr_t __wrap_H5Rcreate(void *  ref, hid_t   loc_id, const char *  name, H5R_type_t   ref_type, hid_t   space_id)
{
	herr_t ret;
	ret = __real_H5Rcreate(ref, loc_id, name, ref_type, space_id);
	return ret;
}

hid_t __wrap_H5Rdereference(hid_t   dataset, H5R_type_t   ref_type, const void *  ref)
{
	hid_t ret;
	ret = __real_H5Rdereference(dataset, ref_type, ref);
	return ret;
}

hid_t __wrap_H5Rget_region(hid_t   dataset, H5R_type_t   ref_type, const void *  ref)
{
	hid_t ret;
	ret = __real_H5Rget_region(dataset, ref_type, ref);
	return ret;
}

herr_t __wrap_H5Rget_obj_type2(hid_t   id, H5R_type_t   ref_type, const void *  _ref, H5O_type_t *  obj_type)
{
	herr_t ret;
	ret = __real_H5Rget_obj_type2(id, ref_type, _ref, obj_type);
	return ret;
}

ssize_t __wrap_H5Rget_name(hid_t   loc_id, H5R_type_t   ref_type, const void *  ref, char *  name, size_t   size)
{
	ssize_t ret;
	ret = __real_H5Rget_name(loc_id, ref_type, ref, name, size);
	return ret;
}

H5G_obj_t __wrap_H5Rget_obj_type1(hid_t   id, H5R_type_t   ref_type, const void *  _ref)
{
	H5G_obj_t ret;
	ret = __real_H5Rget_obj_type1(id, ref_type, _ref);
	return ret;
}

hid_t __wrap_H5Screate(H5S_class_t   type)
{
	hid_t ret;
	ret = __real_H5Screate(type);
	return ret;
}

hid_t __wrap_H5Screate_simple(int   rank, const hsize_t *  dims, const hsize_t *  maxdims)
{
	hid_t ret;
	ret = __real_H5Screate_simple(rank, dims, maxdims);
	return ret;
}

herr_t __wrap_H5Sset_extent_simple(hid_t   space_id, int   rank, const hsize_t *  dims, const hsize_t *  max)
{
	herr_t ret;
	ret = __real_H5Sset_extent_simple(space_id, rank, dims, max);
	return ret;
}

hid_t __wrap_H5Scopy(hid_t   space_id)
{
	hid_t ret;
	ret = __real_H5Scopy(space_id);
	return ret;
}

herr_t __wrap_H5Sclose(hid_t   space_id)
{
	herr_t ret;
	ret = __real_H5Sclose(space_id);
	return ret;
}

herr_t __wrap_H5Sencode(hid_t   obj_id, void *  buf, size_t *  nalloc)
{
	herr_t ret;
	ret = __real_H5Sencode(obj_id, buf, nalloc);
	return ret;
}

hid_t __wrap_H5Sdecode(const void *  buf)
{
	hid_t ret;
	ret = __real_H5Sdecode(buf);
	return ret;
}

hssize_t __wrap_H5Sget_simple_extent_npoints(hid_t   space_id)
{
	hssize_t ret;
	ret = __real_H5Sget_simple_extent_npoints(space_id);
	return ret;
}

int __wrap_H5Sget_simple_extent_ndims(hid_t   space_id)
{
	int ret;
	ret = __real_H5Sget_simple_extent_ndims(space_id);
	return ret;
}

int __wrap_H5Sget_simple_extent_dims(hid_t   space_id, hsize_t *  dims, hsize_t *  maxdims)
{
	int ret;
	ret = __real_H5Sget_simple_extent_dims(space_id, dims, maxdims);
	return ret;
}

htri_t __wrap_H5Sis_simple(hid_t   space_id)
{
	htri_t ret;
	ret = __real_H5Sis_simple(space_id);
	return ret;
}

hssize_t __wrap_H5Sget_select_npoints(hid_t   spaceid)
{
	hssize_t ret;
	ret = __real_H5Sget_select_npoints(spaceid);
	return ret;
}

herr_t __wrap_H5Sselect_hyperslab(hid_t   space_id, H5S_seloper_t   op, const hsize_t *  start, const hsize_t *  _stride, const hsize_t *  count, const hsize_t *  _block)
{
	herr_t ret;
	ret = __real_H5Sselect_hyperslab(space_id, op, start, _stride, count, _block);
	return ret;
}

herr_t __wrap_H5Sselect_elements(hid_t   space_id, H5S_seloper_t   op, size_t   num_elem, const hsize_t *  coord)
{
	herr_t ret;
	ret = __real_H5Sselect_elements(space_id, op, num_elem, coord);
	return ret;
}

H5S_class_t __wrap_H5Sget_simple_extent_type(hid_t   space_id)
{
	H5S_class_t ret;
	ret = __real_H5Sget_simple_extent_type(space_id);
	return ret;
}

herr_t __wrap_H5Sset_extent_none(hid_t   space_id)
{
	herr_t ret;
	ret = __real_H5Sset_extent_none(space_id);
	return ret;
}

herr_t __wrap_H5Sextent_copy(hid_t   dst_id, hid_t   src_id)
{
	herr_t ret;
	ret = __real_H5Sextent_copy(dst_id, src_id);
	return ret;
}

htri_t __wrap_H5Sextent_equal(hid_t   sid1, hid_t   sid2)
{
	htri_t ret;
	ret = __real_H5Sextent_equal(sid1, sid2);
	return ret;
}

herr_t __wrap_H5Sselect_all(hid_t   spaceid)
{
	herr_t ret;
	ret = __real_H5Sselect_all(spaceid);
	return ret;
}

herr_t __wrap_H5Sselect_none(hid_t   spaceid)
{
	herr_t ret;
	ret = __real_H5Sselect_none(spaceid);
	return ret;
}

herr_t __wrap_H5Soffset_simple(hid_t   space_id, const hssize_t *  offset)
{
	herr_t ret;
	ret = __real_H5Soffset_simple(space_id, offset);
	return ret;
}

htri_t __wrap_H5Sselect_valid(hid_t   spaceid)
{
	htri_t ret;
	ret = __real_H5Sselect_valid(spaceid);
	return ret;
}

hssize_t __wrap_H5Sget_select_hyper_nblocks(hid_t   spaceid)
{
	hssize_t ret;
	ret = __real_H5Sget_select_hyper_nblocks(spaceid);
	return ret;
}

hssize_t __wrap_H5Sget_select_elem_npoints(hid_t   spaceid)
{
	hssize_t ret;
	ret = __real_H5Sget_select_elem_npoints(spaceid);
	return ret;
}

herr_t __wrap_H5Sget_select_hyper_blocklist(hid_t   spaceid, hsize_t   startblock, hsize_t   numblocks, hsize_t *  buf)
{
	herr_t ret;
	ret = __real_H5Sget_select_hyper_blocklist(spaceid, startblock, numblocks, buf);
	return ret;
}

herr_t __wrap_H5Sget_select_elem_pointlist(hid_t   spaceid, hsize_t   startpoint, hsize_t   numpoints, hsize_t *  buf)
{
	herr_t ret;
	ret = __real_H5Sget_select_elem_pointlist(spaceid, startpoint, numpoints, buf);
	return ret;
}

herr_t __wrap_H5Sget_select_bounds(hid_t   spaceid, hsize_t *  start, hsize_t *  end)
{
	herr_t ret;
	ret = __real_H5Sget_select_bounds(spaceid, start, end);
	return ret;
}

H5S_sel_type __wrap_H5Sget_select_type(hid_t   spaceid)
{
	H5S_sel_type ret;
	ret = __real_H5Sget_select_type(spaceid);
	return ret;
}

hid_t __wrap_H5FD_core_init(void  )
{
	hid_t ret;
	ret = __real_H5FD_core_init();
	return ret;
}

void __wrap_H5FD_core_term(void  )
{
	__real_H5FD_core_term();

}

herr_t __wrap_H5Pset_fapl_core(hid_t   fapl_id, size_t   increment, hbool_t   backing_store)
{
	herr_t ret;
	ret = __real_H5Pset_fapl_core(fapl_id, increment, backing_store);
	return ret;
}

herr_t __wrap_H5Pget_fapl_core(hid_t   fapl_id, size_t *  increment, hbool_t *  backing_store)
{
	herr_t ret;
	ret = __real_H5Pget_fapl_core(fapl_id, increment, backing_store);
	return ret;
}

hid_t __wrap_H5FD_family_init(void  )
{
	hid_t ret;
	ret = __real_H5FD_family_init();
	return ret;
}

void __wrap_H5FD_family_term(void  )
{
	__real_H5FD_family_term();

}

herr_t __wrap_H5Pset_fapl_family(hid_t   fapl_id, hsize_t   memb_size, hid_t   memb_fapl_id)
{
	herr_t ret;
	ret = __real_H5Pset_fapl_family(fapl_id, memb_size, memb_fapl_id);
	return ret;
}

herr_t __wrap_H5Pget_fapl_family(hid_t   fapl_id, hsize_t *  memb_size, hid_t *  memb_fapl_id)
{
	herr_t ret;
	ret = __real_H5Pget_fapl_family(fapl_id, memb_size, memb_fapl_id);
	return ret;
}

hid_t __wrap_H5FD_log_init(void  )
{
	hid_t ret;
	ret = __real_H5FD_log_init();
	return ret;
}

void __wrap_H5FD_log_term(void  )
{
	__real_H5FD_log_term();

}

herr_t __wrap_H5Pset_fapl_log(hid_t   fapl_id, const char *  logfile, unsigned   flags, size_t   buf_size)
{
	herr_t ret;
	ret = __real_H5Pset_fapl_log(fapl_id, logfile, flags, buf_size);
	return ret;
}

hid_t __wrap_H5FD_multi_init(void  )
{
	hid_t ret;
	ret = __real_H5FD_multi_init();
	return ret;
}

void __wrap_H5FD_multi_term(void  )
{
	__real_H5FD_multi_term();

}

herr_t __wrap_H5Pset_fapl_multi(hid_t   fapl_id, const H5FD_mem_t *  memb_map, const hid_t *  memb_fapl, const char * const *  memb_name, const haddr_t *  memb_addr, hbool_t   relax)
{
	herr_t ret;
	ret = __real_H5Pset_fapl_multi(fapl_id, memb_map, memb_fapl, memb_name, memb_addr, relax);
	return ret;
}

herr_t __wrap_H5Pget_fapl_multi(hid_t   fapl_id, H5FD_mem_t *  memb_map, hid_t *  memb_fapl, char **  memb_name, haddr_t *  memb_addr, hbool_t *  relax)
{
	herr_t ret;
	ret = __real_H5Pget_fapl_multi(fapl_id, memb_map, memb_fapl, memb_name, memb_addr, relax);
	return ret;
}

herr_t __wrap_H5Pset_dxpl_multi(hid_t   dxpl_id, const hid_t *  memb_dxpl)
{
	herr_t ret;
	ret = __real_H5Pset_dxpl_multi(dxpl_id, memb_dxpl);
	return ret;
}

herr_t __wrap_H5Pget_dxpl_multi(hid_t   dxpl_id, hid_t *  memb_dxpl)
{
	herr_t ret;
	ret = __real_H5Pget_dxpl_multi(dxpl_id, memb_dxpl);
	return ret;
}

herr_t __wrap_H5Pset_fapl_split(hid_t   fapl, const char *  meta_ext, hid_t   meta_plist_id, const char *  raw_ext, hid_t   raw_plist_id)
{
	herr_t ret;
	ret = __real_H5Pset_fapl_split(fapl, meta_ext, meta_plist_id, raw_ext, raw_plist_id);
	return ret;
}

hid_t __wrap_H5FD_sec2_init(void  )
{
	hid_t ret;
	ret = __real_H5FD_sec2_init();
	return ret;
}

void __wrap_H5FD_sec2_term(void  )
{
	__real_H5FD_sec2_term();

}

herr_t __wrap_H5Pset_fapl_sec2(hid_t   fapl_id)
{
	herr_t ret;
	ret = __real_H5Pset_fapl_sec2(fapl_id);
	return ret;
}

hid_t __wrap_H5FD_stdio_init(void  )
{
	hid_t ret;
	ret = __real_H5FD_stdio_init();
	return ret;
}

void __wrap_H5FD_stdio_term(void  )
{
	__real_H5FD_stdio_term();

}

herr_t __wrap_H5Pset_fapl_stdio(hid_t   fapl_id)
{
	herr_t ret;
	ret = __real_H5Pset_fapl_stdio(fapl_id);
	return ret;
}

