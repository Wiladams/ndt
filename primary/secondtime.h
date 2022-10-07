#pragma once

// Take a number of seconds (represented as a double, including fractional part) and
// split it out into hours,minutes, seconds, tenths
//
struct SecondTime {
	static constexpr double gSecondsPerHour = 60 * 60;

	double fBaseTime=0;
	size_t fHours = 0;
	size_t fMinutes = 0;
	size_t fSeconds = 0;
	size_t fTenths = 0;

	SecondTime() = default;

	SecondTime(double s)
	{
		setSeconds(s);
	}

	void setSeconds(double d)
	{
		fBaseTime = d;

		double hours = d / (gSecondsPerHour);
		fHours = (size_t)(hours);

		double minutes = (hours - fHours) * 60;
		fMinutes = (size_t)minutes;

		double seconds = (minutes - fMinutes) * 60;
		fSeconds = (size_t)seconds;

		double tenths = (seconds - fSeconds) * 10;
		fTenths = (size_t)tenths;
	}
};
