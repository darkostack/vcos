#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "vcos-core-config.h"

#include "common/code_utils.hpp"
#include "common/thread.hpp"

#include "utils/isrpipe.hpp"

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

    template <typename Type> inline Type &Get(void);

private:
    Instance(void);

    void AfterInit(void);

    ThreadScheduler mThreadScheduler;

    Utils::UartIsrpipe mUartIsrpipe;

    bool mIsInitialized;
};

template <> inline ThreadScheduler &Instance::Get(void)
{
    return mThreadScheduler;
}

template <> inline Utils::UartIsrpipe &Instance::Get(void)
{
    return mUartIsrpipe;
}

} // namespace vc

#endif /* INSTANCE_HPP */
