#pragma once

namespace r5 {

enum CallDataStatus {
    CREATE,
    PROCESS,
    FINISH
};

const char *call_status_to_string(CallDataStatus status);

}
