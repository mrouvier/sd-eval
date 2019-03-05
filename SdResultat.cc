// SdResultat.cc
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



#include "SdResultat.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include "OptAssign.h"
#include "Util.h"

using namespace std;

SdResult::SdResult(Speakers *ref, Speakers *hyp, int delta, const UEM::rlist *uem)
{
  fa = miss = confus_ref = confus_hyp = totaltime = ser = conf = correct_time = 0;
  
  int nr = ref->spks.size();
  int nh = hyp->spks.size();
  cross_time_info *cti = new cross_time_info[nr * nh];

  confusion_matrix = new int[nr * nh];
  correct_ref = new int[nr];
  correct_hyp = new int[nh];
  lost_ref = new int[nr];
  lost_hyp = new int[nh];
  
  memset(cti, 0, sizeof(*cti)*nr*nh); 

  memset(confusion_matrix, 0, sizeof(*confusion_matrix)*nr*nh);
  memset(correct_ref, 0, sizeof(*correct_ref)*nr);
  memset(correct_hyp, 0, sizeof(*correct_hyp)*nh);
  memset(lost_ref, 0, sizeof(*lost_ref)*nr);
  memset(lost_hyp, 0, sizeof(*lost_hyp)*nh);

  vector<flist> fl_ref;
  fl_ref.resize(nr);

  vector<flist> fl_ref_trans;
  fl_ref_trans.resize(nr);

  vector<flist> fl_3way; // hyp-all + ref-all + ref-speaker
  fl_3way.resize(nr);

  vector<flist> fl_hyp;
  fl_hyp.resize(nh);

  flist fl_ref_all;
  flist fl_hyp_all;

  mapping_ser_rh.resize(max(nr, nh));
  mapping_ser_hr.resize(max(nr, nh));

  mapping_purity_hr.resize(nh);
  mapping_purity_rh.resize(nr);

 
  for(unsigned int i = 0; i != ref->spks.size(); i++) {
    flist fl;
    build_flist_from_turns(ref->spks[i]->turns, fl);

    if(uem) {
      flist flt;
      filter_flist_on_uem(fl, uem, fl_ref[i]);
      flist_add_transitions(fl, delta, flt);
      filter_flist_on_uem(flt, uem, fl_ref_trans[i]);

    } else {
      fl_ref[i] = fl;
      flist_add_transitions(fl, delta, fl_ref_trans[i]);
    }
  }
  
  build_flist_from_all_speakers(ref, fl_ref_all);
  build_flist_from_all_speakers(hyp, fl_hyp_all);

  if(uem) {
    flist fl;
    fl = fl_ref_all;
    filter_flist_on_uem(fl, uem, fl_ref_all);
    fl = fl_hyp_all;
    filter_flist_on_uem(fl, uem, fl_hyp_all);
  }

  flist_build_3way_merge(fl_hyp_all, fl_ref_all, fl_ref_trans, fl_3way);

  for(unsigned int i = 0; i != hyp->spks.size(); i++) {
    flist fl;
    build_flist_from_turns(hyp->spks[i]->turns, fl);
   
    if(uem)
      filter_flist_on_uem(fl, uem, fl_hyp[i]);
    else
      fl_hyp[i] = fl;
  }

  compute_per_speaker_time(fl_ref, speech_time_ref);
  compute_per_speaker_time(fl_hyp, speech_time_hyp);
  compute_mapping_error_times(fl_3way, fl_hyp, cti);

  compute_miss(fl_ref_all, fl_hyp, miss_hyp);
  compute_miss(fl_hyp_all, fl_ref, miss_ref);

  compute_delta_error(speech_time_ref, miss_hyp, cti);
  map_ser(cti, nr, nh);

  vector<flist> fl_ref_mixed;
  mix_references_for_scoring(fl_ref, fl_ref_trans, fl_ref_mixed);
  accumulate_confusion(fl_ref_mixed, fl_hyp);

  ser_extract_mapping(ref, hyp, cti);
  compute_ser(cti);

  map_purity(cti, nr, nh);
  compute_purity(cti, nr, nh);

  delete[] cti;
}

