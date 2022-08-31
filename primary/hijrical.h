#pragma once

#include <string>

namespace hijri
{
	static const std::string MonthsLong[] = {
		"",
		"Muharram",
		"Safar",
		"Rabi' al-awwal",
		"Rabi' al-thani",
		"Jumada al-awwal",
		"Jumada al-thani",
		"Rajab",
		"Sha'aban",
		"Ramadan",
		"Shawwal",
		"Dhu al-Qi'dah",
		"Dhu al-Hijjah"
	};

	static const std::string MonthsShort[] = {
		"",
		"Muh",
		"Saf",
		"Rab1",
		"Rab2",
		"Jum1",
		"Jum2",
		"Raj",
		"Sha'",
		"Ram",
		"Shaw",
		"DhuQ",
		"DhuH"
	};

	static bool validateHijri(int year, int month, int day)
	{
		return ((month >= 1 && month <= 12) && ((day>=1) && (day <= 30)));
	}
}