#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "matrix/kaldi-matrix.h"
#include "transform/cmvn.h"

using namespace kaldi;
int ComputeCMVNStats(Matrix<BaseFloat> &feats, Matrix<double>* pstats) {
    try {
        InitCmvnStats(feats.NumCols(), pstats);
        AccCmvnStats(feats, NULL, pstats);

        KALDI_LOG << "Done accumulating CMVN stats.";
        return (0);
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return -1;
    }
}
#pragma once
