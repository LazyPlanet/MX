#pragma once

#include <ctime> 
#include <chrono>
#include <iostream>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/conversion.hpp"

namespace Adoter
{

class CommonTimer {

public:

	static CommonTimer& Instance()
	{
		static CommonTimer _instance;
		return _instance;
	}

	//获取前服务器启动时间(单位：毫秒)
	inline int32_t GetStartTime()
	{
		using namespace std::chrono;
		static const system_clock::time_point start_time = system_clock::now();
		return int32_t(duration_cast<milliseconds>(system_clock::now() - start_time).count());
	}

	//获取当前系统时间(单位：秒)
	inline std::time_t GetTime()
	{
		boost::posix_time::ptime ptime(boost::posix_time::second_clock::local_time()); //universal_time: Get the UTC time.
		std::time_t time = boost::posix_time::to_time_t(ptime);
		return time;
	}

	inline int32_t GetTimeDiff(int32_t old_time, int32_t new_time)
	{
		if (old_time > new_time)
		{
			return (0xFFFFFFFF - old_time) + new_time;
		}
		else
		{
			return new_time - old_time;
		}
	}

	inline int32_t GetTimeDiffToNow(int32_t old_time)
	{
		return GetTimeDiff(old_time, GetStartTime());
	}

	inline std::time_t GetDayBegin(std::time_t cur_t)
	{
		boost::posix_time::ptime ptime = boost::posix_time::from_time_t(cur_t);

		boost::posix_time::ptime day_begin(ptime.date(), boost::posix_time::time_duration(-8, 0, 0));

		return boost::posix_time::to_time_t(day_begin);
	}
	
	inline bool IsSameDay(std::time_t rhs, std::time_t lhs)
	{
		/*
		boost::gregorian::date rhs_date(rhs);
		boost::gregorian::date lhs_date(lhs);

		std::cout << rhs_date.year() << std::endl;
		std::cout << rhs_date.month() << std::endl;
		std::cout << rhs_date.day() << std::endl;
		std::cout << rhs_date.day_of_week() << std::endl;
		return true;
		*/
		return GetDayBegin(lhs) == GetDayBegin(rhs);
	}
	
	inline std::time_t GetWeakBegin(std::time_t cur_t)
	{
		boost::posix_time::ptime ptime = boost::posix_time::from_time_t(cur_t);
		
		auto day_of_week = ptime.date().day_of_week();

		ptime += boost::posix_time::hours(-24 * day_of_week);

		boost::posix_time::ptime week_begin(ptime.date(), boost::posix_time::time_duration(-8, 0, 0));

		auto time = boost::posix_time::to_time_t(week_begin);
		return time;
	}
	
	inline bool IsSameWeek(std::time_t rhs, std::time_t lhs)
	{
		return GetWeakBegin(lhs) == GetWeakBegin(lhs);
	}
	
	//
	//获取当月的第一天, 即: X月1号
	//
	inline std::time_t GetMonthBegin(std::time_t cur_t)
	{
		boost::posix_time::ptime ptime = boost::posix_time::from_time_t(cur_t);
		
		boost::posix_time::ptime month_begin(boost::gregorian::date(ptime.date().year(), ptime.date().month(), 1), boost::posix_time::time_duration(-8, 0, 0));

		auto time = boost::posix_time::to_time_t(month_begin);
		return time;
	}
	
	//
	//获取下个月的该天
	//
	//比如: 当前为1月10号,该函数返回2月10号
	//
	//特别的是: (1) 如果当前为1月31号,该函数则返回2月28号或29号;
	//
	//			(2) 当前时间2005年2月28号,该函数返回的是3月31号;
	//
	inline std::time_t GetNextMonth(std::time_t cur_t)
	{
		boost::posix_time::ptime ptime = boost::posix_time::from_time_t(cur_t);
		
		boost::gregorian::months single(1);
		
		boost::posix_time::ptime next_month(ptime.date() + single, ptime.time_of_day());

		auto time = boost::posix_time::to_time_t(next_month);
		return time;
	}

};

#define CommonTimerInstance CommonTimer::Instance()

/*
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
*/

}
