// Section.cc
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



#include "Section.h"
#include <stdio.h>
#include <algorithm>

Section::Section() {
}

Section::Section(const Section &s) {
    mode = s.mode;
    stime = s.stime;
    etime = s.etime;
    turns = s.turns;
    for(vector<Turn *>::const_iterator i = turns.begin(); i != turns.end(); i++)
        (*i)->ref();
}

Section::~Section() {
    for(vector<Turn *>::const_iterator i = turns.begin(); i != turns.end(); i++)
        (*i)->unref();
}

Section *Sections::add(int mode, int stime, int etime) {
    int id = s.size();
    s.resize(id+1);
    Section *sec = &s[id];
    sec->mode = mode;
    sec->stime = stime;
    sec->etime = etime;
    return sec;
}

void Section::fix_times(int new_stime, int new_etime) {
    stime = new_stime;
    etime = new_etime;
}

void Sections::sort_turns() {
    for(unsigned int i=0; i != s.size(); i++)
        s[i].sort_turns();
}

void Section::sort_turns() {
    sort(turns.begin(), turns.end(), Turn::time_sort());
}
