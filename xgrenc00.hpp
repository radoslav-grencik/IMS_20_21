#ifndef XGRENC00_HPP_
#define XGRENC00_HPP_

#include "simlib.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>

#define BAD_OPTIONS 420
#define EXIT_HELP 421

#define DAYS_IN_YEAR 365
#define YEAR_INCREASE_NOT_SET -4000
#define NOT_SET -420

using namespace std;

int Error(int errnum, string err);

class ArgumentParser {
public:
    long int year = NOT_SET;
    long int recycleRate = NOT_SET;
    long int recycleSuccess = NOT_SET;
    double yearIncrease = YEAR_INCREASE_NOT_SET;

    int PrintHelp();
    int ParseOpt(int argc, char** argv);
};

class Plastic : public Process {
public:
    void Behavior();
};

class Production : public Event {
public:
    void Behavior();
};

void Experiment();

#endif
