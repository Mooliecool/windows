//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Precompiled header to include all compiler API-s.
//
//-------------------------------------------------------------------------------------------------

#pragma once

//-------------------------------------------------------------------------------------------------
//
// We commonly use mdToken as a key in a DynamicFixedSizeHashTable.  This type requires keys
// be a pointer multiple size so on 64 bit we use size_t.  This creates a lot of un-readable 
// #if/#else.  Instead we will switch to using a typedef'd type
//
//-------------------------------------------------------------------------------------------------
typedef size_t mdTokenKey;

CompilerPackage *GetCompilerPackage(_In_opt_ bool fAssertFromMainThread = true);

void DebCheckNoBackgroundThreads(Compiler * pCompiler);
void CheckInMainThread();

DWORD GetStateTlsIndex();

#if IDE 
typedef SafeCriticalSection CompilerIdeCriticalSection;
#define IDE_CODE(arg) arg
#define IDE_ARG(arg) ,arg
#else
typedef CComFakeCriticalSection CompilerIdeCriticalSection;
#define IDE_CODE(arg)
#define IDE_ARG(arg)
#endif

typedef CComCritSecLock<CompilerIdeCriticalSection> CompilerIdeLock;

// CALG_MD5 hash size used in PEBuilder and TextFile
#define CRYPT_HASHSIZE 16

// Resources
#include "..\Compiler\Resources.h"
#include "..\Compiler\StringConstants.h"
#include "..\Compiler\TlsValue.h"
#include "..\Compiler\VbCompilerMacros.h"

#if IDE
#include "..\Compiler\MenuIds.h"
#endif // IDE

// General templates, diagnostics, utilities
#include "..\Compiler\CompilerIdeTransientLock.h"
#include "..\Compiler\CorEnum.h"
#include "..\Compiler\HResult.h"
#include "..\Compiler\HResultInfo.h"
#include "..\Compiler\Templates.h"
#include "..\Compiler\LinkedLists.h"
#include "..\Compiler\TreeTemplates.h"
#include "..\Compiler\GraphTemplates.h"
#include "..\Compiler\StringPoolEntry.h"
#include "..\Compiler\StringBuffer.h"
#include "..\Compiler\Graph.h"
#include "..\Compiler\FastStr.h"
#include "..\Compiler\AutoFastStr.h"
#include "..\Compiler\AutoFastBstr.h"

#if IDE 
#include "..\VsPackage\Threading.h"
#include "..\VsPackage\SnapshotColorInfoBase.h"
#include "..\VsPackage\SnapshotColorInfo.h"
#include "..\VsPackage\CurrentSnapshotColorInfo.h"
#include "..\VsPackage\BufferColorStateManager.h"
#endif // IDE

#include "..\Compiler\WerExceptionReport.h"
#include "..\Compiler\SequentialNameGenerator.h"
#include "..\Compiler\logging.h"

// Lexical and syntax analysis
#include "..\Compiler\Types.h"

#include "..\Compiler\Scanner\Text.h"
#include "..\Compiler\LineMarkerTable.h"
#include "..\Compiler\Scanner\KeywordTable.h"
#include "..\Compiler\StringPool.h"
#include "..\Compiler\Scanner\scanner.h"
#include "..\Compiler\Scanner\XmlCharacter.h"
#include "..\Compiler\TreeHelpers.h"
#include "..\Compiler\Errors.h"
#include "..\Compiler\Parser\ParseTreeAccessorMacros.h"
#include "..\Compiler\Parser\ParseTrees.h"
#include "..\Compiler\Parser\ParserHelper.h"
#include "..\Compiler\Parser\ParseTreeSearcher.h"
#include "..\Compiler\Parser\SimpleParseTreeVisitor.h"
#include "..\Compiler\Parser\AutoParseTreeVisitor.h"
#include "..\Compiler\Parser\Parser.h"

#include "..\Compiler\Parser\ParseTreeHelpers.h"

#include "..\Compiler\Parser\ParseTreeVisitor.h"
#include "..\Compiler\Parser\LocationFixupVisitor.h"
#include "..\Compiler\MapFile.h"

// Symbols
#include "..\Compiler\Symbols\SymbolTable.h"
#include "..\Compiler\Symbols\Symbols.h"
#include "..\Compiler\Symbols\FxSymbolProvider.h"

// Semantic analysis and code gen
#include "..\Compiler\Binder\Cycles.h"
#include "..\Compiler\SemanticAnalyzer\BoundTrees.h"
#include "..\Compiler\SemanticAnalyzer\BoundTreeAllocator.h"
#include "..\Compiler\SemanticAnalyzer\DumpBoundTree.h"
#include "..\Compiler\SemanticAnalyzer\TemporaryManager.h"
#include "..\Compiler\Symbols\TransientSymbol.h"
#include "..\Compiler\Symbols\Declared.h"
#include "..\Compiler\SemanticAnalyzer\XmlSemantics.h"
#include "..\Compiler\Binder\ObsoleteChecker.h"
#include "..\Compiler\NameHelpers.h"
#include "..\Compiler\RuntimeHelpers.h"
#include "..\Compiler\CodeGen\EncDebugInfo.h"
#include "..\Compiler\XmlCommentStream.h"
#include "..\Compiler\XmlTagTypes.h"
#include "..\Compiler\AssemblyIdentity.h"
#include "..\Compiler\TextFile.h"
#include "..\Compiler\Codefile.h"
#include "..\Compiler\langversion.h"

#if IDE 
#include "..\VsPackage\XmlDocParser.h"
#include "..\VsPackage\WaitStateGuard.h"
#endif // IDE

#include "..\Compiler\XmlDocFile.h"
#include "..\Compiler\CompilerFile.h"
#include "..\Compiler\SourceFile.h"
#include "..\Compiler\MetaDataFile.h"
#include "..\Compiler\CodeGen\PEBuilder.h"

#if IDE 
#include "..\VsPackage\IDEExtensionMethodCache.h" // needed by CompilerPackage.h
#include "..\VsPackage\IdeBoundMethodDataCacheManager.h" //needed by CompilerProject.h
#endif  // IDE

#include "..\Compiler\CompilerProject.h"
#include "..\Compiler\Symbols\MetaImport.h"
#include "..\Compiler\TypeName.h"
#include "..\Compiler\TypeNameBuilder.h"
#include "..\Compiler\Attributes.h"

#if IDE 
#include "..\VsPackage\WaitEvents.h" // needed by compiler.h
#include "..\VsPackage\background.h" // needed by compiler.h
#endif // IDE

#include "..\Compiler\Compiler.h"
#include "..\Compiler\CodeGen\MetaEmit.h"
#include "..\Compiler\CLSComplianceChecker.h"
#include "..\Compiler\UpdateProjectBeingCompiled.h"

#if IDE
#include "UIThreadInvoker.h"
#include "..\Compiler\CodeGen\WritePEHelper.h"
#endif
#if IDE 
class  CBaseCompletionSet;
class  CVBViewFilterBase;
enum   CompletionSetFilterMode;
struct LambdaParentInfo;
#include "..\VsPackage\EditFilter.h"
#include "..\VsPackage\GlobalCache.h"
#include "..\VsPackage\Background.h"
#include "..\VsPackage\Messages.h"
#include "..\VsPackage\optdlg.h"
#include "..\VsPackage\SymbolLocatorResult.h"
#include "..\VsPackage\HighlightReferencesInfo.h"
#include "..\VsPackage\IntellisenseDefines.h"
#include "..\VsPackage\cmpinfo.h"
#endif

#if IDE
#include "..\VsPackage\FixedSizeCache.h"
#include "..\VsPackage\TextData.h"
#include "..\VsPackage\DataPool.h"
#include "..\VsPackage\TextDataPool.h"
#include "..\VsPackage\ParseTreeCacheData.h"
#include "..\VsPackage\ParseTreeCacheDataPool.h"
#include "..\VsPackage\SnippetMarkers.h"
#if !defined( VBDBGEE ) 
#include "..\VsPackage\compilertesthooks.h"
#endif // !defined( VBDBGEE ) 
#endif // IDE

#if IDE 
#import "Microsoft.VisualStudio.VisualBasic.LanguageService.tlb" named_guids no_smart_pointers raw_interfaces_only no_namespace
#include "..\VsPackage\compilersharedstate.h"
#include "..\VsPackage\DecompileQueue.h"
#include "..\VsPackage\compilerpackage.h"
#endif // IDE

#include "..\Compiler\CompilerUtilities.h"
#include "..\Compiler\CodeGen\CodeGenerator.h"
#include "..\Compiler\Iterators.h"
#include "..\Compiler\XMLGen.h"
#include "..\Compiler\Symbols\TypeHelpers.h"
#include "..\Compiler\SemanticAnalyzer\Semantics.h"
#include "..\Compiler\SemanticAnalyzer\BoundTreeVisitor.h"
#include "..\Compiler\SemanticAnalyzer\Closures.h"
#include "..\Compiler\Binder\Bindable.h"
#include "..\Compiler\VbErrorWrapper.h"

#if IDE 
#include "..\VsPackage\VBComModule.h"
extern VBComModule _Module;
#endif // IDE

