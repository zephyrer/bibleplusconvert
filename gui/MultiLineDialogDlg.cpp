// MultiLineDialogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MultiLineDialog.h"
#include "MultiLineDialogDlg.h"

extern "C"
{
#include "bible/pdbcommon.h"
#include "bible/pdbdecoder.h"
#include "bible/pdbhelper.h"
#include "bible/dictUtils.h"
#include "bible/memcacher.h"
};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiLineDialogDlg dialog

CMultiLineDialogDlg::CMultiLineDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMultiLineDialogDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMultiLineDialogDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	this->pdblayout = NULL;
	this->pWordBook = NULL;
	this->fp        = NULL;
	this->pMemoryCache = NULL;
}

CMultiLineDialogDlg::~CMultiLineDialogDlg()
{
	if (this->pdblayout != NULL)
	{
		FreePDBFile(this->pdblayout);
		this->pdblayout = NULL;
	}

	if (this->pWordBook != NULL)
	{
		WordBook_fini(this->pWordBook);
		this->pWordBook = NULL;
	}

	if (this->fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}

	if (this->pMemoryCache != NULL)
	{
		memcacher_fini(this->pMemoryCache);
		this->pMemoryCache = NULL;
	}
}

void CMultiLineDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMultiLineDialogDlg)
	DDX_Control(pDX, IDC_LIST, m_ctrList);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_BOOK, m_combo_book);
	DDX_Control(pDX, IDC_COMBO_CHAPTER, m_combo_chapter);
	DDX_Control(pDX, IDC_STATIC_VERSION, m_version_info);
}

BEGIN_MESSAGE_MAP(CMultiLineDialogDlg, CDialog)
	//{{AFX_MSG_MAP(CMultiLineDialogDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_BOOK, &CMultiLineDialogDlg::OnCbnSelchangeComboBook)
	ON_CBN_SELCHANGE(IDC_COMBO_CHAPTER, &CMultiLineDialogDlg::OnCbnSelchangeComboChapter)
	ON_BN_CLICKED(IDC_BUTTON_PREV_BOOK, &CMultiLineDialogDlg::OnBnClickedButtonPrevBook)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_BOOK, &CMultiLineDialogDlg::OnBnClickedButtonNextBook)
	ON_BN_CLICKED(IDC_BUTTON_PREV_CHAP, &CMultiLineDialogDlg::OnBnClickedButtonPrevChap)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_CHAP, &CMultiLineDialogDlg::OnBnClickedButtonNextChap)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiLineDialogDlg message handlers

BOOL CMultiLineDialogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//m_ctrList.SetFont(&m_UseFont,   TRUE); 
	m_ctrList.SetFont("微软雅黑", 24);

	InitBookList();

	OnCbnSelchangeComboBook();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMultiLineDialogDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMultiLineDialogDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMultiLineDialogDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMultiLineDialogDlg::OnCbnSelchangeComboBook()
{
	// TODO: Add your control notification handler code here
	uint16_t nrOfChapters= 0u;
	int i = 0;
	int selbook = 0;

	if (this->pdblayout != NULL)
	{
		selbook = m_combo_book.GetCurSel() + 1;
		nrOfChapters = GetNumberOfChapters(this->pdblayout,selbook);

		m_combo_chapter.ResetContent();

		CString buffer;
		for (i = 0;i < nrOfChapters;i++)
		{
			buffer.Format("%d",i + 1);
			m_combo_chapter.AddString(buffer);
		}
		m_combo_chapter.SetCurSel(0);

		OnCbnSelchangeComboChapter();
	}
}

void CMultiLineDialogDlg::OnCbnSelchangeComboChapter()
{
	// TODO: Add your control notification handler code here
	static uint8_t buffer[4096];

	uint16_t nrOfVerses= 0u;
	int i = 0;
	int selbook = 0;
	int selchapter = 0;
	int verseToView = 0;
	struct VerseOffset verseOffset;
	struct VerseData verseData;
	const MCBLOB* pBLOB = NULL;
	MCBLOB  newBLOB;

	if ((this->pdblayout != NULL) && (this->pWordBook != NULL))
	{
		selbook = m_combo_book.GetCurSel() + 1;
		selchapter = m_combo_chapter.GetCurSel() + 1;

		nrOfVerses = GetNumberOfVerses(this->pdblayout,selbook,selchapter);
		if (memcacher_pull(this->pMemoryCache, selbook, &pBLOB) == MC_OK)
		{
			verseData.verseBuffer = (uint16_t*)pBLOB->pBuffer;
			verseData.nrOfWords = pBLOB->nrOfBytes / 2;
		}
		else
		{
			pBLOB = NULL;
			verseData = DecompressBookAllVerseData(this->pdblayout,selbook,fp);
		}

		m_ctrList.ResetContent();

		//Decode word index data to text data.
		if ((verseData.verseBuffer != NULL) &&
			(verseData.nrOfWords > 0))
		{   
			for (i = 1;i <= nrOfVerses;i++)
			{
				verseToView = i;

				memset(buffer, 0, 4096);

				verseOffset = GetVerseOffsetFromPDB(
					this->pdblayout,
					selbook,
					selchapter,
					verseToView);

				if (verseOffset.offset != VERSEOFFSET_INVAILD)
				{
					DecompressVerse(
						pdblayout,
						verseData.verseBuffer,
						verseData.nrOfWords,
						verseOffset.offset,
						verseOffset.nrOfWords,
						this->pWordBook,
						fp,
						buffer);
					m_ctrList.AddString((char*)buffer);
				}
			}

			if (pBLOB == NULL)
			{
				newBLOB.pBuffer = (unsigned char*)verseData.verseBuffer;
				newBLOB.nrOfBytes = verseData.nrOfWords * 2;
				memcacher_push(this->pMemoryCache,selbook,&newBLOB);
			}
		}

		m_ctrList.Commit();
	}
}

