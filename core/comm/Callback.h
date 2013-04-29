#ifndef CALLBACK_H
#define CALLBACK_H

#include "ConnectionMessage.h"

// template <class T>
class Callback {
public:
	virtual void handle_message(ConnectionMessage &msg) const
	{
	};
};

#endif
