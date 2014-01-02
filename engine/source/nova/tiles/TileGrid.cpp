#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "2d/sceneobject/CompositeSprite.h"
#include "TileGrid.h"

IMPLEMENT_CONOBJECT(TileGrid);  

void TileGrid::copyTo(SimObject* object) {  
    // Fetch other object.  
   TileGrid* pTileGrid = static_cast<TileGrid*>( object );  
  
   // Sanity!  
   AssertFatal(pTileGrid != NULL, "TileGrid::copyTo() - Object is not the correct type.");  
  
   // Copy parent.  
   Parent::copyTo( object );  
  
   // Copy the state.  

   //pTileGrid->mEmitLight = mEmitLight;  
   //pTileGrid->mBrightness = mBrightness;  
} 

bool TileGrid::onAdd()  {  
    // Fail if the parent fails.  
    if (!Parent::onAdd())  
        return false;  
  
    // Do some work here.  
    Con::printf("Hello from TileGrid!");  
	return true;
}  
  
void TileGrid::onRemove() {  
	// Do some work here.  
} 

TileGrid::TileGrid() {
	mDisplayed = false;
	resizeGrid(5,5);
	setDisplayableSize(3,3);
	mStrideX = 1;
	mStrideY = 1;
	mSpriteOffsetX = 0;
	mSpriteOffsetY = 0;
}
TileGrid::TileGrid(const int x, const int y) {  
	mDisplayed = false;
	Con::printf("Hello from TileGrid! Initializing...");  
	setDisplayableSize(3,3);
	resizeGrid(x,y);
	//mTiles = new Tile[x*y];
	mSizeX = x;
	mSizeY = y;
	mStrideX = 1;
	mStrideY = 1;
	mSpriteOffsetX = 0;
	mSpriteOffsetY = 0;
}  

void TileGrid::resizeGrid(const int x, const int y) {
	std::stringstream ss;
	//ss.str("");
	ss << "Tile grid resize, setting to: " << x << "," << y;
	Con::printf(ss.str().c_str());
	
	bool isDisplayed = mDisplayed;
	if(isDisplayed) setDisplayed(false);
	ss.str("");
	ss << "Set to not visible";
	Con::printf(ss.str().c_str());
	mTiles = new Tile[x*y];
	mSizeX = x;
	mSizeY = y;
	ss.str("");
	ss << "Made tile grid";
	Con::printf(ss.str().c_str());
	setDisplayCenter((x/2),(y/2)); // Look at the center of the grid
	ss.str("");
	ss << "Set display center";
	Con::printf(ss.str().c_str());
	setDisplayed(isDisplayed);
	ss.str("");
	ss << "Set to displayed: " << isDisplayed;
	Con::printf(ss.str().c_str());
}


TileGrid::~TileGrid() {
	delete mTiles;
}

void TileGrid::setTile(const int x, const int y, const char* tileAssetID, const U32 frame, const char* logicalPositionArgs) {

	//Con::printf("Setting to:");
	//std::stringstream ss;
	//ss.str("Setting to: ");
	//ss << "<Size = " << mSizeX << " , " << mSizeY << "> " << index(x,y) << " | " << tileAssetID << " | " << frame << " | " << logicalPositionArgs << " |";

	//std::string s = ss.str();

	
	//Con::printf(s.c_str());
	Vector2 center;

	center.x = ((F32)(x+y-1.0)) * mStrideX;
	center.y = ((F32)(y-x-2.0)) * mStrideY;
	// Trying to fix X offset
	center.x += mStrideX * 2;
	//center.y += mStrideY;

	//std::stringstream ss;
	//ss << "Set tile " << x << "," << y << " -> center = " << center.x << "," << center.y;
	//Con::printf(ss.str().c_str());
	mTiles[index(x,y)].initializeTile(tileAssetID, frame, logicalPositionArgs, center);
}

void TileGrid::updateTile(const int x, const int y, const char* tileAssetID, const U32 frame) {
	if(x < 0 || y < 0 || x > mSizeX || y > mSizeY || (x*y) > maxIndex()) return;
	mTiles[index(x,y)].updateTile(tileAssetID, frame, mCompositeSprite);
}

