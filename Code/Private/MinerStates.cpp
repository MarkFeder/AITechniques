#include "Public/MinerStates.h"
#include "Public/Miner.h"
#include "Public/EntityNames.h"
#include "Public/Common/MessageDispatcher.h"
#include "Public/Common/ConsoleUtils.h"
#include "Public/Common/CrudeTimer.h"
#include "Public/Common/MessageTypes.h"

#include <iostream>
using std::cout;

#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif

// EnterMineAndDigForNuggetState

EnterMineAndDigForNuggetState* EnterMineAndDigForNuggetState::Instance()
{
	static EnterMineAndDigForNuggetState instance;

	return &instance;
}

void EnterMineAndDigForNuggetState::Enter(Miner* pMiner)
{
	// if the miner is not already located at the goldmine, he must
	// change location to the gold mine
	if (pMiner->Location() != ELocationType::EL_GoldMine)
	{
		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "\n" << "Walkin' to the goldime";

		pMiner->ChangeLocation(ELocationType::EL_GoldMine);
	}
}

void EnterMineAndDigForNuggetState::Execute(Miner* pMiner)
{
	// The miner digs for gold until he is carrying in excess of MaxNuggets.
	// If he gets thirsty during his digging he packs up work for a while and
	// changes state to go to the saloon for a whiskey
	pMiner->AddToGoldCarried(1);

	pMiner->IncreaseFatigue();

	SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);

	cout << "\n" << ": " << "Pickin' up a nugget";

	// If enough gold mined, go and put it in the bank
	if (pMiner->ArePocketsFull())
	{
		pMiner->GetFSM()->ChangeState(VisitBankAndDepositGoldState::Instance());
	}

	if (pMiner->Thirsty())
	{
		pMiner->GetFSM()->ChangeState(QuenchThirstState::Instance());
	}
}

void EnterMineAndDigForNuggetState::Exit(Miner* pMiner)
{
	SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "\n" << ": " << "Ah'm leavin' the goldmine with mah pockets full o' sweet gold";
}

bool EnterMineAndDigForNuggetState::OnMessage(Miner * pMiner, const Telegram & msg)
{
	// Send msg to global message handler
	return false;
}

// ~EnterMineAndDigForNuggetState

// VisitBankAndDepositGoldState

VisitBankAndDepositGoldState * VisitBankAndDepositGoldState::Instance()
{
	static VisitBankAndDepositGoldState instance;
	
	return &instance;
}

void VisitBankAndDepositGoldState::Enter(Miner * pMiner)
{
	// On entry the miner makes sure he is located at the bank
	if (pMiner->Location() != ELocationType::EL_Bank)
	{
		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "\n" << ": " << "Goin' to the bank. Yes siree";

		pMiner->ChangeLocation(ELocationType::EL_Bank);
	}
}

void VisitBankAndDepositGoldState::Execute(Miner * pMiner)
{
	// Deposit the gold
	pMiner->AddToWealth(pMiner->GoldCarried());

	pMiner->SetGoldCarried(0);

	SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "\n" << ": " << "Depositing gold. Total savings now: " << pMiner->Wealth();

	// Wealthy enough to have a well earned rest?
	if (pMiner->Wealth() >= ComfortLevel)
	{
		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "\n" << ": " << "WooHoo! Rich enough for now. Back home to mah li'lle lady";

		pMiner->GetFSM()->ChangeState(GoHomeAndSleepTilRestedState::Instance());
	}
	else
	{
		// Otherwise, get more gold
		pMiner->GetFSM()->ChangeState(EnterMineAndDigForNuggetState::Instance());
	}
}

void VisitBankAndDepositGoldState::Exit(Miner * pMiner)
{
	SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "\n" << ": " << "Leavin' the bank";
}

bool VisitBankAndDepositGoldState::OnMessage(Miner * pMiner, const Telegram & msg)
{
	// Send msg to global message handler
	return false;
}

// ~VisitBankAndDepositGoldState

// ~GoHomeAndSleepTilRestedState

GoHomeAndSleepTilRestedState * GoHomeAndSleepTilRestedState::Instance()
{
	static GoHomeAndSleepTilRestedState instance;

	return &instance;
}

