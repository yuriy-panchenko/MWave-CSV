
// MWave CSV.h : main header file for the MWave CSV application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMWaveCSVApp:
// See MWave CSV.cpp for the implementation of this class
//

class CMWaveCSVApp : public CWinApp
{
public:
	CMWaveCSVApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMWaveCSVApp theApp;
