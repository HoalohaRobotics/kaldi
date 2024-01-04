#pragma once

#include <string>
#include "..\hoaloha-transcribe\transcribe.h"
#ifdef HOALOHA_TRANSCRIBE_EXPORTS
#define HOALOHA_TRANSCRIBE_API __declspec(dllexport)
#else
#define HOALOHA_TRANSCRIBE_API __declspec(dllimport)
#endif

typedef void(_stdcall* LPEXTFUNCWRITE) (LPCSTR s);

namespace hoalohadll
{
	extern "C" HOALOHA_TRANSCRIBE_API intptr_t Init(CHAR * model_path);
	extern "C" HOALOHA_TRANSCRIBE_API int TranscribeFile(intptr_t model, CHAR * wav_file_path, LPEXTFUNCWRITE transcription, intptr_t lattice, double& likelihood);
	extern "C" HOALOHA_TRANSCRIBE_API int TranscribeWaveData(intptr_t model, intptr_t data, int samp_freq, int sample_count, bool subtract_mean, LPEXTFUNCWRITE write, intptr_t lattice, double& likelihood);
	extern "C" HOALOHA_TRANSCRIBE_API void Cleanup(intptr_t model);
}