from optparse import OptionParser
import xml.etree.ElementTree as ET
import sys 

DEFAULT_INPUT = "input"
DEFAULT_OUTPUT = "out.xml"

def my_print(string, verbose):
    if verbose:
        print string

def check_for_required_attr(in_list, required_list, func):
    for item in required_list:
	if item not in in_list:
	    print "Warning: required attribut", item, "is missing in function", func

def check_for_notdef_attr(in_list, def_list, func):
    for key in in_list:
        if key not in def_list:
                print "Warning: attribute", key, "is not defined in dios.dtd as attribute for element", func

def get_attrib(func, info):
    infodict = {}
    attributlist = info.strip().split(", ")
    for k in attributlist:
        key_value = k.strip().split(": ")
        if len(key_value) > 2:
            print "ERROR: can not parse input!", k, "must have at most one ':', but there are several! Exiting."
            sys.exit()
        elif len(key_value) == 1:
            infodict["unknown"] = key_value[0]
            print "Warning:", k, "is not a key-value object. Can not find delimiter ':' in this string."
        elif len(key_value) == 2:
            infodict[key_value[0].strip()] = key_value[1].strip()
        else:
            pass
    for key in infodict.iterkeys():
	if infodict[key][0] == "'" and infodict[key][-1] == "'":
	    infodict[key] = infodict[key][1:-1]
    # map modes 
    if "mode" in infodict.keys():
	if infodict["mode"] == "1":
	    infodict["mode"] = "r"
	elif infodict["mode"] == "2":
	    infodict["mode"] = "w"
	elif infodict["mode"] == "4":
	    infodict["mode"] = "a"
	else:
	    print "Warning: mode =", infodict["mode"], "is unknown in function", func

    # check for correct attributes
    if func == "fopen":
	check_for_notdef_attr(infodict.keys(), ["name", "mode", "appendrank", "file"], func)
    elif func == "open":
	check_for_notdef_attr(infodict.keys(), ["name", "mode", "appendrank", "file"], func)
    elif func == "stat":
	check_for_notdef_attr(infodict.keys(), ["name", "appendrank", "file"], func)
    elif func == "fclose":
	check_for_required_attr(infodict.keys(), ["file"], func)
	check_for_notdef_attr(infodict.keys(), ["file"], func)
    elif func == "close":
	check_for_required_attr(infodict.keys(), ["file"], func)
	check_for_notdef_attr(infodict.keys(), ["file"], func)
    elif func == "buf_read":
	check_for_required_attr(infodict.keys(), ["file", "size", "block"], func)
	check_for_notdef_attr(infodict.keys(), ["size", "repeat", "block", "file", "partialok"], func)
    elif func == "buf_write":
	check_for_required_attr(infodict.keys(), ["file", "size", "block"], func)
	check_for_notdef_attr(infodict.keys(), ["size", "block", "repeat", "file"], func)
    elif func == "buf_seek":
	check_for_required_attr(infodict.keys(), ["file", "offset"], func)
	check_for_notdef_attr(infodict.keys(), ["filepos", "offset", "file"], func)
    elif func == "read":
	check_for_required_attr(infodict.keys(), ["file", "size", "block"], func)
	check_for_notdef_attr(infodict.keys(), ["size", "block", "repeat", "file", "partialok"], func)
    elif func == "write":
	check_for_required_attr(infodict.keys(), ["file", "size", "block"], func)
	check_for_notdef_attr(infodict.keys(), ["size", "block", "repeat", "file"], func)
    elif func == "seek":
	check_for_required_attr(infodict.keys(), ["file", "offset"], func)
	check_for_notdef_attr(infodict.keys(), ["offset", "file"], func)
    elif func == "sync":
	check_for_required_attr(infodict.keys(), ["file"], func)
	check_for_notdef_attr(infodict.keys(), ["file"], func)
    elif func == "sleep":
	check_for_required_attr(infodict.keys(), ["usec"], func)
	check_for_notdef_attr(infodict.keys(), ["usec"], func)
    elif func == "unlink":
	check_for_notdef_attr(infodict.keys(), ["name", "appendrank", "file"], func)
    elif func == "pread":
	check_for_required_attr(infodict.keys(), ["file", "size", "block", "offset"], func)
	check_for_notdef_attr(infodict.keys(), ["size", "block", "repeat", "file", "offset", "partialok"], func)
    elif func == "pwrite":
	check_for_required_attr(infodict.keys(), ["file", "size", "block", "offset"], func)
	check_for_notdef_attr(infodict.keys(), ["size", "block", "repeat", "file", "offset"], func)
    elif func == "mpi_fopen":
	check_for_required_attr(infodict.keys(), ["file"], func)
	check_for_notdef_attr(infodict.keys(), ["name", "mode", "file"], func)
    elif func == "mpi_fclose":
	check_for_required_attr(infodict.keys(), ["file"], func)
	check_for_notdef_attr(infodict.keys(), ["file"], func)
    elif func == "mpi_fdelete":
	check_for_notdef_attr(infodict.keys(), ["filename", "file"], func)
    elif func == "mpi_set_view":
	check_for_required_attr(infodict.keys(), ["file", "etype", "filetype"], func)
	check_for_notdef_attr(infodict.keys(), ["datarep", "filetype", "etype", "offset", "file"], func)
    elif func == "mpi_read":
	check_for_required_attr(infodict.keys(), ["file", "count", "type", "pos", "sync", "coord"], func)
	check_for_notdef_attr(infodict.keys(), ["coord", "sync", "pos", "type", "count", "offset", "file", "op_id"], func)
    elif func == "mpi_write":
	check_for_required_attr(infodict.keys(), ["file", "count", "type", "pos", "sync", "coord"], func)
	check_for_notdef_attr(infodict.keys(), ["coord", "sync", "pos", "type", "count", "offset", "file", "op_id"], func)
    elif func == "mpi_wait":
	check_for_required_attr(infodict.keys(), ["op_id"], func)
	check_for_notdef_attr(infodict.keys(), ["op_id"], func)
    elif func == "barrier":
	check_for_notdef_attr(infodict.keys(), ["taskgroup"], func)
    elif func == "p2p_send":
	check_for_required_attr(infodict.keys(), ["partner"], func)
	check_for_notdef_attr(infodict.keys(), ["partner"], func)
    elif func == "p2p_recv":
	check_for_required_attr(infodict.keys(), ["partner"], func)
	check_for_notdef_attr(infodict.keys(), ["partner"], func)
    elif func == "fadvise":
	check_for_required_attr(infodict.keys(), ["file", "advise"], func)
	check_for_notdef_attr(infodict.keys(), ["advise", "file"], func)
    else:
        print "Warning: The function", func, "is not defined in dios.dtd!"
    return infodict

