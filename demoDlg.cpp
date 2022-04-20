
// demoDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "demo.h"
#include "demoDlg.h"
#include "afxdialogex.h"
#include"func.h"
//#include<iostream>
#include<cstring>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int* number1;
int* number2;
int* m;

//int 1280 = 1280;
//int 960 = 960;
//int 1280 = 1280;

void CdemoDlg::DrawImage()
{
	CRect rct;
	GetDlgItem(pic)->GetClientRect(&rct);
	int dstW = rct.Width();
	int dstH = rct.Height();
	CDC* pDC = GetDlgItem(pic)->GetDC();
	{
		pDC->SetStretchBltMode(COLORONCOLOR);
		m_image.Draw(pDC->GetSafeHdc(), 0, 0, dstW, dstH);
	}
	ReleaseDC(pDC);
}

int CdemoDlg::OnStreamCB(MV_IMAGE_INFO* pInfo)
{
	MVInfo2Image(m_hCam, pInfo, &m_image);
	
	unsigned char* pDst = (unsigned char*)m_image.GetBits(); //获取原图
	unsigned char* ptemp;	//3个暂存空间，没有实际含义
	unsigned char* ptemp2;
	unsigned char* ptemp3;

	int num1 = 0, num2 = 0; //最后计数时会用到
	int number = 0;
	
	m = &number;
	number1 = &num1;
	number2 = &num2;
	ptemp = new unsigned char[1280 * 960]; //分配暂存空间
	ptemp2 = new unsigned char[1280 * 960];
	ptemp3 = new unsigned char[1280 * 960];
	//original_image = new unsigned char[1280 * 960];
	CString str1; //用以在会话框中显示计数结果
	CString str2;
	//con = 0;
	//while (con == 0)
	//{

	//计数
	//int number = 0;
	int yellow = 0, green = 0;
	int max_area = 5500;//可以容忍的最大豆子面积，超过这个是噪声
	int min_area = 15; //同理
	unsigned char* original_image;
	original_image = new unsigned char[1280 * 960]; //保存一份原图的灰度图像，主要是用于debug查看结果，具体功能中不需要使用

	//将rgb彩色图转化成灰度图
	colorToGray(pDst, ptemp);
	colorToGray(pDst, original_image);

	for (int i = 0; i < 960; ++i)
	{
		for (int j = 0; j < 1280; ++j)
		{
			*(ptemp2 + i * 1280 + j) = 255;//初始化
			*(ptemp3 + i * 1280 + j) = 255;
		}
	}



	//二值化，低于threshold的都被置0，否则置255
	int binarization_threshold = 20;//80
	for (int i = 0; i < 960; ++i)
	{
		for (int j = 0; j < 1280; ++j)
		{
			if (*(ptemp + i * 1280 + j) > binarization_threshold)
				*(ptemp + i * 1280 + j) = 0;
			else
				*(ptemp + i * 1280 + j) = 255;
		}
	}

	//除去边框，若一列中超过三分之二的像素是白的，则识别其为边界，将其置为黑色
	remove(ptemp, 1280, 960);


	//1.1 腐蚀，将绿豆彻底腐蚀消失，然后计算黄豆数目
	corrupt(ptemp, ptemp3, 12);
	//L1距离变换，边界点与非豆子区域的距离值小，vise versa。
	distanceTransform(ptemp3, ptemp2, 1280, 960);
	//将距离变换的结果进行二值化以后，可以进一步削去腐蚀的结果图中的边框
	binary(ptemp2, 100);

	//将2复制给3，2用于备份，而3用于找连通域、计数，在此过程中3的内容发生变化，所以需要2备份一次
	copyfrom_2_to_1(ptemp3, ptemp2);

	//1.2 计算黄豆数目
	for (int i = 1; i < 960 - 1; ++i)
	{
		for (int j = 1; j < 1280 - 1; ++j)
		{
			if (*(ptemp3 + (i)*1280 + j) == 255)//如果是豆子的一部分
			{
				*m = 0;//m是面积
				ConnectedArea(i, j, ptemp3, 1280, m);
				if (*m > 5 && *m < max_area)//只有m面积在此区域内的才被视作黄豆，否则是噪声，拒绝该输出
				{
					yellow += 1;//在计数一轮后，ptemp2成为空图
				}
			}
		}
	}

	//给黄豆上色
	color(pDst, ptemp2, "red");


	//2.1 膨胀还原图像，与原图像相减，再轻微腐蚀，计算绿豆数目。
	copyfrom_2_to_1(ptemp3, ptemp2);
	dilate(ptemp3, ptemp2, 25);



	//2.2 与原图像相减
	//用原图和迭加图相减，得到ptemp是相减后的图片，从而得到黄豆被剔除的图（只剩绿豆）
	img_2_minus_1(ptemp, ptemp3);

	//2.3 对相减后的图片轻微腐蚀，i.e.对绿豆做和刚刚一样的操作

	corrupt(ptemp, ptemp2, 4);
	distanceTransform(ptemp2, ptemp3, 1280, 960);
	binary(ptemp3, 10);

	//重复操作让效果更好，如果仔细精调第一步的参数，也许不需要这一步的重复操作
	corrupt(ptemp3, ptemp2, 4);
	distanceTransform(ptemp2, ptemp3, 1280, 960);
	binary(ptemp3, 25);

	//给绿豆上色
	color(pDst, ptemp3, "green");

	//2.4 计算绿豆数目，同黄豆计数
	for (int i = 1; i < 960 - 1; ++i)
	{
		for (int j = 1; j < 1280 - 1; ++j)
		{
			if (*(ptemp3 + (i)*1280 + j) == 0)//如果是豆子的一部分
			{
				*m = 0;//m是面积
				ConnectedArea(i, j, ptemp3, 1280, m);
				if (*m > min_area && *m < max_area)
				{
					green += 1;//在计数一轮后，ptemp3成为空图
				}
			}
		}
	}

	*number1 = yellow;
	*number2 = green;
	free(original_image);
	//展示最终结果（黄豆、绿豆都被上色）
	DrawImage();
	//在对话框中显示结果
	str1.Format(_T("yellow %d"), num1);
	str2.Format(_T("green %d"), num2);
	GetDlgItem(IDC_EDIT3)->SetWindowText(str1);
	GetDlgItem(IDC_EDIT1)->SetWindowText(str2);

	free(ptemp);
	free(ptemp2);
	free(ptemp3);

	return 0;
}


