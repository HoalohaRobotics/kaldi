using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
            double likelihood = 0;
            
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
                    string input = Console.ReadLine().Trim('"');
                    if (input == "q")
                        break;

                    t.TranscribeFile(input, out transcription, out lattice, ref likelihood);

                    Console.WriteLine(transcription);
                    Console.WriteLine(likelihood);
                }
            }
        }
    }
}
