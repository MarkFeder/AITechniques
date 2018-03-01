#pragma once

#include "State.h"

class Miner;

class EnterMineAndDigForNuggetState : public State
{
public:
	EnterMineAndDigForNuggetState() = default;

	// Copy ctor and assignment are deleted
	EnterMineAndDigForNuggetState(const EnterMineAndDigForNuggetState&) = delete;
	EnterMineAndDigForNuggetState& operator=(const EnterMineAndDigForNuggetState&) = delete;

	// Retrieves the single Instance
	static EnterMineAndDigForNuggetState* Instance();

	virtual void Enter(Miner* pMiner);

	virtual void Execute(Miner* pMiner);

	virtual void Exit(Miner* pMiner);
};

class VisitBankAndDepositGoldState : public State
{
public:
	VisitBankAndDepositGoldState() = default;

	// Copy ctor and assignment are deleted
	VisitBankAndDepositGoldState(const VisitBankAndDepositGoldState&) = delete;
	VisitBankAndDepositGoldState& operator=(const VisitBankAndDepositGoldState&) = delete;

	// Retrieves the single Instance
	static VisitBankAndDepositGoldState* Instance();

	virtual void Enter(Miner* pMiner);

	virtual void Execute(Miner* pMiner);

	virtual void Exit(Miner* pMiner);
};

class GoHomeAndSleepTilRestedState : public State
{
public:
	GoHomeAndSleepTilRestedState() = default;

	// Copy ctor and assignment are deleted
	GoHomeAndSleepTilRestedState(const GoHomeAndSleepTilRestedState&) = delete;
	GoHomeAndSleepTilRestedState& operator=(const GoHomeAndSleepTilRestedState&) = delete;

	// Retrieves the single Instance
	static GoHomeAndSleepTilRestedState* Instance();

	virtual void Enter(Miner* pMiner);

	virtual void Execute(Miner* pMiner);

	virtual void Exit(Miner* pMiner);
};