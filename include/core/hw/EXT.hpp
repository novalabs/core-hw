/* COPYRIGHT (c) 2016 Nova Labs SRL
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
struct EXTDriverTraits {};

template <>
struct EXTDriverTraits<1> {
   static constexpr auto driver = &EXTD1;
};

template <class _EXT>
class EXT_
{
public:
   using EXT = _EXT;

public:
   static void
   start(
      const EXTConfig& config
   )
   {
      ::extStart(EXT::driver, &config);
   }

   static void
   stop()
   {
      ::extStop(EXT::driver);
   }
};

/*! \brief EXT
 *
 */
class EXTChannel
{
public:
   /*! \brief Callback
    *
    */
   using Callback = std::function<void(uint32_t)>;

   /*! \brief Enable the channel
    *
    */
   virtual void
   enable() = 0;


   /*! \brief Disable the channel
    *
    */
   virtual void
   disable() = 0;


   /*! \brief Set the channel callback
    *
    */
   virtual void
   setCallback(
      Callback callback //!< callback function
   ) = 0;


   /*! \brief Reset the callback for the channel
    *
    */
   virtual void
   resetCallback() = 0;
};

template <class _EXT, std::size_t _CH, uint32_t _MODE>
class EXTChannel_:
   public EXTChannel
{
public:
   using EXT = _EXT;

public:
   static Callback callback_impl;

public:
   inline void
   enable()
   {
      ::extChannelEnable(EXT::driver, _CH);
   }

   inline void
   disable()
   {
      ::extChannelDisable(EXT::driver, _CH);
   }

   inline void
   setCallback(
      Callback callback
   )
   {
      callback_impl = callback;

      EXTChannelConfig tmp;

      tmp.mode = _MODE;
      tmp.cb   = _callback;

      extSetChannelMode(EXT::driver, _CH, &tmp);
   }

   inline void
   resetCallback()
   {
      EXTChannelConfig tmp;

      tmp.mode = _MODE;
      tmp.cb   = nullptr;

      extSetChannelMode(EXT::driver, _CH, &tmp);
   }

private:
   static void
   _callback(
      EXTDriver*   extp,
      expchannel_t channel
   )
   {
      callback_impl(channel);
   }
};

template <class _EXT, std::size_t _CH, uint32_t _MODE>
EXTChannel::Callback EXTChannel_<_EXT, _CH, _MODE>::callback_impl;

// --- Aliases -----------------------------------------------------------------

using EXT_1 = EXTDriverTraits<1>;

NAMESPACE_CORE_HW_END
