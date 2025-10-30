#ifndef __COMMANDS__
#define __COMMANDS__

#include <stdint.h>

typedef enum : uint8_t {
    CmdRequestID = 0,
    CmdAckID,
    CmdCreateScreen,
    CmdCreateWindow
} Command;


#endif
