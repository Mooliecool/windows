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
/***************************************************************************/
/*                             ILFormatter.h                               */
/***************************************************************************/

#ifndef ILFormatter_h
#define ILFormatter_h

#include "opinfo.h"
#include "outstring.h"

struct IMetaDataImport;

#define INVALID_IL_OFFSET 0x80000000

/***************************************************************************/
class ILFormatter {
public:
	ILFormatter() : start(0), targetStart(0), stackStart(0) {}

	ILFormatter(IMetaDataImport* aMeta, const BYTE* aStart, 
                const BYTE* aLimit, unsigned maxStack, const COR_ILMETHOD_SECT_EH* eh) 
		: targetStart(0), stackStart(0) {
		init(aMeta, aStart, aLimit, maxStack, eh);
		}
    ~ILFormatter() { delete [] stackStart; delete [] targetStart; }

	void init(IMetaDataImport* aMeta, const BYTE* aStart, 
              const BYTE* aLimit, unsigned maxStack, const COR_ILMETHOD_SECT_EH* eh);
	const BYTE* formatStatement(const BYTE* stmtIL, OutString* out);
	const BYTE* formatInstr(const BYTE* instrIL, OutString* out);
private:

	void formatInstrArgs(OpInfo op, OpArgsVal arg, OutString* out, size_t curIP=INVALID_IL_OFFSET);
    void formatArgs(unsigned numArgs, OutString* out);
    void spillStack(OutString* out);
    void setStackAsTarget(size_t ilOffset);
    void setTarget(size_t ilOffset, size_t depth);

private:
	const BYTE* start;				// keeps us sane
	const BYTE* limit;
	IMetaDataImport* meta;			// used to parse tokens etc

    struct StackEntry {
        OutString val;
        int prec;
    };

    struct Target {
        size_t ilOffset;
        size_t stackDepth;
    };

    Target* targetStart;
    Target* targetEnd;
    Target* targetCur;

    size_t stackDepth();
    void pushAndClear(OutString* val, int prec);
	OutString* pop(int prec = 0);
	OutString* top();
    void popN(size_t num);

	StackEntry* stackStart;
	StackEntry* stackEnd;
	StackEntry* stackCur;   	// points at the next slot to fill

};

#endif

