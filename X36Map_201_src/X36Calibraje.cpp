// X36NuevoDlg.cpp: Dialogo para crear un mapa nuevo
// Implementación

#include "stdafx.h"
#include <dinput.h>
#include "X36Calibraje.h"

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dinput8.lib")

#define SVC_RECALIBRAR 1
#define SVC_DESCALIBRAR 2

LPDIRECTINPUT8  g_pDI=NULL;
LPDIRECTINPUTDEVICE8 g_pJoystick=NULL; 
BOOL CALLBACK DIEnumDevicesCallback(
  LPCDIDEVICEINSTANCE lpddi,  
  LPVOID pvRef  
);


// Constructor de la Clase

CX36Calibraje::CX36Calibraje(CWnd* pParent /*=NULL*/)
: CDialog(IDD_X36MAP_CALIBRAR, pParent)
{
}


/*************************************************
*************** Control de Mensajes **************
**************************************************/

BEGIN_MESSAGE_MAP(CX36Calibraje, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID6Botonx, OnBnClickedBx)
	ON_BN_CLICKED(ID6Botony, OnBnClickedBy)
	ON_BN_CLICKED(ID6Botonz, OnBnClickedBz)
	ON_BN_CLICKED(ID6Botonr, OnBnClickedBr)
	ON_BN_CLICKED(ID6Borrar, OnBnClickedBorrar)
END_MESSAGE_MAP()

void CX36Calibraje::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID6Reglaxr, reglaxr);
	DDX_Control(pDX, ID6Reglaxv, reglaxv);
	DDX_Control(pDX, ID6Reglayr, reglayr);
	DDX_Control(pDX, ID6Reglayv, reglayv);
	DDX_Control(pDX, ID6Reglazr, reglazr);
	DDX_Control(pDX, ID6Reglazv, reglazv);
	DDX_Control(pDX, ID6Reglarr, reglarr);
	DDX_Control(pDX, ID6Reglarv, reglarv);
}

//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización

BOOL CX36Calibraje::OnInitDialog()
{
	CDialog::OnInitDialog();
	OnBnClickedBorrar();
	LeerRegistro();
	EjesPosAnt[0]=200;
	EjesPosAnt[1]=200;
	EjesPosAnt[2]=200;
	EjesPosAnt[3]=200;
	if(IniciarDX()) {
		ActualizarX();
		ActualizarY();
		ActualizarZ();
		ActualizarR();
		SetTimer(1,100,NULL);
	} else {
		OnCancel();
	}
	return TRUE;
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/

void CX36Calibraje::LeerValor(HKEY key,LPCTSTR valor,int nid)
{
	DWORD tipo,tam;
	DWORD dato=0;
	char st[6];

	tam=4;
	RegQueryValueEx(key,valor,0,&tipo,(BYTE*)&dato,&tam);
/*	if((nid==ID6Escalax2) || (nid==ID6Escalay2) ||
		(nid==ID6Escalaz2) || (nid==ID6Escalar2))
	{
		if(dato==0) dato=1;
	}*/
	GetDlgItem(nid)->SetWindowText(itoa(dato,st,10));
}

void CX36Calibraje::LeerRegistro()
{
	HKEY key;

	BroadcastSystemMessage(
					BSF_IGNORECURRENTTASK|BSF_POSTMESSAGE,
					NULL,
					RegisterWindowMessage("X36ServiceCall"),
					SVC_DESCALIBRAR,
					NULL);

	if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_READ,&key))
		return;

	LeerValor(key,"Escala_izq_x",ID6Escalax1);
	LeerValor(key,"Escala_izq_y",ID6Escalay1);
	LeerValor(key,"Escala_izq_z",ID6Escalaz1);
	LeerValor(key,"Escala_izq_r",ID6Escalar1);

	LeerValor(key,"Escala_der_x",ID6Escalax2);
	LeerValor(key,"Escala_der_y",ID6Escalay2);
	LeerValor(key,"Escala_der_z",ID6Escalaz2);
	LeerValor(key,"Escala_der_r",ID6Escalar2);

	LeerValor(key,"ZonaNula_x",ID6ZNulax1);
	LeerValor(key,"ZonaNula_y",ID6ZNulay1);
	LeerValor(key,"ZonaNula_z",ID6ZNulaz1);
	LeerValor(key,"ZonaNula_r",ID6ZNular1);

	LeerValor(key,"Limite_x_i",ID6ZNulax0);
	LeerValor(key,"Limite_y_i",ID6ZNulay0);
	LeerValor(key,"Limite_z_i",ID6ZNular0);
	LeerValor(key,"Limite_r_i",ID6ZNulaz0);

	LeerValor(key,"Limite_x_c",ID6Centrox);
	LeerValor(key,"Limite_y_c",ID6Centroy);
	LeerValor(key,"Limite_z_c",ID6Centroz);
	LeerValor(key,"Limite_r_c",ID6Centror);

	LeerValor(key,"Limite_x_d",ID6ZNulax2);
	LeerValor(key,"Limite_y_d",ID6ZNulay2);
	LeerValor(key,"Limite_z_d",ID6ZNulaz2);
	LeerValor(key,"Limite_r_d",ID6ZNular2);

	RegCloseKey(key);
}

