#include "Common/Public/Entities/BaseGameEntity.h"
#include "Common/Public/Entities/EntityManager.h"
#include "Common/Public/Entities/EntityNames.h"
#include "Common/Public/Time/CrudeTimer.h"
#include "Common/Public/Messaging/MessageTypes.h"
#include "Common/Public/Messaging/MessageDispatcher.h"

// #include "Common/Public/Locations.h"

#include <iostream>
#include <set>

using std::cout;
using std::set;

#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif

MessageDispatcher* MessageDispatcher::Instance()
{
	static MessageDispatcher instance;

	return &instance;
}

void MessageDispatcher::Discharge(BaseGameEntity* pReceiver, const Telegram& msg)
{
	if (!pReceiver->HandleMessage(msg))
	{
		// Telegram could not be handled
		cout << "Message could not be handled";
	}
}

void MessageDispatcher::DispatchCustomMessage(double delay, int sender, int receiver, EMessageType msg, void* extraInfo)
{
	SetTextColor(BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	// Get a pointer to the receiver and sender
	BaseGameEntity* pSender = EntityMgr->GetEntityFromID(sender);
	BaseGameEntity* pReceiver = EntityMgr->GetEntityFromID(receiver);

	// Make sure the receiver is valid
	if (pReceiver == nullptr)
	{
		cout << "\nWarning! No Receiver with ID of " << receiver << " found";
		return;
	}

	// Create the telegram
	Telegram message(0, sender, receiver, msg, extraInfo);

	// If there is no delay, route telegram inmediately
	if (delay <= 0.0f)
	{
		cout << "\nInstant telegram dispatched at time: " << Clock->GetElapsedTime()
			<< " by " << GetNameOfEntity(pSender->ID()) << " for " << GetNameOfEntity(pReceiver->ID())
			<< ". Msg is " << EMsgTypeToStr(msg);

		// send the telegram to the recipient
		Discharge(pReceiver, message);
	}
	else
	{
		// Calculate the time when the telegram should be dispatched
		double currentTime = Clock->GetElapsedTime();

		message.dispatchTime = currentTime + delay;

		// and put the message into the queue
		priorityQueue.insert(message);

		cout << "\nDelayed telegram from " << GetNameOfEntity(pSender->ID()) << " recorded at time "
			<< Clock->GetElapsedTime() << " for " << GetNameOfEntity(pReceiver->ID())
			<< ". Msg is " << EMsgTypeToStr(msg);
	}
}

void MessageDispatcher::DispatchDelayedMessages()
{
	SetTextColor(BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	// Get current time
	double currentTime = Clock->GetElapsedTime();
	
	// Now peek at the queue to see if any telegrams need dispatching.
	// Remove all telegrams from the front of the queue that have gone
	// past their sell by date
	while ( !priorityQueue.empty() &&
			(priorityQueue.begin()->dispatchTime < currentTime) &&
			(priorityQueue.begin()->dispatchTime > 0))
	{
		// Read the telegram from the front of the queue
		const Telegram& telegram = *priorityQueue.begin();

		// Find the recipient
		BaseGameEntity* pReceiver = EntityMgr->GetEntityFromID(telegram.receiver);

		cout << "\nQueued telegram ready for dispatch: Sent to "
			<< GetNameOfEntity(pReceiver->ID()) << ". Msg is " << EMsgTypeToStr(telegram.msg);

		// Send the telegram to the receipient
		Discharge(pReceiver, telegram);

		// Remove it from the queue
		priorityQueue.erase(priorityQueue.begin());
	}
}