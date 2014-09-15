#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "2d/sceneobject/CompositeSprite.h"
#include "Actor.h"

#define TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT 3  //5 10
#define MINIMUM_ARRIVAL_TIME 10
#define ACTOR_NEAR_TILE_EDGE_PERCENT 0.25

#define POINT_DISTANCE_MAXIMUM_COLLAPSE (F64)0.1

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
	mNextStep = 0;
	mNextSpriteStep = 0;
	mFrame = 0;
	mDepth = 0;
	advanceOverrideMoveFlag = false;
	mBorderObjectBeingPassedThrough = 0;

	for(int i = 0; i < MAX_LOCKED_TILES; i++) mLockedTiles[i] = 0;
	mNumLockedTiles = 0;

}
Actor::~Actor() {

}
void Actor::setSprite(const char* assetID, const U32 frame, const F32 sizeX, const F32 sizeY) {

	if(mSpriteID) {
		mCompositeSprite->selectSpriteId(mSpriteID);
		mCompositeSprite->removeSprite();
	}

	SpriteBatchItem::LogicalPosition* position = new SpriteBatchItem::LogicalPosition("0 2"); // Delete this? Was 0 0
	mSpriteID = mCompositeSprite->addSprite(*position);
	mCompositeSprite->selectSpriteId(mSpriteID);
	mCompositeSprite->setSpriteImage(assetID, frame);
	Vector2 size;
	size.x = sizeX;
	size.y = sizeY;
	mCompositeSprite->setSpriteSize(size);

	delete position;
}

bool Actor::advanceSpriteMovePlan() {
	mCompositeSprite->cancelMoveTo();
	if(!mNextSpriteStep) return false;

	F32 speed = mSpeed / (mNextSpriteStep->tile->mExtraMovementCost);
	Vector2 targetDestination;
	targetDestination.x = (F32)mNextSpriteStep->x;
	targetDestination.y = (F32)mNextSpriteStep->y;
	//stringstream ss;
	mDepth = (F32)(mNextSpriteStep->tile->mLogicalY) - (F32)(mNextSpriteStep->tile->mLogicalX);
	
	//mCompositeSprite->selectSpriteId(mSpriteID);
	//mDepth = -1000;
	// The important thing to keep in mind here is:
	// If BATCH ISOLATION is ON, then you set the SceneLayerDepth
	// If BATCH ISOLATION is OFF, then you set the depth for each sprite independently.
	// Thus, we want batch isolation for these; the actor's separate sprites should stay together.
	mCompositeSprite->setSceneLayerDepth(mDepth);
	//mCompositeSprite->setBatchIsolated(true);
	//mCompositeSprite->setSpriteDepth(mDepth);
	//ss << "Setting depth to " << mNextSpriteStep->tile->mLogicalY << " - " << mNextSpriteStep->tile->mLogicalX << " = " << mCompositeSprite->getSceneLayerDepth();
	//Con::printf(ss.str().c_str());
	//mCompositeSprite->
	//stringstream ss;
	//ss << "Now moving to " << targetDestination.x << "," << targetDestination.y << " at cost " << mNextSpriteStep->tile->mExtraMovementCost << " gives speed " << speed;
	//Con::printf(ss.str().c_str());
	U32 timeToArrive;
	mCompositeSprite->moveTo(timeToArrive, targetDestination, speed, true, true, mTileGrid->strideYFactor());

	//mTileGrid->spinTile(mNextSpriteStep->tile->mLogicalX, mNextSpriteStep->tile->mLogicalY);

	//const F32 dist = mCompositeSprite->getLinearVelocity().Normalize(speed);
	//U32 totalMoveTime = (U32)((dist / speed) * 1000.0f);
	cancelEvent(mArrivedEventID);
	cancelEvent(mContinueMoveCheckEventID);

	U32 currentTime = Sim::getCurrentTime();
	U32 continuedMovementTime; //= totalMoveTime;
	
	if(TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT + MINIMUM_ARRIVAL_TIME > timeToArrive) {
		continuedMovementTime = TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT; // It always takes at least (a very short time) to arrive in a new square
		timeToArrive = continuedMovementTime + MINIMUM_ARRIVAL_TIME;
	} else {
		continuedMovementTime = (timeToArrive - TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT);
	}

	popSpritePlan();

	ActorContinuedMovementCheckEvent* pMoveEvent = new ActorContinuedMovementCheckEvent(timeToArrive - continuedMovementTime); //logicalX, logicalY,totalMoveTime - continuedMovementTime); //(totalMoveTime - continuedMovementTime));
	mContinueMoveCheckEventID = Sim::postEvent(this, pMoveEvent, Sim::getCurrentTime() + continuedMovementTime);
	//Con::printf("Continued movement check scheduled");
	return true;
}

