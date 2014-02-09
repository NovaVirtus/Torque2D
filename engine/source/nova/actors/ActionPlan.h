#ifndef _ACTION_PLAN_H_
#define _ACTION_PLAN_H_
struct ActionPlan {
	U32 x;
	U32 y;
	ActionPlan* nextStep;
	inline ActionPlan(const U32 x, const U32 y)  : x(x), y(y) { nextStep = 0; }
	inline ActionPlan(ActionPlan* appendedPlan, const U32 x, const U32 y) { this->x = x; this->y = y; this->nextStep = appendedPlan; }
	//inline ActionPlan(ActionPlan* appendedPlan, const U32 x, const U32 y) : x(x), y(y), nextStep(appendedPlan) {}
};

/*

struct ActionPlan {
		int logicalX;
		int logicalY;

	/// Constructors.
    inline Vector2( void ) {}
    inline Vector2( const Vector2& copy ) : b2Vec2( copy.x, copy.y ) {}
    inline Vector2( const F32 x, const F32 y ) : b2Vec2(x, y) {}
    inline Vector2( const b2Vec2& vec2 ) : b2Vec2( vec2 ) {}
    inline Vector2( const Point2I& point ) : b2Vec2( F32(point.x), F32(point.y) ) {}
    inline Vector2( const Point2F& point ) : b2Vec2( point.x, point.y ) {}
    inline Vector2( const Point2D& point ) : b2Vec2( F32(point.x), F32(point.y) ) {}
    inline Vector2( const char* pString ) { setString( pString ); }

    /// Operators.
    inline Vector2& operator /= (const F32 s)                           { x /= s; y /= s; return *this; }
};
*/
#endif