SdResult::SdResult(const MDTM * ref, const MDTM * hyp,  int delta, const UEM * uem)
{
  fa = miss = confus_ref = confus_hyp = totaltime = ser = conf = correct_time = 0;

  CrossFile *cref = new CrossFile(ref);
  CrossFile *chyp = new CrossFile(hyp);

  int sr = cref->spks.size();
  int sh = chyp->spks.size();

  cross_time_info *cti = new cross_time_info[sr * sh];

  confusion_matrix = new int[sr * sh];
  correct_ref = new int[sr];
  correct_hyp = new int[sh];
  lost_ref = new int[sr];
  lost_hyp = new int[sh];

  memset(cti, 0, sizeof(*cti)*sr*sh); 

  memset(confusion_matrix, 0, sizeof(*confusion_matrix)*sr*sh);
  memset(correct_ref, 0, sizeof(*correct_ref)*sr);
  memset(correct_hyp, 0, sizeof(*correct_hyp)*sh);
  memset(lost_ref, 0, sizeof(*lost_ref)*sr);
  memset(lost_hyp, 0, sizeof(*lost_hyp)*sh);

  mapping_ser_rh.resize(max(sr, sh));
  mapping_ser_hr.resize(max(sr, sh));

  speech_time_ref.resize(sr);
  speech_time_hyp.resize(sh);

  vector<string>::const_iterator rp = cref->spks.begin();
  vector<string>::const_iterator rh = chyp->spks.begin();

  for(map<string, map<int, Speakers *> >::const_iterator r_it = ref->spks.begin(); r_it != ref->spks.end(); r_it++) {
    map<string, map<int, Speakers *> >::const_iterator h_it = hyp->spks.find(r_it->first);
    
    for(map<int, Speakers*>::const_iterator r_itc = r_it->second.begin(); r_itc != r_it->second.end(); r_itc++) {
      map<int, Speakers*>::const_iterator h_itc = h_it->second.find(r_itc->first);
      const UEM::rlist *luem = uem ? uem->find_range(r_it->first, r_itc->first) : NULL;

      Speakers *r = r_itc->second;
      Speakers *h = h_itc->second;
      int nr = r->spks.size();
      int nh = h->spks.size();
	  
      vector<int> sp_ref; 
      vector<int> sp_hyp;

      vector<flist> fl_ref;
      fl_ref.resize(nr);

      vector<flist> fl_ref_trans;
      fl_ref_trans.resize(nr);

      vector<flist> fl_3way;
      fl_3way.resize(nr);
	  
      vector<flist> fl_hyp;
      fl_hyp.resize(nh);

      flist fl_ref_all;
      flist fl_hyp_all;
	  
      for(unsigned int i = 0; i != r->spks.size(); i++) {
	flist fl;
	build_flist_from_turns(r->spks[i]->turns, fl);

	if(luem) {
	  flist flt;
	  filter_flist_on_uem(fl, luem, fl_ref[i]);
	  flist_add_transitions(fl, delta, flt);
	  filter_flist_on_uem(flt, luem, fl_ref_trans[i]);

	} else {
	  fl_ref[i] = fl;
	  flist_add_transitions(fl, delta, fl_ref_trans[i]);
	}
      }
	  
      build_flist_from_all_speakers(r, fl_ref_all);
      build_flist_from_all_speakers(h, fl_hyp_all);
	  

      if(luem) {
	flist fl;
	fl = fl_ref_all;
	filter_flist_on_uem(fl, luem, fl_ref_all);
	fl = fl_hyp_all;
	filter_flist_on_uem(fl, luem, fl_hyp_all);
      }

      flist_build_3way_merge(fl_hyp_all, fl_ref_all, fl_ref_trans, fl_3way);

      for(unsigned int i = 0; i != h->spks.size(); i++) {
	flist fl;
	build_flist_from_turns(h->spks[i]->turns, fl);
   
	if(luem)
	  filter_flist_on_uem(fl, luem, fl_hyp[i]);
	else
	  fl_hyp[i] = fl;
      }

      const int *mapper_r = &cref->icluster[r_it->first][r_itc->first][0];      
      const int *mapper_h = &chyp->icluster[r_it->first][r_itc->first][0];      
	  
      accumulate_per_speaker_time(fl_ref, speech_time_ref, mapper_r);
      accumulate_per_speaker_time(fl_hyp, speech_time_hyp, mapper_h);
      compute_per_speaker_time(fl_ref, sp_ref);
      compute_per_speaker_time(fl_hyp, sp_hyp);
	  
      compute_mapping_error_times(fl_3way , fl_hyp, cti, mapper_r, mapper_h, sh, r, h, cref, chyp, fl_hyp_all, fl_ref_all);

      compute_miss(fl_ref_all, fl_hyp, miss_hyp, mapper_h, sh);
      compute_miss(fl_hyp_all, fl_ref, miss_ref, mapper_r, sr);
    }
  }
  
  compute_delta_error(speech_time_ref, miss_hyp, cti);      
  map_ser(cti, sr, sh);
  ser_extract_mapping(cref, chyp, cti);
  //  print_ser(cref, chyp);
  //  fprintf(stdout,"SER : %f | FA : %d | MISS : %d CONF : %d  |CORRECT : %d | TOTAL_TIME : %d\n",ser, fa, miss, conf, correct_time, totaltime);


  for(map<string, map<int, Speakers *> >::const_iterator r_it = ref->spks.begin(); r_it != ref->spks.end(); r_it++) {
    map<string, map<int, Speakers *> >::const_iterator h_it = hyp->spks.find(r_it->first);
    
    for(map<int, Speakers*>::const_iterator r_itc = r_it->second.begin(); r_itc != r_it->second.end(); r_itc++) {
      map<int, Speakers*>::const_iterator h_itc = h_it->second.find(r_itc->first);
      const UEM::rlist *luem = uem ? uem->find_range(r_it->first, r_itc->first) : NULL;

      Speakers *r = r_itc->second;
      Speakers *h = h_itc->second;
      int nr = r->spks.size();
      int nh = h->spks.size();
	  
      vector<flist> fl_ref;
      fl_ref.resize(nr);

      vector<flist> fl_ref_trans;
      fl_ref_trans.resize(nr);

      vector<flist> fl_hyp;
      fl_hyp.resize(nh);

      flist fl_ref_all;
      flist fl_hyp_all;
	  
      for(unsigned int i = 0; i != r->spks.size(); i++) {
	flist fl;
	build_flist_from_turns(r->spks[i]->turns, fl);

	if(luem) {
	  flist flt;
	  filter_flist_on_uem(fl, luem, fl_ref[i]);
	  flist_add_transitions(fl, delta, flt);
	  filter_flist_on_uem(flt, luem, fl_ref_trans[i]);

	} else {
	  fl_ref[i] = fl;
	  flist_add_transitions(fl, delta, fl_ref_trans[i]);
	}
      }

      for(unsigned int i = 0; i != h->spks.size(); i++) {
	flist fl;
	build_flist_from_turns(h->spks[i]->turns, fl);
   
	if(luem)
	  filter_flist_on_uem(fl, luem, fl_hyp[i]);
	else
	  fl_hyp[i] = fl;
      }

      const int *mapper_r = &cref->icluster[r_it->first][r_itc->first][0];      
      const int *mapper_h = &chyp->icluster[r_it->first][r_itc->first][0];      

      vector<flist> fl_ref_mixed;
      mix_references_for_scoring(fl_ref, fl_ref_trans, fl_ref_mixed, mapper_r, mapper_h);
      accumulate_confusion(fl_ref_mixed, fl_hyp, mapper_r, mapper_h, sr, sh);	  
    }
  }

  compute_ser(cti);
  delete[] cti;
}