bool Actor::advanceActionPlan(U32 timeUntilArrive) {
	if(advanceSpriteMovePlan()) return true; // Try to advance sprite movement first, if possible

	if(mBorderObjectBeingPassedThrough != 0) {
		mBorderObjectBeingPassedThrough->endActorPassthrough(this);
		mBorderObjectBeingPassedThrough = 0;
	}

	if(mNextStep == 0) { // If no more steps, check whether any more moves are required
		Con::executef(this, 2, "checkContinuedMovement"); // May need to do some special handling here...
		return (mNextStep != 0);
	}
	advanceOverrideMoveFlag = true;
	Con::executef(this, 2, "checkContinuedMovement");

	if(!advanceOverrideMoveFlag) {
		//Con::printf("Bailing out of advanceactionplan because of changes from checkcontinuedmovement");
		return false; // If the flag is cleared, we're moving somewhere completely different - and that code path resolved starting the move
	}
	// Return true if we're done with plan after this step
	if(mNextStep == 0) return true;
		
	// There are more steps remaining; store off the location we arrived at
	U32 arrivingX = mNextStep->x;
	U32 arrivingY = mNextStep->y;
	// Advance to the next step; set up the actor arrived event if it's the last step in the plan
	popActionPlan();
	mLogicalX = arrivingX;
	mLogicalY = arrivingY;
	if(mNextStep != 0) { // There are more steps remaining, don't schedule the arrived event

		// Arrived!
		//mDepth = (F32)mLogicalY - (F32)mLogicalX;
		//mCompositeSprite->setSceneLayerDepth(mDepth);
		Tile* nextTile = mTileGrid->getTile(mNextStep->x, mNextStep->y);
		TileLockedState nextState;
		if(mNextStep->nextStep != 0) nextState = TILE_LOCK_TEMPORARY;
		else nextState = TILE_LOCK_INDETERMINATE; // This could also be "working" if we're going to a workstation, etc.

		if(nextTile->mLockState != TILE_UNLOCKED) restartPathToTarget();

		//if(!LockTile(nextTile, nextState)) restartPathToTarget(); We lock this in moveToPosition

		if(!moveToPosition(mNextStep->x, mNextStep->y)) { //Try to go to next space; if we can't get there (wall was built in meantime, etc.) need to repath
			restartPathToTarget();
		} else {
			
			//nextTile->mLockState = nextState; // We could check to make sure it's locked, but moveToPosition should verify this

			//nextTile->mLockState = TILE_LO
		}
		return false;
	} else {
		// Cancel any previous arrived event
		cancelEvent(mArrivedEventID);
		ActorArrivedEvent* newEvent = new ActorArrivedEvent(arrivingX, arrivingY);
		if(timeUntilArrive < TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT) timeUntilArrive = TIME_BEFORE_ARRIVE_TO_CHECK_MOVEMENT;
		mArrivedEventID = Sim::postEvent(this, newEvent, Sim::getCurrentTime() + timeUntilArrive);
		//Con::printf("Arrived event scheduled");
		
		UnlockMoveTiles(mTileGrid->getTile(mLogicalX, mLogicalY)); // Unlocks all but the last tile

		return true;
	}
}

