#pragma once

class Miner;

class State
{
public:

	virtual ~State() = default;

	// This will be executed when the state is entered
	virtual void Enter(Miner* pMiner) = 0;

	// This will be called by the miner's update function each update step
	virtual void Execute(Miner* pMiner) = 0;

	// This will execute when the state is exited
	virtual void Exit(Miner* pMiner) = 0;
};

