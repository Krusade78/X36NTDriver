// X36MapDlg.cpp: Dialogo principal de programa
// Implementación

#include "stdafx.h"
#include "X36MapDlg.h"
#include "X36NuevoDlg.h"
#include "X36Calibraje.h"
#include "X36Editor.h"
#include "X36Import.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SVC_CARGAR			0
#define SVC_PAUSA			3
#define SVC_SENSIBILIDAD	4


//-------------------------------------------//


// Constructor de la Clase

CX36MapDlg::CX36MapDlg(CWnd* pParent /*=NULL*/):
	CDialog(IDD_X36MAP_DIALOG, pParent){}


/*************************************************
*************** Control de Mensajes **************
**************************************************/

void CX36MapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID0ListaMapas, listaMapas);
	DDX_Control(pDX, ID0Activada, activar);
	DDX_Control(pDX, ID0SensiDes, sensiDes);
	DDX_Control(pDX, ID0PreciDes, preciDes);
}

BEGIN_MESSAGE_MAP(CX36MapDlg, CDialog)
	ON_BN_CLICKED(ID0Actualizar, OnBnClickedActualizar)
	ON_LBN_SELCHANGE(ID0ListaMapas, OnLbnSelchangeList1)
	ON_LBN_SELCANCEL(ID0ListaMapas, OnLbnSelcancelList1)
	ON_BN_CLICKED(ID0Cargar, OnBnClickedCargar)
	ON_BN_CLICKED(ID0Cambiar, OnBnClickedCambiar)
	ON_BN_CLICKED(ID0Nuevo, OnBnClickedNuevo)
	ON_BN_CLICKED(ID0Editar, OnBnClickedEditar)
	ON_BN_CLICKED(ID0Importar, OnBnClickedImportar)
	ON_BN_CLICKED(ID0Calibrar, OnBnClickedCalibrar)
	ON_BN_CLICKED(ID0Seleccionar, OnBnClickedSeleccionar)
	ON_BN_CLICKED(ID0Activada, OnBnClickedActivar)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización

BOOL CX36MapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDI_Principal), TRUE);			// Establecer icono grande
	SetIcon(AfxGetApp()->LoadIcon(IDI_Principal), FALSE);		// Establecer icono pequeño

	CargarDatosRaton();
	CargarDatosAuto();
	CargarListaMapas();

	return TRUE;
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/

void CX36MapDlg::CargarListaMapas(void)
{
	this->GetDlgItem(ID0Cargar)->EnableWindow(FALSE);
	this->GetDlgItem(ID0Editar)->EnableWindow(FALSE);
	listaMapas.ResetContent();

	CFileFind busca;
	BOOL va = busca.FindFile("*.xmp");
	while (va) {
		va = busca.FindNextFile();
		listaMapas.AddString(busca.GetFileTitle().GetBuffer());
	}
}

//-------------------------------------------------//

void CX36MapDlg::CambiarRaton()
{
	HKEY key;
	BYTE sensi=(UCHAR)preciDes.GetPos();
	BYTE pausa=(UCHAR)sensiDes.GetPos();
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_WRITE,&key))
	{
		if(ERROR_SUCCESS!=RegSetValueEx(key,"ratonP",0,REG_BINARY,&pausa,1))
		{
			MessageBox(MSG23,"Error",MB_ICONWARNING);
			RegCloseKey(key);
			return;
		}
		if(ERROR_SUCCESS!=RegSetValueEx(key,"ratonS",0,REG_BINARY,&sensi,1))
		{
			MessageBox(MSG23,"Error",MB_ICONWARNING);
			RegCloseKey(key);
			return;
		}
		RegCloseKey(key);
	}
	
	BroadcastSystemMessage(
		BSF_IGNORECURRENTTASK|BSF_POSTMESSAGE,
		NULL,
		RegisterWindowMessage("X36ServiceCall"),
		SVC_PAUSA,
		0);
	BroadcastSystemMessage(
		BSF_IGNORECURRENTTASK|BSF_POSTMESSAGE,
		NULL,
		RegisterWindowMessage("X36ServiceCall"),
		SVC_SENSIBILIDAD,
		0);

	MessageBox(MSG6,"OK",MB_ICONINFORMATION);
}

//-------------------------------------------------//

void CX36MapDlg::CargarDatosAuto()
{
	DWORD ret;
	HKEY key;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_READ,&key))
	{
		DWORD tam=1;
		BYTE c;
		if(ERROR_SUCCESS==RegQueryValueEx(key,"AutoCarga",0,&ret,&c,&tam))
		{
			activar.SetCheck(c);
		}
		if(ERROR_MORE_DATA==RegQueryValueEx(key,"MapaIni",0,&ret,&c,&tam))
		{
			char* ruta=new char[tam];
			if(ERROR_SUCCESS==RegQueryValueEx(key,"MapaIni",0,&ret,(BYTE*)ruta,&tam))
			{
				CString nombre;
				CFile f;
				f.SetFilePath(ruta);
				nombre=f.GetFileName();
				nombre.Insert(0," ");
				GetDlgItem(ID0LabelMapa)->SetWindowText(nombre.GetBuffer());
			}
			delete[] ruta; ruta=NULL;
		}
		RegCloseKey(key);
	}
}

//-------------------------------------------------//

