#pragma once

#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "tree/context-dep.h"
#include "hmm/transition-model.h"
#include "feat\feature-mfcc.h"
#include "online2/online-ivector-feature.h"
#include "fstext/fstext-lib.h"
#include "decoder/decoder-wrappers.h"
#include "nnet3/nnet-am-decodable-simple.h"
#include "nnet3/nnet-utils.h"
#include "base/timer.h"
#include "feat/wave-reader.h"
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
    MfccOptions mfcc_opts;
    OnlineIvectorExtractionConfig extractor_opts;
    nnet3::NnetSimpleComputationOptions decodable_opts;
    nnet3::NnetSimpleComputationOptions nnet3_simple_computation_opts;
    LatticeFasterDecoderConfig decoder_config;
    Lattice* lattice; //this if for storing the last output lattice
};

int LoadModelAndDecoder(ModelAndDecoder* m, string decoder_graph_filename, string model_filename, string word_syms_filename, nnet3::NnetSimpleComputationOptions decodable_opts, LatticeFasterDecoderConfig decoder_config);
void UnloadModelAndDecoder(ModelAndDecoder* m);
int NNet3LatgenFaster(ModelAndDecoder* m, Matrix<BaseFloat>& feats, Matrix<BaseFloat>& ivectors, std::string& transcription, double& likelihood);
int ComputeMFCCFeatsFromFile(MfccOptions mfcc_opts, std::string wav_file_path, Matrix<BaseFloat>* pfeats);
int ComputeMFCCFeatsFromWavData(MfccOptions mfcc_opts, WaveData wave_data, Matrix<BaseFloat>* pfeats);
int ComputeMFCCFeatsFromWavform(MfccOptions mfcc_opts, SubVector<BaseFloat> waveform, BaseFloat samp_freq, bool subtract_mean, Matrix<BaseFloat>* pfeatures);
int ComputeCMVNStats(Matrix<BaseFloat>& feats, Matrix<double>* pstats);
int IVectorExtractorOnline2(Matrix<BaseFloat>& feats, OnlineIvectorExtractionConfig ivector_config, Matrix<BaseFloat>* pivectors);