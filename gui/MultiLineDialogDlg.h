// MultiLineDialogDlg.h : header file
//

#if !defined(AFX_MULTILINEDIALOGDLG_H__2FE2DB06_86BF_11D2_AD97_0060088E1F9C__INCLUDED_)
#define AFX_MULTILINEDIALOGDLG_H__2FE2DB06_86BF_11D2_AD97_0060088E1F9C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MultiLineListBox.h"
#include "afxwin.h"
extern "C"
{
#include "bible/pdbcommon.h"
#include "bible/pdbdecoder.h"
#include "bible/pdbhelper.h"
#include "bible/dictUtils.h"
#include "bible/memcacher.h"
};

/////////////////////////////////////////////////////////////////////////////
// CMultiLineDialogDlg dialog

class CMultiLineDialogDlg : public CDialog
{
// Construction
public:
	CMultiLineDialogDlg(CWnd* pParent = NULL);	// standard constructor
	~CMultiLineDialogDlg();
// Dialog Data
	//{{AFX_DATA(CMultiLineDialogDlg)
	enum { IDD = IDD_MULTILINEDIALOG_DIALOG };
	CMultiLineListBox	m_ctrList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiLineDialogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CFont m_UseFont;
	CRect m_rect;

	FILE* fp;
	struct PDBLayout* pdblayout;
	struct WordBook*  pWordBook;
	MEMORYCACHER* pMemoryCache;

	// Generated message map functions
	//{{AFX_MSG(CMultiLineDialogDlg)
	virtual BOOL OnInitDialog();

	void InitBookList();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_combo_book;
	CComboBox m_combo_chapter;
	afx_msg void OnCbnSelchangeComboBook();
	afx_msg void OnCbnSelchangeComboChapter();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CStatic m_version_info;
	afx_msg void OnBnClickedButtonPrevBook();
	afx_msg void OnBnClickedButtonNextBook();
	afx_msg void OnBnClickedButtonPrevChap();
	afx_msg void OnBnClickedButtonNextChap();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTILINEDIALOGDLG_H__2FE2DB06_86BF_11D2_AD97_0060088E1F9C__INCLUDED_)
