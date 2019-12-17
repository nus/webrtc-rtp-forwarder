#include "call_data_status.h"

namespace r5 {

const char *call_status_to_string(CallDataStatus status) {
    switch (status) {
    case CREATE:
        return "CREATE";
    case PROCESS:
        return "PROCESS";
    case FINISH:
        return "FINISH";
    default:
        return "unknown";
    }
}

}
