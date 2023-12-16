#pragma once

#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "tree/context-dep.h"
#include "hmm/transition-model.h"
#include "fstext/fstext-lib.h"
#include "decoder/decoder-wrappers.h"
#include "nnet3/nnet-am-decodable-simple.h"
#include "nnet3/nnet-utils.h"
#include "base/timer.h"
#include "DecodeFaster.h"

using namespace kaldi;
using namespace kaldi::nnet3;
using namespace fst;

typedef struct ModelAndDecoder
{
    AmNnetSimple am_nnet;
    TransitionModel trans_model;
    Fst<StdArc>* decode_fst;
    LatticeFasterDecoder* decoder;
    fst::SymbolTable* word_syms;
    nnet3::NnetSimpleComputationOptions decodable_opts;
    LatticeFasterDecoderConfig config;
};

int LoadModelAndDecoder(ModelAndDecoder& m, string decoder_graph_filename, string model_filename, string word_syms_filename, nnet3::NnetSimpleComputationOptions decodable_opts, LatticeFasterDecoderConfig config);
void UnloadModelAndDecoder(ModelAndDecoder* pModelAndDecoder);

int NNet3LatgenFaster(string& decode_graph_filename, string& model_filename, string& word_syms_filename, Matrix<BaseFloat>& feats, Matrix<BaseFloat>& ivectors, LatticeFasterDecoderConfig config, nnet3::NnetSimpleComputationOptions decodable_opts, std::string& transcription, Lattice* plattice, double& likelihood);
int NNet3LatgenFaster(ModelAndDecoder& modelAndDecoder, Matrix<BaseFloat>& feats, Matrix<BaseFloat>& ivectors, std::string& transcription, Lattice* plattice, double& likelihood);
