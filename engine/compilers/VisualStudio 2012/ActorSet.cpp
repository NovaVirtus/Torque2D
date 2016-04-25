#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "ActorSet.h"
IMPLEMENT_CONOBJECT(ActorSet);  

class Actor;
void ActorSet::copyTo(SimObject* object) {  
    // Fetch other object.  
   ActorSet* pWall = static_cast<ActorSet*>( object );  
  
   // Sanity!  
   AssertFatal( pWall != NULL, "ActorSet::copyTo() - Object is not the correct type.");  
  
   // Copy parent.  
   Parent::copyTo( object );  
  
   // Copy the state. 
} 

bool ActorSet::onAdd() {
    // Fail if the parent fails.  
    if ( !Parent::onAdd() )  
        return false;  
  
    // Do some work here.  
    Con::printf("Hello from ActorSet!");  
	return true;
}  
  
void ActorSet::onRemove() {
    // Do some work here.  
} 

ActorSet::ActorSet() {  
	mActors.resize(100);
	//ActorContinuedMovementCheckEvent* pMoveEvent = new ActorContinuedMovementCheckEvent(timeToArrive - continuedMovementTime); //logicalX, logicalY,totalMoveTime - continuedMovementTime); //(totalMoveTime - continuedMovementTime));
	//mContinueMoveCheckEventID = Sim::postEvent(this, pMoveEvent, Sim::getCurrentTime() + continuedMovementTime);
} 

ActorSet::~ActorSet() {
	
}

void ActorSet::RemoveActors() {
}
  
void ActorSet::initPersistFields()  
{  
    // Call parent.  
    Parent::initPersistFields();  
  
    // Add my fields here.  
	//addField("visualWall", TypeSimObjectPtr, Offset(mVisualWall, Wall), "Visual tile associated with this tile in the WallGrid.");
} 