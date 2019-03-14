/*
Copyright (C) 2014 Jay Satiro <raysatiro@yahoo.com>
All rights reserved.

This file is part of CSV/jay::util.

https://github.com/jay/CSV

jay::util is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

jay::util is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with jay::util. If not, see <http://www.gnu.org/licenses/>.
*/

/** Error to string functions
*/

#include "strerror.hpp"

#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include <vector>


using namespace std;


namespace jay {
namespace util {


string ios_strerror( ios::iostate state )
{
    if( !state )
        return "No errors";

    string s;

    if( ( state & ios::eofbit ) )
    {
        if( s.length() )
            s += ", ";

        s += "EOF on input operation";
    }

    if( ( state & ios::failbit ) )
    {
        if( s.length() )
            s += ", ";

        s += "Logical error on i/o operation";
    }

    if( ( state & ios::badbit ) )
    {
        if( s.length() )
            s += ", ";

        s += "Read/write error on i/o operation";
    }

    if( !s.length() )
        s = "Unknown error";

    return s;
}


} // namespace util
} // namespace jay
