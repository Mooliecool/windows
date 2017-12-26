
//-----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//-----------------------------------------------------------------

using System;
using Client.ServiceReference1;

namespace Microsoft.Samples.HandyManService
{

    class Program
    {
        static void Main(string[] args)
        {
            int selection = 0;
            while (true)
            {
                Greetings();
                selection = GetSelection();
                if (selection == 3)
                    break;
                else
                    Diagnose(selection);
            }
        }

        static void Diagnose(int selection)
        {
            ServiceClient client = new ServiceClient();
            string rtn = "";
            if (selection == 1)
                rtn = client.DiagnoseStart();
            else if (selection == 2)
                rtn = client.DiagnoseAC();        
            Console.WriteLine(rtn);
            bool input;
            while (!rtn.ToLower().Contains("diagnosis complete"))
            {
                input = GetInput();
                rtn = client.NextStep(input);
                Console.WriteLine(rtn);
            }
            client.Close();
            client.ChannelFactory.Close();
        }
        static void Greetings()
        {
            Console.WriteLine("Welcome to HandyMan Service");
            Console.WriteLine("Select an option (enter 1, 2 or 3)");
            Console.WriteLine("1. My car is not starting");
            Console.WriteLine("2. My car air conditioner is not cooling");
            Console.WriteLine("3. Exit");
        }
        static bool GetInput()
        {
            char input='a';
            ConsoleKeyInfo cki;
            while(input != 'y' && input !='n')
            {
                cki = Console.ReadKey();
                input = char.ToLower(cki.KeyChar);
            }
            return input == 'y';
        }

        static int GetSelection()
        {
            char input = 'a';
            ConsoleKeyInfo cki;
            while (input != '1' && input != '2' && input != '3')
            {
                cki = Console.ReadKey();
                input = char.ToLower(cki.KeyChar);
            }
            return int.Parse(input.ToString());
        }
    }
}
