#ifndef NET_NOTIFIER_HPP
#define NET_NOTIFIER_HPP

#include "vcos-core-config.h"

#include <stdbool.h>
#include <stdint.h>

#include <vcos/net/types.h>

#include "common/locator.hpp"

#include "net/tasklet.hpp"

namespace vc {
namespace Net {

class Notifier : public InstanceLocator
{
public:
    class Callback
    {
        friend class Notifier;

    public:
        typedef void (*Handler)(Callback &aCallback, vcNetChangedFlags aFlags);

        Callback(Instance &aInstance, Handler aHandler);

    private:
        void Invoke(vcNetChangedFlags aFlags) { mHandler(*this, aFlags); }

        Handler   mHandler;
        Callback *mNext;
    };

    explicit Notifier(void);

    vcNetError RegisterCallback(vcNetStateChangedCallback aCallback, void *aContext);

    void RemoveCallback(vcNetStateChangedCallback aCallback, void *aContext);

    void Signal(vcNetChangedFlags aFlags);

    void SignalIfFirst(vcNetChangedFlags aFlags);

    bool IsPending(void) const { return (mFlagsToSignal != 0); }

    bool HasSignaled(vcNetChangedFlags aFlags) const { return (mSignaledFlags & aFlags) == aFlags; }

private:
    enum
    {
        kMaxExternalHandlers  = VCOS_NET_CONFIG_MAX_STATECHANGE_HANDLERS,
        kFlagsStringLineLimit = 70,
        kMaxFlagNameLength    = 25,
        kFlagStringBufferSize = kFlagsStringLineLimit + kMaxFlagNameLength,
    };

    struct ExternalCallback
    {
        vcNetStateChangedCallback mHandler;
        void *                    mContext;
    };

    void        RegisterCallback(Callback &aCallback);
    static void HandleStateChanged(Tasklet &aTasklet);
    void        HandleStateChanged(void);

    void        LogChangedFlags(vcNetChangedFlags aFlag) const;
    const char *FlagToString(vcNetChangedFlags aFlag) const;

    vcNetChangedFlags mFlagsToSignal;
    vcNetChangedFlags mSignaledFlags;
    Tasklet           mTask;
    Callback *        mCallbacks;
    ExternalCallback  mExternalCallbacks[kMaxExternalHandlers];
};

} // namespace Net
} // namespace vc

#endif /* NET_NOTIFIER_HPP */
