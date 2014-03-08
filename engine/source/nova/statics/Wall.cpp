#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "Wall.h"
IMPLEMENT_CONOBJECT(Wall);  

void Wall::copyTo(SimObject* object) {  
    // Fetch other object.  
   Wall* pWall = static_cast<Wall*>( object );  
  
   // Sanity!  
   AssertFatal( pWall != NULL, "Wall::copyTo() - Object is not the correct type.");  
  
   // Copy parent.  
   Parent::copyTo( object );  
  
   // Copy the state. 
} 

bool Wall::onAdd() {
    // Fail if the parent fails.  
    if ( !Parent::onAdd() )  
        return false;  
  
    // Do some work here.  
    Con::printf("Hello from Wall!");  
	return true;
}  
  
void Wall::onRemove() {
    // Do some work here.  
} 

Wall::Wall() {  
	mSpriteID=0;
	mFrame = 0;
	mTileOne = 0;
	mTileTwo = 0;
	mBatch = 0;
	mDepth = 0;
	mPositionRelativeToTileOne = TILE_UP;
} 

Wall::~Wall() {
	free(mAssetID);
	//delete mMovementRestrictions;
}

void Wall::initializeWall(const char* assetID, const U32 frame, Point2D* center, const char* logicalPositionArgs, Tile* tileOne, TileRelativePosition positionRelativeToTileOne, Tile* tileTwo, SpriteBatch* foregroundBatch) {
	mAssetID = (char*)malloc(strlen(assetID)+1 * sizeof(char));
	strcpy(mAssetID, assetID);
	mFrame = frame;
	mCenter = new Point2D(center->x, center->y);
	mLogicalPosition = new SpriteBatchItem::LogicalPosition(logicalPositionArgs);
	mTileOne = tileOne;
	mTileTwo = tileTwo; 
	mPositionRelativeToTileOne = positionRelativeToTileOne;

	//std::stringstream ss;
	
	//if(tileTwo != 0) ss << tileTwo->mLogicalX << "," << tileTwo->mLogicalY;
	

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
	mTileOne->addNeighboringWall(this, positionRelativeToTileOne, foregroundBatch);
	if(mTileTwo != 0) mTileTwo->addNeighboringWall(this, mTileTwo->GetOppositePosition(positionRelativeToTileOne), foregroundBatch);
	
}

void Wall::destroyWall() {
	removeFromBatch();
	if(mTileOne != 0) mTileOne->removeNeighboringWall(mPositionRelativeToTileOne);
	if(mTileTwo != 0) mTileTwo->removeNeighboringWall(mTileTwo->GetOppositePosition(mPositionRelativeToTileOne));
}

void Wall::addToBatch(SpriteBatch* batch) {
	if(mBatch != 0) return; // Already in a batch
	mBatch = batch;

	mSpriteID = mBatch->addSprite(*mLogicalPosition);
	mBatch->setSpriteDataObject(this);
	mBatch->setSpriteImage(mAssetID,mFrame);
	mBatch->setSpriteDepth(mDepth);
}

void Wall::removeFromBatch() {
	if(mBatch == 0) return;

	mBatch->selectSpriteId(mSpriteID);
	mBatch->removeSprite();
	mBatch = 0;
	mSpriteID = 0;
}
  
void Wall::initPersistFields()  
{  
    // Call parent.  
    Parent::initPersistFields();  
  
    // Add my fields here.  
	//addField("visualWall", TypeSimObjectPtr, Offset(mVisualWall, Wall), "Visual tile associated with this tile in the WallGrid.");
} 