//-----------------------------------------------------

bool CX36Calibraje::GuardarValor(HKEY key,LPCTSTR valor,int nid,bool bin)
{
	char num[5]={0,0,0,0,0};
	DWORD dato=0;
	DWORD tipo,tam;

	if(bin) {
		tipo=REG_BINARY;
		tam=1;
	} else {
		tipo=REG_DWORD;
		tam=4;
	}

	GetDlgItem(nid)->GetWindowText(num,5);
	dato=atoi(num);
	if(ERROR_SUCCESS==RegSetValueEx(key,valor,0,tipo,(BYTE*)&dato,tam))
		return true;
	else
		return false;
}

bool CX36Calibraje::GuardarRegistro()
{
	HKEY key;
	bool ok;

	if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_WRITE,&key))
		goto error;

	ok=GuardarValor(key,"Escala_izq_x",ID6Escalax1,true);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Escala_izq_y",ID6Escalay1,true);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Escala_izq_z",ID6Escalaz1,true);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Escala_izq_r",ID6Escalar1,true);
	if(!ok) goto error2;

	ok=GuardarValor(key,"Escala_der_x",ID6Escalax2,true);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Escala_der_y",ID6Escalay2,true);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Escala_der_z",ID6Escalaz2,true);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Escala_der_r",ID6Escalar2,true);
	if(!ok) goto error2;

	ok=GuardarValor(key,"ZonaNula_x",ID6ZNulax1,true);
	if(!ok) goto error2;
	ok=GuardarValor(key,"ZonaNula_y",ID6ZNulay1,true);
	if(!ok) goto error2;
	ok=GuardarValor(key,"ZonaNula_z",ID6ZNulaz1,true);
	if(!ok) goto error2;
	ok=GuardarValor(key,"ZonaNula_r",ID6ZNular1,true);
	if(!ok) goto error2;

	ok=GuardarValor(key,"Limite_x_i",ID6ZNulax0,false);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Limite_y_i",ID6ZNulay0,false);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Limite_z_i",ID6ZNulaz0,false);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Limite_r_i",ID6ZNular0,false);
	if(!ok) goto error2;

	ok=GuardarValor(key,"Limite_x_c",ID6Centrox,false);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Limite_y_c",ID6Centroy,false);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Limite_z_c",ID6Centroz,false);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Limite_r_c",ID6Centror,false);
	if(!ok) goto error2;

	ok=GuardarValor(key,"Limite_x_d",ID6ZNulax2,false);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Limite_y_d",ID6ZNulay2,false);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Limite_z_d",ID6ZNulaz2,false);
	if(!ok) goto error2;
	ok=GuardarValor(key,"Limite_r_d",ID6ZNular2,false);
	if(!ok) goto error2;

	RegCloseKey(key);

	return true;

error2:
	RegCloseKey(key);
error:
	MessageBox(MSG23,"Error",MB_ICONWARNING);
	return false;
}


/**********************************************************
						DirectX
***********************************************************/

