#pragma once

#include "State.h"

class Miner;
struct Telegram;

class EnterMineAndDigForNuggetState : public State<Miner>
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

	virtual bool OnMessage(Miner* pMiner, const Telegram& msg);
};

class VisitBankAndDepositGoldState : public State<Miner>
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

	virtual bool OnMessage(Miner* pMiner, const Telegram& msg);
};

class GoHomeAndSleepTilRestedState : public State<Miner>
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

	virtual bool OnMessage(Miner* pMiner, const Telegram& msg);
};

class QuenchThirstState : public State<Miner>
{
public:
	QuenchThirstState() = default;

	// Copy ctor and assignment are deleted
	QuenchThirstState(const QuenchThirstState&) = delete;
	QuenchThirstState& operator=(const QuenchThirstState&) = delete;

	// Retrieves the single Instance
	static QuenchThirstState* Instance();

	virtual void Enter(Miner* pMiner);

	virtual void Execute(Miner* pMiner);

	virtual void Exit(Miner* pMiner);

	virtual bool OnMessage(Miner* pMiner, const Telegram& msg);
};