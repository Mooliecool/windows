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
//
// disasm.cpp : Defines the entry point for the console application.
//
#include "strike.h"
#include "util.h"
#include "disasm.h"
#include <dbghelp.h>

#include "corhdr.h"

#include "cor.h"
#include "dacprivate.h"

#include "openum.h"

#include "sos_md.h"

#define SOS_INCLUDE 1
#include "corhlpr.h"
#include "corhlpr.cpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define printf ExtOut

// typedef unsigned char BYTE;
struct OpCode
{
	int code;
	char *name;
	int args;
	BYTE b1;
	BYTE b2;

	unsigned int getCode() { 
		if (b1==0xFF) return b2;
		else return (0xFE00 | b2);
	}
};

#define OPCODES_LENGTH 0x122

#undef OPDEF
#define OPDEF(c,s,pop,push,args,type,l,s1,s2,ctrl) {c, s, args, s1, s2},
static OpCode opcodes[] =
{
#include "opcode.def"
};

static ULONG position = 0;
static BYTE *pBuffer = NULL;

// The UNALIGNED is because on IA64 alignment rules would prevent
// us from reading a pointer from an unaligned source.
template <typename T>
T readData ( ) {
    T val = *((T UNALIGNED*)(pBuffer+position));
    position += sizeof(T);
    return val;
}

unsigned int readOpcode()
{
	unsigned int c = readData<BYTE>();
	if (c == 0xFE)
	{
		c = readData<BYTE>();
		c |= 0x100;
	}
	return c;
}

void DisassembleToken(IMetaDataImport *i,
                      DWORD token)
{
    HRESULT hr;

    switch (TypeFromToken(token))
    {
    default:
        printf("<unknown token type %08x>", TypeFromToken(token));
        break;

    case mdtTypeDef:
        {
            ULONG cLen;
            WCHAR szName[50];

            hr = i->GetTypeDefProps(token, szName, 49, &cLen, NULL, NULL);

            if (FAILED(hr))
                wcscpy(szName, L"<unknown type def>");

            printf("%S", szName);
        }
        break;

    case mdtTypeRef:
        {
            ULONG cLen;
            WCHAR szName[50];

            hr = i->GetTypeRefProps(token, NULL, szName, 49, &cLen);

            if (FAILED(hr))
                wcscpy(szName, L"<unknown type ref>");

            printf("%S", szName);
        }
        break;

    case mdtFieldDef:
        {
            ULONG cLen;
            WCHAR szFieldName[50];
            WCHAR szClassName[50];
            mdTypeDef mdClass;

            hr = i->GetFieldProps(token, &mdClass, szFieldName, 49, &cLen,
                                  NULL, NULL, NULL, NULL, NULL, NULL);

            if (FAILED(hr))
                wcscpy(szFieldName, L"<unknown field def>");

            hr = i->GetTypeDefProps(mdClass, szClassName, 49, &cLen,
                                    NULL, NULL);

            if (FAILED(hr))
                wcscpy(szClassName, L"<unknown type def>");

            printf("%S::%S", szClassName, szFieldName);
        }
        break;

    case mdtMethodDef:
        {
            ULONG cLen;
            WCHAR szFieldName[50];
            WCHAR szClassName[50];
            mdTypeDef mdClass;

            hr = i->GetMethodProps(token, &mdClass, szFieldName, 49, &cLen,
                                   NULL, NULL, NULL, NULL, NULL);

            if (FAILED(hr))
                wcscpy(szFieldName, L"<unknown method def>");

            hr = i->GetTypeDefProps(mdClass, szClassName, 49, &cLen,
                                    NULL, NULL);

            if (FAILED(hr))
                wcscpy(szClassName, L"<unknown type def>");

            printf("%S::%S", szClassName, szFieldName);
        }
        break;

    case mdtMemberRef:
        {
            mdTypeRef cr = mdTypeRefNil;
            LPWSTR pMemberName;
            WCHAR memberName[50];
            ULONG memberNameLen;

            hr = i->GetMemberRefProps(token, &cr, memberName, 49,
                                      &memberNameLen, NULL, NULL);

            if (FAILED(hr))
            {
                pMemberName = L"<unknown member ref>";
            }
            else
                pMemberName = memberName;

            ULONG cLen;
            WCHAR szName[50];

            if(TypeFromToken(cr) == mdtTypeRef)
            {
                if (FAILED(i->GetTypeRefProps(cr, NULL, szName, 50, &cLen)))
                {
                    wcscpy(szName, L"<unknown type ref>");
                }
            }
            else if(TypeFromToken(cr) == mdtTypeDef)
            {
                if (FAILED(i->GetTypeDefProps(cr, szName, 49, &cLen,
                                              NULL, NULL)))
                {
                    wcscpy(szName, L"<unknown type def>");
                }
            }
            else if(TypeFromToken(cr) == mdtTypeSpec)
            {
                IMDInternalImport *pIMDI = NULL;
                if (SUCCEEDED(GetMDInternalFromImport(i, &pIMDI)))
                {
                    CQuickBytes out;
                    ULONG cSig;
                    PCCOR_SIGNATURE sig = pIMDI->GetSigFromToken(cr, &cSig);
                    PrettyPrintType(sig, &out, pIMDI);
                    MultiByteToWideChar (CP_ACP, 0, asString(&out), -1, szName, 50);

                    pIMDI->Release();
                }
                else
                {
                    wcscpy(szName, L"<unknown type spec>");
                }
            }
            else
            {
                wcscpy(szName, L"<unknown type token>");
            }
            
            printf("%S::%S ", szName, pMemberName);
        }
        break;
    }
}

