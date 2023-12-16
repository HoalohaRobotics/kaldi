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
#include "NNet3LatgenFaster.h"

using namespace kaldi;
using namespace kaldi::nnet3; 
using namespace fst;

// Callers of this are responsible for calling UnloadModelAndDecoder to prevent memory leak.
int LoadModelAndDecoder(
    ModelAndDecoder& m,
    string decoder_graph_filename, 
    string model_filename, 
    string word_syms_filename, 
    nnet3::NnetSimpleComputationOptions decodable_opts,
    LatticeFasterDecoderConfig config) 
{
    bool binary;
    Input ki(model_filename, &binary);
    m.trans_model.Read(ki.Stream(), binary);
    m.am_nnet.Read(ki.Stream(), binary);
    SetBatchnormTestMode(true, &(m.am_nnet.GetNnet()));
    SetDropoutTestMode(true, &(m.am_nnet.GetNnet()));
    CollapseModel(CollapseModelConfig(), &(m.am_nnet.GetNnet()));

    m.decodable_opts = decodable_opts;
    m.config = config;

    CachingOptimizingCompiler compiler(m.am_nnet.GetNnet(),
        decodable_opts.optimize_config);

    m.decode_fst = fst::ReadFstKaldiGeneric(decoder_graph_filename);

    //LatticeFasterDecoder decoder(decode_fst, config);
    m.decoder = new LatticeFasterDecoder(*m.decode_fst, m.config);

    m.word_syms = fst::SymbolTable::ReadText(word_syms_filename);

    return 0;
}

void UnloadModelAndDecoder(ModelAndDecoder* pModelAndDecoder)
{
    delete pModelAndDecoder->decoder;
    delete pModelAndDecoder->decode_fst;
    delete pModelAndDecoder->word_syms;
}

int NNet3LatgenFaster(
    string& decode_graph_filename,
    string& model_filename,
    string& word_syms_filename,
    Matrix<BaseFloat>& feats,
    Matrix<BaseFloat>& ivectors,
    LatticeFasterDecoderConfig config,
    nnet3::NnetSimpleComputationOptions decodable_opts,
    std::string& transcription,
    Lattice* plattice,
    double& likelihood)
{
    ModelAndDecoder m;
    LoadModelAndDecoder(m, decode_graph_filename, model_filename, word_syms_filename, decodable_opts, config);
    NNet3LatgenFaster(m, feats, ivectors, transcription, plattice, likelihood);

    return 0;
}

int NNet3LatgenFaster(
    ModelAndDecoder& modelAndDecoder,
    Matrix<BaseFloat>& feats,
    Matrix<BaseFloat>& ivectors,
    std::string& transcription,
    Lattice* plattice,
    double& likelihood)
{
    try {
        typedef kaldi::int32 int32;
        using fst::SymbolTable;
        using fst::Fst;
        using fst::StdArc;

        Timer timer;
        bool allow_partial = false;
        int32 online_ivector_period = 10;  //TODO: Shouldnt this be in a config?
        double tot_like = 0.0;
        kaldi::int64 frame_count = 0;

        DecodableAmNnetSimple nnet_decodable(
            modelAndDecoder.decodable_opts, modelAndDecoder.trans_model, modelAndDecoder.am_nnet,
            feats, NULL, &ivectors, online_ivector_period);

        bool determinize = false;
        Lattice lattice;
        Decode(*modelAndDecoder.decoder, nnet_decodable, modelAndDecoder.trans_model, modelAndDecoder.word_syms, modelAndDecoder.decodable_opts.acoustic_scale, determinize, allow_partial, transcription, &lattice, likelihood);

        kaldi::int64 input_frame_count =
            frame_count * modelAndDecoder.decodable_opts.frame_subsampling_factor;

        double elapsed = timer.Elapsed();
        KALDI_LOG << "Time taken " << elapsed
            << "s: real-time factor assuming 100 frames/sec is "
            << (elapsed * 100.0 / input_frame_count);
        KALDI_LOG << "Done ";
        KALDI_LOG << "Overall log-likelihood per frame is "
            << (tot_like / frame_count) << " over "
            << frame_count << " frames.";

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return -1;
    }
}