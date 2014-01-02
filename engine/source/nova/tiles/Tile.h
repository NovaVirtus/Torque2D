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
		U32 mFrame;
		

		char* mArgs;
		// char* tileAnimationID; // SpriteBatch::setSpriteAnimation( const char* pAssetId )
		// SpriteBatch::setSpriteLocalPosition( const Vector2& localPosition )
		// SpriteBatch::setSpriteDepth( const F32 depth )
		// SpriteBatch::setSpriteFlipX( const bool flipX )
		// SpriteBatch::setSpriteFlipY( const bool flipY )
    public:  
		SpriteBatchItem::LogicalPosition* mLogicalPosition; // Was private
        Tile();// const SpriteBatchItem::LogicalPosition& logicalPosition);
		void initializeTile(const char* tileAssetID, const U32 frame, const char* logicalPositionArgs, const Vector2& gridTile);
		void updateTile(const char* tileAssetID, const U32 frame, SpriteBatch* batch);
		void spinTile(SpriteBatch* batch);
		void addToSpriteBatch(SpriteBatch* batch);
		void removeFromSpriteBatch(SpriteBatch* batch);
		Vector2* mCenter;
       ~Tile(); 
        virtual bool onAdd();  
        virtual void onRemove();  
          
        virtual void copyTo(SimObject* object);  
          
        static void initPersistFields();  
          
        DECLARE_CONOBJECT( Tile );  
    };  
#endif