#include "Public/Elsa.h"

void Elsa::Update()
{
	// Set text color to green
	SetTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	m_pStateMachine->Update();
}
