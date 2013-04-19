#include "Service.h"

void Service::register_error_callback(Callback err_cb)
{
	error_callbacks.push_back(err_cb);
}


void Service::unregister_error_callback(Callback err_cb)
{
}


void Service::clear_error_callbacks()
{
	error_callbacks.clear();
}


void Service::register_message_callback(Callback msg_cb)
{
	message_callbacks.push_back(msg_cb);
}


void Service::unregister_message_callback(Callback msg_cb)
{
	
}


void Service::clear_message_callbacks()
{
	message_callbacks.clear();
}


void Service::clear_all_callbacks()
{
	clear_error_callbacks();
	clear_message_callbacks();
}

