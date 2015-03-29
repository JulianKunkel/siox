#include "../TraceReader.hpp"

int main( int argc, char ** argv )
{
	TraceReader tr = TraceReader( "activities.dat", "system-info.dat", "ontology.dat", "association.dat", "", "");
	while( true ) {
		std::shared_ptr<Activity> a = tr.nextActivity();
		if( a == nullptr )
			break;
		tr.printActivity( a );
//		delete( a );
	}
	return 0;
}
