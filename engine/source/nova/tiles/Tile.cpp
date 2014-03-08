#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "Tile.h"
IMPLEMENT_CONOBJECT(Tile);  


void Tile::copyTo(SimObject* object) {  
    // Fetch other object.  
   Tile* pTile = static_cast<Tile*>( object );  
  
   // Sanity!  
   AssertFatal( pTile != NULL, "Tile::copyTo() - Object is not the correct type.");  
  
   // Copy parent.  
   Parent::copyTo( object );  
  
   // Copy the state. 
} 

bool Tile::onAdd() {
    // Fail if the parent fails.  
    if ( !Parent::onAdd() )  
        return false;  
  
    // Do some work here.  
    Con::printf("Hello from Tile!");  
	return true;
}  
  
void Tile::onRemove() {
    // Do some work here.  
} 

Tile::Tile() {  
	mBatch = 0;
	mSpriteID = 0;
	mCostPast = 0;
	mCostFuture = 0;
	mLogicalX = 0;
	mLogicalY = 0;
	mInClosedSet = false;
	mFrame = 0;
	mExtraMovementCost = 1;
	mMovementRestrictions = new bool[4];// = new bool[4]; // 0 -> y+1, 1 -> x+1, 2 -> y-1, 3 -> x-1
	mNeighboringWalls[TILE_UP] = 0;
	mNeighboringWalls[TILE_LEFT] = 0;
	mNeighboringWalls[TILE_RIGHT] = 0;
	mNeighboringWalls[TILE_DOWN] = 0;
	updateMoveCostsAndRestrictions();
} 

Tile::~Tile() {
	free(mTileAssetID);
	delete mMovementRestrictions;
}

void Tile::updateMoveCostsAndRestrictions() {
	mExtraMovementCost = (F32)(mFrame == 2? 1 : 2);
	//if(mFrame == 2) { // Empty tile
	if(1 == 0) {
		mMovementRestrictions[TILE_UP] = false;
		mMovementRestrictions[TILE_DOWN] = false;
		mMovementRestrictions[TILE_RIGHT] = true;
		mMovementRestrictions[TILE_LEFT] = true; // Can only go left
	} else { // Was all false before
		mMovementRestrictions[TILE_UP] = (mNeighboringWalls[TILE_UP] != 0); 
		mMovementRestrictions[TILE_DOWN] = (mNeighboringWalls[TILE_DOWN] != 0);
		mMovementRestrictions[TILE_RIGHT] = (mNeighboringWalls[TILE_RIGHT] != 0);
		mMovementRestrictions[TILE_LEFT] = (mNeighboringWalls[TILE_LEFT] != 0);
	}
}

void Tile::initializeTile(const char* tileAssetID, const U32 frame, const char* logicalPositionArgs, const Vector2& center, const U32 logicalX, const U32 logicalY) {
	mTileAssetID = (char*)malloc(strlen(tileAssetID)+1 * sizeof(char));
	strcpy(mTileAssetID, tileAssetID);
	mFrame = frame;
	mLogicalPosition = new SpriteBatchItem::LogicalPosition(logicalPositionArgs);
	mCenter = new Point2D(center.x, center.y);
	mLogicalX = logicalX;
	mLogicalY = logicalY;
	updateMoveCostsAndRestrictions();
}

void Tile::updateTile(const char* tileAssetID, const U32 frame) {
	if(mTileAssetID) free(mTileAssetID);
	mTileAssetID = (char*)malloc(strlen(tileAssetID)+1 * sizeof(char));
	strcpy(mTileAssetID, tileAssetID);
	mFrame = frame;
	
	if(mBatch != 0) {
		mBatch->selectSpriteId(mSpriteID);
		mBatch->setSpriteImage(mTileAssetID, mFrame);
	}
	updateMoveCostsAndRestrictions();
}

void Tile::spinTile() {
	if(mBatch == 0) return;
	mFrame = (mFrame+1) % 4;
	mBatch->selectSpriteId(mSpriteID);
	mBatch->setSpriteImageFrame(mFrame);

	updateMoveCostsAndRestrictions();
}

void Tile::addToSpriteBatch(SpriteBatch* batch, SpriteBatch* foregroundBatch) {
	if(mBatch != 0) return; // Already present
	mBatch = batch;
	mSpriteID = mBatch->addSprite(*mLogicalPosition);
	mBatch->setSpriteDataObject(this);
	mBatch->setSpriteImage(mTileAssetID,mFrame);
	mBatch->setSpriteDepth((F32)mLogicalX - (F32)mLogicalY);
	if(mNeighboringWalls[TILE_UP] || mNeighboringWalls[TILE_DOWN] || mNeighboringWalls[TILE_LEFT] || mNeighboringWalls[TILE_RIGHT]) {
		Con::printf("-----Adding neighbors");
		if(mNeighboringWalls[TILE_UP]) { mNeighboringWalls[TILE_UP]->addToBatch(foregroundBatch); Con::printf("Adding one tile to batch (up)"); }
		if(mNeighboringWalls[TILE_DOWN]) { mNeighboringWalls[TILE_DOWN]->addToBatch(foregroundBatch); Con::printf("Adding one tile to batch (down)"); }
		if(mNeighboringWalls[TILE_LEFT]) { mNeighboringWalls[TILE_LEFT]->addToBatch(foregroundBatch); Con::printf("Adding one tile to batch (left)"); }
		if(mNeighboringWalls[TILE_RIGHT])  { mNeighboringWalls[TILE_RIGHT]->addToBatch(foregroundBatch); Con::printf("Adding one tile to batch (right)"); }
		Con::printf("-----Added neighbors");
	}
}

void Tile::removeFromSpriteBatch() {
	if(mBatch == 0) return;
	// This technically means that the walls will be in-view if we're moving towards a tile and the tile comes into view,
	// but not in view if we're moving away from a tile and one of the bordering tiles comes into view.
	// That's okay! It should be well off screen in either case, and it'll get re-added before it's important in this case.
	// The extra checks to avoid this aren't worth the time, as a result.
	if(mNeighboringWalls[TILE_UP]) mNeighboringWalls[TILE_UP]->removeFromBatch();
	if(mNeighboringWalls[TILE_DOWN]) mNeighboringWalls[TILE_DOWN]->removeFromBatch();
	if(mNeighboringWalls[TILE_LEFT]) mNeighboringWalls[TILE_LEFT]->removeFromBatch();
	if(mNeighboringWalls[TILE_RIGHT]) mNeighboringWalls[TILE_RIGHT]->removeFromBatch();

	mBatch->selectSpriteId(mSpriteID);
	mBatch->removeSprite();
	mSpriteID = 0;
	mBatch = 0;
}
  
void Tile::addNeighboringWall(Wall* wall, TileRelativePosition relativePosition, SpriteBatch* foregroundBatch) {
			// Should not need to check for existence of an existing wall here; this should be handled elsewhere if necessary
			mNeighboringWalls[relativePosition] = wall;
			if(mBatch != 0) { // If we're being displayed, add the wall to foreground batch
				wall->addToBatch(foregroundBatch);
			}
			mMovementRestrictions[relativePosition] = true;
		}

void Tile::initPersistFields() {  
    // Call parent.  
    Parent::initPersistFields();  
  
    // Add my fields here.  
	//addField("visualTile", TypeSimObjectPtr, Offset(mVisualTile, Tile), "Visual tile associated with this tile in the TileGrid.");
} 