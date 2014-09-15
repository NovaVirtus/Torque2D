#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "BorderObject.h"
#include "../actors/Actor.h"
IMPLEMENT_CONOBJECT(BorderObject);  

class Actor;

void BorderObject::copyTo(SimObject* object) {  
    // Fetch other object.  
   BorderObject* pWall = static_cast<BorderObject*>( object );  
  
   // Sanity!  
   AssertFatal( pWall != NULL, "BorderObject::copyTo() - Object is not the correct type.");  
  
   // Copy parent.  
   Parent::copyTo( object );  
  
   // Copy the state. 
} 

bool BorderObject::onAdd() {
    // Fail if the parent fails.  
    if ( !Parent::onAdd() )  
        return false;  
  
    // Do some work here.  
    Con::printf("Hello from BorderObject!");  
	return true;
}  
  
void BorderObject::onRemove() {
    // Do some work here.  
} 

BorderObject::BorderObject() {  
	mSpriteID=0;
	mFrame = 0;
	mTileOne = 0;
	mTileTwo = 0;
	mBatch = 0;
	mDepth = 0;
	mPositionRelativeToTileOne = TILE_UP;
	mBlocksMovement = false;
	mExtraMovementCost = 0;
} 

BorderObject::~BorderObject() {
	free(mAssetID);
	//delete mMovementRestrictions;
}

void BorderObject::startActorPassthrough(Actor* actor) {
	mBatch->selectSpriteId(mSpriteID);
	mBatch->setSpriteVisible(false);
}
void BorderObject::endActorPassthrough(Actor* actor) {
	mBatch->selectSpriteId(mSpriteID);
	mBatch->setSpriteVisible(true);
}

void BorderObject::initializeBorderObject(const char* assetID, const U32 frame, Point2D* center, const char* logicalPositionArgs, Tile* tileOne, TileRelativePosition positionRelativeToTileOne, 
										  Tile* tileTwo, SpriteBatch* foregroundBatch, bool blocksMovement, F32 extraMovementCost) {
	mAssetID = (char*)malloc(strlen(assetID)+1 * sizeof(char));
	strcpy(mAssetID, assetID);
	mFrame = frame;
	mCenter = new Point2D(center->x, center->y);
	mLogicalPosition = new SpriteBatchItem::LogicalPosition(logicalPositionArgs);
	mTileOne = tileOne;
	mTileTwo = tileTwo; 
	mPositionRelativeToTileOne = positionRelativeToTileOne;

	mBlocksMovement = blocksMovement;
	mExtraMovementCost = extraMovementCost;
	//initializeBorderProperties();
	
	mDepth = (F32)(tileOne->mLogicalY) - (F32)(tileOne->mLogicalX);
	switch(mPositionRelativeToTileOne) {
		// 0 3 2 1
		// Doubled when doubling other depth
		case TILE_UP: mDepth += 0.5; break;
		case TILE_LEFT: mDepth += 0.5; break; // 
		case TILE_DOWN: mDepth -= 0.5; break; // this one is 1 - 1 - 0.5 = -0.5
		case TILE_RIGHT: mDepth -= 0.5; break;
	}
	//ss << "Initialized wall " << tileOne->mLogicalX << "," << tileOne->mLogicalY << " ; ";
	//ss << "RelPosition = " << positionRelativeToTileOne << "; depth is " << mDepth;
	//Con::printf(ss.str().c_str());
	//if(tileTwo != 0) mDepth = (mDepth + ((F32)tileTwo->mLogicalY - (F32)tileTwo->mLogicalX)) / 2.0;

	//mDepth = -100;
	mTileOne->addNeighboringBorderObject(this, positionRelativeToTileOne, foregroundBatch, mBlocksMovement, mExtraMovementCost);
	if(mTileTwo != 0) mTileTwo->addNeighboringBorderObject(this, mTileTwo->GetOppositePosition(positionRelativeToTileOne), foregroundBatch, mBlocksMovement, mExtraMovementCost);
	
}

void BorderObject::destroyBorderObject() {
	removeFromBatch();
	if(mTileOne != 0) mTileOne->removeNeighboringBorderObject(mPositionRelativeToTileOne);
	if(mTileTwo != 0) mTileTwo->removeNeighboringBorderObject(mTileTwo->GetOppositePosition(mPositionRelativeToTileOne));
}

void BorderObject::addToBatch(SpriteBatch* batch) {
	if(mBatch != 0) return; // Already in a batch
	mBatch = batch;

	mSpriteID = mBatch->addSprite(*mLogicalPosition);
	mBatch->setSpriteDataObject(this);
	mBatch->setSpriteImage(mAssetID,mFrame);
	mBatch->setSpriteDepth(mDepth);
}

void BorderObject::removeFromBatch() {
	if(mBatch == 0) return;

	mBatch->selectSpriteId(mSpriteID);
	mBatch->removeSprite();
	mBatch = 0;
	mSpriteID = 0;
}
  
void BorderObject::initPersistFields()  
{  
    // Call parent.  
    Parent::initPersistFields();  
  
    // Add my fields here.  
	//addField("visualWall", TypeSimObjectPtr, Offset(mVisualWall, Wall), "Visual tile associated with this tile in the WallGrid.");
} 