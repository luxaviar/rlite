#pragma once

#ifdef _WIN32
#include <io.h>

#define access _access
#define ftruncate _chsize

#define F_OK 0
#define W_OK 2
#define R_OK 4

#else

#include <sys/file.h>

#endif