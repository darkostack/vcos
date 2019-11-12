#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vcos/instance.h>

#include "cli/cli.hpp"
#include "cli/cli_uart.hpp"

namespace vc {
namespace Cli {

const struct Command Interpreter::sCommands[] = {
    {"help", &Interpreter::ProcessHelp},
    {"version", &Interpreter::ProcessVersion},
};

Interpreter::Interpreter(Instance *aInstance)
    : mServer(NULL)
    , mInstance(aInstance)
{
    (void) aInstance;
}

int Interpreter::Hex2Bin(const char *aHex, uint8_t *aBin, uint16_t aBinLength)
{
    size_t hexLength = strlen(aHex);
    const char *hexEnd = aHex + hexLength;
    uint8_t *cur = aBin;
    uint8_t numChars = hexLength & 1;
    uint8_t byte = 0;
    int rval;

    VerifyOrExit((hexLength + 1) / 2 <= aBinLength, rval = -1);

    while (aHex < hexEnd) {
        if ('A' <= *aHex && *aHex <= 'F') {
            byte |= 10 + (*aHex - 'A');
        } else if ('a' <= *aHex && *aHex <= 'f') {
            byte |= 10 + (*aHex - 'a');
        } else if ('0' <= *aHex && *aHex <= '9') {
            byte |= *aHex - '0';
        } else {
            ExitNow(rval = -1);
        }

        aHex++;
        numChars++;

        if (numChars >= 2) {
            numChars = 0;
            *cur++   = byte;
            byte     = 0;
        } else {
            byte <<= 4;
        }
    }

    rval = static_cast<int>(cur - aBin);

exit:
    return rval;
}

void Interpreter::OutputBytes(const uint8_t *aBytes, uint8_t aLength) const
{
    for (int i = 0; i < aLength; i++) {
        mServer->OutputFormat("%02x", aBytes[i]);
    }
}

int Interpreter::ParseLong(char *aString, long &aLong)
{
    char *endptr;
    aLong = strtol(aString, &endptr, 0);
    return (*endptr == '\0') ? 1 : 0;
}

int Interpreter::ParseUnsignedLong(char *aString, unsigned long &aUnsignedLong)
{
    char *endptr;
    aUnsignedLong = strtoul(aString, &endptr, 0);
    return (*endptr == '\0') ? 1 : 0;
}

void Interpreter::ProcessHelp(int aArgc, char *aArgv[])
{
    (void) aArgc;
    (void) aArgv;

    for (unsigned int i = 0; i < VC_ARRAY_LENGTH(sCommands); i++) {
        mServer->OutputFormat("%s\r\n", sCommands[i].mName);
    }
}

void Interpreter::ProcessVersion(int aArgc, char *aArgv[])
{
    (void) aArgc;
    (void) aArgv;

    const char *version = vcInstanceGetVersionString();
    mServer->OutputFormat("%s\r\n", static_cast<const char *>(version));
}

void Interpreter::ProcessLine(char *aBuf, uint16_t aBufLength, Server &aServer)
{
    char *argv[kMaxArgs];
    char *cmd;
    uint8_t argc = 0, i = 0;

    mServer = &aServer;

    VerifyOrExit(aBuf != NULL);

    for (; *aBuf == ' '; aBuf++, aBufLength--)
        ;

    for (cmd = aBuf + 1; (cmd < aBuf + aBufLength) && (cmd != NULL); ++cmd) {
        VerifyOrExit(argc < kMaxArgs, mServer->OutputFormat("Error: too many args (max %d)\r\n", kMaxArgs));
        if (*cmd == ' ' || *cmd == '\r' || *cmd == '\n') {
            *cmd = '\0';
        }
        if (*(cmd - 1) == '\0' && *cmd != ' ') {
            argv[argc++] = cmd;
        }
    }

    cmd = aBuf;

    for (i = 0; i < VC_ARRAY_LENGTH(sCommands); i++) {
        if (strcmp(cmd, sCommands[i].mName) == 0) {
            (this->*sCommands[i].mCommand)(argc, argv);
            break;
        }
    }

    if (i == VC_ARRAY_LENGTH(sCommands)) {
        mServer->OutputFormat("Unknown command: %s\r\n", cmd);
    } else {
        mServer->OutputFormat("Done\r\n");
    }

exit:
    return;
}

Interpreter &Interpreter::GetOwner(void)
{
    Interpreter &interpreter = Uart::sUartServer->GetInterpreter();
    return interpreter;
}

} // namespace vc
} // namespace vc
