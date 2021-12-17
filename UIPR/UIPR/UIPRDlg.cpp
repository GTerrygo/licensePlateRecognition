
// UIPRDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "UIPR.h"
#include "UIPRDlg.h"
#include "afxdialogex.h"
#pragma once

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;
using namespace cv;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{

}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUIPRDlg 对话框
void CUIPRDlg::DoDataExchange(CDataExchange* pDX)
{
	// 处理MFC默认的数据交换   
	CDialogEx::DoDataExchange(pDX);
	// 处理控件IDC_SUMMAND_EDIT和变量m_editSummand之间的数据交换   


	DDX_Text(pDX, IDC_EDIT1, inforshow);
}


CUIPRDlg::CUIPRDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UIPR_DIALOG, pParent)
	, inforshow(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


BEGIN_MESSAGE_MAP(CUIPRDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CUIPRDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CUIPRDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, &CUIPRDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON2, &CUIPRDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CUIPRDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CUIPRDlg::OnBnClickedButton4)

END_MESSAGE_MAP()


// CUIPRDlg 消息处理程序

BOOL CUIPRDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。
	inforshow = " ";
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
		cv::namedWindow("view", cv::WINDOW_AUTOSIZE);
		HWND hWnd = (HWND)  cvGetWindowHandle("view");
		HWND hParent = ::GetParent(hWnd);
		::SetParent(hWnd, GetDlgItem(IDC_STATIC)->m_hWnd);
		::ShowWindow(hParent, SW_HIDE);

		cv::namedWindow("view2", cv::WINDOW_AUTOSIZE);
		hWnd = (HWND)cvGetWindowHandle("view2");
		hParent = ::GetParent(hWnd);
		::SetParent(hWnd, GetDlgItem(IDC_STATIC_p)->m_hWnd);
		::ShowWindow(hParent, SW_HIDE);

	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUIPRDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUIPRDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUIPRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUIPRDlg::OnBnClickedOk()
{
	CString picPath;   //定义图片路径变量
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
		NULL, this);   //选择文件对话框
	if (dlg.DoModal() == IDOK)
	{
		picPath = dlg.GetPathName();  //获取图片路径
	}
	//CString to string  使用这个方法记得字符集选用“使用多字节字符”，不然会报错
	String picpath=picPath.GetBuffer(0);
	

	cv::Mat image = imread(picpath);
	member=EPR(image);
	cv::Mat imagedst;
	//以下操作获取图形控件尺寸并以此改变图片尺寸
	CRect rect;
	GetDlgItem(IDC_STATIC)->GetClientRect(&rect);
	cv::Rect dst(rect.left, rect.top, rect.right, rect.bottom);
	cv::resize(image, imagedst, Size(rect.Width(), rect.Height()));
    cv::imshow("view", imagedst);
	
	
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}





void CUIPRDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString svm1Path;   
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
		NULL, this);   //选择文件对话框
	if (dlg.DoModal() == IDOK)
	{
		svm1Path = dlg.GetPathName();  //获取路径
	}
	//CString to string  使用这个方法记得字符集选用“使用多字节字符”，不然会报错
	String svm1path = svm1Path.GetBuffer(0);

	svm1 = ml::SVM::load(svm1path);
	// 将被加数和加数的加和赋值给m_editSum   
	if (svm1.empty()==0)
	    inforshow = "load svm1 successfully";
	else
		inforshow = "load svm1 fail";

	// 根据各变量的值更新相应的控件。和的编辑框会显示m_editSum的值   
	UpdateData(FALSE);
}


void CUIPRDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CUIPRDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString svm2Path;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
		NULL, this);   //选择文件对话框
	if (dlg.DoModal() == IDOK)
	{
		svm2Path = dlg.GetPathName();  //获取路径
	}
	//CString to string  使用这个方法记得字符集选用“使用多字节字符”，不然会报错
	String svm2path = svm2Path.GetBuffer(0);

	svm2 = ml::SVM::load(svm2path);
	// 将被加数和加数的加和赋值给m_editSum   
	if (svm2.empty() == 0)
		inforshow = "load svm2 successfully";
	else
		inforshow = "load svm2 fail";

	// 根据各变量的值更新相应的控件。和的编辑框会显示m_editSum的值   
	UpdateData(FALSE);
}


void CUIPRDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	member.setParameters(Size(3, 11), Size(15, 3));
	member.preprocess();
	member.searchPlate(svm1);
	Mat allROI;
	if (member.ROI.size()==0)
		allROI = Mat::zeros(36, 136, member.srcImage.type());
	else
	{
		allROI = Mat::zeros(36, int(136 * member.ROI.size()), member.srcImage.type());
		for (int i = 0; i < member.ROI.size(); i++)
		{
			member.ROI[i].copyTo(allROI(Rect(i * 136, 0, 136, 36)));
		}
	}
	cv::Mat imagedst;
	//以下操作获取图形控件尺寸并以此改变图片尺寸
	CRect rect;
	GetDlgItem(IDC_STATIC_p)->GetClientRect(&rect);
	cv::Rect dst(rect.left, rect.top, rect.right, rect.bottom);
	cv::resize(allROI, imagedst, Size(rect.Width(), rect.Height()));
	cv::imshow("view2", imagedst);
}




void CUIPRDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	member.charObtain(svm2);
	if (member.Plate.data() != 0)
	{
		inforshow = " ";
		for (int j = 0; j < member.ROI.size(); j++)
		{
			inforshow= inforshow+member.Plate[j].c_str();
		}
	}
	else
		inforshow= "未能正确识别车牌" ;
	UpdateData(FALSE);
}


void CUIPRDlg::OnStnClickedStatic2()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CUIPRDlg::OnClickedStatic3()
{
	// TODO: 在此添加控件通知处理程序代码
}
