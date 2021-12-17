
// UIPRDlg.h: 头文件
//

#pragma once
#include "EPR1.h"
#include "opencv2/highgui/highgui_c.h"


// CUIPRDlg 对话框
class CUIPRDlg : public CDialogEx
{
// 构造
public:
	CUIPRDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UIPR_DIALOG };
#endif
	cv::Ptr<cv::ml::SVM> svm1;
	cv::Ptr<cv::ml::SVM> svm2;
	EPR member;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CString inforshow;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnStnClickedStatic2();
	afx_msg void OnClickedStatic3();
};

