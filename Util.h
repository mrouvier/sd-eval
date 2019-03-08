// Util.h
//
// Copyright (C) 2013  LNE (author: Olivier Galibert)
// Copyright (C) 2019  LIA - Avignon University (author: Mickael Rouvier)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.



#ifndef __SDEVAL_UTIL_H__
#define __SDEVAL_UTIL_H__

#include <string>
#include <vector>
#include <iostream>

using namespace std;

vector<string> split(const string& str, const string& delim);

string filename_to_basename(string name);

int to_int(string s);
double to_double(string s);
string to_utf8(string s);
string from_utf8(string s);

int to_time_ms(string s);
string time_ms_to_string(int t);



#endif
