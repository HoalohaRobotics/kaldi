#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "fstext/fstext-lib.h"
#include "lat/kaldi-lattice.h"
#include "lat/lattice-functions.h"

using namespace kaldi;
int Lattice1Best(CompactLattice* pclattice) {
    try {
        using namespace kaldi;
        typedef kaldi::int32 int32;
        typedef kaldi::int64 int64;
        using fst::VectorFst;
        using fst::StdArc;

        BaseFloat acoustic_scale = 1.0;
        BaseFloat lm_scale = 5.0;
        BaseFloat word_ins_penalty = 0.0;

        if (acoustic_scale == 0.0 || lm_scale == 0.0)
            KALDI_ERR << "Do not use exactly zero acoustic or LM scale (cannot be inverted)";


        CompactLattice best_path;
        CompactLatticeShortestPath(*pclattice, &best_path);

        fst::ScaleLattice(fst::LatticeScale(1.0 / lm_scale, 1.0 / acoustic_scale), &best_path);

        if (best_path.Start() == fst::kNoStateId) {
            KALDI_WARN << "Possibly empty lattice for utterance (no output)";
        }
        else {
            if (word_ins_penalty > 0.0) {
                AddWordInsPenToCompactLattice(-word_ins_penalty, &best_path);
            }
            fst::ScaleLattice(fst::LatticeScale(1.0 / lm_scale, 1.0 / acoustic_scale),
                &best_path);
        }

        KALDI_LOG << "Done converting to best path ";
        return(0);
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return -1;
    }
}
