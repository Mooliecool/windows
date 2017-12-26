// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
namespace System.Reflection.Emit {
using System;
using System.Security.Permissions;

[System.Runtime.InteropServices.ComVisible(true)]
public struct OpCode
{
	internal String m_stringname;
	internal StackBehaviour m_pop;
	internal StackBehaviour m_push;
	internal OperandType m_operand;
	internal OpCodeType m_type;
	internal int m_size;
	internal byte m_s1;
	internal byte m_s2;
	internal FlowControl m_ctrl;

	// Specifies whether the current instructions causes the control flow to
	// change unconditionally.
	internal bool m_endsUncondJmpBlk;


	// Specifies the stack change that the current instruction causes not
	// taking into account the operand dependant stack changes.
	internal int m_stackChange;

	internal OpCode(String stringname, StackBehaviour pop, StackBehaviour push, OperandType operand, OpCodeType type, int size, byte s1, byte s2, FlowControl ctrl, bool endsjmpblk, int stack)
	{
		m_stringname = stringname;
		m_pop = pop;
		m_push = push;
		m_operand = operand;
		m_type = type;
		m_size = size;
		m_s1 = s1;
		m_s2 = s2;
		m_ctrl = ctrl;
		m_endsUncondJmpBlk = endsjmpblk;
		m_stackChange = stack;

	}

	internal bool EndsUncondJmpBlk()
	{
		return m_endsUncondJmpBlk;
	}

	internal int StackChange()
	{
		return m_stackChange;
	}

	public OperandType OperandType
	{
		get
		{
			return (m_operand);
		}
	}

	public FlowControl FlowControl
	{
		get
		{
			return (m_ctrl);
		}
	}

	public OpCodeType OpCodeType
	{
		get
		{
			return (m_type);
		}
	}


	public StackBehaviour StackBehaviourPop
	{
		get
		{
			return (m_pop);
		}
	}

	public StackBehaviour StackBehaviourPush
	{
		get
		{
			return (m_push);
		}
	}

	public int Size
	{
		get
		{
			return (m_size);
		}
	}

	public short Value
	{
		get
		{
			if (m_size == 2)
				return (short) (m_s1 << 8 | m_s2);
			return (short) m_s2;
		}
	}


	public String Name
	{
		get
		{
			return m_stringname;
		}
	}

    public override bool Equals(Object obj)
    {
        if (obj is OpCode)
          return Equals((OpCode)obj);
        else
          return false;
    }
    
    public bool Equals(OpCode obj)
    {
        return obj.m_s1 == m_s1 && obj.m_s2 == m_s2;
    }
    
    public static bool operator ==(OpCode a, OpCode b)
    {
        return a.Equals(b);
    }
        
    public static bool operator !=(OpCode a, OpCode b)
    {
        return !(a == b);
    }
	
	public override int GetHashCode()
	{
		return this.m_stringname.GetHashCode();
	}

	public override String ToString()
	{
		return m_stringname;
	}

}
}
