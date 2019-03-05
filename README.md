# SD-EVAL

The main metric that is used for speaker diarization experiments is the Diarization Error Rate (DER) as described and used by NIST in the RT evaluations. It is measured as the fraction of time that is not attributed correctly to a speaker or to non-speech. To measure it, a script names MD-eval, developed by NIST, was used.

Unfortunately, MD-eval tools were reaching their limits under two conditions :
* Cross-show conditions, where speakers have to be detected when recurring in different shows
* Overlapping speech, where multiple speakers speak simultaneously and must be both detected and transcribed

SD-eval tools propose to measure DER by tackle these limits. SD-eval is based on Hungarian algorithm which mapp the hypothesis segments with the reference segments. Once the mapping is established the DER score is calculated.

SD-eval is based on the evaluation source-code given by the LNE during REPERE evaluation.


# Evaluation

Single-show evaluation :

```bash
./sd-eval hypothesis.mdtm reference.mdtm
```

Single-show evaluation using an UEM file :

```bash
./sd-eval -u reference.uem hypothesis.mdtm reference.mdtm
```


Single-show evaluation using a 250ms collar :

```bash
./sd-eval -d 150 hypothesis.mdtm reference.mdtm
```

Cross-show evaluation :

```bash
./sd-eval -x hypothesis.mdtm reference.mdtm
```

Example output :

```bash
Diarization                                 fa.       %    miss.       %    conf.       %     DER
--------------------------------------  -------  ------  -------  ------  -------  ------  ------
BFMTV_BFMStory_2012-07-24_175800          14457  0.0043   159131  0.0472   321104  0.0952  0.1466
BFMTV_BFMStory_2012-07-26_175800           9701  0.0033   100775  0.0344   242603  0.0828  0.1204
BFMTV_BFMStory_2012-07-27_175800          22753  0.0084   121701  0.0447   173481  0.0638  0.1168
BFMTV_BFMStory_2012-08-24_175800           9656  0.0034   113111  0.0401   424978  0.1508  0.1944
BFMTV_BFMStory_2012-11-08_175800           8068  0.0091     8341  0.0094    90937  0.1020  0.1205
BFMTV_BFMStory_2012-11-09_175800           8533  0.0089    21876  0.0229   136768  0.1433  0.1752
BFMTV_BFMStory_2012-11-14_175800           3842  0.0039    64447  0.0660     4595  0.0047  0.0746
BFMTV_BFMStory_2012-11-15_175800           8897  0.0099     6471  0.0072    50114  0.0559  0.0731
BFMTV_CultureEtVous_2012-02-20_065040      1525  0.0127    19713  0.1640    28118  0.2340  0.4107
BFMTV_CultureEtVous_2012-07-23_065040       690  0.0073     7220  0.0767    28832  0.3063  0.3903
BFMTV_CultureEtVous_2012-10-05_064700      3873  0.0390    21241  0.2137    31207  0.3139  0.5666
BFMTV_CultureEtVous_2012-10-08_064700      2637  0.0306    21945  0.2550      630  0.0073  0.2930
--------------------------------------  -------  ------  -------  ------  -------  ------  ------
TOTAL                                    384045  0.0104  1679279  0.0454  2989641  0.0808  0.1366
```
