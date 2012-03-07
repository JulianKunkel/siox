Folgende Bereiche:
Globale Variablen (global)
Initialisierungsfunktion (init)
Finalisierung (final)
Wrapper für jede Funktion (wrapper) (before oder after)

//interface NAME
global: 
 
//ontology VARNAME DESCRIPTION STORAGETYPE

//activity
wrapper (after):
 siox_aid _aid = siox_start_activity( unid, <CALL> );
 <CALL>();
 siox_end_activity(_aid)

//cause DESCRIPTOR


//descriptorMap <FROM_TYPE> <TOTYPE> <FROM_VARNAME> <TO_VARNAME> 
init:
    siox_dtid dtid_FROM_TYPE = siox_register_datatype( <FROM_TYPE>,  typeof(FROM_VARNAME) );
    siox_dtid dtid_TOTYPE = siox_register_datatype( <TOTPYE>,  typeof(TO_VARNAME) );

    siox_dmid dmidFROM_TYPETOTYPE = siox_register_descriptor_map( unid, dtid_FROM_TYPE, dtid_TOTYPE );

wrapper (after):
    siox_map_descriptor(unid, dmidFROM_TYPETOTYPE, FROM_VARNAME, TO_VARNAME);

//releaseDescriptor TYPE VARNAME
wrapper (after):
	siox_release_descriptor(unid, dtid_TYPE, VARNAME);


//report TYPE VARNAME
init:
	

wrapper (after):
         siox_report_activity()



//Beispiel für open und write:

int wrap_open(){
}


