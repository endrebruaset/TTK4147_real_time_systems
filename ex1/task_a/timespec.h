#ifndef TIME_H
#define TIME_H

#include <time.h>

struct timespec timespec_normalized(time_t sec, long nsec);

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);

struct timespec timespec_add(struct timespec lhs, struct timespec rhs);

int timespec_cmp(struct timespec lhs, struct timespec rhs);

#endif