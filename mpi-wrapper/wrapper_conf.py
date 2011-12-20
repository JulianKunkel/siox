## -*- coding: utf-8 -*-


  # write can actually invoke a nested write from the tracing library, this must be prevented
before = {
  "header" :  "#include \"siox-ll.h\"\n siox_unid s_unid;"
	 }

attributes = {
		"_init" : ["""
						SIOX_register_node( %(name)s );
						SIOX_register_discriptor_map(s_unid, "FileName",
						"FileHandle");
						""", 
						"""

						"""]
  "_open" : ["""
						SIOX_recive_desriptor(s_unid, "FileName", name);
						""", 
						"""
						SIOX_map_descriptor( s_unid, "FileName", name,
						"FileHandle", returnValue );
						"""]
  "_read" : ["""
						siox_aid s_aid = SIOX_start_activity(s_unid);
						""", 
						"""
						SIOX_end_activity(s_aid)
						"""],
  "_write" : ["""
						siox_aid s_aid = SIOX_start_activity(s_unid);
						""", 
						"""
						SIOX_end_activity(s_aid);
						"""],
  "_close" : ["""

						""",
						"""
						SIOX_unregister_node(s_unid);
						"""]
}

