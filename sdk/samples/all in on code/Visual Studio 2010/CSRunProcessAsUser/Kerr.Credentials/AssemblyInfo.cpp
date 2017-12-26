#include "stdafx.h"

using namespace Reflection;
using namespace Runtime::InteropServices;
using namespace Security::Permissions;

[assembly: AssemblyVersionAttribute("1.0.0.1")];
[assembly: ComVisible(false)];
[assembly: CLSCompliantAttribute(true)];
[assembly: SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
