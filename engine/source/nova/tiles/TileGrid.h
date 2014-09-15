#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif  
#ifndef _TILE_H_
#include "Tile.h"
#endif

#ifndef _TILE_GRID_H_
#define _TILE_GRID_H_

#include "../statics/BorderObject.h"
#include <string>
#include <sstream>
#include "../actors/ActionPlan.h"

using namespace std;

		enum CameraPosition { // "Up" direction
			CAMERA_NORTH = 0, CAMERA_EAST = 1, CAMERA_SOUTH = 2, CAMERA_WEST = 3
		};

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
      
			F32 mDebugSavedX, mDebugSavedY;

			bool mDisplayed;
			
			CameraPosition mCurrentCameraRotation;
			U32 mNumDisplayedTilesFromCenterX;
			U32 mNumDisplayedTilesFromCenterY;

			U32 mMinDisplayedX;
			U32 mMinDisplayedY;

			U32 mMaxDisplayedX;
			U32 mMaxDisplayedY;

			bool mFlipX;
			bool mFlipY;
		
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
				for(U32 x=minCol; x <= maxCol; x++) mTiles[index(x,y)].removeFromSpriteBatch();
			}
			inline void removeColumn(const U32 x, const U32 minRow, const U32 maxRow) {
				for(U32 y=minRow; y <= maxRow; y++) mTiles[index(x,y)].removeFromSpriteBatch();
			}
			inline void addRow(const U32 y, const U32 minCol, const U32 maxCol) {
				for(U32 x=minCol; x <= maxCol; x++) mTiles[index(x,y)].addToSpriteBatch(mBackgroundSprite, mForegroundSprite, mFlipX, mFlipY);
			}
			inline void addColumn(const U32 x, const U32 minRow, const U32 maxRow) {
				for(U32 y=minRow; y <= maxRow; y++) mTiles[index(x,y)].addToSpriteBatch(mBackgroundSprite, mForegroundSprite, mFlipX, mFlipY);
			}
		
	protected:

    public:  
		// Now that I can find intersections,
		// 1) Check which lines that we'll be crossing (x, y)
		// 2) For each of these, find the intersection point
		// 3) Check if crossing and Y boundary within epsilon distance of each other and if so roll them up into one diagonal crossing
		// 4) Find the distance to this point, calculate time it will take to move there and move at current speed to that point
		// 5) Schedule event at that point to move to next
		/*inline void TestFindIntersection() {
			Point2D result;
			bool returnValue;
			stringstream ss;
			returnValue = FindIntersection(0, 0, 5, 0, 3, -10, 3, 10, result);
			ss << "TestFind1: " << returnValue << " yields " << result.x << "," << result.y;
			Con::printf(ss.str().c_str());
			ss.clear();
			returnValue = FindIntersection(0, 0, 0, 5, -5, 3, 5, 3, result);
			ss << "TestFind2: " << returnValue << " yields " << result.x << "," << result.y;
			Con::printf(ss.str().c_str());
			ss.clear();
			returnValue = FindIntersection(0, 0, 2, 2, 0, 5, 5, 0, result);
			ss << "TestFind3: " << returnValue << " yields " << result.x << "," << result.y;
			Con::printf(ss.str().c_str());
		}*/
		inline const CameraPosition CurrentRotation() { return mCurrentCameraRotation; }
		void rotateCamera(CameraPosition newOrientation);
		inline void rotateCameraRelative(int offset) {
			switch(mCurrentCameraRotation) {
				case CAMERA_NORTH: if(offset > 0) rotateCamera(CAMERA_EAST); else if(offset < 0) rotateCamera(CAMERA_WEST); break;
				case CAMERA_EAST: if(offset > 0) rotateCamera(CAMERA_SOUTH); else if(offset < 0) rotateCamera(CAMERA_NORTH); break;
				case CAMERA_SOUTH: if(offset > 0) rotateCamera(CAMERA_WEST); else if(offset < 0) rotateCamera(CAMERA_EAST); break;
				case CAMERA_WEST: if(offset > 0) rotateCamera(CAMERA_NORTH); else if(offset < 0) rotateCamera(CAMERA_SOUTH); break;
			}
		}

		inline bool FindIntersection(F32 oX1, F32 oY1, F32 oX2, F32 oY2, F32 dX1, F32 dY1, F32 dX2, F32 dY2, Point2D & result) {
			F32 epsilon = (F32)0.0001;
			F32 d = (oX1 - oX2) * (dY1 - dY2) - (oY1 - oY2) * (dX1 - dX2);
			F32 absD = (d >= 0? d : -d);
			if(absD < epsilon) return false;
			F32 pre = oX1 * oY2 - oY1 * oX2;
			F32 post = dX1 * dY2 - dY1 * dX2;
			F32 x = (pre * (dX1 - dX2) - (oX1 - oX2) * post) / d;
			F32 y = (pre * (dY1 - dY2) - (oY1 - oY2) * post) / d;
			if (x < (min(oX1, oX2) - epsilon) || x > (max(oX1, oX2) + epsilon) || x < (min(dX1, dX2) - epsilon) || x > (max(dX1, dX2) + epsilon)) return false;
			if (y < (min(oY1, oY2) - epsilon) || y > (max(oY1, oY2) + epsilon) || y < (min(dY1, dY2) - epsilon) || y > (max(dY1, dY2) + epsilon)) return false;
			result.x = x;
			result.y = y;
			return true;
		}
		/*
		inline void AddString(stringstream* ss, bool values[]) {
			if(values[TILE_LEFT]) (*ss) << "Left=1";
			else *ss << "Left=0";

			if(values[TILE_RIGHT]) *ss << "Right=1";
			else *ss << "Right=0";

			if(values[TILE_UP]) *ss << "Up=1";
			else *ss << "Up=0";

			if(values[TILE_DOWN]) *ss << "Down=1";
			else *ss << "Down=0";
		}*/
		inline bool canMoveBetweenTiles(Tile* from, Tile* to) {
			S32 offsetX = to->mLogicalX - from->mLogicalX;
			S32 offsetY = to->mLogicalY - from->mLogicalY;
			return canMoveBetweenTiles(from, to, offsetX, offsetY);
		}
		inline bool canMoveBetweenTiles(Tile* from, Tile* to, S32 offsetX, S32 offsetY) {
			if(offsetX > 0) {
				if(from->mMovementRestrictions[TILE_RIGHT]) return false;
				if(to->mMovementRestrictions[TILE_LEFT]) return false;
			} else if(offsetX < 0) {
				if(from->mMovementRestrictions[TILE_LEFT]) return false;
				if(to->mMovementRestrictions[TILE_RIGHT]) return false;
			}
			
			if(offsetY > 0) {
				if(from->mMovementRestrictions[TILE_UP]) return false;
				if(to->mMovementRestrictions[TILE_DOWN]) return false;
			} else if(offsetY < 0) {
				if(from->mMovementRestrictions[TILE_DOWN]) return false;
				if(to->mMovementRestrictions[TILE_UP]) return false;
			}
			return true;
		}


		//void TileGrid::tryAStarAddNeighbor(priority_queue<Tile*, vector<Tile*>, TileDistanceCompare> openSet, Tile* from, Tile* goal, Tile* t);
		inline bool TryToAddNeighbor(Tile* from, S32 offsetX, S32 offsetY, F32 curCostPast, Tile* goal, Tile* & to, F32 fixedCost, bool checkLock) {
			Tile* intermediateOne = 0;
			Tile* intermediateTwo = 0;
			if(!isValidLocation(from->mLogicalX + offsetX, from->mLogicalY + offsetY)) return false;
			to = &mTiles[index(from->mLogicalX + offsetX, from->mLogicalY + offsetY)];

			if(checkLock) {
				if(to->isLocked()) return false;
			}

			// First, check for movement restricted and return false if we hit one
			//mMovementRestrictions: bool[4]; // 0 -> y+1, 1 -> x+1, 2 -> y-1, 3 -> x-1
			if(!canMoveBetweenTiles(from, to, offsetX, offsetY)) return false;
			
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
			F32 distance = from->actualDistance(to, fixedCost);//, intermediateOne, intermediateTwo);
			F32 tentativeScore = curCostPast + distance;
			if(tentativeScore < to->mCostPast) {
				to->mCameFrom = from;
				to->mCostPast = tentativeScore;
				to->mCostFuture = tentativeScore + to->estimatedDistance(goal);
				to->mCostMovePrev = distance;
				return true;
			}
			return false;
		}
		//void tryAStarAddNeighbor(Tile* from, Tile* goal, Tile* t);
		void tryDijkstras();
		bool tryAStar(Tile* origin, Tile* goal);
		
		U32 mCenterX, mCenterY;
		U32 mSizeX, mSizeY;
		F32 mStrideX, mStrideY;
		F32 mSpriteOffsetX, mSpriteOffsetY;

		CompositeSprite* mBackgroundSprite;
		CompositeSprite* mForegroundSprite;
		SceneWindow* mSceneWindow;
		Tile* mTiles;

		TileGrid();
		TileGrid(const U32 x, const U32 y);
		~TileGrid(); 
		
		
		void resizeGrid(const U32 x, const U32 y);
		void setDisplayableSize(const U32 x, const U32 y);
		void setDisplayCenter(const U32 x, const U32 y);
		void setDisplayed(bool displayed);
		void addBorderObject(const U32 x, const U32 y, TileRelativePosition positionRelativeToTile, const char* assetID, const U32 frame, const char* logicalPositionArgs, bool blocksMovement, F32 extraMovementCost);
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

		inline void getTileCenter(const U32 logicalX, const U32 logicalY, F32& worldX, F32& worldY) { // Could be negative, to account for walls
			worldX = (F32)((logicalX + logicalY) * mStrideX);
			worldY = (F32)(((S32)logicalY - (S32)logicalX + 0.5) * mStrideY);
		}

		inline void setDefaultSpriteStride( const Vector2& defaultStride ) { 
			mStrideX = defaultStride.x; 
			mStrideY = defaultStride.y; 
			// Trying to fix fence problem - one approach would have been this, plus double logical position args in script
			//Vector2 newDefaultStride;
			//newDefaultStride.x = defaultStride.x/2;
			//newDefaultStride.y = defaultStride.y/2;
			if(mBackgroundSprite) mBackgroundSprite->setDefaultSpriteStride(defaultStride); 
			if(mForegroundSprite) mForegroundSprite->setDefaultSpriteStride(defaultStride);
		}

		inline F32 strideYFactor() { return (mStrideY / mStrideX); }

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
				Point2D* position = t->mCenter;
				ss << "<Logical" << current->x << "," << current->y << " -> World" << position->x << "," << position->y << "> ";
			}
			Con::printf(ss.str().c_str());
		}


		inline ActionPlan* getPathToTarget(const U32 fromX, const U32 fromY, const U32 toX, const U32 toY) {
			if(!isValidLocation(toX, toY)) return 0;
			if(!tryAStar(&mTiles[index(fromX, fromY)], &mTiles[index(toX, toY)])) return 0;
			Tile* t = &mTiles[index(toX, toY)];
			ActionPlan* next = new ActionPlan(toX, toY);
			ActionPlan* head = next;

			t = t->mCameFrom;
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

ConsoleMethod(TileGrid, printActualCost, void, 6, 6, "Display the distance between two specified tiles.") {

	Tile* from = object->getTile(dAtoi(argv[2]), dAtoi(argv[3]));
	Tile* to = object->getTile(dAtoi(argv[4]), dAtoi(argv[5]));
	F32 distance = 1;
	S32 offsetX = (S32)(to->mLogicalX) - (S32)(from->mLogicalX);
	S32 offsetY = (S32)(to->mLogicalY) - (S32)(from->mLogicalY);
	if(offsetX != 0 && offsetY != 0) distance = (F32)1.414214;
	stringstream ss;
	ss << "Actual distance between " << from->mLogicalX << "," << from->mLogicalY << " and " << to->mLogicalX << "," << to->mLogicalY << " = ";
	ss << from->actualDistance(to, distance);
	Con::printf(ss.str().c_str());
}
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

ConsoleMethod(TileGrid, addBorderObject, void, 10, 10, "(Console text)") {
//const U32 x, const U32 y, TileRelativePosition positionRelativeToTile, const char* assetID, const U32 frame, const char* logicalPositionArgs
	U32 logicalX, logicalY;
	U32 relativePositionIn;
	TileRelativePosition relativePosition;
	U32 frame;
	logicalX = dAtoi(argv[2]);
	logicalY = dAtoi(argv[3]);
	relativePositionIn = dAtoi(argv[4]); // TILE_UP = 0, TILE_RIGHT = 1, TILE_DOWN = 2, TILE_LEFT = 3
	relativePosition = TILE_UP;
	switch(relativePositionIn) {
		case 0: relativePosition = TILE_UP; break;
		case 1: relativePosition = TILE_RIGHT; break;
		case 2: relativePosition = TILE_DOWN; break; 
		case 3: relativePosition = TILE_LEFT; break;
	}
	frame = dAtoi(argv[6]);
	// LogicalX, LogicalY, RelPos, Asset, Frame, LogicalPositionArgs

	object->addBorderObject(logicalX, logicalY, relativePosition, argv[5], frame, argv[7], dAtob(argv[8]), dAtof(argv[9]));
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

ConsoleMethod(TileGrid, rotateCamera, void, 3, 3, "(rotateOffset) - Rotates by a particular offset.\n"
											"@return No return value." ) {
	Con::printf("Trying to rotate camera");
	object->rotateCameraRelative(dAtoi(argv[2]));
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