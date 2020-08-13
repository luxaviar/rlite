#pragma once

#ifdef _WIN32

#define sleep(x) Sleep((x) * 1000);
#define usleep(x) Sleep((x));
#define strerror_r(errno,buf,len) strerror_s(buf,len,errno)
#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#else

#include <unistd.h>

#endif