void TileGrid::spinTile(const int x, const int y) {
	if(x < 0 || y < 0 || x > mSizeX || y > mSizeY || (x*y) > maxIndex()) return;
	mTiles[index(x,y)].spinTile(mCompositeSprite);
}

void TileGrid::setDisplayableSize(const int x, const int y) {
	// Can truncate the remainder...
	mNumDisplayedTilesFromCenterX = (int)(x / 2);
	mNumDisplayedTilesFromCenterY = (int)(y / 2);
}

void TileGrid::setDisplayed(bool displayed) {
	if(displayed==mDisplayed) return;
	std::stringstream ss;
	ss.str("");
	ss << "Displaying (" << displayed << "): " << mMinDisplayedX << ":" << mMaxDisplayedX << ", " << mMinDisplayedY << ":" << mMaxDisplayedY;
	Con::printf(ss.str().c_str());
	if(displayed) {
		for(int x = mMinDisplayedX; x <= mMaxDisplayedX; x++) addColumn(x, mMinDisplayedY, mMaxDisplayedY);
	} else {
		for(int x = mMinDisplayedX; x <= mMaxDisplayedX; x++) removeColumn(x, mMinDisplayedY, mMaxDisplayedY);
	}

	mDisplayed=displayed;
}

void TileGrid::setDisplayCenter(const int x, const int y) {
	if(!mDisplayed) {
		mCenterX = x;
		mCenterY = y;
		mMinDisplayedX=std::max(0,x-mNumDisplayedTilesFromCenterX);
		mMinDisplayedY=std::max(0,y-mNumDisplayedTilesFromCenterY);
	
		mMaxDisplayedX=std::min(mSizeX-1,x+mNumDisplayedTilesFromCenterX);
		mMaxDisplayedY=std::min(mSizeY-1,y+mNumDisplayedTilesFromCenterY);
	} else {
		if ((x==mCenterX) && (y==mCenterY)) return;

		int newMinX;
		int newMinY;
		int newMaxX;
		int newMaxY;

		newMinX=std::max(0,x-mNumDisplayedTilesFromCenterX);
		newMinY=std::max(0,y-mNumDisplayedTilesFromCenterY);
	
		newMaxX=std::min(mSizeX-1,x+mNumDisplayedTilesFromCenterX);
		newMaxY=std::min(mSizeY-1,y+mNumDisplayedTilesFromCenterY);

		if(newMinX < mMinDisplayedX) {
			for(int curColumn = newMinX; curColumn < (std::min(mMinDisplayedX,newMaxX)); curColumn++) addColumn(curColumn, newMinY, newMaxY); // Don't include mMinDisplayedX because it's already present
		} else if(newMinX > mMinDisplayedX) {
			for(int curColumn = mMinDisplayedX; curColumn < newMinX; curColumn++) removeColumn(curColumn, mMinDisplayedY, mMaxDisplayedY); // Don't include new minX because don't want to remove it
		}
		// Max value increased: add columns
		if(newMaxX > mMaxDisplayedX) {
			for(int curColumn = std::max(newMinX,mMaxDisplayedX+1); curColumn <= newMaxX; curColumn++) addColumn(curColumn, newMinY, newMaxY);
		} else if(newMaxX < mMaxDisplayedX) { // Max value decreased: remove columns
			for(int curColumn = newMaxX + 1; curColumn <= mMaxDisplayedX; curColumn++) removeColumn(curColumn, mMinDisplayedY, mMaxDisplayedY);
		}

		if(newMinY < mMinDisplayedY) {
			for(int curRow = newMinY; curRow < std::min(newMaxY+1,mMinDisplayedY); curRow++) addRow(curRow, newMinX, newMaxX); 
		} else if(newMinY > mMinDisplayedY) {
			for(int curRow = mMinDisplayedY; curRow < newMinY; curRow++) removeRow(curRow, mMinDisplayedX, mMaxDisplayedX);
		}

		// Max value increased: add rows
		if(newMaxY > mMaxDisplayedY) {
			for(int curRow = std::max(newMinY,mMaxDisplayedY+1); curRow <= newMaxY; curRow++) addRow(curRow, newMinX, newMaxX);
		} else if(newMaxY < mMaxDisplayedY) { // Max value decreased: remove rows
			for(int curRow = newMaxY + 1; curRow <= mMaxDisplayedY; curRow++) removeRow(curRow, mMinDisplayedX, mMaxDisplayedX);
		}
		


		mCenterX = x;
		mCenterY = y;

		mMinDisplayedX=newMinX;
		mMinDisplayedY=newMinY;
		mMaxDisplayedX=newMaxX;
		mMaxDisplayedY=newMaxY;
	}
}

