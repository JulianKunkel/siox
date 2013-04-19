#include <list>

#include "Callback.h"

class Service {
public:
	void register_error_callback(Callback err_cb);
	void unregister_error_callback(Callback err_cb);
	void clear_error_callbacks();
	
	void register_message_callback(Callback msg_cb);
	void unregister_message_callback(Callback msg_cb);
	void clear_message_callbacks();

	void clear_all_callbacks();
	
private:
	std::list<Callback> error_callbacks;
	std::list<Callback> message_callbacks;
	
};
