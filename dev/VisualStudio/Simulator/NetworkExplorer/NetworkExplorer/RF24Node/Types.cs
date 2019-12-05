using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NetworkExplorer.RF24Node
{

    enum NetworkingMode
    {
        MANUAL,
        MESH,
        AUTO
    }

    enum ErrCode
    {
        OK,
        NOT_SUPPORTED,
        FAIL
    }

}
