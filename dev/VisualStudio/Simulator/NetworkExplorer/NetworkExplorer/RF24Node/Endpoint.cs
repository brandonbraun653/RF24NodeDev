using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using NetworkExplorer.RF24Node;

namespace NetworkExplorer.RF24Node
{
    using LogicalAddress = UInt16;

    sealed class Endpoint
    {

        public Endpoint()
        {

        }

        ~Endpoint()
        {

        }

        /// <summary>
        /// Configures the network connection strategy for this endpoint
        /// </summary>
        /// <param name="mode"> The desired connection mode </param>
        public void assignNetworkingMode(NetworkingMode mode)
        {

        }

        public ErrCode setLogicalAddress(LogicalAddress address)
        {

        }

        public void getLogicalAddress()
        {

        }

        public void requestAddress()
        {

        }

        public void ping()
        {

        }

        public void read()
        {

        }

        public void write()
        {

        }


    }
}