SdResult::~SdResult()
{
  delete[] confusion_matrix;
  delete[] correct_ref;
  delete[] correct_hyp;
  delete[] lost_ref;
  delete[] lost_hyp;
}

void SdResult::compute_delta_error(const vector<int> &speech_time_ref, const vector<int> &miss_hyp, cross_time_info *cti)
{
  int nh = miss_hyp.size();
  for(unsigned int r = 0; r < speech_time_ref.size(); r++)
    for(unsigned int h = 0; h < miss_hyp.size(); h++) {
      cross_time_info &i = cti[r*nh+h];
      i.delta_error = speech_time_ref[r] + miss_hyp[h] - i.fa - i.miss - i.conf;
    }
}

void SdResult::print_ser(Speakers * ref, Speakers * hyp)
{
  fprintf(stdout,"--------------------------------------------------------------\n");
  fprintf(stdout,"SER : %f | FA : %d | MISS : %d CONF : %d  | CORRECT : %d | TOTAL_TIME : %d\n",ser, fa, miss, conf, correct_time, totaltime);
  fprintf(stdout,"--------------------------------------------------------------\n");
  fprintf(stdout,"Speakers Mapping Ref (%d) to Hyp (%d) :\n",(int)ref->spks.size(),(int)hyp->spks.size());
  for(unsigned int it = 0; it != ref->spks.size(); it++) {
    fprintf(stdout," %s --> %s \n ",to_utf8(ref->spks[it]->name).c_str(),mapping_ser_rh[it] != -1 ? to_utf8(hyp->spks[mapping_ser_rh[it]]->name).c_str() : "AUCUN");
     }
  
  
}
void SdResult::print_ser(CrossFile * ref, CrossFile * hyp )
{
  for(unsigned int it = 0; it != ref->spks.size(); it++)
    fprintf(stdout," %s --> %s \n ",to_utf8(ref->spks[it]).c_str(),mapping_ser_rh[it] != -1 ? to_utf8(hyp->spks[mapping_ser_rh[it]]).c_str() : "AUCUN");
}

void SdResult::ser_extract_mapping(const Speakers *ref, const Speakers *hyp, const cross_time_info *cti)
{
  unsigned int nr = ref->spks.size();
  unsigned int nh = hyp->spks.size();

  ref_names.resize(nr);
  for(unsigned int r = 0; r != nr; r++)
    ref_names[r] = ref->spks[r]->name;
  hyp_names.resize(nh);
  for(unsigned int h = 0; h != nh; h++)
    hyp_names[h] = hyp->spks[h]->name;

  for(unsigned int r = 0; r != nr; r++) {
    rh[ref->spks[r]->name] = mapping_ser_rh[r] != -1 ? hyp->spks[mapping_ser_rh[r]]->name : "AUCUN";
    list<pair<string, int> > &tm = rtimes[ref->spks[r]->name];
    for(unsigned int h=0; h != nh; h++)
      if(cti[r*nh+h].common)
	tm.push_back(pair<string, int>(hyp->spks[h]->name, cti[r*nh+h].common));
  }
}

void SdResult::ser_extract_mapping(const CrossFile *ref, const CrossFile *hyp, const cross_time_info *cti)
{
  unsigned int nr = ref->spks.size();
  unsigned int nh = hyp->spks.size();

  ref_names.resize(nr);
  for(unsigned int r = 0; r != nr; r++)
    ref_names[r] = ref->spks[r];
  hyp_names.resize(nh);
  for(unsigned int h = 0; h != nh; h++)
    hyp_names[h] = hyp->spks[h];

  for(unsigned int r = 0; r != nr; r++) {
    rh[ref->spks[r]] = mapping_ser_rh[r] != -1 ? hyp->spks[mapping_ser_rh[r]] : "AUCUN";
    list<pair<string, int> > &tm = rtimes[ref->spks[r]];
    for(unsigned int h=0; h != nh; h++)
      if(cti[r*nh+h].common)
	tm.push_back(pair<string, int>(hyp->spks[h], cti[r*nh+h].common));
  }
}

void SdResult::compute_ser(const cross_time_info *cti)
{
  int nh = hyp_names.size();
  int nr = ref_names.size();

  for(int i=0; i<nh*nr; i++)
    conf += confusion_matrix[i];

  for(int i=0; i<nr; i++) {
    totaltime += speech_time_ref[i];
    miss += lost_ref[i];
    correct_time += correct_ref[i];
  }

  for(int i=0; i<nh; i++) {
    fa += lost_hyp[i];
    correct_time += correct_hyp[i];
  }

  ser = (double)(fa + miss + conf) / (double)totaltime;  
}

