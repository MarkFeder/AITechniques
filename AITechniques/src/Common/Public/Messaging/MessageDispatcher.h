#pragma once

#include <set>
#include "Common/Public/Misc/ConsoleUtils.h"
#include "Telegram.h"

class BaseGameEntity;

// to make code easier to read
const double SEND_MSG_INMEDIATELY = 0.0f;
const int NO_ADDITIONAL_INFO = 0;

// to make life easier ...
#define Dispatch MessageDispatcher::Instance()

class MessageDispatcher
{
private:

	// A std::set is used as the container for the delayed messages
	// because of the benefit of automatic sorting and avoidance
	// of duplicates. messages are sorted by their dispatch time.
	std::set<Telegram> priorityQueue;

	// This method is utilized by DispatchMessage or DispatchDelayedMessages.
	// This method calls the message handling member function of the receiving
	// entity, pReceiver, with the newly created telegram
	void Discharge(BaseGameEntity* pReceiver, const Telegram& msg);

	MessageDispatcher() = default;

public:

	//Copy constructor and assignment should be deleted
	MessageDispatcher(const MessageDispatcher&) = delete;
	MessageDispatcher& operator=(const MessageDispatcher&) = delete;

	// This class is a singleton
	static MessageDispatcher* Instance();

	// Send a message to another agent. Receiving agent is referenced by ID.
	void DispatchCustomMessage(double delay, int sender, int receiver, EMessageType msg, void* extraInfo);

	// Send out any delayed messages. This method is called each time through the main game loop.
	void DispatchDelayedMessages();
};
