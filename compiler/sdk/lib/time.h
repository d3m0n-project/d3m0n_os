#ifndef TIME_H
#define TIME_H

struct tm {
	int		tm_sec;
	int		tm_min;
	int		tm_hour;
	int		tm_mday;
	int		tm_mon;
	int		tm_year;
	int		tm_wday;
	int		tm_yday;
	int		tm_isdst;
};

static const int days_before_month[] = {
	0, 31, 59, 90, 120, 151,
	181, 212, 243, 273, 304, 334
};

static const char *const weekday_short[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char *const weekday_long[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday", "Saturday"
};

static const char *const month_short[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char *const month_long[] = {
	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December"
};

static int	is_leap_year(int year)
{
	return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}

static inline struct tm	*gmtime(const time_t *timer)
{
	static struct tm tm;
	time_t t = *timer;

	tm.tm_sec = t % 60;
	t /= 60;

	tm.tm_min = t % 60;
	t /= 60;

	tm.tm_hour = t % 24;

	// days since 1970-01-01
	int days = t / 24;

	// 1970-01-01 was Thursday
	tm.tm_wday = (days + 4) % 7;

	int year = 1970;
	while (1)
	{
		int days_in_year = is_leap_year(year) ? 366 : 365;
		if (days < days_in_year)
			break;

		days -= days_in_year;
		year++;
	}

	tm.tm_year = year - 1900;
	tm.tm_yday = days;
	int month = 0;
	while (month < 11)
	{
		int days_in_month;
		if (month == 1)
			days_in_month = is_leap_year(year)?29 : 28;
		else if (month == 3 || month == 5 || month == 8 || month == 10)
			days_in_month = 30;
		else
			days_in_month = 31;

		if (days < days_in_month)
			break;

		days -= days_in_month;
		month++;
	}
	tm.tm_mon = month;
	tm.tm_mday = days + 1;
	tm.tm_isdst = 0;
	return &tm;
}

struct tm	*localtime(const time_t *timer)
{
	// TODO: timezone relative time
	return gmtime(timer);
}

static int	days_in_month(int year, int month)
{
	static const int days[] = {
		31, 28, 31, 30, 31, 30,
		31, 31, 30, 31, 30, 31
	};

	if (month == 1)
		return days[month] + is_leap_year(year);

	return days[month];
}

static inline time_t	mktime(struct tm * tm)
{
	long long days = 0;
	long long seconds;

	int year = tm->tm_year + 1900;
	int month = tm->tm_mon;

	year += month / 12;
	month %= 12;
	if (month < 0)
	{
		month += 12;
		year--;
	}

	tm->tm_year = year - 1900;
	tm->tm_mon = month;

	// days from 1970-01-01 to the beginning of year
	if (year >= 1970)
	{
		for (int y = 1970; y < year; y++)
			days += 365 + is_leap_year(y);
	}
	else
	{
		for (int y = year; y < 1970; y++)
			days -= 365 + is_leap_year(y);
	}

	// days preceding current month
	for (int m = 0; m < month; m++)
		days += days_in_month(year, m);

	days += tm->tm_mday - 1;
	seconds = days * 86400LL;
	seconds += tm->tm_hour * 3600LL;
	seconds += tm->tm_min * 60LL;
	seconds += tm->tm_sec;

	tm->tm_wday = -1;
	tm->tm_yday = -1;
	tm->tm_isdst = 0;

	return (time_t)seconds;
}

static inline double	difftime(time_t end, time_t begin)
{
	return  (end - begin);
}


static int put_char(char **dst, size_t *remaining, char c)
{
	if (*remaining <= 1)
		return 0;

	**dst = c;
	(*dst)++;
	(*remaining)--;

	return 1;
}


static int put_string(char **dst, size_t *remaining, const char *str)
{
	while (*str) {
		if (!put_char(dst, remaining, *str))
			return 0;

		str++;
	}
	return 1;
}



static int	put_number(char **dst, size_t *remaining, unsigned int value, unsigned int width)
{
	char buf[16];
	unsigned int digits = 0;

	do {
		buf[digits++] = '0' + (value % 10);
		value /= 10;
	} while (value != 0);

	while (digits < width)
		buf[digits++] = '0';

	while (digits > 0)
	{
		if (!put_char(dst, remaining, buf[--digits]))
			return 0;
	}

	return 1;
}


static inline size_t	strftime(char *restrict s, size_t max, const char *restrict format, const struct tm *restrict tm)
{
	char *dst = s;
	size_t remaining = max;
	if (max == 0)
		return 0;

	while (*format)
	{
		if (*format != '%')
		{
			if (!put_char(&dst, &remaining, *format++))
				goto fail;
			continue;
		}

		format++;

		switch (*format)
		{
		case '%':
			if (!put_char(&dst, &remaining, '%'))
				goto fail;
			break;

		case 'a':
			if (tm->tm_wday < 0 || tm->tm_wday > 6)
				goto fail;

			if (!put_string(&dst, &remaining, weekday_short[tm->tm_wday]))
				goto fail;
			break;

		case 'A':
			if (tm->tm_wday < 0 || tm->tm_wday > 6)
				goto fail;

			if (!put_string(&dst, &remaining, weekday_long[tm->tm_wday]))
				goto fail;
			break;

		case 'b':
		case 'h':
			if (tm->tm_mon < 0 || tm->tm_mon > 11)
				goto fail;

			if (!put_string(&dst, &remaining, month_short[tm->tm_mon]))
				goto fail;
			break;

		case 'B':
			if (tm->tm_mon < 0 || tm->tm_mon > 11)
				goto fail;

			if (!put_string(&dst, &remaining, month_long[tm->tm_mon]))
				goto fail;
			break;

		case 'c':
			/*
			 * Equivalent to:
			 *
			 * %a %b %d %H:%M:%S %Y
			 */
			if (!put_string(&dst, &remaining, weekday_short[tm->tm_wday]))
				goto fail;

			if (!put_char(&dst, &remaining, ' '))
				goto fail;

			if (!put_string(&dst, &remaining, month_short[tm->tm_mon]))
				goto fail;

			if (!put_char(&dst, &remaining, ' '))
				goto fail;

			if (!put_number(&dst, &remaining, tm->tm_mday, 2))
				goto fail;

			if (!put_char(&dst, &remaining, ' '))
				goto fail;

			if (!put_number(&dst, &remaining, tm->tm_hour, 2))
				goto fail;

			if (!put_char(&dst, &remaining, ':'))
				goto fail;

			if (!put_number(&dst, &remaining, tm->tm_min, 2))
				goto fail;

			if (!put_char(&dst, &remaining, ':'))
				goto fail;

			if (!put_number(&dst, &remaining, tm->tm_sec, 2))
				goto fail;

			if (!put_char(&dst, &remaining, ' '))
				goto fail;

			if (!put_number(&dst, &remaining, tm->tm_year + 1900, 4))
				goto fail;

			break;

		case 'd':
			if (!put_number(&dst, &remaining, tm->tm_mday, 2))
				goto fail;
			break;

		case 'H':
			if (!put_number(&dst, &remaining, tm->tm_hour, 2))
				goto fail;
			break;

		case 'I': {
			int hour = tm->tm_hour % 12;
			if (hour == 0)
				hour = 12;

			if (!put_number(&dst, &remaining, (unsigned int)hour, 2))
				goto fail;

			break;
		}

		case 'j':
			if (!put_number(&dst, &remaining, tm->tm_yday + 1, 3))
				goto fail;
			break;

		case 'm':
			if (!put_number(&dst, &remaining, tm->tm_mon + 1, 2))
				goto fail;
			break;

		case 'M':
			if (!put_number(&dst, &remaining, tm->tm_min, 2))
				goto fail;
			break;

		case 'p':
			if (!put_string(&dst, &remaining, tm->tm_hour < 12 ? "AM" : "PM"))
				goto fail;
			break;

		case 'S':
			if (!put_number(&dst, &remaining, tm->tm_sec, 2))
				goto fail;
			break;

		case 'u': {
			/*
			 * tm_wday:
			 *   Sunday = 0
			 *   Monday = 1
			 *
			 * %u:
			 *   Monday = 1
			 *   Sunday = 7
			 */
			unsigned int day = tm->tm_wday == 0 ? 7 : tm->tm_wday;
			if (!put_number(&dst, &remaining, day, 1))
				goto fail;

			break;
		}

		case 'w':
			if (!put_number(&dst, &remaining, tm->tm_wday, 1))
				goto fail;
			break;

		case 'x':
			/*
			 * %Y-%m-%d
			 */
			if (!put_number(&dst, &remaining, tm->tm_year + 1900, 4))
				goto fail;

			if (!put_char(&dst, &remaining, '-'))
				goto fail;

			if (!put_number(&dst, &remaining, tm->tm_mon + 1, 2))
				goto fail;

			if (!put_char(&dst, &remaining, '-'))
				goto fail;

			if (!put_number(&dst, &remaining, tm->tm_mday, 2))
				goto fail;

			break;

		case 'X':
			/*
			 * %H:%M:%S
			 */
			if (!put_number(&dst, &remaining, tm->tm_hour, 2))
				goto fail;

			if (!put_char(&dst, &remaining, ':'))
				goto fail;

			if (!put_number(&dst, &remaining, tm->tm_min, 2))
				goto fail;

			if (!put_char(&dst, &remaining, ':'))
				goto fail;

			if (!put_number(&dst, &remaining, tm->tm_sec, 2))
				goto fail;

			break;

		case 'y':
			if (!put_number(&dst, &remaining, (tm->tm_year + 1900) % 100, 2))
				goto fail;
			break;

		case 'Y':
			if (!put_number(&dst, &remaining, tm->tm_year + 1900, 4))
				goto fail;
			break;

		default:
			goto fail;
		}

		format++;
	}
	*dst = '\0';
	return (size_t)(dst - s);

fail:
	if (max > 0)
		s[0] = '\0';

	return 0;
}

#endif