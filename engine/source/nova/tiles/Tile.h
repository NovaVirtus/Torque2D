#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif
#include "2d/core/SpriteBatchItem.h"
#include "2d/core/SpriteBatch.h"
#include <string>
#include <sstream>

#ifndef _TILE_H_
#define _TILE_H_


class Tile : public SimObject  
    {  
    private:  
        typedef SimObject Parent;  
		// U32 spriteID; -> currently not using because deleting and remaking them
		// SpriteBatch::selectSpriteId( const U32 batchId )
        // U32 spriteID;
		U32 mSpriteID;
		char* mTileAssetID; // void SpriteBatch::setSpriteImage( const char* pAssetId, const U32 imageFrame )
		
		

		char* mArgs;
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
		void updateTile(const char* tileAssetID, const U32 frame, SpriteBatch* batch);
		void spinTile(SpriteBatch* batch);
		void addToSpriteBatch(SpriteBatch* batch);
		void removeFromSpriteBatch(SpriteBatch* batch);

		inline F32 actualDistance(Tile* to, F32 euclidianDistance) { // Would in theory account for things like walls and etc.
			//if(to == 0) Con::printf("Null distance..");
			if(to->mFrame == 2) {
				Con::printf("Hitting cabin..");
				return euclidianDistance + (F32)3;
			}

			return (F32)0.05 + euclidianDistance;
			//F32 xDist = (F32)(mLogicalX - to->mLogicalX);
			//F32 yDist = (F32)(mLogicalY - to->mLogicalY);
			//return sqrt((xDist * xDist) + (yDist * yDist));
		}
		inline F32 estimatedDistance(Tile* to) {
			F32 xDist = ((F32)(mLogicalX)) - ((F32)(to->mLogicalX));
			F32 yDist = ((F32)(mLogicalY)) - ((F32)(to->mLogicalY));
			return (F32) sqrt((xDist * xDist) + (yDist * yDist));
		}

		F32 mCostPast;
		F32 mCostFuture;
		//
		U32 mLogicalX;
		U32 mLogicalY;
		bool mInClosedSet;
		Tile* mCameFrom;

		Vector2* mCenter;
       ~Tile(); 
        virtual bool onAdd();  
        virtual void onRemove();  
          
        virtual void copyTo(SimObject* object);  
          
        static void initPersistFields();  
          
        DECLARE_CONOBJECT( Tile );  
    };  
#endif