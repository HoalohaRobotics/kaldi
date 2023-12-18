// TODO: Copywrite?
// TODO: Figure out how to build the model(s) in windows (instead of copy from WSL)

#include "transcribe.h"

int main(int argc, char* argv[]) {
    ModelAndDecoder m;
    hoaloha::Init(&m);

    string transcription;
    Lattice lattice;
    double likelihood;
    string wav_file_path;

    while (true)
    {
        transcription = "";
        std::cout << "Wav File Path: ";
        std::getline(std::cin >> std::ws, wav_file_path);
        wav_file_path.erase(remove(wav_file_path.begin(), wav_file_path.end(), '\"'), wav_file_path.end());
        if (wav_file_path.compare("q") == 0)
        {
            break;
        }
        hoaloha::TranscribeFile(&m, wav_file_path, transcription, &lattice, likelihood);
        std::cout << transcription + '\n';
    }

    hoaloha::Cleanup(&m);

    return 0;

}


