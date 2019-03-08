// UEM.h
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



#ifndef __SDEVAL_UEM_H__
#define __SDEVAL_UEM_H__

#include <string>
#include <map>
#include <list>
#include <fstream>

using namespace std;



class UEM {
    public:
        struct Range {
            int stime, etime;
            Range(int s, int e) {
                stime = s; etime = e;
            }
        };

        typedef std::list<Range> rlist;
        std::map<string, std::map<int, rlist> > ranges;

        UEM();
        ~UEM();

        void find_coverage(string file, int channel, int stime, int etime, std::list<const Range *> &r) const;
        bool is_in(string file, int channel, int time) const;

        void add(string file, int channel, int stime, int etime);

        const rlist *find_range(string name, int channel) const;

    private:
        std::map<string, std::map<int, std::map<int, const Range *> > > end_times;


};

UEM* uem_file_parse(const string&);

#endif
