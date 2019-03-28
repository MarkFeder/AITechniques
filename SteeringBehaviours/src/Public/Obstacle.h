#pragma once

#include "Common/Public/2D/Vector2D.h"
#include "Common/Public/Entities/BaseGameEntity.h"
#include "Common/Public/Misc/Cgdi.h"

#include <Windows.h>

class Obstacle : public BaseGameEntity
{
public:

	Obstacle(double x, double y, double r) : BaseGameEntity(0, Vector2D(x, y), r) {};
	Obstacle(Vector2D pos, double radius) : BaseGameEntity(0, pos, radius) {};

	virtual ~Obstacle() = default;

	// These ones are defined as pure virtual functions in BaseGameEntity so they must be
	// implemented
	void Update(double timeElapsed) override {};

	void Render() override { gdi->BlackPen(); gdi->Circle(Pos(), BRadius()); }

	bool HandleMessage(const Telegram& msg) override { return false; }

	void Write(std::ostream& os) const override;

	void Read(std::ifstream& in) override;
};