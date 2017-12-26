// Client.cpp : main project file.

#include "CalculatorService.h"
#include "EchoService.h"

using namespace System;
using namespace System::ServiceModel;

int main(array<System::String ^> ^args)
{
    EchoServiceClient ^ echoClient = gcnew EchoServiceClient();
	Console::WriteLine(L"Echo(\"Is anyone there?\") returned: {0}", echoClient->Echo(L"Is anyone there?"));
	
    CalculatorServiceClient ^ calculatorClient = gcnew CalculatorServiceClient();

	Console::WriteLine(L"Add(5) returned: {0}", calculatorClient->Add(5));
	Console::WriteLine(L"Add(-3) returned: {0}", calculatorClient->Add(-3));

    Console::WriteLine();
    Console::WriteLine(L"Press Enter to exit...");
    Console::ReadLine();

    return 0;
}
