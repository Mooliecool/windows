//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Data;
using System.Globalization;
using System.Diagnostics;

namespace GameApp
{
    /// <summary>
    /// This class contains static methods used to create AI instances based
    /// on a specified type.
    /// </summary>
    public sealed class AIHandler
    {
        /// <summary>
        /// Private constructor for AIHandler.
        /// </summary>
        private AIHandler()
        {

        }
        
        /// <summary>
        /// Create an instace of the specified AI type.  Because the types
        /// are derived, a polymorphed AI pointer is returned.
        /// </summary>
        /// <param name="type">AI type to create</param>
        /// <returns>Reference to instantiated AI class</returns>
        static public AI Create(AI.AIType type)
        {
            AI aiInst = Instance(type);
            aiInst.Load(type);

            return aiInst;
        }

        /// <summary>
        /// Create a copy of the specified AI instance.  Because the types
        /// are derived, a polymorphed AI pointer is returned.
        /// </summary>
        /// <param name="ai">AI instance to be copied</param>
        /// <returns>Reference to instantiated AI class</returns>
        static public AI Create(AI ai)
        {
            AI aiInst = Instance(ai.Type);
            aiInst.Load(ai);

            return aiInst;
        }

        /// <summary>
        /// Create an instance of the AI class as specified by the given
        /// DataRow.
        /// Because the types are derived, a polymorphed AI pointer is
        /// returned.
        /// </summary>
        /// <param name="ai">AI instance to be created</param>
        /// <returns>Reference to instantiated AI class</returns>
        static public AI Create(DataRow dr)
        {
            AI.AIType aiType = (AI.AIType)int.Parse((string)dr["Type"], 
                CultureInfo.InvariantCulture);
            Debug.Assert(aiType >= 0 && aiType < AI.AIType.Count,
                "AI.AI: Invalid AI Type");

            AI aiInst = Instance(aiType);
            aiInst.Load(dr);

            return aiInst;
        }

        /// <summary>
        /// Allocate an instance of a derived AI class based on the specified
        /// type.
        /// </summary>
        /// <param name="type">AI type to instance</param>
        /// <returns>Reference to instantiated AI class</returns>
        static private AI Instance(AI.AIType type)
        {
            switch (type)
            {
                case AI.AIType.DuckAndFire:
                    return new DuckAndFireAI();
                case AI.AIType.Flyby:
                    return new FlybyAI();
                case AI.AIType.ShotArc:
                    return new ShotArcAI();
                case AI.AIType.Static:
                    return new StaticAI();
                case AI.AIType.Aggressive:
                    return new AggressiveAI();
            }

            Debug.Assert(false,
                "AIHandler.Instance: Invalid AI type specified");

            return null;
        }
    }
}
