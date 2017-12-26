using System;

namespace Microsoft.SDK.Samples.VistaBridge.Library.PowerManagement
{
    internal enum ExecutionState : uint
    {
        SystemRequired = 0x00000001,
        DisplayRequired = 0x00000002,
        Continuous = 0x80000000,
    }
}
