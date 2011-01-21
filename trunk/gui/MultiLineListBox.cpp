// MultiLineListBox.cpp : implementation file
//

#include "stdafx.h"
#include "MultiLineListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiLineListBox

BEGIN_MESSAGE_MAP(CMultiLineListBox, CListBox)
	//{{AFX_MSG_MAP(CMultiLineListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiLineListBox message handlers

CMultiLineListBox::CMultiLineListBox()
{
	this->nMaxItemWidth  = 0;
	this->nMaxItemHeight = 0;
	this->nTextPading = 3;
	this->nIndexWidth = 40;
	this->colorBackgroundHigh	= RGB(230,230,250);
	this->colorBackgroundLow	= RGB(255,255,255);
	this->colorBackgroundSelect = RGB(32,128,255);

	this->colorTextNotSelect	= RGB(0,0,0);
	this->colorTextSelected		= RGB(255,255,255);
	this->colorIndexText		= RGB(255,128,0);

	this->pitemlist = new vector<CString>;

	this->pFont = NULL;
}

CMultiLineListBox::CMultiLineListBox(
				  COLORREF colorBackgroundHigh,
				  COLORREF colorBackgroundLow,
				  COLORREF colorBackgroundSelect,
				  COLORREF colorTextNotSelect,
				  COLORREF colorTextSelected,
				  COLORREF colorIndexText)
{
	this->nMaxItemWidth  = 0;
	this->nMaxItemHeight = 0;
	this->nTextPading = 3;
	this->nIndexWidth = 40;
	this->colorBackgroundHigh	= colorBackgroundHigh;
	this->colorBackgroundLow	= colorBackgroundLow;
	this->colorBackgroundSelect = colorBackgroundSelect;

	this->colorTextNotSelect	= colorTextNotSelect;
	this->colorTextSelected		= colorTextSelected;
	this->colorIndexText		= colorIndexText;

	this->pitemlist = new vector<CString>;

	this->pFont = NULL;
}


CMultiLineListBox::~CMultiLineListBox()
{
	delete pitemlist;
}

void CMultiLineListBox::AddString(LPCTSTR lpszItem)
{
	CString buffer(lpszItem);

	// get rectBlock area...
	if (nMaxItemWidth == 0)
	{
		CRect crlist;
		GetClientRect(&crlist);
		nMaxItemWidth = crlist.Width() + 5;
	}

	// get the max string height
	CRect rcLabel(
		nIndexWidth + nTextPading,
		nTextPading,
		nMaxItemWidth - nTextPading,
		0);
	int itemHeight = 0;
	itemHeight = CalculateStringHeight(buffer, rcLabel);

	if (itemHeight > nMaxItemHeight)
	{
		nMaxItemHeight = itemHeight + nTextPading;
	}

	// save cstring in vector
	pitemlist->push_back(buffer);
}

void CMultiLineListBox::Commit()
{
	vector<CString>::iterator itemiterator;

	for (itemiterator = pitemlist->begin();
		itemiterator != pitemlist->end();
		itemiterator++)
	{
		CString buffer(*itemiterator);
		CListBox::AddString(buffer);
	}
}

void CMultiLineListBox::Rollback()
{
	delete pitemlist;
	pitemlist = new vector<CString>;
}

int CMultiLineListBox::DeleteString(UINT nIndex)
{
	pitemlist->erase(pitemlist->begin() + nIndex);
	return CListBox::DeleteString(nIndex);
}

int CMultiLineListBox::FindString(
	int nStartAfter, LPCTSTR lpszItem) const
{
	//Because clistbox findstring can not find the right string.
	vector<CString>::iterator itemiterator;
	int nIndex = 0;

	for (itemiterator = pitemlist->begin() + nStartAfter;
		itemiterator != pitemlist->end();
		itemiterator++, nIndex++)
	{
		if(*itemiterator == lpszItem)
		{
			return nIndex;
		}
		else
		{
			continue;
		}
	}

	return LB_ERR;
}

void CMultiLineListBox::ResetContent()
{
	delete pitemlist;
	pitemlist = new vector<CString>;

	CListBox::ResetContent();
}

void CMultiLineListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	// all items are of fixed size
	// must use LBS_OWNERDRAWVARIABLE for this to work
	// Using the flags below, calculate the required rectangle for 
	// the text and set the item height for this specific item based
	// on the return value (new height).
	lpMIS->itemHeight = nMaxItemHeight;
}

void CMultiLineListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	COLORREF rColor;
	rColor = ((lpDIS->itemID % 2) == 0)?
		colorBackgroundHigh:colorBackgroundLow;
	CBrush brush(rColor);

	CString sLabel(pitemlist->at(lpDIS->itemID));

	CString sIndex;
	sIndex.Format(" %2d",lpDIS->itemID + 1);

	CRect rectBlock(
		lpDIS->rcItem.left,
		lpDIS->rcItem.top,
		lpDIS->rcItem.right,
		lpDIS->rcItem.bottom);

	CRect rectText(
		lpDIS->rcItem.left + nIndexWidth + nTextPading,
		lpDIS->rcItem.top + nTextPading,
		lpDIS->rcItem.right - nTextPading,
		lpDIS->rcItem.bottom - nTextPading);

	CRect rectIndex(
		lpDIS->rcItem.left + nTextPading,
		lpDIS->rcItem.top + nTextPading,
		lpDIS->rcItem.left + nIndexWidth,
		lpDIS->rcItem.bottom - nTextPading);
	
	// draw label and index
	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item selected
		CBrush labelBrush(colorBackgroundSelect);
		pDC->FillRect(&rectBlock,&labelBrush);

		pDC->SetTextColor(colorTextSelected);
		pDC->SetBkColor(colorBackgroundSelect);
		pDC->DrawText(sLabel, -1, &rectText, DT_WORDBREAK);

		rectBlock = lpDIS->rcItem;
		pDC->SetTextColor(colorIndexText);
		pDC->DrawText(sIndex, -1, &rectIndex, DT_WORDBREAK);
	}
	else if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		// item brought into box
		pDC->SetBkColor(rColor);
		pDC->FillRect(&rectBlock,&brush);

		pDC->SetTextColor(colorTextNotSelect);
		pDC->SetBkColor(rColor);
		pDC->DrawText(sLabel, -1, &rectText, DT_WORDBREAK);

		rectBlock = lpDIS->rcItem;
		pDC->SetTextColor(colorIndexText);
		pDC->DrawText(sIndex, -1, &rectIndex, DT_WORDBREAK);
	}
	else if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		// item deselected		
		pDC->SetBkColor(rColor);
		pDC->FillRect(&rectBlock,&brush);

		pDC->SetTextColor(colorTextNotSelect);
		pDC->SetBkColor(rColor);
		pDC->DrawText(sLabel, -1, &rectText, DT_WORDBREAK);

		rectBlock = lpDIS->rcItem;
		pDC->SetTextColor(colorIndexText);
		pDC->DrawText(sIndex, -1, &rectIndex, DT_WORDBREAK);
	}

	CRect rectLine(
		lpDIS->rcItem.left,
		lpDIS->rcItem.bottom - 1,
		lpDIS->rcItem.right,
		lpDIS->rcItem.bottom
		);

	// draw separe line
	CBrush lineBrush(RGB(0,0,0));
	pDC->FillRect(&rectLine,&lineBrush);
}

void CMultiLineListBox::SetFont(
	CFont* pFont, BOOL bRedraw)
{
	//Standard interface
	this->pFont = pFont;
	CListBox::SetFont(pFont,bRedraw);
}

void CMultiLineListBox::SetFont(LPSTR fontname, int fontheight)
{
	//Custom interface, easy for use.
	//Because I just care about fontname and fontheight.
	LOGFONT   lf; 

	//Clear font struct.
	memset(&lf,   0,   sizeof(LOGFONT));

	//Fill font description.
	lf.lfHeight = fontheight;
	strcpy_s(lf.lfFaceName,   fontname);

	//Create font.
	m_UseFont.CreateFontIndirect(&lf); 

	//Select font.
	SetFont(&m_UseFont,   TRUE);  
}

int CMultiLineListBox::CalculateStringHeight(
	CString& buffer, CRect& rcLabel ) const
{
	int itemHeight = 0;
	HDC hDC = ::GetDC(NULL); 
	if (pFont)
	{
		SelectObject(hDC, pFont->GetSafeHandle()); 
	}

	itemHeight = DrawText(hDC, buffer, -1, rcLabel, DT_WORDBREAK | DT_CALCRECT);
	::ReleaseDC(NULL, hDC);

	return itemHeight;
}

BOOL CMultiLineListBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// This make sure, roll mouse whell will change only one line.
	// roll down is -
	// roll up is +
	int topindex = CListBox::GetTopIndex();
	int nrcount  = CListBox::GetCount();
	
	if (zDelta > 0)
	{
		topindex = (topindex > 0)?topindex- 1:0;
	}
	else
	{
		topindex = (topindex + 1 < nrcount)?topindex + 1:topindex;
	}

	CListBox::SetTopIndex(topindex);

	//Avoid warning
	nFlags = nFlags;
	pt = pt;

	return TRUE;
}

