#ifndef CLI_HPP
#define CLI_HPP

#include <stdint.h>

#include "vcos-core-config.h"

#include "common/code_utils.hpp"
#include "common/instance.hpp"

namespace vc {
namespace Cli {

class Interpreter;
class Server;

struct Command
{
    const char *mName;
    void (Interpreter::*mCommand)(int aArgc, char *aArgv[]);
};

class Interpreter
{
public:
    Interpreter(Instance *aInstance);

    void ProcessLine(char *aBuf, uint16_t aBufLength, Server &aServer);

    static int ParseLong(char *aString, long &aLong);

    static int ParseUnsignedLong(char *aString, unsigned long &aUnsignedLong);

    static int Hex2Bin(const char *aHex, uint8_t *aBin, uint16_t aBinLength);

    void OutputBytes(const uint8_t *aBytes, uint8_t aLength) const;

private:
    enum
    {
        kMaxArgs = 32,
    };

    void ProcessHelp(int aArgc, char *aArgv[]);
    void ProcessPs(int aArgc, char *aArgv[]);
    void ProcessVersion(int aArgc, char *aArgv[]);

    static Interpreter &GetOwner(void);

    static const struct Command sCommands[];
    Server *                    mServer;
    Instance *                  mInstance;
};

} // namespace Cli
} // namespace vc

#endif /* CLI_HPP */
