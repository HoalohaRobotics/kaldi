// Copyright 2009-2012  Microsoft Corporation
//                      Johns Hopkins University (author: Daniel Povey)

// See ../../COPYING for clarification regarding multiple authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.


// TODO: Figure out how to build the model(s) in windows (instead of copy from WSL)

#include "base\kaldi-common.h"
#include "util\common-utils.h"
#include "feat\feature-common.h"
#include "feat\feature-mfcc.h"
#include "decoder\decoder-wrappers.h"
#include "nnet3\nnet-am-decodable-simple.h"
#include "online2/online-ivector-feature.h"
#include "NNet3LatgenFaster.h"


using namespace kaldi;

int ComputeMFCCFeats(MfccOptions mfcc_opts, std::string wav_file_path, Matrix<BaseFloat>* pfeats);
int ComputeCMVNStats(Matrix<BaseFloat>& feats, Matrix<double>* pstats);
int IVectorExtractorOnline2(Matrix<BaseFloat>& feats, OnlineIvectorExtractionConfig ivector_config, Matrix<BaseFloat>* pivectors);
//int LatticeScale(Lattice* lattice);
//int Lattice1Best(int argc, char* argv[]);
//int LatticeAlignWords(int argc, char* argv[]);
//int NBestToCTM(int argc, char* argv[]);

int main(int argc, char* argv[]) {

    // robbie wav filename often has spaces and to handle double-click on file need to do this...
    std::string wav_file_path = "";
    for (int i = 1; i < argc; i++) {
        wav_file_path += argv[i];
        wav_file_path += " ";
    }

    // Read Configs
    MfccOptions mfcc_opts;
    const char* mfcc_config_file = "conf/mfcc_hires.conf";
    ParseOptions po("");
    mfcc_opts.Register(&po);
    po.ReadConfigFile(mfcc_config_file);
    
    OnlineIvectorExtractionConfig extractor_opts;
    const char* extractor_config_file = "conf/ivector_extractor.conf";
    ParseOptions po2("");
    extractor_opts.Register(&po2);
    po2.ReadConfigFile(extractor_config_file);

    LatticeFasterDecoderConfig decoder_config;
    nnet3::NnetSimpleComputationOptions nnet3_simple_computation_opts;
    const char* decoder_config_file = "conf/lattice_faster_decoder.conf";
    const char* nnet3_simple_computation_opts_file = "conf/nnet_simple_computation.conf";
    ParseOptions po3("");
    decoder_config.Register(&po3);
    nnet3_simple_computation_opts.Register(&po3);
    po3.ReadConfigFile(decoder_config_file);
    po3.ReadConfigFile(nnet3_simple_computation_opts_file);

    Lattice lattice;
    std::string transcription = "";
    double likelihood = 0;
    Matrix<BaseFloat> feats;
    Matrix<float> iVectors;
    //Matrix<double> stats;

    std::string word_syms_filename = "exp/chain_cleaned/tdnn_1d_sp/graph_tgsmall/words.txt";
    std::string model_filename = "exp/chain_cleaned/tdnn_1d_sp/final.mdl";
    std::string decode_graph_filename = "exp/chain_cleaned/tdnn_1d_sp/graph_tgsmall/HCLG.fst";
    
    ModelAndDecoder m;
    LoadModelAndDecoder(
        m, 
        decode_graph_filename, 
        model_filename, 
        word_syms_filename, 
        nnet3_simple_computation_opts, 
        decoder_config);


    while (true)
    {
        transcription = "";
        std::cout << "Wav File Path: ";

        std::getline(std::cin >> std::ws, wav_file_path);
        wav_file_path.erase(remove(wav_file_path.begin(), wav_file_path.end(), '\"'), wav_file_path.end());

        if (ComputeMFCCFeats(mfcc_opts, wav_file_path, &feats) != 0) {
            std::cout << "Failed to compute MFCC Features.\n";
            continue;
        }

        //if (ComputeCMVNStats(feats, &stats) != 0) {
        //    std::cout << "Failed to compute CMVN Stats\n";
        //    continue;
        //}

        if (IVectorExtractorOnline2(feats, extractor_opts, &iVectors) != 0) {
            std::cout << "Failed to extract iVectors\n";
            continue;
        }

        if (NNet3LatgenFaster(m, feats, iVectors, transcription, &lattice, likelihood) != 0)
        {
            std::cout << "Failed to Generate Lattice (Decode)\n";
            continue;
        }
        std::cout << transcription + '\n';

        std::ofstream out("transcription.txt");
        out << transcription;
        out.close();
    }
    
    return 0;

    //// lattice scale (is this already done in NNet3LatgenFaster?)
    //if (LatticeScale(&lattice) != 0) {
    //    std::cout << "Failed to Scale Lattice";
    //    return -1;
    //}

    ////lattice-1best (is this already done in NNet3LatgenFaster?)
    //if (Lattice1Best(0, NULL) != 0) {
    //    std::cout << "Failed to Find Lattice1Best";
    //    return -1;
    //}

    ////lattice-align-words (is this already done in NNet3LatgenFaster?)
    //if (LatticeAlignWords(0, NULL) != 0) {
    //    std::cout << "Failed to Lattice Align Words";
    //    return -1;
    //}

    ////nbest-to-ctm (Need timing info?)
    //if (NBestToCTM(0, NULL) != 0) {
    //    std::cout << "Failed to Convert Best to CTM";
    //    return -1;
    //}
}


