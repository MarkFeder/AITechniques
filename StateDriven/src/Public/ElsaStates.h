#pragma once

#include "Public/FSM/State.h"

class Elsa;

class WifeGlobalState : public State<Elsa>
{
public:
	WifeGlobalState() = default;

	// Copy ctor and assignment are deleted
	WifeGlobalState(const WifeGlobalState&) = delete;
	WifeGlobalState& operator=(const WifeGlobalState&) = delete;

	// Retrieves the single Instance
	static WifeGlobalState* Instance();

	virtual void Enter(Elsa* pWife) {};

	virtual void Execute(Elsa* pWife);

	virtual void Exit(Elsa* pWife) {};

	virtual bool OnMessage(Elsa* pWife, const Telegram& msg);
};

class DoHouseWorkState : public State<Elsa>
{
public:
	DoHouseWorkState() = default;

	// Copy ctor and assignment are deleted
	DoHouseWorkState(const DoHouseWorkState&) = delete;
	DoHouseWorkState& operator=(const DoHouseWorkState&) = delete;

	// Retrieves the single Instance
	static DoHouseWorkState* Instance();

	virtual void Enter(Elsa* pWife) {};

	virtual void Execute(Elsa* pWife);

	virtual void Exit(Elsa* pWife) {};

	virtual bool OnMessage(Elsa* pWife, const Telegram& msg);
};

class VisitBathroomState : public State<Elsa>
{
public:
	VisitBathroomState() = default;

	// Copy ctor and assignment are deleted
	VisitBathroomState(const VisitBathroomState&) = delete;
	VisitBathroomState& operator=(const VisitBathroomState&) = delete;

	// Retrieves the single Instance
	static VisitBathroomState* Instance();

	virtual void Enter(Elsa* pWife);

	virtual void Execute(Elsa* pWife);

	virtual void Exit(Elsa* pWife);

	virtual bool OnMessage(Elsa* pWife, const Telegram& msg);
};

class CookStewState : public State<Elsa>
{
public:
	CookStewState() = default;

	// Copy ctor and assignment are deleted
	CookStewState(const CookStewState&) = delete;
	CookStewState& operator=(const CookStewState&) = delete;

	// Retrieves the single Instance
	static CookStewState* Instance();

	virtual void Enter(Elsa* pWife);

	virtual void Execute(Elsa* pWife);

	virtual void Exit(Elsa* pWife);

	virtual bool OnMessage(Elsa* pWife, const Telegram& msg);
};