/*------------------------------------------------------------------------------
*  Copyright(c)
*  创建人: cs
*  日期: 2023.6
*  描述: 尺寸导出
*  版本: 2024SP0
------------------------------------------------------------------------------*/
// CxCappExternDimToXmlDoc.cpp : Implementation of CCxCappExternDimToXmlDoc

#include "stdafx.h"
#include "CxCappExternDimToXmlDoc.h"
#include "ExternToolDimToXmlDocDlg.h"
#include <atlenc.h>
#include <msxml.h>
#include "File_Pub.h"
#import "..\include\msxml6.tlb"
using namespace MSXML2;


// CCxCappExternDimToXmlDoc
Caxa::ERR CCxCappExternDimToXmlDoc::Function()
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	CX_MANAGE_STATE()

	CComPtr<ICxDocument> spDoc;
	spDoc = GetCxDocument();
	CComBSTR bsFilePath;
	spDoc->GetPathName(&bsFilePath);
	CString csFilePath = (COLE2CT)bsFilePath;
	if (csFilePath.IsEmpty())
	{
		CString sVal;
		sVal.LoadString(IDS_CAPP_DIMTOXMLNOTFILE);//_T("文件未保存不能进行尺寸导出操作，请保存后再试！")
		AfxMessageBox(sVal);
		EndCommand();
		return cxFAIL;
	}
	INT nFileName = csFilePath.ReverseFind('\\');
	INT nLength = csFilePath.GetLength();
	CString csFileName;
	BOOL bIsCxp = FALSE;
	if (nFileName != -1 && nLength != -1)
	{
		csFileName = csFilePath.Right(nLength - nFileName - 1);
		csFilePath = csFilePath.Left(nFileName);
		if (csFileName.Right(4) == _T(".cxp"))
		{
			csFileName.Replace(_T(".cxp"), _T(".xml"));
			bIsCxp = TRUE;
		}
	}
	if (!bIsCxp)
	{
		CString sVal;
		sVal.LoadString(IDS_CAPP_DIMTOXMLNOTCXPFILE);//_T("当前打开文件非cxp文件")
		AfxMessageBox(sVal);
		EndCommand();
		return cxFAIL;
	}

		ExternToolDimToXmlDocDlg dlg;
	dlg.m_csCxpFilePath = csFilePath;
	if (dlg.DoModal() == IDOK)
	{
		CString csXmlFilePath;
		csXmlFilePath = dlg.m_csXmlPath;

		CString csExportXml;
		CString csExportXmlName = csFileName;
		csExportXml = csXmlFilePath + _T("\\") + csExportXmlName;

		if (HasSameDoc(csXmlFilePath, csExportXmlName))
		{
			CString str1;
			CString str2;
			str1.LoadString(IDS_CAPP_DIMTOXMLSHADEF);//_T("在此路径下，有")
			str2.LoadString(IDS_CAPP_DIMTOXMLSHADES);//_T("的同名文件，是否要将原文件覆盖？")
			CString str = dlg.m_csXmlPath + str1 + csExportXmlName + str2;
			INT nRes = AfxMessageBox(str, MB_YESNO | MB_ICONWARNING);
			if (nRes == IDNO)
			{
				EndCommand();
				return cxOK;
			}
		}

		BOOL bDataChoice = GetCurReguDataChoice(&m_vDataChoice);
		if (m_vDataChoice.empty() || !bDataChoice)
		{
			CString sVal;
			sVal.LoadString(IDS_CAPP_DIMTOXMLWRONGSET);//_T("没有找到当前规程相关配置信息，请检查配置文件中相关配置是否正确！")
			AfxMessageBox(sVal);
			EndCommand();
			return cxOK;
		}

		vector<EXPORTDATA_CARD> vReguData;
		GetReguDimData(&vReguData);
		if (vReguData.empty())
		{
			CString sVal;
			sVal.LoadString(IDS_CAPP_DIMTOXMLNONDIMDATA);//_T("当前工艺文件无尺寸数据导出！")
			AfxMessageBox(sVal);
			EndCommand();
			return cxOK;
		}

		BOOL bExport = TransReguDimDataToXmlFile(vReguData, csExportXml);
		if (bExport)
		{
			CString sVal;
			sVal.LoadString(IDS_CAPP_DIMTOXMLSUCCEED);//_T("尺寸导出成功！")
			AfxMessageBox(sVal);
		}
	}

	EndCommand();
	return cxOK;
}

BOOL CCxCappExternDimToXmlDoc::HasSameDoc(CString i_csFilePath, CString i_csFileName)
{
	BOOL bHasSame = FALSE;
	CFileFind cFinder;
	BOOL bNotEmpty = cFinder.FindFile(i_csFilePath + _T("\\*.*"));
	while (bNotEmpty)
	{
		bNotEmpty = cFinder.FindNextFile();
		if (!cFinder.IsDirectory())
		{
			if (i_csFileName == cFinder.GetFileName())
			{
				bHasSame = TRUE;
				break;
			}
		}
	}

	return bHasSame;
}

BOOL CCxCappExternDimToXmlDoc::TransStrToFile_XML(CString i_csXmlStr, CString i_csXmlFilePath)
{
	CStdioFile CSFile;
	try
	{
		CSFile.Open(i_csXmlFilePath, CFile::modeCreate | CFile::modeWrite);
		DWORD dwFileLen = CSFile.GetLength();
		if (0 == dwFileLen)
		{
			const unsigned char LeadBytes[] = {0xEF, 0XBB, 0XBF};
			CSFile.Write(LeadBytes, sizeof(LeadBytes));
		}
		int nSrcLen = (int)wcslen(i_csXmlStr);
		CStringA utf8String(i_csXmlStr);
		int nBufLen = (nSrcLen + 1) * 6;
		LPSTR buffer = utf8String.GetBufferSetLength(nBufLen);
		int nLen = AtlUnicodeToUTF8(i_csXmlStr, nSrcLen, buffer, nBufLen);
		buffer[nLen] = 0;
		utf8String.ReleaseBuffer();
		CSFile.Write(utf8String.GetBuffer(), nLen);
		CSFile.Close();
	}
	catch (CFileException *e)
	{
		CString csErroInfo;
		csErroInfo.Format(_T("%s"), e->ReportError());

		CX_MANAGE_STATE_EX(CxGetResHandle(TEXT("ResCappDb.dll")));
		CString sVal;
		sVal.LoadString(IDS_CAPP_DIMTOXMLCREATEFAIL);//_T("文件创建失败！")
		AfxMessageBox(sVal);
		return FALSE;
	}

	return TRUE;
}