bool CX36Calibraje::IniciarDX()
{
	HRESULT         hr;
 
	hr = DirectInput8Create(AfxGetInstanceHandle(), DIRECTINPUT_VERSION,IID_IDirectInput8, (void**)&g_pDI, NULL); 
	if(FAILED(hr))
	{ 
	    MessageBox("DirectInput8Create","Error",MB_ICONSTOP);
		return false;
	}
	hr =g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, DIEnumDevicesCallback,
		NULL, DIEDFL_ATTACHEDONLY);
	if(FAILED(hr))
	{ 
	    MessageBox("EnumDevices","Error",MB_ICONSTOP);
		return false;
	}
    if( NULL == g_pJoystick )
    {
        MessageBox( MSG24,"Error",MB_ICONSTOP);
        return false;
    }
    if( FAILED(g_pJoystick->SetDataFormat(&c_dfDIJoystick2)) )
	{
        MessageBox( "SetDataFormat","Error",MB_ICONSTOP);
        return false;
	}
	if( FAILED( g_pJoystick->SetCooperativeLevel( this->m_hWnd, DISCL_EXCLUSIVE |DISCL_FOREGROUND ) ) )
	{
        MessageBox( "SetCooperativeLevel","Error",MB_ICONSTOP);
		return false;
	}
	DIDEVCAPS devCaps;
	devCaps.dwSize=sizeof(DIDEVCAPS);
	if( FAILED( g_pJoystick->GetCapabilities(&devCaps) ) )
	{
        MessageBox( "GetCapabilities","Error",MB_ICONSTOP);
		return false;
	}
	nEjes=(UCHAR)devCaps.dwAxes;

	DIPROPDWORD dipdw; 
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD); 
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        dipdw.diph.dwHow        = DIPH_DEVICE; 
		dipdw.diph.dwObj		= 0;
		dipdw.dwData			= DIPROPCALIBRATIONMODE_RAW;
   if( FAILED( g_pJoystick->SetProperty( DIPROP_CALIBRATIONMODE, &dipdw.diph ) ) ) 
		return false;

	return true;
}

BOOL CALLBACK DIEnumDevicesCallback(
  LPCDIDEVICEINSTANCE lpddi,  
  LPVOID pvRef  
)
{
	CString nombre;
	nombre.Insert(0,lpddi->tszProductName);
	if(nombre.CompareNoCase("Saitek X36F + X35T")==0 || nombre.CompareNoCase("Saitek X36F")==0)
	{
		g_pDI->CreateDevice(lpddi->guidInstance,&g_pJoystick, NULL);
		return DIENUM_STOP;
	} else {
		return DIENUM_CONTINUE;
	}
}

//-----------------------------------------------------
//-----------------------------------------------------

void CX36Calibraje::ReglasVirtuales(LONG Axis[])
{
	LONG ancho1,ancho2;
	CSliderCtrl* reglas[4]={&reglaxv,&reglayv,&reglazv,&reglarv};
	const int dlg[4]={ID6xvval,ID6yvval,ID6zvval,ID6rvval};
	char n[6];

	for(UCHAR Idx=0; Idx<nEjes; Idx++)
	{
		// Antivibraciones

		if (Axis[Idx]>EjesPosAnt[Idx])
		{
            if (Axis[Idx]>(EjesPosAnt[Idx]+Escala_der[Idx])) {
				EjesPosAnt[Idx]=Axis[Idx];
			} else {
				Axis[Idx]=EjesPosAnt[Idx];
			}
		} else {
            if (Axis[Idx]<(EjesPosAnt[Idx]-Escala_izq[Idx])) {
				EjesPosAnt[Idx]=Axis[Idx];
			} else {
				Axis[Idx]=EjesPosAnt[Idx];
			}
		}

		//-------------------


        if((Axis[Idx]>=(Limite[Idx].c-ZonaNula[Idx])) && (Axis[Idx]<=(Limite[Idx].c+ZonaNula[Idx])))
		{
			Axis[Idx]=0;
		} else {
            if(Axis[Idx]<Limite[Idx].i)
			{
				Axis[Idx]=Limite[Idx].i;
			} else {
				if(Axis[Idx]>Limite[Idx].d)
				{
					Axis[Idx]=Limite[Idx].d;
				}
			}
            ancho1=(Limite[Idx].c-ZonaNula[Idx])-Limite[Idx].i;
            ancho2=Limite[Idx].d-(Limite[Idx].c+ZonaNula[Idx]);
			if(Axis[Idx]<Limite[Idx].c)
			{
				Axis[Idx]=0-((Limite[Idx].c-ZonaNula[Idx])-Axis[Idx]);
				if(ancho1>ancho2)
				{
					Axis[Idx]=(Axis[Idx]*ancho2)/ancho1;
				}
			} else {
				Axis[Idx]-=(Limite[Idx].c+ZonaNula[Idx]);
				if(ancho2>ancho1)
				{
					Axis[Idx]=(Axis[Idx]*ancho1)/ancho2;
				}
			}
		}
//		Axis[Idx]*= Escala_mul[Idx];
//		Axis[Idx]/= Escala_div[Idx];
		reglas[Idx]->SetPos(Axis[Idx]);
		GetDlgItem(dlg[Idx])->SetWindowText(itoa(Axis[Idx],n,10));
	}
}