bool Actor::findCrossingPoints(Point2D* currentTileCenter, const F32 strideX, const F32 strideY, const S32 logicalOffsetX, const S32 logicalOffsetY, Point2D* currentPosition, Point2D* targetPosition, bool & crossingX, Point2D & intersectionX, bool & crossingY, Point2D & intersectionY) {
	if(logicalOffsetX == 0 && logicalOffsetY == 0) {
		crossingX = false;
		crossingY = false;
		return false;
	}
	//stringstream ss;
	//ss << "=====================";
	//ss << "<" << logicalOffsetX << "," << logicalOffsetY << "> ";
	//ss << "Movement being compared is from " << currentPosition->x << "," << currentPosition->y << " to " << targetPosition->x << "," << targetPosition->y;
	
	F64 crossingX1, crossingY1, crossingX2, crossingY2;
	if(logicalOffsetX != 0) {
		crossingX1 = currentTileCenter->x;
		crossingX2 = crossingX1;
		crossingY1 = currentTileCenter->y;
		crossingY2 = crossingY1;
		// Assuming stride is full tile length; we're trying to draw lines from essentially the near edge of 1 tile to the direction we're moving (right/left) + 1 tile up/down
		if(logicalOffsetX > 0) { // On the screen, the moves this catches are: Straight right, right + down, or straight down; 
			crossingX1 += strideX;
			crossingY2 -= strideY;
			//Bottom-left corner: - 1/2 strideX< -1 stride Y
			/*crossingX1 -= (0.5 * strideX) * 0.5;
			crossingY1 -= (strideY) * 0.5;
			// Top-right corner: Add strideX + 1/2 stride Y
			crossingX2 += (strideX) * 0.5;
			crossingY2 += (0.5 * strideY) * 0.5;*/
			//crossingX = FindIntersection(currentPosition->x, currentPosition->y, targetPosition->x, targetPosition->y, crossX1.x, crossX1.y, crossX2.x, crossX2.y, intersectionX);
		} else if(logicalOffsetX < 0) {
			crossingX2 -= strideX;
			crossingY1 += strideY;
			//Top-right corner
			/*crossingX1 += (0.5 * strideX) * 0.5;
			crossingY1 += (strideY) * 0.5;
			//Bottom-left corner
			crossingX2 -= (strideX) * 0.5;
			crossingY2 -= (0.5 * strideY) * 0.5;*/
		}
		//ss << " | X Compared vs. " << crossingX1 << "," << crossingY1 << "->" << crossingX2 << "," << crossingY2;
		crossingX = FindIntersection(currentPosition->x, currentPosition->y, targetPosition->x, targetPosition->y, crossingX1, crossingY1, crossingX2, crossingY2, intersectionX);
	} else {
		crossingX = false;
	}

	if(logicalOffsetY != 0) {
		crossingX1 = currentTileCenter->x;
		crossingX2 = crossingX1;
		crossingY1 = currentTileCenter->y;
		crossingY2 = crossingY1;
		if(logicalOffsetY > 0) {
			crossingY1 += strideY;
			crossingX2 += strideX;
			/*crossingX1 -= (0.5 * strideX) * 0.5;
			crossingY1 += (strideY) * 0.5;
			
			crossingX2 += (strideX) * 0.5;
			crossingY2 -= (0.5 * strideY) * 0.5;*/
		} else {
			crossingX1 -= strideX;
			crossingY2 -= strideY;
			/*crossingX1 -= (strideX) * 0.5;
			crossingY1 += (0.5 * strideY) * 0.5;

			crossingX2 += (0.5 * strideX) * 0.5;
			crossingY2 -= (strideY) * 0.5;*/
		}
		//ss << " | Y Compared vs. " << crossingX1 << "," << crossingY1 << "->" << crossingX2 << "," << crossingY2;
		crossingY = FindIntersection(currentPosition->x, currentPosition->y, targetPosition->x, targetPosition->y, crossingX1, crossingY1, crossingX2, crossingY2, intersectionY);
	} else { 
		crossingY = false;
	}
	//ss << " ||";
	//if(crossingX) ss << " Crossing X at " << intersectionX.x << "," << intersectionX.y << " |";
	//if(crossingY) ss << " Crossing Y at " << intersectionY.x << "," << intersectionY.y << " |";
	//Con::printf(ss.str().c_str());
	return (crossingX || crossingY);
}

