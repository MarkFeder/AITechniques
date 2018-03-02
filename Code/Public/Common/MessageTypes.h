#pragma once

#include <string>

enum class EMessageType : short
{
	EMT_NoMessage = 0,
	EMT_HitHoneyImHome = 1,
	EMT_StewReady = 2
};

inline std::string EMsgTypeToStr(const EMessageType& emt)
{
	switch (emt)
	{
		case EMessageType::EMT_HitHoneyImHome:
			return "HitHoneyImHome";

		case EMessageType::EMT_StewReady:
			return "StewReady";

		default:
			return "Not Recognized!";
	}
}
