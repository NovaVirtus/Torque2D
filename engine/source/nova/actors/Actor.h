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
		ActionPlan* mNextStep;
		SpriteMovementPlan* mNextSpriteStep;
		SceneWindow* mSceneWindow;
		
		bool advanceOverrideMoveFlag;

		U32 mArrivedEventID, mContinueMoveCheckEventID;
		U32 mLogicalX, mLogicalY;
		U32 mSpriteID;
		U32 mFrame;

		F32 mSpeed;
		F32 mDepth;

		void addToWindow(const U32 logicalX, const U32 logicalY);
		void setArrivingIn(U32 destinationX, U32 destinationY, U32 simTicksRemaining);
		void setSprite(const char* assetID, const U32 frame, const F32 sizeX, const F32 sizeY);
		
		bool advanceSpriteMovePlan();
		bool advanceActionPlan(U32 timeUntilArrive);

		bool moveToPosition(const U32 logicalX, const U32 logicalY);
		
		bool findCrossingPoints(Point2D* currentTileCenter, const F32 strideX, const F32 strideY, const S32 logicalOffsetX, const S32 logicalOffsetY, Point2D* currentPosition, Point2D* targetPosition, bool & crossingX, Point2D & intersectionX, bool & crossingY, Point2D & intersectionY);
			
		inline F64 DistanceBetween(F64 x1, F64 y1, F64 x2, F64 y2) {
			F64 x = (x1 - x2);
			x = x * x;
			F64 y = (y1 - y2);
			y = y * y;
			return sqrt(x + y);
		}

		// Now that I can find intersections,
		// 1) Check which lines that we'll be crossing (x, y)
		// 2) For each of these, find the intersection point
		// 3) Check if crossing and Y boundary within epsilon distance of each other and if so roll them up into one diagonal crossing
		// 4) Find the distance to this point, calculate time it will take to move there and move at current speed to that point
		// 5) Schedule event at that point to move to next
		/*inline void TestFindIntersection() {
			Point2D result;
			bool returnValue;
			/*stringstream ss;
			returnValue = FindIntersection(0, 0, 5, 0, 3, -10, 3, 10, result);
			ss << "TestFind1: " << returnValue << " yields " << result.x << "," << result.y;
			Con::printf(ss.str().c_str());
			ss.clear();
			returnValue = FindIntersection(0, 0, 0, 5, -5, 3, 5, 3, result);
			/*ss << "TestFind2: " << returnValue << " yields " << result.x << "," << result.y;
			Con::printf(ss.str().c_str());
			ss.clear();
			returnValue = FindIntersection(0, 0, 2, 2, 0, 5, 5, 0, result);
			/*ss << "TestFind3: " << returnValue << " yields " << result.x << "," << result.y;
			Con::printf(ss.str().c_str());
		}*/

		inline bool FindIntersection(F64 oX1, F64 oY1, F64 oX2, F64 oY2, F64 dX1, F64 dY1, F64 dX2, F64 dY2, Point2D & result) {
			F64 epsilon = (F64)0.0001;
			F64 d = (oX1 - oX2) * (dY1 - dY2) - (oY1 - oY2) * (dX1 - dX2);
			F64 absD = (d >= 0? d : -d);
			if(absD < epsilon) return false;
			F64 pre = oX1 * oY2 - oY1 * oX2;
			F64 post = dX1 * dY2 - dY1 * dX2;
			F64 x = (pre * (dX1 - dX2) - (oX1 - oX2) * post) / d;
			F64 y = (pre * (dY1 - dY2) - (oY1 - oY2) * post) / d;
			if (x < (min(oX1, oX2) - epsilon) || x > (max(oX1, oX2) + epsilon) || x < (min(dX1, dX2) - epsilon) || x > (max(dX1, dX2) + epsilon)) return false;
			if (y < (min(oY1, oY2) - epsilon) || y > (max(oY1, oY2) + epsilon) || y < (min(dY1, dY2) - epsilon) || y > (max(dY1, dY2) + epsilon)) return false;
			result.x = x;
			result.y = y;
			return true;
		}

		inline U32 getFrameForOffset(S32 logicalOffsetX, S32 logicalOffsetY) {
			if(logicalOffsetY < 0) {
				if(logicalOffsetX > 0) return 0;
				else if(logicalOffsetX == 0) return 1;
				return 2;
			} else if(logicalOffsetY > 0) {
				if(logicalOffsetX < 0) return 4;
				else if(logicalOffsetX == 0) return 5;
				else return 6;
			} else {
				if(logicalOffsetX < 0) return 3;
				else return 7; // "If offsetX > 0", but we need to have either offsetX or offsetY != 0...
			}
		}

		inline void recenterInCurrentTile() { moveToPosition(mLogicalX, mLogicalY); }//, mLogicalX, 1 + mTileGrid->getTile(mLogicalX, mLogicalY)->mExtraMovementCost); }

		inline void popActionPlan() { ActionPlan* old = mNextStep; mNextStep = mNextStep->nextStep; delete old; }
		inline void popSpritePlan() { SpriteMovementPlan* old = mNextSpriteStep; mNextSpriteStep = mNextSpriteStep->nextStep; delete old; }

		inline void cancelEvent(U32& eventHandle) {
			if(!eventHandle) return;
			Sim::cancelEvent(eventHandle);
			eventHandle = 0;
		}

		inline void cancelCurrentMove() {
			mHasTarget = false;
			mTargetX = 0;
			mTargetY = 0;
			//if(mNextStep == 0) return;
			if(mArrivedEventID != 0) cancelEvent(mArrivedEventID);
			if(mContinueMoveCheckEventID != 0) cancelEvent(mContinueMoveCheckEventID);
			if(mNextStep != 0) deleteActionPlan(mNextStep);
			// Optional: recent in current tile?
		}

		inline void updateCurrentLogicalPosition() {
			Vector2 currentPosition = mCompositeSprite->getPosition();
			mTileGrid->getLogicalCoordinates(currentPosition.x, currentPosition.y, mLogicalX, mLogicalY, true);
			
		}

		inline bool startRelativeMove(const int relativeX, const int relativeY) {
			//if(mNextStep) return false;
			U32 toX, toY;

			if(mTileGrid->getRelativeMove(mLogicalX, mLogicalY, relativeX, relativeY, toX, toY)) {
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
				return false;
			}
		}
		
		inline bool startAbsoluteMove(const U32 toX, const U32 toY) {
			//Con::printf("Trying to start absolute move");
			if(mHasTarget && (mTargetX == toX && mTargetY == toY)) return true; // Maybe should be false...
			cancelCurrentMove();
			ActionPlan* newPlan = mTileGrid->getPathToTarget(mLogicalX, mLogicalY, toX, toY);
			if(!newPlan) {
				return false; // No path possible
			}
			//Con::printf("Trying to start absolute move2");
			mNextStep = newPlan;
			mHasTarget = true;
			mTargetX = toX;
			mTargetY = toY;
			startActionPlan(newPlan);
			debugActionPlan();
			return true;
		}

		inline bool overrideCurrentMoveRelative(int relativeX, int relativeY) {
			advanceOverrideMoveFlag = false;
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
			updateCurrentLogicalPosition();
			
			//cancelCurrentMove();
			if(startAbsoluteMove(toX, toY)) {
				return true;
			} else {
				cancelCurrentMove();
				recenterInCurrentTile();
				Con::executef(this, 2, "onMoveCancelled");
				return false;
			}
		}
		
		inline void arrivedAtDestination(U32 destinationX, U32 destinationY) {
			deleteActionPlan(mNextStep);
			mLogicalX = destinationX;
			mLogicalY = destinationY;
			mHasTarget = false;
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
			ActionPlan* current;
			for(current = mNextStep; current != 0; current = current->nextStep) {
				Tile* t = mTileGrid->getTile(current->x, current->y);
				Point2D* position = t->mCenter;
			}
		}
	
		inline void startActionPlan(ActionPlan* plan) {
			// deleteActionPlan(mNextStep); Should never have to delete...
			//Con::printf("Starting action plan");
			mNextStep = plan;
			moveToPosition(mNextStep->x, mNextStep->y);//, nextStep->speedDivisor);
			debugActionPlan();
		}

		inline void restartPathToTarget() {
			ActionPlan* current = 0;
			U32 toX, toY;
			for(current = mNextStep; ((current != 0) && (current->nextStep != 0)); current = current->nextStep) {
				// Do nothing, currently
			}
			if(current == 0) return;
			toX = current->x;
			toY = current->y;

			cancelCurrentMove();
			startAbsoluteMove(toX, toY);
		}

		DECLARE_CONOBJECT( Actor );  

	};

		ConsoleMethod(Actor, moveToRelativePosition, bool, 4, 4, "(int logicalX, int logicalX) - Moves this actor to the scene at specified location.\n"
																	"@param logicalX The X coordinate.\n"
																	"@param logicalY The Y coordinate.\n"
																	"@return No return value.") {
			
			if(object->mNextStep != 0) {
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

				//if(object->mNextStep) return false; 
				if(object->mNextStep) return object->overrideCurrentMoveAbsolute(logicalX, logicalY);
				else return object->startAbsoluteMove(logicalX, logicalY);
			} else {
				return false; // No path possible
			}
		}

		ConsoleMethod(Actor, moveToAbsolutePosition, bool, 4, 4, "(U32 logicalX, U32 logicalX) - Moves this actor to the scene at specified location.\n"
																	"@param logicalX The X coordinate.\n"
																	"@param logicalY The Y coordinate.\n"
																	"@return No return value.") {
			//Con::printf("Trying to move to abs position");
																		if(object->mNextStep) {
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