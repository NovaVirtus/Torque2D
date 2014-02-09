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
#include "../actors/ActionPlan.h"

using namespace std;

struct TileDistanceCompare {
	bool operator()(Tile* t1, Tile* t2) const {
		//return (t1->lastKnownCost) < (t2->lastKnownCost);
		return (t1->mCostFuture) > (t2->mCostFuture); // Backwards: Want to go from lowest (Distance) to highest
	};
};

class TileGrid : public SimObject  
    {  
    private:  
        typedef SimObject Parent;  
      
			bool mDisplayed;
			
			U32 mNumDisplayedTilesFromCenterX;
			U32 mNumDisplayedTilesFromCenterY;

			U32 mMinDisplayedX;
			U32 mMinDisplayedY;

			U32 mMaxDisplayedX;
			U32 mMaxDisplayedY;
		
			inline U32 constrainU32ToRange(U32 number, S32 offsetValue, U32 min, U32 max) {
				U32 working;
				if(offsetValue >= 0) {
					working = number + offsetValue;
					if(working < number) return max;
					return (working > max ? max : working);
				} else {
					if((U32)(-1 * offsetValue) > number) return min;
					working = number + offsetValue;
					return (working < min ? min : working);
				}
			}

			inline void removeRow(const U32 y, const U32 minCol, const U32 maxCol) {
				for(U32 x=minCol; x <= maxCol; x++) mTiles[index(x,y)].removeFromSpriteBatch(mCompositeSprite);
			}
			inline void removeColumn(const U32 x, const U32 minRow, const U32 maxRow) {
				for(U32 y=minRow; y <= maxRow; y++) mTiles[index(x,y)].removeFromSpriteBatch(mCompositeSprite);
			}
			inline void addRow(const U32 y, const U32 minCol, const U32 maxCol) {
				for(U32 x=minCol; x <= maxCol; x++) mTiles[index(x,y)].addToSpriteBatch(mCompositeSprite);
			}
			inline void addColumn(const U32 x, const U32 minRow, const U32 maxRow) {
				for(U32 y=minRow; y <= maxRow; y++) mTiles[index(x,y)].addToSpriteBatch(mCompositeSprite);
			}
		
	protected:

    public:  
		//void TileGrid::tryAStarAddNeighbor(priority_queue<Tile*, vector<Tile*>, TileDistanceCompare> openSet, Tile* from, Tile* goal, Tile* t);
		inline bool TryToAddNeighbor(Tile* from, S32 offsetX, S32 offsetY, F32 curCostPast, Tile* goal, Tile* & to, F32 fixedCost) {
			Tile* intermediateOne = 0;
			Tile* intermediateTwo = 0;
			if(!isValidLocation(from->mLogicalX + offsetX, from->mLogicalY + offsetY)) return false;
			to = &mTiles[index(from->mLogicalX + offsetX, from->mLogicalY + offsetY)];
			if(offsetX == 2) {
				intermediateOne = &mTiles[index(from->mLogicalX + 1, from->mLogicalY)];
				intermediateTwo = &mTiles[index(from->mLogicalX + 1, from->mLogicalY + offsetY)];
			} else if(offsetX == -2) {
				intermediateOne = &mTiles[index(from->mLogicalX - 1, from->mLogicalY)];
				intermediateTwo = &mTiles[index(from->mLogicalX - 1, from->mLogicalY + offsetY)];
			} else if(offsetY == 2) {
				intermediateOne = &mTiles[index(from->mLogicalX, from->mLogicalY + 1)];
				intermediateTwo = &mTiles[index(from->mLogicalX + offsetX, from->mLogicalY + 1)];
			} else if(offsetY == -2) {
				intermediateOne = &mTiles[index(from->mLogicalX, from->mLogicalY - 1)];
				intermediateTwo = &mTiles[index(from->mLogicalX + offsetX, from->mLogicalY - 1)];
			}
			F32 tentativeScore = curCostPast + from->actualDistance(to, fixedCost, intermediateOne, intermediateTwo);
			if(tentativeScore < to->mCostPast) {
				to->mCameFrom = from;
				to->mCostPast = tentativeScore;
				to->mCostFuture = tentativeScore + to->estimatedDistance(goal);
				return true;
			}
			return false;
		}
		//void tryAStarAddNeighbor(Tile* from, Tile* goal, Tile* t);
		void tryDijkstras();
		void tryAStar(Tile* origin, Tile* goal);
		
		U32 mCenterX, mCenterY;
		U32 mSizeX, mSizeY;
		F32 mStrideX, mStrideY;
		F32 mSpriteOffsetX, mSpriteOffsetY;

		CompositeSprite* mCompositeSprite;
		SceneWindow* mSceneWindow;
		Tile* mTiles;

		TileGrid();
		TileGrid(const U32 x, const U32 y);
		~TileGrid(); 
		
		void resizeGrid(const U32 x, const U32 y);
		void setDisplayableSize(const U32 x, const U32 y);
		void setDisplayCenter(const U32 x, const U32 y);
		void setDisplayed(bool displayed);
		void setTile(const U32 x, const U32 y, const char* tileAssetID, const U32 frame, const char* logicalPositionArgs);
        void spinTile(const U32 x, const U32 y);
		void updateTile(const U32 x, const U32 y, const char* tileAssetID, const U32 frame);
		void updateWindowCenter();

        virtual bool onAdd();  
        virtual void onRemove();  
        virtual void copyTo(SimObject* object);  

		static void initPersistFields();  

		bool getLogicalCoordinates(const F32 worldX, const F32 worldY, U32& logicalX, U32& logicalY, bool constrainWithinGrid);
		bool getRelativeMove(const U32 fromX, const U32 fromY, const S32 offsetX, const S32 offsetY, U32& toX, U32& toY);

        Tile* getTile(const U32 x, const U32 y); 
		
		inline U32 index(const U32 x, const U32 y) { return (y * mSizeX) + x; }
		inline U32 maxIndex() { return (mSizeX * mSizeY) - 1; }

		inline bool isValidLocation(const U32 x, const U32 y) { return ((x >= 0) && (y >= 0) && (x < mSizeX) && (y < mSizeY) && ((x * y) <= maxIndex())); }

		inline void getTileCenter(const U32 logicalX, const U32 logicalY, F32& worldX, F32& worldY) {
			worldX = (F32)((logicalX + logicalY) * mStrideX);
			worldY = (F32)(((S32)logicalY - (S32)logicalX + 0.5) * mStrideY);
		}

		inline void setDefaultSpriteStride( const Vector2& defaultStride ) { 
			mStrideX = defaultStride.x; 
			mStrideY = defaultStride.y; 
			if(mCompositeSprite) mCompositeSprite->setDefaultSpriteStride(defaultStride); 
		}

		inline ActionPlan* recurrentPrependStep(const U32 fromX, const U32 fromY, const U32 toX, const U32 toY) {
			if(fromX == toX && fromY == toY) return 0;
			
			U32 newX, newY;
			U32 differenceX = fromX - toX;
			U32 differenceY = fromY - toY;

			if(differenceX > 1) differenceX = 1;
			if(differenceY > 1) differenceY = 1;
			if(differenceX < -1) differenceX = -1;
			if(differenceY < -1) differenceY = -1;

			newX = toX - differenceX;
			newY = toY - differenceY;
			ActionPlan* head = new ActionPlan(newX, newY);
			//ActionPlan* head = new ActionPlan(recurrentPrependStep(fromX, fromY, newX, newY), newX, newY);
			return head;
		}

		inline void debugSpecifiedActionPlan(ActionPlan* current) {
			std::stringstream ss;
			ss << "Action plan = ";
			for( ; current != 0; current = current->nextStep) {
				Tile* t = getTile(current->x, current->y);
				Vector2* position = t->mCenter;
				ss << "<Logical" << current->x << "," << current->y << " -> World" << position->x << "," << position->y << "> ";
			}
			Con::printf(ss.str().c_str());
		}


		inline ActionPlan* getPathToTarget(const U32 fromX, const U32 fromY, const U32 toX, const U32 toY) {
			if(!isValidLocation(toX, toY)) return 0;
			tryAStar(&mTiles[index(fromX, fromY)], &mTiles[index(toX, toY)]);
			ActionPlan* next = new ActionPlan(toX, toY);
			ActionPlan* head = next;

			Tile* t = mTiles[index(toX, toY)].mCameFrom;
			while(t != 0) {
				if(t->mLogicalX == fromX && t->mLogicalY == fromY) break;
				head = new ActionPlan(head, t->mLogicalX, t->mLogicalY);
				t = t->mCameFrom;
			}
			/*
			Tile* t = mTiles[index(toX, toY)].mCameFrom;
			std::stringstream ss1;
			ss1 << "GPTT ending tile: " << t->mLogicalX << "," << t->mLogicalY;
			Con::printf(ss1.str().c_str());
			while(t != 0) {
				std::stringstream ss2;
				ss2 << "GTPP next tile: " << t->mLogicalX << "," << t->mLogicalY << " vs " << fromX << "," << fromY;
				Con::printf(ss2.str().c_str());
				//if((t->mLogicalX == fromX) && (t->mLogicalY == fromY)) break;
				if(!(t->mLogicalX == fromX) && (t->mLogicalY == fromY)) {
					head = new ActionPlan(next, t->mLogicalX, t->mLogicalY);
					next = head;
				}
				t = t->mCameFrom;
			}*/
			//debugSpecifiedActionPlan(head);
			return head;
		}

        DECLARE_CONOBJECT( TileGrid );  
    };  

