#include "State-Driven/Public/ElsaStates.h"
#include "State-Driven/Public/Elsa.h"
#include "State-Driven/Public/Locations.h"
#include "Common/Public/Entities/EntityNames.h"

#include "Common/Public/Messaging/MessageDispatcher.h"
#include "Common/Public/Messaging/MessageTypes.h"
#include "Common/Public/Messaging/Telegram.h"

#include "Common/Public/Time/CrudeTimer.h"

#include <iostream>
using std::cout;

#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif

// WifeGlobalState 

WifeGlobalState* WifeGlobalState::Instance()
{
	static WifeGlobalState instance;

	return &instance;
}

void WifeGlobalState::Execute(Elsa * pWife)
{
	// 1 in 10 change of needing the bathroom
	if (RandFloat() < 0.1)
	{
		pWife->GetFSM()->ChangeState(VisitBathroomState::Instance());
	}
}

bool WifeGlobalState::OnMessage(Elsa * pWife, const Telegram& msg)
{
	switch (msg.msg)
	{
		case EMessageType::EMT_HitHoneyImHome:

			cout << "\nMessage handled by " << GetNameOfEntity(pWife->ID())
				<< " at time: " << Clock->GetElapsedTime();

			cout << "\n" << GetNameOfEntity(pWife->ID()) 
				<< ": Hi honey. Let me make you some of mah fine country stew";

			pWife->GetFSM()->ChangeState(CookStewState::Instance());

			return true;
	}

	return false;
}

// ~WifeGlobalState

// VisitBathroomState

VisitBathroomState* VisitBathroomState::Instance()
{
	static VisitBathroomState instance;

	return &instance;
}

void VisitBathroomState::Enter(Elsa* pWife)
{
	cout << "\n" << GetNameOfEntity(pWife->ID()) << ": Walkin' to the can. Need to powda mah pretty li'lle nose";
}

void VisitBathroomState::Execute(Elsa * pWife)
{
	cout << "\n" << GetNameOfEntity(pWife->ID()) << ": Ahhhhhh! Sweet relief!";

	pWife->GetFSM()->RevertToPreviousState();
}

void VisitBathroomState::Exit(Elsa* pWife)
{
	cout << "\n" << GetNameOfEntity(pWife->ID()) << ": Leavin' the Jon";
}

bool VisitBathroomState::OnMessage(Elsa * pWife, const Telegram& msg)
{
	return false;
}

// ~VisitBathroomState

// DoHouseWorkState

DoHouseWorkState* DoHouseWorkState::Instance()
{
	static DoHouseWorkState instance;

	return &instance;
}

void DoHouseWorkState::Execute(Elsa * pWife)
{
	switch (RandInt(0, 2))
	{
	case 0:
		cout << "\n" << GetNameOfEntity(pWife->ID()) << ": Moppin' the floor";
		break;

	case 1:
		cout << "\n" << GetNameOfEntity(pWife->ID()) << ": Washin' the dishes";
		break;

	case 2:
		cout << "\n" << GetNameOfEntity(pWife->ID()) << ": Makin' the beed";
		break;
	}
}

bool DoHouseWorkState::OnMessage(Elsa * pWife, const Telegram& msg)
{
	return false;
}

// ~DoHouseWorkState

// CookStewState

CookStewState* CookStewState::Instance()
{
	static CookStewState instance;
	
	return &instance;
}

void CookStewState::Enter(Elsa* pWife)
{
	// If not already cooking, put the stew in the oven
	if (!pWife->IsCooking())
	{
		cout << "\n" << GetNameOfEntity(pWife->ID())
			<< ": Puttin' the stew in the oven";

		// Send a delayed message to myself so that I know when to take 
		// the stew out of the oven
		Dispatch->DispatchCustomMessage(1.5, pWife->ID(), pWife->ID(), 
			EMessageType::EMT_StewReady, NO_ADDITIONAL_INFO);

		pWife->SetCooking(true);
	}
}

void CookStewState::Execute(Elsa* pWife)
{
}

void CookStewState::Exit(Elsa* pWife)
{
}

bool CookStewState::OnMessage(Elsa* pWife, const Telegram& msg)
{
	switch (msg.msg)
	{
		case EMessageType::EMT_StewReady:
			
			cout << "\nMessage received by " << GetNameOfEntity(pWife->ID())
				<< " at time: " << Clock->GetElapsedTime();
			
			cout << "\n" << GetNameOfEntity(pWife->ID())
				<< ": Stew ready! Let's eat";

			// let know that stew is ready
			Dispatch->DispatchCustomMessage(SEND_MSG_INMEDIATELY, pWife->ID(),
				(int)EEntityName::EEN_MinerBob, EMessageType::EMT_StewReady, NO_ADDITIONAL_INFO);

			pWife->SetCooking(false);
			pWife->GetFSM()->ChangeState(DoHouseWorkState::Instance());

			return true;
	}

	return false;
}

// ~CookStewState