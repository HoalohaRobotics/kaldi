#include "base/kaldi-common.h"
#include "feat/feature-mfcc.h"
#include "feat/wave-reader.h"
#include "util/common-utils.h"

using namespace kaldi;

int ComputeMFCCFeatsFromWavData(MfccOptions mfcc_opts, WaveData wave_data, Matrix<BaseFloat>* pfeatures) {
    try {
        // Define defaults for global options.
        bool subtract_mean = false;
        BaseFloat vtln_warp = 1.0;
        std::string utt2spk_rspecifier;
        int32 channel = -1;
        BaseFloat min_duration = 0.0;
        Mfcc mfcc(mfcc_opts);

        if (wave_data.Duration() < min_duration) {
            KALDI_WARN << "wav data is too short ("
                << wave_data.Duration() << " sec): producing no output.";
            return(0);
        }

        int32 num_chan = wave_data.Data().NumRows(), this_chan = channel;
        {  // This block works out the channel (0=left, 1=right...)
            KALDI_ASSERT(num_chan > 0);  // should have been caught in
            // reading code if no channels.
            if (channel == -1) {
                this_chan = 0;
                if (num_chan != 1)
                    KALDI_WARN << "Channel not specified but you have data with "
                    << num_chan << " channels; defaulting to zero";
            }
            else {
                if (this_chan >= num_chan) {
                    KALDI_WARN << "wav data has "
                        << num_chan << " channels but you specified channel "
                        << channel << ", producing no output.";
                }
                return(0);
            }
        }
        BaseFloat vtln_warp_local;  // Work out VTLN warp factor.
        vtln_warp_local = vtln_warp;

        SubVector<BaseFloat> waveform(wave_data.Data(), this_chan);

        try {
            mfcc.ComputeFeatures(waveform, wave_data.SampFreq(),
                vtln_warp_local, pfeatures);
        }
        catch (...) {
            KALDI_WARN << "Failed to compute features for utterance";
        }
        if (subtract_mean) {
            Vector<BaseFloat> mean(pfeatures->NumCols());
            mean.AddRowSumMat(1.0, *pfeatures);
            mean.Scale(1.0 / pfeatures->NumRows());
            for (int32 i = 0; i < pfeatures->NumRows(); i++)
                pfeatures->Row(i).AddVec(-1.0, mean);
        }
    }

    catch (const std::exception& e) {
        std::cerr << e.what();
        return -1;
    }
    return (0);
}

int ComputeMFCCFeatsFromFile(MfccOptions mfcc_opts, std::string wav_file_path, Matrix<BaseFloat>* pfeatures) {
    try {
        // Define defaults for global options.
        bool subtract_mean = false;
        BaseFloat vtln_warp = 1.0;
        std::string utt2spk_rspecifier;
        int32 channel = -1;
        BaseFloat min_duration = 0.0;
        Mfcc mfcc(mfcc_opts);

        std::filebuf wav_fb;
        WaveData wave_data;

        if (wav_fb.open(wav_file_path, std::ios::in | std::ios::binary))
        {
            std::istream wav_stream(&wav_fb);
            wave_data.Read(wav_stream);
        }
        else
        {
            KALDI_ERR << "Could not read wav file " << wav_file_path;
        }

        ComputeMFCCFeatsFromWavData(mfcc_opts, wave_data, pfeatures);
    }

    catch (const std::exception& e) {
        std::cerr << e.what();
        return -1;
    }
    return (0);
}
