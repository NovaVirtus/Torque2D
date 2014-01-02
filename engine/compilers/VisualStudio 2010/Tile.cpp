#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "Tile.h"
IMPLEMENT_CONOBJECT(Tile);  

void Tile::copyTo( SimObject* object )  
{  
    // Fetch other object.  
   Tile* pTile = static_cast<Tile*>( object );  
  
   // Sanity!  
   AssertFatal( pTile != NULL, "Tile::copyTo() - Object is not the correct type.");  
  
   // Copy parent.  
   Parent::copyTo( object );  
  
   // Copy the state. 
} 

bool Tile::onAdd()  
{  
    // Fail if the parent fails.  
    if ( !Parent::onAdd() )  
        return false;  
  
    // Do some work here.  
    Con::printf("Hello from Tile!");  
	return true;
}  
  
void Tile::onRemove()  
{  
    // Do some work here.  
} 

Tile::Tile() {  
	mSpriteID=0;
} 

Tile::~Tile() {
	free(mTileAssetID);
}

void Tile::initializeTile(const char* tileAssetID, const U32 frame, const char* logicalPositionArgs, const Vector2& center) {
	mTileAssetID = (char*)malloc(strlen(tileAssetID)+1 * sizeof(char));
	strcpy(mTileAssetID, tileAssetID);
	mFrame = frame;
	//Con::printf("Initialized tile with value:");
	//Con::printf(logicalPositionArgs);
	//Con::printf("Initial tile asset ID");
	//Con::printf(tileAssetID);
	//Con::printf("Copied tile asset ID");
	//Con::printf(mTileAssetID);
	mLogicalPosition = new SpriteBatchItem::LogicalPosition(logicalPositionArgs);
	mCenter = new Vector2();
	mCenter->x = center.x;
	mCenter->y = center.y;
	//strcpy(mArgs, logicalPositionArgs);
}

void Tile::updateTile(const char* tileAssetID, const U32 frame, SpriteBatch* batch) {
	if(mTileAssetID) free(mTileAssetID);
	mTileAssetID = (char*)malloc(strlen(tileAssetID)+1 * sizeof(char));
	strcpy(mTileAssetID, tileAssetID);
	mFrame = frame;
	
	if(mSpriteID != 0) {
		batch->selectSpriteId(mSpriteID);
		batch->setSpriteImage(tileAssetID, frame);
	}
}

void Tile::spinTile(SpriteBatch* batch) {
	if(mSpriteID == 0) return;
	//std::stringstream ss;
	//ss.str("");
	//ss << "Setting frame from " << mFrame;
	mFrame = (mFrame+1)%4;
	//ss << " to " << mFrame;
	//Con::printf(ss.str().c_str());
	batch->selectSpriteId(mSpriteID);
	batch->setSpriteImageFrame(mFrame);
}

void Tile::addToSpriteBatch(SpriteBatch* batch) {
	if(mSpriteID != 0) return; // Already present

	mSpriteID = batch->addSprite(*mLogicalPosition);
	batch->setSpriteDataObject(this);
	batch->setSpriteImage(mTileAssetID,mFrame);
}

void Tile::removeFromSpriteBatch(SpriteBatch* batch) {
	if (mSpriteID==0) return;

	batch->selectSpriteId(mSpriteID);
	batch->removeSprite();
	mSpriteID=0;
}
  
void Tile::initPersistFields()  
{  
    // Call parent.  
    Parent::initPersistFields();  
  
    // Add my fields here.  
	//addField("visualTile", TypeSimObjectPtr, Offset(mVisualTile, Tile), "Visual tile associated with this tile in the TileGrid.");
} 