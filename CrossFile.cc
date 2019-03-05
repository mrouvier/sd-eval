#include "CrossFile.h"
#include "Util.h"
#include <stdlib.h>
#include <stdio.h>

CrossFile::CrossFile(const MDTM * ref)
{

 
  for(map<string,map<int, Speakers *> >::const_iterator r_it = ref->spks.begin(); r_it != ref->spks.end(); r_it++) {
    for(map<int, Speakers*>::const_iterator r_itc = r_it->second.begin(); r_itc != r_it->second.end(); r_itc++) {
      int count = 0;
      for(unsigned int i = 0; i < r_itc->second->spks.size(); i++) {
	map<string, int>::iterator j = spkm.find(r_itc->second->spks[i]->name);
	if(j == spkm.end()) {
	  spks.push_back(r_itc->second->spks[i]->name);
	  spkm[r_itc->second->spks[i]->name] = spks.size()-1;
	  cluster.resize(spks.size());
	  cluster[spks.size()-1].push_back(info_speak(r_it->first, r_itc->first, count));
	  icluster[r_it->first][r_itc->first].push_back(spks.size()-1);
	} 
	else {
	  cluster[j->second].push_back(info_speak(r_it->first, r_itc->first, count));
	  icluster[r_it->first][r_itc->first].push_back(j->second);
	}
	count++;
      }
    }
  }
}

CrossFile::~CrossFile(){}

void CrossFile::print_speaker()
{
  int j = 0;
  for(vector<string>::const_iterator i = spks.begin(); i != spks.end(); i++) {
    fprintf(stdout,"Speaker %d : %s\n", j, to_utf8(*i).c_str());
    for(list<info_speak>::const_iterator ls = cluster[j].begin(); ls != cluster[j].end(); ls++) {
      fprintf(stdout,"File : %s -- Channel : %d -- Speaker : %d\n",to_utf8(file(*ls)).c_str(), channel(*ls), speak(*ls));
    }
    j++;
  }
  for(map<string, map<int, vector<int> > >::const_iterator i = icluster.begin(); i != icluster.end(); i++)
    for(map<int, vector<int> >::const_iterator ii = i->second.begin(); ii != i->second.end(); ii++)
      for(vector<int>::const_iterator iii = ii->second.begin(); iii != ii->second.end(); iii++)
	fprintf(stdout, "File : %s channel : %d Speaker : %d\n", to_utf8(i->first).c_str(), ii->first, *iii);


}

string CrossFile::file(const info_speak a)
{
  return a.file_name;

}

int CrossFile::channel(const info_speak a)
{

  return a.channel;
}

int CrossFile::speak(const info_speak a)
{
  return a.speaker;
}
