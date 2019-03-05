// Turn.h
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



#ifndef TURN_H
#define TURN_H

#include <string>
#include <vector>

using namespace std;

class Turn_Entry {
    public:
        enum { TEXT, SYNC, EVENT, CTM};
        int type;

        Turn_Entry(int type);
        virtual ~Turn_Entry();
};


class Turn_Event : public Turn_Entry {
    public:
        enum { NOISE, LEX, PRON, LANG, ENTITY };
        enum { BEGIN, END, PREVIOUS, NEXT, INSTANT };

        int type, extent;
        string desc;

        Turn_Event(int type, int extent, string desc);
        virtual ~Turn_Event();
};

class Turn {
    public:
        enum { PRIMARY, BACKCHANNEL, OUT_FIELD, COMPLEMENTARY, TURN_REQUEST };

        class Speaker *spk;
        class Section *sec;

        struct time_sort {
            bool operator() (const Turn *t1, const Turn *t2) const {
                return t1->stime == t2->stime ? t1->etime < t2->etime : t1->stime < t2->stime;
            }
        };

        int channel, stime, etime;
        bool superposed;
        int backchannel_type;

        string mode, fidelity;

        vector<Turn_Entry *> ents;

        Turn(Speaker *spk, Section *sec, int stime, int etime);
        ~Turn();

        void add_event(int type, int extent, string desc);

        void ref() { refcount++; }
        void unref() { refcount--; if(!refcount) delete this; }

        // ILYA_TDF
        Turn& operator+=(const Turn& turn_to_add);

    private:
        int refcount;
};
inline Turn operator+(Turn lhs, const Turn& rhs)
{
    lhs += rhs;
    return lhs;
}

#endif