//-----------------------------------------------------

LONG CX36Calibraje::LeerEdit(int nid)
{
	char num[5]={0,0,0,0,0};
	GetDlgItem(nid)->GetWindowText(num,5);
	return (LONG)atoi(num);
}

//-----------------------------------------------------

bool CX36Calibraje::Comprobar(
							  LPCSTR eje,
							  UCHAR e,
							  int izq,
							  int cen,
							  int der,
							  int nul,
							  int mul,
							  int div,
							  int rmi,
							  int rma,
							  CSliderCtrl* regla)
{
	struct {LONG i; LONG c; LONG d;}Limite_aux;
	UCHAR ZonaNula_aux;
	UCHAR Escala_izq_aux;
	UCHAR Escala_der_aux;

	//Leer valores
	char s[6];
	GetDlgItem(izq)->GetWindowText(s,6);
	Limite_aux.i=atoi(s);
	GetDlgItem(der)->GetWindowText(s,6);
	Limite_aux.d=atoi(s);
	GetDlgItem(cen)->GetWindowText(s,6);
	Limite_aux.c=atoi(s);
	GetDlgItem(nul)->GetWindowText(s,6);
	ZonaNula_aux=atoi(s);
	GetDlgItem(mul)->GetWindowText(s,6);
	Escala_izq_aux=atoi(s);
	GetDlgItem(div)->GetWindowText(s,6);
	Escala_der_aux=atoi(s);

	//Comprobar valores
	if(Escala_der_aux<0 || Escala_der_aux>255)
	{
		MessageBox("Escala : derecha (0 - 255)",eje,MB_ICONWARNING);
		return false;
	}
	if(Escala_izq_aux<0 || Escala_izq_aux>255)
	{
		MessageBox("Escala : izquierda (0 - 255)",eje,MB_ICONWARNING);
		return false;
	}
	if((Limite_aux.d<0 || Limite_aux.d>20000) || (Limite_aux.i<0 || Limite_aux.d>20000) || (Limite_aux.c<0 || Limite_aux.c>20000))
	{
		MessageBox("Valor fuera de rango (0 - 20000)",eje,MB_ICONWARNING);
		return false;
	}
	if(Limite_aux.d<Limite_aux.i)
	{
		MessageBox("Los límites no cuadran",eje,MB_ICONWARNING);
		return false;
	}
	if((ZonaNula_aux/2)>(Limite_aux.d-Limite_aux.i))
	{
		MessageBox("Zona nula demasiado grande",eje,MB_ICONWARNING);
		return false;
	}

	LONG l1=(Limite_aux.d-(Limite_aux.c+ZonaNula_aux));
	LONG l2=((Limite_aux.c-ZonaNula_aux)-Limite_aux.i);
	if(l1<=l2) {
//		l1*=Escala_mul_aux; l1/=Escala_div_aux;
		regla->SetRange(-l1,l1);
		GetDlgItem(rmi)->SetWindowText(itoa(-l1,s,10));
		GetDlgItem(rma)->SetWindowText(itoa(l1,s,10));
	} else {
//		l2*=Escala_mul_aux; l2/=Escala_div_aux;
		regla->SetRange(-l2,l2);
		GetDlgItem(rmi)->SetWindowText(itoa(-l2,s,10));
		GetDlgItem(rma)->SetWindowText(itoa(l2,s,10));
	}
	Limite[e].i=Limite_aux.i;
	Limite[e].c=Limite_aux.c;
	Limite[e].d=Limite_aux.d;
	ZonaNula[e]=ZonaNula_aux;
	Escala_izq[e]=Escala_izq_aux;
	Escala_der[e]=Escala_der_aux;

	return true;
}

