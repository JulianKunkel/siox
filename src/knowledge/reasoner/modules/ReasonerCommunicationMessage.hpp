#ifndef KNOWLEDGE_REASONER_MODULES_MESSAGE_HPP
#define KNOWLEDGE_REASONER_MODULES_MESSAGE_HPP

#include <knowledge/reasoner/ReasoningDatatypes.hpp>

/*
 Contains the messages that are exchanged between the reasoners.
 */
namespace knowledge {

	//@serializable
	struct ReasonerMessageData{
		//@serializable
		enum class ResponseType : uint8_t {
			NONE = 0,
			PROCES = 1,
			NODE = 2,
			SYSTEM = 3
		};

		ResponseType expectedResponse;

		string reasonerID; // identifier of the reasoner sending this message
	};
}

#endif