#include "helpers.h"

#include <rtc_base/helpers.h>

namespace r5 {

std::string CreatePeerId() {
    return std::move(::rtc::CreateRandomUuid());
}

}
