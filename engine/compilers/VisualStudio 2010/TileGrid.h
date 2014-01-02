#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif  
#ifndef _TILE_H_
#include "Tile.h"
#endif

#ifndef _TILE_GRID_H_
#define _TILE_GRID_H_


#include <string>
#include <sstream>

class TileGrid : public SimObject  
    {  
    private:  
        typedef SimObject Parent;  
      
			bool mDisplayed;
			float mStrideX, mStrideY;
			int mNumDisplayedTilesFromCenterX;
			int mNumDisplayedTilesFromCenterY;

			int mMinDisplayedX;
			int mMinDisplayedY;



			int mMaxDisplayedX;
			int mMaxDisplayedY;
		
			inline void removeRow(const int y, const int minCol, const int maxCol) {
				for(int x=minCol; x <= maxCol; x++) mTiles[index(x,y)].removeFromSpriteBatch(mCompositeSprite);
			}
			inline void removeColumn(const int x, const int minRow, const int maxRow) {
				for(int y=minRow; y <= maxRow; y++) mTiles[index(x,y)].removeFromSpriteBatch(mCompositeSprite);
			}
			inline void addRow(const int y, const int minCol, const int maxCol) {
				for(int x=minCol; x <= maxCol; x++) mTiles[index(x,y)].addToSpriteBatch(mCompositeSprite);
			}
			inline void addColumn(const int x, const int minRow, const int maxRow) {
				for(int y=minRow; y <= maxRow; y++) mTiles[index(x,y)].addToSpriteBatch(mCompositeSprite);
			}
		
	protected:

    public:  
		int mCenterX;
		int mCenterY;
		
		F32 mSpriteOffsetX;
		F32 mSpriteOffsetY;

		SceneWindow* mSceneWindow;
		void updateWindowCenter();

		Tile* mTiles;
		CompositeSprite* mCompositeSprite;
		inline int index(const int x, const int y) { return (y*mSizeX)+x; }
		inline bool isValidLocation(const int x, const int y) { return (x > 0 && y > 0 && x <= mSizeX && y <= mSizeY && (x*y) <= maxIndex()); }

		inline int maxIndex() { return (mSizeX * mSizeY) - 1; }
		int mSizeX, mSizeY;
		TileGrid();
		void resizeGrid(const int x, const int y);
        TileGrid(const int x, const int y);
		void setTile(const int x, const int y, const char* tileAssetID, const U32 frame, const char* logicalPositionArgs);
		void updateTile(const int x, const int y, const char* tileAssetID, const U32 frame);
		void spinTile(const int x, const int y);
		inline void setDefaultSpriteStride( const Vector2& defaultStride ) { 
			mStrideX = defaultStride.x; mStrideY = defaultStride.y; if(mCompositeSprite) mCompositeSprite->setDefaultSpriteStride(defaultStride); }
		// Returns true if valid coordinates
		
		Tile* getTile(const int x, const int y);

		bool getLogicalCoordinates(const F32 worldX, const F32 worldY, int& logicalX, int& logicalY, bool constrainWithinGrid);

		void setDisplayableSize(const int x, const int y);
		void setDisplayCenter(const int x, const int y);
		void setDisplayed(bool displayed);
        ~TileGrid(); 
        virtual bool onAdd();  
        virtual void onRemove();  
          
        virtual void copyTo(SimObject* object);  
          
        static void initPersistFields();  
          
        DECLARE_CONOBJECT( TileGrid );  
    };  

ConsoleMethod(TileGrid, setWindowSize, void, 4, 4, "(float sizeX, [float sizeY]]) - Sets the number of tiles loaded into the composite sprite at one time.\n"
																	"@param sizeX The number of tiles per row to load.\n"
																	"@param sizeY The number of tiles per column to load.\n"
																	"@return No return value.") {
	
	object->setDisplayableSize(std::max(1,dAtoi(argv[2])),std::max(1,dAtoi(argv[3])));

}

ConsoleMethod(TileGrid, setWindowCenter, void, 4, 4, "(float sizeX, [float sizeY]]) - Sets the center of the batch of tiles loaded into the composite sprite at one time.\n"
																	"@param sizeX The number of tiles per row to load.\n"
																	"@param sizeY The number of tiles per column to load.\n"
																	"@return No return value.") {
	
	object->setDisplayCenter(std::max(0,dAtoi(argv[2])),std::max(0,dAtoi(argv[3])));

}

