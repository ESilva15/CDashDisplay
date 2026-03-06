#ifndef __COMMANDS__
#define __COMMANDS__

#include <stdint.h>

typedef enum : uint8_t {
    CmdUnknown = 0,
    CmdRequestID = 1,
    CmdAckID = 2,
    CmdCreateWindow = 3,
    CmdDestroyWindow = 4,
    CmdUpdateWinDims = 5, // NOTE: change this to a move cmd instead
    CmdUpdateWin = 6,
    CMDData = 7,
} Command;

char* CommandToStr(Command id);

#endif
