#ifndef __SDEVAL_CROSSFILE_H__
#define __SDEVAL_CROSSFILE_H__

#include <vector>
#include <map>
#include <list>
#include "DiaFormat.h"
#include <cmath>
#include <functional>

using namespace std;

class CrossFile {
    public: 
        struct info_speak {
            string file_name;
            int channel;
            int speaker;
            info_speak(string f, int c, int s) {
                file_name = f; channel = c; speaker = s;
            }
        };

        struct classcomp {
            bool operator() (const string& lhs, const string& rhs) const
            {return 0 < lhs.compare(rhs);}
        };




        vector<string> spks;
        map<string, int, classcomp> spkm;

        vector< list <info_speak > > cluster;
        map<string, map<int, vector<int> > > icluster;



        CrossFile(const DiaFormat*);
        ~CrossFile();

        void print_speaker();
        string file(const info_speak );
        int channel(const info_speak );
        int speak(const info_speak );

};

#endif