void SdResult::map_ser(const cross_time_info *cti, int nr, int nh)
{
  int nbElement = max(nh, nr);
  OptAssign *T = new OptAssign(nbElement);
  T->clear_cost_values();

  for(int r = 0; r < nr; r++)
    for(int h = 0; h < nh; h++)
      T->set_cost_value(r, h, cti[r * nh + h].delta_error);

  int *n =T->munker();

  for(int i=0; i < nbElement; i++){
    mapping_ser_hr[i] = -1;
    mapping_ser_rh[i] = -1;
  }
  
  for(int r=0; r < nr; r++){
    if(n[r] < nh) {
      if(cti[r * nh + n[r]].delta_error > 0 && cti[r * nh + n[r]].common > 0) {
	mapping_ser_rh[r] = n[r];
	mapping_ser_hr[n[r]] = r;
      }
    }  
  }
  
  
  delete T;
  delete[] n;

}

void SdResult::map_purity(const cross_time_info *cti, int nr, int nh)
{
  //hypothese
  for(int j = 0 ; j < nh ;j++) {
    int tmp = 0;
    int pos = 0;
    int tmp_de =0;
    for(int i = 0 ; i < nr ; i++) {
      if(cti[i*nh+j].common > tmp) {
	tmp = cti[i*nh+j].common;
	pos = i;
	tmp_de = cti[i*nh+j].delta_error;
      }
      if(cti[i*nh+j].common == tmp) {
	if(cti[i*nh+j].delta_error > tmp_de) {
	  tmp = cti[i*nh+j].common;
	  pos = i;
	  tmp_de = cti[i*nh+j].delta_error;
	}
      }
    }
    
    mapping_purity_hr[j] = pos;
  }

  //reference

  for(int j = 0 ; j < nr ;j++) {
    int tmp = 0;
    int pos = 0;
    int tmp_de =0;
    for(int i = 0 ; i < nh ; i++) {
      if(cti[j*nh+i].common > tmp) {
	tmp = cti[j*nh+i].common;
	pos = i;
	tmp_de = cti[j*nh+i].delta_error;
      }
      if(cti[j*nh+i].common == tmp) {
	if(cti[j*nh+i].delta_error > tmp_de) {
	  tmp = cti[j*nh+i].common;
	  pos = i;
	  tmp_de = cti[j*nh+i].delta_error;
	}
      }
    }
    
    mapping_purity_rh[j] = pos;
  }
}

void SdResult::compute_purity(const cross_time_info *cti, int nr, int nh)
{
 
  //hypothese 
  double pih = 0;
  double pir = 0;
  double pith = 0;
  double pitr = 0;
  double th = 0;
  double tr = 0;
  int nb_h = 0;
  int nb_r = 0;

  for(int i = 0; i< nh; i++) {
    int posr =  mapping_purity_hr[i];
    if(speech_time_hyp[i] > 0) {
    pih += (speech_time_hyp[i] - cti[posr*nh+i].erreur_hyp)/(double)speech_time_hyp[i];
    pir += (speech_time_ref[posr] - cti[posr*nh+i].erreur_ref)/(double)speech_time_ref[posr];
    pith += speech_time_hyp[i] - cti[posr*nh+i].erreur_hyp;
    pitr += speech_time_ref[posr] - cti[posr*nh+i].erreur_ref;
    th += speech_time_hyp[i];
    tr += speech_time_ref[posr];
    nb_h++;
    }

  }
  purity_hyp1 = pih / nb_h;
  purity_hyp2 = pir / nb_h;
  purity_hyp3 = pith / th;
  purity_hyp4 = pitr / tr;

  //reference
 pih = pir = pith = pitr = th = tr = 0;
 
  for(int i = 0; i< nr; i++) {
    int posr =  mapping_purity_rh[i]; 
    if(speech_time_ref[i] > 0) {
    pir += (speech_time_ref[i] - cti[i*nh+posr].erreur_ref)/(double)speech_time_ref[i];
    pih += (speech_time_hyp[posr] - cti[i*nh+posr].erreur_hyp)/(double)speech_time_hyp[posr];
    pith += speech_time_hyp[posr] - cti[i*nh+posr].erreur_hyp;
    pitr += speech_time_ref[i] - cti[i*nh+posr].erreur_ref;
    th += speech_time_hyp[posr];
    tr += speech_time_ref[i];
    nb_r++;
    }

  }
  purity_ref1 = pih / nb_r;
  purity_ref2 = pir / nb_r;
  purity_ref3 = pith / th;
  purity_ref4 = pitr / tr;

  
} 

void SdResult::print_purity()
{
  fprintf(stdout,"--------------------------------------------------------------\n");
  fprintf(stdout,"Purity :\n Hypothese : %f %f %f %f \n Reference : %f %f %f %f \n", purity_hyp1, purity_hyp2, purity_hyp3, purity_hyp4, purity_ref1, purity_ref2, purity_ref3, purity_ref4);
  fprintf(stdout,"--------------------------------------------------------------\n");
}

void SdResult::compute_mapping_error_times(const vector<flist> &fl_3way, const vector<flist> &fl_hyp, cross_time_info *cti)
{
  int nh = fl_hyp.size();
  for(unsigned int r = 0; r != fl_3way.size(); r++)
    for(unsigned int h = 0; h != fl_hyp.size(); h++)
      accumulate_mapping_error_times(fl_hyp[h], fl_3way[r], cti[r*nh+h]);
}

