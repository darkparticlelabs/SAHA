using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DotNetInjectDll
{
    public class Class1
    {
        static int EntryPoint(String pwzArgument)
        {
            System.Media.SystemSounds.Beep.Play();

            MessageBox.Show(
                "I am a managed app.\n\n" +
                "I am running inside: [" +
                System.Diagnostics.Process.GetCurrentProcess().ProcessName +
                "]\n\n" + (String.IsNullOrEmpty(pwzArgument) ?
                "I was not given an argument" :
                "I was given this argument: [" + pwzArgument + "]"));

            return 0;
        }

        static void Main(string[] args)
        {
            EntryPoint("hello world");
        }
    }
}
