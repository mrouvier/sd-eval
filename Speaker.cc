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



#include "Speaker.h"
#include "Util.h"

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

Speaker::Speaker(string _name) : name(_name)
{
}

Speaker::~Speaker()
{
  for(vector<Turn *>::const_iterator i = turns.begin(); i != turns.end(); i++)
    (*i)->unref();
}

void Speaker::set_type(string value)
{
  field_set("speaker type", type, value);
}

void Speaker::set_dialect(string value)
{
  field_set("speaker dialect", dialect, value);
}

void Speaker::set_accent(string value)
{
  field_set("speaker accent", accent, value);
}

void Speaker::field_set(const char *name, string &var, string value)
{
  if(true || var.length() == 0) {
    var = value;
    return;
  }

  if(var == value)
    return;

  fprintf(stderr, "Speaker: Error, setting field %s to a different value (%s -> %s)\n",
	  name, to_utf8(var).c_str(), to_utf8(value).c_str());
  exit(1);
}

void Speaker::sort_turns()
{
  sort(turns.begin(), turns.end(), Turn::time_sort());
}

Speakers::Speakers()
{
}

Speakers::~Speakers()
{
  for(unsigned int i=0; i != spks.size(); i++)
    delete spks[i];
}

Speaker *Speakers::find(string name) const
{
  map<string, Speaker *>::const_iterator i = spkm.find(name);
  return i == spkm.end() ? 0 : i->second;
}

Speaker *Speakers::get(string name)
{
  Speaker *s = find(name);
  if(!s) {
    s = new Speaker(name);
    spks.push_back(s);
    spkm[name] = s;
  }
  return s;
}

void Speakers::sort_turns()
{
  for(unsigned int i=0; i != spks.size(); i++)
    spks[i]->sort_turns(); 
}
