#include "log.h"

LOG_LEVEL_E log_level = LOG_LEVEL_INFO;

void set_log_level(LOG_LEVEL_E level) {
    log_level = level;
}

LOG_LEVEL_E get_log_level() {
    return log_level;
}
