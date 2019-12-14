using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using Serilog;

namespace NetworkExplorer
{

    public partial class App : Application
    {
        private sealed class NativeMethods
        {
            private const string RF24DLL = @"EmbeddedDLL.dll";

            /* Constructor and Destructor */
            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern void test_PipeCommunication();

        }

        static void Main(string[] args)
        {
            Log.Logger = new LoggerConfiguration()
                .MinimumLevel.Debug()
                .WriteTo.Console()
                .WriteTo.File("logs\\NetworkExplorer.log", flushToDiskInterval: TimeSpan.FromMilliseconds(100))
                .CreateLogger();

            NativeMethods.test_PipeCommunication();

            RF24Node.Endpoint test = new RF24Node.Endpoint();

            RF24Node.EndpointConfig cfg = new RF24Node.EndpointConfig();

            cfg.rxQueueSize = 256;
            cfg.txQueueSize = 128;
            cfg.version = 1;

            test.Configure(cfg);
            test.SetNetworkingMode(RF24Node.NetworkMode.NET_MODE_STATIC);
            test.SetEnpointStaticAddress(RF24Node.Endpoint.RootNode0);

            test = null;
        }
    }
}
