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
	extern "C" HOALOHA_TRANSCRIBE_API int Init(ModelAndDecoder * m);
	extern "C" HOALOHA_TRANSCRIBE_API int Transcribe(ModelAndDecoder * m, std::string wav_file_path, std::string & transcription, Lattice * lattice, double& likelihood);
	extern "C" HOALOHA_TRANSCRIBE_API void Cleanup(ModelAndDecoder * m);
}