# SD-EVAL

The main metric that is used for speaker diarization experiments is the Diarization Error Rate (DER) as described and used by NIST in the RT evaluations. It is measured as the fraction of time that is not attributed correctly to a speaker or to non-speech. To measure it, a script names MD-eval, developed by NIST, was used.

Unfortunately, MD-eval tools were reaching their limits under two conditions :
* Cross-show conditions, where speakers have to be detected when recurring in different shows
* Overlapping speech, where multiple speakers speak simultaneously and must be both detected and transcribed

SD-eval tools propose to measure DER by tackle these limits. The problem is to find a mapping that minimizes the total error. This mapping is solved by using an Assignment Problem, which is solved deterministically in o(n3) time with the Hungarian Algorithm. Once the mapping established computing the DER score is calculated.

SD-eval is based on the evaluation source-code given by the LNE during REPERE evaluation.


# Evaluation

Single-show evaluation :

```bash
./sd-eval hypothesis.mdtm reference.mdtm
```

Cross-show evaluation :

```bash
./sd-eval -x hypothesis.mdtm reference.mdtm
```