void SdResult::compute_mapping_error_times(const vector<flist> &fl_3way, const vector<flist> &fl_hyp, cross_time_info *cti,
					   const int *mapper_r, const int *mapper_h, int sh,
					   Speakers *ref, Speakers *hyp, CrossFile *cref, CrossFile *chyp, flist fl_hyp_all, flist fl_ref_all)
{
  vector<string> tt;
  tt.resize(cref->spks.size());
  copy(cref->spks.begin(), cref->spks.end(), tt.begin());
  vector<string>::iterator it;

  for(unsigned int r_it = 0; r_it != fl_3way.size(); r_it++) {
    it = find(tt.begin(), tt.end(), ref->spks[r_it]->name);
    tt.erase(it);
    
    vector<string> th;
    th.resize(chyp->spks.size());
    copy(chyp->spks.begin(), chyp->spks.end(), th.begin());
    vector<string>::iterator ith;
    
    for(unsigned int h_it = 0; h_it != fl_hyp.size(); h_it++) {
      ith = find(th.begin(), th.end(), hyp->spks[h_it]->name);
      th.erase(ith);    
      
      accumulate_mapping_error_times(fl_hyp[h_it], fl_3way[r_it], cti[mapper_r[r_it] * sh + mapper_h[h_it]]);
    }

    flist fhyp;
    for(int i = 0; i != (int)th.size(); i++)
      accumulate_mapping_error_times(fhyp, fl_3way[r_it], cti[mapper_r[r_it] * sh + chyp->spkm[th[i]]]);
  }

  vector<flist> fref2;
  fref2.resize(tt.size());
  vector<flist> fref;
  flist_build_3way_merge(fl_hyp_all, fl_ref_all, fref2, fref);
  
  for(int i = 0; i != (int)tt.size(); i++)
    for(unsigned int h_it = 0; h_it != fl_hyp.size(); h_it++)
      accumulate_mapping_error_times(fl_hyp[h_it], fref[0], cti[cref->spkm[tt[i]] * sh + mapper_h[h_it]]);
}


void SdResult::compute_miss(const flist &opposite_all, const vector<flist> &fl, vector<int> &result, const int *mapping, int s)
{
  if(result.size() != (unsigned int)s) {
    result.resize(s);
    memset(&result[0], 0, s*sizeof(int));
  }
  for(unsigned int i = 0; i != fl.size(); i++)
    result[mapping[i]] += flist_compute_missing_time(fl[i], opposite_all);
}

void SdResult::compute_miss(const flist &opposite_all, const vector<flist> &fl, vector<int> &result)
{
  result.resize(fl.size());
  for(unsigned int i = 0; i != fl.size(); i++)
    result[i] = flist_compute_missing_time(fl[i], opposite_all);
}

int SdResult::compute_single_speaker_time(const flist &fl)
{
  int total_time = 0;
  int previous = 0;

  for(flist::const_iterator fli = fl.begin(); fli != fl.end(); fli++) {
    if(fli->mode == frontier::POSITIF)
      total_time += fli->time - previous;
    previous = fli->time;
  }

  return total_time;
}

void SdResult::compute_per_speaker_time(const vector<flist> &fl, vector<int> &speech_time)
{
  speech_time.resize(fl.size());
  for(unsigned int i=0; i != fl.size(); i++)
    speech_time[i] = compute_single_speaker_time(fl[i]);
}

void SdResult::accumulate_per_speaker_time(const vector<flist> &fl, vector<int> &speech_time, const int *mapping)
{
  for(unsigned int i=0; i != fl.size(); i++)
    speech_time[mapping[i]] += compute_single_speaker_time(fl[i]);
}

namespace {
  struct turn_less {
    bool operator() (const Turn * T1, const Turn *T2) const {
      return T1->stime < T2->stime;
    }
  };
}

void SdResult::build_flist_from_all_speakers(const Speakers *spk, flist &fo)
{
  vector<Turn *> spt;
  for(unsigned int i = 0; i != spk->spks.size(); i++) {
    int nt = spk->spks[i]->turns.size();
    int ct = spt.size();
    spt.resize(ct + nt);
    memcpy(&spt[ct], &spk->spks[i]->turns[0], nt * sizeof(Turn *));
  }
  
  sort(spt.begin(), spt.end(), turn_less());
  build_flist_from_turns(spt, fo);
}

int SdResult::merge_mode_hyp_ref_all(int mode_hyp, int mode_ref)
{
  switch(mode_hyp) {
  case frontier::POSITIF:
    switch(mode_ref) {
    case frontier::POSITIF: return frontier::PP;
    case frontier::NEGATIF: return frontier::PN;
    default: abort();
    }
  case frontier::NEGATIF:
    switch(mode_ref) {
    case frontier::POSITIF: return frontier::NP;
    case frontier::NEGATIF: return frontier::NN;
    default: abort();
    }
  default: abort();
  }
}

int SdResult::merge_mode_hr_all_ref(int mode_hr, int mode_ref)
{
  switch(mode_hr) {
  case frontier::PP:
    switch(mode_ref) {
    case frontier::POSITIF:    return frontier::PPP;
    case frontier::NEGATIF:    return frontier::PPN;
    case frontier::TRANSITION: return frontier::PPT;
    default: abort();
    }
  case frontier::PN:
    switch(mode_ref) {
    case frontier::NEGATIF:    return frontier::PNN;
    case frontier::TRANSITION: return frontier::PNT;
    default: abort();
    }
  case frontier::NP:
    switch(mode_ref) {
    case frontier::POSITIF:    return frontier::NPP;
    case frontier::NEGATIF:    return frontier::NPN;
    case frontier::TRANSITION: return frontier::NPT;
    default: abort();
    }
  case frontier::NN:
    switch(mode_ref) {
    case frontier::NEGATIF:    return frontier::NNN;
    case frontier::TRANSITION: return frontier::NNT;
    default: abort();
    }
  default: abort();
  }
}

