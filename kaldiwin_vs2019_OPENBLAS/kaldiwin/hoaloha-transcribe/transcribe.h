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

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "NNet3LatgenFaster.h"
#include "base/timer.h"
#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace hoaloha
{
    int Init(ModelAndDecoder* m, CHAR* model_path);
    int TranscribeFile(ModelAndDecoder* m, string wav_file_path, string& transcription, Lattice* lattice, double& likelihood);
    int TranscribeWaveData(ModelAndDecoder* m, short* data, int samp_freq, int sample_count, bool subtract_mean, string& transcription, Lattice* lattice, double& likelihood);
    int TranscribeFeats(ModelAndDecoder* m, Matrix<BaseFloat>& feats, string& transcription, Lattice* lattice, double& likelihood);
    void Cleanup(ModelAndDecoder* m);

    int Init(ModelAndDecoder* m, CHAR* model_path) {
        
        // Read Configs
        std::cout << "HOALOHA: Loading Model From " << model_path << "\n";

        fs::path root{ model_path };

        fs::path mfcc_config_file = root / "conf/mfcc_hires.conf" ;
        fs::path extractor_config_file = root / "conf/ivector_extractor.conf";
        fs::path decoder_config_file = root / "conf/lattice_faster_decoder.conf";
        fs::path nnet3_simple_computation_opts_file = root / "conf/nnet_simple_computation.conf";

        ParseOptions po("");
        m->mfcc_opts.Register(&po);
        po.ReadConfigFile(mfcc_config_file.string());

        ParseOptions po2("");
        m->extractor_opts.Register(&po2);
        po2.ReadConfigFile(extractor_config_file.string());

        //fix paths in this config to include root
        fs::path fixed_path = root / m->extractor_opts.cmvn_config_rxfilename;
        m->extractor_opts.cmvn_config_rxfilename = fixed_path.string();
        fixed_path = root / m->extractor_opts.diag_ubm_rxfilename;
        m->extractor_opts.diag_ubm_rxfilename = fixed_path.string();
        fixed_path = root / m->extractor_opts.global_cmvn_stats_rxfilename;
        m->extractor_opts.global_cmvn_stats_rxfilename = fixed_path.string();
        fixed_path = root / m->extractor_opts.ivector_extractor_rxfilename;
        m->extractor_opts.ivector_extractor_rxfilename = fixed_path.string();
        fixed_path = root / m->extractor_opts.lda_mat_rxfilename;
        m->extractor_opts.lda_mat_rxfilename = fixed_path.string();
        fixed_path = root / m->extractor_opts.splice_config_rxfilename;
        m->extractor_opts.splice_config_rxfilename = fixed_path.string();

        ParseOptions po3("");
        m->decoder_config.Register(&po3);
        m->nnet3_simple_computation_opts.Register(&po3);
        po3.ReadConfigFile(decoder_config_file.string());
        po3.ReadConfigFile(nnet3_simple_computation_opts_file.string());


        fs::path word_syms_filename = root / "exp/chain_cleaned/tdnn_1d_sp/graph_tgsmall/words.txt";
        fs::path model_filename = root / "exp/chain_cleaned/tdnn_1d_sp/final.mdl";
        fs::path decode_graph_filename = root / "exp/chain_cleaned/tdnn_1d_sp/graph_tgsmall/HCLG.fst";
        return LoadModelAndDecoder(
            m,
            decode_graph_filename.string(),
            model_filename.string(),
            word_syms_filename.string(),
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

    int TranscribeWaveData(ModelAndDecoder* m, short* data, int samp_freq, int sample_count, bool subtract_mean, string& transcription, Lattice* lattice, double& likelihood)
    {
        Timer timer;
        Matrix<BaseFloat> feats;
        float* data_floats = (float*)malloc(sizeof(float) * sample_count);

        for (int i = 0; i < sample_count; i++)
        {
            data_floats[i] = data[i];
        }
        SubVector<float> waveform(data_floats, sample_count);

        if (ComputeMFCCFeatsFromWavform(m->mfcc_opts, waveform, samp_freq, subtract_mean, &feats) != 0) {
            std::cout << "Failed to compute MFCC Features.\n";
            return(-1);
        }

        free(data_floats);

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
