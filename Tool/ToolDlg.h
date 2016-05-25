
// ToolDlg.h : 头文件
//

#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "../HBase/HBase.h"
using namespace Humble;

// CToolDlg 对话框
class CToolDlg : public CDialogEx
{
// 构造
public:
	CToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
    void setStyle(void);
    void getAllFileName(const char *pszParentPathName, std::list<std::string> &lstFileName);
    void setCommand(void);
    void initParser(void);

// 实现
protected:
	HICON m_hIcon;
    std::list<std::string> m_lstCommName;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnMenuClick(UINT nID);
    afx_msg void OnBnClickedButton4();
    afx_msg void OnCbnSelchangeCombo2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

public:
    CFont m_objFont;
    CEdit m_CtrInput;
    CListCtrl m_CtrOutput;
    CIPAddressCtrl m_CtrIp;
    CEdit m_CtrPort;
    CComboBox m_CtrParser;
    CComboBox m_CtrCommand;
    CButton m_CtrLink;
    CButton m_CtrClose;
    CStatic m_CtrTotalNum;    
};