void SdResult::flist_merge(const flist &f1, const flist &f2, int m1_post, int m2_post, int (*merge)(int f1m, int f2m), flist &result)
{
  flist::const_iterator f1i = f1.begin();
  flist::const_iterator f2i = f2.begin();
  while(f1i != f1.end() || f2i != f2.end()) {
    int time;
    if(f2i == f2.end()) 
      time = f1i->time;
    else if(f1i == f1.end()) 
      time = f2i->time;
    else
      time = min(f1i->time,f2i->time);

    int mode1 = f1i == f1.end() ? m1_post : f1i->mode;
    int mode2 = f2i == f2.end() ? m2_post : f2i->mode;

    int final_mode = merge(mode1, mode2);
    result.push_back(frontier(final_mode, time));

    if(f1i != f1.end() && f1i->time == time)
      f1i++;

    if(f2i != f2.end() && f2i->time == time)
      f2i++;
  }

} 

void SdResult::flist_build_3way_merge(const flist &fl_hyp_all, const flist &fl_ref_all, const vector<flist> &fl_ref, vector<flist> &fl_3way)
{
  flist fl_hr_all;
  flist_merge(fl_hyp_all, fl_ref_all, frontier::NEGATIF, frontier::NEGATIF, merge_mode_hyp_ref_all, fl_hr_all);

  fl_3way.resize(fl_ref.size());
  for(unsigned int i = 0; i < fl_ref.size(); i++) 
     flist_merge(fl_hr_all, fl_ref[i], frontier::NN, frontier::NEGATIF, merge_mode_hr_all_ref, fl_3way[i]);
}

   
void SdResult::build_flist_from_turns(const vector<Turn *> &spt, flist &fl)
{  
  fl.clear();
 
  for(vector<Turn *>::const_iterator it = spt.begin(); it != spt.end(); it++) {
    int start = (*it)->stime;
    int end = (*it)->etime;
  
    if(fl.empty()) {
      fl.push_back(frontier(frontier::NEGATIF, start));
      fl.push_back(frontier(frontier::POSITIF, end));

    } else if(end > fl.back().time) {
      if(start > fl.back().time) {	
	fl.push_back(frontier(frontier::NEGATIF, start));
	fl.push_back(frontier(frontier::POSITIF, end));
      } else
	fl.back().time = end;
    }
  }
}

void SdResult::filter_flist_on_uem(const flist &fi, const list<UEM::Range> *uem, flist &fo)
{
  fo.clear();

  list<UEM::Range>::const_iterator uemi = uem->begin();
  flist::const_iterator fii = fi.begin();

  while(fii != fi.end() && uemi != uem->end()) {
    if(uemi->stime < fii->time) {
      if(uemi->etime >= fii->time) {
	if(fii->mode != frontier::NEGATIF) {
	  if(fo.empty() || fo.back().time < uemi->stime)
	    fo.push_back(frontier(frontier::NEGATIF, uemi->stime));
	  fo.push_back(frontier(fii->mode, fii->time));

	} else
	  fo.push_back(*fii);
	fii++;

      } else {
	if(fii->mode != frontier::NEGATIF) {
	  if(fo.empty() || fo.back().time < uemi->stime)
	    fo.push_back(frontier(frontier::NEGATIF, uemi->stime));
	  fo.push_back(frontier(fii->mode, uemi->etime));
	} 
	uemi++;
      }

    } else 
      fii++;
  }
}


void SdResult::flist_add_transitions(const flist &fi, int delta, flist &fo)
{ 
  if(!delta) {
    fo = fi;
    return;
  }

  for(flist::const_iterator fii = fi.begin(); fii != fi.end(); fii++) {  
    int start = fii->time - delta;
    int end = fii->time + delta;
    int cmode = fii->mode;

    if(start < 0)
      start = 0;
    
    if(fii == fi.begin()) {
      fo.push_back(frontier(cmode, start));
      fo.push_back(frontier(frontier::TRANSITION, end));

    } else {
      if(start > fo.back().time ) {
	fo.push_back(frontier(cmode, start));
	fo.push_back(frontier(frontier::TRANSITION, end));

      } else if(start < fo.back().time ) {
	  fo.pop_back();
	  fo.push_back(frontier(frontier::TRANSITION, end));
	}
      }
    fo.back().time = end; 
  }
}

void SdResult::accumulate_mapping_error_times(const flist &fl_hyp, const flist &fl_3way, cross_time_info &cti)
{
  flist::const_iterator h_it = fl_hyp.begin();
  int tbeg = 0;
  int thyp = 0;
  int hypbef = 0;
  for(flist::const_iterator f_it = fl_3way.begin(); f_it != fl_3way.end(); f_it++) {
    int tend = f_it->time;
    while(h_it != fl_hyp.end()) {
      int dinter = min(h_it->time, tend);
      if(h_it->mode == frontier::POSITIF)
	thyp += dinter - hypbef;
      if(h_it->time > tend)
	break;
      hypbef = dinter;
      h_it++; 
    } 
    
    accumulate_mapping_error_time(thyp, tend-tbeg-thyp, f_it->mode, cti);

    if(h_it != fl_hyp.end()) 
      hypbef = min(h_it->time, tend);
    tbeg = tend; 
    thyp = 0;
  }
  while(h_it != fl_hyp.end()) {
    if(h_it->mode == frontier::POSITIF)
      thyp += h_it->time - tbeg;
    tbeg=h_it->time;
    h_it++;
  }
  
  accumulate_mapping_error_time(thyp, 0, frontier::PNN, cti);
}

