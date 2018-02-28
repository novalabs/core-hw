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
struct ICUDriverTraits {};

#if STM32_ICU_USE_TIM1
template <>
struct ICUDriverTraits<1> {
    static constexpr auto driver = &ICUD1;
};
#endif

#if STM32_ICU_USE_TIM2
template <>
struct ICUDriverTraits<2> {
    static constexpr auto driver = &ICUD2;
};
#endif

#if STM32_ICU_USE_TIM3
template <>
struct ICUDriverTraits<3> {
    static constexpr auto driver = &ICUD3;
};
#endif

#if STM32_ICU_USE_TIM4
template <>
struct ICUDriverTraits<4> {
    static constexpr auto driver = &ICUD4;
};
#endif

#if STM32_ICU_USE_TIM5
template <>
struct ICUDriverTraits<5> {
    static constexpr auto driver = &ICUD5;
};
#endif

#if STM32_ICU_USE_TIM14
template <>
struct ICUDriverTraits<14> {
    static constexpr auto driver = &ICUD14;
};
#endif

#if STM32_ICU_USE_TIM15
template <>
struct ICUDriverTraits<15> {
    static constexpr auto driver = &ICUD15;
};
#endif

class ICUChannel
{
public:
    using Configuration = ::ICUConfig;

    /*! \brief Level selection
     *
     */
    enum class Level {
        LOW, //!< Active low
        HIGH //!< Active high
    };

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
    setLevel(Level level) = 0;

    virtual void
    setFrequency(
        uint32_t frequency
    ) = 0;

    virtual void
    disable() = 0;

    virtual void
	enableCallbacks() = 0;

    virtual void
	disableCallbacks() = 0;

    virtual void
    setWidthCallback(
    	std::function<void(uint32_t)>&& callback
    ) = 0;

    virtual void
	resetWidthCallback() = 0;

    virtual void
    setPeriodCallback(
    	std::function<void(uint32_t)>&& callback
    ) = 0;

    virtual void
	resetPeriodCallback() = 0;

    virtual void
    setOverflowCallback(
    	std::function<void()>&& callback
    ) = 0;

    virtual void
	resetOverflowCallback() = 0;
};

template <class _ICU, std::size_t _CHANNEL>
class ICUChannel_:
    public ICUChannel
{
	static_assert(_CHANNEL > 0 && _CHANNEL < 3, "CHANNEL must be 1 or 2");
public:
    using ICU = _ICU;
    const int CHANNEL = _CHANNEL;

public:
    ICUChannel_()
	{
    	switch(CHANNEL) {
			case 1:
				_configuration.channel = ICU_CHANNEL_1;
				break;
			case 2:
				_configuration.channel = ICU_CHANNEL_2;
				break;
			default:
				break;
    	}
    }

    inline void
	start() {
    	::icuStart(ICU::driver, &_configuration);
    }

    inline void
	start(const Configuration& config) {
    	// Make a (non const) copy of the configuration, as we will override a lot of stuff
    	_configuration = config;

    	// Let's start by overriding the channel
    	switch(CHANNEL) {
			case 1:
				_configuration.channel = ICU_CHANNEL_1;
				break;
			case 2:
				_configuration.channel = ICU_CHANNEL_2;
				break;
			default:
				break;
    	}

    	::icuStart(ICU::driver, &_configuration);
    }

    inline void
	stop() {
    	::icuStop(ICU::driver);
    }

    inline void
    setLevel(Level level)
    {
		switch (level) {
		  case Level::LOW:
			  _configuration.mode = ICU_INPUT_ACTIVE_LOW;
			  break;
		  case Level::HIGH:
			  _configuration.mode = ICU_INPUT_ACTIVE_HIGH;
		  default:
			  break;
		}

		if (ICU::driver->state == ICU_READY) {
			stop();
			::icuStart(ICU::driver, &_configuration);
		}
    }

    inline void
	setFrequency(
		uint32_t frequency
	)
	{
    	_configuration.frequency = frequency;

		if (ICU::driver->state == ICU_READY) {
			stop();
			::icuStart(ICU::driver, &_configuration);
		}
	}

    inline void
    enable()
    {
		::icuStartCapture(ICU::driver);
    }

    inline void
    disable()
    {
    	::icuStopCapture(ICU::driver);
    }

    inline void
	enableCallbacks() {
    	::icuEnableNotifications(ICU::driver);
    }

    void
	disableCallbacks() {
    	::icuDisableNotifications(ICU::driver);
    }

    inline void
	setWidthCallback(
		std::function<void(uint32_t)>&& callback
    )
    {
    	_width_callback_impl = std::move(callback);

    	_configuration.width_cb = _width_callback;
    }

    inline void
    resetWidthCallback()
    {
    	_configuration.width_cb = nullptr;
    }

    inline void
	setPeriodCallback(
		std::function<void(uint32_t)>&& callback
    )
    {
    	_period_callback_impl = std::move(callback);

    	_configuration.period_cb = _period_callback;
    }

    inline void
    resetPeriodCallback()
    {
    	_configuration.period_cb = nullptr;
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
    _width_callback(
    	ICUDriver *icup
    )
    {
    	_width_callback_impl(icuGetWidthX(icup));
    }

    static inline void
    _period_callback(
    	ICUDriver *icup
    )
    {
    	_period_callback_impl(icuGetPeriodX(icup));
    }

    static inline void
    _overflow_callback(
    	ICUDriver *icup
    )
    {
    	_overflow_callback_impl();
    }

    static std::function<void(uint32_t)> _width_callback_impl;
    static std::function<void(uint32_t)> _period_callback_impl;
    static std::function<void()> _overflow_callback_impl;
};

template <class _ICU, std::size_t _CHANNEL>
std::function<void(uint32_t)>ICUChannel_<_ICU, _CHANNEL>::_width_callback_impl;

template <class _ICU, std::size_t _CHANNEL>
std::function<void(uint32_t)>ICUChannel_<_ICU, _CHANNEL>::_period_callback_impl;

template <class _ICU, std::size_t _CHANNEL>
std::function<void()>ICUChannel_<_ICU, _CHANNEL>::_overflow_callback_impl;
// --- Aliases -----------------------------------------------------------------

using ICU_1 = ICUDriverTraits<1>;
using ICU_2 = ICUDriverTraits<2>;
using ICU_3 = ICUDriverTraits<3>;
using ICU_4 = ICUDriverTraits<4>;
using ICU_5 = ICUDriverTraits<5>;
using ICU_14 = ICUDriverTraits<14>;
using ICU_15 = ICUDriverTraits<15>;

NAMESPACE_CORE_HW_END
