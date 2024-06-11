/*------------------------------------------------------------------------------
*  Copyright(c)
*  创建人: cs
*  日期: 2023.6
*  描述: 尺寸导出
*  版本: 2024SP0
------------------------------------------------------------------------------*/
// CxCappExternDimToXmlDoc.h : Declaration of the CCxCappExternDimToXmlDoc

#pragma once
#include "resource.h"       // main symbols
#include "ICxCappCommandImpl.h"


#include "..\Interface\AppFillin.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

struct EXPORTDATA_DIM
{
	CString csNum;
	CString csKeyNum;
	CString csDimType;
	map<CString, CString> mSingleData;
};

struct EXPORTDATA_CARD
{
	CString csOPRNumber;
	CString csOPRName;
	vector<EXPORTDATA_DIM> vDim;
};

struct DIMENSION_PIC
{
	CString csPID;
	CString csDimRowNum;
	CString csNum;
	CString csDimType;
	map<CString, CString> mDimData;

	BOOL JudgeSame(CString i_csPID, CString i_csDRNum)
	{
		if (i_csPID == csPID && i_csDRNum == csDimRowNum)
		{
			return TRUE;
		}
		return FALSE;
	}
};

struct DATACHOICE
{
	CString csTemplateName;
	CString csOPRNum;
	CString csOPRName;
	CString csRecordCol;
	CString csDataCol;
	CString csDataSource;
	DOUBLE dx1, dx2, dy1, dy2;
	DATACHOICE()
	{
		dx1 = 0.0;
		dx2 = 0.0;
		dy1 = 0.0;
		dy2 = 0.0;
	}
	BOOL FINDTEMPLATE(CString i_csTemplatename)
	{
		if (i_csTemplatename == csTemplateName)
		{
			return TRUE;
		}
		return FALSE;
	}
};

// CCxCappExternDimToXmlDoc

class ATL_NO_VTABLE CCxCappExternDimToXmlDoc :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCxCappExternDimToXmlDoc, &CLSID_CxCappExternDimToXmlDoc>,
	public ICxCappCommandImpl<>
	//public IDispatchImpl<ICxCappExternDimToXmlDoc, &IID_ICxCappExternDimToXmlDoc, &LIBID_AppFillinLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CCxCappExternDimToXmlDoc()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CXCAPPEXTERNDIMTOXMLDOC)


BEGIN_COM_MAP(CCxCappExternDimToXmlDoc)
	COM_INTERFACE_ENTRY(ICxCommand)
	COM_INTERFACE_ENTRY(ICxTarget)
	COM_INTERFACE_ENTRY2(IDispatch, ICxCommand)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
	vector<DATACHOICE> m_vDataChoice;
	map<CString, CString> m_mDisplayTxt;

public:
	virtual Caxa::ERR	Function();
	BOOL HasSameDoc(CString i_csFilePath, CString i_csFileName);
	BOOL TransStrToFile_XML(CString i_csXmlStr, CString i_csXmlFilePath);
	BOOL GetCurReguDataChoice(vector<DATACHOICE>* vDataChoice);
	void GetReguDimData(vector<EXPORTDATA_CARD>* o_vRegu);
	BOOL TransReguDimDataToXmlFile(vector<EXPORTDATA_CARD> i_vRegu, CString csXmlFilePath);
	BOOL GetTwoDValue(CString i_csPoint, DOUBLE* o_dx, DOUBLE* o_dy);
	//void GetDimDataFromCardCol_DSys(vector<EXPORTDATA_CARD>* vData);
	void GetDimDataFromCardColSon(ICxCappCard* i_spNowCard, CString i_csRecordCol, CString i_csDataCol, vector<EXPORTDATA_DIM>* o_vColDimData);
	//void GetDimDataFromCardPic_DSys(vector<EXPORTDATA_CARD>* vData);
	void GetDimDataFromCardPicSon(ICxCappCard* i_spNowCard, IBox* i_spBox, vector<EXPORTDATA_DIM>* o_vPicDimData);
	void GetObjRequiredData(ICxDbObject* i_pObj, vector<DIMENSION_PIC>* o_vDIM_PIC, IBox* i_spBox, ICxDbBlkReference* i_pBlkRef = NULL);
	//BOOL GetDimTxtRequiredData(CString csDimTxt, map<CString, CString>* mDimData, CString& csType);
	CString GetDimKey_Num(ICxDbObject* i_pObj);
	BOOL DealWithCardData(ICxCappCard* i_spCard, EXPORTDATA_CARD& o_EX_CARD);
	void GetDataInfo(CString i_csDev, CString* o_csType, map<CString, CString>* o_mDevInfo);
	void GetRecordInfo(CString i_csRecordTxt, CString* o_csRecord);
	void DealWithDimData(vector<EXPORTDATA_DIM>& vDimData);
	BOOL GetCardOPRData(ICxCappCard* i_spCard, EXPORTDATA_CARD EX_CARD, CString* o_csOPRNameTxt, CString* o_csOPRNumTxt);
};

OBJECT_ENTRY_AUTO(__uuidof(CxCappExternDimToXmlDoc), CCxCappExternDimToXmlDoc)