ULONG GetILSize(DWORD_PTR ilAddr)
{
    ULONG uRet = 0;


    static BYTE headerArray[1024];
    HRESULT Status = g_ExtData->ReadVirtual(ilAddr, headerArray, sizeof(headerArray), NULL);    
    if (SUCCEEDED(Status))
    {            
        COR_ILMETHOD_DECODER header((COR_ILMETHOD *)headerArray);
        // uRet = header.GetHeaderSize();
        uRet = header.GetOnDiskSize((COR_ILMETHOD *)headerArray);
    }

    return uRet;
}
  
void DecodeIL(IMetaDataImport *pImport, BYTE *buffer, ULONG bufSize)
{
    // First decode the header
    COR_ILMETHOD *pHeader = (COR_ILMETHOD *) buffer;    
    COR_ILMETHOD_DECODER header(pHeader);    

    // Set globals
	position = 0;	
	pBuffer = (BYTE *) header.Code;

    UINT indentCount = 0;
    ULONG endCodePosition = header.GetCodeSize();
	while(position < endCodePosition)
	{	
        for (unsigned e=0;e<header.EHCount();e++)
        {
            IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT ehBuff;
            const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* ehInfo;
            
            ehInfo = header.EH->EHClause(e,&ehBuff);
            if (ehInfo->TryOffset == position)
            {
                printf ("%*s.try\n%*s{\n", indentCount, "", indentCount, "");
                indentCount+=2;
            }
            else if ((ehInfo->TryOffset + ehInfo->TryLength) == position)
            {
                indentCount-=2;
                printf("%*s} // end .try\n", indentCount, "");
            }

            if (ehInfo->HandlerOffset == position)
            {
                if (ehInfo->Flags == COR_ILEXCEPTION_CLAUSE_FINALLY)
                    printf("%*s.finally\n%*s{\n", indentCount, "", indentCount, "");
                else
                    printf("%*s.catch\n%*s{\n", indentCount, "", indentCount, "");

                indentCount+=2;
            }
            else if ((ehInfo->HandlerOffset + ehInfo->HandlerLength) == position)
            {
                indentCount-=2;
                
                if (ehInfo->Flags == COR_ILEXCEPTION_CLAUSE_FINALLY)
                    printf("%*s} // end .finally\n", indentCount, "");
                else
                    printf("%*s} // end .catch\n", indentCount, "");
            }
        }        
        
        printf("%*sIL_%04x: ", indentCount, "", position);
		unsigned int c = readOpcode();
		OpCode opcode = opcodes[c];
		printf("%s ", opcode.name);

		switch(opcode.args)
		{
		case InlineNone: break;
		
		case ShortInlineVar:
            printf("VAR OR ARG %d",readData<BYTE>()); break;
		case InlineVar:
            printf("VAR OR ARG %d",readData<unsigned short>()); break;
		case InlineI:
			printf("%d",readData<long>()); 
			break;
		case InlineR:
			printf("%f",readData<double>());
			break;
		case InlineBrTarget:
            printf("IL_%04x",readData<long>() + position); break;
		case ShortInlineBrTarget:
    		printf("IL_%04x",readData<BYTE>()  + position); break;
		case InlineI8:
            printf("%ld", readData<__int64>()); break;
            
		case InlineMethod:
		case InlineField:
		case InlineType:
        case InlineTok:
        case InlineSig:        
        {
            long l = readData<long>();
            DisassembleToken(pImport, l);
            // printf("TOKEN %x", l); 
            break;
        }
            
		case InlineString:
        {
            long l = readData<long>();

            ULONG numChars;
            wchar_t str[84];

            if (pImport->GetUserString((mdString) l, str, 80, &numChars) == S_OK)
            {
                if (numChars < 80)
                    str[numChars] = 0;
                wcscpy_s(&str[79], 4, L"...");
                wchar_t* ptr = str;
                while(*ptr != 0) {
                    if (*ptr < 0x20 || * ptr >= 0x80) {
                        *ptr = '.';
                    }
                    ptr++;
                }

                printf("\"%S\"", str);
            }
            else
            {
                printf("STRING %x", l); 
            }
            break;
		}
            
		case InlineSwitch:
        {
            long cases = readData<long>();
            long *pArray = new long[cases];            
            long i=0;
            for(i=0;i<cases;i++)
            {
                pArray[i] = readData<long>();
            }
            printf("(");
            for(i=0;i<cases;i++)
            {
                if (i != 0)
                    printf(", ");
                printf("IL_%04x",pArray[i] + position);
            }
            printf(")");
            delete [] pArray;
            break;
		}
        case ShortInlineI:
            printf("%d", readData<char>()); break;
		case ShortInlineR:		
            printf("%f", readData<float>()); break;
		default: printf("Error, unexpected opcode type\n"); break;
		}

        printf("\n");
	}
}

