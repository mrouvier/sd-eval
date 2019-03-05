// Speaker.h
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



#ifndef SPEAKER_H
#define SPEAKER_H

#include <map>
#include <vector>

#include "Turn.h"

using namespace std;

class Speaker {
public:
  string name;

  string type, dialect, accent;

  bool global;

  vector<Turn *> turns;

  Speaker(string name);
  ~Speaker();

  void set_type(string type);
  void set_dialect(string dialect);
  void set_accent(string accent);

  void sort_turns();

private:
  static void field_set(const char *name, string &var, string value);
};

class Speakers {
public:
  vector<Speaker *> spks;
  map<string, Speaker *> spkm;

  Speakers();
  ~Speakers();

  Speaker *find(string name) const;
  Speaker *get(string name);

  void sort_turns();
};

#endif
