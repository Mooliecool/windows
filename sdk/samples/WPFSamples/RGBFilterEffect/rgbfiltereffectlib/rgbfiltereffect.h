#pragma once
#include "resource.h"
#include "RGBFilterEffectLib.h"

//+--------------------------------------------------------------------------
//
//  Abstract:
//     CRGBFilterEffect
// 
//    <<TODO: Place a description of what your effect does here >>
//
//----------------------------------------------------------------------------
class ATL_NO_VTABLE CRGBFilterEffect :
    public CComObjectRoot,
    public CComCoClass<CRGBFilterEffect, &CLSID_RGBFilterEffect>,
    public IMILBitmapEffectPrimitive,
    public IMILBitmapEffectPrimitiveImpl,
    public IPropertyBag2,
    public IMILBitmapEffectConnectionsInfo,
    public IMILBitmapEffectConnectorInfo
{
public:
    DECLARE_REGISTRY_RESOURCEID(IDR_RGBFILTEREFFECT)
        
    DECLARE_PROTECT_FINAL_CONSTRUCT();
    
    BEGIN_COM_MAP(CRGBFilterEffect)
        COM_INTERFACE_ENTRY(IMILBitmapEffectPrimitive)
        COM_INTERFACE_ENTRY(IMILBitmapEffectPrimitiveImpl)
        COM_INTERFACE_ENTRY(IPropertyBag2)
        COM_INTERFACE_ENTRY(IMILBitmapEffectConnectionsInfo)
        COM_INTERFACE_ENTRY(IMILBitmapEffectConnectorInfo)
    END_COM_MAP();

    DECLARE_AGGREGATABLE(CRGBFilterEffect)

    HRESULT FinalConstruct();
      
    CRGBFilterEffect();
    virtual ~CRGBFilterEffect();

    //**********************************************************************
    // IMILBitmapEffectPrimitive methods
    //**********************************************************************
    STDMETHODIMP GetOutput(ULONG uiIndex,
                           IMILBitmapEffectRenderContext *pContext,
                           VARIANT_BOOL *pfModifyInPlace,
                           IWICBitmapSource **ppBitmapSource);
    STDMETHODIMP TransformPoint(ULONG uiIndex,
                                MIL_2DPOINTD *p,
                                VARIANT_BOOL fForwardTransform,
                                IMILBitmapEffectRenderContext *pContext,
                                VARIANT_BOOL *pfPointTransformed);
    STDMETHODIMP TransformRect(ULONG uiIndex,
                               MIL_RECTD *p,
                               VARIANT_BOOL fForwardTransform,
                               IMILBitmapEffectRenderContext *pContext);
    STDMETHODIMP HasAffineTransform(ULONG uiIndex, 
                                    VARIANT_BOOL *pfAffine);
    STDMETHODIMP HasInverseTransform(ULONG uiIndex, 
                                     VARIANT_BOOL *pfHasInverse);
    STDMETHODIMP GetAffineMatrix(ULONG uiIndex, 
                                 MIL_MATRIX3X2D *pMatrix);
    
    //**********************************************************************
    // IMILBitmapEffectPrimitiveImpl methods
    //**********************************************************************
    STDMETHODIMP IsDirty(ULONG uiOutputIndex, VARIANT_BOOL *pfDirty);
    STDMETHODIMP IsVolatile(ULONG uiOutputIndex, VARIANT_BOOL *pfVolatile);
    STDMETHODIMP GetOutputBounds(ULONG uiIndex, IMILBitmapEffectRenderContextImpl *pRenderContext, MIL_RECTD *pRect);

    //**********************************************************************
    // IPropertyBag2 methods
    //**********************************************************************
    STDMETHODIMP CountProperties(ULONG *pcProperties);
    STDMETHODIMP GetPropertyInfo(ULONG iProperty, ULONG cProperties, PROPBAG2 *pPropBag, ULONG *pcProperties);
    STDMETHODIMP LoadObject(LPCOLESTR pstrName, DWORD dwHint, IUnknown *pUnkObject, IErrorLog *pErrLog);
    STDMETHODIMP Read(ULONG cProperties, PROPBAG2 *pPropBag, IErrorLog *pErrLog, VARIANT *pvarValue, HRESULT *phrError);
    STDMETHODIMP Write(ULONG cProperties, PROPBAG2 *pPropBag, VARIANT *pvarValue);
    
    //**********************************************************************
    // IMILBitmapEffectConnectionsInfo
    //**********************************************************************
    STDMETHODIMP GetNumberInputs(ULONG *puiNumInputs);
    STDMETHODIMP GetNumberOutputs(ULONG *puiNumOutputs);
    STDMETHODIMP GetInputConnectorInfo(ULONG uiIndex, IMILBitmapEffectConnectorInfo **ppConnectorInfo);
    STDMETHODIMP GetOutputConnectorInfo(ULONG uiIndex, IMILBitmapEffectConnectorInfo **ppConnectorInfo);

    //**********************************************************************
    // IMILBitmapEffectConnectorInfo
    //**********************************************************************
    STDMETHODIMP GetIndex(ULONG *puiIndex);
    STDMETHODIMP GetOptimalFormat(WICPixelFormatGUID *pFormat);
    STDMETHODIMP GetNumberFormats(ULONG *pulNumberFormats);
    STDMETHODIMP GetFormat(ULONG ulIndex, WICPixelFormatGUID *pFormat);
    
private:
    double m_dRed;
	double m_dGreen;
	double m_dBlue;
};

OBJECT_ENTRY_AUTO(__uuidof(RGBFilterEffect), CRGBFilterEffect)
extern const WCHAR *c_pwcsRed;
extern const WCHAR *c_pwcsGreen;
extern const WCHAR *c_pwcsBlue;