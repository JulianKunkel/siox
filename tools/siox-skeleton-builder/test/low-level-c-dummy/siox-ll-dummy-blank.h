//@print_fname
siox_node * lookup_node_id( const char * hostname );
//@print_fname
void siox_process_set_attribute( siox_attribute * attribute, void * value );
//@print_fname
siox_attribute * siox_ontology_register_attribute( const char * domain, const char * name, enum siox_ont_storage_type storage_type );
//@print_fname
siox_attribute * siox_ontology_register_attribute_with_unit( const char * domain, const char * name, const char * unit, enum siox_ont_storage_type storage_type );
//@print_fname
int siox_ontology_set_meta_attribute( siox_attribute * parent, siox_attribute * meta_attribute, void * value );
//@print_fname
siox_attribute * siox_ontology_lookup_attribute_by_name( const char * domain, const char * name );
//@print_fname
siox_unique_interface * siox_system_information_lookup_interface_id( const char * interface_name, const char * implementation_identifier );
//@print_fname
siox_associate * siox_associate_instance( const char * instance_information );
//@print_fname
siox_component * siox_component_register( siox_unique_interface * uiid, const char * instance_name );
//@print_fname
void siox_component_set_attribute( siox_component * component, siox_attribute * attribute, void * value );
//@print_fname
siox_component_activity * siox_component_register_activity( siox_unique_interface * uiid, const char * activity_name );
//@print_fname
void siox_component_unregister( siox_component * component );
//@print_fname
siox_activity * siox_activity_start( siox_component_activity * activity );
//@print_fname
void siox_activity_stop( siox_activity * activity );
//@print_fname
void siox_activity_set_attribute( siox_activity * activity, siox_attribute * attribute, void * value );
//@print_fname
void siox_activity_report_error( siox_activity * activity, siox_activity_error error );
//@print_fname
void siox_activity_end( siox_activity * activity );
//@print_fname
void siox_activity_link_to_parent( siox_activity * activity_child, siox_activity * activity_parent );
//@print_fname
siox_remote_call * siox_remote_call_setup( siox_activity * activity, siox_node * target_siox_node, siox_unique_interface * target_uid, siox_associate * target_iid );
//@print_fname
void siox_remote_call_set_attribute( siox_remote_call * remote_call, siox_attribute * attribute, void * value );
//@print_fname
void siox_remote_call_start( siox_remote_call * remote_call );
//@print_fname
siox_activity * activity siox_activity_start_from_remote_call( siox_component * component, siox_node * caller_siox_node_if_known, siox_unique_interface * caller_uid_if_known, siox_associate * caller_instance_if_known );
