#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif

#ifndef _WALL_H_
#define _WALL_H_

#include "2d/core/SpriteBatchItem.h"
#include "2d/core/SpriteBatch.h"
#include <string>
#include <sstream>
#include "nova/tiles/Tile.h"

enum TileRelativePosition;
class Tile;

//bool* mMovementRestrictions;// = new bool[4]; // 0 -> y+1, 1 -> x+1, 2 -> y-1, 3 -> x-3
class Wall : public SimObject {  
    private:  
        typedef SimObject Parent;  
		// SpriteBatch::selectSpriteId( const U32 batchId )
		U32 mSpriteID;
		char* mAssetID;
		F32 mDepth;
		TileRelativePosition mPositionRelativeToTileOne;
    public:  
		SpriteBatchItem::LogicalPosition* mLogicalPosition; // Was private
        U32 mFrame;
		Tile* mTileOne;
		Tile* mTileTwo;
		//Vector2* mCenter;
		SpriteBatch* mBatch;
		void initializeWall(const char* assetID, const U32 frame, Point2D* center, const char* logicalPosition, Tile* tileOne, TileRelativePosition positionRelativeToTileOne, Tile* tileTwo, SpriteBatch* foregroundBatch);
		void switchAsset(const char* assetID);
		void switchFrame(const U32 frame);
		void addToBatch(SpriteBatch* batch);
		void removeFromBatch();
		void destroyWall();

		Wall();// const SpriteBatchItem::LogicalPosition& logicalPosition);
		
		Point2D* mCenter;
       ~Wall(); 
        virtual bool onAdd();  
        virtual void onRemove();  
          
        virtual void copyTo(SimObject* object);  
          
        static void initPersistFields();  
          
        DECLARE_CONOBJECT( Wall );  
    };  
#endif