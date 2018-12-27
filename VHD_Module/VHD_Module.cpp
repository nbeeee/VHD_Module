// VHD_Module.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "PersonDetection.h"
#include "VHD_Module.h"
#include "VHD_API.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CVHD_ModuleApp

BEGIN_MESSAGE_MAP(CVHD_ModuleApp, CWinApp)
END_MESSAGE_MAP()


// CVHD_ModuleApp 构造

CVHD_ModuleApp::CVHD_ModuleApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CVHD_ModuleApp 对象

CVHD_ModuleApp theApp;


// CVHD_ModuleApp 初始化

BOOL CVHD_ModuleApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


PersonDetector * pDetector = NULL;

//CObjec 可以为你自己的函数了,你的类封在这里，所有的接口都通过外部输进来
int VHD_Init()  //模型初始化
{
	//在这里new出来

	if(NULL == pDetector)
	{
		pDetector = new PersonDetector();
	}
	return pDetector->VHD_Init();
}
int VHD_Uninit()  //卸载模型
{
	if (pDetector)
	{
		return pDetector->VHD_Uninit();
	}
	return 0;
}
int VHD_Reset()  //算法重置
{
	return pDetector->VHD_Reset();
}
int VHD_SetImageSize(int nWidth, int nHeight, int nImgType)  //设置输入图片的宽，高，格式
{
	return pDetector->VHD_SetImageSize(nWidth, nHeight, nImgType);
}
int VHD_InputImage(DetectionResult* pDetectionResult, char* pImgBuf, int nSize) //输入图片，返回结果
{
	int retCode = -1;
	if (NULL != pDetectionResult)
	{
		INPersonDetect inResult;
		memset(&inResult, 0, sizeof(INPersonDetect));
		cout << "start VHD_InputImage" << endl;
		retCode = pDetector->VHD_InputImage(&inResult,pImgBuf, nSize);
		if (retCode == 0)
		{
			pDetectionResult->state = inResult.state;
			pDetectionResult->numDetections = inResult.numDetections;
			if (pDetectionResult->numDetections > 32)
			{
				pDetectionResult->numDetections = 32;
			}
			else if (pDetectionResult->numDetections < 0)
			{
				pDetectionResult->numDetections = 0;
			}
			for (int i = 0; i < pDetectionResult->numDetections; i++)
			{
				pDetectionResult->persongeos[i].left = inResult.persongeos[i].left;
				pDetectionResult->persongeos[i].top = inResult.persongeos[i].top;
				pDetectionResult->persongeos[i].right = inResult.persongeos[i].right;
				pDetectionResult->persongeos[i].bottom = inResult.persongeos[i].bottom;
				pDetectionResult->persongeos[i].score = inResult.persongeos[i].score;
			}

			if (inResult.persongeos != NULL)
			{
				delete inResult.persongeos;
			}
		}
	}
	else {
		cout << "pDetectionResult NULL ..." << endl;
		retCode = -2;
	}
	return retCode;
}
int VHD_SetAlgMode(int nType) //设置算法模式，单一算法或复用
{
	return pDetector->VHD_SetAlgMode(nType);
}