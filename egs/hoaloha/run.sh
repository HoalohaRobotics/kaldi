#!/usr/bin/env bash

export dir=exp/chain_cleaned/tdnn_1d_sp
export graph_dir=$dir/graph_tgsmall

# book-keeping
echo "utt1 $1" > data/convert_me/wav.scp
utils/copy_data_dir.sh data/convert_me/ data/convert_me_hires

# Mel Frequency Cepstral Coefficients
steps/make_mfcc.sh --nj 1 --mfcc-config conf/mfcc_hires.conf --cmd run.pl data/convert_me_hires

# Cepstral mean and variance normalization (CMVN) is a computationally efficient normalization technique for robust speech recognition.
steps/compute_cmvn_stats.sh data/convert_me_hires

# ivector extraction
steps/online/nnet2/extract_ivectors_online.sh --cmd run.pl --nj 1 data/convert_me_hires exp/nnet3_cleaned/extractor exp/nnet3_cleaned/ivectors_convert_me_hires

# decode using the nnet3_cleaned model
steps/nnet3/decode.sh --acwt 1.0 --post-decode-acwt 10.0 --nj 1 --cmd run.pl --online-ivector-dir exp/nnet3_cleaned/ivectors_convert_me_hires $graph_dir data/convert_me_hires $dir/decode_convert_me_tgsmall

# at this point we can get a transcript in CTM format from the lattice
steps/get_ctm.sh data/convert_me exp/chain_cleaned/tdnn_1d_sp/graph_tgsmall/ exp/chain_cleaned/tdnn_1d_sp/decode_convert_me_tgsmall
cat exp/chain_cleaned/tdnn_1d_sp/decode_convert_me_tgsmall/score_20/convert_me.ctm

# this "rescores" the result using the rnnlm_lstm_1a model (takes longer and *should* produce better results)
#rnnlm/lmrescore_pruned.sh --cmd "run.pl --mem 8G" --weight 0.45 --max-ngram-order 4 data/lang_test_tgsmall exp/rnnlm_lstm_1a/ data/convert_me_hires exp/chain_cleaned/tdnn_1d_sp/decode_convert_me_tgsmall exp/chain_cleaned/tdnn_1d_sp/decode_convert_me_tgsmall_rescore

# this generates the transcript in CTM format from the rescored lattice
#steps/get_ctm.sh data/convert_me exp/chain_cleaned/tdnn_1d_sp/graph_tgsmall/ exp/chain_cleaned/tdnn_1d_sp/decode_convert_me_tgsmall_rescore
#cat exp/chain_cleaned/tdnn_1d_sp/decode_convert_me_tgsmall_rescore/score_20/convert_me.ctm