int __stdcall StreamCB(MV_IMAGE_INFO* pInfo, ULONG_PTR nUserVal)
{
	CdemoDlg* pDlg = (CdemoDlg*)nUserVal;
	return (pDlg->OnStreamCB(pInfo));
}

// CdemoDlg 对话框



CdemoDlg::CdemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEMO_DIALOG, pParent)
	, m_bRun(FALSE)
	, m_hCam(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, yellow);
	DDX_Control(pDX, IDC_EDIT1, green);
}

BEGIN_MESSAGE_MAP(CdemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENCAM, &CdemoDlg::OnBnClickedOpencam)
	ON_BN_CLICKED(IDC_StartGrab, &CdemoDlg::OnBnClickedStartgrab)
	ON_BN_CLICKED(IDC_CloseCam, &CdemoDlg::OnBnClickedClosecam)
END_MESSAGE_MAP()


// CdemoDlg 消息处理程序

BOOL CdemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	MVSTATUS_CODES r;
	r = MVInitLib();
	if (r != MVST_SUCCESS)
	{
		MessageBox(_T("函数库初始化失败！"), _T("提示"), MB_ICONWARNING);
		return TRUE;
	}
	r = MVUpdateCameraList();
	if (r != MVST_SUCCESS)
	{
		MessageBox(_T("查找连接计算机的相机失败！"), _T("提示"), MB_ICONWARNING);
		return TRUE;
	}
	GetDlgItem(IDC_OPENCAM)->EnableWindow(true);
	GetDlgItem(IDC_StartGrab)->EnableWindow(false);
	GetDlgItem(IDC_CloseCam)->EnableWindow(false);
	GetDlgItem(IDC_OPENCAM)->EnableWindow(true);
	GetDlgItem(IDC_StartGrab)->EnableWindow(false);
	GetDlgItem(IDC_CloseCam)->EnableWindow(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CdemoDlg::OnPaint()
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
HCURSOR CdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CdemoDlg::OnBnClickedOpencam()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCams = 0;
	MVGetNumOfCameras(&nCams);
	if (nCams == 0)
	{
		MessageBox(_T(" 没 有 找 到 相 机 , 请 确 认 连 接 和 相 机 IP 设 置 "), _T(" 提 示"),MB_ICONWARNING);
			return;
	}
	MVSTATUS_CODES r = MVOpenCamByIndex(0, &m_hCam);
	if (m_hCam == NULL)
	{
		if (r == MVST_ACCESS_DENIED)
			MessageBox(_T(" 无 法 打 开 相 机 ， 可 能 正 被 别 的 软 件 控 制 "), _T(" 提 示 "),
				MB_ICONWARNING);
		else
			MessageBox(_T("无法打开相机"), _T("提示"), MB_ICONWARNING);
		return;
	}
	int w, h;
	MVGetWidth(m_hCam, &w);
	MVGetHeight(m_hCam, &h);
	MVGetPixelFormat(m_hCam, &m_PixelFormat);
	m_image.CreateByPixelFormat(w, h, m_PixelFormat);
	GetDlgItem(IDC_OPENCAM)->EnableWindow(false);
	GetDlgItem(IDC_StartGrab)->EnableWindow(true);
	GetDlgItem(IDC_CloseCam)->EnableWindow(false);
}


void CdemoDlg::OnBnClickedStartgrab()
{
	// TODO: 在此添加控件通知处理程序代码
	TriggerModeEnums enumMode;
	MVGetTriggerMode(m_hCam, &enumMode);
	if (enumMode != TriggerMode_Off)
	{
		MVSetTriggerMode(m_hCam, TriggerMode_Off);
		Sleep(100);
	}
	MVStartGrab(m_hCam, StreamCB, (ULONG_PTR)this);
	m_bRun = true;
	GetDlgItem(IDC_OPENCAM)->EnableWindow(false);
	GetDlgItem(IDC_StartGrab)->EnableWindow(false);
	GetDlgItem(IDC_CloseCam)->EnableWindow(true);
	
	//}
		//GetDlgItem(IDC_EDIT3)->SetWindowTextW(_T("001"));
	
	
}


void CdemoDlg::OnBnClickedClosecam()
{
	// TODO: 在此添加控件通知处理程序代码
	MVStopGrab(m_hCam);
	MVCloseCam(m_hCam);
	m_bRun = false;
	GetDlgItem(IDC_OPENCAM)->EnableWindow(true);
	GetDlgItem(IDC_StartGrab)->EnableWindow(false);
	GetDlgItem(IDC_CloseCam)->EnableWindow(false);
}


void CdemoDlg::OnClose()
{
	if (m_bRun != false)
	{
		MVStopGrab(m_hCam);
	}
	MVTerminateLib();
	CDialog::OnClose();
}