#ifndef ACTIVITYMULTIPLEXERLISTENER_H
#define ACTIVITYMULTIPLEXERLISTENER_H 

#include <monitoring/multiplexer/MultiplexerListener.hpp>

namespace monitoring{
/**
 * ActivityMultiplexerListener
 * Interface plugins that want to register to a multiplexer need to implement. 
 */
template <class TYPE>
class MultiplexerListenerAsync : MultiplexerListener<TYPE>
{
};

}

#endif /* ACTIVITYMULTIPLEXERLISTENER_H */
