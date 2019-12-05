using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace NetworkExplorer.RF24Node.Network
{
    class Frame
    {
        
    }


    sealed class FrameBuffer
    {
        private UIntPtr _buffer;
        
        public FrameBuffer()
        {

        }


        public UIntPtr data()
        {
            return _buffer;
        }
    }


    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct FrameHeaderField
    {
        UInt16 number;
        UInt16 dstNode;
        UInt16 srcNode;
        byte msgType;
        byte reserved;
    }
}
