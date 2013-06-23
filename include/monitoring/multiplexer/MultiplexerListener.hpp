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
	 * @param	Activity	a logged Activity
	 * @return	void
	 */
	virtual void Notify(TYPE * element) {};

};

}

#endif /* MULTIPLEXERLISTENER_H */