if __name__ == "__main__":
    # read in options
    option_parser = OptionParser()
    option_parser.add_option("--encoding", action="store", dest="encoding",
                            default="ISO-8859-1", type="string",
                            help="Encoding of the XML-file.")
    option_parser.add_option("-v", "--verbose", action="store_true", 
			    dest="verbose", default=False,
                            help="Turn verbosity on.")
    option_parser.add_option("-f", "--file", action="store", dest="inputdata",
                            default=DEFAULT_INPUT, type="string",
                            help="Path to the input-file.")
    option_parser.add_option("-o", "--outfile", action="store", dest="outfile",
                            default=DEFAULT_OUTPUT, type="string",
                            help="Path to the output-file.")
    options, args = option_parser.parse_args()

    my_print("The following options have been read:", options.verbose)
    my_print("Encoding = " + options.encoding + ", " + "verbosity = " + str(options.verbose), options.verbose)
    my_print("The input will be read from the file " + options.inputdata + ".", options.verbose)
    my_print("The output will be written to the file " + options.outfile + ".", options.verbose)
    
    # read in output from program iopattern
    f = open(options.inputdata, "r")
    content = f.readlines() # content is a list containing the lines of the file as elements
    f.close()

    # processing the data and creating the XML-tree
    root = ET.Element("iotest")
    nestlevel = 1
    maxbufsize = 1
    for line in content:
        if line.strip().startswith("Structure for process"):
            if nestlevel > 1:
                print "Warning: There are nested processes. This might be due to an erroneous input-file. Please check the input- and output-file of this program carefully."
	    if line.strip() != "Structure for process 1":
		proclist[0].set("bufsize", maxbufsize)
		maxbufsize = 1
            proclist = [ET.SubElement(root, "task")]
        elif len(line.strip()) > 2 and line.strip()[0] in ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"] and ":" in line.strip():
            repeat, remainder = line.strip().split(":", 1)
            if len(proclist) > nestlevel:
                for i in reversed(range(nestlevel, len(proclist))):
                    del proclist[i]
            proclist.append(ET.SubElement(proclist[nestlevel-1], "eventset", attrib={"repeat": repeat}))
            if remainder.strip() == "[":
                nestlevel = nestlevel + 1
            else:
                func, info = remainder.strip().split("(", 1)
                infodict = get_attrib(func, info[:-2])
                proclist.append(ET.SubElement(proclist[nestlevel], func, attrib=infodict))
		if "bytes" in infodict.keys():
		    if infodict["bytes"] > maxbufsize:
			maxbufsize = infodict["bytes"]
        elif line.strip() == "]":
            nestlevel = nestlevel - 1
            if nestlevel < 1:
                print "ERROR: Input-file has too many ']'. Exiting."
                sys.exit()
        else:
            my_print("The following line is being ignored.", options.verbose)
	    my_print(line, options.verbose)
    proclist[0].set("bufsize", maxbufsize)

    # writing the XML-tree to a file
    tree = ET.ElementTree(root)
    outfile = open(options.outfile, "w")
    tree.write(outfile, encoding=options.encoding)
    outfile.close()

