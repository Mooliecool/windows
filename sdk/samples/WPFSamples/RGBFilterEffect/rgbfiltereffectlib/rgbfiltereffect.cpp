#include "stdafx.h"
#include "RGBFilterEffect.h"

// <<TODO: Update this variable with the number of properties your effect exposes >>
const ULONG c_ulNumberProperties = 3; // Number of properties this effect exposes
const WCHAR *c_pwcsRed = L"Red";
const WCHAR *c_pwcsGreen = L"Green";
const WCHAR *c_pwcsBlue = L"Blue";

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::CRGBFilterEffect
//
//  Synopsis:
//      Ctor
//
//----------------------------------------------------------------------------
CRGBFilterEffect::CRGBFilterEffect()
{
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::~CRGBFilterEffect
//
//  Synopsis:
//      dtor
//
//----------------------------------------------------------------------------
CRGBFilterEffect::~CRGBFilterEffect()
{
	m_dRed = 0; //default to 0.
	m_dGreen = 0; //default to 0.
	m_dBlue = 0; //default to 0.
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::FinalConstruct
//
//  Synopsis:
//
//----------------------------------------------------------------------------
HRESULT CRGBFilterEffect::FinalConstruct()
{
    // <<TODO: Add any initialization code you may need here >>
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetOutput
//
//  Synopsis:
//      This method is what does the actual pixel processing for the effect.
//      The caller will be requesting the output for some output pin on
//      the effect (uiIndex, which is normally == 0 for an effect with 1 output).
//      The passed in render context contains information about how the effect
//      should render (see interface for details). pfModifyInPlace will indicate
//      whether the effect should attempt to modify the input image in place.
//      If this is VARIANT_TRUE then you can just write the output pixels over
//      the input image and return the input image. If your effect can't operate
//      in-place then return VARIANT_FALSE through this parameter to indicate
//      that a new image was created.
//
// Arguments:
//      uiIndex - zero-based index of output pin whose output is desired
//      pRenderContext - the render context to use
//      pfModifyInPlace - On input, indicates whether the effect should operate
//          in place. On output, indicates whether the effect did the operation
//          in place.
//      ppBitmapSource - returns the output from the effect.
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::GetOutput(ULONG uiIndex,
                                      IMILBitmapEffectRenderContext *pRenderContext,
                                      VARIANT_BOOL *pfModifyInPlace,
                                      IWICBitmapSource **ppBitmapSource)
{
    // <<TODO: Here is where you would do the pixel processing for your effect >>
    assert(uiIndex == 0);
    assert(ppBitmapSource);
    assert(pRenderContext);
    assert(pfModifyInPlace);
    if (pRenderContext == NULL || ppBitmapSource == NULL || pfModifyInPlace == NULL)
    {
        return E_POINTER;
    }
    
    *ppBitmapSource = NULL;
    
    HRESULT hr = S_OK;
    
    //**********************************************************************
    // Get the object that is aggregating our effect. Every effect is wrapped
    // by an object that implements IMILBitmapEffect.
    //**********************************************************************
    CComPtr<IMILBitmapEffect> spOuterEffect;
    if (SUCCEEDED(hr))
    {
        hr = QueryInterface(__uuidof(IMILBitmapEffect), reinterpret_cast<void**>(&spOuterEffect));
    }
    
    CComPtr<IMILBitmapEffectImpl> spOuterEffectImpl;
    if (SUCCEEDED(hr))
    {
        hr = QueryInterface(__uuidof(IMILBitmapEffectImpl), reinterpret_cast<void**>(&spOuterEffectImpl));
    }
    
    //**********************************************************************
    // Ask the aggregator to get our input source
    //**********************************************************************
    CComPtr<IWICBitmapSource> spInputSource;
    if (SUCCEEDED(hr))
    {
        VARIANT_BOOL vbModifyInPlace = *pfModifyInPlace;
        hr = spOuterEffectImpl->GetInputBitmapSource(0, pRenderContext, &vbModifyInPlace, &spInputSource);
    }
    
    //**********************************************************************
    // Get our render context's implementation interface
    //**********************************************************************
    CComPtr<IMILBitmapEffectRenderContextImpl> spRenderContextImpl;
    if (SUCCEEDED(hr))
    {
        hr = pRenderContext->QueryInterface(__uuidof(IMILBitmapEffectRenderContextImpl),
                                            reinterpret_cast<void**>(&spRenderContextImpl));
    }

    //**********************************************************************
    // Create our imaging factory
    //**********************************************************************
    CComPtr<IWICImagingFactory> spImagingFactory;
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory),
                              reinterpret_cast<void**>(&spImagingFactory));
    }
    
    //**********************************************************************
    // Make sure our input image is a IWICBitmap so we can access the pixels
    //**********************************************************************
    CComPtr<IWICBitmap> spSrcBitmap;
    if (SUCCEEDED(hr))
    {
        hr = spInputSource->QueryInterface(__uuidof(IWICBitmap),
                                           reinterpret_cast<void**>(&spSrcBitmap));
    }
    if (FAILED(hr) && hr == E_NOINTERFACE)
    {
        hr = spImagingFactory->CreateBitmapFromSource(spInputSource, WICBitmapNoCache, &spSrcBitmap);
    }
    
    //**********************************************************************
    // Determine the size of our input image
    //**********************************************************************
    UINT uiInputWidth = 0;
    UINT uiInputHeight = 0;
    if (SUCCEEDED(hr))
    {
        hr = spSrcBitmap->GetSize(&uiInputWidth, &uiInputHeight);
    }
    UINT uiOutputWidth = uiInputWidth;
    UINT uiOutputHeight = uiInputHeight;
    
    //**********************************************************************
    // Create our output image
    //**********************************************************************
    WICPixelFormatGUID pixelFormat = GUID_WICPixelFormatUndefined;
    if (SUCCEEDED(hr))
    {
        hr = spSrcBitmap->GetPixelFormat(&pixelFormat);
    }
    CComPtr<IWICBitmap> spDstBitmap;
    if (SUCCEEDED(hr))
    {
        hr = spImagingFactory->CreateBitmap(uiOutputWidth, uiOutputHeight, pixelFormat,
                                            WICBitmapCacheOnLoad, &spDstBitmap);
    }

    //**********************************************************************
    // Update the transform on the RenderContext. This is necessary if
    // the output image is a different size than the input image. Without
    // updating the transformation matrix on the render context, the output
    // effect may not be centered properly on whatever the effect is being
    // applied to.
    //**********************************************************************
    if (SUCCEEDED(hr) &&
        (uiOutputWidth != uiInputWidth ||
         uiOutputHeight != uiInputHeight))
    {
        MILMatrixF matrix;
        hr = spRenderContextImpl->GetTransform(&matrix);
        if (SUCCEEDED(hr))
        {
            matrix._31 -= static_cast<FLOAT>((uiOutputWidth - uiInputWidth) / 2);
            matrix._32 -= static_cast<FLOAT>((uiOutputHeight - uiInputHeight) / 2);
            hr = spRenderContextImpl->UpdateTransform(&matrix);
        }
    }
    
    //**********************************************************************
    // Get pointer to our source pixels
    //**********************************************************************
    WICRect rcInputLock = {0, 0, uiInputWidth, uiInputHeight};
    CComPtr<IWICBitmapLock> spInputLock;
    hr = spSrcBitmap->Lock(&rcInputLock, WICBitmapLockWrite, &spInputLock);
    BYTE *pbInputData;
    UINT uiInputBufferSize;
    UINT uiInputStride;
    if (SUCCEEDED(hr) && spInputLock)
    {
        hr = spInputLock->GetDataPointer(&uiInputBufferSize, (BYTE**)&pbInputData);
        if (SUCCEEDED(hr))
        {
            hr = spInputLock->GetStride(&uiInputStride);
        }
    }
    
    //**********************************************************************
    // Get pointer to our destination pixels
    //**********************************************************************
    WICRect rcOutputLock = {0, 0, uiOutputWidth, uiOutputHeight};
    CComPtr<IWICBitmapLock> spOutputLock;
    hr = spDstBitmap->Lock(&rcOutputLock, WICBitmapLockWrite, &spOutputLock);
    BYTE *pbOutputData;
    UINT uiOutputBufferSize;
    UINT uiOutputStride;
    if (SUCCEEDED(hr) && spOutputLock)
    {
        hr = spOutputLock->GetDataPointer(&uiOutputBufferSize, (BYTE**)&pbOutputData);
        if (SUCCEEDED(hr))
        {
            hr = spOutputLock->GetStride(&uiOutputStride);
        }
    }

    //**********************************************************************
    // Process our pixels
    //**********************************************************************
    assert(uiInputWidth == uiOutputWidth);
    assert(uiInputHeight == uiOutputHeight);

    BYTE *pbInputRow = pbInputData;
    BYTE *pbOutputRow = pbOutputData;
    const int c_iBytesPerPixel = 4;
    if (SUCCEEDED(hr))
    {
        for (UINT uiY = 0; uiY < uiInputHeight; uiY++)
        {
            BYTE *pbInputColumn = pbInputRow;
            BYTE *pbOutputColumn = pbOutputRow;

			double pixel;
            for (UINT uiX = 0; uiX < uiInputWidth; uiX++)
            {
				//Red
				pixel = pbInputColumn[2] + (255 * m_dRed);
				pixel = pixel >= 0 ? pixel : 0; //clamp to positive 
				pbOutputColumn[2] = pixel <= 255 ? (BYTE)pixel : 255; //clamp to 255

				//Green
				pixel = pbInputColumn[1] + (255 * m_dGreen);
				pixel = pixel >= 0 ? pixel : 0; //clamp to positive 
				pbOutputColumn[1] = pixel <= 255 ? (BYTE)pixel : 255; //clamp to 255

				//Blue
				pixel = pbInputColumn[0] + (255 * m_dBlue);
				pixel = pixel >= 0 ? pixel : 0; //clamp to positive 
				pbOutputColumn[0] = pixel <= 255 ? (BYTE)pixel : 255; //clamp to 255
				
				//Alpha
				pbOutputColumn[3] = pbInputColumn[3] ; 

				//Next Pixel
				pbInputColumn += c_iBytesPerPixel;
                pbOutputColumn += c_iBytesPerPixel;
            }
			//Next Row
            pbInputRow += uiInputStride;
            pbOutputRow += uiOutputStride;
        }
    }

    if (SUCCEEDED(hr))
    {
        *ppBitmapSource = spDstBitmap;
        (*ppBitmapSource)->AddRef();
    }
    return hr;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::TransformPoint
//
//  Synopsis:
//      Transform the given point.
//
// Arguments:
//      uiIndex - zero-based index of output pin through which to transform the point
//      p - pointer to the point to be transformed.
//      fForwardTransform - is point being transformed from front of the effect graph
//          towards the end, or vice versa (i.e. inverse transform).
//      pContext - render context to use
//      pfPointTransformed - returns whether the point transformed to a known location
//          If the transform is undefined this will return VARIANT_FALSE
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::TransformPoint(ULONG uiIndex, MIL_2DPOINTD *p,
                VARIANT_BOOL fForwardTransform, IMILBitmapEffectRenderContext *pContext,
                VARIANT_BOOL *pfPointTransformed)
{
   HRESULT hr = S_OK;
    
    CComPtr<IMILBitmapEffectOutputConnector> spOutputConnector;
    
    //**********************************************************************
    // Get the connections object for current effect
    //**********************************************************************
    CComPtr<IMILBitmapEffectConnections> spConnections;
    hr = this->QueryInterface(__uuidof(IMILBitmapEffectConnections),
                                         reinterpret_cast<void**>(&spConnections));

    //**********************************************************************
    // Find the input connector on the effect
    //**********************************************************************
    CComPtr<IMILBitmapEffectInputConnector> spInputConnector;
    if (SUCCEEDED(hr))
    {
        hr = spConnections->GetInputConnector(0, &spInputConnector);
    }
    
    VARIANT_BOOL vbConnected = VARIANT_FALSE;
    while (true)
    {
        if (SUCCEEDED(hr))
        {
            hr = spInputConnector->IsConnected(&vbConnected);
        }
        if (vbConnected == VARIANT_FALSE)
        {
            break;
        }
        //**********************************************************************
        // Find the output connector the input connector is connected to
        //**********************************************************************
        if (SUCCEEDED(hr))
        {
            spOutputConnector.Release();
            hr = spInputConnector->GetConnection(&spOutputConnector);
        }
        
        //**********************************************************************
        // Check if the output connector is an interior connector. In this case
        // it is interior portion of a group effect which means we need to step
        // outside the group.
        //**********************************************************************
        CComPtr<IMILBitmapEffectInteriorInputConnector> spInteriorInput;
        if (SUCCEEDED(hr))
        {
            hr = spOutputConnector->QueryInterface(__uuidof(IMILBitmapEffectInteriorInputConnector),
                                                   reinterpret_cast<void**>(&spInteriorInput));
        }
        
        //**********************************************************************
        // If it isn't an interior connector we are done
        //**********************************************************************
        if (FAILED(hr))
        {
            if (hr == E_NOINTERFACE)
            {
                hr = S_OK;
            }
            break;
        }
        
        //**********************************************************************
        // Otherwise, get the input connector that is associated with the
        // interior connector (i.e. step outside the current group effect)
        // Loop back up and see if we are inside yet another group object.
        //**********************************************************************
        spInputConnector.Release();
        hr = spInteriorInput->GetInputConnector(&spInputConnector);
        if (FAILED(hr))
        {
            break;
        }
    }
    if (SUCCEEDED(hr) && vbConnected == VARIANT_TRUE)
    {
        CComPtr<IMILBitmapEffect> spPreviousEffect;
        if (SUCCEEDED(hr))
        {
            spOutputConnector->GetBitmapEffect(&spPreviousEffect);
        }
        CComPtr<IMILBitmapEffectPrimitive> spPrimitive;
        if (SUCCEEDED(hr))
        {
            hr = spPreviousEffect->QueryInterface(__uuidof(IMILBitmapEffectPrimitive),
                                                  reinterpret_cast<void**>(&spPrimitive));
        }
        if ( ! fForwardTransform)
        {
            // Do your work here
        }
        if (SUCCEEDED(hr))
        {
            hr = spPrimitive->TransformPoint(0, p, fForwardTransform, pContext, pfPointTransformed);
        }
        if (fForwardTransform)
        {
            // Do your work here
        }
    }
	if (SUCCEEDED(hr))
	{
		*pfPointTransformed = VARIANT_TRUE;
	}
    return hr;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::TransformRect
//
//  Synopsis:
//      Transform the given rect
//
// Arguments:
//      uiIndex - zero-based index of output pin through which to transform the point
//      p - pointer to the rect to be transformed.
//      fForwardTransform - is point being transformed from front of the effect graph
//          towards the end, or vice versa (i.e. inverse transform).
//      pContext - render context to use
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::TransformRect(ULONG uiIndex, MIL_RECTD *p,
                                          VARIANT_BOOL fForwardTransform,
                                          IMILBitmapEffectRenderContext *pContext)
{
    HRESULT hr = S_OK;
    
    //**********************************************************************
    // Get the connections object for current effect
    //**********************************************************************
    CComPtr<IMILBitmapEffectConnections> spConnections;
    hr = this->QueryInterface(__uuidof(IMILBitmapEffectConnections),
                              reinterpret_cast<void**>(&spConnections));

    //**********************************************************************
    // Find the input connector on the effect
    //**********************************************************************
    CComPtr<IMILBitmapEffectInputConnector> spInputConnector;
    if (SUCCEEDED(hr))
    {
        hr = spConnections->GetInputConnector(0, &spInputConnector);
    }
    
    CComPtr<IMILBitmapEffectOutputConnector> spOutputConnector;
    VARIANT_BOOL vbConnected = VARIANT_FALSE;
    while (true)
    {
        if (SUCCEEDED(hr))
        {
            hr = spInputConnector->IsConnected(&vbConnected);
        }
        if (vbConnected == VARIANT_FALSE)
        {
            break;
        }
        //**********************************************************************
        // Find the output connector the input connector is connected to
        //**********************************************************************
        if (SUCCEEDED(hr))
        {
            spOutputConnector.Release();
            hr = spInputConnector->GetConnection(&spOutputConnector);
        }
        
        //**********************************************************************
        // Check if the output connector is an interior connector. In this case
        // it is interior portion of a group effect which means we need to step
        // outside the group.
        //**********************************************************************
        CComPtr<IMILBitmapEffectInteriorInputConnector> spInteriorInput;
        if (SUCCEEDED(hr))
        {
            hr = spOutputConnector->QueryInterface(__uuidof(IMILBitmapEffectInteriorInputConnector),
                                                   reinterpret_cast<void**>(&spInteriorInput));
        }
        
        //**********************************************************************
        // If it isn't an interior connector we are done
        //**********************************************************************
        if (FAILED(hr))
        {
            if (hr == E_NOINTERFACE)
            {
                hr = S_OK;
            }
            break;
        }
        
        //**********************************************************************
        // Otherwise, get the input connector that is associated with the
        // interior connector (i.e. step outside the current group effect)
        // Loop back up and see if we are inside yet another group object.
        //**********************************************************************
        spInputConnector.Release();
        hr = spInteriorInput->GetInputConnector(&spInputConnector);
        if (FAILED(hr))
        {
            break;
        }
    }
    if (SUCCEEDED(hr))
    {
		CComPtr<IMILBitmapEffect> spPreviousEffect;
		CComPtr<IMILBitmapEffectPrimitive> spPrimitive;
		if (vbConnected == VARIANT_TRUE)
		{
			if (SUCCEEDED(hr))
			{
				spOutputConnector->GetBitmapEffect(&spPreviousEffect);
			}
			if (SUCCEEDED(hr))
			{
				hr = spPreviousEffect->QueryInterface(__uuidof(IMILBitmapEffectPrimitive),
													  reinterpret_cast<void**>(&spPrimitive));
			}
		}
        if ( ! fForwardTransform)
        {
            // Do your work here
        }
        if (SUCCEEDED(hr) && vbConnected == VARIANT_TRUE)
        {
            hr = spPrimitive->TransformRect(uiIndex, p, fForwardTransform, pContext);
        }
        if (fForwardTransform)
        {
            // Do your work here
        }
    }
    return hr;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::HasAffineTransform
//
//  Synopsis:
//      Has Affine transform?
//
// Arguments:
//      uiIndex - zero-based index of output pin
//      pfAffine - returns whether the effect has an affine transform
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::HasAffineTransform(ULONG uiIndex, VARIANT_BOOL *pfAffine)
{
    HRESULT hr = S_OK;
    *pfAffine = VARIANT_TRUE;
    return hr;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::HasInverseTransform
//
//  Synopsis:
//      Has inverse transform?
//
// Arguments:
//      uiIndex - zero-based index of output pin
//      pfAffine - returns whether the effect has an inverse transform
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::HasInverseTransform(ULONG uiIndex, VARIANT_BOOL *pfHasInverse)
{
    HRESULT hr = S_OK;
    *pfHasInverse = VARIANT_TRUE;
    return hr;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetAffineMatrix
//
//  Synopsis:
//      Get the affine matrix.
//
// Arguments:
//      uiIndex - zero-based index of output pin
//      pMatrix - returns the affine matrix describing the effects transform
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::GetAffineMatrix(ULONG uiIndex, MIL_MATRIX3X2D *pMatrix)
{
    HRESULT hr = S_OK;
    pMatrix->S_11 = 1.0f;
    pMatrix->S_12 = 0.0f;
    pMatrix->S_21 = 0.0f;
    pMatrix->S_22 = 1.0f;
    pMatrix->DX = 0.0f;
    pMatrix->DY = 0.0f;
    return hr;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::IsDirty
//
//  Synopsis:
//      Is the effect dirty?
//
// Arguments:
//      uiIndex - zero-based index of output pin
//      pfDirty - returns whether the current effect is dirty (i.e. needs to
//          be re-run).
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::IsDirty(ULONG uiOutputIndex, VARIANT_BOOL *pfDirty)
{
    HRESULT hr = S_OK;
    *pfDirty = VARIANT_TRUE;
    return hr;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::IsVolatile
//
//  Synopsis:
//      Is the effect volatile?
//
// Arguments:
//      uiIndex - zero-based index of output pin
//      pfDirty - returns whether the current effect is considered volatile.
//          If it is then the effects framework will never attempt to cache
//          this effects output.
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::IsVolatile(ULONG uiOutputIndex, VARIANT_BOOL *pfVolatile)
{
    HRESULT hr = S_OK;
    *pfVolatile = VARIANT_TRUE;
    return hr;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::CountProperties
//
//  Synopsis:
//      IPropertyBag2 method. See MSDN for details.
//
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::CountProperties(ULONG *pcProperties)
{
    if (pcProperties == NULL)
    {
        return(E_INVALIDARG);
    }
    *pcProperties = c_ulNumberProperties;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetPropertyInfo
//
//  Synopsis:
//      IPropertyBag2 method. See MSDN for details.
//
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::GetPropertyInfo(ULONG iProperty, ULONG cProperties, PROPBAG2 *pPropBag, ULONG *pcProperties)
{
 HRESULT hr = S_OK;

    if (pPropBag == NULL || pcProperties == NULL)
    {
        return(E_INVALIDARG);
    }
    
	if( iProperty + cProperties > c_ulNumberProperties )
	{
		return( E_INVALIDARG );
	}

    ZeroMemory(pPropBag, sizeof(PROPBAG2) * cProperties);
    *pcProperties = 0;

    int j = 0;
    for (ULONG i = iProperty; i < iProperty + cProperties; i++)
    {
        switch (i)
        {
            case 0:
            {
				pPropBag[j].dwType = PROPBAG2_TYPE_DATA;
                pPropBag[j].vt = VT_R8;
                int iNumBytes = (wcslen(c_pwcsRed) + 1) * sizeof(WCHAR);
                LPVOID pvData = CoTaskMemAlloc(iNumBytes);
                if (pvData == NULL)
                {
                    hr=E_OUTOFMEMORY;
                    break;
                }
                memcpy(pvData, c_pwcsRed, iNumBytes);
                pPropBag[j].pstrName = reinterpret_cast<LPOLESTR>(pvData);
                (*pcProperties)++;
            }
            case 1:
            {
				pPropBag[j].dwType = PROPBAG2_TYPE_DATA;
                pPropBag[j].vt = VT_R8;
                int iNumBytes = (wcslen(c_pwcsGreen) + 1) * sizeof(WCHAR);
                LPVOID pvData = CoTaskMemAlloc(iNumBytes);
                if (pvData == NULL)
                {
                    hr=E_OUTOFMEMORY;
                    break;
                }
                memcpy(pvData, c_pwcsGreen, iNumBytes);
                pPropBag[j].pstrName = reinterpret_cast<LPOLESTR>(pvData);
                (*pcProperties)++;
            }
            case 2:
            {
				pPropBag[j].dwType = PROPBAG2_TYPE_DATA;
                pPropBag[j].vt = VT_R8;
                int iNumBytes = (wcslen(c_pwcsBlue) + 1) * sizeof(WCHAR);
                LPVOID pvData = CoTaskMemAlloc(iNumBytes);
                if (pvData == NULL)
                {
                    hr=E_OUTOFMEMORY;
                    break;
                }
                memcpy(pvData, c_pwcsBlue, iNumBytes);
                pPropBag[j].pstrName = reinterpret_cast<LPOLESTR>(pvData);
                (*pcProperties)++;
            }
            break;
        }
        if (FAILED(hr))
        {
            break;
        }
        j++;
    }

    if (FAILED(hr))
    {
        int j = 0;
        for (ULONG i = iProperty; i < cProperties; i++)
        {
            if (pPropBag[j].pstrName)
            {
                CoTaskMemFree(reinterpret_cast<LPVOID>(pPropBag[j].pstrName));
                pPropBag[j].pstrName = NULL;
            }
            j++;
        }
    }

    return hr;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::LoadObject
//
//  Synopsis:
//      IPropertyBag2 method. See MSDN for details.
//
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::LoadObject(LPCOLESTR pstrName, DWORD dwHint, IUnknown *pUnkObject, IErrorLog *pErrLog)
{
    HRESULT hr = S_OK;
    return hr;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::Read
//
//  Synopsis:
//      IPropertyBag2 method. See MSDN for details.
//
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::Read(ULONG cProperties, PROPBAG2 *pPropBag, IErrorLog *pErrLog, VARIANT *pvarValue, HRESULT *phrError)
{
     HRESULT hr = S_OK;
    if (pPropBag == NULL || pvarValue == NULL)
    {
        return(E_INVALIDARG);
    }
    
    for (ULONG i = 0; i < cProperties; i++)
    {
        if (_wcsicmp(pPropBag[i].pstrName, c_pwcsRed) == 0)
        {
            pvarValue[i].vt = VT_R8;
            pvarValue[i].dblVal = m_dRed;
            phrError[i] = S_OK;
        }
        else if (_wcsicmp(pPropBag[i].pstrName, c_pwcsGreen) == 0)
        {
            pvarValue[i].vt = VT_R8;
            pvarValue[i].dblVal = m_dGreen;
            phrError[i] = S_OK;
        }
        else if (_wcsicmp(pPropBag[i].pstrName, c_pwcsBlue) == 0)
        {
            pvarValue[i].vt = VT_R8;
            pvarValue[i].dblVal = m_dBlue;
            phrError[i] = S_OK;
        }
        else
        {
            phrError[i] = E_FAIL;
            hr=E_FAIL; // a property was requested that we don't have
        }
    }
    return(hr);
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::Write
//
//  Synopsis:
//      IPropertyBag2 method. See MSDN for details.
//
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::Write(ULONG cProperties, PROPBAG2 *pPropBag, VARIANT *pvarValue)
{
    HRESULT hrRes = S_OK;
    HRESULT hr;
    if (pPropBag == NULL || pvarValue == NULL)
    {
        return(E_INVALIDARG);
    }
    
    for (ULONG i = 0; i < cProperties; i++)
    {
        if (_wcsicmp(pPropBag[i].pstrName, c_pwcsRed) == 0)
        {
            VARIANTARG varResult;
            VariantInit(&varResult);
            hr=VariantChangeType(&varResult, &pvarValue[i], 0, VT_R8);
            if (SUCCEEDED(hr))
            {
				if( varResult.dblVal <= 1.0 && varResult.dblVal >= -1.0)
					m_dRed = varResult.dblVal;
				else
					hrRes = E_INVALIDARG;
            }
            else
            {
                hrRes = hr;
            }
        }
        else if (_wcsicmp(pPropBag[i].pstrName, c_pwcsGreen) == 0)
        {
            VARIANTARG varResult;
            VariantInit(&varResult);
            hr=VariantChangeType(&varResult, &pvarValue[i], 0, VT_R8);
            if (SUCCEEDED(hr))
            {
				if( varResult.dblVal <= 1.0 && varResult.dblVal >= -1.0)
					m_dGreen = varResult.dblVal;
				else
					hrRes = E_INVALIDARG;
            }
            else
            {
                hrRes = hr;
            }
        }
        else if (_wcsicmp(pPropBag[i].pstrName, c_pwcsBlue) == 0)
        {
            VARIANTARG varResult;
            VariantInit(&varResult);
            hr=VariantChangeType(&varResult, &pvarValue[i], 0, VT_R8);
            if (SUCCEEDED(hr))
            {
				if( varResult.dblVal <= 1.0 && varResult.dblVal >= -1.0)
					m_dBlue = varResult.dblVal;
				else
					hrRes = E_INVALIDARG;
            }
            else
            {
                hrRes = hr;
            }
        }
        else
        {
            hrRes = E_FAIL;
            hr=hrRes;
        }
    }
    return(hrRes);
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetNumberInputs
//
//  Synopsis:
//      IMILBitmapEffectConnectionsInfo method.
//      Returns the number of input pins exposed by this effect.
//
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::GetNumberInputs(ULONG *puiNumInputs)
{
    *puiNumInputs = 1;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetNumberOutputs
//
//  Synopsis:
//      IMILBitmapEffectConnectionsInfo method.
//      Returns the number of output pins exposed by this effect.
//
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::GetNumberOutputs(ULONG *puiNumOutputs)
{
    *puiNumOutputs = 1;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetInputConnectorInfo
//
//  Synopsis:
//      IMILBitmapEffectConnectionsInfo method.
//      Returns the description for the specified input pin
//
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::GetInputConnectorInfo(ULONG uiIndex, IMILBitmapEffectConnectorInfo **ppConnectorInfo)
{
    *ppConnectorInfo = this;
    AddRef();
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetOutputConnectorInfo
//
//  Synopsis:
//      IMILBitmapEffectConnectionsInfo method.
//      Returns the description for the specified output pin
//
//----------------------------------------------------------------------------
STDMETHODIMP
CRGBFilterEffect::GetOutputConnectorInfo(ULONG uiIndex, IMILBitmapEffectConnectorInfo **ppConnectorInfo)
{
    *ppConnectorInfo = this;
    AddRef();
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetIndex
//
//  Synopsis:
//      IMILBitmapEffectConnectorInfo method.
//      Returns the index for the pin.
//
//----------------------------------------------------------------------------
STDMETHODIMP 
CRGBFilterEffect::GetIndex(ULONG *puiIndex)
{
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetOptimalFormat
//
//  Synopsis:
//      IMILBitmapEffectConnectorInfo method.
//      Returns the optimal image format for this pin
//
//----------------------------------------------------------------------------
STDMETHODIMP 
CRGBFilterEffect::GetOptimalFormat(WICPixelFormatGUID *pFormat)
{
    *pFormat = GUID_WICPixelFormat32bppBGRA;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetNumberFormats
//
//  Synopsis:
//      IMILBitmapEffectConnectorInfo method.
//      Returns the number of formats supported by this pin
//
//----------------------------------------------------------------------------
STDMETHODIMP 
CRGBFilterEffect::GetNumberFormats(ULONG *pulNumberFormats)
{
    *pulNumberFormats = 1;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      CRGBFilterEffect::GetFormat
//
//  Synopsis:
//      IMILBitmapEffectConnectorInfo method.
//      Returns the requested format
//
//----------------------------------------------------------------------------
STDMETHODIMP 
CRGBFilterEffect::GetFormat(ULONG ulIndex, WICPixelFormatGUID *pFormat)
{
    *pFormat = GUID_WICPixelFormat32bppBGRA;
    return S_OK;
}
