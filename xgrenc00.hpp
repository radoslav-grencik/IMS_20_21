#ifndef XGRENC00_HPP_
#define XGRENC00_HPP_

#include "simlib.h"
#include <iostream>
#include <string>
#include <unistd.h>

#define BAD_OPTIONS 420
#define EXIT_HELP 421

#define DAYSINYEAR 365
#define YEARINCREASENOTSET -4000

using namespace std;

int Error(int errnum, string err);

void Experiment();

#endif