void GoHomeAndSleepTilRestedState::Enter(Miner * pMiner)
{
	if (pMiner->Location() != ELocationType::EL_Shack)
	{
		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "\n" << ": " << "Walkin' home";

		pMiner->ChangeLocation(ELocationType::EL_Shack);

		// Let the Miner's wife that is at home
		Dispatch->DispatchCustomMessage(SEND_MSG_INMEDIATELY, pMiner->ID(), (int)EEntityName::EEN_Elsa, 
			EMessageType::EMT_HitHoneyImHome, NO_ADDITIONAL_INFO);
	}
}

void GoHomeAndSleepTilRestedState::Execute(Miner * pMiner)
{
	// If miner is not fatigued start to dig for nuggets again
	if (!pMiner->Fatigued())
	{
		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "\n" << ": " << "What a God darn Fantastic nap! Time to find more gold";
		
		pMiner->GetFSM()->ChangeState(EnterMineAndDigForNuggetState::Instance());
	}
	else
	{
		// sleep
		pMiner->DecreaseFatigue();

		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "\n" << ": " << "ZZZZ...";
	}
}

void GoHomeAndSleepTilRestedState::Exit(Miner * pMiner)
{
	SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "\n" << ": " << "Leaving the house";
}

bool GoHomeAndSleepTilRestedState::OnMessage(Miner* pMiner, const Telegram& msg)
{
	SetTextColor(BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	switch (msg.msg)
	{
		case EMessageType::EMT_StewReady:

			cout << "\nMessage handled by " << GetNameOfEntity(pMiner->ID()) 
				<< " at time: " << Clock->GetElapsedTime();

			SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);

			cout << "\n" << GetNameOfEntity(pMiner->ID())
				<< ": Okay Hun, ahm a comin'!";

			pMiner->GetFSM()->ChangeState(EatStewState::Instance());

			return true;

		default:

			return false;
	}
}

// ~GoHomeAndSleepTilRestedState

// QuenchThirstState

QuenchThirstState * QuenchThirstState::Instance()
{
	static QuenchThirstState instance;

	return &instance;
}

void QuenchThirstState::Enter(Miner * pMiner)
{
	if (pMiner->Location() != ELocationType::EL_Saloon)
	{
		pMiner->ChangeLocation(ELocationType::EL_Saloon);

		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "\n" << ": " << "Boy, ah sure is thusty! Walking to the saloon";
	}
}

void QuenchThirstState::Execute(Miner * pMiner)
{
	if (pMiner->Thirsty())
	{
		pMiner->BuyAndDrinkWhiskey();

		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "\n" << ": " << "That's mighty fine sippin liquer";

		pMiner->GetFSM()->ChangeState(EnterMineAndDigForNuggetState::Instance());
	}
	else
	{
		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "\nERROR!\nERROR!\nERROR!";
	}
}

void QuenchThirstState::Exit(Miner * pMiner)
{
	SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "\n"  << ": " << "Leaving the saloon, feelin' good";
}

bool QuenchThirstState::OnMessage(Miner * pMiner, const Telegram & msg)
{
	// Send msg to global message handler
	return false;
}

// ~QuenchThirstState

// EatStewState

EatStewState* EatStewState::Instance()
{
	static EatStewState instance;

	return &instance;
}

void EatStewState::Enter(Miner * pMiner)
{
	cout << "\n" << GetNameOfEntity(pMiner->ID()) << ": " << "Smells Reaal good Elsa!";
}

void EatStewState::Execute(Miner * pMiner)
{
	cout << "\n" << GetNameOfEntity(pMiner->ID()) << ": " << "Tastes real good too!";

	pMiner->GetFSM()->RevertToPreviousState();
}

void EatStewState::Exit(Miner * pMiner)
{
	cout << "\n" << GetNameOfEntity(pMiner->ID()) << ": " << "Thankya li'lle lady. Ah better get back to whatever ah wuz doin'";
}

bool EatStewState::OnMessage(Miner * pMiner, const Telegram & msg)
{
	// Send msg to global message handler
	return false;
}

// ~EatStewState