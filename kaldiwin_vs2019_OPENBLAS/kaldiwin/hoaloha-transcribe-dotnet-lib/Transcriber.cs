using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace hoaloha_transcribe_dotnet
{
    public delegate void WriteDelegate(string s);

    public class Transcriber : IDisposable
    {
        [DllImport("Robot_Kaldi_Core.dll", SetLastError = true)]
        private static extern IntPtr Init(string model_path);

        [DllImport("Robot_Kaldi_Core.dll", SetLastError = true)]
        private static extern int TranscribeFile(IntPtr model, string wav_file_path, WriteDelegate transcription, ref double likelihood);

        [DllImport("Robot_Kaldi_Core.dll", SetLastError = true)]
        private static extern int TranscribeWaveData(IntPtr model, IntPtr data, int samp_freq, int sample_count, WriteDelegate transcription, ref double likelihood);

        [DllImport("Robot_Kaldi_Core.dll", SetLastError = true)]
        private static extern void Cleanup(IntPtr model);

        private IntPtr m_model;
        private bool disposedValue;

        public Transcriber(string model_path)
        {
            m_model = Init(model_path);
        }

        public void TranscribeFile(string file_path, out string transcription, ref double likelihood)
        {
            string t = "";
            TranscribeFile(this.m_model, file_path, s => { t = s; }, ref likelihood);
            transcription = t;
        }
        public void TranscribeWaveData(IntPtr data, int sample_freq, int sample_count, out string transcription, ref double likelihood)
        {
            string t = "";
            TranscribeWaveData(this.m_model, data, sample_freq, sample_count, s => { t = s; }, ref likelihood);
            transcription = t;
        }
        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // dispose managed state (managed objects)
                }

                // free unmanaged resources (unmanaged objects) and override finalizer
                // set large fields to null
                Cleanup(m_model);
                disposedValue = true;
            }
        }

        // override finalizer only if 'Dispose(bool disposing)' has code to free unmanaged resources
        ~Transcriber()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            Dispose(disposing: false);
        }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
    }
}
