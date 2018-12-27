#pragma once;

typedef struct {
	float score, left, top, right, bottom;
}PersonGeo;

typedef struct
{
	int state;
	int numDetections;
	PersonGeo persongeos[32];
}DetectionResult;





extern "C" __declspec(dllexport) int VHD_Init();  //ģ�ͳ�ʼ��
extern "C" __declspec(dllexport) int VHD_Uninit();  //ж��ģ��
extern "C" __declspec(dllexport) int VHD_Reset();  //�㷨����
extern "C" __declspec(dllexport) int VHD_SetImageSize(int nWidth, int nHeight, int nImgType);  //��������ͼƬ�Ŀ��ߣ���ʽ
extern "C" __declspec(dllexport) int VHD_InputImage(DetectionResult* pDetectionResult,char* pImgBuf, int nSize); //����ͼƬ�����ؽ��
extern "C" __declspec(dllexport) int VHD_SetAlgMode(int nType); //�����㷨ģʽ����һ�㷨����
