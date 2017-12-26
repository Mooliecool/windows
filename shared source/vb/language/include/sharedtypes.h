
//-------------------------------------------------------------------------------------------------
//
// Includes types that are 
//   1) Independent
//   2) Used by multpile components in their interfaces
// This is essentially a forward declaration of non-pointer types that are used accross the various
// component interfaces
//

typedef WCHAR STRING;
typedef unsigned __int64 NameFlags;

#include "..\Compiler\Symbols\SymbolTypes.h"

// Symbol Forward Declarations
class   BCSYM;
class   BCSYM_CCContainer;
class   BCSYM_Container;
class   BCSYM_ExtensionCallLookupResult;
class   BCSYM_GenericBinding;
class   BCSYM_GenericParam;
class   BCSYM_GenericTypeBinding;
class   BCSYM_Hash;
class   BCSYM_NamedRoot;
class   BCSYM_NamespaceRing;
class   BCSYM_Namespace;
typedef BCSYM Symbol;
typedef BCSYM Type;

// Biltree Forward Declarations
namespace ILTree
{
	class   ILNode;
	struct  AttributeApplicationExpression;
	struct  Expression;
	struct  SymbolReferenceExpression;
	struct  ProcedureBlock;
}

class   BITSET;

struct  BlockScope;
struct  CODE_BLOCK;
class   CompilationCaches;
class   Compiler;
class   CompilerFile;
class   CompilerHost;
class   CompilerPackage;
class   CompilerProject;
class   CompilerTaskProvider;
struct  ExtensionMethodLookupCacheEntry;
class   GenericBindingCache;
struct  Location;
class   MetaEmit;
class   NorlsAllocator;
enum    PlatformKinds;
struct  Resource;
enum    RuntimeVersion;
struct  SecAttrInfo;
class   SourceFile;
class   SourceFileView;
class   StringBuffer;
class   StringPool;
struct  Symbols;
struct  SWITCH_TABLE;
struct  Temporary;
class   TemporaryManager;
class   ThreadSyncManager;
struct  Token;



namespace ParseTree
{
    struct PunctuatorLocation;
};

