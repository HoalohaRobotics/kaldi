#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "tree/context-dep.h"
#include "hmm/transition-model.h"
#include "fstext/fstext-lib.h"
#include "decoder/decoder-wrappers.h"
#include "nnet3/nnet-am-decodable-simple.h"
#include "nnet3/nnet-utils.h"
#include "base/timer.h"

using namespace kaldi;

template <typename FST>
bool Decode(
    LatticeFasterDecoderTpl<FST>& decoder, // not const but is really an input.
    DecodableInterface& decodable, // not const but is really an input.
    const TransitionInformation& trans_model,
    const fst::SymbolTable* word_syms,
    double acoustic_scale,
    bool determinize,
    bool allow_partial,
    std::string& transcription,
    Lattice* lattice,
    double& likelihood) {

    using fst::VectorFst;

    if (!decoder.Decode(&decodable)) {
        KALDI_WARN << "Failed to decode utterance";
        return false;
    }
    if (!decoder.ReachedFinal()) {
        if (allow_partial) {
            KALDI_WARN << "Outputting partial output for utterance since no final-state reached\n";
        }
        else {
            KALDI_WARN << "Not producing output for utterance since no final-state reached and "
                << "--allow-partial=false.\n";
            return false;
        }
    }

    LatticeWeight weight;
    int32 num_frames;
    // First do some stuff with word-level traceback...
    VectorFst<LatticeArc> decoded;
    if (!decoder.GetBestPath(&decoded))
        // Shouldn't really reach this point as already checked success.
        KALDI_ERR << "Failed to get traceback for utterance";

    std::vector<int32> alignment;
    std::vector<int32> words;
    GetLinearSymbolSequence(decoded, &alignment, &words, &weight);
    num_frames = alignment.size();


    for (size_t i = 0; i < words.size(); i++) {
        std::string s = word_syms->Find(words[i]);
        if (s == "")
            KALDI_ERR << "Word-id " << words[i] << " not in symbol table.";
        transcription += s + ' ';
    }

    likelihood = -(weight.Value1() + weight.Value2());

    decoder.GetRawLattice(lattice);
    if (lattice->NumStates() == 0)
        KALDI_ERR << "Unexpected problem getting lattice for utterance ";
    fst::Connect(lattice);

    if (acoustic_scale != 0.0)
        fst::ScaleLattice(fst::AcousticLatticeScale(1.0 / acoustic_scale), lattice);

    KALDI_LOG << "Log-like per frame for utterance is "
        << (likelihood / num_frames) << " over "
        << num_frames << " frames.";
    KALDI_VLOG(2) << "Cost for utterance is "
        << weight.Value1() << " + " << weight.Value2();

    return true;
}