// VHD_Module.h : VHD_Module DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

// CVHD_ModuleApp
// �йش���ʵ�ֵ���Ϣ������� VHD_Module.cpp
//

class CVHD_ModuleApp : public CWinApp
{
public:
	CVHD_ModuleApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
