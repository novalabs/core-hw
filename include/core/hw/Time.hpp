/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/hw/namespace.hpp>
#include <core/hw/common.hpp>

#include "hal.h"

#include <ctime>

// std::timespec is C++17
namespace std {
using ::timespec;
}

NAMESPACE_CORE_HW_BEGIN

#if MAC_USE_PTP

class PTPTime
{
public:
    static void
    saveToRTC();

    static void
    loadFromRTC();

    static void
    get(
        std::timespec* time
    );

    static void
    get(
        std::timespec& time
    );

    static void
    set(
        const std::timespec* time
    );

    static void
    set(
        const std::timespec& time
    );
};
#endif // if MAC_USE_PTP

NAMESPACE_CORE_HW_END
