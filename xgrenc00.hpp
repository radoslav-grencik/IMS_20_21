#ifndef XGRENC00_HPP_
#define XGRENC00_HPP_

#include <iostream>
#include <string>
#include <unistd.h>

#define BAD_OPTIONS 420
#define EXIT_HELP 421

using namespace std;

int Error(int errnum, string err);

int ParseOpt(int argc, char** argv);

int PrintHelp();

#endif
