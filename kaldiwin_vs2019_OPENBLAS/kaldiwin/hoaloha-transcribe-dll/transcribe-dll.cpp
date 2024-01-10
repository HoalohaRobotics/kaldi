#include "transcribe-dll.h"
#include <cstdint>

namespace hoalohadll
{
	intptr_t Init(CHAR * model_path)
	{
		ModelAndDecoder* model = new ModelAndDecoder();
		hoaloha::Init(model, model_path);
		return (intptr_t)model;
	}

	int TranscribeFile(intptr_t model, CHAR* wav_file_path, LPEXTFUNCWRITE write, double& likelihood)
	{
		std::string wav_path = wav_file_path;
		std::string transcription;
		int ret= hoaloha::TranscribeFile((ModelAndDecoder*) model, wav_path, transcription, likelihood);
		write(transcription.c_str());
		return ret;
	}

	int TranscribeWaveData(intptr_t model, intptr_t data, int samp_freq, int sample_count, LPEXTFUNCWRITE write, double& likelihood)
	{
		std::string transcription;
		int ret = hoaloha::TranscribeWaveData((ModelAndDecoder*)model, (short*)data, samp_freq, sample_count, transcription, likelihood);
		write(transcription.c_str());
		return ret;
	}

	void Cleanup(intptr_t model)
	{
		hoaloha::Cleanup((ModelAndDecoder*)model);
		delete (ModelAndDecoder*)model;
	}
}