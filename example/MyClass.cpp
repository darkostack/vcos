#include "MyClass.h"
#include "OtherClass.h"

namespace example {

int MyClass::myFunction() {
    OtherClass o = OtherClass();
    return o.otherFunction();
}

}
