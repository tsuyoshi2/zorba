/*
 * Copyright 2006-2008 The FLWOR Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or  implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// ZorbaSingleThread.h : Declaration of the CZorbaSingleThread

#pragma once
#include "resource.h"       // main symbols

#include "ZorbaCOMApi.h"
#include "zorba/zorba_engine_singlethread_api.h"
#include "zorba/api_shared_types.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CZorbaSingleThread

class ATL_NO_VTABLE CZorbaSingleThread :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CZorbaSingleThread, &CLSID_ZorbaSingleThread>,
	public IDispatchImpl<IZorbaSingleThread, &IID_IZorbaSingleThread, &LIBID_ZorbaCOMApiLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
  zorba::ZorbaSingleThread_t     zorba_singlethread;
public:
	CZorbaSingleThread()
	{
    zorba_singlethread = zorba::ZorbaSingleThread::getInstance();
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ZORBASINGLETHREAD)


BEGIN_COM_MAP(CZorbaSingleThread)
	COM_INTERFACE_ENTRY(IZorbaSingleThread)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

    virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createQuery( 
        /* [defaultvalue][in] */ BSTR aQueryString,
        /* [defaultvalue][in] */ IStaticQueryContext *sctx,
        /* [defaultvalue][in] */ BSTR xquery_source_uri,
        /* [defaultvalue][in] */ VARIANT_BOOL routing_mode,
        /* [defaultvalue][in] */ IXQueryTreePlans *tree_plans,
        /* [retval][out] */ IXQuery **xquery);
    virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createDynamicQueryContext( 
        /* [retval][out] */ IDynamicQueryContext **dctx);
};

OBJECT_ENTRY_AUTO(__uuidof(::ZorbaSingleThread), CZorbaSingleThread)
