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
    int TranscribeFile(ModelAndDecoder* m, string wav_file_path, string& transcription, Lattice* lattice, double& likelihood);
    int TranscribeWavData(ModelAndDecoder* m, WaveData& waveData, string& transcription, Lattice* lattice, double& likelihood);
    int TranscribeFeats(ModelAndDecoder* m, Matrix<BaseFloat>& feats, string& transcription, Lattice* lattice, double& likelihood);
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

    int TranscribeFile(ModelAndDecoder* m, string wav_file_path, string& transcription, Lattice* lattice, double& likelihood)
    {
        Timer timer;
        Matrix<BaseFloat> feats;

        if (ComputeMFCCFeatsFromFile(m->mfcc_opts, wav_file_path, &feats) != 0) {
            std::cout << "Failed to compute MFCC Features.\n";
            return(-1);
        }
        int ret = TranscribeFeats(m, feats, transcription, lattice, likelihood);
        std::cout << "HOALOHA: transcibe time: " << timer.Elapsed() << "\n";
        return ret;
    }

    int TranscribeWavData(ModelAndDecoder* m, WaveData& waveData, string& transcription, Lattice* lattice, double& likelihood)
    {
        Timer timer;
        Matrix<BaseFloat> feats;

        if (ComputeMFCCFeatsFromWavData(m->mfcc_opts, waveData, &feats) != 0) {
            std::cout << "Failed to compute MFCC Features.\n";
            return(-1);
        }

        int ret = TranscribeFeats(m, feats, transcription, lattice, likelihood);
        std::cout << "HOALOHA: transcribe time: " << timer.Elapsed() << "\n";
        return ret;
    }

    int TranscribeFeats(ModelAndDecoder* m, Matrix<BaseFloat>& feats, string& transcription, Lattice* lattice, double& likelihood)
    {
        Matrix<float> iVectors;

        if (IVectorExtractorOnline2(feats, m->extractor_opts, &iVectors) != 0) {
            std::cout << "Failed to extract iVectors\n";
            return(-1);
        }

        if (NNet3LatgenFaster(m, feats, iVectors, transcription, lattice, likelihood) != 0)
        {
            std::cout << "Failed to Generate Lattice (Decode)\n";
            return(-1);
        }
        return 0;
    }

    void Cleanup(ModelAndDecoder* m)
    {
        UnloadModelAndDecoder(m);
    }
}
