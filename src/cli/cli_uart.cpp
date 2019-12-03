#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli/cli_uart.hpp"

#include "utils/isrpipe.hpp"

namespace vc {
namespace Cli {

static const char sCommandPrompt[] = {'>', ' '};
static const char sEraseString[]   = {'\b', ' ', '\b'};
static const char CRNL[]           = {'\r', '\n'};

Uart *Uart::sUartServer;

static vcDEFINE_ALIGNED_VAR(sCliUartRaw, sizeof(Uart), uint64_t);

extern "C" void vcCliUartInit(vcInstance *aInstance)
{
    Instance *instance = static_cast<Instance *>(aInstance);

    Uart::sUartServer = new (&sCliUartRaw) Uart(instance);

    Uart::sUartServer->ThreadCreate(instance);
}

extern "C" void vcCliUartOutputBytes(const uint8_t *aBytes, uint8_t aLength)
{
    Uart::sUartServer->GetInterpreter().OutputBytes(aBytes, aLength);
}

extern "C" void vcCliUartOutputFormat(const char *aFmt, ...)
{
    va_list ap;
    va_start(ap, aFmt);
    Uart::sUartServer->OutputFormat(aFmt, ap);
    va_end(ap);
}

Uart::Uart(Instance *aInstance)
    : mInterpreter(aInstance)
    , mUartThread()
    , mUartThreadPid(KERNEL_PID_UNDEF)
{
    mRxLength   = 0;
    mTxHead     = 0;
    mTxLength   = 0;
    mSendLength = 0;
}

void Uart::ThreadCreate(Instance *aInstance)
{
    mUartThreadPid =
        mUartThread.Create(mUartThreadStack, sizeof(mUartThreadStack), VCOS_CONFIG_THREAD_PRIORITY_MAIN - 1,
                           THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST, UartThreadFunc,
                           static_cast<void *>(aInstance), "uart");
}

void *Uart::UartThreadFunc(void *aArgs)
{
    Instance *instance = static_cast<Instance *>(aArgs);

    char c;

    while (1)
    {
        if (instance->Get<Utils::UartIsrpipe>().Read(&c, 1))
        {
            Uart::sUartServer->ReceiveTask(reinterpret_cast<const uint8_t *>(&c), 1);
        }
    }

    return NULL;
}

void Uart::ReceiveTask(const uint8_t *aBuf, uint16_t aBufLength)
{
    const uint8_t *end;

    end = aBuf + aBufLength;

    for (; aBuf < end; aBuf++)
    {
        switch (*aBuf)
        {
        case '\r':
        case '\n':
            Output(CRNL, sizeof(CRNL));

            if (mRxLength > 0)
            {
                mRxBuffer[mRxLength] = '\0';
                ProcessCommand();
            }

            Output(sCommandPrompt, sizeof(sCommandPrompt));
            break;

        case '\b':
        case 127:
            if (mRxLength > 0)
            {
                Output(sEraseString, sizeof(sEraseString));
                mRxBuffer[--mRxLength] = '\0';
            }
            break;

        default:
            if (mRxLength < kRxBufferSize)
            {
                Output(reinterpret_cast<const char *>(aBuf), 1);
                mRxBuffer[mRxLength++] = static_cast<char>(*aBuf);
            }
            break;
        }
    }
}

int Uart::ProcessCommand(void)
{
    if (mRxBuffer[mRxLength - 1] == '\n')
    {
        mRxBuffer[--mRxLength] = '\0';
    }

    if (mRxBuffer[mRxLength - 1] == '\r')
    {
        mRxBuffer[--mRxLength] = '\0';
    }

    mInterpreter.ProcessLine(mRxBuffer, mRxLength, *this);

    mRxLength = 0;

    return 0;
}

int Uart::Output(const char *aBuf, uint16_t aBufLength)
{
    uint16_t remaining = kTxBufferSize - mTxLength;
    uint16_t tail;

    if (aBufLength > remaining)
    {
        aBufLength = remaining;
    }

    for (int i = 0; i < aBufLength; i++)
    {
        tail            = (mTxHead + mTxLength) % kTxBufferSize;
        mTxBuffer[tail] = *aBuf++;
        mTxLength++;
    }

    Send();

    return aBufLength;
}

int Uart::OutputFormat(const char *aFmt, ...)
{
    char    buf[kMaxLineLength];
    va_list ap;
    va_start(ap, aFmt);
    vsnprintf(buf, sizeof(buf), aFmt, ap);
    va_end(ap);
    return Output(buf, static_cast<uint16_t>(strlen(buf)));
}

int Uart::OutputFormatV(const char *aFmt, va_list aAp)
{
    char buf[kMaxLineLength];
    vsnprintf(buf, sizeof(buf), aFmt, aAp);
    return Output(buf, static_cast<uint16_t>(strlen(buf)));
}

void Uart::Send(void)
{
    while (mTxLength > 0)
    {
        if (mTxLength > kTxBufferSize - mTxHead)
        {
            mSendLength = kTxBufferSize - mTxHead;
        }
        else
        {
            mSendLength = mTxLength;
        }

        if (mSendLength > 0)
        {
            for (uint16_t i = 0; i < mSendLength; i++)
            {
                putchar(mTxBuffer[mTxHead + i]);
                fflush(stdout);
            }
        }

        mTxHead = (mTxHead + mSendLength) % kTxBufferSize;

        mTxLength -= mSendLength;

        mSendLength = 0;
    }
}

} // namespace Cli
} // namespace vc
