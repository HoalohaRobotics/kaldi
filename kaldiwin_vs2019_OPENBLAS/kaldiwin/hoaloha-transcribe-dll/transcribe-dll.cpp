#include "transcribe-dll.h"
#include <cstdint>

namespace hoalohadll
{
	intptr_t Init()
	{
		ModelAndDecoder* model = new ModelAndDecoder();
		hoaloha::Init(model);
		return (intptr_t)model;
	}

	int Transcribe(intptr_t model, CHAR* wav_file_path, CHAR* transcription, intptr_t lattice, double& likelihood)
	{
		std::string w(wav_file_path);
		std::string t(transcription);
		return hoaloha::TranscribeFile((ModelAndDecoder*) model, w, t, (Lattice*)lattice, likelihood);
	}

	void Cleanup(intptr_t model)
	{
		hoaloha::Cleanup((ModelAndDecoder*)model);
		delete (ModelAndDecoder*)model;
	}
}