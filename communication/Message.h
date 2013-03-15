/**
 * @file Message.h SIOX Message
 * 
 * @version 0.1
 * @authors Alvaro Aguilera, Holger Mickler
 */

#ifndef MESSAGE_H
#define MESSAGE_H

/**
 * A Message is the basic transmission unit exchanged by the SIOX processes.
 */
class Message {
public:
	Message();
	virtual ~Message();
	
	int type;
	int size;
	void *payload;
};

#endif // MESSAGE_H