DWORD_PTR GetObj(DacpObjectData& tokenArray, UINT item)
{
    if (item < tokenArray.dwNumComponents)
    {
        DWORD_PTR dwAddr = (DWORD_PTR) tokenArray.ArrayDataPtr + tokenArray.dwComponentSize*item;
        DWORD_PTR objPtr;
        if (SUCCEEDED(MOVE(objPtr,dwAddr)))
        {
            return objPtr;
        }
    }
    return NULL;
}


void DisassembleToken(DacpObjectData& tokenArray,
                      DWORD token)
{    
    switch (TypeFromToken(token))
    {
    default:
        printf("<unknown token type (token=%08x)>", token);
        break;

    case mdtTypeDef:
        {
            DWORD_PTR runtimeMethodHandle = GetObj(tokenArray, RidFromToken(token));

            // The first field is the MethodDesc
            DWORD_PTR methodTableAddr = runtimeMethodHandle + sizeof(DWORD_PTR); // skip methodtable
            DWORD_PTR methodTable;
            MOVE(methodTable, methodTableAddr);
            NameForMT_s (methodTable, g_mdName,mdNameLen);
            printf ("%x \"%S\"", token, g_mdName);
        }
        break;

    case mdtSignature:
    case mdtTypeRef:
        {
            printf ("%x (%p)", token, (ULONG64) GetObj(tokenArray, RidFromToken(token)));
        }
        break;

    case mdtFieldDef:
        {
            printf ("%x (%p)", token, (ULONG64) GetObj(tokenArray, RidFromToken(token)));
        }
        break;

    case mdtMethodDef:
        {
            DWORD_PTR runtimeMethodHandle = GetObj(tokenArray, RidFromToken(token));

            // The first field is the MethodDesc
            DWORD_PTR methodDescAddr = runtimeMethodHandle + sizeof(DWORD_PTR); // skip methodtable
            DWORD_PTR methodDesc;
            MOVE(methodDesc, methodDescAddr);
            NameForMD_s (methodDesc, g_mdName, mdNameLen);
            printf ("%x %S", token, g_mdName);
        }
        break;

    case mdtMemberRef:
        {
            printf ("%x (%p)", token, (ULONG64) GetObj(tokenArray, RidFromToken(token)));
        }
        break;
    case mdtString:
        {
            DWORD_PTR strObj = GetObj(tokenArray, RidFromToken(token));
            printf ("%x \"", token);
            StringObjectContent (strObj, FALSE, 40);
            printf ("\"");
        }
        break;
    }
}

