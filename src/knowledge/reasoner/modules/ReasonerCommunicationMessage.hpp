#ifndef KNOWLEDGE_REASONER_MODULES_MESSAGE_HPP
#define KNOWLEDGE_REASONER_MODULES_MESSAGE_HPP

#include <knowledge/reasoner/ReasoningDatatypes.hpp>

/*
 The message header that is exchanged between the reasoners.
 After a message header is send, the data specific to the ResponseType is transmitted.
 */
namespace knowledge {

	//@serializable
	struct ReasonerMessageData{
		//@serializable
		enum class DataType : uint8_t {
			NONE = 0,
			PROCESS = 1,
			NODE = 2,
			SYSTEM = 3
		};

		DataType containedData;
		DataType expectedResponse;

		string reasonerID; // identifier of the reasoner sending this message
	};
}

#endif