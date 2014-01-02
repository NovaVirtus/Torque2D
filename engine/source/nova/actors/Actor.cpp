#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "2d/sceneobject/CompositeSprite.h"
#include "Actor.h"


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
	mSpeed = 1;
	return true;
}  
  
void Actor::onRemove() {  
	// Do some work here.  
} 

Actor::Actor() {
	mSpriteID = 0;
}
Actor::~Actor() {

}
void Actor::setSprite(const char* assetID, const U32 frame, const int sizeX, const int sizeY) {

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

bool Actor::moveToPosition(const int logicalX, const int logicalY) {
	if (!(mTileGrid->isValidLocation(logicalX, logicalY))) return false;
	// Right now for simplicity's sake, update the logical position as soon as possible. But in practice, we'll want to update it over time - probably via calculating the time to enter the next square and setting up an event
	//mLogicalX = logicalX;
	//mLogicalX = logicalY;
	
	int testX = 0;
	int testY = 0;
	
	/*std::stringstream sTest;
	sTest << "Current tile grid size: " << mTileGrid->mSizeX << "," << mTileGrid->mSizeY;
	Con::printf(sTest.str().c_str());
	for(testY = 0; testY < mTileGrid->mSizeY; testY++) {
		for(testX = 0; testX < mTileGrid->mSizeX; testX++) {
			std::stringstream ss;
			ss << "Checking tile " << testX << ", " << testY << " -> " << mTileGrid->index(testX, testY) << " | " << mTileGrid->isValidLocation(testX,testY) << " | = ";
			if(mTileGrid->isValidLocation(testX,testY)) {
				Tile* t = mTileGrid->getTile(testX, testY);
				Vector2* position = t->mCenter;
				ss << position->x << ", " << position->y;
			}
			
			
			Con::printf(ss.str().c_str());
		}
	}*/

	Tile* t = mTileGrid->getTile(logicalX,logicalY);
	Vector2* position = t->mCenter;
	// Speed is F32
	
	mCompositeSprite->moveTo(*position, mSpeed, true, true, (mTileGrid->mStrideY / mTileGrid->mStrideX));
	
	// Create and post event.
	 // Calculate the linear velocity for the specified speed.
    Vector2 linearVelocity = *position - mCompositeSprite->getPosition();
	linearVelocity.y /= (mTileGrid->mStrideY / mTileGrid->mStrideX);

	//const F32 distance = mCompositeSprite->getLinearVelocity().Normalize(mSpeed);
	const F32 distance = linearVelocity.Normalize(mSpeed);
	// mTileGrid->mStrideY / mTileGrid->mStrideX;
	


    // Calculate the time it will take to reach the target.
    const U32 time = (U32)((distance / mSpeed) * 1000.0f);

	// Cancel any previous arrived event
	if(mArrivedEventID != 0) {
		Sim::cancelEvent(mArrivedEventID);
		mArrivedEventID = 0;
	}
	if(mContinueMoveCheckEventID != 0) {
		Sim::cancelEvent(mContinueMoveCheckEventID);
		mContinueMoveCheckEventID = 0;
	}

	U32 targetTime = Sim::getCurrentTime();
	U32 targetOffset = (time - 1);
	if(targetOffset <= 0) targetOffset = 1;
	ActorArrivedEvent* pEvent = new ActorArrivedEvent(logicalX, logicalY);
	mArrivedEventID = Sim::postEvent(this, pEvent, 1 + targetTime + time);
	ActorContinuedMovementCheckEvent* pMoveEvent = new ActorContinuedMovementCheckEvent(logicalX, logicalY);
	mContinueMoveCheckEventID = Sim::postEvent(this, pMoveEvent, targetTime + targetOffset);

	std::stringstream ss;
	ss << "Actor added to tile " << logicalX << "," << logicalY << " -> center = " << position->x << "," << position->y;
	Con::printf(ss.str().c_str());

	return true;
}

void Actor::addToWindow(const int logicalX, const int logicalY) {
	if (!(mTileGrid->isValidLocation(logicalX, logicalY))) return;
	
	mLogicalX = logicalX;
	mLogicalY = logicalY;
	
	Tile* t = mTileGrid->getTile(logicalX,logicalY);
	Vector2* position = t->mCenter;
	std::stringstream ss;
	mCompositeSprite->setPosition(*position);
	ss << "Actor added to tile " << logicalX << "," << logicalY << " -> center = " << position->x << "," << position->y;
	Con::printf(ss.str().c_str());
	
	//mTileGrid->getWorldCoordinates(logicalX, logicalY, position);

	//mCompositeSprite->setPosition(position);
//	
	//position.x = worldX;
	//position.y = worldY;
	//std::stringstream ss;
	//ss << "Actor added to tile " << logicalX << "," << logicalY << " -> center = " << position.x << "," << position.y;
	//Con::printf(ss.str().c_str());
	//mCompositeSprite->setPosition(position);

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

