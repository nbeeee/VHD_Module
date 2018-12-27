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





extern "C" __declspec(dllexport) int VHD_Init();  //模型初始化
extern "C" __declspec(dllexport) int VHD_Uninit();  //卸载模型
extern "C" __declspec(dllexport) int VHD_Reset();  //算法重置
extern "C" __declspec(dllexport) int VHD_SetImageSize(int nWidth, int nHeight, int nImgType);  //设置输入图片的宽，高，格式
extern "C" __declspec(dllexport) int VHD_InputImage(DetectionResult* pDetectionResult,char* pImgBuf, int nSize); //输入图片，返回结果
extern "C" __declspec(dllexport) int VHD_SetAlgMode(int nType); //设置算法模式，单一算法或复用