void SdResult::accumulate_mapping_error_time(int thyp, int thyn, int mode, cross_time_info &cti)
{
  
  switch(mode) {
  case frontier::PPP:
    cti.common += thyp;
    cti.conf += thyn;
    cti.erreur_ref += thyn;
  case frontier::PPN:
    cti.erreur_hyp += thyp;
    break;
  case frontier::PPT:
    cti.common += thyp;
    break;
  case frontier::PNN:
    cti.fa += thyp;
    cti.erreur_hyp += thyp;
    break;
  case frontier::PNT:
    cti.common += thyp;
    break;
  case frontier::NPP:
    cti.miss += thyn;
    cti.erreur_ref += thyn;
    break;
  case frontier::NPN:
    break;
  case frontier::NPT:
    break;
  case frontier::NNN:
    break;
  case frontier::NNT:
    break;
  default:
    abort();
  }
}

int SdResult::flist_compute_missing_time(const flist &hyp, const flist &opposite_all)
{
  int fa = 0;
  flist::const_iterator hi = hyp.begin();
  int tbeg = 0;
  int thyp = 0;
  int hypbef = 0;
  for(flist::const_iterator oi = opposite_all.begin(); oi != opposite_all.end(); oi++) {
    int tend = oi->time;
    while(hi != hyp.end()) {
      int dinter = min(hi->time, tend);
      if(hi->mode == frontier::POSITIF)
	thyp += dinter - hypbef;
      
      if(hi->time > tend)
	break;
      hypbef = dinter;
      
      hi++; 
      
    } 
    
    if(oi->mode == frontier::NEGATIF)
      fa += thyp;
    if(hi != hyp.end())
      hypbef = min(hi->time, tend);
    tbeg = tend; 
    thyp = 0;
  }

  while(hi != hyp.end()) {
    if(hi->mode == frontier::POSITIF)
      thyp += hi->time - tbeg;
    tbeg=hi->time;
    hi++;
  } 
 
  fa += thyp;
  return fa;
}

void SdResult::mix_references_for_scoring(const vector<flist> &fl_ref, const vector<flist> &fl_ref_trans, vector<flist> &fl_ref_mixed, const int *mapper_r, const int *mapper_h)
{
  fl_ref_mixed.resize(fl_ref.size());
  for(unsigned int i=0; i != fl_ref.size(); i++)
    if(mapping_ser_rh[mapper_r ? mapper_r[i] : i] != -1)
      fl_ref_mixed[i] = fl_ref_trans[i];
    else
      fl_ref_mixed[i] = fl_ref[i];
}

