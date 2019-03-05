// Section.h
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



#ifndef SECTION_H
#define SECTION_H

#include "Turn.h"

#include <vector>

using namespace std;

class Section {
public:
  enum { TRANS, NONTRANS, FILLER };
  int mode;
  int stime, etime;

  vector<Turn *> turns;

  Section();
  Section(const Section &s);
  ~Section();
  // ILYA_TDF
  void fix_times(int new_stime, int new_etime);
  void sort_turns();
};

class Sections {
public:
  vector<Section> s;

  Section *add(int mode, int stime, int etime);
  void sort_turns();
};

#endif
