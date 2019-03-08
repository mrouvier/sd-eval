// MDTM.h
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



#ifndef __SDEVAL_MTDM_H__
#define __SDEVAL_MTDM_H__

#include <iostream>
#include <fstream>
#include <map>

#include "Speaker.h"
#include "Section.h"
#include "DiaFormat.h"


using namespace std;

class MDTM : public DiaFormat {
    public:
        MDTM();
        ~MDTM();
};

DiaFormat* mdtm_file_parse(const string&);

#endif

