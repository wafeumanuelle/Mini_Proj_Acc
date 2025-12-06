
// AccViewSensor.h : fichier d'en-tête principal pour l'application AccViewSensor
//
#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"       // symboles principaux
#include "AccSensor.h"

// CAccViewSensorApp:
// Consultez AccViewSensor.cpp pour l'implémentation de cette classe
//

class CAccViewSensorApp : public CWinApp
{
public:
	CAccViewSensorApp();


// Substitutions
public:
	virtual BOOL InitInstance();

// Implémentation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAccViewSensorApp theApp;
