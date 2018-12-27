// VHD_Module.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "PersonDetection.h"
#include "VHD_Module.h"
#include "VHD_API.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CVHD_ModuleApp

BEGIN_MESSAGE_MAP(CVHD_ModuleApp, CWinApp)
END_MESSAGE_MAP()


// CVHD_ModuleApp ����

CVHD_ModuleApp::CVHD_ModuleApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CVHD_ModuleApp ����

CVHD_ModuleApp theApp;


// CVHD_ModuleApp ��ʼ��

BOOL CVHD_ModuleApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


PersonDetector * pDetector = NULL;

//CObjec ����Ϊ���Լ��ĺ�����,��������������еĽӿڶ�ͨ���ⲿ�����
int VHD_Init()  //ģ�ͳ�ʼ��
{
	//������new����

	if(NULL == pDetector)
	{
		pDetector = new PersonDetector();
	}
	return pDetector->VHD_Init();
}
int VHD_Uninit()  //ж��ģ��
{
	if (pDetector)
	{
		return pDetector->VHD_Uninit();
	}
	return 0;
}
int VHD_Reset()  //�㷨����
{
	return pDetector->VHD_Reset();
}
int VHD_SetImageSize(int nWidth, int nHeight, int nImgType)  //��������ͼƬ�Ŀ��ߣ���ʽ
{
	return pDetector->VHD_SetImageSize(nWidth, nHeight, nImgType);
}
int VHD_InputImage(DetectionResult* pDetectionResult, char* pImgBuf, int nSize) //����ͼƬ�����ؽ��
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
int VHD_SetAlgMode(int nType) //�����㷨ģʽ����һ�㷨����
{
	return pDetector->VHD_SetAlgMode(nType);
}