#if !defined(AFX_MULTILINELISTBOX_H__4725BE30_2395_11E0_AE6C_005056C00008__INCLUDED_)
#define AFX_MULTILINELISTBOX_H__4725BE30_2395_11E0_AE6C_005056C00008__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MultiLineListBox.h : header file
//
#include <vector>
using std::vector;

/////////////////////////////////////////////////////////////////////////////
// CMultiLineListBox
/*
 *	Sample code
 *  @code
 *		AddString(...);
 *		AddString(...);
 *		AddString(...);
 *		Commit();
 *  @endcode
 */

class CMultiLineListBox : public CListBox
{
//Constructor/Destructor
public:
	CMultiLineListBox();
	CMultiLineListBox(
		COLORREF colorBackgroundOdd,
		COLORREF colorBackgroundEven,
		COLORREF colorBackgroundSelect,
		COLORREF colorTextNotSelect,
		COLORREF colorTextSelected,
		COLORREF colorIndexText);
	~CMultiLineListBox();

//User interface
public:
	//Just use AddString, not use InsertString, This is one commit.
	//If you want to modify it, better ResetContent, AddString again.
	void AddString(LPCTSTR lpszItem);
	void Commit();
	void Rollback();

	//Delete string from index.
	int DeleteString(UINT nIndex);

	//Find a string.
	int FindString(int nStartAfter, LPCTSTR lpszItem) const;

	//Clear all content
	void ResetContent();

	//Set user font, must call from insert.
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
	void SetFont(LPSTR fontname, int fontheight);

//Inner interface.
private:
	int CalculateStringHeight(CString& buffer, CRect& rcLabel ) const;

//Overrides
public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);

//Data
private:
	//Max height and width for each item
	int nMaxItemWidth;
	int nMaxItemHeight;
	int nTextPading;
	int nIndexWidth;

	//Color for item.
	COLORREF colorBackgroundHigh;
	COLORREF colorBackgroundLow;
	COLORREF colorBackgroundSelect;
	COLORREF colorTextNotSelect;
	COLORREF colorTextSelected;
	COLORREF colorIndexText;

	//Data contain.
	vector<CString>* pitemlist;

	//Font.
	CFont* pFont;
	CFont  m_UseFont;

//Message interface
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_MULTILINELISTBOX_H__BF8E1EC2_833B_11D2_8BE7_A813DF000000__INCLUDED_)
