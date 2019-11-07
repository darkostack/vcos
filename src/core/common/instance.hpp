#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "vcos-core-config.h"

#include "common/code_utils.hpp"

typedef struct vcInstance
{
} vcInstance;

namespace vc {

class Instance : public vcInstance
{
public:
    static Instance &InitSingle(void);
    
    static Instance &Get(void);

    bool IsInitialized(void) const { return mIsInitialized; }

    void Reset(void);

    void Finalize(void);

private:
    Instance(void);

    void AfterInit(void);

    bool mIsInitialized;
};

} // namespace vc

#endif /* INSTANCE_HPP */
