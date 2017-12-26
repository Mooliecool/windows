//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;

namespace Microsoft.Samples.Scenario_Client
{
    class Program
    {
        static void Main(string[] args)
        {
            double value;
            string option;

            try
            {
                ServiceReference1.BankServiceClient bank = new ServiceReference1.BankServiceClient();
                value = GetAmount("Enter initial deposit amount ($): ");
                Console.WriteLine(String.Format("\tBalance: ${0}", bank.OpenAccount(value)));

                do
                {
                    Console.Write("What would you like to do? ('D'eposit; 'W'ithdraw; 'C'lose account): ");
                    option = Console.ReadLine();
                    Console.WriteLine();

                    switch (option)
                    {
                        case "D":
                            value = GetAmount("Please enter an amount to deposit ($): ");
                            Console.WriteLine(String.Format("\tBalance: ${0}", bank.Deposit(value)));
                            break;
                        case "W":
                            value = GetAmount("Please enter an amount to withdraw ($): ");
                            Console.WriteLine(String.Format("\tBalance: ${0}", bank.Withdraw(value)));
                            break;
                        case "C":
                            Console.WriteLine(String.Format("\tBalance: ${0}", bank.CloseAccount()));
                            break;
                        default:
                            Console.WriteLine("Invalid entry. Please try again");
                            break;
                    }

                } while (option != "C");
            }
            catch (Exception e)
            {
                Console.WriteLine(String.Format("Bank Teller failed: {0}", e.Message));
            }

            Console.WriteLine("Bank Teller completed. Press [ENTER] to continue ...");
            Console.ReadLine();

        }

        static double GetAmount(string prompt)
        {            
            double amount;
            while (true)
            {
                Console.Write(prompt);
                if (Double.TryParse(Console.ReadLine(), out amount))
                {
                    return amount;
                }
                else
                {
                    Console.WriteLine("Invalid data entered. Please try again.");
                }
            }
        }
    }
}
