#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "2d/sceneobject/CompositeSprite.h"
#include "Actor.h"

#define TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT 10
#define MINIMUM_ARRIVAL_TIME 10
#define ACTOR_NEAR_TILE_EDGE_PERCENT 0.25

IMPLEMENT_CONOBJECT(Actor);  


void Actor::copyTo(SimObject* object) {  
    // Fetch other object.  
   Actor* pActor = static_cast<Actor*>( object );  
  
   // Sanity!  
   AssertFatal(pActor != NULL, "TileGrid::copyTo() - Object is not the correct type.");  
  
   // Copy parent.  
   Parent::copyTo( object );  
  
   // Copy the state.  

   //pTileGrid->mEmitLight = mEmitLight;  
   //pTileGrid->mBrightness = mBrightness;  
} 

bool Actor::onAdd()  {  
    // Fail if the parent fails.  
    if (!Parent::onAdd())  
        return false;  
  
    // Do some work here.  
    Con::printf("Hello from Actor!");  

	return true;
}  
  
void Actor::onRemove() {  
	// Do some work here.  
} 

Actor::Actor() {
	mSpriteID = 0;
	mSpeed = 1;
	mTargetX = 0;
	mTargetY = 0;
	nextStep = 0;
	advanceOverrideMoveFlag = false;
}
Actor::~Actor() {

}
void Actor::setSprite(const char* assetID, const U32 frame, const F32 sizeX, const F32 sizeY) {

	if(mSpriteID) {
		mCompositeSprite->selectSpriteId(mSpriteID);
		mCompositeSprite->removeSprite();
	}

	SpriteBatchItem::LogicalPosition* position = new SpriteBatchItem::LogicalPosition("0 0"); // Delete this?
	mSpriteID = mCompositeSprite->addSprite(*position);
	mCompositeSprite->selectSpriteId(mSpriteID);
	mCompositeSprite->setSpriteImage(assetID, frame);
	Vector2 size;
	size.x = sizeX;
	size.y = sizeY;
	mCompositeSprite->setSpriteSize(size);
	
	delete position;
}

bool Actor::advanceActionPlan(U32 curDestinationX, U32 curDestinationY, U32 timeUntilArrive) {
	if(nextStep == 0) { // If no more steps, check whether any more moves are required
		Con::executef(this, 2, "checkContinuedMovement"); // May need to do some special handling here...
		return (nextStep != 0);
	}
	advanceOverrideMoveFlag = true;
	Con::executef(this, 2, "checkContinuedMovement");

	if(!advanceOverrideMoveFlag) {
		Con::printf("Bailing out of advanceactionplan because of changes from checkcontinuedmovement");
		return false; // If the flag is cleared, we're moving somewhere completely different - and that code path resolved starting the move
	}
	// Return true if we're done with plan after this step
	if(nextStep == 0) return true;
		
	// There are more steps remaining; store off the location we arrived at
	U32 arrivingX = nextStep->x;
	U32 arrivingY = nextStep->y;
	// Advance to the next step; set up the actor arrived event if it's the last step in the plan
	popActionPlan();
	if(nextStep != 0) { // There are more steps remaining, don't schedule the arrived event
		moveToPosition(nextStep->x, nextStep->y);
		return false;
	} else {
		// Cancel any previous arrived event
		cancelEvent(mArrivedEventID);
		ActorArrivedEvent* newEvent = new ActorArrivedEvent(arrivingX, arrivingY);
		if(timeUntilArrive < TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT) timeUntilArrive = TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT;
		mArrivedEventID = Sim::postEvent(this, newEvent, Sim::getCurrentTime() + timeUntilArrive);
		return true;
	}
}

bool Actor::moveToPosition(const U32 logicalX, const U32 logicalY) {
	F32 strideYFactor = mTileGrid->mStrideY / mTileGrid->mStrideX;

	debugActionPlan();
	if (!(mTileGrid->isValidLocation(logicalX, logicalY))) return false;
	// Right now for simplicity's sake, update the logical position as soon as possible. 
	// But in practice, we'll want to update it over time - probably via calculating the time to enter the next square and setting up an event
	
	Tile* t = mTileGrid->getTile(logicalX,logicalY);
	Vector2* position = t->mCenter;
	// Speed is F32
	mCompositeSprite->cancelMoveTo();
	mCompositeSprite->moveTo(*position, mSpeed, true, true, strideYFactor);
	
	// Create and post event.
	 // Calculate the linear velocity for the specified speed.
    Vector2 linearVelocity = *position - mCompositeSprite->getPosition();
	linearVelocity.y /= strideYFactor;

	//const F32 distance = mCompositeSprite->getLinearVelocity().Normalize(mSpeed);
	const F32 distance = linearVelocity.Normalize(mSpeed);
	// mTileGrid->mStrideY / mTileGrid->mStrideX;
	
    // Calculate the time it will take to reach the target.
    U32 totalMoveTime = (U32)((distance / mSpeed) * 1000.0f);

	cancelEvent(mArrivedEventID);
	cancelEvent(mContinueMoveCheckEventID);

	U32 currentTime = Sim::getCurrentTime();
	U32 continuedMovementTime = totalMoveTime;
	
	std::stringstream sNew;
	sNew << "Initialmovement: " << totalMoveTime << "; ";
	

	if(TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT + MINIMUM_ARRIVAL_TIME > totalMoveTime) {
		continuedMovementTime = TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT; // It always takes at least (a very short time) to arrive in a new square
		totalMoveTime = continuedMovementTime + MINIMUM_ARRIVAL_TIME;
	} else {
		continuedMovementTime = (totalMoveTime - TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT);
	}

	sNew << " Movementtime calc: " << continuedMovementTime << " , " << totalMoveTime;
	Con::printf(sNew.str().c_str());
	//totalMoveTime = 1000;
	//continuedMovementTime = 900;

	// We now add this event at the end...
	ActorContinuedMovementCheckEvent* pMoveEvent = new ActorContinuedMovementCheckEvent(logicalX, logicalY,totalMoveTime - continuedMovementTime); //(totalMoveTime - continuedMovementTime));
	mContinueMoveCheckEventID = Sim::postEvent(this, pMoveEvent, Sim::getCurrentTime() + continuedMovementTime);

	return true;
}

void Actor::addToWindow(const U32 logicalX, const U32 logicalY) {
	if (!(mTileGrid->isValidLocation(logicalX, logicalY))) return;
	
	mLogicalX = logicalX;
	mLogicalY = logicalY;
	
	Tile* t = mTileGrid->getTile(logicalX,logicalY);
	Vector2* position = t->mCenter;
	mCompositeSprite->setPosition(*position);
	
}

void Actor::initPersistFields() {  
    // Call parent.  
    Parent::initPersistFields();  
	
	addField("TileGrid",TypeSimObjectPtr, Offset(mTileGrid, Actor), "TileGrid to use for this Actor.");
	addField("SceneWindow",TypeSimObjectPtr, Offset(mSceneWindow, Actor), "SceneWindow to use for this Actor.");
	addField("CompositeSprite",TypeSimObjectPtr, Offset(mCompositeSprite, Actor), "Composite sprite to use for this Actor.");
	addField("Speed",TypeF32, Offset(mSpeed, Actor), "Speed of this Actor.");
    // Add my fields here.  
   //addField("EmitLight", TypeBool, Offset(mEmitLight, TileGrid), "Flags whether the light is on or off.");  
    //addField("Brightness", TypeF32, Offset(mBrightness, TileGrid), "Sets the brightness of the light.");   
}           

