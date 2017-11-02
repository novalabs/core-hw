#include <core/hw/Time.hpp>

NAMESPACE_CORE_HW_BEGIN

#if MAC_USE_PTP

void
PTPTime::saveToRTC()
{
    timespec timestamp;

    get(&timestamp);

    struct tm tmp_time;
    localtime_r(&timestamp.tv_sec, &tmp_time);

    RTCDateTime rtcdt;
    rtcConvertStructTmToDateTime(&tmp_time, (timestamp.tv_nsec / (1000 * 1000)), &rtcdt);

    rtcSetTime(&RTCD1, &rtcdt);
}

void
PTPTime::loadFromRTC()
{
    RTCDateTime rtcdt;

    rtcGetTime(&RTCD1, &rtcdt);

    struct tm tmp_time;
    uint32_t  millis;
    rtcConvertDateTimeToStructTm(&rtcdt, &tmp_time, &millis);

    timespec timestamp;
    timestamp.tv_sec  = std::mktime(&tmp_time);
    timestamp.tv_nsec = millis * 1000 * 1000;

    set(&timestamp);
}

void
PTPTime::get(
    std::timespec* time
)
{
    struct ptptime_t timestamp;

    macPTPGetTime(&timestamp);

    time->tv_sec  = timestamp.tv_sec;
    time->tv_nsec = timestamp.tv_nsec;
}

void
PTPTime::set(
    const std::timespec* time
)
{
    struct ptptime_t ts;

    ts.tv_sec  = time->tv_sec;
    ts.tv_nsec = time->tv_nsec;

    macPTPSetTime(&ts);
}

void
PTPTime::get(
    std::timespec& time
)
{
    struct ptptime_t timestamp;

    macPTPGetTime(&timestamp);

    time.tv_sec  = timestamp.tv_sec;
    time.tv_nsec = timestamp.tv_nsec;
}

void
PTPTime::set(
    const std::timespec& time
)
{
    struct ptptime_t ts;

    ts.tv_sec  = time.tv_sec;
    ts.tv_nsec = time.tv_nsec;

    macPTPSetTime(&ts);
}
#endif // if MAC_USE_PTP

NAMESPACE_CORE_HW_END
