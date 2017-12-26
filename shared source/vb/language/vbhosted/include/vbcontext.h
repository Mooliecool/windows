#pragma once

#import "mscorlib.tlb" raw_interfaces_only raw_native_types named_guids rename("_Module", "_ReflectionModule") rename("ReportEvent", "_ReflectionReportEvent") rename("value", "_value")
#include "vcclr.h"
#include "CompilerContext.h"

class VbContext
#if FV_DEADBEEF
    : public Deadbeef<VbContext> // Must be last base class!
#endif
{
public:
    VbContext(gcroot<Microsoft::Compiler::VisualBasic::CompilerContext ^> compilerContext = gcroot<Microsoft::Compiler::VisualBasic::CompilerContext ^>());

    virtual ~VbContext();

    static STDMETHODIMP SetAssemblyRefs(/*[in]*/ gcroot<System::Collections::Generic::IList<System::Reflection::Assembly ^> ^> referenceAssemblies, /*[in]*/ class Compiler *pCompiler, /*[in]*/ struct IVbCompilerProject *pVbCompilerProject, /*[in]*/ ErrorTable *pErrorTable);
    STDMETHODIMP SetImports(/*[in]*/ struct IVbCompilerProject *pVbCompilerProject);
    STDMETHODIMP NamespaceExists(/*[in]*/ BSTR name, /*[out,retval]*/ BOOL *pRetVal);
    STDMETHODIMP FindTypes(/*[in]*/ BSTR name,
                           /*[in]*/ BSTR prefix,
                           /*[out,retval]*/ gcroot<array<System::Type^,1>^>& pType );
    STDMETHODIMP FindVariable(/*[in]*/ BSTR name, 
                              /*[out, retval]*/ gcroot<System::Type^>& pType);
    STDMETHODIMP SetOptions(/*[in]*/ struct IVbCompilerProject *pVbCompilerProject);
    static STDMETHODIMP SetDefaultOptions(/*[in]*/ struct IVbCompilerProject *pVbCompilerProject);
    STDMETHODIMP SetOptions(/*[in]*/ SourceFile *pSourceFile);

private:
    static STDMETHODIMP SetOptions(/*[in]*/ struct IVbCompilerProject *pVbCompilerProject, /*[in]*/ gcroot<Microsoft::Compiler::VisualBasic::CompilerOptions ^> compilerOptions);
    gcroot<Microsoft::Compiler::VisualBasic::CompilerContext ^> m_compilerContext;

    // Do not want to support copy semantics for this class, so 
    // declare as private and don't implement...
    VbContext(const VbContext &source);
    VbContext &operator=(const VbContext &source);
};
