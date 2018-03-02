#pragma once

struct Telegram;

template<class entity_type>
class State
{
public:

	virtual ~State() = default;

	// This will be executed when the state is entered
	virtual void Enter(entity_type* pEntity) = 0;

	// This will be called by the miner's update function each update step
	virtual void Execute(entity_type* pEntity) = 0;

	// This will execute when the state is exited
	virtual void Exit(entity_type* pEntity) = 0;

	// This executes if the agent receives a message from the 
	// message dispatcher
	virtual bool OnMessage(entity_type* pEntity, const Telegram& msg) = 0;
};

