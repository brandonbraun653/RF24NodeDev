using System;
using System.Collections.Generic;
using System.Linq;
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
        static void Main(string[] args)
        {
            Log.Logger = new LoggerConfiguration()
                .MinimumLevel.Debug()
                .WriteTo.Console()
                .WriteTo.File("logs\\NetworkExplorer.log", flushToDiskInterval: TimeSpan.FromMilliseconds(100))
                .CreateLogger();



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