void DecodeDynamicIL(BYTE *data, ULONG Size, DacpObjectData& tokenArray)
{
    // There is no header for this dynamic guy.
    // Set globals
	position = 0;	
	pBuffer = data;

    // At this time no exception information will be displayed (fix soon)
    UINT indentCount = 0;
    ULONG endCodePosition = Size;
	while(position < endCodePosition)
	{	        
        printf("%*sIL_%04x: ", indentCount, "", position);
		unsigned int c = readOpcode();
		OpCode opcode = opcodes[c];
		printf("%s ", opcode.name);

		switch(opcode.args)
		{
		case InlineNone: break;
		
		case ShortInlineVar:
            printf("VAR OR ARG %d",readData<BYTE>()); break;
		case InlineVar:
            printf("VAR OR ARG %d",readData<unsigned short>()); break;
		case InlineI:
			printf("%d",readData<long>()); 
			break;
		case InlineR:
			printf("%f",readData<double>());
			break;
		case InlineBrTarget:
            printf("IL_%04x",readData<long>() + position); break;
		case ShortInlineBrTarget:
    		printf("IL_%04x",readData<BYTE>()  + position); break;
		case InlineI8:
            printf("%ld", readData<__int64>()); break;
            
		case InlineMethod:
		case InlineField:
		case InlineType:
        case InlineTok:
        case InlineSig:        
		case InlineString:            
        {
            long l = readData<long>();  
            DisassembleToken(tokenArray, l);            
            break;
        }
                        
		case InlineSwitch:
        {
            long cases = readData<long>();
            long *pArray = new long[cases];            
            long i=0;
            for(i=0;i<cases;i++)
            {
                pArray[i] = readData<long>();
            }
            printf("(");
            for(i=0;i<cases;i++)
            {
                if (i != 0)
                    printf(", ");
                printf("IL_%04x",pArray[i] + position);
            }
            printf(")");
            delete [] pArray;
            break;
		}
        case ShortInlineI:
            printf("%d", readData<char>()); break;
		case ShortInlineR:		
            printf("%f", readData<float>()); break;
		default: printf("Error, unexpected opcode type\n"); break;
		}

        printf("\n");
	}
}



/******************************************************************************/
// CQuickBytes utilities
char* asString(CQuickBytes *out) {
    SIZE_T oldSize = out->Size();
    out->ReSize(oldSize + 1);
    char* cur = &((char*) out->Ptr())[oldSize]; 
    *cur = 0;   
    out->ReSize(oldSize);   		// Don't count the null character
    return((char*) out->Ptr()); 
}

void appendStr(CQuickBytes *out, const char* str, unsigned len=-1) {
    if(len == (unsigned)(-1)) len = (unsigned)strlen(str); 
    SIZE_T oldSize = out->Size();
    out->ReSize(oldSize + len);
    char* cur = &((char*) out->Ptr())[oldSize]; 
    memcpy(cur, str, len);  
        // Note no trailing null!   
}

void appendChar(CQuickBytes *out, char chr) {
    SIZE_T oldSize = out->Size();
    out->ReSize(oldSize + 1); 
    ((char*) out->Ptr())[oldSize] = chr; 
        // Note no trailing null!   
}

void insertStr(CQuickBytes *out, const char* str) {
    unsigned len = (unsigned)strlen(str); 
    SIZE_T oldSize = out->Size();
    out->ReSize(oldSize + len); 
    char* cur = &((char*) out->Ptr())[len];
	memmove(cur,out->Ptr(),oldSize);
    memcpy(out->Ptr(), str, len);  
        // Note no trailing null!   
}

