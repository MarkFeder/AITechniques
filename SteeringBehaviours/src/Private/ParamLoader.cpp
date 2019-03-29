#include "Public/ParamLoader.h"

//----------------------------- Instance ---------------------------

ParamLoader* ParamLoader::Instance()
{
	static ParamLoader instance;

	return &instance;
}