bool CX36Calibraje::ActualizarX()
{
	return Comprobar("Advertencias - Eje X",0,
						ID6ZNulax0,
						ID6Centrox,
						ID6ZNulax2,
						ID6ZNulax1,
						ID6Escalax1,
						ID6Escalax2,
						ID6xvmin,
						ID6xvmax,
						&reglaxv);
}
bool CX36Calibraje::ActualizarY()
{
	return Comprobar("Advertencias - Eje Y",1,
						ID6ZNulay0,
						ID6Centroy,
						ID6ZNulay2,
						ID6ZNulay1,
						ID6Escalay1,
						ID6Escalay2,
						ID6yvmin,
						ID6yvmax,
						&reglayv);
}
bool CX36Calibraje::ActualizarZ()
{
	return Comprobar("Advertencias - Eje Z",2,
						ID6ZNulaz0,
						ID6Centroz,
						ID6ZNulaz2,
						ID6ZNulaz1,
						ID6Escalaz1,
						ID6Escalaz2,
						ID6zvmin,
						ID6zvmax,
						&reglazv);
}
bool CX36Calibraje::ActualizarR()
{
	return Comprobar("Advertencias - Eje R",3,
						ID6ZNular0,
						ID6Centror,
						ID6ZNular2,
						ID6ZNular1,
						ID6Escalar1,
						ID6Escalar2,
						ID6rvmin,
						ID6rvmax,
						&reglarv);
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/

void CX36Calibraje::OnOK()
{
	if(!ActualizarX()) return;
	if(!ActualizarY()) return;
	if(nEjes==4) {
		if(!ActualizarZ()) return;
		if(!ActualizarR()) return;
	}

	if(!GuardarRegistro()) return;
	BroadcastSystemMessage(
				BSF_IGNORECURRENTTASK|BSF_POSTMESSAGE,
				NULL,
				RegisterWindowMessage("X36ServiceCall"),
				SVC_RECALIBRAR,
				NULL);

	struct {
			DIPROPHEADER diph;
			LONG data[3];
	} dipdw;
	dipdw.diph.dwSize       = sizeof(dipdw); 
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	dipdw.diph.dwHow        = DIPH_BYID; 
	dipdw.data[1]=0;

	// Eje x
	dipdw.diph.dwObj=2;
	dipdw.data[0]=LeerEdit(ID6xvmin);
	dipdw.data[2]=LeerEdit(ID6xvmax);
	if( FAILED(g_pJoystick->SetProperty( DIPROP_CALIBRATION, &dipdw.diph ) ) ) 
	{
		MessageBox(MSG25,"Error",MB_ICONWARNING);
	}
	// Eje y
	dipdw.diph.dwObj=258;
	dipdw.data[0]=LeerEdit(ID6yvmin);
	dipdw.data[2]=LeerEdit(ID6yvmax);	
	if( FAILED(g_pJoystick->SetProperty( DIPROP_CALIBRATION, &dipdw.diph ) ) ) 
	{
		MessageBox(MSG26,"Error",MB_ICONWARNING);
	}
	if(nEjes==4)
	{
		// Eje z
		dipdw.diph.dwObj=514;
		dipdw.data[0]=LeerEdit(ID6zvmin);
		dipdw.data[2]=LeerEdit(ID6zvmax);	
		if( FAILED(g_pJoystick->SetProperty( DIPROP_CALIBRATION, &dipdw.diph ) ) ) 
		{
			MessageBox(MSG27,"Error",MB_ICONWARNING);
		}
		// Eje r
		dipdw.diph.dwObj=1282;
		dipdw.data[0]=LeerEdit(ID6rvmin);
		dipdw.data[2]=LeerEdit(ID6rvmax);	
		if( FAILED(g_pJoystick->SetProperty( DIPROP_CALIBRATION, &dipdw.diph ) ) ) 
		{
			MessageBox(MSG28,"Error",MB_ICONWARNING);
		}
	}
	KillTimer(1);
	if(g_pJoystick!=NULL) { g_pJoystick->Unacquire(); g_pJoystick->Release(); g_pJoystick=NULL;}
	if(g_pDI!=NULL) {g_pDI->Release(); g_pDI=NULL;}

	CDialog::OnOK();
}

void CX36Calibraje::OnCancel()
{
	BroadcastSystemMessage(
					BSF_IGNORECURRENTTASK|BSF_POSTMESSAGE,
					NULL,
					RegisterWindowMessage("X36ServiceCall"),
					SVC_RECALIBRAR,
					NULL);
	KillTimer(1);
	if(g_pJoystick!=NULL) { g_pJoystick->Unacquire(); g_pJoystick->Release(); g_pJoystick=NULL;}
	if(g_pDI!=NULL) {g_pDI->Release(); g_pDI=NULL;}
	CDialog::OnCancel();
}

void CX36Calibraje::OnBnClickedBorrar()
{
	reglaxr.SetRangeMax(0);
	reglayr.SetRangeMax(0);
	reglazr.SetRangeMax(0);
	reglarr.SetRangeMax(0);
	reglaxr.SetRangeMin(65535);
	reglayr.SetRangeMin(65535);
	reglazr.SetRangeMin(65535);
	reglarr.SetRangeMin(65535);
	GetDlgItem(ID6xrmin)->SetWindowText("0");
	GetDlgItem(ID6yrmin)->SetWindowText("0");
	GetDlgItem(ID6zrmin)->SetWindowText("0");
	GetDlgItem(ID6rrmin)->SetWindowText("0");
	GetDlgItem(ID6xrmax)->SetWindowText("0");
	GetDlgItem(ID6yrmax)->SetWindowText("0");
	GetDlgItem(ID6zrmax)->SetWindowText("0");
	GetDlgItem(ID6rrmax)->SetWindowText("0");
}

//-------------------------------------------------//

void CX36Calibraje::OnTimer(UINT nIDEvent)
{
	if( FAILED( g_pJoystick->Poll() ) )
	{
		g_pJoystick->Acquire();
	} else {
		DIJOYSTATE2 js;
		if(!FAILED(g_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &js ) ) )
		{
			char n[6];

			// Eje x
			if(js.lX>reglaxr.GetRangeMax())
			{
				reglaxr.SetRangeMax(js.lX);
				GetDlgItem(ID6xrmax)->SetWindowText(itoa(js.lX,n,10));
			}
			if(js.lX<reglaxr.GetRangeMin())
			{
				reglaxr.SetRangeMin(js.lX);
				GetDlgItem(ID6xrmin)->SetWindowText(itoa(js.lX,n,10));
			}
			GetDlgItem(ID6xrval)->SetWindowText(itoa(js.lX,n,10));
			reglaxr.SetPos(js.lX);

			// Eje y
			if(js.lY>reglayr.GetRangeMax())
			{
				reglayr.SetRangeMax(js.lY);
				GetDlgItem(ID6yrmax)->SetWindowText(itoa(js.lY,n,10));
			}
			if(js.lY<reglayr.GetRangeMin())
			{
				reglayr.SetRangeMin(js.lY);
				GetDlgItem(ID6yrmin)->SetWindowText(itoa(js.lY,n,10));
			}
			GetDlgItem(ID6yrval)->SetWindowText(itoa(js.lY,n,10));
			reglayr.SetPos(js.lY);

			// Eje z
			if(js.rglSlider[0]>reglazr.GetRangeMax())
			{
				reglazr.SetRangeMax(js.rglSlider[0]);
				GetDlgItem(ID6zrmax)->SetWindowText(itoa(js.rglSlider[0],n,10));
			}
			if(js.rglSlider[0]<reglazr.GetRangeMin())
			{
				reglazr.SetRangeMin(js.rglSlider[0]);
				GetDlgItem(ID6zrmin)->SetWindowText(itoa(js.rglSlider[0],n,10));
			}
			GetDlgItem(ID6zrval)->SetWindowText(itoa(js.rglSlider[0],n,10));
			reglazr.SetPos(js.rglSlider[0]);

			// Eje r
			if(js.lRz>reglarr.GetRangeMax())
			{
				reglarr.SetRangeMax(js.lRz);
				GetDlgItem(ID6rrmax)->SetWindowText(itoa(js.lRz,n,10));
			}
			if(js.lRz<reglarr.GetRangeMin())
			{
				reglarr.SetRangeMin(js.lRz);
				GetDlgItem(ID6rrmin)->SetWindowText(itoa(js.lRz,n,10));
			}
			GetDlgItem(ID6rrval)->SetWindowText(itoa(js.lRz,n,10));
			reglarr.SetPos(js.lRz);

			// Virtual
			LONG eje[4]={js.lX,js.lY,js.rglSlider[0],js.lRz};
			ReglasVirtuales(eje);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

//-------------------------------------------------//

void CX36Calibraje::OnBnClickedBx()
{
	ActualizarX();
}

void CX36Calibraje::OnBnClickedBy()
{
	ActualizarY();
}

void CX36Calibraje::OnBnClickedBz()
{
	ActualizarZ();
}

void CX36Calibraje::OnBnClickedBr()
{
	ActualizarR();
}
