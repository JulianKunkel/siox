using namespace std;

class DatabaseSystemInformationOptions: public core::ComponentOptions {
	public:
    	string hostaddr;
    	int port;
    	string user;
    	string password;
    	string dbname;

    	SERIALIZE_CONTAINER( MEMBER( hostaddr ) MEMBER( port ) MEMBER( user ) MEMBER( password ) MEMBER( dbname ) )
};