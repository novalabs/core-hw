/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
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

#ifndef POLLING_INTERVAL
#define POLLING_INTERVAL 10
#endif

#ifndef POLLING_DELAY
#define POLLING_DELAY    10
#endif

template <std::size_t S>
struct SDCDriverTraits {};

#if HAL_USE_SDC
template <>
struct SDCDriverTraits<1> {
    static constexpr auto driver = &SDCD1;
};
#endif

class SDC
{
public:
    using Callback = std::function<bool(void)>;
};

template <class _SDC>
class SDC_:
    public SDC
{
public:
    using SDC = _SDC;

    static Callback _insert_callback_impl;
    static Callback _remove_callback_impl;

    void
    start()
    {
        sdcStart(SDC::driver, nullptr);
    }

    void
    stop()
    {
        sdcStop(SDC::driver);
    }

    void
    connect()
    {
        sdcConnect(SDC::driver);
    }

    void
    disconnect()
    {
        sdcDisconnect(SDC::driver);
    }

    void
    setInsertCallback(
        Callback callback
    )
    {
        _insert_callback_impl = callback;
    }

    void
    setRemoveCallback(
        Callback callback
    )
    {
        _remove_callback_impl = callback;
    }

    bool
    isReady()
    {
        return _ready;
    }

    static void
    polling(
        void* args
    )
    {
        event_listener_t el0, el1;

        static const evhandler_t evhndl[] = {
            _insertHandler,
            _removeHandler
        };

        tmr_init();

        chEvtRegister(&_inserted_event, &el0, 0);
        chEvtRegister(&_removed_event, &el1, 1);

        while (!chThdShouldTerminateX()) {
            chEvtDispatch(evhndl, chEvtWaitOneTimeout(ALL_EVENTS, TIME_INFINITE));
        }
    } // polling

private:
    static virtual_timer_t tmr;
    static unsigned        _cnt;
    static bool _ready;
    static event_source_t _inserted_event, _removed_event;

    static void
    tmrfunc(
        void* p
    )
    {
        chSysLockFromISR();

        if (blkIsInserted(SDC::driver)) {
            if (_cnt == 0) {
                chEvtBroadcastI(&_inserted_event);
            }

            _cnt = 1;
            //module.sd_led.write(fs_ready); //LED
        } else {
            if (_cnt == 1) {
                chEvtBroadcastI(&_removed_event);
            }

            _cnt = 0;
        }

#if 0
        if (_cnt > 0) {
            if (blkIsInserted(SDC::driver)) {
                if (--_cnt == 0) {
                    chEvtBroadcastI(&_inserted_event);
                }
            } else {
                _cnt = POLLING_INTERVAL;
            }
        } else {
            if (!blkIsInserted(SDC::driver)) {
                _cnt = POLLING_INTERVAL;
                chEvtBroadcastI(&_removed_event);
            }
        }
#endif
        chVTSetI(&tmr, MS2ST(POLLING_DELAY), tmrfunc, nullptr);
        chSysUnlockFromISR();
    } // tmrfunc

    static void
    tmr_init()
    {
        chEvtObjectInit(&_inserted_event);
        chEvtObjectInit(&_removed_event);
        chSysLock();
        _cnt = 0;
        chVTSetI(&tmr, MS2ST(POLLING_DELAY), tmrfunc, nullptr);
        chSysUnlock();
    }

    static void
    _insertHandler(
        eventid_t id
    )
    {
        (void)id;

        if (sdcConnect(SDC::driver) != HAL_SUCCESS) {
            return;
        }

        if (_insert_callback_impl) {
            if (_insert_callback_impl()) {
                _ready = true;
            } else {
                sdcDisconnect(SDC::driver);
            }
        }
    }      // InsertHandler

    /*
     * Card removal event.
     */
    static void
    _removeHandler(
        eventid_t id
    )
    {
        (void)id;

        if (_remove_callback_impl) {
            _remove_callback_impl();
        }

        sdcDisconnect(SDC::driver);

        _ready = false;
    }
};

template <class _SDC>
bool SDC_<_SDC>::_ready = false;

template <class _SDC>
virtual_timer_t SDC_<_SDC>::tmr;

template <class _SDC>
unsigned SDC_<_SDC>::_cnt = 0;

template <class _SDC>
event_source_t SDC_<_SDC>::_inserted_event;

template <class _SDC>
event_source_t SDC_<_SDC>::_removed_event;

template <class _SDC>
SDC::Callback SDC_<_SDC>::_insert_callback_impl;

template <class _SDC>
SDC::Callback SDC_<_SDC>::_remove_callback_impl;

// --- Aliases -----------------------------------------------------------------

using SDC_1 = SDCDriverTraits<1>;

NAMESPACE_CORE_HW_END
