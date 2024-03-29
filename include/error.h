/*  This file is part of vtrace.
    vtrace is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    vtrace is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with vtrace. If not, see <https://www.gnu.org/licenses/>.  */

#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>

#define SUCCESS 0
#define ERROR 1

#define PROPAGATE(call, error, log)			\
    {							\
	if (!(call)) {					\
	    fprintf(stderr, "ERROR: %s\n", (log));	\
	    return (error);				\
	}						\
    }

#define PROPAGATE_CLEANUP_BEGIN(call, log)		\
    if (!(call)) {					\
    fprintf(stderr, "ERROR: %s\n", (log));		\
    
#define PROPAGATE_CLEANUP_END(error)			\
    return (error);					\
    }

#define WARNING(call, log)			\
    {							\
	if (!(call)) {					\
	    fprintf(stderr, "WARNING: %s\n", (log));	\
	}						\
    }

#endif // ERROR_H_
