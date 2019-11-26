#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "vcos-core-config.h"

#include "common/code_utils.hpp"
#include "common/thread.hpp"
#include "common/timer.hpp"

#include "net/message.hpp"
#include "net/tasklet.hpp"
#include "net/timer.hpp"

#include "utils/isrpipe.hpp"

typedef struct vcInstance
{
} vcInstance;

namespace vc {

class Instance : public vcInstance
{
public:
    static Instance &Init(void);

    static Instance &Get(void);

    bool IsInitialized(void) const { return mIsInitialized; }

    void Reset(void);

    void Finalize(void);

    template <typename Type> inline Type &Get(void);

private:
    explicit Instance(void);

    void AfterInit(void);

    ThreadScheduler mThreadScheduler;

    TimerScheduler mTimerScheduler;

    Utils::UartIsrpipe mUartIsrpipe;

    Net::MessagePool mNetMessagePool;

    Net::TaskletScheduler mNetTaskletScheduler;

    Net::TimerMilliScheduler mNetTimerMilliScheduler;

    Net::TimerMicroScheduler mNetTimerMicroScheduler;

    bool mIsInitialized;
};

template <> inline ThreadScheduler &Instance::Get(void)
{
    return mThreadScheduler;
}

template <> inline TimerScheduler &Instance::Get(void)
{
    return mTimerScheduler;
}

template <> inline Utils::UartIsrpipe &Instance::Get(void)
{
    return mUartIsrpipe;
}

template <> inline Net::MessagePool &Instance::Get(void)
{
    return mNetMessagePool;
}

template <> inline Net::TaskletScheduler &Instance::Get(void)
{
    return mNetTaskletScheduler;
}

template <> inline Net::TimerMilliScheduler &Instance::Get(void)
{
    return mNetTimerMilliScheduler;
}

template <> inline Net::TimerMicroScheduler &Instance::Get(void)
{
    return mNetTimerMicroScheduler;
}

} // namespace vc

#endif /* INSTANCE_HPP */
