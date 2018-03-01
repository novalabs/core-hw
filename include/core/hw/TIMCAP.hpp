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
#include "hal_timcap.h"

NAMESPACE_CORE_HW_BEGIN

template <std::size_t E>
struct TIMCAPDriverTraits {};

#if STM32_TIMCAP_USE_TIM1
template <>
struct TIMCAPDriverTraits<1> {
    static constexpr auto driver = &TIMCAPD1;
};
#endif

#if STM32_TIMCAP_USE_TIM2
template <>
struct TIMCAPDriverTraits<2> {
    static constexpr auto driver = &TIMCAPD2;
};
#endif

#if STM32_TIMCAP_USE_TIM3
template <>
struct TIMCAPDriverTraits<3> {
    static constexpr auto driver = &TIMCAPD3;
};
#endif

#if STM32_TIMCAP_USE_TIM4
template <>
struct TIMCAPDriverTraits<4> {
    static constexpr auto driver = &TIMCAPD4;
};
#endif

#if STM32_TIMCAP_USE_TIM5
template <>
struct TIMCAPDriverTraits<5> {
    static constexpr auto driver = &TIMCAPD5;
};
#endif

#if STM32_TIMCAP_USE_TIM14
template <>
struct TIMCAPDriverTraits<14> {
    static constexpr auto driver = &TIMCAPD14;
};
#endif

#if STM32_TIMCAP_USE_TIM15
template <>
struct TIMCAPDriverTraits<15> {
    static constexpr auto driver = &TIMCAPD15;
};
#endif

#if STM32_TIMCAP_USE_TIM17
template <>
struct TIMCAPDriverTraits<17> {
    static constexpr auto driver = &TIMCAPD17;
};
#endif

class TIMCAPMaster {
public:
    using Configuration = ::TIMCAPConfig;
public:
    virtual void
	start() = 0;

    virtual void
	start(const Configuration& config) = 0;

    virtual void
	stop() = 0;

    virtual void
    enable() = 0;

    virtual void
    setFrequency(
        uint32_t frequency
    ) = 0;

    virtual void
    disable() = 0;

    virtual void
    setOverflowCallback(
    	std::function<void()>&& callback
    ) = 0;

    virtual void
	resetOverflowCallback() = 0;
};

template <class _TIMCAP>
class TIMCAPMaster_ : public TIMCAPMaster {
public:
    using TIMCAP = _TIMCAP;
public:
    inline void
 	start() {
     	::timcapStart(TIMCAP::driver, &_configuration);
     }

     inline void
 	start(const Configuration& config) {
     	// Make a (non const) copy of the configuration, as we will override a lot of stuff
     	_configuration = config;

     	::timcapStart(TIMCAP::driver, &_configuration);
    }

     inline void
 	stop() {
     	::timcapStop(TIMCAP::driver);
     }

     inline void
 	setFrequency(
 		uint32_t frequency
 	)
 	{
     	_configuration.frequency = frequency;

		::timcapStart(TIMCAP::driver, &_configuration);
 	}

     inline void
     enable()
     {
 		::timcapEnable(TIMCAP::driver);
     }

     inline void
     disable()
     {
     	::timcapDisable(TIMCAP::driver);
     }

     inline void
 	setOverflowCallback(
 		std::function<void()>&& callback
     )
     {
     	_overflow_callback_impl = std::move(callback);

     	_configuration.overflow_cb = _overflow_callback;
     }

     inline void
     resetOverflowCallback()
     {
     	_configuration.overflow_cb = nullptr;
     }
 private:
     Configuration _configuration;

     static inline void
     _overflow_callback(
     	TIMCAPDriver *TIMCAPp
     )
     {
     	_overflow_callback_impl();
     }

     static std::function<void()> _overflow_callback_impl;
};

template <class _TIMCAP>
std::function<void()>TIMCAPMaster_<_TIMCAP>::_overflow_callback_impl;

class TIMCAPChannel
{
public:
    using Configuration = ::TIMCAPConfig;

    enum class Mode {
    	DISABLED, //!< Disabled
        LOW, //!< Active low
        HIGH //!< Active high
    };

public:
    virtual void
    setMode(Mode level) = 0;

    virtual void
    setPeriodCallback(
    	std::function<void(uint32_t)>&& callback
    ) = 0;

    virtual void
	resetPeriodCallback() = 0;
};

template <class _TIMCAP, std::size_t _CHANNEL>
class TIMCAPChannel_:
    public TIMCAPChannel
{
	static_assert(_CHANNEL < 4, "CHANNEL must be 0 .. 3");
public:
    using TIMCAP = _TIMCAP;
    const int CHANNEL = _CHANNEL;

public:
    inline void
    setMode(Mode mode)
    {
		switch (mode) {
		  case Mode::LOW:
			  const_cast<Configuration*>(TIMCAP::driver->config)->modes[_CHANNEL] = TIMCAP_INPUT_ACTIVE_LOW;
			  break;
		  case Mode::HIGH:
			  const_cast<Configuration*>(TIMCAP::driver->config)->modes[_CHANNEL] = TIMCAP_INPUT_ACTIVE_HIGH;
			  break;
		  default:
			  const_cast<Configuration*>(TIMCAP::driver->config)->modes[_CHANNEL] = TIMCAP_INPUT_DISABLED;
			  break;
		}

		::timcapStart(TIMCAP::driver, TIMCAP::driver->config);
    }

    inline void
	setPeriodCallback(
		std::function<void(uint32_t)>&& callback
    )
    {
    	_capture_callback_impl = std::move(callback);

    	const_cast<Configuration*>(TIMCAP::driver->config)->capture_cb_array[_CHANNEL] = _capture_callback;

		::timcapStart(TIMCAP::driver, TIMCAP::driver->config);
    }

    inline void
    resetPeriodCallback()
    {
    	const_cast<Configuration*>(TIMCAP::driver->config)->capture_cb_array[_CHANNEL] = nullptr;

		::timcapStart(TIMCAP::driver, TIMCAP::driver->config);
    }

private:
    static inline void
	_capture_callback(
    	TIMCAPDriver *TIMCAPp
    )
    {
    	_capture_callback_impl(timcap_lld_get_ccr(TIMCAPp, _CHANNEL));
    }

    static std::function<void(uint32_t)> _capture_callback_impl;
};

template <class _TIMCAP, std::size_t _CHANNEL>
std::function<void(uint32_t)>TIMCAPChannel_<_TIMCAP, _CHANNEL>::_capture_callback_impl;

// --- Aliases -----------------------------------------------------------------

using TIMCAP_1 = TIMCAPDriverTraits<1>;
using TIMCAP_2 = TIMCAPDriverTraits<2>;
using TIMCAP_3 = TIMCAPDriverTraits<3>;
using TIMCAP_4 = TIMCAPDriverTraits<4>;
using TIMCAP_5 = TIMCAPDriverTraits<5>;
using TIMCAP_14 = TIMCAPDriverTraits<14>;
using TIMCAP_15 = TIMCAPDriverTraits<15>;
using TIMCAP_17 = TIMCAPDriverTraits<17>;

NAMESPACE_CORE_HW_END
