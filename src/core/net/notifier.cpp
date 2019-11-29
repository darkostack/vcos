#include "net/notifier.hpp"

#include "common/code_utils.hpp"
#include "common/locator-getters.hpp"

namespace vc {
namespace Net {

Notifier::Callback::Callback(Instance &aInstance, Handler aHandler)
    : mHandler(aHandler)
    , mNext(NULL)
{
    assert(aHandler != NULL);
    aInstance.Get<Notifier>().RegisterCallback(*this);
}

Notifier::Notifier(void)
    : mFlagsToSignal(0)
    , mSignaledFlags(0)
    , mTask(&Notifier::HandleStateChanged)
    , mCallbacks(NULL)
{
    for (unsigned int i = 0; i < kMaxExternalHandlers; i++)
    {
        mExternalCallbacks[i].mHandler = NULL;
        mExternalCallbacks[i].mContext = NULL;
    }
}

void Notifier::RegisterCallback(Callback &aCallback)
{
    aCallback.mNext = mCallbacks;
    mCallbacks      = &aCallback;
}

vcNetError Notifier::RegisterCallback(vcNetStateChangedCallback aCallback, void *aContext)
{
    vcNetError        error          = VC_NET_ERROR_NONE;
    ExternalCallback *unusedCallback = NULL;

    VerifyOrExit(aCallback != NULL);

    for (unsigned int i = 0; i < kMaxExternalHandlers; i++)
    {
        ExternalCallback &callback = mExternalCallbacks[i];

        if (callback.mHandler == NULL)
        {
            if (unusedCallback == NULL)
            {
                unusedCallback = &callback;
            }

            continue;
        }

        VerifyOrExit((callback.mHandler != aCallback) || (callback.mContext != aContext), error = VC_NET_ERROR_ALREADY);
    }

    VerifyOrExit(unusedCallback != NULL, error = VC_NET_ERROR_NO_BUFS);

    unusedCallback->mHandler = aCallback;
    unusedCallback->mContext = aContext;

exit:
    return error;
}

void Notifier::RemoveCallback(vcNetStateChangedCallback aCallback, void *aContext)
{
    VerifyOrExit(aCallback != NULL);

    for (unsigned int i = 0; i < kMaxExternalHandlers; i++)
    {
        ExternalCallback &callback = mExternalCallbacks[i];

        if ((callback.mHandler == aCallback) && (callback.mContext == aContext))
        {
            callback.mHandler = NULL;
            callback.mContext = NULL;
        }
    }

exit:
    return;
}

void Notifier::Signal(vcNetChangedFlags aFlags)
{
    mFlagsToSignal |= aFlags;
    mSignaledFlags |= aFlags;
    mTask.Post();
}

void Notifier::SignalIfFirst(vcNetChangedFlags aFlags)
{
    if (!HasSignaled(aFlags))
    {
        Signal(aFlags);
    }
}

void Notifier::HandleStateChanged(Tasklet &aTasklet)
{
    aTasklet.Get<Notifier>().HandleStateChanged();
}

void Notifier::HandleStateChanged(void)
{
    vcNetChangedFlags flags = mFlagsToSignal;

    VerifyOrExit(flags != 0);

    mFlagsToSignal = 0;

    LogChangedFlags(flags);

    for (Callback *callback = mCallbacks; callback != NULL; callback = callback->mNext)
    {
        callback->Invoke(flags);
    }

    for (unsigned int i = 0; i < kMaxExternalHandlers; i++)
    {
        ExternalCallback &callback = mExternalCallbacks[i];

        if (callback.mHandler != NULL)
        {
            callback.mHandler(flags, callback.mContext);
        }
    }

exit:
    return;
}

void Notifier::LogChangedFlags(vcNetChangedFlags) const
{
}

const char *Notifier::FlagToString(vcNetChangedFlags) const
{
    return "";
}

} // namespace Net
} // namespace vc