void TileGrid::updateWindowCenter() {

		
		if(mSceneWindow) {

			F32 x;
			F32 y;
			int logicalX, logicalY;

			mSceneWindow->getWindowCenter(x,y);
			x = x + mSpriteOffsetX;
			y = y + mSpriteOffsetY;
			
			getLogicalCoordinates(x,y,logicalX,logicalY,true);

			//spinTile(logicalX,logicalY);
			setDisplayCenter(logicalX,logicalY);

		} else Con::printf("No scene window mapped!"); 
}

Tile* TileGrid::getTile(const int x, const int y)  {
	Tile* t = &mTiles[index(x,y)];
	return t;
}
bool TileGrid::getLogicalCoordinates(const F32 worldX, const F32 worldY, int& logicalX, int& logicalY, bool constrainWithinGrid) {
	F32 worldXOverStrideX = (worldX / mStrideX);
	F32 worldYOverStrideY = (worldY / mStrideY);

	logicalY = (int)((worldXOverStrideX+worldYOverStrideY) / 2);
	logicalX = (int)((worldXOverStrideX)-logicalY);
			
	F32 logicalXWorldCenter = (logicalX+logicalY) * mStrideX;
	F32 logicalYWorldCenter = (logicalY-logicalX) * mStrideY + (mStrideY / 2);

	F32 dispXPercentFromCenter = (worldX - logicalXWorldCenter) / mStrideX;
	F32 dispYPercentFromCenter = (worldY - logicalYWorldCenter) / mStrideY;
			
	//ss.str("");
	//ss << "LogicalCoordinates: In column/row " << logicalX << "," << logicalY << " for coordinates " << worldX << "," << worldY;
	//Con::printf(ss.str().c_str());
	//ss.str("");
	//ss << "LogicalCoordinates: X from center = " << dispXPercentFromCenter << " Y from center = " << dispYPercentFromCenter;
	//Con::printf(ss.str().c_str());

	if(std::abs(dispXPercentFromCenter) + std::abs(dispYPercentFromCenter) > 1) {
		if(dispXPercentFromCenter > 0) {
			if(dispYPercentFromCenter > 0) logicalY++;
			else logicalX++;
		}
		else {
			if(dispYPercentFromCenter > 0) logicalX--;
			else logicalY--;
		}
	}

	if(constrainWithinGrid) {
		if(logicalX >= mSizeX) logicalX = mSizeX - 1;
		else if(logicalX < 0) logicalX = 0;

		if(logicalY >= mSizeY) logicalY = mSizeY - 1;
		else if(logicalY < 0) logicalY = 0;
					
		return true;
	}
	else {
		return(isValidLocation(logicalX, logicalY));
	}
}

void TileGrid::initPersistFields() {  
    // Call parent.  
    Parent::initPersistFields();  
	
	addField("SceneWindow",TypeSimObjectPtr, Offset(mSceneWindow, TileGrid), "Scene window viewing this tile grid.");
	addField("CompositeSprite",TypeSimObjectPtr, Offset(mCompositeSprite, TileGrid), "Composite sprite to use for this tile grid.");
    // Add my fields here.  
   //addField("EmitLight", TypeBool, Offset(mEmitLight, TileGrid), "Flags whether the light is on or off.");  
    //addField("Brightness", TypeF32, Offset(mBrightness, TileGrid), "Sets the brightness of the light.");   
}                      