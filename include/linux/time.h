#ifndef _LINUX_TIME_H
#define _LINUX_TIME_H

struct timespec {
	time_t	tv_sec;		/* seconds */
	long	tv_nsec;	/* nanoseconds */
};

/* Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines were long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
static inline unsigned long
mktime (unsigned int year, unsigned int mon,
	unsigned int day, unsigned int hour,
	unsigned int min, unsigned int sec)
{
	if (0 >= (int) (mon -= 2)) {	/* 1..12 -> 11,12,1..10 */
		mon += 12;		/* Puts Feb last since it has leap day */
		year -= 1;
	}

	return (((
		(unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
			year*365 - 719499
	    )*24 + hour /* now have hours */
	  )*60 + min /* now have minutes */
	)*60 + sec; /* finally seconds */
}



struct timeval {
	time_t		tv_sec;		/* seconds */
	suseconds_t	tv_usec;	/* microseconds */
};

struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};

#define SECSPERMIN	60L
#define MINSPERHOUR	60L
#define HOURSPERDAY	24L
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	(SECSPERHOUR * HOURSPERDAY)
#define DAYSPERWEEK	7
#define MONSPERYEAR	12

#define YEAR_BASE	1900
#define EPOCH_YEAR      1970
#define EPOCH_WDAY      4

#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)


/* Used by other time functions.  */
struct tm {
    int tm_sec;                   /* Seconds.     [0-60] (1 leap second) */
    int tm_min;                   /* Minutes.     [0-59] */
    int tm_hour;                  /* Hours.       [0-23] */
    int tm_mday;                  /* Day.         [1-31] */
    int tm_mon;                   /* Month.       [0-11] */
    int tm_year;                  /* Year - 1900.  */
    int tm_wday;                  /* Day of week. [0-6] */
    int tm_yday;                  /* Days in year.[0-365] */
    int tm_isdst;                 /* DST.         [-1/0/1]*/

# ifdef __USE_BSD
    long int tm_gmtoff;           /* Seconds east of UTC.  */
    const char *tm_zone;        /* Timezone abbreviation.  */
# else
    long int __tm_gmtoff;         /* Seconds east of UTC.  */
    const char *__tm_zone;      /* Timezone abbreviation.  */
# endif
};

static inline char *asctime_r(const struct tm *tim_p, char *result)
{
    static const char day_name[7][3] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
    static const char mon_name[12][3] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    sprintf (result, "%.3s %.3s %.2d %.2d:%.2d:%.2d %d\n",
	    day_name[tim_p->tm_wday],
	    mon_name[tim_p->tm_mon],
	    tim_p->tm_mday, tim_p->tm_hour, tim_p->tm_min,
	    tim_p->tm_sec, 1900 + tim_p->tm_year);
    return result;
}

static inline struct tm *localtime_r(const time_t * tim_p, struct tm *res)
{
    static const int mon_lengths[2][MONSPERYEAR] = {
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
      {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
    } ;

    static const int year_lengths[2] = {
      365,
      366
    } ;

    long days, rem;
    int y;
    int yleap;
    const int *ip;

    days = ((long) *tim_p) / SECSPERDAY;
    rem = ((long) *tim_p) % SECSPERDAY;
    while (rem < 0)
    {
	rem += SECSPERDAY;
	--days;
    }
    while (rem >= SECSPERDAY)
    {
	rem -= SECSPERDAY;
	++days;
    }

    /* compute hour, min, and sec */
    res->tm_hour = (int) (rem / SECSPERHOUR);
    rem %= SECSPERHOUR;
    res->tm_min = (int) (rem / SECSPERMIN);
    res->tm_sec = (int) (rem % SECSPERMIN);

    /* compute day of week */
    if ((res->tm_wday = ((EPOCH_WDAY + days) % DAYSPERWEEK)) < 0)
	res->tm_wday += DAYSPERWEEK;

    /* compute year & day of year */
    y = EPOCH_YEAR;
    if (days >= 0)
    {
	for (;;)
	{
	    yleap = isleap(y);
	    if (days < year_lengths[yleap])
		break;
	    y++;
	    days -= year_lengths[yleap];
	}
    }
    else
    {
	do
	{
	    --y;
	    yleap = isleap(y);
	    days += year_lengths[yleap];
	} while (days < 0);
    }

    res->tm_year = y - YEAR_BASE;
    res->tm_yday = days;
    ip = mon_lengths[yleap];
    for (res->tm_mon = 0; days >= ip[res->tm_mon]; ++res->tm_mon)
	days -= ip[res->tm_mon];
    res->tm_mday = days + 1;

    /* set daylight saving time flag */
    res->tm_isdst = -1;

    return (res);
}

static inline char *ctime_r(const time_t * tim_p, char * result)
{
    struct tm tm;
    return asctime_r (localtime_r (tim_p, &tm), result);
}

#endif
