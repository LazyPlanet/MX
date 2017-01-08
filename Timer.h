#pragma once

#include <chrono>

//获取当前时间(单位：ms)
inline int32_t GetTime()
{
    using namespace std::chrono;
    static const system_clock::time_point start_time = system_clock::now();
    return int32_t(duration_cast<milliseconds>(system_clock::now() - start_time).count());
}

inline int32_t GetTimeDiff(int32_t old_time, int32_t new_time)
{
    if (old_time > new_time)
        return (0xFFFFFFFF - old_time) + new_time;
    else
        return new_time - old_time;
}

inline int32_t GetTimeDiffToNow(int32_t old_time)
{
    return GetTimeDiff(old_time, GetTime());
}

struct IntervalTimer
{
public:

    IntervalTimer() : _interval(0), _current(0)
    {
    }

    void Update(time_t diff)
    {
        _current += diff;
        if (_current < 0)
            _current = 0;
    }

    bool Passed()
    {
        return _current >= _interval;
    }

    void Reset()
    {
        if (_current >= _interval)
            _current %= _interval;
    }

    void SetCurrent(time_t current)
    {
        _current = current;
    }

    void SetInterval(time_t interval)
    {
        _interval = interval;
    }

    time_t GetInterval() const
    {
        return _interval;
    }

    time_t GetCurrent() const
    {
        return _current;
    }

private:

    time_t _interval;
    time_t _current;
};

struct TimeTracker
{
public:

    TimeTracker(time_t expiry) : i_expiryTime(expiry)
    {
    }

    void Update(time_t diff)
    {
        i_expiryTime -= diff;
    }

    bool Passed() const
    {
        return i_expiryTime <= 0;
    }

    void Reset(time_t interval)
    {
        i_expiryTime = interval;
    }

    time_t GetExpiry() const
    {
        return i_expiryTime;
    }

private:

    time_t i_expiryTime;
};

struct TimeTrackerSmall
{
public:

    TimeTrackerSmall(int32_t expiry = 0)
        : i_expiryTime(expiry)
    {
    }

    void Update(int32_t diff)
    {
        i_expiryTime -= diff;
    }

    bool Passed() const
    {
        return i_expiryTime <= 0;
    }

    void Reset(int32_t interval)
    {
        i_expiryTime = interval;
    }

    int32_t GetExpiry() const
    {
        return i_expiryTime;
    }

private:

    int32_t i_expiryTime;
};

struct PeriodicTimer
{
public:

    PeriodicTimer(int32_t period, int32_t start_time)
        : i_period(period), i_expireTime(start_time)
    {
    }

    bool Update(const int32_t diff)
    {
        if ((i_expireTime -= diff) > 0)
            return false;

        i_expireTime += i_period > int32_t(diff) ? i_period : diff;
        return true;
    }

    void SetPeriodic(int32_t period, int32_t start_time)
    {
        i_expireTime = start_time;
        i_period = period;
    }

    // Tracker interface
    void TUpdate(int32_t diff) { i_expireTime -= diff; }
    bool TPassed() const { return i_expireTime <= 0; }
    void TReset(int32_t diff, int32_t period)  { i_expireTime += period > diff ? period : diff; }

private:

    int32_t i_period;
    int32_t i_expireTime;
};
