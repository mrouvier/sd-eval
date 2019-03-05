// Util.cc
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



#include "Util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>


vector<string> split(const string& str, const string& delim) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

string filename_to_basename(string name)
{
  int pstart = name.find_last_of('/');
  if(pstart != -1)
    pstart++;
  else
    pstart = 0;

  int pend = name.find_last_of('.');
  if(pend == -1 || pend <= pstart)
    pend = name.size();
  return name.substr(pstart, pend-pstart);
}

int to_int(string s)
{
  return strtol(to_utf8(s).c_str(), 0, 10);
}

double to_double(string s)
{
  return strtod(to_utf8(s).c_str(), 0);
}

string to_utf8(string s)
{
	return s;
}

string from_utf8(string s)
{
	return s;
}

int to_time_ms(string s)
{
  return static_cast<int>(1000*strtod(s.c_str(), 0)+0.5);
}

string time_ms_to_string(int t)
{
  char buf[64];
  if(t<0)
    sprintf(buf, "-%d.%03d", (-t)/1000, (-t) % 1000);
  else
    sprintf(buf, "%d.%03d", t/1000, t % 1000);
  return buf;
}

static inline bool issp(char c)
{
  return c == ' ' || c == '\t' || c == '\r';
}

// ILYA_TDF
static inline bool istb(char c)
{
  return c == '\t' || c == '\r';
}

  