bool Actor::moveToPosition(const U32 logicalX, const U32 logicalY) {
	F32 strideX = mTileGrid->mStrideX;
	F32 strideY = mTileGrid->mStrideY;
	Tile* currentTile = mTileGrid->getTile(mLogicalX, mLogicalY);
	//debugActionPlan();
	if (!(mTileGrid->isValidLocation(logicalX, logicalY))) {
		cancelCurrentMove(); // handles unlocking tiles
		return false;
	}
	
	Tile* targetTile = mTileGrid->getTile(logicalX,logicalY);
	if(targetTile->mLockState != TILE_UNLOCKED && (targetTile != currentTile)) { cancelCurrentMove(); return false; }
	if(!(mTileGrid->canMoveBetweenTiles(currentTile, targetTile))) { cancelCurrentMove(); return false; }
	// Right now for simplicity's sake, update the logical position as soon as possible. 
	// But in practice, we'll want to update it over time - probably via calculating the time to enter the next square and setting up an event
	// Sanity check
	UnlockMoveTiles(currentTile);

	Point2D* targetPosition = targetTile->mCenter;
	Vector2* curSpriteVector = &(mCompositeSprite->getPosition());
	Point2D* currentPosition = new Point2D(curSpriteVector->x, curSpriteVector->y);
	Point2D* currentTileCenter = currentTile->mCenter;
	
	S32 offsetX = (S32)logicalX - (S32)mLogicalX;
	S32 offsetY = (S32)logicalY - (S32)mLogicalY;
	
	if(offsetX != 0 || offsetY != 0) {
		//if(mSpriteID == 0) return false;
		if(mSpriteID != 0) {
			mFrame = getFrameForOffset(offsetX, offsetY);
			mCompositeSprite->selectSpriteId(mSpriteID);
			mCompositeSprite->setSpriteImageFrame(mFrame);
		}
	}
	Point2D crossX1, crossX2, crossY1, crossY2;	
	Point2D intersectionX, intersectionY, intersectionXY;
	bool crossingX = false;
	bool crossingY = false;
	
	mNextSpriteStep = new SpriteMovementPlan(targetPosition->x, targetPosition->y, targetTile);
	SpriteMovementPlan* lastStep = mNextSpriteStep;
	Tile* intermediateTileOne = 0;
	Tile* intermediateTileTwo = 0;
	
	if(offsetX != 0 && offsetY != 0) {
		U32 tempLogicalX, tempLogicalY;
		tempLogicalX = (mLogicalX + (offsetX > 0 ? 1 : -1));
		tempLogicalY = (mLogicalY + (offsetY > 0 ? 1 : -1));
		intermediateTileOne = mTileGrid->getTile(tempLogicalX, mLogicalY);
		intermediateTileTwo = mTileGrid->getTile(mLogicalX, tempLogicalY);
		if(intermediateTileOne->mLockState != TILE_UNLOCKED) { cancelCurrentMove(); return false; }
		if(intermediateTileTwo->mLockState != TILE_UNLOCKED) { cancelCurrentMove(); return false; }
		
		LockTile(intermediateTileOne, TILE_LOCK_TEMPORARY);
		LockTile(intermediateTileTwo, TILE_LOCK_TEMPORARY);
	}
	LockTile(targetTile, TILE_LOCK_TEMPORARY); // Need to check if is last planned move..
	
	if(findCrossingPoints(currentTileCenter, strideX, strideY, offsetX, offsetY, currentPosition, targetPosition, crossingX, intersectionX, crossingY, intersectionY)) {
		if(crossingX && crossingY) {
			if(DistanceBetween(intersectionX.x, intersectionX.y, intersectionY.x, intersectionY.y) < POINT_DISTANCE_MAXIMUM_COLLAPSE) { // Check if x and y are crossed at very near same time and if so, collapse
				//Con::printf("=====================Collapsing movements");
				mNextSpriteStep = new SpriteMovementPlan(mNextSpriteStep, (intersectionX.x + intersectionY.x) / 2, (intersectionX.y + intersectionY.y) / 2, mTileGrid->getTile(mLogicalX, mLogicalY));
			} else {
				F64 distX = DistanceBetween(intersectionX.x, intersectionX.y, currentPosition->x, currentPosition->y);
				F64 distY = DistanceBetween(intersectionY.x, intersectionY.y, currentPosition->x, currentPosition->y);
			
				if(distX <= distY) { // Go to the x destination first; shouldn't really need to handle equal case, but sanity check...
					// We build these from end to start, because we're appending. We have the move to middle of target tile already, so get to the target tile
					mNextSpriteStep = new SpriteMovementPlan(mNextSpriteStep, intersectionY.x, intersectionY.y, intermediateTileOne); // After crossing x border, get to y border
					// Get the lock on the tile we need to step through
					mNextSpriteStep = new SpriteMovementPlan(mNextSpriteStep, intersectionX.x, intersectionX.y, currentTile); // Get to x border
				} else { // Go to y destination first
					mNextSpriteStep = new SpriteMovementPlan(mNextSpriteStep, intersectionX.x, intersectionX.y, intermediateTileTwo); // After crossing y border, get to x border
					mNextSpriteStep = new SpriteMovementPlan(mNextSpriteStep, intersectionY.x, intersectionY.y, currentTile); // Get to y border
					//mNextSpriteStep = new SpriteMovementPlan(mNextSpriteStep, intersectionY.x, intersectionY.y, mTileGrid->getTile(mLogicalX, mLogicalY), 0, 0); // Get to y border
				}
			}
		} else if(crossingX) {
			mNextSpriteStep = new SpriteMovementPlan(mNextSpriteStep, intersectionX.x, intersectionX.y, currentTile);
		} else if(crossingY) {
			mNextSpriteStep = new SpriteMovementPlan(mNextSpriteStep, intersectionY.x, intersectionY.y, currentTile);
		}
	}

	mCompositeSprite->cancelMoveTo();
	advanceSpriteMovePlan();
	// Set border object being passed through; there should only ever be one of these per move...
	mBorderObjectBeingPassedThrough  = 0;
	if(offsetX > 0) mBorderObjectBeingPassedThrough = currentTile->mNeighboringBorderObjects[TILE_RIGHT];
	if(offsetX < 0 && mBorderObjectBeingPassedThrough == 0) mBorderObjectBeingPassedThrough = currentTile->mNeighboringBorderObjects[TILE_LEFT];
	else if(offsetY > 0 && mBorderObjectBeingPassedThrough == 0) mBorderObjectBeingPassedThrough = currentTile->mNeighboringBorderObjects[TILE_UP];
	else if(offsetY < 0 && mBorderObjectBeingPassedThrough == 0) mBorderObjectBeingPassedThrough = currentTile->mNeighboringBorderObjects[TILE_DOWN];
	if(mBorderObjectBeingPassedThrough != 0) mBorderObjectBeingPassedThrough->startActorPassthrough(this);
	
	return true;
}

