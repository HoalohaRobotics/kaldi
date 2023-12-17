#pragma once

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


#include "NNet3LatgenFaster.h"
#include "base/timer.h"

namespace hoaloha
{
    int Init(ModelAndDecoder* m);
    int Transcribe(ModelAndDecoder* m, string wav_file_path, string& transcription, Lattice* lattice, double& likelihood);
    void Cleanup(ModelAndDecoder* m);

    int Init(ModelAndDecoder* m) {
        // Read Configs
        const char* mfcc_config_file = "conf/mfcc_hires.conf";
        const char* extractor_config_file = "conf/ivector_extractor.conf";
        const char* decoder_config_file = "conf/lattice_faster_decoder.conf";
        const char* nnet3_simple_computation_opts_file = "conf/nnet_simple_computation.conf";
        ParseOptions po("");
        m->mfcc_opts.Register(&po);
        po.ReadConfigFile(mfcc_config_file);

        ParseOptions po2("");
        m->extractor_opts.Register(&po2);
        po2.ReadConfigFile(extractor_config_file);

         ParseOptions po3("");
        m->decoder_config.Register(&po3);
        m->nnet3_simple_computation_opts.Register(&po3);
        po3.ReadConfigFile(decoder_config_file);
        po3.ReadConfigFile(nnet3_simple_computation_opts_file);


        std::string word_syms_filename = "exp/chain_cleaned/tdnn_1d_sp/graph_tgsmall/words.txt";
        std::string model_filename = "exp/chain_cleaned/tdnn_1d_sp/final.mdl";
        std::string decode_graph_filename = "exp/chain_cleaned/tdnn_1d_sp/graph_tgsmall/HCLG.fst";
        return LoadModelAndDecoder(
            m,
            decode_graph_filename,
            model_filename,
            word_syms_filename,
            m->nnet3_simple_computation_opts,
            m->decoder_config);
    }

    int Transcribe(ModelAndDecoder* m, string wav_file_path, string& transcription, Lattice* lattice, double& likelihood)
    {
        kaldi::Timer timer1;
        kaldi::Timer timer2;
        Matrix<BaseFloat> feats;
        Matrix<float> iVectors;
        //Matrix<double> stats;


        timer1.Reset();
        timer2.Reset();

        if (ComputeMFCCFeatsFromFile(m->mfcc_opts, wav_file_path, &feats) != 0) {
            std::cout << "Failed to compute MFCC Features.\n";
            return(-1);
        }
        std::cout << "HOALOHA: MFCC time: " << timer1.Elapsed() << "\n";
        timer1.Reset();

        //if (ComputeCMVNStats(feats, &stats) != 0) {
        //    std::cout << "Failed to compute CMVN Stats\n";
        //    continue;
        //}

        if (IVectorExtractorOnline2(feats, m->extractor_opts, &iVectors) != 0) {
            std::cout << "Failed to extract iVectors\n";
            return(-1);
        }
        std::cout << "HOALOHA: IVector time: " << timer1.Elapsed() << "\n";
        timer1.Reset();

        if (NNet3LatgenFaster(m, feats, iVectors, transcription, lattice, likelihood) != 0)
        {
            std::cout << "Failed to Generate Lattice (Decode)\n";
            return(-1);
        }
        std::cout << "HOALOHA: Decode time: " << timer1.Elapsed() << "\n";
        timer1.Reset();

        std::cout << "HOALOHA: Total time: " << timer2.Elapsed() << "\n";
        timer2.Reset();

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

        return 0;
    }

    void Cleanup(ModelAndDecoder* m)
    {
        UnloadModelAndDecoder(m);
    }
}
