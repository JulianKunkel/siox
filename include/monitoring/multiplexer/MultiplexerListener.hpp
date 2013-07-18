#ifndef MULTIPLEXERLISTENER_H
#define MULTIPLEXERLISTENER_H 


namespace monitoring{
/**
 * ActivityMultiplexerListener
 * Interface plugins that want to register to a multiplexer need to implement. 
 */
template <class TYPE>
class MultiplexerListener
{
public:
	/**
	 * Receive logged activity from a multiplexer
	 *
	 * @param	element	a logged Activity
	 */
	virtual void Notify(TYPE * element) {};

};

}

#endif /* MULTIPLEXERLISTENER_H */
