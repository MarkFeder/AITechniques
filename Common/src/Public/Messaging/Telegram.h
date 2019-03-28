#pragma once

#include "MessageTypes.h"

#include <iostream>
#include <math.h>

// This defines a telegram. A telegram is a data structure that
// records information required to dispatch messages. Messages
// are used by game agents to communicate with each other
struct Telegram
{
	// The entity that sent this telegram
	int sender;

	// The entity that receives the telegram
	int receiver;

	// The type of message itself. These are all enumerated
	EMessageType msg;

	// Messages can be dispatched inmediately or delayed for a specified
	// amount of time.
	double dispatchTime;

	// Any additional information that may accompany the message
	void* extraInfo;

	// Custom Telegram constructors
	Telegram()
		: sender(-1),
		receiver(-1),
		msg(EMessageType::EMT_NoMessage),
		dispatchTime(-1),
		extraInfo(nullptr)
	{}

	Telegram(double time, int sender, int receiver, EMessageType msg, void* info = nullptr)
		: sender(sender),
		receiver(receiver),
		msg(msg),
		dispatchTime(-1),
		extraInfo(info)
	{}
};

// These telegrams will be stored in a priority queue. Therefore, the >
// operator needs to be overloaded so that the PQ can sort the telegram
// by the time priority. Note how the times must be smaller than
// SmallestDelay apart before two telegram are considered unique
const double smallestDelay = 0.25;

inline bool operator==(const Telegram& t1, const Telegram& t2)
{
	return (fabs(t1.dispatchTime - t2.dispatchTime) < smallestDelay) &&
		(t1.sender == t2.sender) &&
		(t1.receiver == t2.receiver) &&
		(t1.msg == t2.msg);
}

inline bool operator<(const Telegram& t1, const Telegram& t2)
{
	if (t1 == t2)
		return false;
	else
		return t1.dispatchTime < t2.dispatchTime;
}

inline std::ostream& operator<<(std::ostream& os, const Telegram& t)
{
	os << "time: " << t.dispatchTime << " Sender: " << t.sender
		<< " Receiver: " << t.receiver << " Msg: " << EMsgTypeToStr(t.msg);

	return os;
}

// Helper function for dereferencing the extraInfo field of the telegram
// to the required type
template <class T>
inline T DereferenceToType(void* p)
{
	return static_cast<T>(p);
}
