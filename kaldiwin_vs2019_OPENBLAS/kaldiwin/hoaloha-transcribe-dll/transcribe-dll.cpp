#include "transcribe-dll.h"

namespace hoalohadll
{
	int Init(ModelAndDecoder* m)
	{
		return hoaloha::Init(m);
	}

	int Transcribe(ModelAndDecoder* m, std::string wav_file_path, std::string& transcription, Lattice* lattice, double& likelihood)
	{
		return hoaloha::TranscribeFile(m, wav_file_path, transcription, lattice, likelihood);
	}

	void Cleanup(ModelAndDecoder* m)
	{
		hoaloha::Cleanup(m);
	}
}