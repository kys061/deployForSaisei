#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#ifndef VSWTC		/* FreeBSD platform header file is not defined VSWTC variables */
#define VSWTC 7
#endif

#ifdef EZIO_G500_CONFIG
#include "include/ezio_g500.h"
#elif defined(EZIO_390_CONFIG)
#include "include/ezio_390.h"
#elif defined(EZIO_340_CONFIG)
#include "include/ezio_340.h"
#elif defined(EZIO_380_CONFIG)
#include "include/ezio_380.h"
#else
#include "include/ezio_300.h"
#endif


