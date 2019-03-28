#pragma once

#include <stdlib.h>
#include <fstream>
#include <time.h>

#include "Public/Locations.h"
#include "Public/Miner.h"
#include "Public/Elsa.h"
#include "Public/Entities/EntityManager.h"
#include "Public/Entities/EntityNames.h"

#include "Public/Messaging/MessageDispatcher.h"
#include "Public/Misc/ConsoleUtils.h"

std::ofstream os;
#define UPDATE_CALLS 30
#define ELAPSED_TIME -1

int main()
{
	// Define this to send output to a text file (see Locations.h)
#ifdef TEXTOUTPUT
	os.open("output.txt");
#endif

	// Seed random number generator
	srand((unsigned)time(nullptr));
	
	// Create a Miner
	Miner* pMiner = new Miner((int)EEntityName::EEN_MinerBob);

	// Create Elsa (Miner's wife)
	Elsa* pElsa = new Elsa((int)EEntityName::EEN_Elsa);

	// Register these entities
	EntityMgr->RegisterEntity(pMiner);
	EntityMgr->RegisterEntity(pElsa);

	// Run Miner and Elsa through a few Update calls
	for (int i = 0; i < UPDATE_CALLS; ++i)
	{
		pMiner->Update(ELAPSED_TIME);
		pElsa->Update(ELAPSED_TIME);

		// dispatch any delayed messages
		Dispatch->DispatchDelayedMessages();

		Sleep(800);
	}

	// tidy up
	delete pMiner;
	delete pElsa;

	// Wait for a keypress before exiting
	PressAnyKeyToContinue();

	return EXIT_SUCCESS;
}