static void appendStrNum(CQuickBytes *out, int num) {
    char buff[16];  
    sprintf_s(buff, COUNTOF(buff), "%d", num);   
    appendStr(out, buff);   
}


//PrettyPrinting type names
PCCOR_SIGNATURE PrettyPrintType(
    PCCOR_SIGNATURE typePtr,            // type to convert,     
    CQuickBytes *out,                   // where to put the pretty printed string   
    IMDInternalImport *pIMDI)           // ptr to IMDInternal class with ComSig
{
    mdToken  tk;    
    const char* str;    
	//bool isValueArray;
	int typ;
	CQuickBytes tmp;
	CQuickBytes Appendix;
	BOOL Reiterate;
    int n;

	do {
		Reiterate = FALSE;
		switch(typ = *typePtr++) {    
			case ELEMENT_TYPE_VOID          :   
				str = "void"; goto APPEND;  
			case ELEMENT_TYPE_BOOLEAN       :   
				str = "bool"; goto APPEND;  
			case ELEMENT_TYPE_CHAR          :   
				str = "char"; goto APPEND; 
			case ELEMENT_TYPE_I1            :   
				str = "int8"; goto APPEND;  
			case ELEMENT_TYPE_U1            :   
				str = "uint8"; goto APPEND; 
			case ELEMENT_TYPE_I2            :   
				str = "int16"; goto APPEND; 
			case ELEMENT_TYPE_U2            :   
				str = "uint16"; goto APPEND;    
			case ELEMENT_TYPE_I4            :   
				str = "int32"; goto APPEND; 
			case ELEMENT_TYPE_U4            :   
				str = "uint32"; goto APPEND;    
			case ELEMENT_TYPE_I8            :   
				str = "int64"; goto APPEND; 
			case ELEMENT_TYPE_U8            :   
				str = "uint64"; goto APPEND;    
			case ELEMENT_TYPE_R4            :   
				str = "float32"; goto APPEND;   
			case ELEMENT_TYPE_R8            :   
				str = "float64"; goto APPEND;   
			case ELEMENT_TYPE_U             :   
				str = "native uint"; goto APPEND;   
			case ELEMENT_TYPE_I             :   
				str = "native int"; goto APPEND;    
			case ELEMENT_TYPE_R             :   
				str = "native float"; goto APPEND;  
			case ELEMENT_TYPE_OBJECT        :   
				str = "object"; goto APPEND;    
			case ELEMENT_TYPE_STRING        :   
				str = "string"; goto APPEND;    
			case ELEMENT_TYPE_TYPEDBYREF        :   
				str = "typedref"; goto APPEND;    
			APPEND: 
				appendStr(out, (char*)str);
				break;  

			case ELEMENT_TYPE_VALUETYPE    :   
				str = "valuetype ";   
				goto DO_CLASS;  
			case ELEMENT_TYPE_CLASS         :   
				str = "class "; 
				goto DO_CLASS;  

			DO_CLASS:
				appendStr(out, (char*)str);
				typePtr += CorSigUncompressToken(typePtr, &tk); 
                if(IsNilToken(tk))
				{
					appendStr(out, "[ERROR! NIL TOKEN]");
				}
				else PrettyPrintClass(out, tk, pIMDI);
				break;  

			case ELEMENT_TYPE_SZARRAY    :   
				insertStr(&Appendix,"[]");
				Reiterate = TRUE;
				break;
			case ELEMENT_TYPE_ARRAY       :   
				{   
				typePtr = PrettyPrintType(typePtr, out, pIMDI);
				unsigned rank = CorSigUncompressData(typePtr);  
				if (rank == 0) {
					appendStr(out, "[BAD: RANK == 0!]");
				}
				else {
					_ASSERTE(rank != 0);
					int* lowerBounds = (int*) _alloca(sizeof(int)*2*rank);
					int* sizes       = &lowerBounds[rank];  
					memset(lowerBounds, 0, sizeof(int)*2*rank); 
					
					unsigned numSizes = CorSigUncompressData(typePtr);  
					_ASSERTE(numSizes <= rank);
				    unsigned i;
					for(i =0; i < numSizes; i++)
						sizes[i] = CorSigUncompressData(typePtr);   
					
					unsigned numLowBounds = CorSigUncompressData(typePtr);  
					_ASSERTE(numLowBounds <= rank); 
					for(i = 0; i < numLowBounds; i++)   
						typePtr+=CorSigUncompressSignedInt(typePtr,&lowerBounds[i]); 
					
					appendChar(out, '[');    
					if (rank == 1 && numSizes == 0 && numLowBounds == 0)
						appendStr(out, "...");  
					else {
						for(i = 0; i < rank; i++)   
						{   
							//if (sizes[i] != 0 || lowerBounds[i] != 0)   
							{   
								if (lowerBounds[i] == 0 && i < numSizes)    
									appendStrNum(out, sizes[i]);    
								else    
								{   
									if(i < numLowBounds)
									{
										appendStrNum(out, lowerBounds[i]);  
										appendStr(out, "...");  
										if (/*sizes[i] != 0 && */i < numSizes)  
											appendStrNum(out, lowerBounds[i] + sizes[i] - 1);   
									}
								}   
							}   
							if (i < rank-1) 
                                appendChar(out, ',');    
						}   
					}
					appendChar(out, ']'); 
				}
				} break;    

			case ELEMENT_TYPE_VAR        :   
                appendChar(out, '!');
                n  = CorSigUncompressData(typePtr);
  				appendStrNum(out, n);
				break;

			case ELEMENT_TYPE_MVAR        :   
                appendChar(out, '!');    
                appendChar(out, '!');    
                n  = CorSigUncompressData(typePtr);
   				appendStrNum(out, n);
				break;

			case ELEMENT_TYPE_FNPTR :   
				appendStr(out, "method ");  
				appendStr(out, "METHOD"); // was: typePtr = PrettyPrintSignature(typePtr, 0x7FFF, "*", out, pIMDI, NULL);
				break;

            case ELEMENT_TYPE_GENERICINST :
            {
			  typePtr = PrettyPrintType(typePtr, out, pIMDI);
              appendStr(out, "<");    
			  unsigned numArgs = CorSigUncompressData(typePtr);    
			  bool needComma = false;
			  while(numArgs--)
              {
			      if (needComma)
                      appendChar(out, ',');
			      typePtr = PrettyPrintType(typePtr, out, pIMDI);
			      needComma = true;
			  }
              appendStr(out, ">");
			  break;
			}

			case ELEMENT_TYPE_PINNED	:
				str = " pinned"; goto MODIFIER;
			case ELEMENT_TYPE_PTR           :
				str = "*"; goto MODIFIER;
			case ELEMENT_TYPE_BYREF         :
				str = "&"; goto MODIFIER;
			MODIFIER:
				insertStr(&Appendix, str);
				Reiterate = TRUE;
				break;  

			default:    
			case ELEMENT_TYPE_SENTINEL      :   
			case ELEMENT_TYPE_END           :   
				//_ASSERTE(!"Unknown Type");
				if(typ)
				{
					char sz[64];
					sprintf_s(sz,COUNTOF(sz),"/* UNKNOWN TYPE (0x%X)*/",typ);
					appendStr(out, sz);
				}
				break;  
		} // end switch
	} while(Reiterate);
	if (Appendix.Size() > 0)
		appendStr(out,asString(&Appendix));

    return(typePtr);    
}

