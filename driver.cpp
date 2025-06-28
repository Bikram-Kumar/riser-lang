#pragma once

#include "parser.hh"
#include "scanner.h"

#include <fstream>
#include <string>


namespace Riser
{
    class Driver
    {
    public:
        Driver() = default;
        ~Driver() = default;
    
        int parse(const char* filename);
    };


}
