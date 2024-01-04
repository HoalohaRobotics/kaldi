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
        [DllImport("hoaloha-transcribe-dll.dll", SetLastError = true)]
        private static extern IntPtr Init(string model_path);

        [DllImport("hoaloha-transcribe-dll.dll", SetLastError = true)]
        private static extern int TranscribeFile(IntPtr model, string wav_file_path, WriteDelegate transcription, IntPtr lattice, ref double likelihood);

        [DllImport("hoaloha-transcribe-dll.dll", SetLastError = true)]
        private static extern int TranscribeWaveData(IntPtr model, IntPtr data, int samp_freq, int sample_count, bool subtract_mean, WriteDelegate transcription, IntPtr lattice, ref double likelihood);

        [DllImport("hoaloha-transcribe-dll.dll", SetLastError = true)]
        private static extern void Cleanup(IntPtr model);

        private IntPtr m_model;
        private bool disposedValue;

        public Transcriber(string model_path)
        {
            m_model = Init(model_path);
        }

        public void TranscribeFile(string file_path, out string transcription, out IntPtr lattice, ref double likelihood)
        {
            string t = "";
            lattice = new IntPtr();

            TranscribeFile(this.m_model, file_path, s => { t = s; }, lattice, ref likelihood);
            transcription = t;
        }
        public void TranscribeWaveData(IntPtr data, int sample_freq, int sample_count, out string transcription, out IntPtr lattice, ref double likelihood)
        {
            string t = "";
            lattice = new IntPtr();

            TranscribeWaveData(this.m_model, data, sample_freq, sample_count, false, s => { t = s; }, lattice, ref likelihood);
            transcription = t;
        }
        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // TODO: dispose managed state (managed objects)
                }

                // TODO: free unmanaged resources (unmanaged objects) and override finalizer
                // TODO: set large fields to null
                Cleanup(m_model);
                disposedValue = true;
            }
        }

        // TODO: override finalizer only if 'Dispose(bool disposing)' has code to free unmanaged resources
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
