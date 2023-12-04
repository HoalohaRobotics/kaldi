#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "fstext/fstext-lib.h"
#include "lat/kaldi-lattice.h"

using namespace kaldi;
int LatticeScale(Lattice* plattice) {
    try {
        bool write_compact = true;
        BaseFloat acoustic_scale = 10.0;
        BaseFloat inv_acoustic_scale = 1.0;
        BaseFloat lm_scale = 1.0;
        BaseFloat acoustic2lm_scale = 0.0;
        BaseFloat lm2acoustic_scale = 0.0;

        KALDI_ASSERT(acoustic_scale == 1.0 || inv_acoustic_scale == 1.0);
        if (inv_acoustic_scale != 1.0)
            acoustic_scale = 1.0 / inv_acoustic_scale;

        std::vector<std::vector<double> > scale(2);
        scale[0].resize(2);
        scale[1].resize(2);
        scale[0][0] = lm_scale;
        scale[0][1] = acoustic2lm_scale;
        scale[1][0] = lm2acoustic_scale;
        scale[1][1] = acoustic_scale;

        ScaleLattice(scale, plattice);

        KALDI_LOG << "Done Scaling lattice";
        return (0);
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return -1;
    }
}
