// sd-eval.cc
//
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
#include <iomanip>
#include "MDTM.h"
#include "RTTM.h"
#include "DiaFormat.h"
#include "UEM.h"
#include "SdResultat.h"
#include "Logging.h"
#include "Error.h"
#include "clipp.h"

using namespace std;
using namespace clipp;


int main(int argc, char** argv) {

    string uem_file = "";
    string hyp_file;
    string ref_file;
    string hyp_format = "mdtm";
    string ref_format = "mdtm";
    int collar = 250;
    bool cross_show=false;

    auto cli = (
            with_suffix("=", (option("--hyp-format").doc("Hypothesis format") & value("hyp-format", hyp_format))),
            with_suffix("=", (option("--ref-format").doc("Reference format") & value("ref-format", ref_format))),
            with_suffix("=", (option("--uem").doc("UEM files") & value("uem", uem_file))),
            with_suffix("=", (option("--collar").doc("Collar") & value("collar", collar))),
            option("--cross-show").doc("Cross show evaluation").set(cross_show),
            value("hypothesis file", hyp_file),
            value("reference file", ref_file)
            );


    if(!parse(argc, argv, cli)) {
        auto fmt = doc_formatting{}.doc_column(40);
        cerr << make_man_page(cli, argv[0], fmt);
        return EXIT_FAILURE;
    }

    try {

        SDEVAL_ASSERT(collar >= 0, "Error collar must be positive");
        SDEVAL_ASSERT(hyp_format == "rttm" || hyp_format == "mdtm","Error format hypothesis");
        SDEVAL_ASSERT(ref_format == "rttm" || ref_format == "mdtm","Error format reference");

        DiaFormat* hyp = NULL;
        DiaFormat* ref = NULL;
        UEM* uem = NULL;

        if(hyp_format == "mdtm")
            hyp = mdtm_file_parse(hyp_file);
        if(hyp_format == "rttm")
            hyp = rttm_file_parse(hyp_file);

        if(ref_format == "mdtm")
            ref = mdtm_file_parse(ref_file);
        if(ref_format == "rttm")
            ref = rttm_file_parse(ref_file);

        if(uem_file != "")
            uem = uem_file_parse(uem_file);

        map<string,map<int, SdResult *> > result;

        sd_evaluate(ref, hyp, result, collar, uem, cross_show);


        int fa_total = 0;
        int miss_total = 0;
        int conf_total = 0;
        int totaltime_total = 0;

        cout<<"Diarization                                 fa.       %    miss.       %    conf.       %     DER"<<endl;
        cout<<"--------------------------------------  -------  ------  -------  ------  -------  ------  ------"<<endl;

        for(map<string,map<int, SdResult *> >::const_iterator i = result.begin(); i != result.end(); i++) {
            for(map<int, SdResult *>::const_iterator j = i->second.begin(); j != i->second.end(); j++) {
                float fap = ((float)j->second->fa / (float)j->second->totaltime);
                float missp = ((float)j->second->miss / (float)j->second->totaltime);
                float confp = ((float)j->second->conf / (float)j->second->totaltime);
                float der = j->second->ser;

                cout<<std::left<<std::setw(38)<<i->first<<"  "<<std::right<<std::setw(7)<< j->second->fa << "  " << std::right<< std::fixed << setprecision(4) << std::setw(5) << fap << "  " << std::right << std::setw(7) << j->second->miss <<"  "<< std::right << setprecision(4) << std::setw(5) << missp <<"  " << std::right << std::setw(7) << j->second->conf << "  " << std::right<< setprecision(4) << std::setw(5) << confp << "  " << std::right << setprecision(4) << std::setw(5) << der <<endl;

                totaltime_total += j->second->totaltime;
                fa_total += j->second->fa;
                miss_total += j->second->miss;
                conf_total += j->second->conf;

            }
        }
        if(cross_show == false) {
            cout<<"--------------------------------------  -------  ------  -------  ------  -------  ------  ------"<<endl;
            cout<<std::left<<std::setw(38)<<"TOTAL"<<"  "<<std::right<<std::setw(7)<< fa_total << "  " << std::right<< std::fixed << setprecision(4) << std::setw(5) << (float)fa_total/(float)totaltime_total << "  " << std::right << std::setw(7) << miss_total <<"  "<< std::right << setprecision(4) << std::setw(5) << (float)miss_total / (float)totaltime_total<<"  " << std::right << std::setw(7) << conf_total << "  " << std::right<< setprecision(4) << std::setw(5) << (float)conf_total/(float)totaltime_total << "  " << std::right << setprecision(4) << std::setw(5) << float(fa_total+miss_total+conf_total)/(float)totaltime_total <<endl;
        }

        return EXIT_SUCCESS;
    }
    catch(const std::exception& e) {
        return EXIT_FAILURE;
    }


}