void CX36MapDlg::CargarDatosRaton()
{
	DWORD ret;

	sensiDes.SetRange(1,99);
	preciDes.SetRange(1,20);
	sensiDes.SetPos(50);
	GetDlgItem(ID0SensiEd)->SetWindowText("50");
	preciDes.SetPos(4);
	GetDlgItem(ID0PreciEd)->SetWindowText("4");

	// Cargar Registro

	HKEY key;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_READ,&key))
	{
		DWORD tam=1;
		BYTE c;
		if(ERROR_SUCCESS==RegQueryValueEx(key,"ratonP",0,&ret,&c,&tam))
		{
			char n[4];
			sensiDes.SetPos(c);
			GetDlgItem(ID0SensiEd)->SetWindowText(itoa(c,n,10));
		}
		if(ERROR_SUCCESS==RegQueryValueEx(key,"ratonS",0,&ret,&c,&tam))
		{
			char n[4];
			preciDes.SetPos(c);
			GetDlgItem(ID0PreciEd)->SetWindowText(itoa(c,n,10));
		}
		RegCloseKey(key);
	}
}

//-------------------------------------------------//

void CX36MapDlg::CargarMapa(bool ini)
{
	CFile f;
	CString nombre;
	CString clave;

	listaMapas.GetText(listaMapas.GetCurSel(),nombre);
	nombre.Append(".xmp");

	if (f.Open(nombre,CFile::typeBinary|CFile::modeRead,NULL)==0) {
		MessageBox(MSG1,"Error",MB_ICONERROR);
		CargarListaMapas();
		return;
	}
	if (f.GetLength()<1568) {
		MessageBox(MSG2,"Error",MB_ICONERROR);
		f.Close();
		return;
	}

	nombre=f.GetFilePath();

	f.Close();

	if(ini) clave="MapaIni"; else clave="Mapa";
	
	HKEY key;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_WRITE,&key))
	{
		if(ERROR_SUCCESS==RegSetValueEx(key,clave.GetBuffer(),0,REG_SZ,(BYTE*)nombre.GetBuffer(),nombre.GetLength()))
		{
			if(!ini) {
				BroadcastSystemMessage(
					BSF_IGNORECURRENTTASK|BSF_POSTMESSAGE,
					NULL,
					RegisterWindowMessage("X36ServiceCall"),
					SVC_CARGAR,
					NULL);
			} else {
				CFile f;
				f.SetFilePath(nombre.GetBuffer());
				nombre=f.GetFileName();
				nombre.Insert(0," ");
				GetDlgItem(ID0LabelMapa)->SetWindowText(nombre.GetBuffer());
			}
		} else {
			MessageBox(MSG23,"Error",MB_ICONERROR);
		}
		RegCloseKey(key);
	} else {
		MessageBox(MSG23,"Error",MB_ICONERROR);
	}
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/


void CX36MapDlg::OnBnClickedActualizar()
{
	CargarListaMapas();
}

void CX36MapDlg::OnLbnSelchangeList1()
{
	if(listaMapas.GetCurSel()!=-1) {
		this->GetDlgItem(ID0Cargar)->EnableWindow(TRUE);
		this->GetDlgItem(ID0Seleccionar)->EnableWindow(TRUE);
		this->GetDlgItem(ID0Editar)->EnableWindow(TRUE);
	}
}

void CX36MapDlg::OnLbnSelcancelList1()
{
	this->GetDlgItem(ID0Cargar)->EnableWindow(FALSE);
	this->GetDlgItem(ID0Seleccionar)->EnableWindow(FALSE);
	this->GetDlgItem(ID0Editar)->EnableWindow(FALSE);
}

void CX36MapDlg::OnBnClickedCargar()
{
	CargarMapa();
}

void CX36MapDlg::OnBnClickedCambiar()
{
	CambiarRaton();
}

void CX36MapDlg::OnBnClickedImportar()
{
	CX36Import imp(this);
	CargarListaMapas();
}

void CX36MapDlg::OnBnClickedNuevo()
{
	CX36NuevoDlg nuevo(this);
	if(nuevo.DoModal()==IDOK) CargarListaMapas();
}

void CX36MapDlg::OnBnClickedEditar()
{
	CString nombre;
	listaMapas.GetText(listaMapas.GetCurSel(),nombre);
	nombre.Append(".xmp");
	this->ShowWindow(SW_HIDE);
	CX36Editor editor(nombre);
	editor.DoModal();
	this->ShowWindow(SW_SHOW);
}

void CX36MapDlg::OnBnClickedCalibrar()
{
	this->ShowWindow(SW_HIDE);
	CX36Calibraje calib(this);
	calib.DoModal();
	this->ShowWindow(SW_SHOW);
}

void CX36MapDlg::OnBnClickedActivar()
{
	HKEY key;
	BYTE c=(BYTE)activar.GetCheck();
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_WRITE,&key))
	{
		if(ERROR_SUCCESS!=RegSetValueEx(key,"AutoCarga",0,REG_BINARY,&c,1))
		{
			MessageBox(MSG23,"Error",MB_ICONWARNING);
		}
		RegCloseKey(key);
	}
}

void CX36MapDlg::OnBnClickedSeleccionar()
{
	CargarMapa(true);
}

void CX36MapDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	char valor[3];
	CSliderCtrl *sl=(CSliderCtrl*)pScrollBar;
	sprintf(valor,"%u",(UCHAR)sl->GetPos());
	if(sl==&sensiDes)
		GetDlgItem(ID0SensiEd)->SetWindowText(valor);
	else
		GetDlgItem(ID0PreciEd)->SetWindowText(valor);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
