using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace NetworkExplorer.RF24Node
{

    enum NetworkMode
    {
        NET_MODE_STATIC,
        NET_MODE_MESH
    }

    enum Event
    {
        CONNECT,
        DISCONECT,
        MSG_TX,
        MSG_RX
    }

    enum ErrCode
    {
        OK,
        NOT_SUPPORTED,
        FAIL
    }


    [StructLayout(LayoutKind.Sequential)]
    public struct EndpointConfig
    {
        public UInt32 version;        // Struct version ID
        public UInt16 rxQueueSize;    // Size of the network rx queue in bytes
        public UInt16 txQueueSize;    // Size of the network tx queue in bytes 
        
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct EndpointStatus
    {

    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void EventFunctionPtr();
}
