#include <exception>
#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include "TraceReader.hpp"

using namespace boost;
using namespace std;

/* 
 This little program provides a command line interface to reading siox-traces.
 */
int main(int argc, char ** argv){
	try{
		program_options::options_description desc("Synopsis");
		desc.add_options() 
	    ("help", "This help message :-)")
	    ("afile", program_options::value<string>()->default_value("activities.dat"), "Activity file")
	    ("ofile", program_options::value<string>()->default_value("ontology.dat"), "Ontology file")
	    ("sfile", program_options::value<string>()->default_value("system-info.dat"), "System information file")
	    ("Afile", program_options::value<string>()->default_value("association.dat"), "Association file")
	    ;

		program_options::variables_map vm;
		program_options::store(program_options::parse_command_line(argc, argv, desc), vm);
		program_options::notify(vm);

		if (vm.count("help")) {
			cout << desc << endl;
			return 1;
		}

		TraceReader tr = TraceReader(vm["afile"].as<string>(), vm["sfile"].as<string>(), vm["ofile"].as<string>(), vm["Afile"].as<string>());

		// Now output all activities:
		while(true){
			Activity * a = tr.nextActivity();
			if( a == nullptr )
				break;
			tr.printActivity(a);
		}
	}catch(std::exception & e){
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}