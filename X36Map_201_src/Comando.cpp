// Comando.h: Dialogo para crear/editar comandos
// Implementación

#include "stdafx.h"
#include "Comando.h"

// Des/Constructor de la Clase

CComando::CComando(bool ins,CString* nombre,CListCtrl* lista,UINT64* teclas,UCHAR* teclasExt,CWnd* pParent /*=NULL*/)
	: CDialog(IDD_X36MAP_CMD_EDITAR, pParent)
{
	insertar=ins;
	pNombre=nombre;
	pLista=lista;
	pTeclas=teclas;
	pTeclasExt=teclasExt;
}

CComando::~CComando()
{
	codigos.RemoveAll();
	codigosExt.RemoveAll();
}


/*************************************************
*************** Control de Mensajes **************
**************************************************/

void CComando::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID3Pulsaciones, pulsacion);
	DDX_Control(pDX, ID3ListaTeclas, listaTeclas);
	DDX_Control(pDX, ID3Nombre, nombre);
}

BEGIN_MESSAGE_MAP(CComando, CDialog)
	ON_BN_CLICKED(ID3Guardar, OnBnClickedAceptar)
	ON_BN_CLICKED(ID3InsTecla, OnBnClickedInstecla)
	ON_BN_CLICKED(ID3BoTecla, OnBnClickedBotecla)
//	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización

BOOL CComando::OnInitDialog()
{
	CDialog::OnInitDialog();
	nombre.LimitText(31);
	if(insertar) {
		nombre.SetWindowText(pNombre->GetBuffer());
		CargarListas();
	}

	return TRUE;
}

//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/

void CComando::CargarListas()
{
	UCHAR code[8];
	bool E0s[8];
	LONG tecla;
	TCHAR n_tecla[32];

	code[7]=(UCHAR)((*pTeclas&0xFF00000000000000)/0xFFFFFFFFFFFFFF);
	E0s[7]=((*pTeclasExt&128)==128) ? true : false;
	code[6]=(UCHAR)((*pTeclas&0xFF000000000000)/0xFFFFFFFFFFFF);
	E0s[6]=((*pTeclasExt&64)==64) ? true : false;
	code[5]=(UCHAR)((*pTeclas&0xFF0000000000)/0xFFFFFFFFFF);
	E0s[5]=((*pTeclasExt&32)==32) ? true : false;
	code[4]=(UCHAR)((*pTeclas&0xFF00000000)/0xFFFFFFFF);
	E0s[4]=((*pTeclasExt&16)==16) ? true : false;
	code[3]=(UCHAR)((*pTeclas&0xFF000000)/0xFFFFFF);
	E0s[3]=((*pTeclasExt&8)==8) ? true : false;
	code[2]=(UCHAR)((*pTeclas&0xFF0000)/0xFFFF);
	E0s[2]=((*pTeclasExt&4)==4) ? true : false;
	code[1]=(UCHAR)((*pTeclas&0xFF00)/0xFF);
	E0s[1]=((*pTeclasExt&2)==2) ? true : false;
	code[0]=(UCHAR)(*pTeclas&0xFF);
	E0s[0]=((*pTeclasExt&1)==1) ? true : false;

	for(UCHAR i=0;i<8;i++) {
		if(code[i]!=0) {
			codigos.AddTail(code[i]);
			codigosExt.AddTail(E0s[i]);
			tecla=(65536*MapVirtualKey(code[i],0))+(0x1000000*E0s[i]);
			GetKeyNameText(tecla,n_tecla,32);
			listaTeclas.AddString(n_tecla);
		} else {
			break;
		}
	}
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/

void CComando::OnBnClickedAceptar()
{
	UINT64 aux1=0;
	UCHAR aux2=0;
	CString n_aux1,n_aux2;

	// Comprobar campo nombre
	nombre.GetWindowText(n_aux1);
	if(n_aux1.IsEmpty()) {
		MessageBox(MSG13,"Error",MB_ICONERROR);
		return;
	}
	n_aux1.TrimLeft();
	n_aux1.TrimRight();
	if(n_aux1.IsEmpty()) {
		MessageBox(MSG13,"Error",MB_ICONERROR);
		return;
	}
	for(UINT16 i=0;i<pLista->GetItemCount();i++)
	{
		n_aux2.Empty();
		n_aux2=pLista->GetItemText(i,0);
		if(n_aux2.CompareNoCase(n_aux1.GetBuffer())==0) {
			MessageBox(MSG14,"Error",MB_ICONERROR);
			return;
		}
	}

	// Calcular codigo de pulsaciones
	for(UCHAR i=0;i<listaTeclas.GetCount();i++)
	{
        switch (i) 
		{
			case 0:
				aux1+=codigos.GetAt(codigos.FindIndex(i));
				aux2+=codigosExt.GetAt(codigosExt.FindIndex(i));
				break;
			case 1:
				aux1+=codigos.GetAt(codigos.FindIndex(i))*0x100;
				aux2+=codigosExt.GetAt(codigosExt.FindIndex(i))*2;
				break;
			case 2:
				aux1+=codigos.GetAt(codigos.FindIndex(i))*0x10000;
				aux2+=codigosExt.GetAt(codigosExt.FindIndex(i))*4;
				break;
			case 3:
				aux1+=codigos.GetAt(codigos.FindIndex(i))*0x1000000;
				aux2+=codigosExt.GetAt(codigosExt.FindIndex(i))*8;
				break;
			case 4:
				aux1+=codigos.GetAt(codigos.FindIndex(i))*0x100000000;
				aux2+=codigosExt.GetAt(codigosExt.FindIndex(i))*16;
				break;
			case 5:
				aux1+=codigos.GetAt(codigos.FindIndex(i))*0x10000000000;
				aux2+=codigosExt.GetAt(codigosExt.FindIndex(i))*32;
				break;
			case 6:
				aux1+=codigos.GetAt(codigos.FindIndex(i))*0x1000000000000;
				aux2+=codigosExt.GetAt(codigosExt.FindIndex(i))*64;
				break;
			case 7:
				aux1+=codigos.GetAt(codigos.FindIndex(i))*0x100000000000000;
				aux2+=codigosExt.GetAt(codigosExt.FindIndex(i))*128;
		}
	}

	// Enviar
	*pTeclas=aux1;
	*pTeclasExt=aux2;
	pNombre->Empty();
	pNombre->Append(n_aux1);
	OnOK();
}

void CComando::OnBnClickedInstecla()
{
	CString descripcion;

	if(listaTeclas.GetCount()==8) {
		MessageBox(MSG15,MSG12,MB_ICONWARNING);
		return;
	}
	if(pulsacion.GetWindowTextLength()==0) return;

	pulsacion.GetWindowText(descripcion);
	listaTeclas.AddString(descripcion.GetBuffer());
	codigos.AddTail(pulsacion.scanCode);
	codigosExt.AddTail(pulsacion.teclaExtendida);
}

void CComando::OnBnClickedBotecla()
{
	int idx=listaTeclas.GetCurSel();
	if(idx!=LB_ERR) {
		codigos.RemoveAt(codigos.FindIndex(idx));
		codigosExt.RemoveAt(codigosExt.FindIndex(idx));
		listaTeclas.DeleteString(idx);
	}
}