ConsoleMethod(TileGrid, setSpriteOffset, void, 4, 4, "(float sizeX, [float sizeY]]) - Sets the center of the batch of tiles loaded into the composite sprite at one time.\n"
																	"@param sizeX The number of tiles per row to load.\n"
																	"@param sizeY The number of tiles per column to load.\n"
																	"@return No return value.") {
	
	object->mSpriteOffsetX = dAtof(argv[2]);
	object->mSpriteOffsetY = dAtof(argv[3]);

}

ConsoleMethod(TileGrid, updateWindowCenter, void, 2, 2, "Updates the center of the tile grid.\n"
																	"@return No return value.") {
	object->updateWindowCenter();

}

ConsoleMethod(TileGrid, initializeGridTile, void, 7, 7,  "(float strideX, [float strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                                    "@param tileX The x position of the new tile.\n"
                                                                    "@param tileY The y position of the new tile.\n"
																	"@param tileAssetID The asset ID of the tile.\n"
																	"@param frame The frame of the new tile.\n"
																	"@param logicalPositionArgs The logical position of the new tile.\n"
                                                                    "@return No return value.") {
	object->setTile(std::max(0,dAtoi(argv[2])),std::max(0,dAtoi(argv[3])),argv[4],dAtoi(argv[5]),argv[6]);
}

ConsoleMethod(TileGrid, updateGridTile, void, 6, 6,  "(float strideX, [float strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                                    "@param tileX The x position of the new tile.\n"
                                                                    "@param tileY The y position of the new tile.\n"
																	"@param tileAssetID The asset ID of the tile.\n"
																	"@param frame The frame of the new tile.\n"
                                                                    "@return No return value.") {
	object->updateTile(std::max(0,dAtoi(argv[2])),std::max(0,dAtoi(argv[3])),argv[4],dAtoi(argv[5]));
}

ConsoleMethod(TileGrid, spinGridTile, void, 4, 4,  "(float strideX, [float strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                                    "@param tileX The x position of the new tile.\n"
                                                                    "@param tileY The y position of the new tile.\n"
                                                                    "@return No return value.") {
	object->spinTile(std::max(0,dAtoi(argv[2])),std::max(0,dAtoi(argv[3])));
}

ConsoleMethod(TileGrid, resizeGrid, void, 4, 4,  "(float strideX, [float strideY]]) - Sets the number of tiles on the grid.\n"
                                                                    "@param tileX The new number of tiles, X.\n"
                                                                    "@param tileY The new number of tiles, Y.\n"
                                                                    "@return No return value.") {
	object->resizeGrid(std::max(1,dAtoi(argv[2])),std::max(1,dAtoi(argv[3])));
}

ConsoleMethod(TileGrid, clearSprites, void, 2, 2,    "() - Removes all sprites.\n"
                                                            "@return No return value." )
{
	object->setDisplayed(false);
    //for(int i=0; i <= object->maxIndex(); i++) {
//		object->mTiles[i].removeFromSpriteBatch(object->mCompositeSprite);
//	}
	//return;
}

ConsoleMethod(TileGrid, addSprites, void, 2, 2,    "() - Adds all sprites.\n"
                                                            "@return No return value." )
{
	object->setDisplayed(true);
    std::stringstream ss;
	//ss.str("Setting to: ");
	//ss << "<Size = " << object->mSizeX << " , " << object->mSizeY << ">, so eval up to " << (((object->mSizeY) * (object->mSizeX))-1);

	//std::string s = ss.str();

	//Con::printf(s.c_str());
	//for(int i=0; i <= object->maxIndex(); i++)
			//object->mTiles[i].addToSpriteBatch(object->mCompositeSprite);
	//for(int i=0; i<(object->mSizeY*(object->mSizeX)); i++) {
			//std::stringstream ss;
			//ss.str("Setting to: ");
			//ss << "<Size = " << object->mSizeX << " , " << object->mSizeY << "> Evaluating " << i;

			//std::string s = ss.str();

			//Con::printf(s.c_str());
		//object->mTiles[i].addToSpriteBatch(object->mCompositeSprite);
	//}
	//return;
}

ConsoleMethod(TileGrid, setDefaultSpriteStride, void, 3, 4,  "(float strideX, [float strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                                    "@param strideX The default stride of the local X axis.\n"
                                                                    "@param strideY The default stride of the local Y axis.\n"
                                                                    "@return No return value.")
{
    Vector2 stride;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("strideX strideY")
    if ( (elementCount == 2) && (argc == 3) )
    {
        stride.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        stride.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (strideX, [strideY])
    else if (elementCount == 1)
    {
        stride.x = dAtof(argv[2]);

        if (argc > 3)
            stride.y = dAtof(argv[3]);
        else
            stride.y = stride.x;
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setDefaultSpriteStride() - Invalid number of parameters!");
        return;
    }

    object->setDefaultSpriteStride( stride );
}

#endif