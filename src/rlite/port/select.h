#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#else

#include <sys/select.h>

#endif