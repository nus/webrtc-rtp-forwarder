#pragma once

namespace r5 {

class CallDataInterface {
public:
    virtual void Proceed() = 0;
    ~CallDataInterface() {}
};

}
