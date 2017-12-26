
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.EnterpriseServices;
using System.Runtime.InteropServices;

// Enterprise Service attributes used to define the application and component configuration
[assembly: ApplicationName("ServiceModelSample")]
[assembly: ApplicationID("E146E066-D3D1-4e0e-B175-30160BD368DE")]
[assembly: ApplicationActivation(ActivationOption.Library)]
[assembly: ApplicationAccessControl(true)]
[assembly: CLSCompliant(true)]

namespace Microsoft.ServiceModel.Samples
{
	[Guid("C551FBA9-E3AA-4272-8C2A-84BD8D290AC7")]
	[ComVisible(true)]
	// Define the component's interface.
	public interface ICalculator
	{
		double Add(double n1, double n2);
		double Subtract(double n1, double n2);
		double Multiply(double n1, double n2);
		double Divide(double n1, double n2);
	}

	[Guid("BE62FF5B-8B53-476b-A385-0F66043049F6")]
	[ProgId("ServiceModelSample.EsCalculator")]
	[ComVisible(true)]
	// Supporting implementation for the ICalculator interface.
	public class ESCalculatorService : ServicedComponent, ICalculator
	{
		public double Add(double n1, double n2)
		{
			return n1 + n2;
		}
		public double Subtract(double n1, double n2)
		{
			return n1 - n2;
		}
		public double Multiply(double n1, double n2)
		{
			return n1 * n2;
		}
		public double Divide(double n1, double n2)
		{
			return n1 / n2;
		}
	}
}
