// X36NuevoDlg.cpp: Dialogo para crear un mapa nuevo
// Implementación

#include "stdafx.h"
#include "X36NuevoDlg.h"

// Constructor de la Clase

CX36NuevoDlg::CX36NuevoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_X36MAP_NUEVO, pParent){}


/*************************************************
*************** Control de Mensajes **************
**************************************************/

void CX36NuevoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID1Nombre, nombreCtl);
}

//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización

BOOL CX36NuevoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	nombreCtl.LimitText(50);
	return TRUE;
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/

bool CX36NuevoDlg::CrearArchivo()
{
	CString nombre;

	nombreCtl.GetWindowText(nombre);
	if(!nombre.IsEmpty()) {
		nombre.TrimLeft();
		nombre.TrimRight();
		if(nombre.IsEmpty()) return false;
	} else {
		return false;
	}

	nombre.Append(".xmp");
	CFile f;
	if(f.Open(nombre.GetBuffer(),CFile::modeRead)!=0) {
		f.Close();
		MessageBox(MSG16,MSG12,MB_ICONWARNING);
		return false;
	} else {
		if(f.Open(nombre.GetBuffer(),CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive)==0) {
			MessageBox(MSG8,"Error",MB_ICONERROR);
			return false;
		}
	}
	f.SetLength(1568);
	f.Close();
	return true;
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/

void CX36NuevoDlg::OnOK()
{
	if(CrearArchivo()) {
		CDialog::OnOK();
	} else {
		nombreCtl.SetFocus();
	}
}