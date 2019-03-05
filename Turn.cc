// Turn.cc
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



#include "Turn.h"

Turn_Entry::Turn_Entry(int _type) {
    type = _type;
}

Turn_Entry::~Turn_Entry() {
}


Turn_Event::Turn_Event(int _type, int _extent, string _desc) : Turn_Entry(EVENT) {
    type = _type;
    extent = _extent;
    desc = _desc;
}

Turn_Event::~Turn_Event() {
}

Turn::Turn(Speaker *_spk, Section *_sec, int _stime, int _etime) {
    spk = _spk;
    sec = _sec;
    stime = _stime;
    etime = _etime;
    superposed = false;
    backchannel_type = PRIMARY;
    refcount = 1;
}

Turn::~Turn() {
    for(vector<Turn_Entry *>::const_iterator i = ents.begin(); i != ents.end(); i++)
        delete *i;
}


// ILYA_TDF
Turn& Turn::operator+=(const Turn& turn_to_add) {
    if (turn_to_add.stime < stime)
        stime = turn_to_add.stime;
    if (turn_to_add.etime > etime)
        etime = turn_to_add.etime;

    for(vector<Turn_Entry *>::const_iterator i = turn_to_add.ents.begin(); i != turn_to_add.ents.end(); i++) {
        ents.push_back(*i);
    }

    return *this;
}
