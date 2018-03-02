#pragma once

#include <iostream>
#include <string>

enum class EEntityName : short
{
	EEN_MinerBob = 1,
	EEN_Elsa = 2
};

inline std::string GetNameOfEntity(short n)
{
	switch (n)
	{
		case 1:
			return "Miner Bob";
		
		case 2:
			return "Elsa";

		default:
			return "UNKNOWN!";
	}
}
