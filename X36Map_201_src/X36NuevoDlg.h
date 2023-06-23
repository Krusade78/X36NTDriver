// X36NuevoDlg.h: Dialogo para crear un mapa nuevo
// Definición

#pragma once

class CX36NuevoDlg : public CDialog
{
public:
	CX36NuevoDlg(CWnd* pParent = NULL);   // Constructor

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:
	bool CrearArchivo();
	CEdit nombreCtl;
};
