//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Workflow.Activities;

namespace Microsoft.Workflow.Samples
{
	public sealed partial class BankMachineWorkflow: SequentialWorkflowActivity
	{
		public BankMachineWorkflow()
		{
			InitializeComponent();
		}

        private double depositValue;
        private double withdrawalValue;
        private double balanceValue;
        private int serviceValue;

        public double Deposit
        {
            get { return depositValue; }
            set { depositValue = value; }
        }

        public double Withdrawal
        {
            get { return withdrawalValue; }
            set { withdrawalValue = value; }
        }

        public double Balance
        {
            get { return balanceValue; }
            set { balanceValue = value; }
        }

        public int Service
        {
            get { return serviceValue; }
            set { serviceValue = value; }
        }

        private void UserInputExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Please enter '1' to deposit, '2' to withdraw, or any other number to Exit");
            serviceValue = 0;
            try
            {
                serviceValue = int.Parse(System.Console.ReadLine());
            }
            catch (FormatException ex)
            {
                Console.WriteLine(ex.Message);
            }
            
        }

        private void CheckOption(object sender, ConditionalEventArgs e)
        {
            e.Result = false;
            if (serviceValue == 1 || serviceValue == 2 || serviceValue == 0)
            {
                e.Result = true;
            }
        }

        private void CheckDeposit(object sender, ConditionalEventArgs e)
        {
            // Go into the deposit activity if user selects 1.
            e.Result = (serviceValue == 1);
            
            
        }

        private void DepositExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Enter desired deposit amount:");
            try
            {
                depositValue = double.Parse(System.Console.ReadLine());
                if (depositValue > 0)
                {
                    balanceValue += depositValue;
                }
                else
                {
                    Console.WriteLine("Negative deposit amount is not permitted.");
                }
            }
            catch (FormatException)
            {
                Console.WriteLine("Deposit attempt failed because deposit amount entered was not a numeric value.");
            }
        }

        private void CheckWithdrawal(object sender, ConditionalEventArgs e)
        {
            // Go into the withdrawal activity if user selects 2.
            e.Result = (serviceValue == 2);
            
        }

        private void WithdrawalExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Enter desired withdrawal amount:");
            try
            {
                withdrawalValue = double.Parse(System.Console.ReadLine());
                if (balanceValue >= withdrawalValue && withdrawalValue > 0)
                {
                    balanceValue -= withdrawalValue;
                }
                else
                {
                    Console.WriteLine("Withdrawal attempt failed: the withdrawal amount specified was either greater than the account balance or a negative value");
                }
            }
            catch (FormatException)
            {
                Console.WriteLine("Withdrawal attempt failed because withdrawal amount entered was not a numeric value.");
            }
        }
	}

}