void SdResult::accumulate_confusion(const vector<flist> &fl_ref, const vector<flist> &fl_hyp, const int *mapper_r, const int *mapper_h, int sr, int sh)
{
  vector<flist::const_iterator> flri;
  flri.resize(fl_ref.size());
  for(unsigned int i=0; i != fl_ref.size(); i++)
    flri[i] = fl_ref[i].begin();

  vector<flist::const_iterator> flhi;
  flhi.resize(fl_hyp.size());
  for(unsigned int i=0; i != fl_hyp.size(); i++)
    flhi[i] = fl_hyp[i].begin();

  int cur_time = 0;
  for(;;) {
    vector<unsigned int> ridx;
    vector<bool> r_is_t;
    vector<unsigned int> hidx;
    int time = -1;

    // Build the list of who's in the segment

    for(unsigned int i=0; i != fl_ref.size(); i++)
      if(flri[i] != fl_ref[i].end()) {
	if(time == -1 || flri[i]->time < time)
	  time = flri[i]->time;
	if(flri[i]->mode != frontier::NEGATIF) {
	  ridx.push_back(i);
	  r_is_t.push_back(flri[i]->mode == frontier::TRANSITION);
	}
      }

    for(unsigned int i=0; i != fl_hyp.size(); i++)
      if(flhi[i] != fl_hyp[i].end()) {
	if(time == -1 || flhi[i]->time < time)
	  time = flhi[i]->time;
	if(flhi[i]->mode != frontier::NEGATIF)
	  hidx.push_back(i);
      }

    if(time == -1)
      break;

    if(cur_time == 2530110)
      fprintf(stderr, "%d - %d\n", int(ridx.size()), int(hidx.size()));

    // Shortcut when nobody's home
    if(!ridx.empty() || !hidx.empty()) {
      int duration = time - cur_time;
    
      // Hyp and ref mapped together end up in correct time and are
      // removed from the lists
      for(unsigned int i=0; i != ridx.size();) {
	unsigned int r = ridx[i];
	bool dropped = false;
	if(mapping_ser_rh[mapper_r ? mapper_r[r] : r] != -1) {
	  unsigned int h = mapping_ser_rh[mapper_r ? mapper_r[r] : r];
	  unsigned int j;
	  for(j=0; j != hidx.size(); j++)
	    if((mapper_r ? mapper_h[hidx[j]] : hidx[j]) == h)
	      break;
	  if(j != hidx.size()) {
	    correct_ref[mapper_r ? mapper_r[r] : r] += duration;
	    correct_hyp[h] += duration;
	    ridx.erase(ridx.begin() + i);
	    r_is_t.erase(r_is_t.begin() + i);
	    hidx.erase(hidx.begin() + j);
	    dropped = true;
	    if(cur_time == 2530110)
	      fprintf(stderr, "hyp/ref %d/%d dropped\n", i, j);
	  }
	}      
	if(!dropped)
	  i++;
      }

      // Ref in transition is removed from the list if mapped to some hyp
      for(unsigned int i=0; i != ridx.size();) {
	unsigned int r = ridx[i];
	if(r_is_t[i] && mapping_ser_rh[r] != -1) {
	  ridx.erase(ridx.begin() + i);
	  r_is_t.erase(r_is_t.begin() + i);

	} else
	  i++;
      }

      if(hidx.empty()) {
	// If there's no hyp, we're all in lost_ref
	//if(!ridx.empty())
	//  printf("%d %d -- %d -- %d\n", cur_time, time, int(ridx.size()), duration);
	for(unsigned int i=0; i != ridx.size(); i++)
	  lost_ref[mapper_r ? mapper_r[ridx[i]] : ridx[i]] += duration;

      } else if(ridx.empty()) {
	// If there's no ref, we're all in lost_hyp
	for(unsigned int i=0; i != hidx.size(); i++)
	  lost_hyp[mapper_h ? mapper_h[hidx[i]] : hidx[i]] += duration;

      } else {
	// Otherwise we're in confusion.  Amount of confusion time to give
	// is equal to the max of the ref and hyp times

	unsigned int conf_time = ridx.size() > hidx.size() ? ridx.size()*duration : hidx.size()*duration;

	// Number of slots, otoh, is equal to the product of the number of
	// refs and hyps
	unsigned int conf_slots = ridx.size()*hidx.size();

	// Give the time equally in all slots, but ensure no loss through rounding
	unsigned int slot = 0;
	for(unsigned int i=0; i != ridx.size(); i++) {
	  unsigned int r = ridx[i];
	  for(unsigned int j=0; j != hidx.size(); j++) {
	    unsigned int h = hidx[j];
	    confusion_matrix[mapper_r ? mapper_r[r]*sh + mapper_h[h] : r*fl_hyp.size() + h] += (slot+1)*conf_time/conf_slots - slot*conf_time/conf_slots;
	    slot++;
	  }
	}
      }
    }

    for(unsigned int i=0; i != fl_ref.size(); i++)
      if(flri[i] != fl_ref[i].end() && flri[i]->time == time)
	flri[i]++;

    for(unsigned int i=0; i != fl_hyp.size(); i++)
      if(flhi[i] != fl_hyp[i].end() && flhi[i]->time == time)
	flhi[i]++;

    cur_time = time;
  }
}


void sd_evaluate(const MDTM * ref, const MDTM *  hyp, map<string,map<int, SdResult *> > & result, int delta, const UEM * uem, bool cross)
{
  if(!cross) {
    const list<UEM::Range> * range_uem = 0;
    for(map<string,map<int, Speakers *> >::const_iterator r_it = ref->spks.begin(); r_it != ref->spks.end(); r_it++) {
      //    fprintf(stdout,"\nFichier : %s\n",to_utf8(r_it->first).c_str());
      map<string,map<int, Speakers *> >::const_iterator h_it = hyp->spks.find(r_it->first);
      
      for(map<int, Speakers*>::const_iterator r_itc = r_it->second.begin(); r_itc != r_it->second.end(); r_itc++) {
	map<int, Speakers*>::const_iterator h_itc = h_it->second.find(r_itc->first);
	if(uem != 0){
	  range_uem = uem->find_range(r_it->first,r_itc->first);
	}
	result[r_it->first][r_itc->first] = new SdResult(r_itc->second, h_itc->second, delta,range_uem);
	
      }
    }
  } else {
    result["CrossFile"][0] = new SdResult(ref, hyp, delta, uem);

  }
}





void compute_global_ser(map<string,map<int, SdResult *> > & result)
{
  double fat = 0;  double misst = 0;  double conft =0; double confus_reft = 0;  double confus_hypt = 0;  double totaltimet = 0;
  for(map<string,map<int, SdResult *> >::const_iterator it = result.begin(); it != result.end(); it++) {
    for(map<int, SdResult *>::const_iterator i = it->second.begin(); i != it->second.end(); i++) {
      fat += i->second->fa;
      misst += i->second->miss;
      conft += i->second->conf;
      confus_reft += i->second->confus_ref;
      confus_hypt += i->second->confus_hyp;
      totaltimet += i->second->totaltime;
    }
  }
  double sert = (fat + misst + conft ) / totaltimet;
  
  fprintf(stdout,"The global SER is : %f\n",sert);
  
}
void print_uem_range(const UEM * uem)
{
  printf("Enter to uem\n");
  for(map<string, map<int, UEM::rlist> >::const_iterator i = uem->ranges.begin(); i != uem->ranges.end(); i++) {
    fprintf(stdout,"For the file %s\n",to_utf8(i->first).c_str());
    for(map<int, UEM::rlist>::const_iterator j = i->second.begin(); j != i->second.end(); j++) {
      fprintf(stdout," channel %d the range are:\n ",j->first);
      for(list<UEM::Range>::const_iterator w = j->second.begin(); w != j->second.end(); w++) {
	fprintf(stdout,"%d %d\n",w->stime,w->etime);
      }
    }
  } 
	
}
