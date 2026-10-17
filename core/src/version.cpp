#include "di/version.hpp"

#ifndef DI_VERSION
#error "DI_VERSION must be defined by the build system"
#endif

namespace di {

std::string_view version() noexcept {
    return DI_VERSION;
}

} // namespace di
