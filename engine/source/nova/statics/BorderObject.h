#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif

#ifndef _BORDER_OBJECT_H_
#define _BORDER_OBJECT_H_

#include "2d/core/SpriteBatchItem.h"
#include "2d/core/SpriteBatch.h"
#include <string>
#include <sstream>
#include "nova/tiles/Tile.h"
//#include "nova/actors/Actor.h"

enum TileRelativePosition;
class Tile;
class Actor;

//bool* mMovementRestrictions;// = new bool[4]; // 0 -> y+1, 1 -> x+1, 2 -> y-1, 3 -> x-3
class BorderObject : public SimObject {  
    //private:  
	protected:
        typedef SimObject Parent;  
		U32 mSpriteID;
		char* mAssetID;
		F32 mDepth;
		TileRelativePosition mPositionRelativeToTileOne;
		bool mBlocksMovement;
		F32 mExtraMovementCost;
    public:  
		SpriteBatchItem::LogicalPosition* mLogicalPosition; // Was private
        U32 mFrame;
		Tile* mTileOne;
		Tile* mTileTwo;
		//Vector2* mCenter;
		SpriteBatch* mBatch;
		virtual void initializeBorderObject(const char* assetID, const U32 frame, Point2D* center, const char* logicalPosition, Tile* tileOne, 
			TileRelativePosition positionRelativeToTileOne, Tile* tileTwo, SpriteBatch* foregroundBatch, bool blocksMovement, F32 extraMovementCost);
		
		//virtual void switchAsset(const char* assetID);
		//virtual void switchFrame(const U32 frame);
		virtual void addToBatch(SpriteBatch* batch);
		virtual void removeFromBatch();
		virtual void destroyBorderObject();


		virtual void startActorPassthrough(Actor* actor);
		virtual void endActorPassthrough(Actor* actor);

		BorderObject();// const SpriteBatchItem::LogicalPosition& logicalPosition);
		
		Point2D* mCenter;
       ~BorderObject(); 
        virtual bool onAdd();  
        virtual void onRemove();  
          
        virtual void copyTo(SimObject* object);  
          
        static void initPersistFields();  
          
        DECLARE_CONOBJECT( BorderObject );  
    };  
#endif