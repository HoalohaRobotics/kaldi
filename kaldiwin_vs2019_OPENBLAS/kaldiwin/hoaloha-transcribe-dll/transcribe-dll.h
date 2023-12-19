#pragma once

#include <string>
#include "..\hoaloha-transcribe\transcribe.h"
//#include "..\hoaloha-transcribe\NNet3LatgenFaster.h"

#ifdef HOALOHA_TRANSCRIBE_EXPORTS
#define HOALOHA_TRANSCRIBE_API __declspec(dllexport)
#else
#define HOALOHA_TRANSCRIBE_API __declspec(dllimport)
#endif
namespace hoalohadll
{
	extern "C" HOALOHA_TRANSCRIBE_API intptr_t Init();
	extern "C" HOALOHA_TRANSCRIBE_API int Transcribe(intptr_t model, CHAR * wav_file_path, CHAR * transcription, intptr_t lattice, double& likelihood);
	extern "C" HOALOHA_TRANSCRIBE_API void Cleanup(intptr_t model);
}