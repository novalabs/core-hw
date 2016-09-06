/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/hw/namespace.hpp>
#include <core/hw/common.hpp>

#include <core/hw/GPIO.hpp>

#include "hal.h"
#include "qei.h"

NAMESPACE_CORE_HW_BEGIN

template <std::size_t E>
struct QEIDriverTraits {};

#if STM32_QEI_USE_TIM2
template <>
struct QEIDriverTraits<2> {
   static constexpr auto driver = &QEID2;
};
#endif

#if STM32_QEI_USE_TIM4
template <>
struct QEIDriverTraits<4> {
   static constexpr auto driver = &QEID4;
};
#endif

class QEI
{
public:
   using CountDataType = qeicnt_t;
   using DeltaDataType = qeidelta_t;

   virtual void
   start(
      const QEIConfig* config
   ) = 0;

   virtual void
   stop() = 0;

   virtual void
   enable() = 0;

   virtual void
   disable() = 0;

   virtual CountDataType
   getCount() = 0;

   virtual DeltaDataType
   getDelta() = 0;
};

template <class _QEI>
class QEI_:
   public QEI
{
public:
   using QEI = _QEI;

public:
   inline void
   enable()
   {
      ::qeiEnable(QEI::driver);
   }

   inline void
   disable()
   {
      ::qeiDisable(QEI::driver);
   }

   inline void
   start(
      const QEIConfig* config
   )
   {
      ::qeiStart(QEI::driver, config);
   }

   inline void
   stop()
   {
      ::qeiStop(QEI::driver);
   }

   CountDataType
   getCount()
   {
      return ::qeiGetCount(QEI::driver);
   }

   DeltaDataType
   getDelta()
   {
      return ::qeiUpdate(QEI::driver);
   }
};

// --- Aliases -----------------------------------------------------------------

using QEI_2 = QEIDriverTraits<2>;
using QEI_4 = QEIDriverTraits<4>;

NAMESPACE_CORE_HW_END