ConsoleMethod(TileGrid, setWindowSize, void, 4, 4, "(F32 sizeX, [F32 sizeY]]) - Sets the number of tiles loaded into the composite sprite at one time.\n"
																	"@param sizeX The number of tiles per row to load.\n"
																	"@param sizeY The number of tiles per column to load.\n"
																	"@return No return value.") {
	
	object->setDisplayableSize(std::max(1,dAtoi(argv[2])),std::max(1,dAtoi(argv[3])));

}

ConsoleMethod(TileGrid, setWindowCenter, void, 4, 4, "(F32 sizeX, [F32 sizeY]]) - Sets the center of the batch of tiles loaded into the composite sprite at one time.\n"
																	"@param sizeX The number of tiles per row to load.\n"
																	"@param sizeY The number of tiles per column to load.\n"
																	"@return No return value.") {
	
	object->setDisplayCenter(std::max(0,dAtoi(argv[2])),std::max(0,dAtoi(argv[3])));

}

ConsoleMethod(TileGrid, setSpriteOffset, void, 4, 4, "(F32 sizeX, [F32 sizeY]]) - Sets the center of the batch of tiles loaded into the composite sprite at one time.\n"
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

ConsoleMethod(TileGrid, initializeGridTile, void, 7, 7,  "(F32 strideX, [F32 strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                                    "@param tileX The x position of the new tile.\n"
                                                                    "@param tileY The y position of the new tile.\n"
																	"@param tileAssetID The asset ID of the tile.\n"
																	"@param frame The frame of the new tile.\n"
																	"@param logicalPositionArgs The logical position of the new tile.\n"
                                                                    "@return No return value.") {
	object->setTile(std::max(0,dAtoi(argv[2])),std::max(0,dAtoi(argv[3])),argv[4],dAtoi(argv[5]),argv[6]);
}

ConsoleMethod(TileGrid, updateGridTile, void, 6, 6,  "(F32 strideX, [F32 strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                                    "@param tileX The x position of the new tile.\n"
                                                                    "@param tileY The y position of the new tile.\n"
																	"@param tileAssetID The asset ID of the tile.\n"
																	"@param frame The frame of the new tile.\n"
                                                                    "@return No return value.") {
	object->updateTile(std::max(0,dAtoi(argv[2])),std::max(0,dAtoi(argv[3])),argv[4],dAtoi(argv[5]));
}

ConsoleMethod(TileGrid, spinGridTile, void, 4, 4,  "(F32 strideX, [F32 strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                                    "@param tileX The x position of the new tile.\n"
                                                                    "@param tileY The y position of the new tile.\n"
                                                                    "@return No return value.") {
	object->spinTile(std::max(0,dAtoi(argv[2])),std::max(0,dAtoi(argv[3])));
}

ConsoleMethod(TileGrid, resizeGrid, void, 4, 4,  "(F32 strideX, [F32 strideY]]) - Sets the number of tiles on the grid.\n"
                                                                    "@param tileX The new number of tiles, X.\n"
                                                                    "@param tileY The new number of tiles, Y.\n"
                                                                    "@return No return value.") {
	object->resizeGrid(std::max(1,dAtoi(argv[2])),std::max(1,dAtoi(argv[3])));
}

ConsoleMethod(TileGrid, clearSprites, void, 2, 2,    "() - Removes all sprites.\n"
                                                            "@return No return value." )
{
	object->setDisplayed(false);
}

ConsoleMethod(TileGrid, addSprites, void, 2, 2,    "() - Adds all sprites.\n"
                                                            "@return No return value." )
{
	object->setDisplayed(true);
}

ConsoleMethod(TileGrid, setDefaultSpriteStride, void, 3, 4,  "(F32 strideX, [F32 strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                                    "@param strideX The default stride of the local X axis.\n"
                                                                    "@param strideY The default stride of the local Y axis.\n"
                                                                    "@return No return value.")
{
    Vector2 stride;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("strideX strideY")
    if ( (elementCount == 2) && (argc == 3) ) {
        stride.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        stride.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    } else if (elementCount == 1) { // (strideX, [strideY])
        stride.x = dAtof(argv[2]);
        if (argc > 3) stride.y = dAtof(argv[3]);
        else stride.y = stride.x;
    } else { // Invalid
		Con::warnf("CompositeSprite::setDefaultSpriteStride() - Invalid number of parameters!");
        return;
    }

    object->setDefaultSpriteStride(stride);
}

#endif