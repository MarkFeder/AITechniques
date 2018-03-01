#include "../Public/MinerStates.h"
#include "../Public/Miner.h"
#include "../Public/Common/ConsoleUtils.h"

#include <iostream>
using std::cout;

#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif

#define LOG_STATE(TEXT) SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY); cout << "\n" << ": " << #TEXT;

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

		LOG_STATE("Walkin' to the goldime");
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
		pMiner->ChangeState(VisitBankAndDepositGoldState::Instance());
	}

	if (pMiner->Thirsty())
	{
		// pMiner->ChangeState(QuenchThirst::Instance());
	}
}

void EnterMineAndDigForNuggetState::Exit(Miner* pMiner)
{
	SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "\n" << ": " << "Ah'm leavin' the goldmine with mah pockets full o' sweet gold";
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

		pMiner->ChangeState(GoHomeAndSleepTilRestedState::Instance());
	}
	else
	{
		// Otherwise, get more gold
		pMiner->ChangeState(EnterMineAndDigForNuggetState::Instance());
	}
}

void VisitBankAndDepositGoldState::Exit(Miner * pMiner)
{
	SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "\n" << ": " << "Leavin' the bank";
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
	}
}

void GoHomeAndSleepTilRestedState::Execute(Miner * pMiner)
{
	// If miner is not fatigued start to dig for nuggets again
	if (!pMiner->Fatigued())
	{
		SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "\n" << ": " << "What a God darn Fantastic nap! Time to find more gold";
		
		pMiner->ChangeState(EnterMineAndDigForNuggetState::Instance());
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

// ~GoHomeAndSleepTilRestedState