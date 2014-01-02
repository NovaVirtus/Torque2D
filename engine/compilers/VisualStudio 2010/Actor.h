#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif
#include "2d/core/SpriteBatchItem.h"
#include "2d/core/SpriteBatch.h"
#include "2d/core/CoreMath.h"
#include <string>
#include <sstream>
#include "TileGrid.h"

#ifndef _ACTOR_H_
#define _ACTOR_H_

class Actor : public SimObject  
{
    private:  
        typedef SimObject Parent;  
	protected:
	public:
		CompositeSprite* mCompositeSprite;
		Actor();
		~Actor();
		virtual bool onAdd();  
        virtual void onRemove();  
        virtual void copyTo(SimObject* object);  
        static void initPersistFields();  
      
		TileGrid* mTileGrid;
		SceneWindow* mSceneWindow;
		U32 spriteID;
		void setSprite(const char* assetID, const U32 frame, const int sizeX, const int sizeY);
		void addToWindow(const int logicalX, const int logicalY);

		DECLARE_CONOBJECT( Actor );  

};

		ConsoleMethod(Actor, addToScene, void, 4, 4, "(int logicalX, int logicalX) - Adds this actor to the scene at specified location.\n"
																	"@param logicalX The X coordinate.\n"
																	"@param logicalY The Y coordinate.\n"
																	"@return No return value.") {
			object->addToWindow(dAtoi(argv[2]),dAtoi(argv[3]));

		}
		ConsoleMethod(Actor, initializeSprite, void, 6, 6,  "(float strideX, [float strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                            		"@param tileAssetID The asset ID of the tile.\n"
																	"@param frame The frame of the new tile.\n"
                                                                    "@return No return value.") {
			object->setSprite(argv[2], dAtoi(argv[3]), dAtoi(argv[4]), dAtoi(argv[5]));
																		//object->setTile(std::max(0,dAtoi(argv[2])),std::max(0,dAtoi(argv[3])),argv[4],dAtoi(argv[5]),argv[6]);
}

#endif