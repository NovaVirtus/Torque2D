#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "2d/sceneobject/CompositeSprite.h"
#include "Actor.h"


IMPLEMENT_CONOBJECT(Actor);  

void Actor::copyTo(SimObject* object) {  
    // Fetch other object.  
   Actor* pActor = static_cast<Actor*>( object );  
  
   // Sanity!  
   AssertFatal(pActor != NULL, "TileGrid::copyTo() - Object is not the correct type.");  
  
   // Copy parent.  
   Parent::copyTo( object );  
  
   // Copy the state.  

   //pTileGrid->mEmitLight = mEmitLight;  
   //pTileGrid->mBrightness = mBrightness;  
} 

bool Actor::onAdd()  {  
    // Fail if the parent fails.  
    if (!Parent::onAdd())  
        return false;  
  
    // Do some work here.  
    Con::printf("Hello from Actor!");  
	return true;
}  
  
void Actor::onRemove() {  
	// Do some work here.  
} 

Actor::Actor() {

}
Actor::~Actor() {

}
void Actor::setSprite(const char* assetID, const U32 frame, const int sizeX, const int sizeY) {
	if(spriteID) mCompositeSprite->removeSprite();

	SpriteBatchItem::LogicalPosition* position = new SpriteBatchItem::LogicalPosition("0 0"); // Delete this?
	spriteID = mCompositeSprite->addSprite(*position);
	mCompositeSprite->setSpriteImage(assetID, frame);
	Vector2 size;
	size.x = sizeX;
	size.y = sizeY;
	mCompositeSprite->setSpriteSize(size);
	delete position;
}

void Actor::addToWindow(const int logicalX, const int logicalY) {
	if (!(mTileGrid->isValidLocation(logicalX, logicalY))) return;
	Tile* t = mTileGrid->getTile(logicalX,logicalY);
	Vector2* position = t->mCenter;
	std::stringstream ss;
	ss << "Actor added to tile " << logicalX << "," << logicalY << " -> center = " << position->x << "," << position->y;
	Con::printf(ss.str().c_str());
	mCompositeSprite->setPosition(*position);
	//mTileGrid->getWorldCoordinates(logicalX, logicalY, position);

	//mCompositeSprite->setPosition(position);
//	
	//position.x = worldX;
	//position.y = worldY;
	//std::stringstream ss;
	//ss << "Actor added to tile " << logicalX << "," << logicalY << " -> center = " << position.x << "," << position.y;
	//Con::printf(ss.str().c_str());
	//mCompositeSprite->setPosition(position);

}

void Actor::initPersistFields() {  
    // Call parent.  
    Parent::initPersistFields();  
	
	addField("TileGrid",TypeSimObjectPtr, Offset(mTileGrid, Actor), "TileGrid to use for this Actor.");
	addField("SceneWindow",TypeSimObjectPtr, Offset(mSceneWindow, Actor), "SceneWindow to use for this Actor.");
	addField("CompositeSprite",TypeSimObjectPtr, Offset(mCompositeSprite, Actor), "Composite sprite to use for this Actor.");
    // Add my fields here.  
   //addField("EmitLight", TypeBool, Offset(mEmitLight, TileGrid), "Flags whether the light is on or off.");  
    //addField("Brightness", TypeF32, Offset(mBrightness, TileGrid), "Sets the brightness of the light.");   
}           

