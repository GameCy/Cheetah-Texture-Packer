#ifndef COMMANDLINE_H
#define COMMANDLINE_H
#include "atlasbuilder.h"


class CommandLine
{
public:
    static void PrintHelp(const char *error=0);
    static int Run(int argc, char *argv[]);
};

#endif // COMMANDLINE_H
