#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif
#include "2d/core/SpriteBatchItem.h"
#include "2d/core/SpriteBatch.h"
#include "2d/sceneobject/CompositeSprite.h"
#include "2d/core/CoreMath.h"
#include <string>
#include <sstream>
#include "nova/Tiles/TileGrid.h"
#include "ActorArrivedEvent.h"
#include "ActorContinuedMovementCheckEvent.h"
#include "ActionPlan.h"

#ifndef _ACTOR_H_
#define _ACTOR_H_

class Actor : public SimObject  
{
    private:  
        typedef SimObject Parent;  
	protected:
	public:
		Actor();
		~Actor();
		virtual bool onAdd();  
        virtual void onRemove();  
        virtual void copyTo(SimObject* object);  
        static void initPersistFields();  
      
		CompositeSprite* mCompositeSprite;
		
		bool mHasTarget;
		U32 mTargetX, mTargetY;

		TileGrid* mTileGrid;
		ActionPlan* nextStep;
		SceneWindow* mSceneWindow;
		
		bool advanceOverrideMoveFlag;

		U32 mArrivedEventID, mContinueMoveCheckEventID;
		U32 mLogicalX, mLogicalY;
		U32 mSpriteID;
		
		F32 mSpeed;
		
		void addToWindow(const U32 logicalX, const U32 logicalY);
		void setArrivingIn(U32 destinationX, U32 destinationY, U32 simTicksRemaining);
		void setSprite(const char* assetID, const U32 frame, const F32 sizeX, const F32 sizeY);
		
		bool advanceActionPlan(U32 curDestinationX, U32 curDestinationY, U32 timeUntilArrive);
		bool moveToPosition(const U32 logicalX, const U32 logicalY);
		
		
		inline void recenterInCurrentTile() {  moveToPosition(mLogicalX, mLogicalY); }

		inline void popActionPlan() { ActionPlan* old = nextStep; nextStep = nextStep->nextStep; delete old; }

		inline void cancelEvent(U32& eventHandle) {
			if(!eventHandle) return;
			Sim::cancelEvent(eventHandle);
			eventHandle = 0;
		}

		inline void cancelCurrentMove() {
			mHasTarget = false;
			mTargetX = 0;
			mTargetY = 0;
			//if(nextStep == 0) return;
			if(mArrivedEventID != 0) cancelEvent(mArrivedEventID);
			if(mContinueMoveCheckEventID != 0) cancelEvent(mContinueMoveCheckEventID);
			if(nextStep != 0) deleteActionPlan(nextStep);
			// Optional: recent in current tile?
		}

		inline void updateCurrentLogicalPosition() {
			Vector2 currentPosition = mCompositeSprite->getPosition();
			std::stringstream ss;
			ss << "2Actor updating local coordinates; currently thinks it's at (sprite = " << currentPosition.x << "," << currentPosition.y << ") " << mLogicalX << "," << mLogicalY;
			Con::printf("UCLP1");
			mTileGrid->getLogicalCoordinates(currentPosition.x, currentPosition.y, mLogicalX, mLogicalY, true);
			Con::printf("UCLP2");
			ss << "; after update: " << mLogicalX << "," << mLogicalY;
			Con::printf(ss.str().c_str());
			Con::printf("UCLP3");
		}

		inline bool startRelativeMove(const int relativeX, const int relativeY) {
			Con::printf("StartRelativeMove1");
			//if(nextStep) return false;
			Con::printf("StartRelativeMove2");
			U32 toX, toY;

			if(mTileGrid->getRelativeMove(mLogicalX, mLogicalY, relativeX, relativeY, toX, toY)) {
				Con::printf("StartRelativeMove3");
				if(mHasTarget && (mTargetX == toX && mTargetY == toY)) return true;
				cancelCurrentMove();
				ActionPlan* newPlan = new ActionPlan(toX, toY);
				startActionPlan(newPlan);
				debugActionPlan();
				mHasTarget = true;
				mTargetX = toX;
				mTargetY = toY;
				return true;
			} else {
				cancelCurrentMove();
				Con::printf("StartRelativeMove3-f");
				return false;
			}
		}
		
		inline bool startAbsoluteMove(const U32 toX, const U32 toY) {
			if(mHasTarget && (mTargetX == toX && mTargetY == toY)) return true; // Maybe should be false...
			cancelCurrentMove();
			ActionPlan* newPlan = mTileGrid->getPathToTarget(mLogicalX, mLogicalY, toX, toY);
			if(!newPlan) {
				std::stringstream ss;
				ss << "Actor cannot path from " << mLogicalX << "," << mLogicalY << " to " << toX << "," << toY << "!";
				Con::printf(ss.str().c_str());
				return false; // No path possible
			}
			nextStep = newPlan;
			std::stringstream ss;
			ss << "Actor trying to path";
			Con::printf(ss.str().c_str());
			mHasTarget = true;
			mTargetX = toX;
			mTargetY = toY;
			startActionPlan(newPlan);
			debugActionPlan();
			return true;
		}

		inline bool overrideCurrentMoveRelative(int relativeX, int relativeY) {
			advanceOverrideMoveFlag = false;
			Con::printf("OCMR");
			updateCurrentLogicalPosition();
			//cancelCurrentMove();
			// Pass in 0,0 -> "stop"
			if((!(relativeX == 0 && relativeY == 0)) && startRelativeMove(relativeX, relativeY)) {
				return true;
			} else {
				cancelCurrentMove();
				recenterInCurrentTile();
				Con::executef(this, 2, "onMoveCancelled");
				return false;
			}
		}

