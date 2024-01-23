#include "date.h"
#include <time.h>
#include <stdlib.h>
#include "oom.h"

char *get_date_header_string(size_t *length)
{
	char *date = (char *)malloc(256);
	if (!date) oom();

	time_t m_current_time = time(NULL) - 7200;
	struct tm *m_local_time = localtime(&m_current_time);

	int l = strftime(date, 40, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", m_local_time);
	if (l == 0) return NULL;

	return date;
}
