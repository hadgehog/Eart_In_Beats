#pragma once
#include "..\config.h"

#include <cstdint>

class HTime{
public:
	// hundred nano-seconds resolution. HNS in 1 second
	static const uint64_t HNSResolution = 10000000;
private:
	// HNS in 1 minute
	static const uint64_t MinuteTicks = HNSResolution * 60;
	// HNS in 1 hour
	static const uint64_t HourTicks = MinuteTicks * 60;
	// HNS in 1 day
	static const uint64_t DayTicks = HourTicks * 24;
	// HNS in 1 year
	// 365.2425 - http://en.wikipedia.org/wiki/Gregorian_calendar
	static const uint64_t YearTicks = HNSResolution * static_cast<uint64_t>(60.0 * 60.0 * 24.0 * 365.2425);
	static const uint64_t Y1970FullTicks = YearTicks * 1970;
	static const uint64_t Y1601FullTicks = YearTicks * 1601;
	// how much time taken by leap days
	static const uint64_t Leap1970Ticks = Y1970FullTicks % DayTicks;
	static const uint64_t Leap1601Ticks = Y1601FullTicks % DayTicks;
	static const uint64_t Y1970Ticks = Y1970FullTicks - Leap1970Ticks;
	static const uint64_t Y1601Ticks = Y1601FullTicks - Leap1601Ticks;
public:
	// Returns number of milliseconds since some event(for example since the system was started).
	static uint64_t GetTimeMsRelative();
	// Converts Unix time(seconds since 1970 year) to Library time(hundred nano-seconds(i.e. 1/10,000,000 of second) since 0 year).
	static uint64_t UnixTimeToLib(uint64_t unixTime);
	// Converts Library time(hundred nano-seconds(i.e. 1/10,000,000 of second) since 0 year) to Unix time(seconds since 1970 year).
	static uint64_t LibTimeToUnix(uint64_t libTime);
	static uint64_t GetCurrentLibTime();
//#ifdef HAVE_VISUAL_STUDIO
//	static uint64_t FileTimeToLib(FILETIME fileTime);
//	static FILETIME LibTimeToFile(uint64_t libTime);
//
//	// based on http://stackoverflow.com/questions/11615998/how-do-i-parse-a-date-in-a-metro-c-cx-app
//	static uint64_t UniversalTimeToLib(int64_t universalTime);
//	static int64_t LibTimeToUniversal(uint64_t libTime);
//#endif
private:
	static uint64_t GetSysClockEpochTicks();
};