BOOL CCxCappExternDimToXmlDoc::GetCurReguDataChoice(vector<DATACHOICE>* vDataChoice)
{
	vector<DATACHOICE> vTemp;
	CComPtr<ICxPathMng>spPathMng = GetPathMng();
	CComBSTR bsLocalRoot;
	spPathMng->GetLocalRoot(&bsLocalRoot);
	CString csXmlPath(bsLocalRoot);
	TCHAR szLang[LANGSRTLEN] = {};
	CxGetLANGStr(szLang);
	CString  strLang = szLang;
	strLang.TrimLeft();
	strLang.TrimRight();
	if (strLang == _T("zh-CN"))
		csXmlPath += _T("\\Setting\\zh-CN\\DataChoiceForDownstreamSys.xml");
	else if(strLang == _T("zh-TW"))
		csXmlPath += _T("\\Setting\\zh-TW\\DataChoiceForDownstreamSys.xml");
	else
		csXmlPath += _T("\\Setting\\en-US\\DataChoiceForDownstreamSys.xml");
	try
	{
		MSXML2::IXMLDOMDocumentPtr spDoc = nullptr;
		HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
		if (FAILED(hr))
		{
			return FALSE;
		}
		VARIANT_BOOL vb = spDoc->load((_variant_t)csXmlPath);
		if (!vb)
		{
			return FALSE;
		}

		CString csRegu = GetReguName();

		MSXML2::IXMLDOMNodePtr IXMLDim = spDoc->selectSingleNode(_T("Dimension"));
		if (IXMLDim != nullptr)
		{
			MSXML2::IXMLDOMNodePtr IXMLRegu = IXMLDim->selectSingleNode(_T("//ReguName[@name='" + (_bstr_t)csRegu + "']"));
			if (IXMLRegu != nullptr)
			{
				MSXML2::IXMLDOMNodeListPtr IXMLReguList = IXMLRegu->selectNodes(_T("Template"));
				LONG lList = IXMLReguList->Getlength();
				for (LONG i = 0; i < lList; i++)
				{
					MSXML2::IXMLDOMNodePtr IXMLTemplate = IXMLReguList->Getitem(i);
					MSXML2::IXMLDOMElementPtr IXMLTemplateEle = IXMLTemplate;
					if (IXMLTemplateEle != nullptr)
					{
						_variant_t vTempName, vOPRNum, vOPRName, vDataSour;
						vTempName = IXMLTemplateEle->getAttribute(_T("name"));
						vOPRNum = IXMLTemplateEle->getAttribute(_T("OPRNumber"));
						vOPRName = IXMLTemplateEle->getAttribute(_T("OPRName"));
						vDataSour = IXMLTemplateEle->getAttribute(_T("DataSource"));

						CString csTempName = (LPCTSTR)(_bstr_t)vTempName;
						CString csOPRNum = (LPCTSTR)(_bstr_t)vOPRNum;
						CString csOPRName = (LPCTSTR)(_bstr_t)vOPRName;
						CString csDataSour = (LPCTSTR)(_bstr_t)vDataSour;

						DATACHOICE DTemp;
						DTemp.csTemplateName = csTempName;
						DTemp.csOPRNum = csOPRNum;
						DTemp.csOPRName = csOPRName;
						DTemp.csDataSource = csDataSour;

						if (csDataSour == _T("1"))
						{
							_variant_t vPointMin, vPointMax;
							vPointMax = IXMLTemplateEle->getAttribute(_T("PointMax"));
							vPointMin = IXMLTemplateEle->getAttribute(_T("PointMin"));
							CString csPointMin = (LPCTSTR)(_bstr_t)vPointMin;
							CString csPointMax = (LPCTSTR)(_bstr_t)vPointMax;

							{
								DOUBLE dx1, dx2, dy1, dy2;

								BOOL bGet1 = GetTwoDValue(csPointMax, &dx1, &dy1);
								BOOL bGet2 = GetTwoDValue(csPointMin, &dx2, &dy2);
								if (!bGet1 || !bGet2)
								{
									return FALSE;
								}

								DTemp.dx1 = min(dx1, dx2);
								DTemp.dx2 = max(dx1, dx2);
								DTemp.dy1 = min(dy1, dy2);
								DTemp.dy2 = max(dy1, dy2);
								vTemp.push_back(DTemp);
							}
						}
						else if (csDataSour == _T("0"))
						{
							_variant_t vRecordCol, vDataCol;
							vRecordCol = IXMLTemplateEle->getAttribute(_T("RecordCol"));
							vDataCol = IXMLTemplateEle->getAttribute(_T("DataCol"));
							CString csRecordCol = (LPCTSTR)(_bstr_t)vRecordCol;
							CString csDataCol = (LPCTSTR)(_bstr_t)vDataCol;

							DTemp.csRecordCol = csRecordCol;
							DTemp.csDataCol = csDataCol;
							vTemp.push_back(DTemp);
						}
					}
				}
			}
			else
			{
				return FALSE;
			}
		}

		vDataChoice->swap(vTemp);
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
}

void CCxCappExternDimToXmlDoc::GetReguDimData(vector<EXPORTDATA_CARD>* o_vRegu)
{
	vector<EXPORTDATA_CARD> vCardDimData;

	CComPtr <ICxCappGlobal> spGlobal;
	GetGlobalInfo2(&spGlobal);
	CComQIPtr<ICxCappCardColl> spCardList;
	GetCardList2(&spCardList);
	LONG_PTR pos;
	CComPtr<ICxDbObjColl> pCardCollObj;
	spCardList->GetCardRefList(&pCardCollObj);
	pCardCollObj->GetHeadPosition(&pos);
	while (pos)
	{
		CComPtr<ICxDbObject> pObj;
		pCardCollObj->GetNext(&pos, &pObj);
		CComQIPtr<ICxCappCardRef> spCardRef = pObj;
		ATLASSERT(spCardRef != NULL);
		CComPtr<ICxCappCard> spCard;
		spCardRef->GetCard(&spCard);
		EXPORTDATA_CARD EX_CARD;
		if (spCard)
		{
			BOOL bFindTemplate = DealWithCardData(spCard, EX_CARD);
			if (!bFindTemplate)
			{
				continue;
			}
			CString csOPRName, csOPRNum;
			BOOL bGetOPR = GetCardOPRData(spCard, EX_CARD, &csOPRName, &csOPRNum);
			if (!bGetOPR)
			{
				continue;
			}
			EX_CARD.csOPRName = csOPRName;
			EX_CARD.csOPRNumber = csOPRNum;

			INT nCardType;
			spCard->GetCardTypeINT(&nCardType);
			if (nCardType == 4)
			{
				continue;
			}
		}
		CComPtr<ICxDbObjColl> pContinueList;
		spCardRef->GetContinuePageList(&pContinueList);
		LONG_PTR subpos(0);
		pContinueList->GetHeadPosition(&subpos);

		while (subpos)
		{
			CComPtr<ICxDbObject> pSubObj;
			pContinueList->GetNext(&subpos, &pSubObj);
			CComQIPtr<ICxCappCardRef> pContinueRef = pSubObj;
			CComQIPtr<ICxCappCard> spContinueCard;
			pContinueRef->GetCard(&spContinueCard);

			if (spContinueCard == NULL) continue;
			EXPORTDATA_CARD CONEX_CARD;
			DealWithCardData(spContinueCard, CONEX_CARD);
			EX_CARD.vDim.insert(EX_CARD.vDim.end(), CONEX_CARD.vDim.begin(), CONEX_CARD.vDim.end());
			INT nConCardType;
			spContinueCard->GetCardTypeINT(&nConCardType);
			if (nConCardType == 4)
			{
				break;
			}
		}

		vCardDimData.push_back(EX_CARD);
	}

	o_vRegu->swap(vCardDimData);
	return;
}

BOOL CCxCappExternDimToXmlDoc::TransReguDimDataToXmlFile(vector<EXPORTDATA_CARD> i_vRegu, CString csXmlFilePath)
{
	MSXML2::IXMLDOMDocument2Ptr pXmlPtr;
	MSXML2::IXMLDOMElementPtr pXmlRoot;
	::CoInitialize(NULL);
	HRESULT hRes;
	hRes = pXmlPtr.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if (FAILED(hRes))
	{
		return FALSE;
	}
	pXmlPtr->raw_createElement(_T("Info_DimensionDataToDownstreamSys"), &pXmlRoot);
	pXmlPtr->raw_appendChild(pXmlRoot, NULL);
	INT nNum = 1;

	for (auto v1 : i_vRegu)
	{
		CString csOPRNum = v1.csOPRNumber;
		CString csOPRName = v1.csOPRName;
		MSXML2::IXMLDOMNodePtr pDataListNode = pXmlRoot->appendChild(pXmlPtr->createElement(_T("Data")));
		MSXML2::IXMLDOMElementPtr pDataNode = pDataListNode;
		pDataNode->setAttribute(_T("OPRNumber"), (LPCTSTR)csOPRNum);
		pDataNode->setAttribute(_T("OPRName"), (LPCTSTR)csOPRName);

		vector<EXPORTDATA_DIM> vDim = v1.vDim;
		for (auto v2 : vDim)
		{
			CString csNum/*v2.csNum*/;
			{
				csNum.Format(_T("%d"), nNum);
				++nNum;
			}
			CString csKeyNum = v2.csKeyNum;
			CString csDimType = v2.csDimType;
			auto mTemp = m_mDisplayTxt.find(csDimType);
			if (mTemp != m_mDisplayTxt.end())
			{
				csDimType = mTemp->second;
			}
			MSXML2::IXMLDOMNodePtr pColListNode = pDataListNode->appendChild(pXmlPtr->createElement(_T("Col")));
			MSXML2::IXMLDOMElementPtr pColNode = pColListNode;
			pColNode->setAttribute(_T("num"), (LPCTSTR)csNum);
			pColNode->setAttribute(_T("KeyNum"), (LPCTSTR)csKeyNum);
			pColNode->setAttribute(_T("DimType"), (LPCTSTR)csDimType);

			map<CString, CString> mSingleData = v2.mSingleData;
			for (auto v3 : mSingleData)
			{
				CString csName = v3.first;
				CString csText = v3.second;
				CComBSTR bsText = (COLE2CT)csText;
				MSXML2::IXMLDOMNodePtr pDimDataListNode = pColListNode->appendChild(pXmlPtr->createElement(_T("DimData")));
				MSXML2::IXMLDOMElementPtr pDimDataNode = pDimDataListNode;
				pDimDataNode->setAttribute(_T("name"), (LPCTSTR)csName);
				pDimDataNode->put_text(bsText);
			}
		}
	}

	pXmlPtr->save(CComVariant::CComVariant(csXmlFilePath));
	pXmlPtr.Release();
	pXmlPtr = NULL;
	return TRUE;
}

BOOL CCxCappExternDimToXmlDoc::GetTwoDValue(CString i_csPoint, DOUBLE* o_dx, DOUBLE* o_dy)
{
	CString csPoint = i_csPoint;
	INT nSym1 = csPoint.Find(_T("("));
	INT nSym2 = csPoint.Find(_T(","));
	INT nSym3 = csPoint.Find(_T(")"));

	if (nSym1 != -1 && nSym2 != -1 && nSym3 != -1)
	{
		CString csdx = csPoint.Mid(nSym1 + 1, nSym2 - nSym1 - 1);
		CString csdy = csPoint.Mid(nSym2 + 1, nSym3 - nSym2 - 1);

		*o_dx = _ttof(csdx);
		*o_dy = _ttof(csdy);
		return TRUE;
	}

	return FALSE;
}

void CCxCappExternDimToXmlDoc::GetDimDataFromCardColSon(ICxCappCard* i_spNowCard, CString i_csRecordCol, CString i_csDataCol, vector<EXPORTDATA_DIM>* o_vColDimData)
{
	CComPtr<ICxCappCard> spCard = i_spNowCard;

	CComPtr<ICxCappTable> spTable1, spTable2;
	CComPtr<ICxCappColumn> spRecordCol, spDataCol;
	INT nColType1, nColType2;
	CComBSTR bsRecordCol, bsDataCol;
	bsRecordCol = (COLE2CT)i_csRecordCol;
	bsDataCol = (COLE2CT)i_csDataCol;
	i_spNowCard->GetColumnByName(bsRecordCol, &spTable1, &spRecordCol, &nColType1);
	i_spNowCard->GetColumnByName(bsDataCol, &spTable2, &spDataCol, &nColType2);
	BOOL bSame;
	CompareTable(spTable1, spTable2, &bSame);
	if (!bSame || !spRecordCol || !spDataCol)
	{
		return;
	}

	vector<EXPORTDATA_DIM> vDimTemp;
	INT nAllRowNum;
	spDataCol->GetColumnAllRowNum(&nAllRowNum);
	for (INT i = 0; i < nAllRowNum; i++)
	{
		CComPtr<ICxCappCell> spRecordCell, spDataCell;
		spRecordCol->GetCell(i, &spRecordCell);
		spDataCol->GetCell(i, &spDataCell);
		CComBSTR bsRecordTxt, bsDataTxt;
		spRecordCell->GetCellNonFormatContentText(&bsRecordTxt);
		spDataCell->GetCellNonFormatContentText(&bsDataTxt);
		CString csRecordTxt(bsRecordTxt);
		CString csDataTxt(bsDataTxt);

		CString csRecord, csType;
		map<CString, CString> mDimDataTemp;
		GetRecordInfo(csRecordTxt, &csRecord);
		GetDataInfo(csDataTxt, &csType, &mDimDataTemp);
		if (mDimDataTemp.empty())
		{
			continue;
		}
		EXPORTDATA_DIM EX_DIM;
		EX_DIM.csDimType = csType;
		EX_DIM.csKeyNum = csRecord;
		EX_DIM.csNum;
		EX_DIM.mSingleData = mDimDataTemp;
		vDimTemp.push_back(EX_DIM);
	}

	o_vColDimData->swap(vDimTemp);
	return;
}

void CCxCappExternDimToXmlDoc::GetDimDataFromCardPicSon(ICxCappCard* i_spNowCard, IBox* i_spBox, vector<EXPORTDATA_DIM>* o_vPicDimData)
{
	vector<DIMENSION_PIC> vNumTemp, vPicDimTemp;
	CComQIPtr<ICxDbPaper> spPaper;
	i_spNowCard->GetParentCard(&spPaper);
	CComPtr<ICxDbBlock> spPaperBlk;
	spPaper->GetRefBlk(&spPaperBlk);
	CComQIPtr<ICxDbObjMgr> spPaperMgr = spPaperBlk;
	LONG_PTR lPos;
	spPaperMgr->GetHeadStatusPosition(CXDBOBJSTATUS_VALID, &lPos);
	while (lPos)
	{
		CComPtr<ICxDbObject> spObj;
		spPaperMgr->GetStatusNext(CXDBOBJSTATUS_VALID, &lPos, &spObj);
		CComQIPtr<ICxDbBlkReference> spBlkRef = spObj;
		if (spBlkRef)
		{
			GUID GUID_CxPID = { 0xdf9bf670, 0x448d, 0x4d9f,{ 0x95, 0x14, 0x4e, 0x28, 0x95, 0xb7, 0xe7, 0x5c } };
			CComPtr<ICxExtDataObj> spExtDataPIDObj;
			CComBSTR bsName = _T("CX_CAPPNUM");
			spObj->GetExtDataObj(GUID_CxPID, bsName, &spExtDataPIDObj);
			if (spExtDataPIDObj)
			{
				LONG lID;
				CComQIPtr<ICxExtDataAccess> spAccessID = spExtDataPIDObj;
				spAccessID->get_ValueByLong(&lID);
				if (lID > 0)
				{
					GUID GUID_CxDimRowNum = { 0xcbc9afe9, 0xe592, 0x4e2a,{ 0xa6, 0xf3, 0x13, 0x6a, 0x5b, 0xc0, 0x93, 0x2c } };
					CComPtr<ICxExtDataObj> spExtDataDRNumObj;
					spObj->GetExtDataObj(GUID_CxDimRowNum, bsName, &spExtDataDRNumObj);
					if (spExtDataDRNumObj)
					{
						INT nDimRowNum;
						CComQIPtr<ICxExtDataAccess> spAccessDRNum = spExtDataDRNumObj;
						spAccessDRNum->get_ValueByInt(&nDimRowNum);
						CString csDimKeyNum = GetDimKey_Num(spObj);
						if (csDimKeyNum.GetLength())
						{
							DIMENSION_PIC DIM_PIC;
							CString csPID, csDimRowNum;
							csPID.Format(_T("%d"), lID);
							csDimRowNum.Format(_T("%d"), nDimRowNum);
							DIM_PIC.csPID = csPID;
							DIM_PIC.csDimRowNum = csDimRowNum;
							DIM_PIC.csNum = csDimKeyNum;
							vNumTemp.push_back(DIM_PIC);
							continue;
						}
					}
				}
			}
		}

		CComPtr<ICxDbDim2LnsAng> sp2LnsAngle;
		CComPtr<ICxDbDim3PtsAng> sp3PtAng;
		CComPtr<ICxDbDimGeoTol> spDimGeoTol;
		CComPtr<ICxDbDimRough> spDimRough;
		sp2LnsAngle = spObj;
		sp3PtAng = spObj;
		spDimGeoTol = spObj;
		spDimRough = spObj;
		CComQIPtr<ICxDbDimension> spDim = spObj;
		if (sp2LnsAngle || sp3PtAng || spDimGeoTol || spDimRough || spDim)
		{
			PID* pPID = NULL;
			spObj->GetPID((ULONG_PTR*)&pPID);
			LONG lID = pPID->lID;
			vector<DIMENSION_PIC> vDIM_PICTemp;
			GetObjRequiredData(spObj, &vDIM_PICTemp, i_spBox);
			for (auto v : vDIM_PICTemp)
			{
				CString csPID;
				csPID.Format(_T("%d"), lID);
				v.csPID = csPID;
				vPicDimTemp.push_back(v);
			}
			continue;
		}

		if (spBlkRef)
		{
			CComQIPtr<ICxDbBlock> spDbBlk1;
			spBlkRef->GetRefBlk(&spDbBlk1);
			CComQIPtr<ICxDbObjMgr> spDbMgr1 = spDbBlk1;
			LONG_PTR lPos1;
			spDbMgr1->GetHeadStatusPosition(CXDBOBJSTATUS_VALID, &lPos1);
			while (lPos1)
			{
				CComPtr<ICxDbObject> spObj1;
				spDbMgr1->GetStatusNext(CXDBOBJSTATUS_VALID, &lPos1, &spObj1);
				CComPtr<ICxDbDim2LnsAng> sp2LnsAngle1;
				CComPtr<ICxDbDim3PtsAng> sp3PtAng1;
				CComPtr<ICxDbDimGeoTol> spDimGeoTol1;
				CComPtr<ICxDbDimRough> spDimRough1;
				sp2LnsAngle1 = spObj1;
				sp3PtAng1 = spObj1;
				spDimGeoTol1 = spObj1;
				spDimRough1 = spObj1;
				CComQIPtr<ICxDbDimension> spDim1 = spObj1;
				if (sp2LnsAngle1 || sp3PtAng1 || spDimGeoTol1 || spDimRough1 || spDim1)
				{
					PID* pPID1 = NULL;
					spObj1->GetPID((ULONG_PTR*)&pPID1);
					LONG lID1 = pPID1->lID;
					vector<DIMENSION_PIC> vDIM_PICTemp1;
					GetObjRequiredData(spObj1, &vDIM_PICTemp1, i_spBox, spBlkRef);
					for (auto v1 : vDIM_PICTemp1)
					{
						CString csPID1;
						csPID1.Format(_T("%d"), lID1);
						v1.csPID = csPID1;
						vPicDimTemp.push_back(v1);
					}
					continue;
				}
				CComQIPtr<ICxDbBlkReference> spBlkRef2 = spObj1;
				if (spBlkRef2)
				{
					CComQIPtr<ICxDbBlock> spDbBlk2;
					spBlkRef2->GetRefBlk(&spDbBlk2);
					CComQIPtr<ICxDbObjMgr> spDbMgr2 = spDbBlk2;
					LONG_PTR lPos2;
					spDbMgr2->GetHeadStatusPosition(CXDBOBJSTATUS_VALID, &lPos2);
					while (lPos2)
					{
						CComPtr<ICxDbObject> spObj2;
						spDbMgr2->GetStatusNext(CXDBOBJSTATUS_VALID, &lPos2, &spObj2);
						CComPtr<ICxDbDim2LnsAng> sp2LnsAngle2;
						CComPtr<ICxDbDim3PtsAng> sp3PtAng2;
						CComPtr<ICxDbDimGeoTol> spDimGeoTol2;
						CComPtr<ICxDbDimRough> spDimRough2;
						sp2LnsAngle2 = spObj2;
						sp3PtAng2 = spObj2;
						spDimGeoTol2 = spObj2;
						spDimRough2 = spObj2;
						CComQIPtr<ICxDbDimension> spDim2 = spObj2;
						if (sp2LnsAngle2 || sp3PtAng2 || spDimGeoTol2 || spDimRough2 || spDim2)
						{
							PID* pPID2 = NULL;
							spObj2->GetPID((ULONG_PTR*)&pPID2);
							LONG lID2 = pPID2->lID;
							vector<DIMENSION_PIC> vDIM_PICTemp2;
							GetObjRequiredData(spObj2, &vDIM_PICTemp2, i_spBox, spBlkRef2);
							for (auto v2 : vDIM_PICTemp2)
							{
								CString csPID2;
								csPID2.Format(_T("%d"), lID2);
								v2.csPID = csPID2;
								vPicDimTemp.push_back(v2);
							}
							continue;
						}
					}
				}
			}
		}
	}

	for (auto v1 : vNumTemp)
	{
		for (INT i = 0; i < vPicDimTemp.size(); i++)
		{
			if (v1.JudgeSame(vPicDimTemp[i].csPID, vPicDimTemp[i].csDimRowNum))
			{
				vPicDimTemp[i].csNum = v1.csNum;
				break;
			}
		}
	}

	vector<EXPORTDATA_DIM> vEX_DIM;
	for (auto v : vPicDimTemp)
	{
		EXPORTDATA_DIM EX_DIM;
		EX_DIM.csDimType = v.csDimType;
		EX_DIM.csKeyNum = v.csNum;
		EX_DIM.csNum;
		EX_DIM.mSingleData = v.mDimData;
		vEX_DIM.push_back(EX_DIM);
	}

	o_vPicDimData->swap(vEX_DIM);
	return;
}

void CCxCappExternDimToXmlDoc::GetObjRequiredData(ICxDbObject* i_pObj, vector<DIMENSION_PIC>* o_vDIM_PIC, IBox* i_spBox, ICxDbBlkReference* i_pBlkRef)
{
	//2024sp0 cs
	CComPtr<ICxCappConfig> spConfig;
	CComQIPtr<ICxDocument> spDoc = GetCxDocument();
	GetConfigInfo(&spConfig, spDoc);
	BSTR m_bsNonDev;
	spConfig->GetNonDev(&m_bsNonDev);
	CString m_csNonDev(m_bsNonDev);

	CComPtr<ICxDbObject> spDimObj = i_pObj;
	CComPtr<ICxDbDim2LnsAng> sp2LnsAngle;
	CComPtr<ICxDbDim3PtsAng> sp3PtAng;
	CComPtr<ICxDbDimGeoTol> spDimGeoTol;
	CComPtr<ICxDbDimRough> spDimRough;
	CComQIPtr<ICxDbDimension> spDim;
	sp2LnsAngle = spDimObj;
	sp3PtAng = spDimObj;
	spDimGeoTol = spDimObj;
	spDimRough = spDimObj;
	spDim = spDimObj;

	CString csType;
	vector<CString> aGeoTols;
	CString csDimTxt;
	vector<map<CString, CString>> vDimensionInfos;
	map<CString, CString> mDimensionInfo;
	DOUBLE dx(0.0), dy(0.0), dz(0.0);
	if (spDimGeoTol)
	{
		vector<pair<DOUBLE, DOUBLE>> vGTCoords;
		vGTCoords = GetGeoTolsLocation(spDimGeoTol);
		DOUBLE dxTemp(0.0), dyTemp(0.0);
		for (auto v : vGTCoords)
		{
			dxTemp += v.first;
			dyTemp += v.second;
		}
		dx = dxTemp / vGTCoords.size();
		dy = dyTemp / vGTCoords.size();
		csType = _T("GeoTol");
		VARIANT_BOOL bAllRoundSymbol = VARIANT_FALSE;
		spDimGeoTol->get_AllRoundSymbol(&bAllRoundSymbol);
		bAllRoundSymbol == VARIANT_TRUE ? TRUE : FALSE;

		CComPtr<ICxGeoTolColl> spGeoTolList;
		spDimGeoTol->get_GeoTols(&spGeoTolList);
		if (spGeoTolList == NULL)
		{
			return;
		}
		LONG_PTR lPos;
		spGeoTolList->GetHeadPosition(&lPos);
		while (lPos)
		{
			CComPtr<ICxGeoTol> spGeoTol;
			spGeoTolList->GetNext(&lPos, &spGeoTol);
			if (spGeoTol == NULL)
			{
				continue;
			}
			CComBSTR bszDatum1, bszDatum2, bszDatum3;
			CComBSTR bszPostFix1, bszPostFix2, bszPreFix1, bszPreFix2;
			CComBSTR bszVal1, bszVal2;
			LONG nSymbol(0);
			spGeoTol->get_Datum1(&bszDatum1);
			spGeoTol->get_Datum2(&bszDatum2);
			spGeoTol->get_Datum3(&bszDatum3);
			spGeoTol->get_Symbol(&nSymbol);
			spGeoTol->get_TolePostfix1(&bszPostFix1);
			spGeoTol->get_TolePostfix2(&bszPostFix2);
			spGeoTol->get_TolePrefix1(&bszPreFix1);
			spGeoTol->get_TolePrefix2(&bszPreFix2);
			spGeoTol->get_ToleVal1(&bszVal1);
			spGeoTol->get_ToleVal2(&bszVal2);

			GeoTolGlyphInfo GeoTolInfo;
			GeoTolInfo.m_lSymbol = nSymbol;
			GeoTolInfo.m_sVal1 = bszVal1;
			GeoTolInfo.m_sVal2 = bszVal2;
			if (GeoTolInfo.m_lSymbol == 14)
			{
				continue;
			}
			GeoTolInfo.m_sTolePrefix1 = bszPreFix1;
			GeoTolInfo.m_sTolePrefix2 = bszPreFix2;
			GeoTolInfo.m_sTolePostfix1 = bszPostFix1;
			GeoTolInfo.m_sTolePostfix2 = bszPostFix2;
			GeoTolInfo.m_sTopNote = L"";
			GeoTolInfo.m_sDownNote = L"";
			GeoTolInfo.m_sBase1 = bszDatum1;
			GeoTolInfo.m_sBase2 = bszDatum2;
			GeoTolInfo.m_sBase3 = bszDatum3;
			GeoTolInfo.m_bAllRoundSymbol = bAllRoundSymbol;
			GeoTolInfo.m_iRank = 7;
			GeoTolInfo.m_dBaseVal = 200;
			GeoTolInfo.m_bIfSelF = FALSE;
			GeoTolInfo.m_dRatio = 0.876;
			aGeoTols.push_back(GenGeoTolFormatStr(GeoTolInfo, mDimensionInfo));
			CtrlChar2SpecialChar(mDimensionInfo[_T("sTolePrefix1")]);
			CtrlChar2SpecialChar(mDimensionInfo[_T("sTolePrefix2")]);
			NoteAnalysisForDownStreamSys(&mDimensionInfo[_T("sTolePostfix1")]);
			NoteAnalysisForDownStreamSys(&mDimensionInfo[_T("sTolePostfix2")]);
			vDimensionInfos.push_back(mDimensionInfo);
		}
	}
	else if (spDimRough)
	{
		pair<DOUBLE, DOUBLE> Coord = GetRoughLocation(spDimRough);
		dx = Coord.first;
		dy = Coord.second;
		csType = _T("RoughTol");
		csDimTxt = GenRoughFormatStr(spDimRough, mDimensionInfo);
		if (mDimensionInfo[_T("sUpVal")].GetLength())
		{
			CString sUpValueType = mDimensionInfo[_T("sUpVal")].Left(2);
			if (sUpValueType != _T("Ra") && sUpValueType != _T("Rz") && sUpValueType != _T("Ry"))
			{
				mDimensionInfo[_T("sUpVal")] = _T("Ra") + mDimensionInfo[_T("sUpVal")];
			}
		}
	}
	else if (sp2LnsAngle || sp3PtAng)
	{
		CComPtr<IXYZ> ptDim;
		spDim->get_LocPt(&ptDim);
		ptDim->GetXYZ(&dx, &dy, &dz);
		csType = _T("AngTol");
		csDimTxt = GenAngFormatStr(m_csNonDev, spDim, mDimensionInfo);
		CtrlChar2SpecialChar(mDimensionInfo[_T("sdBaseVal")]);
		CtrlChar2SpecialChar(mDimensionInfo[_T("sPrefix")]);
		CtrlChar2SpecialChar(mDimensionInfo[_T("sPostfix")]);
		CtrlChar2SpecialChar(mDimensionInfo[_T("sUpTol")]);
		CtrlChar2SpecialChar(mDimensionInfo[_T("sDownTol")]);
		NoteAnalysisForDownStreamSys(&mDimensionInfo[_T("sDownTol")]);
	}
	else if (spDim)
	{
		CComPtr<IXYZ> ptDim;
		spDim->get_LocPt(&ptDim);
		ptDim->GetXYZ(&dx, &dy, &dz);
		csType = _T("DimTol");
		csDimTxt = GenTolFormatStr(m_csNonDev, spDim, mDimensionInfo);
		CtrlChar2SpecialChar(mDimensionInfo[_T("sPrefix")]);
		CtrlChar2SpecialChar(mDimensionInfo[_T("sPostfix")]);
	}
	else
	{
		return;
	}

	CComQIPtr<IXYZ> spCoord;
	VARIANT_BOOL vbIn(0);
	CX_GENSUPPORTOBJECT(CX_SPT_XYZ, &spCoord);
	spCoord->PutXYZ(dx, dy, 0.);
	if (i_pBlkRef)
	{
		CComQIPtr<IMatrix> spMatrix;
		i_pBlkRef->GetMatrix(&spMatrix);
		CComQIPtr<IXYZ>spPtT;
		spCoord->TransToMatPln(spMatrix, &spPtT);
		i_spBox->IfPtIn(spPtT, &vbIn);
	}
	else
	{
		i_spBox->IfPtIn(spCoord, &vbIn);
	}

	if (vbIn == VARIANT_FALSE)
	{
		return;
	}

	vector<DIMENSION_PIC> vD_PICTemp;
	if (aGeoTols.size())
	{
		for (INT i = 0; i < aGeoTols.size(); i++)
		{
			DIMENSION_PIC DIM_PIC;
			DIM_PIC.csDimType = csType;
			CString csDimRowNum;
			csDimRowNum.Format(_T("%d"), i);
			DIM_PIC.csDimRowNum = csDimRowNum;
			DIM_PIC.mDimData = vDimensionInfos[i];
			vD_PICTemp.push_back(DIM_PIC);
		}
	}
	else
	{
		DIMENSION_PIC DIM_PIC;
		DIM_PIC.csDimType = csType;
		DIM_PIC.csDimRowNum = _T("0");
		DIM_PIC.mDimData = mDimensionInfo;
		vD_PICTemp.push_back(DIM_PIC);
	}

	o_vDIM_PIC->swap(vD_PICTemp);
}

CString CCxCappExternDimToXmlDoc::GetDimKey_Num(ICxDbObject* i_pObj)
{
	CComQIPtr<ICxDbBlkReference> spBlkRef = i_pObj;
	if (spBlkRef)
	{
		CComQIPtr<ICxDbBlock> spBlk;
		spBlkRef->GetRefBlk(&spBlk);
		CComQIPtr<ICxDbObjMgr> spTempMgr1 = spBlk;
		LONG_PTR pos1(0);
		spTempMgr1->GetHeadStatusPosition(CXDBOBJSTATUS_VALID, &pos1);
		while (pos1)
		{
			CComQIPtr<ICxDbObject> spDimObj1;
			spTempMgr1->GetStatusNext(CXDBOBJSTATUS_VALID, &pos1, &spDimObj1);
			CComQIPtr<ICxDbText> spTxt = spDimObj1;
			if (spTxt)
			{
				CComBSTR bsContent;
				spTxt->GetContents(&bsContent);
				CString csContent = bsContent;
				INT nNum = _wtoi(csContent);
				CString csNum;
				csNum.Format(_T("%d"), nNum);
				if (csNum == csContent && nNum > 0)
				{
					return csNum;
				}
			}

			CComQIPtr<ICxDbBlkReference> spBlkRef2 = spDimObj1;
			if (spBlkRef2)
			{
				CComQIPtr<ICxDbBlock> spBlk2;
				spBlkRef2->GetRefBlk(&spBlk2);
				CComQIPtr<ICxDbObjMgr> spTempMgr2 = spBlk2;
				LONG_PTR pos2(0);
				spTempMgr2->GetHeadStatusPosition(CXDBOBJSTATUS_VALID, &pos2);
				while (pos2)
				{
					CComQIPtr<ICxDbObject> spDimObj2;
					spTempMgr2->GetStatusNext(CXDBOBJSTATUS_VALID, &pos2, &spDimObj2);
					CComQIPtr<ICxDbText> spTxt2 = spDimObj2;
					if (spTxt2)
					{
						CComBSTR bsContent;
						spTxt2->GetContents(&bsContent);
						CString csContent = bsContent;
						INT nNum = _wtoi(csContent);
						CString csNum;
						csNum.Format(_T("%d"), nNum);
						if (csNum == csContent && nNum > 0)
						{
							return csNum;
						}
					}

					CComQIPtr<ICxDbBlkReference> spBlkRef3 = spDimObj2;
					if (spBlkRef3)
					{
						CComQIPtr<ICxDbBlock> spBlk3;
						spBlkRef2->GetRefBlk(&spBlk3);
						CComQIPtr<ICxDbObjMgr> spTempMgr3 = spBlk3;
						LONG_PTR pos3(0);
						spTempMgr3->GetHeadStatusPosition(CXDBOBJSTATUS_VALID, &pos3);
						while (pos3)
						{
							CComQIPtr<ICxDbObject> spDimObj3;
							spTempMgr2->GetStatusNext(CXDBOBJSTATUS_VALID, &pos3, &spDimObj3);
							CComQIPtr<ICxDbText> spTxt3 = spDimObj3;
							if (spTxt3)
							{
								CComBSTR bsContent;
								spTxt3->GetContents(&bsContent);
								CString csContent = bsContent;
								INT nNum = _wtoi(csContent);
								CString csNum;
								csNum.Format(_T("%d"), nNum);
								if (csNum == csContent && nNum > 0)
								{
									return csNum;
								}
							}
						}
					}
				}
			}
		}
	}
	return _T("");
}

BOOL CCxCappExternDimToXmlDoc::DealWithCardData(ICxCappCard* i_spCard, EXPORTDATA_CARD& o_EX_CARD)
{
	CComBSTR bsTemplateName;
	i_spCard->GetCardTemplateName(&bsTemplateName);
	CString csTemplateName(bsTemplateName);
	CString csOPRName, csOPRNum, csDataSource, csRecordCol, csDataCol;
	DOUBLE dx1(0.0), dx2(0.0), dy1(0.0), dy2(0.0);
	BOOL bTemp = FALSE;
	for (auto v : m_vDataChoice)
	{
		if (v.FINDTEMPLATE(csTemplateName))
		{
			csOPRName = v.csOPRName;
			csOPRNum = v.csOPRNum;
			csDataSource = v.csDataSource;
			csRecordCol = v.csRecordCol;
			csDataCol = v.csDataCol;
			dx1 = v.dx1;
			dx2 = v.dx2;
			dy1 = v.dy1;
			dy2 = v.dy2;
			bTemp = TRUE;
			break;
		}
	}
	if (!bTemp)
	{
		return bTemp;
	}
	EXPORTDATA_CARD EX_CARD;
	EX_CARD.csOPRName = csOPRName;
	EX_CARD.csOPRNumber = csOPRNum;
	vector<EXPORTDATA_DIM> vDim;
	if (csDataSource == _T("0"))
	{
		GetDimDataFromCardColSon(i_spCard, csRecordCol, csDataCol, &vDim);
	}
	else if (csDataSource == _T("1"))
	{
		CComPtr<IBox> spBox;
		CX_GENSUPPORTOBJECT(CX_SPT_BOX, &spBox);
		spBox->SetMaxPosCoord(dx2, dy2, 0.0);
		spBox->SetMinPosCoord(dx1, dy1, 0.0);
		GetDimDataFromCardPicSon(i_spCard, spBox, &vDim);
	}

	if (vDim.empty())
	{
		//EX_CARD.vDim = vDim;
		o_EX_CARD = EX_CARD;
		return bTemp;
	}
	DealWithDimData(vDim);

	EX_CARD.vDim = vDim;
	o_EX_CARD = EX_CARD;
	//vCardDimData.push_back(EX_CARD);
	return bTemp;
}

void CCxCappExternDimToXmlDoc::GetDataInfo(CString i_csDev, CString *o_csType, map<CString, CString>* o_mDevInfo)
{
	CString csType;
	map<CString, CString> mDevInfo;
	GetStrDimDataForDownStreamSys(i_csDev, &mDevInfo, &csType);

	*o_csType = csType;
	o_mDevInfo->swap(mDevInfo);
}

void CCxCappExternDimToXmlDoc::GetRecordInfo(CString i_csRecordTxt, CString* o_csRecord)
{
	vector<CString> vPicSym;
	FindPicSymAllIndeedForDownStreamSys(i_csRecordTxt, &vPicSym);
	if (vPicSym.size())
	{
		*o_csRecord = vPicSym.front();
	}
	else
	{
		*o_csRecord = i_csRecordTxt;
	}
}

void CCxCappExternDimToXmlDoc::DealWithDimData(vector<EXPORTDATA_DIM>& vDimData)
{
	CComBSTR bsRoot;
	CComPtr<ICxPathMng> spPathMng = GetPathMng();
	spPathMng->GetLocalRoot(&bsRoot);
	CString csXmlPath(bsRoot);
	TCHAR szLang[LANGSRTLEN] = {};
	CxGetLANGStr(szLang);
	CString  strLang = szLang;
	strLang.TrimLeft();
	strLang.TrimRight();
	if (strLang == _T("zh-CN"))
		csXmlPath += _T("\\Setting\\zh-CN\\DevInfoIndeed.xml");
	else if (strLang == _T("zh-TW"))
		csXmlPath += _T("\\Setting\\zh-TW\\DevInfoIndeed.xml");
	else
		csXmlPath += _T("\\Setting\\en-US\\DevInfoIndeed.xml");

	map<CString, map<CString, CString>> mIndeedDev;
	map<CString, CString> mDisplayTxt;
	FindXmlIndeedForDownStreamSys(&mIndeedDev, csXmlPath, &mDisplayTxt);
	m_mDisplayTxt.swap(mDisplayTxt);

	for (int i = 0; i < vDimData.size(); i++)
	{
		map<CString, CString> mIndeed;
		CString csType = vDimData[i].csDimType;
		auto pos = mIndeedDev.find(csType);
		if (pos != mIndeedDev.end())
		{
			mIndeed = pos->second;
		}
		else
		{
			return;
		}

		map<CString, CString> mShowDev;
		for (auto m1 : mIndeed)
		{
			for (auto m2 : vDimData[i].mSingleData)
			{
				if (m1.second == m2.first)
				{
					mShowDev.insert(make_pair(m1.first, m2.second));
				}
			}
		}

		vDimData[i].mSingleData = mShowDev;
	}
}

BOOL CCxCappExternDimToXmlDoc::GetCardOPRData(ICxCappCard* i_spCard, EXPORTDATA_CARD EX_CARD, CString* o_csOPRNameTxt, CString* o_csOPRNumTxt)
{
	CComBSTR bsOPRName, bsOPRNum;
	bsOPRName = (COLE2T)EX_CARD.csOPRName;
	bsOPRNum = (COLE2T)EX_CARD.csOPRNumber;

	CComPtr<ICxCappTable> spOPRNameTable, spOPRNumTable;
	CComPtr<ICxCappColumn> spOPRNameCol, spOPRNumCol;
	INT nOPRNameColType, nOPRNumColType;
	i_spCard->GetColumnByName(bsOPRName, &spOPRNameTable, &spOPRNameCol, &nOPRNameColType);
	i_spCard->GetColumnByName(bsOPRNum, &spOPRNumTable, &spOPRNumCol, &nOPRNumColType);
	if (!spOPRNameCol || !spOPRNumCol)
	{
		return FALSE;
	}

	CComPtr<ICxCappCell> spOPRNameCell, spOPRNumCell;
	spOPRNameCol->GetCell(0, &spOPRNameCell);
	spOPRNumCol->GetCell(0, &spOPRNumCell);
	if (!spOPRNameCell || !spOPRNumCell)
	{
		return FALSE;
	}

	CComBSTR bsOPRNameTxt, bsOPRNumTxt;
	spOPRNameCell->GetCellNonFormatContentText(&bsOPRNameTxt);
	spOPRNumCell->GetCellNonFormatContentText(&bsOPRNumTxt);
	CString csOPRNameTxt(bsOPRNameTxt), csOPRNumTxt(bsOPRNumTxt);

	*o_csOPRNameTxt = csOPRNameTxt;
	*o_csOPRNumTxt = csOPRNumTxt;
	return TRUE;
}
