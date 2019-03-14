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

#ifndef JAY_UTIL_STRERROR_HPP_
#define JAY_UTIL_STRERROR_HPP_

#include <ios>
#include <string>


namespace jay {
namespace util {


// Returns an iostate error message.
std::string ios_strerror( std::ios::iostate state );


} // namespace util
} // namespace jay
#endif // JAY_UTIL_STRERROR_HPP_
