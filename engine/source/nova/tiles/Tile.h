#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif

#ifndef _TILE_H_
#define _TILE_H_

#include "2d/core/SpriteBatchItem.h"
#include "2d/core/SpriteBatch.h"
#include <string>
#include <sstream>
#include "nova/statics/BorderObject.h"

		enum TileRelativePosition {
			TILE_UP = 0, TILE_RIGHT = 1, TILE_DOWN = 2, TILE_LEFT = 3
		};

		enum TileLockedState {
			TILE_UNLOCKED = 0, TILE_LOCK_TEMPORARY = 1, TILE_LOCK_INDETERMINATE = 2, TILE_LOCK_WORKING = 3, TILE_LOCK_OBJECT = 4
		};


class BorderObject;
//bool* mMovementRestrictions;// = new bool[4]; // 0 -> y+1, 1 -> x+1, 2 -> y-1, 3 -> x-3
class Tile : public SimObject  
    {  
    private:  
        typedef SimObject Parent;  
		U32 mSpriteID;
		char* mTileAssetID; // void SpriteBatch::setSpriteImage( const char* pAssetId, const U32 imageFrame )
		SpriteBatch* mBatch;
		F32 mNeighborExtraMoveCost[4];
		TileLockedState mLockState;
    public:  
		inline void changeLockedStatus(TileLockedState newState) {  mLockState = newState; } // Only if the tile is already locked by this actor

		inline bool lockTile(TileLockedState newState) {
			if(mLockState != TILE_UNLOCKED) return false;
			mLockState = newState;
			setFrame(1);
			return true;
		}
		inline void unlockTile() {
			mLockState = TILE_UNLOCKED;
			setFrame(2);
		}
		bool mMovementRestrictions[4];// = new bool[4]; // 0 -> y+1, 1 -> x+1, 2 -> y-1, 3 -> x-3
		BorderObject* mNeighboringBorderObjects[4];
		F32 mExtraCostToNeighbor[4];
		SpriteBatchItem::LogicalPosition* mLogicalPosition; // Was private
        U32 mFrame;
		F32 mExtraMovementCost;
		
		Tile();// const SpriteBatchItem::LogicalPosition& logicalPosition);
		void initializeTile(const char* tileAssetID, const U32 frame, const char* logicalPositionArgs, const Vector2& gridTile, const U32 logicalX, const U32 logicalY);
		void updateTile(const char* tileAssetID, const U32 frame);
		void spinTile();
		void setFrame(const U32 frame);
		void addToSpriteBatch(SpriteBatch* batch, SpriteBatch* foregroundBatch, bool flipX, bool flipY);
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
		void addNeighboringBorderObject(BorderObject* newBorder, TileRelativePosition relativePosition, SpriteBatch* foregroundBatch, bool obstructsMovement, F32 extraMoveCost);

		inline void removeNeighboringBorderObject(TileRelativePosition relativePosition) {
			mNeighboringBorderObjects[relativePosition] = 0;
		}
		
		void updateMoveCostsAndRestrictions();

		

		inline void setFlip(bool flipX, bool flipY) {
			if(mBatch == 0) return;
			mBatch->selectSpriteId(mSpriteID);
			mBatch->setSpriteFlipX(flipX);
			mBatch->setSpriteFlipY(flipY);
		}
		inline F32 getTileExtraMoveCost(Tile* to, S32 offsetX, S32 offsetY) {
			F32 cost = 0;
			if(offsetX > 0) cost += std::max(mNeighborExtraMoveCost[TILE_RIGHT], to->mNeighborExtraMoveCost[TILE_LEFT]);
			else if(offsetX < 0) cost += std::max(mNeighborExtraMoveCost[TILE_LEFT], to->mNeighborExtraMoveCost[TILE_RIGHT]);
			
			if(offsetY > 0) cost += std::max(mNeighborExtraMoveCost[TILE_UP], to->mNeighborExtraMoveCost[TILE_DOWN]);
			else if(offsetY < 0) cost += std::max(mNeighborExtraMoveCost[TILE_DOWN], to->mNeighborExtraMoveCost[TILE_UP]);
			return cost;
		}
		inline F32 actualDistance(Tile* to, F32 euclidianDistance) {//, Tile* intermediateOne, Tile* intermediateTwo) { // Would in theory account for things like walls and etc.
			
			return (F32)0.05 + euclidianDistance + to->mExtraMovementCost + 
				//(F32)((intermediateOne == 0 ? 0 : 0.5 * intermediateOne->mExtraMovementCost) + (intermediateTwo == 0 ? 0 : 0.5 * intermediateTwo->mExtraMovementCost)) + 
				getTileExtraMoveCost(to, (S32)to->mLogicalX - (S32)mLogicalX, (S32)to->mLogicalY - (S32)mLogicalY);
		
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