#ifndef _ACTION_PLAN_H_
#define _ACTION_PLAN_H_
struct ActionPlan {
	U32 x;
	U32 y;
	ActionPlan* nextStep;
	F32 speedDivisor;
	inline ActionPlan(const U32 x, const U32 y)  : x(x), y(y) { nextStep = 0; }
	inline ActionPlan(ActionPlan* appendedPlan, const U32 x, const U32 y) { this->x = x; this->y = y; this->nextStep = appendedPlan; }
	inline ~ActionPlan() { if(this->nextStep != 0) delete this->nextStep;  };
};
struct SpriteMovementPlan {
	F64 x;
	F64 y;
	Tile* tile;
	SpriteMovementPlan* nextStep;
	
	inline ~SpriteMovementPlan() { if(this->nextStep != 0) delete this->nextStep; }

	inline SpriteMovementPlan(const F64 x, const F64 y, Tile* speedTile) : x(x), y(y) { 
		nextStep = 0;
		tile = speedTile; 
	}
	inline SpriteMovementPlan(SpriteMovementPlan* appendedPlan, const F64 x, const F64 y, Tile* speedTile) : x(x), y(y) {
		nextStep = appendedPlan; 
		tile = speedTile; 
	}
};

#endif