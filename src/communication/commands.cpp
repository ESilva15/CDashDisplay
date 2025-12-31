#include "commands.h"

char* CommandToStr(Command id) {
  switch(id) {
    case CmdRequestID:
      return (char*)"CmdRequestID";
    case CmdAckID:
      return (char*)"CmdAckID";
    case CmdCreateWindow:
      return (char*)"CmdCreateWindow";
    case CmdDestroyWindow:
      return (char*)"CmdDestroyWindow";
    default:
      return (char*)"CmdUnknown";
  }
}
