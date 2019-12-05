using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

using NetworkExplorer.RF24Node.Network;

namespace NetworkExplorer
{
    sealed class HeaderHelperWrapper
    {
        private const string RF24DLL = @"EmbeddedDLL.dll";

        [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr new__HeaderHelper();

        [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr new__HeaderHelper(UIntPtr frameBuffer);

        [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr new__HeaderHelper(UInt16 dstNode, byte type);

        [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void delete__HeaderHelper(IntPtr obj);

        [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void copyFromFrameBuffer(IntPtr obj, UIntPtr frameBuffer);

        [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void copyInstance(IntPtr obj, IntPtr srcInstance);

        [DllImport(RF24DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void copyFrameHeader(IntPtr obj, UIntPtr srcInstance);
    }

    class HeaderHelper
    {
        private IntPtr instance;

        public HeaderHelper()
        {
            instance = HeaderHelperWrapper.new__HeaderHelper();
        }

        public HeaderHelper(ref FrameBuffer buffer)
        {
            instance = HeaderHelperWrapper.new__HeaderHelper(buffer.data());
        }

        public HeaderHelper(UInt16 dstNode, byte type)
        {
            instance = HeaderHelperWrapper.new__HeaderHelper(dstNode, type);
        }

        ~HeaderHelper()
        {
            HeaderHelperWrapper.delete__HeaderHelper(instance);
        }
    }
}
