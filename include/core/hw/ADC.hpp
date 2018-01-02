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
struct ADCDriverTraits {};

#if STM32_ADC_USE_ADC1
template <>
struct ADCDriverTraits<1> {
    static constexpr auto driver = &ADCD1;
};
#endif

#if STM32_ADC_USE_ADC2
template <>
struct ADCDriverTraits<2> {
    static constexpr auto driver = &ADCD2;
};
#endif

#if STM32_ADC_USE_ADC3

template <>
struct ADCDriverTraits<3> {
    static constexpr auto driver = &ADCD3;
};
#endif

#if STM32_ADC_USE_ADC4

template <>
struct ADCDriverTraits<4> {
    static constexpr auto driver = &ADCD4;
};
#endif

class ADC
{
public:
    virtual void
    start() = 0;

    virtual void
    stop() = 0;
};

template <class _ADC>
class ADC_:
    public ADC
{
public:
    using ADC = _ADC;

public:
    inline void
    start()
    {
        ::adcStart(ADC::driver, nullptr);
    }

    inline void
    stop()
    {
        ::adcStop(ADC::driver);
    }
};

/*! \brief ADC
 *
 */
class ADCConversionGroup
{
public:
    using Configuration = ::ADCConversionGroup;

public:
    /*! \brief Callback
     *
     */
    using SampleType      = adcsample_t;
    using ChannelCallback = std::function<void(SampleType)>;

    virtual void
    start(
        const ::ADCConversionGroup& config
    ) = 0;

    virtual void
    stop() = 0;


    /*! \brief Set the channel callback
     *
     */
    virtual void
    setChannelCallback(
        std::size_t     channel,
        ChannelCallback callback
    ) = 0;


    /*! \brief Reset the callback for the channel
     *
     */
    virtual void
    resetChannelCallback(
        std::size_t channel
    ) = 0;
};

template <class _ADC, std::size_t _CHANNELS, std::size_t _DEPTH>
class ADCConversionGroup_:
    public ADCConversionGroup
{
public:
    using ADC = _ADC;

public:
    static ChannelCallback callbacks_impl[_CHANNELS];

public:
    inline void
    start(
        const ::ADCConversionGroup& config
    )
    {
        ::adcStart(ADC::driver, nullptr);
        _adc_conversion_group = config;
        _adc_conversion_group.num_channels = _CHANNELS;
        _adc_conversion_group.end_cb       = _callback;
        ::adcStartConversion(ADC::driver, &_adc_conversion_group, _buffer, _DEPTH);
    }

    inline void
    stop()
    {
        ::adcStopConversion(ADC::driver);
        ::adcStop(ADC::driver);
    }

    inline void
    setChannelCallback(
        std::size_t     channel,
        ChannelCallback callback
    )
    {
        CORE_ASSERT(channel < _CHANNELS);

        callbacks_impl[channel] = callback;
    }

    inline void
    resetChannelCallback(
        std::size_t channel
    )
    {
        CORE_ASSERT(channel < _CHANNELS);

        callbacks_impl[channel] = ChannelCallback();
    }

private:
    static ::ADCConversionGroup _adc_conversion_group;
    SampleType _buffer[_CHANNELS * _DEPTH];

    static void
    _callback(
        ADCDriver*   adcp,
        adcsample_t* buffer,
        size_t       n
    )
    {
        for (std::size_t i = 0; i < _CHANNELS; i++) {
            if (callbacks_impl[i]) {
                callbacks_impl[i](buffer[i]);
            }
        }
    }
};

template <class _ADC, std::size_t _CHANNELS, std::size_t _DEPTH>
ADCConversionGroup::ChannelCallback ADCConversionGroup_<_ADC, _CHANNELS, _DEPTH>::callbacks_impl[_CHANNELS];

template <class _ADC, std::size_t _CHANNELS, std::size_t _DEPTH>
  ::ADCConversionGroup ADCConversionGroup_<_ADC, _CHANNELS, _DEPTH>::_adc_conversion_group;

// --- Aliases -----------------------------------------------------------------

using ADC_1 = ADCDriverTraits<1>;
using ADC_2 = ADCDriverTraits<2>;
using ADC_3 = ADCDriverTraits<3>;
using ADC_4 = ADCDriverTraits<4>;

NAMESPACE_CORE_HW_END
