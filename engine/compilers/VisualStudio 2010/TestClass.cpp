#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "TestClass.h"
IMPLEMENT_CONOBJECT(TestClass);  

void TestClass::copyTo( SimObject* object )  
{  
    // Fetch other object.  
   TestClass* pTestClass = static_cast<TestClass*>( object );  
  
   // Sanity!  
   AssertFatal( pTestClass != NULL, "TestClass::copyTo() - Object is not the correct type.");  
  
   // Copy parent.  
   Parent::copyTo( object );  
  
   // Copy the state.  
   pTestClass->mEmitLight = mEmitLight;  
   pTestClass->mBrightness = mBrightness;  
} 

bool TestClass::onAdd()  
{  
    // Fail if the parent fails.  
    if ( !Parent::onAdd() )  
        return false;  
  
    // Do some work here.  
    Con::printf("Hello from TestClass!");  
}  
  
void TestClass::onRemove()  
{  
    // Do some work here.  
} 

TestClass::TestClass()  
{  
    mEmitLight = false;  
    mBrightness = 1.0f;  
}  
  
void TestClass::initPersistFields()  
{  
    // Call parent.  
    Parent::initPersistFields();  
  
    // Add my fields here.  
    addField("EmitLight", TypeBool, Offset(mEmitLight, TestClass), "Flags whether the light is on or off.");  
    addField("Brightness", TypeF32, Offset(mBrightness, TestClass), "Sets the brightness of the light.");   
} 