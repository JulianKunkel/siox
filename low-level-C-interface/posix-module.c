Folgende Bereiche:
Globale Variablen (global)
Initialisierungsfunktion (init)
Finalisierung (final)
Wrapper für jede Funktion (wrapper) (before oder after)

//interface NAME
global:
    SWID = NAME;
    /// HWID und TID beschaffen und node registrieren

//ontology VARNAME STORAGETYPE
global:
    siox_dtid dtidVARNAME;
init:
    dtidVARNAME = siox_register_datatype( VARNAME, STORAGETYPE );

//activity
wrapper (before):
    siox_aid _aidCALL = siox_start_activity( unid, <CALL> );
wrapper (after):
    siox_stop_activity(_aidCALL);
final:
    siox_end_activity(_aidCALL);

//cause DESCRIPTOR
global:
init:
    /// DESCRIPTOR merken



//descriptorMap <FROM_TYPE> <TOTYPE> <FROM_VARNAME> <TO_VARNAME>
init:
    siox_dtid dtid_FROM_TYPE = siox_register_datatype( <FROM_TYPE>,  typeof(FROM_VARNAME) );
    siox_dtid dtid_TOTYPE = siox_register_datatype( <TO_TYPE>,  typeof(TO_VARNAME) );

    siox_dmid dmidFROM_TYPETO_TYPE = siox_register_descriptor_map( unid, dtid_FROM_TYPE, dtid_TO_TYPE );

wrapper (after):
    siox_map_descriptor(unid, dmidFROM_TYPETOTYPE, FROM_VARNAME, TO_VARNAME);

//releaseDescriptor TYPE VARNAME
wrapper (after):
    siox_release_descriptor(unid, dtid_TYPE, VARNAME);


//report TYPE VARNAME
init:


wrapper (after):
         siox_report_activity(_aid, dtid_TYPE, VARNAME, "")


//splice PROGRAMMCODE
PROGRAMMCODE
/// An dieser Stelle einfügen!

//======================================================================
// Beispiele der fertigen Wrapper für open und write:
//======================================================================

// global:

// init:
init()
{
    // Von open()
    siox_dtid dtid_filename = siox_register_datatype( filename,  typeof(pathname) );
    siox_dtid dtid_filehandle = siox_register_datatype( filehandle,  typeof(function_return_value) );

    siox_dmid dmidFROM_TYPEfunction_return_value = siox_register_descriptor_map( unid, dtid_filename, dtid_filehandle );

    // Von write()

}

int wrap_open(){
//cause pathname
//descriptorMap filename filehandle pathname function_return_value


    // before:
    filehandle function_return_value;
    siox_aid _aidOPEN = siox_start_activity( unid, <CALL> );

    // call:
    function_return_value = true_open( pathname, flags, mode );

    // after:
    siox_stop_activity(_aidOPEN);
    siox_map_descriptor(unid, dmid_filename_filehandle, pathname, function_return_value);
}

final()
{
    // Von open()
    siox_end_activity(_aidCALL);

    // von write()
}
