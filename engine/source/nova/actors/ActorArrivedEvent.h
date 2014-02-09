//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _ACTOR_ARRIVED_EVENT_H_
#define _ACTOR_ARRIVED_EVENT_H_

#ifndef _ACTOR_H_
#include "Actor.h"
#endif

//-----------------------------------------------------------------------------

class ActorArrivedEvent : public SimEvent {
private:
	U32 mLogicalX;
	U32 mLogicalY;
public:
	ActorArrivedEvent(const U32 logicalX, const U32 logicalY) {
		mLogicalX = logicalX;
		mLogicalY = logicalY;
	}

    virtual ~ActorArrivedEvent() {}

    virtual void process(SimObject *object) {
		Actor* curActor = (Actor*) object;
		curActor->arrivedAtDestination(mLogicalX, mLogicalY);
    }

};

#endif // _SCENE_OBJECT_MOVE_TO_EVENT_H_