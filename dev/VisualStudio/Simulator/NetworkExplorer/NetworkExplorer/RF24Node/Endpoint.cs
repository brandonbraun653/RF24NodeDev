using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

using NetworkExplorer.RF24Node;

namespace NetworkExplorer.RF24Node
{
    using LogicalAddress = UInt16;



    sealed class Endpoint
    {
        private sealed class NativeMethods
        {
            private const string RF24DLL = @"EmbeddedDLL.dll";

            /* Constructor and Destructor */
            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern IntPtr new__Endpoint();

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern void delete__Endpoint(IntPtr obj);


            /* Initialization and Configuration */
            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_configure(IntPtr obj, ref EndpointConfig cfg);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_setNetworkingMode(IntPtr obj, int cfg);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_setEnpointStaticAddress(IntPtr obj, UInt16 address);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_setParentStaticAddress(IntPtr obj, UInt16 address);


            /* Networking Operations */
            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_requestAddress(IntPtr obj);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_renewAddressReservation(IntPtr obj);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_connect(IntPtr obj);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_disconnect(IntPtr obj);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_reconnect(IntPtr obj);


            /* Asynchronous Processing */
            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_onEvent(IntPtr obj, int evnt, EventFuncPtr_t callback);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_processMessageBuffers(IntPtr obj);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_processDHCPServer(IntPtr obj);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_processMessageRequests(IntPtr obj);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_processEventHandlers(IntPtr obj);


            /* Data Transfer IO */
            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_write(IntPtr obj, int dst, byte[] data, int size);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_read(IntPtr obj, byte[] data, int size);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern bool EP_packetAvailable(IntPtr obj);

            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern int EP_nextPacketLength(IntPtr obj);


            /* Information Getters */
            [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
            public static extern UInt16 EP_isConnected(IntPtr obj);

        }

        private readonly IntPtr Instance;

        public static readonly LogicalAddress RootNode0 = 0000;
        public static readonly LogicalAddress RootNode1 = 1000;
        public static readonly LogicalAddress RootNode2 = 2000;
        public static readonly LogicalAddress RootNode3 = 3000;
        public static readonly LogicalAddress RootNode4 = 4000;


        public Endpoint()
        {
            Instance = NativeMethods.new__Endpoint();
        }

        ~Endpoint()
        {
            NativeMethods.delete__Endpoint(Instance);
        }

        public UInt16 Configure(EndpointConfig cfg)
        {
            return NativeMethods.EP_configure(Instance, ref cfg);
        }

        public UInt16 SetNetworkingMode(NetworkMode mode) 
        { 
            return NativeMethods.EP_setNetworkingMode(Instance, (int)mode); 
        }

        public UInt16 SetEnpointStaticAddress(LogicalAddress address) 
        {
            return NativeMethods.EP_setEnpointStaticAddress(Instance, address);
        }

        public UInt16 SetParentStaticAddress(LogicalAddress address) 
        {
            return NativeMethods.EP_setParentStaticAddress(Instance, address);
        }

        public UInt16 RequestAddress() 
        {
            return NativeMethods.EP_requestAddress(Instance);
        }

        public UInt16 RenewAddressReservation() 
        {
            return NativeMethods.EP_renewAddressReservation(Instance);
        }

        public UInt16 Connect() 
        {
            return NativeMethods.EP_connect(Instance);
        }

        public UInt16 Disconnect() 
        {
            return NativeMethods.EP_disconnect(Instance);
        }

        public UInt16 Reconnect()
        {
            return NativeMethods.EP_reconnect(Instance);
        }

        public UInt16 OnEvent(Event evt, EventFuncPtr_t function)
        {
            return NativeMethods.EP_onEvent(Instance, (int)evt, function);
        }

        public UInt16 ProcessMessageBuffers() 
        {
            return NativeMethods.EP_processMessageBuffers(Instance);
        }

        public UInt16 ProcessDHCPServer() 
        {
            return NativeMethods.EP_processDHCPServer(Instance);
        }

        public UInt16 ProcessMessageRequests() 
        {
            return NativeMethods.EP_processMessageRequests(Instance);
        }

        public UInt16 ProcessEventHandlers() 
        {
            return NativeMethods.EP_processEventHandlers(Instance);
        }

        public UInt16 Write(LogicalAddress dst, byte[] data, int length) 
        {
            return NativeMethods.EP_write(Instance, dst, data, length);
        }

        public UInt16 Read(byte[] data, int length) 
        {
            return NativeMethods.EP_read(Instance, data, length);
        }

        public bool PacketAvailable() 
        {
            return NativeMethods.EP_packetAvailable(Instance);
        }

        public int NextPacketLength() 
        {
            return NativeMethods.EP_nextPacketLength(Instance);
        }

        public EndpointStatus GetEndpointStatus() 
        {
            var temp = new NotImplementedException();
            throw temp;

            //return new EndpointStatus();
        }

        public UInt16 IsConnected() 
        {
            return NativeMethods.EP_isConnected(Instance);
        }
    }
}
