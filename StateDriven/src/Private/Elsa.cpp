#include "Public/Elsa.h"

void Elsa::Update(double timeElapsed)
{
	// Set text color to green
	SetTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	m_pStateMachine->Update();
}

bool Elsa::HandleMessage(const Telegram& msg)
{
	return m_pStateMachine ? m_pStateMachine->HandleMessage(msg) : false;
}
