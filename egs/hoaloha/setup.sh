#!/usr/bin/env bash

# used to build the language graph.  Only done once.  I committed the built graph data, but 
# included this for reference

# run from kaldi/egs/hoaloh in a docker container started like this...
# sudo docker run -it -v /mnt/c/Users/Gersh/Git/kaldi_hoaloha/egs/hoaloha:/opt/kaldi/egs/hoaloha kaldiasr/kaldi:latest bash
# replace '/mnt/c/Users/Gersh/Git/kaldi_hoaloha/egs/hoaloha' with the path to your haloha dir in the kaldi/egs tree

cd ../wsj/s5
cp ../../hoaloha/*.gz .

tar -xzf 0013_librispeech_v1_chain.tar.gz
tar -xzf 0013_librispeech_v1_lm.tar.gz
tar -xzf 0013_librispeech_v1_extractor.tar.gz

export dir=exp/chain_cleaned/tdnn_1d_sp
export graph_dir=$dir/graph_tgsmall

# build the decoding graph from the lang_test_tgsmall data
utils/mkgraph.sh --self-loop-scale 1.0 --remove-oov data/lang_test_tgsmall $dir $graph_dir

cp -r ./exp ../../hoaloha/s5

rm *.gz