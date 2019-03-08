// MDTM.cc
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

#include <iostream>

#include "MDTM.h"
#include "Speaker.h"
#include "Util.h"
#include "Logging.h"
#include "Error.h"


using namespace std;

MDTM::MDTM() { }

MDTM::~MDTM() { }

DiaFormat* mdtm_file_parse(const string& s) {
    DiaFormat* mdtm = new MDTM();

    string line;

    fstream f(s.c_str());
    SDEVAL_ASSERT(f.is_open(), "Error for open : "+s);
    while(getline(f, line)) {
        vector<string> vec = split(line, " ");        

        SDEVAL_ASSERT(vec.size() == 8, "Error on MDTM format");

        string file = vec[0];
        int chan = to_int(vec[1]);
        int stime = to_time_ms(vec[2]);
        int etime = stime + to_time_ms( vec[3] );
        string type = to_utf8( vec[4] );
        //confidence
        string sub = to_utf8( vec[6] );
        string spkid = to_utf8( vec[7] );

        if(type == "speaker") {
            Speakers *spkf = mdtm->spks[file][chan];
            if(!spkf) {
                spkf = new Speakers;
                mdtm->spks[file][chan] = spkf;
            }
            Speaker *spk = spkf->get(spkid);
            if(sub == "adult_male" || sub == "male")
                spk->set_type("male");
            else if(sub == "adult_female" || sub == "female")
                spk->set_type("female");
            else if(sub == "child")
                spk->set_type("child");
            else if(sub != "unknown" && sub != "NA") {
                SDEVAL_ASSERT(false, "Error on speaker subtype \""+sub+"\" unknown");
            }

            spk->turns.push_back(new Turn(spk, 0, stime, etime));

        }
        else {
            SDEVAL_ASSERT(false, "Error on MDTM line type \""+line+"\" unhandled");
        }

    }
    f.close();

    for(map<string, map<int, Speakers *> >::const_iterator i = mdtm->spks.begin(); i != mdtm->spks.end(); i++)
        for(map<int, Speakers *>::const_iterator j = i->second.begin(); j != i->second.end(); j++)
            j->second->sort_turns();

    return mdtm;
}



