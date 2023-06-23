// X36Map.cpp : define los comportamientos de las clases para la aplicación.
//

#include "stdafx.h"
#include "X36MapDlg.h"

#pragma once
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//********** Definicion de la Clase Principal

class CX36MapApp : public CWinApp
{
public:
	CX36MapApp();
	virtual BOOL InitInstance();
};

//******* Objeto de Aplicación

extern CX36MapApp theApp;
CX36MapApp theApp;

// Implementación de la clase principal

CX36MapApp::CX36MapApp(){}

BOOL CX36MapApp::InitInstance()
{
	CWinApp::InitInstance();
	CX36MapDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return TRUE;
}