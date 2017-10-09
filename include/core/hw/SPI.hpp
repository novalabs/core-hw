/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

//TODO: hal

#include <core/hw/namespace.hpp>
#include <core/hw/common.hpp>

#include <core/hw/GPIO.hpp>

NAMESPACE_CORE_HW_BEGIN

template <std::size_t S>
struct SPIDriverTraits {};

#if STM32_SPI_USE_SPI1
template <>
struct SPIDriverTraits<1> {
    static constexpr auto driver = &SPID1;
};
#endif

#if STM32_SPI_USE_SPI2
template <>
struct SPIDriverTraits<2> {
    static constexpr auto driver = &SPID2;
};
#endif

#if STM32_SPI_USE_SPI3
template <>
struct SPIDriverTraits<3> {
    static constexpr auto driver = &SPID3;
};
#endif

class SPIMaster
{
public:
    using Configuration = ::SPIConfig;

public:
    virtual void
    start(
        const Configuration& config
    ) = 0;

    virtual void
    stop() = 0;

    virtual void
    acquireBus() = 0;

    virtual void
    releaseBus() = 0;

    virtual void
    ignore(
        size_t n
    ) = 0;

    virtual void
    exchange(
        size_t      n,
        const void* txbuf,
        void*       rxbuf
    ) = 0;

    virtual void
    send(
        size_t      n,
        const void* txbuf
    ) = 0;

    virtual void
    receive(
        size_t n,
        void*  rxbuf
    ) = 0;
};

template <class _SPI>
class SPIMaster_:
    public SPIMaster
{
public:
    using SPI = _SPI;
    inline void
    start(
        const Configuration& config
    )
    {
        ::spiStart(SPI::driver, &config);
    }

    inline void
    stop()
    {
        ::spiStop(SPI::driver);
    }

    inline void
    acquireBus()
    {
#if SPI_USE_MUTUAL_EXCLUSION
        ::spiAcquireBus(SPI::driver);
#endif
    }

    inline void
    releaseBus()
    {
#if SPI_USE_MUTUAL_EXCLUSION
        ::spiReleaseBus(SPI::driver);
#endif
    }

    inline void
    ignore(
        size_t n
    )
    {
        ::spiIgnore(SPI::driver, n);
    }

    inline void
    exchange(
        size_t      n,
        const void* txbuf,
        void*       rxbuf
    )
    {
        ::spiExchange(SPI::driver, n, txbuf, rxbuf);
    }

    inline void
    send(
        size_t      n,
        const void* txbuf
    )
    {
        ::spiSend(SPI::driver, n, txbuf);
    }

    inline void
    receive(
        size_t n,
        void*  rxbuf
    )
    {
        ::spiReceive(SPI::driver, n, rxbuf);
    }
};

class SPIDevice
{
public:
    using Configuration = ::SPIConfig;

public:
    virtual void
    start(
        const Configuration& config
    ) = 0;

    virtual void
    stop() = 0;

    virtual void
    select() = 0;

    virtual void
    deselect() = 0;

    virtual void
    acquireBus(
        bool start = true
    ) = 0;

    virtual void
    releaseBus(
        bool stop = true
    ) = 0;

    virtual void
    ignore(
        size_t n
    ) = 0;

    virtual void
    exchange(
        size_t      n,
        const void* txbuf,
        void*       rxbuf
    ) = 0;

    virtual void
    send(
        size_t      n,
        const void* txbuf
    ) = 0;

    virtual void
    receive(
        size_t n,
        void*  rxbuf
    ) = 0;
};

template <class _SPI, class _CS>
class SPIDevice_:
    public SPIDevice
{
public:
    using SPI = _SPI;
    using CS  = _CS;

    inline void
    start(
        const Configuration& config
    )
    {
        ::spiStart(SPI::driver, &config);
    }

    inline void
    stop()
    {
        ::spiStop(SPI::driver);
    }

    inline void
    select()
    {
        _cs.clear();
    }

    inline void
    deselect()
    {
        _cs.set();
    }

    inline void
    acquireBus(
        bool start = true
    )
    {
#if SPI_USE_MUTUAL_EXCLUSION
        ::spiAcquireBus(SPI::driver);
#endif

        if (start) {
            //   ::spiStart(SPI::driver, SPI::driver->config);
        }
    }

    inline void
    releaseBus(
        bool stop = true
    )
    {
        if (stop) {
            //    ::spiStop(SPI::driver);
        }

#if SPI_USE_MUTUAL_EXCLUSION
        ::spiReleaseBus(SPI::driver);
#endif
    }

    inline void
    ignore(
        size_t n
    )
    {
        ::spiIgnore(SPI::driver, n);
    }

    inline void
    exchange(
        size_t      n,
        const void* txbuf,
        void*       rxbuf
    )
    {
        ::spiExchange(SPI::driver, n, txbuf, rxbuf);
    }

    inline void
    send(
        size_t      n,
        const void* txbuf
    )
    {
        ::spiSend(SPI::driver, n, txbuf);
    }

    inline void
    receive(
        size_t n,
        void*  rxbuf
    )
    {
        ::spiReceive(SPI::driver, n, rxbuf);
    }

private:
    static CS _cs;
    static SPIMaster_<SPI> _master;
};

// --- Aliases -----------------------------------------------------------------

using SPI_1 = SPIDriverTraits<1>;
using SPI_2 = SPIDriverTraits<2>;
using SPI_3 = SPIDriverTraits<3>;

NAMESPACE_CORE_HW_END
