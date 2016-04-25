#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif

#ifndef _ACTOR_SET_H_
#define _ACTOR_SET_H_

#include "2d/core/SpriteBatchItem.h"
#include "2d/core/SpriteBatch.h"
#include <string>
#include <sstream>
#include <vector>
#include "nova/actors/Actor.h"

class Actor;

//bool* mMovementRestrictions;// = new bool[4]; // 0 -> y+1, 1 -> x+1, 2 -> y-1, 3 -> x-3
class ActorSet : public SimObject {  
    //private:  
	protected:
        typedef SimObject Parent;  
    public:  
		std::vector<Actor*> mActors;

		void RemoveActors();
		
		inline static bool updateAnActor(Actor* a) { return !(a->updateActorTick()); }
		inline void updateTick() {
			for(std::vector<Actor*>::size_type pos = mActors.size(); pos >= 0; pos--) {
				//if(!((*it)->updateTick())) mActors.erase(std::remove(
				mActors.erase(std::remove_if(std::begin(mActors), std::end(mActors), &ActorSet::updateAnActor));
			}
		}

		ActorSet();// const SpriteBatchItem::LogicalPosition& logicalPosition);
		
		Point2D* mCenter;
       ~ActorSet(); 
        virtual bool onAdd();  
        virtual void onRemove();  
          
        virtual void copyTo(SimObject* object);  
          
        static void initPersistFields();  
        inline void addActor(Actor* a) {
			mActors.push_back(a);
		}
        DECLARE_CONOBJECT( ActorSet );  

    };  
	ConsoleMethod(ActorSet, addActor, void, 3, 3, "(U32 logicalX, U32 logicalX) - Adds this actor to the scene at specified location.\n"
																	"@param logicalX The X coordinate.\n"
																	"@param logicalY The Y coordinate.\n"
																	"@return No return value.") {
		object->addActor(dynamic_cast<Actor*>(Sim::findObject(argv[2])));
	}
	ConsoleMethod(ActorSet, updateTick, void, 2, 2, "() - Runs an update tick.\n"
													"@return No return value.") {
		object->updateTick();
	}
#endif