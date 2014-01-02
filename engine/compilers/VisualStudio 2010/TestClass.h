#ifndef _SIMBASE_H_  
#include "sim/simBase.h"  
#endif  
#ifndef _TEST_CLASS_H_
#define _TEST_CLASS_H_

class TestClass : public SimObject  
    {  
    private:  
        typedef SimObject Parent;  
      
        bool mEmitLight;  
        F32 mBrightness;  
          
    public:  
        TestClass();
        virtual ~TestClass() {}  
        virtual bool onAdd();  
        virtual void onRemove();  
          
        virtual void copyTo(SimObject* object);  
          
        static void initPersistFields();  
          
        DECLARE_CONOBJECT( TestClass );  
    };  
#endif