		inline bool overrideCurrentMoveAbsolute(const U32 toX, const U32 toY) {
			if(mHasTarget && (mTargetX == toX && mTargetY == toY)) return true; // Maybe should be false...
			advanceOverrideMoveFlag = false;
			Con::printf("Override current move Absolute");
			updateCurrentLogicalPosition();
			
			Con::printf("Trying to override");

			//cancelCurrentMove();
			if(startAbsoluteMove(toX, toY)) {
				return true;
			} else {
				cancelCurrentMove();
				recenterInCurrentTile();
				Con::printf("Trying to execute cancelled move statement - abs");
				Con::executef(this, 2, "onMoveCancelled");
				return false;
			}
		}
		
		inline void arrivedAtDestination(U32 destinationX, U32 destinationY) {
			std::stringstream ss;
			ss << "Actor arrived at destination " << destinationX << ", " << destinationY;
			Con::printf(ss.str().c_str());
			deleteActionPlan(nextStep);
			mLogicalX = destinationX;
			mLogicalY = destinationY;
			Con::executef(this, 2, "onArrived");
			debugActionPlan();
		}

		inline void deleteActionPlan(ActionPlan*& head) { 
			if(head == 0) return;

			deleteActionPlan(head->nextStep);
			delete head;
			head = 0;
		}


		inline void debugActionPlan() {
			std::stringstream ss;
			ss << "Action plan = ";
			ActionPlan* current;
			for(current = nextStep; current != 0; current = current->nextStep) {
				Tile* t = mTileGrid->getTile(current->x, current->y);
				Vector2* position = t->mCenter;
				ss << "<Logical" << current->x << "," << current->y << " -> World" << position->x << "," << position->y << "> ";
			}
			Con::printf(ss.str().c_str());
		}
	
		inline void startActionPlan(ActionPlan* plan) {
			std::stringstream ss;
			ss << "Actor starting action plan";
			Con::printf(ss.str().c_str());
			// deleteActionPlan(nextStep); Should never have to delete...
			nextStep = plan;
			moveToPosition(nextStep->x, nextStep->y);
			debugActionPlan();
		}

		DECLARE_CONOBJECT( Actor );  

	};

		ConsoleMethod(Actor, moveToRelativePosition, bool, 4, 4, "(int logicalX, int logicalX) - Moves this actor to the scene at specified location.\n"
																	"@param logicalX The X coordinate.\n"
																	"@param logicalY The Y coordinate.\n"
																	"@return No return value.") {
			
			if(object->nextStep != 0) {
				return object->overrideCurrentMoveRelative(dAtoi(argv[2]), dAtoi(argv[3]));
			} else {
				return object->startRelativeMove(dAtoi(argv[2]), dAtoi(argv[3]));
			}

		}

		ConsoleMethod(Actor, moveToWorldCoordinates, bool, 4, 5, "(U32 logicalX, U32 logicalX) - Moves this actor to the scene at specified location.\n"
																	"@param logicalX The X coordinate.\n"
																	"@param logicalY The Y coordinate.\n"
																	"@param constrain If true, tries to move to closest tile to the location dragged over.\n"
																	"@return No return value.") {
																		
			U32 logicalX, logicalY;
			bool returnValue = object->mTileGrid->getLogicalCoordinates(dAtof(argv[2]), dAtof(argv[3]), logicalX, logicalY, dAtob(argv[4]));//false);
			
			if(returnValue) {
				
				//object->mTileGrid->spinTile(logicalX, logicalY);

				//if(object->nextStep) return false; 
				if(object->nextStep) return object->overrideCurrentMoveAbsolute(logicalX, logicalY);
				else return object->startAbsoluteMove(logicalX, logicalY);
			} else {
				return false; // No path possible
			}
		}

		ConsoleMethod(Actor, moveToAbsolutePosition, bool, 4, 4, "(U32 logicalX, U32 logicalX) - Moves this actor to the scene at specified location.\n"
																	"@param logicalX The X coordinate.\n"
																	"@param logicalY The Y coordinate.\n"
																	"@return No return value.") {
			if(object->nextStep) {
				return object->overrideCurrentMoveAbsolute(dAtoi(argv[2]), dAtoi(argv[3]));
			} else {
				return object->startAbsoluteMove(dAtoi(argv[2]), dAtoi(argv[3]));
			}

		}

		ConsoleMethod(Actor, addToScene, void, 4, 4, "(U32 logicalX, U32 logicalX) - Adds this actor to the scene at specified location.\n"
																	"@param logicalX The X coordinate.\n"
																	"@param logicalY The Y coordinate.\n"
																	"@return No return value.") {
			object->addToWindow(dAtoi(argv[2]),dAtoi(argv[3]));

		}
		ConsoleMethod(Actor, initializeSprite, void, 6, 6,  "(float strideX, [float strideY]]) - Sets the stride which scales the position at which sprites are created.\n"
                                                            		"@param tileAssetID The asset ID of the tile.\n"
																	"@param frame The frame of the new tile.\n"
                                                                    "@return No return value.") {
			object->setSprite(argv[2], dAtoi(argv[3]), dAtof(argv[4]), dAtof(argv[5]));
																		//object->setTile(std::max(0,dAtoi(argv[2])),std::max(0,dAtoi(argv[3])),argv[4],dAtoi(argv[5]),argv[6]);
		}

#endif