#pragma once

#include "Public/Entities/BaseGameEntity.h"
#include "Public/FSM/StateMachine.h"
#include "Locations.h"

// #define GET_PROPERTY(NameMethod, PropertyName, PropertyType) PropertyType Get##NameMethod() const { return PropertyName; }

// The amount of gold a miner must have before he feels comfortable
const int ComfortLevel = 5;
// The amount of nuggets a miner can carry
const int MaxNuggets = 3;
// Above this value a miner is thirsty
const int ThirstLevel = 5;
// Above this value, a miner is sleepy
const int TirednessThreshold = 5;

class Miner : public BaseGameEntity
{
private:

	// A pointer to an instance of a State
	StateMachine<Miner>* m_pStateMachine;

	// The current location type
	ELocationType m_location;

	// How many nuggets the miner hast in his pockets
	int m_iGoldCarried;

	// How much money the miner has deposited in the bank
	int m_iMoneyInBank;

	// The higher the value, the thirstier the miner
	int m_iThirst;

	// The higher the value, the more tired the miner
	int m_iFatigue;

public:

	Miner(int ID);
	
	// This must be implemented
	void Update(double timeElapsed) override;

	// This method handle received message
	bool HandleMessage(const Telegram& msg) override;

	// This method retrieves the current state machine owned by the Miner
	StateMachine<Miner>* GetFSM() const { return m_pStateMachine; }

	// Performs operations related to Miner's location
	ELocationType Location() const { return m_location; }
	void ChangeLocation(const ELocationType& newLocation) { m_location = newLocation; }

	// Performs operations related to Miner's gold carried
	int GoldCarried() const { return m_iGoldCarried; }
	void SetGoldCarried(const int newGoldCarried) { m_iGoldCarried = newGoldCarried; }
	void AddToGoldCarried(const int val);
	bool ArePocketsFull() const { return m_iGoldCarried >= MaxNuggets; }

	// Performs operations related to Miner's fatigue
	bool Fatigued() const;
	void DecreaseFatigue() { m_iFatigue -= 1; }
	void IncreaseFatigue() { m_iFatigue += 1; }

	// Performs operations related to Miner's wealth
	int Wealth() const { return m_iMoneyInBank; }
	void SetWealth(const int val) { m_iMoneyInBank = val; }
	void AddToWealth(const int val);

	// Performs operations related to Miner's thirst
	bool Thirsty() const;
	void BuyAndDrinkWhiskey() { m_iThirst = 0; m_iMoneyInBank -= 2; }
};