void Actor::addToWindow(const U32 logicalX, const U32 logicalY) {
	if (!(mTileGrid->isValidLocation(logicalX, logicalY))) return;
	
	mLogicalX = logicalX;
	mLogicalY = logicalY;
	
	Tile* t = mTileGrid->getTile(logicalX,logicalY);
	Point2D* position = t->mCenter;
	Vector2 vPosition;
	vPosition.x = (F32)position->x;
	vPosition.y = (F32)position->y;

	mCompositeSprite->setPosition(vPosition);
	//mCompositeSprite->setPosition(*position);
	
}

void Actor::initPersistFields() {  
    // Call parent.  
    Parent::initPersistFields();  
	
	addField("Depth",TypeF32,Offset(mDepth,Actor), "Depth of this Actor.");
	addField("TileGrid",TypeSimObjectPtr, Offset(mTileGrid, Actor), "TileGrid to use for this Actor.");
	addField("SceneWindow",TypeSimObjectPtr, Offset(mSceneWindow, Actor), "SceneWindow to use for this Actor.");
	addField("CompositeSprite",TypeSimObjectPtr, Offset(mCompositeSprite, Actor), "Composite sprite to use for this Actor.");
	addField("Speed",TypeF32, Offset(mSpeed, Actor), "Speed of this Actor.");
    // Add my fields here.  
   //addField("EmitLight", TypeBool, Offset(mEmitLight, TileGrid), "Flags whether the light is on or off.");  
    //addField("Brightness", TypeF32, Offset(mBrightness, TileGrid), "Sets the brightness of the light.");   
}           

