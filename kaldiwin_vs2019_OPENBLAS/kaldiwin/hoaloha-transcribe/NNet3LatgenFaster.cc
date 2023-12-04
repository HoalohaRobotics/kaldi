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

int NNet3LatgenFaster(
    Matrix<BaseFloat>& feats,
    Matrix<BaseFloat>& ivectors,
    LatticeFasterDecoderConfig config,
    nnet3::NnetSimpleComputationOptions decodable_opts,
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

        std::string word_syms_filename = "exp/chain_cleaned/tdnn_1d_sp/graph_tgsmall/words.txt";
        int32 online_ivector_period = 10;

        std::string model_in_filename = "exp/chain_cleaned/tdnn_1d_sp/final.mdl";
        std::string fst_in_str = "exp/chain_cleaned/tdnn_1d_sp/graph_tgsmall/HCLG.fst";
        TransitionModel trans_model;

        AmNnetSimple am_nnet;
        {
            bool binary;
            Input ki(model_in_filename, &binary);
            trans_model.Read(ki.Stream(), binary);
            am_nnet.Read(ki.Stream(), binary);
            SetBatchnormTestMode(true, &(am_nnet.GetNnet()));
            SetDropoutTestMode(true, &(am_nnet.GetNnet()));
            CollapseModel(CollapseModelConfig(), &(am_nnet.GetNnet()));
        }

        fst::SymbolTable* word_syms = NULL;
        if (word_syms_filename != "") {
            if (!(word_syms = fst::SymbolTable::ReadText(word_syms_filename))) {
                KALDI_ERR << "Could not read symbol table from file " << word_syms_filename;
                return(-1);
            }
        }

        double tot_like = 0.0;
        kaldi::int64 frame_count = 0;
        // this compiler object allows caching of computations across
        // different utterances.
        CachingOptimizingCompiler compiler(am_nnet.GetNnet(),
            decodable_opts.optimize_config);

        Fst<StdArc>* decode_fst = fst::ReadFstKaldiGeneric(fst_in_str);
        timer.Reset();

        LatticeFasterDecoder decoder(*decode_fst, config);

        DecodableAmNnetSimple nnet_decodable(
            decodable_opts, trans_model, am_nnet,
            feats, NULL, &ivectors, online_ivector_period = online_ivector_period);

        bool determinize = false;
        Lattice lattice;
        Decode(decoder, nnet_decodable, trans_model, word_syms, decodable_opts.acoustic_scale, determinize, allow_partial, transcription, &lattice, likelihood);

        delete decode_fst; // delete this only after decoder goes out of scope.


        kaldi::int64 input_frame_count =
            frame_count * decodable_opts.frame_subsampling_factor;

        double elapsed = timer.Elapsed();
        KALDI_LOG << "Time taken " << elapsed
            << "s: real-time factor assuming 100 frames/sec is "
            << (elapsed * 100.0 / input_frame_count);
        KALDI_LOG << "Done ";
        KALDI_LOG << "Overall log-likelihood per frame is "
            << (tot_like / frame_count) << " over "
            << frame_count << " frames.";

        delete word_syms;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return -1;
    }
}