/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/hw/namespace.hpp>
#include <core/hw/common.hpp>

#include <functional>

#include "hal.h"

NAMESPACE_CORE_HW_BEGIN

template <std::size_t E>
struct PWMDriverTraits {};

#if STM32_PWM_USE_TIM1
template <>
struct PWMDriverTraits<1> {
    static constexpr auto driver = &PWMD1;
};
#endif

#if STM32_PWM_USE_TIM2
template <>
struct PWMDriverTraits<2> {
    static constexpr auto driver = &PWMD2;
};
#endif

#if STM32_PWM_USE_TIM3
template <>
struct PWMDriverTraits<3> {
    static constexpr auto driver = &PWMD3;
};
#endif

#if STM32_PWM_USE_TIM4
template <>
struct PWMDriverTraits<4> {
    static constexpr auto driver = &PWMD4;
};
#endif

#if STM32_PWM_USE_TIM5
template <>
struct PWMDriverTraits<5> {
    static constexpr auto driver = &PWMD5;
};
#endif

#if STM32_PWM_USE_TIM15
template <>
struct PWMDriverTraits<15> {
    static constexpr auto driver = &PWMD15;
};
#endif

class PWMMaster
{
public:
    using Configuration = ::PWMConfig;

public:
    virtual void
    start(
        const Configuration& config
    ) = 0;

    virtual void
    stop() = 0;

    virtual void
    setFrequency(
        uint32_t frequency
    ) = 0;

    virtual void
    setPeriod(
        uint32_t period
    ) = 0;

    virtual uint32_t
    getFrequency() = 0;

    virtual uint32_t
    getPeriod() = 0;


    virtual void
    setCallback(
        std::function<void()>callback
    ) = 0;

    virtual void
    resetCallback() = 0;

    virtual void
	enableCallback() = 0;

    virtual void
	disableCallback() = 0;
};


template <class _PWM>
class PWMMaster_:
    public
    PWMMaster
{
public:
    using PWM = _PWM;
    using CountDataType = pwmcnt_t;

public:
    static std::function<void()> callback_impl;

    inline void
    start(
        const Configuration& config
    )
    {
        ::pwmStart(PWM::driver, &config);
    }

    inline void
    stop()
    {
        ::pwmStop(PWM::driver);
    }

    inline void
    setFrequency(
        uint32_t frequency
    )
    {
        const_cast<Configuration*>(PWM::driver->config)->frequency = frequency;

        if (PWM::driver->state == PWM_READY) {
            stop();
            ::pwmStart(PWM::driver, PWM::driver->config);
        }
    }

    inline void
    setPeriod(
        CountDataType period
    )
    {
        ::pwmChangePeriod(PWM::driver, period);
    }

    inline uint32_t
    getFrequency()
    {
        return PWM::driver->config->frequency;
    }

    inline uint32_t
    getPeriod()
    {
        return PWM::driver->config->period;
    }

    inline void
    setCallback(
        std::function<void()>callback
    )
    {
        callback_impl = callback;

        const_cast<PWMConfig*>(PWM::driver->config)->callback = _callback;
    }

    inline void
    resetCallback()
    {
        const_cast<PWMConfig*>(PWM::driver->config)->callback = nullptr;
    }

    inline void
	enableCallback() {
        ::pwmEnablePeriodicNotification(PWM::driver);
    }

    inline void
	disableCallback() {
        ::pwmDisablePeriodicNotification(PWM::driver);
    }
private:
    static inline void
    _callback(
        PWMDriver* pwmp
    )
    {
        callback_impl();
    }
};

template <class _PWM>
std::function<void()>PWMMaster_<_PWM>::callback_impl;


class PWMChannel
{
public:
    using CountDataType = pwmcnt_t;

    virtual void
    enable() = 0;

    virtual void
    disable() = 0;

    virtual bool
    set(
        CountDataType value
    ) = 0;

    virtual bool
    setI(
        CountDataType value
    ) = 0;

    virtual uint32_t
    getPeriod() = 0;
};

template <class _PWM, std::size_t _CHANNEL>
class PWMChannel_:
    public PWMChannel
{
public:
    using PWM = _PWM;
    const int CHANNEL = _CHANNEL;

public:
    inline void
    enable()
    {
        ::pwmEnableChannel(PWM::driver, CHANNEL, 0);
    }

    inline void
    disable()
    {
        ::pwmDisableChannel(PWM::driver, CHANNEL);
    }

    inline bool
    set(
        CountDataType value
    )
    {
        ::pwmEnableChannel(PWM::driver, CHANNEL, value);
        return true;
    }

    inline bool
    setI(
        CountDataType value
    )
    {
        pwmEnableChannelI(PWM::driver, CHANNEL, value);
        return true;
    }

    inline uint32_t
    getPeriod()
    {
        return PWM::driver->config->period;
    }
};

// --- Aliases -----------------------------------------------------------------

using PWM_1 = PWMDriverTraits<1>;
using PWM_2 = PWMDriverTraits<2>;
using PWM_3 = PWMDriverTraits<3>;
using PWM_4 = PWMDriverTraits<4>;
using PWM_5 = PWMDriverTraits<5>;
using PWM_15 = PWMDriverTraits<15>;

NAMESPACE_CORE_HW_END