// Protection against null names, used by ILDASM/SOS
char* szStdNamePrefix[] = {"MO","TR","TD","","FD","","MD","","PA","II","MR","","CA","","PE","","","SG","","","EV",
"","","PR","","","MOR","TS","","","","","AS","","","AR","","","FL","ET","MAR"};

#define MAKE_NAME_IF_NONE(psz, tk) { if(!(psz && *psz)) { char* sz = (char*)_alloca(16); \
sprintf_s(sz,16,"$%s$%X",szStdNamePrefix[tk>>24],tk&0x00FFFFFF); psz = sz; } }

const char* PrettyPrintClass(
    CQuickBytes *out,                   // where to put the pretty printed string   
	mdToken tk,					 		// The class token to look up 
    IMDInternalImport *pIMDI)           // ptr to IMDInternalImport class with ComSig
{
    if(tk == mdTokenNil)  // Zero resolution scope for "somewhere here" TypeRefs
    {
		appendStr(out,"[*]");
		return(asString(out));
    }
	if(!pIMDI->IsValidToken(tk))
	{
		char str[1024];
		sprintf_s(str,COUNTOF(str)," [ERROR: INVALID TOKEN 0x%8.8X] ",tk);
		appendStr(out, str);
		return(asString(out));
	}
	switch(TypeFromToken(tk))
	{
		case mdtTypeRef:
        case mdtTypeDef:
			{
				const char *nameSpace = 0;  
				const char *name = 0;
				mdToken tkEncloser;
				
				if (TypeFromToken(tk) == mdtTypeRef)
				{
					tkEncloser = pIMDI->GetResolutionScopeOfTypeRef(tk);
					pIMDI->GetNameOfTypeRef(tk, &nameSpace, &name);
				}
				else 
				{
					if(FAILED(pIMDI->GetNestedClassProps(tk,&tkEncloser))) tkEncloser = mdTypeDefNil;
					pIMDI->GetNameOfTypeDef(tk, &name, &nameSpace);
				}
				MAKE_NAME_IF_NONE(name,tk);
				if((tkEncloser == mdTokenNil) || RidFromToken(tkEncloser))
				{
					PrettyPrintClass(out,tkEncloser,pIMDI);
					if (TypeFromToken(tkEncloser) == mdtTypeRef || TypeFromToken(tkEncloser) == mdtTypeDef)
					{
                        appendChar(out, '/');    
						//nameSpace = ""; //don't print namespaces for nested classes!
					}
				}
				if(TypeFromToken(tk)==mdtTypeDef)
				{
					unsigned L = (unsigned)strlen(name)+1;
					char* szFN = NULL;
					if(nameSpace && *nameSpace)
					{
						const char* sz = nameSpace;
						L+= (unsigned)strlen(sz)+1;
						szFN = new char[L];
						sprintf_s(szFN,L,"%s.",sz);
					}
					else
					{
						szFN = new char[L];
						*szFN = 0;
					}
					strcat_s(szFN,L, name);
					appendStr(out, szFN);
					if (szFN) delete[] (szFN);
				}
				else
				{
					if (nameSpace && *nameSpace) {
						appendStr(out, nameSpace);  
                        appendChar(out, '.');    
					}

					appendStr(out, name);
				}
			}
			break;

		case mdtAssemblyRef:
			{
				LPCSTR	szName = NULL;
                pIMDI->GetAssemblyRefProps(tk,NULL,NULL,&szName,NULL,NULL,NULL,NULL);
				if(szName && *szName)
				{
                    appendChar(out, '[');    
					appendStr(out, szName);
                    appendChar(out, ']');    
				}
			}
			break;
		case mdtAssembly:
			{
				LPCSTR	szName = NULL;
				pIMDI->GetAssemblyProps(tk,NULL,NULL,NULL,&szName,NULL,NULL);
				if(szName && *szName)
				{
                    appendChar(out, '[');    
					appendStr(out, szName);
                    appendChar(out, ']');    
				}
			}
			break;
		case mdtModuleRef:
			{
				LPCSTR	szName = NULL;
				pIMDI->GetModuleRefProps(tk,&szName);
				if(szName && *szName)
				{
                    appendChar(out, '[');    
					appendStr(out, ".module ");
					appendStr(out, szName);
                    appendChar(out, ']');    
				}
			}
			break;

		case mdtTypeSpec:
			{
                ULONG cSig;
                PCCOR_SIGNATURE sig = pIMDI->GetSigFromToken(tk, &cSig);
                PrettyPrintType(sig, out, pIMDI);
			}
            break;

        case mdtModule:
            break;
		
        default:
			{
				char str[128];
				sprintf_s(str,COUNTOF(str)," [ERROR: INVALID TOKEN TYPE 0x%8.8X] ",tk);
				appendStr(out, str);
			}
	}
	return(asString(out));
}

