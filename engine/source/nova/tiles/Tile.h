#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif

#ifndef _TILE_H_
#define _TILE_H_

#include "2d/core/SpriteBatchItem.h"
#include "2d/core/SpriteBatch.h"
#include <string>
#include <sstream>
#include "nova/statics/Wall.h"

		enum TileRelativePosition {
			TILE_UP = 0, TILE_RIGHT = 1, TILE_DOWN = 2, TILE_LEFT = 3
		};


class Wall;
//bool* mMovementRestrictions;// = new bool[4]; // 0 -> y+1, 1 -> x+1, 2 -> y-1, 3 -> x-3
class Tile : public SimObject  
    {  
    private:  
        typedef SimObject Parent;  
		// U32 spriteID; -> currently not using because deleting and remaking them
		// SpriteBatch::selectSpriteId( const U32 batchId )
        // U32 spriteID;
		U32 mSpriteID;
		char* mTileAssetID; // void SpriteBatch::setSpriteImage( const char* pAssetId, const U32 imageFrame )
		SpriteBatch* mBatch;

		Wall* mNeighboringWalls[4];
		// char* tileAnimationID; // SpriteBatch::setSpriteAnimation( const char* pAssetId )
		// SpriteBatch::setSpriteLocalPosition( const Vector2& localPosition )
		// SpriteBatch::setSpriteDepth( const F32 depth )
		// SpriteBatch::setSpriteFlipX( const bool flipX )
		// SpriteBatch::setSpriteFlipY( const bool flipY )
    public:  

		SpriteBatchItem::LogicalPosition* mLogicalPosition; // Was private
        U32 mFrame;
		F32 mExtraMovementCost;
		Tile();// const SpriteBatchItem::LogicalPosition& logicalPosition);
		void initializeTile(const char* tileAssetID, const U32 frame, const char* logicalPositionArgs, const Vector2& gridTile, const U32 logicalX, const U32 logicalY);
		void updateTile(const char* tileAssetID, const U32 frame);
		void spinTile();
		void addToSpriteBatch(SpriteBatch* batch, SpriteBatch* foregroundBatch);
		void removeFromSpriteBatch();
		inline TileRelativePosition GetOppositePosition(TileRelativePosition p) {
			switch(p) {
				case TILE_UP: return TILE_DOWN;
				case TILE_RIGHT: return TILE_LEFT;
				case TILE_DOWN: return TILE_UP;
				case TILE_LEFT: return TILE_RIGHT;
				default: return TILE_UP;
			}
		}
		void addNeighboringWall(Wall* wall, TileRelativePosition relativePosition, SpriteBatch* foregroundBatch);

		inline void removeNeighboringWall(TileRelativePosition relativePosition) {
			mNeighboringWalls[relativePosition] = 0;
		}
		
		void updateMoveCostsAndRestrictions();

		bool* mMovementRestrictions;// = new bool[4]; // 0 -> y+1, 1 -> x+1, 2 -> y-1, 3 -> x-3

		inline F32 actualDistance(Tile* to, F32 euclidianDistance, Tile* intermediateOne, Tile* intermediateTwo) { // Would in theory account for things like walls and etc.
			return (F32)0.05 + euclidianDistance + to->mExtraMovementCost + 
				(F32)((intermediateOne == 0 ? 0 : 0.5 * intermediateOne->mExtraMovementCost) + (intermediateTwo == 0 ? 0 : 0.5 * intermediateTwo->mExtraMovementCost));
		
		}
		inline F32 estimatedDistance(Tile* to) {
			if(this == to) return 0;
			F32 xDist = ((F32)(mLogicalX)) - ((F32)(to->mLogicalX));
			F32 yDist = ((F32)(mLogicalY)) - ((F32)(to->mLogicalY));
			return ((F32) sqrt((xDist * xDist) + (yDist * yDist))) + to->mExtraMovementCost;
		}

		F32 mCostPast;
		F32 mCostFuture;
		F32 mCostMovePrev;
		//
		U32 mLogicalX;
		U32 mLogicalY;
		bool mInClosedSet;
		Tile* mCameFrom;

		Point2D* mCenter;
       ~Tile(); 
        virtual bool onAdd();  
        virtual void onRemove();  
          
        virtual void copyTo(SimObject* object);  
          
        static void initPersistFields();  
          
        DECLARE_CONOBJECT( Tile );  
    };  
#endif