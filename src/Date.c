#include "XenoWS/Date.h"
#include <time.h>
#include <stdlib.h>

char *GetDateHeaderString(size_t *length)
{
	char *date = (char *)malloc(40);

	time_t m_current_time = time(NULL) - 7200;
	struct tm *m_local_time = localtime(&m_current_time);

	*length = strftime(date, 40, "Date: %a, %d %b %Y %H:%M:%S GMT\n", m_local_time);

	return date;
}
