// SdResultat.h
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



#ifndef _SDRESULT_H_
#define _SDRESULT_H_
#include "Speaker.h"
#include <list>
#include "MDTM.h"
#include "UEM.h"
#include "CrossFile.h"

class SdResult {
public:

  
  double purity_ref1;
  double purity_hyp1;
  double purity_ref2;
  double purity_hyp2;
  double purity_ref3;
  double purity_hyp3;
  double purity_ref4;
  double purity_hyp4;
  vector<int> mapping_purity_hr;
  vector<int> mapping_purity_rh;
  vector<int> mapping_ser_rh;
  vector<int> mapping_ser_hr;
  vector<int> speech_time_ref;
  vector<int> speech_time_hyp;
  vector<int> miss_hyp;
  vector<int> miss_ref;
  int fa;
  int miss;
  int conf;
  int confus_ref;
  int confus_hyp;
  int totaltime;
  int correct_time;
  double ser;
  map<string, string> rh;
  map<string, list<pair<string, int> > > rtimes;

  int *confusion_matrix, *correct_ref, *correct_hyp, *lost_ref, *lost_hyp;
  vector<string> ref_names, hyp_names;

  SdResult(Speakers *ref, Speakers *hyp, int delta, const UEM::rlist *uem = NULL);
  SdResult(const MDTM * , const MDTM * , int, const UEM *);
  ~SdResult();

  void print_ser(Speakers * , Speakers * );
  void print_ser(CrossFile * , CrossFile * );
  void print_purity();

private:
  struct cross_time_info {
    int common, erreur_ref, erreur_hyp, fa, miss, conf, delta_error;
  };

  struct frontier {
    enum {
      NEGATIF, TRANSITION, POSITIF,
      PP, PN, NN, NP,
      PPP, PPN, PPT, PNN, PNT, NNN, NNT, NPP, NPN, NPT
    };

    int mode; // Mode of the region up to time
    int time; // Time in ms

    frontier(int _mode, int _time){
      mode = _mode;
      time = _time;
    }
  }; 

  typedef list<frontier> flist;

  void computation_time(const flist & hyp, const flist & fro, int & erreur_hyp, int & erreur_ref, int & common, int &, int &, int &);

  void build_flist_from_turns(const vector<Turn *> &spt, flist &fl);
  void build_flist_from_all_speakers(const Speakers *spk, flist &fo);
  void filter_flist_on_uem(const flist &fi, const list<UEM::Range> *uem, flist &fo);
  void flist_add_transitions(const flist &fi, int delta, flist &fo);
  void flist_build_3way_merge(const flist &fl_hyp_all, const flist &fl_ref_all, const vector<flist> &fl_ref, vector<flist> &fl_3way);

  void compute_mapping_error_times(const vector<flist> &fl_3way, const vector<flist> &fl_hyp, cross_time_info *cti);

  void compute_mapping_error_times(const vector<flist> &fl_3way, const vector<flist> &fl_hyp, cross_time_info *cti,
				   const int *mapper_r, const int *mapper_h, int sh,
				   Speakers *ref, Speakers *hyp, CrossFile *cref, CrossFile *chyp, flist fl_hyp_all, flist fl_ref_all);
  void accumulate_mapping_error_times(const flist &fl_hyp, const flist &fl_3way, cross_time_info &cti);
  void accumulate_mapping_error_time(int thyp, int thyn, int mode, cross_time_info &cti);

  int compute_single_speaker_time(const flist &fl);
  void compute_per_speaker_time(const vector<flist> &fl, vector<int> &speech_time);
  void accumulate_per_speaker_time(const vector<flist> &fl, vector<int> &speech_time, const int *mapping);

  int flist_compute_missing_time(const flist &hyp, const flist &opposite_all);

  void compute_miss(const flist &opposite_all, const vector<flist> &fl, vector<int> &result);
  void compute_miss(const flist &opposite_all, const vector<flist> &fl, vector<int> &result, const int *mapping, int s);

  void compute_delta_error(const vector<int> &speech_time_ref, const vector<int> &miss_hyp, cross_time_info *cti);

  void ser_extract_mapping(const Speakers *ref, const Speakers *hyp, const cross_time_info *cti);
  void ser_extract_mapping(const CrossFile *ref, const CrossFile *hyp, const cross_time_info *cti);

  void map_ser(const cross_time_info *cti, int nr, int nh);
  void compute_ser(const cross_time_info *cti);
  void map_purity(const cross_time_info *cti, int nr, int nh);
  void compute_purity(const cross_time_info *cti, int nr, int nh);

  void mix_references_for_scoring(const vector<flist> &fl_ref, const vector<flist> &fl_ref_trans, vector<flist> &fl_ref_mixed, const int *mapper_r = NULL, const int *mapper_h = NULL);
  void accumulate_confusion(const vector<flist> &fl_ref, const vector<flist> &fl_hyp, const int *mapper_r = NULL, const int *mapper_h = NULL, int sr = 0, int sh = 0);

  static int merge_mode_hyp_ref_all(int mode_hyp, int mode_ref);
  static int merge_mode_hr_all_ref(int mode_hr, int mode_ref);
  void flist_merge(const flist &f1, const flist &f2, int m1_post, int m2_post, int (*merge)(int, int), flist &result);
};

void sd_evaluate(const MDTM * ref, const MDTM *  hyp, map<string,map<int, SdResult *> > & result, int delta, const UEM *, bool);
void compute_global_ser(map<string,map<int, SdResult *> > & result);
void print_uem_range(const UEM * uem);

#endif
