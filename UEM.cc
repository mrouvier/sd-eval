// UEM.cc
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


#include "UEM.h"
#include "Util.h"
#include "Logging.h"
#include "Error.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

using namespace std;

UEM::UEM() {
}

UEM::~UEM() {
}

void UEM::add(string file, int chan, int stime, int etime)
{
  rlist &r = ranges[file][chan];
  r.push_back(Range(stime, etime));
  end_times[file][chan][etime] = &r.back();
}

void UEM::find_coverage(string file, int channel, int stime, int etime, std::list<const Range *> &r) const {
  r.clear();
  map<string, map<int, map<int, const Range *> > >::const_iterator i1 = end_times.find(file);
  if(i1 == end_times.end())
    return;
  map<int, map<int, const Range *> >::const_iterator i2 = i1->second.find(channel);
  if(i2 == i1->second.end())
    return;

  const map<int, const Range *> &et = i2->second;
  map<int, const Range *>::const_iterator n = et.upper_bound(stime);
  while(n != et.end() && etime > n->second->stime) {
    r.push_back(n->second);
    n++;
  }
}

bool UEM::is_in(string file, int channel, int time) const {
  map<string, map<int, map<int, const Range *> > >::const_iterator i1 = end_times.find(file);
  if(i1 == end_times.end())
    return false;
  map<int, map<int, const Range *> >::const_iterator i2 = i1->second.find(channel);
  if(i2 == i1->second.end())
    return false;

  const map<int, const Range *> &et = i2->second;
  map<int, const Range *>::const_iterator n = et.upper_bound(time);
  return n != et.end() && n->second->stime <= time;
}


UEM* uem_file_parse(const string& s) {
    UEM* uem = new UEM();

    string line;

    fstream f(s.c_str());
    SDEVAL_ASSERT(f.is_open(), "Error for open : "+s);
    while(getline(f, line)) {
        vector<string> vec = split(line, " ");

        SDEVAL_ASSERT(vec.size() == 4, "Error on UEM format");

        string file = to_utf8(vec[0]);
        int chan = to_int(vec[1]);
        int stime = to_time_ms(vec[2]);
        int etime = to_time_ms(vec[3]);
        uem->add(file, chan, stime, etime);
    }
    f.close();

    return uem;
}

const UEM::rlist *UEM::find_range(string name, int channel) const {
  map<string, map<int, rlist> >::const_iterator i = ranges.find(name);
  if(i == ranges.end()) {
    return 0;
  }
  else {
    map<int, rlist>::const_iterator j = i->second.find(channel);
    if( j == i->second.end()) {
      return 0;
    }
    else {
      return &(j->second);
    }
  }
}
