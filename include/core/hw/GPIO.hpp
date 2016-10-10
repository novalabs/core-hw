/* COPYRIGHT (c) 2016 Nova Labs SRL
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
   static constexpr auto driver = GPIOA;
};

template <>
struct GPIODriverTraits<1> {
   static constexpr auto driver = GPIOB;
};

template <>
struct GPIODriverTraits<2> {
   static constexpr auto driver = GPIOC;
};

template <>
struct GPIODriverTraits<3> {
   static constexpr auto driver = GPIOD;
};

template <>
struct GPIODriverTraits<4> {
   static constexpr auto driver = GPIOE;
};

template <>
struct GPIODriverTraits<5> {
   static constexpr auto driver = GPIOF;
};

/*! \brief Pad interface
 *
 */
struct Pad {
   /*! \brief Pad modes
    *
    */
   enum Mode : iomode_t {
      INPUT            = PAL_MODE_INPUT, //!< Digital floating input
      INPUT_PULLUP     = PAL_MODE_INPUT_PULLUP, //!< Digital input with pull up
      INPUT_PULLDOWN   = PAL_MODE_INPUT_PULLDOWN, //!< Digital input with pull down
      INPUT_ANALOG     = PAL_MODE_INPUT_ANALOG, //!< Analog input
      OUTPUT_PUSHPULL  = PAL_MODE_OUTPUT_PUSHPULL, //!< Digital push-pull output
      OUTPUT_OPENDRAIN = PAL_MODE_OUTPUT_OPENDRAIN //!< Digital open-drain output
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
};

/*! \brief Pad
 *
 * \tparam _GPIO GPIODriverTraits driver
 * \tparam _PAD pad
 */
template <class _GPIO, std::size_t _PAD>
struct Pad_:
   public Pad {
   using GPIO = _GPIO;
   inline void
   set()
   {
      palSetPad(GPIO::driver, _PAD);
   }

   inline void
   clear()
   {
      palClearPad(GPIO::driver, _PAD);
   }

   inline void
   toggle()
   {
      palTogglePad(GPIO::driver, _PAD);
   }

   inline void
   write(
      bool high
   )
   {
      palWritePad(GPIO::driver, _PAD, high ? PAL_HIGH : PAL_LOW);
   }

   inline bool
   read()
   {
      return palReadPad(GPIO::driver, _PAD) == PAL_HIGH;
   }

   inline void
   setMode(
      Mode mode
   )
   {
      palSetPadMode(GPIO::driver, _PAD, mode);
   }
};

// --- Aliases -----------------------------------------------------------------

using GPIO_A = GPIODriverTraits<0>;
using GPIO_B = GPIODriverTraits<1>;
using GPIO_C = GPIODriverTraits<2>;
using GPIO_D = GPIODriverTraits<3>;
using GPIO_E = GPIODriverTraits<4>;
using GPIO_F = GPIODriverTraits<5>;

NAMESPACE_CORE_HW_END