void CMultiLineDialogDlg::InitBookList()
{
	//Set combo
	uint16_t nrOfBooks= 0u;

	int i = 0;

	fp = fopen("cuvnp_GBK.pdb", "rb");

	if (fp == NULL)
	{
		CFileDialog openfile(TRUE);
		CString filename;
		
		if (openfile.DoModal() == IDOK)
		{
			filename = openfile.GetPathName();
			fp = fopen(filename, "rb");
		}
		else
		{
			SendMessage(WM_CLOSE);
		}
	}

	if (fp != NULL)
	{
		fseek(fp,0,SEEK_SET);
		this->pdblayout = DecodePDBFile(fp);

		if (this->pdblayout != NULL)
		{
			//Optimize word.
			this->pWordBook = BuildWordDictInRam(this->pdblayout, fp);
			if (this->pWordBook == NULL)
			{
				printf("Error build word dict!\n");
				FreePDBFile(this->pdblayout);
				this->pdblayout = NULL;
			}
		}

		if (this->pdblayout != NULL)
		{
			m_version_info.SetWindowText((char*)pdblayout->version.versionInfo);

			nrOfBooks = this->pdblayout->version.totalBooks;
			pMemoryCache = memcacher_init(nrOfBooks);

			m_combo_book.ResetContent();

			for (i = 0;i < nrOfBooks;i++)
			{
				m_combo_book.AddString(
					(char*)(this->pdblayout->version.bookBriefInfo[i].complexName));
			}
			m_combo_book.SetCurSel(0);
		}
	}
}

void CMultiLineDialogDlg::OnSize(UINT nType, int cx, int cy)
{
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_LIST);     //获取控件句柄
	if((pWnd) && (nType!=SIZE_MINIMIZED))//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
	{
		CRect rect;   //获取控件变化前大小
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标
		//cx/m_rect.Width()为对话框在横向的变化比例

		if ((m_rect.Width() == 0) || (m_rect.Height() == 0))
		{
		}
		else
		{
			rect.left=rect.left*cx/m_rect.Width();/////调整控件大小
			rect.right=rect.right*cx/m_rect.Width();
			rect.top=rect.top*cy/m_rect.Height();
			rect.bottom=rect.bottom*cy/m_rect.Height();
			pWnd->MoveWindow(rect);//设置控件大小
		}
	}
	GetClientRect(&m_rect);//将变化后的对话框大小设为旧大小
}
void CMultiLineDialogDlg::OnBnClickedButtonPrevBook()
{
	// TODO: Add your control notification handler code here
	int selBook = 0;
	selBook = m_combo_book.GetCurSel();

	if (selBook > 0)
	{
		m_combo_book.SetCurSel(selBook - 1);
		OnCbnSelchangeComboBook();
	}
}

void CMultiLineDialogDlg::OnBnClickedButtonNextBook()
{
	// TODO: Add your control notification handler code here
	int selBook = 0;
	selBook = m_combo_book.GetCurSel();

	if (selBook < this->pdblayout->version.totalBooks - 1)
	{
		m_combo_book.SetCurSel(selBook + 1);
		OnCbnSelchangeComboBook();
	}
}

void CMultiLineDialogDlg::OnBnClickedButtonPrevChap()
{
	// TODO: Add your control notification handler code here
	int selChaper = 0;
	selChaper = m_combo_chapter.GetCurSel();

	if (selChaper > 0)
	{
		m_combo_chapter.SetCurSel(selChaper - 1);
		OnCbnSelchangeComboChapter();
	}
}

void CMultiLineDialogDlg::OnBnClickedButtonNextChap()
{
	// TODO: Add your control notification handler code here
	int selChaper = 0;
	selChaper = m_combo_chapter.GetCurSel();

	if (selChaper < m_combo_chapter.GetCount() - 1)
	{
		m_combo_chapter.SetCurSel(selChaper + 1);
		OnCbnSelchangeComboChapter();
	}
}
