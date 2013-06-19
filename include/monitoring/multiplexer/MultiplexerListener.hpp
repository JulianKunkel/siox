#ifndef ACTIVITYMULTIPLEXERLISTENER_H
#define ACTIVITYMULTIPLEXERLISTENER_H 


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
	virtual void Notify(TYPE * element) =0;

};

}

#endif /* ACTIVITYMULTIPLEXERLISTENER_H */
