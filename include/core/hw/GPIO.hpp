/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/hw/namespace.hpp>
#include <core/hw/common.hpp>

#include "hal.h"

NAMESPACE_CORE_HW_BEGIN

template <std::size_t N>
struct GPIODriverTraits {};

template <>
struct GPIODriverTraits<0> {
    static constexpr auto driver = reinterpret_cast<uint32_t>(GPIOA_BASE);
};

template <>
struct GPIODriverTraits<1> {
    static constexpr auto driver = reinterpret_cast<uint32_t>(GPIOB_BASE);
};

template <>
struct GPIODriverTraits<2> {
    static constexpr auto driver = reinterpret_cast<uint32_t>(GPIOC_BASE);
};

template <>
struct GPIODriverTraits<3> {
    static constexpr auto driver = reinterpret_cast<uint32_t>(GPIOD_BASE);
};

template <>
struct GPIODriverTraits<4> {
    static constexpr auto driver = reinterpret_cast<uint32_t>(GPIOE_BASE);
};

template <>
struct GPIODriverTraits<5> {
    static constexpr auto driver = reinterpret_cast<uint32_t>(GPIOF_BASE);
};

/*! \brief Pad interface
 *
 */
struct Pad {
    /*! \brief Pad modes
     *
     */
    enum class Mode : iomode_t {
        RESET            = PAL_MODE_RESET, //!< Reser state
        INPUT            = PAL_MODE_INPUT, //!< Digital floating input
        INPUT_PULLUP     = PAL_MODE_INPUT_PULLUP, //!< Digital input with pull up
        INPUT_PULLDOWN   = PAL_MODE_INPUT_PULLDOWN, //!< Digital input with pull down
        INPUT_ANALOG     = PAL_MODE_INPUT_ANALOG, //!< Analog input
        OUTPUT_PUSHPULL  = PAL_MODE_OUTPUT_PUSHPULL, //!< Digital push-pull output
        OUTPUT_OPENDRAIN = PAL_MODE_OUTPUT_OPENDRAIN, //!< Digital open-drain output
        ALTERNATE_0      = PAL_MODE_ALTERNATE(0),
        ALTERNATE_1      = PAL_MODE_ALTERNATE(1),
        ALTERNATE_2      = PAL_MODE_ALTERNATE(2),
        ALTERNATE_3      = PAL_MODE_ALTERNATE(3),
        ALTERNATE_4      = PAL_MODE_ALTERNATE(4),
        ALTERNATE_5      = PAL_MODE_ALTERNATE(5),
        ALTERNATE_6      = PAL_MODE_ALTERNATE(6),
        ALTERNATE_7      = PAL_MODE_ALTERNATE(7),
        ALTERNATE_8      = PAL_MODE_ALTERNATE(8),
        ALTERNATE_9      = PAL_MODE_ALTERNATE(9),
        ALTERNATE_10     = PAL_MODE_ALTERNATE(10),
        ALTERNATE_11     = PAL_MODE_ALTERNATE(11),
        ALTERNATE_12     = PAL_MODE_ALTERNATE(12),
        ALTERNATE_13     = PAL_MODE_ALTERNATE(13),
        ALTERNATE_14     = PAL_MODE_ALTERNATE(14),
        ALTERNATE_15     = PAL_MODE_ALTERNATE(15)
    };

    /*! \brief Set the pad
     *
     */
    virtual void
    set() = 0;


    /*! \brief Clear the pad
     *
     */
    virtual void
    clear() = 0;


    /*! \brief Toggle the pad
     *
     */
    virtual void
    toggle() = 0;


    /*! \brief Write a value to the pad
     *
     */
    virtual void
    write(
        bool high //!< [in] pad high
    ) = 0;


    /*! \brief Read pad level
     *
     * \return pad level
     * \retval true the pad is high
     * \retval false the pad is low
     */
    virtual bool
    read() = 0;


    /*! \brief Set pad mode
     *
     */
    virtual void
    setMode(
        Mode mode //!< mode
    ) = 0;

    /*! \brief Set pad default mode
     *
     */
    virtual void
    setDefaultMode() = 0;

    /*! \brief Set pad alternate mode
     *
     */
    virtual void
    setAlternateMode() = 0;

    virtual bool
	isNC() = 0;
};

/*! \brief Pad
 *
 * \tparam _GPIO GPIODriverTraits driver
 * \tparam _PAD pad
 * \tparam _DEFAULT_MODE default mode
 * \tparam _ALTERNATE_MODE alternate mode
 */
template <class _GPIO, std::size_t _PAD, Pad::Mode _DEFAULT_MODE = Pad::Mode::RESET, Pad::Mode _ALTERNATE_MODE = Pad::Mode::RESET>
struct Pad_:
    public Pad {
    using GPIO = _GPIO;

    Pad_() {
        if(_DEFAULT_MODE != Mode::RESET) {
            setMode(_DEFAULT_MODE);
        }
    }

    Pad_(bool high) {
        write(high);

        if(_DEFAULT_MODE != Mode::RESET) {
            setMode(_DEFAULT_MODE);
        }
    }

    inline void
    set()
    {
        palSetPad(reinterpret_cast<stm32_gpio_t*>(GPIO::driver), _PAD);
    }

    inline void
    clear()
    {
        palClearPad(reinterpret_cast<stm32_gpio_t*>(GPIO::driver), _PAD);
    }

    inline void
    toggle()
    {
        palTogglePad(reinterpret_cast<stm32_gpio_t*>(GPIO::driver), _PAD);
    }

    inline void
    write(
        bool high
    )
    {
        palWritePad(reinterpret_cast<stm32_gpio_t*>(GPIO::driver), _PAD, high ? PAL_HIGH : PAL_LOW);
    }

    inline bool
    read()
    {
        return palReadPad(reinterpret_cast<stm32_gpio_t*>(GPIO::driver), _PAD) == PAL_HIGH;
    }

    inline void
    setMode(
        Mode mode
    )
    {
        palSetPadMode(reinterpret_cast<stm32_gpio_t*>(GPIO::driver), _PAD, static_cast<iomode_t>(mode));
    }

    inline void
    setDefaultMode()
    {
        setMode(_DEFAULT_MODE);
    }

    inline void
    setAlternateMode()
    {
        setMode(_ALTERNATE_MODE);
    }

    inline bool
	isNC()
    {
    	return false;
    }
};

template <bool DEFAULT_VALUE>
struct NCPad:
    public Pad {
    NCPad()
    {
        _value = DEFAULT_VALUE;
    }

    inline void
    set()
    {}

    inline void
    clear()
    {}

    inline void
    toggle()
    {}

    inline void
    write(
        bool high
    )
    {
        _value = high;
    }

    inline bool
    read()
    {
        return _value;
    }

    inline void
    setMode(
        Mode mode
    )
    {}

    inline void
    setDefaultMode()
    {}

    inline void
    setAlternateMode()
    {}

    inline bool
	isNC()
    {
    	return true;
    }

private:
    bool _value;
};

// --- Aliases -----------------------------------------------------------------

using GPIO_A = GPIODriverTraits<0>;
using GPIO_B = GPIODriverTraits<1>;
using GPIO_C = GPIODriverTraits<2>;
using GPIO_D = GPIODriverTraits<3>;
using GPIO_E = GPIODriverTraits<4>;
using GPIO_F = GPIODriverTraits<5>;

NAMESPACE_CORE_HW_END
