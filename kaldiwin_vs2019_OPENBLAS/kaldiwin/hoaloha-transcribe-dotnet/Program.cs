using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NAudio.Wave;

namespace hoaloha_transcribe_dotnet
{
    public delegate void WriteDelegate(string s);

    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            IntPtr lattice;
            string transcription;
            double likelihood;
            
            if(args.Length != 1)
            {
                Console.WriteLine("One Parameter Required: path to model.\nPress any key to exit.");
                Console.ReadLine();
                return;
            }

            string model_path = args[0];

            using (Transcriber t = new Transcriber(model_path))
            {
                while (true)
                {
                    Console.Write("\nFile: ");
                    var input = Console.ReadLine().Trim('"');
                    if (input == "q")
                        break;

                    transcription = "";
                    likelihood = 0;

                    try 
                    {
                        using (WaveFileReader reader = new WaveFileReader(input))
                        {
                            var data = new byte[(reader.WaveFormat.BitsPerSample / (sizeof(byte) * 8)) * reader.SampleCount];
                            reader.Read(data, 0, data.Length);

                            IntPtr p = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(byte)) * data.Length);
                            Marshal.Copy(data, 0, p, data.Length);
                            t.TranscribeWaveData(p, reader.WaveFormat.SampleRate, (int)reader.SampleCount, out transcription, out lattice, ref likelihood);
                            Marshal.FreeHGlobal(p);

                            Console.WriteLine(transcription);
                            Console.WriteLine(likelihood);
                        }
                    }
                    catch (Exception e)
                    {
                        Console.Write(e);
                    }
                }
            }
        }
    }
}
