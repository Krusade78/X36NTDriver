// Ed_LComandos.cpp: Dialogo de lista de comandos
// Implementación

#include "stdafx.h"
#include "X36Editor.h"
#include "Comando.h"
#include "Ed_LComandos.h"

// Des/Constructor de la Clase

CEd_LComandos::CEd_LComandos(CX36Editor* editor)
	: CDialog(IDD_X36MAP_COMANDOS,editor)
{
	pIndicesAux=NULL;
	this->editor=editor;
}

CEd_LComandos::~CEd_LComandos()
{
	codigosT.RemoveAll();
	codigosTExt.RemoveAll();
}


/*************************************************
*************** Control de Mensajes **************
**************************************************/

void CEd_LComandos::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID2ListaComandos, lista);
}


BEGIN_MESSAGE_MAP(CEd_LComandos, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID2Borrar, OnBnClickedCmdborrar)
	ON_BN_CLICKED(ID2Nuevo, OnBnClickedCmdnuevo)
	ON_BN_CLICKED(ID2Editar, OnBnClickedCmdeditar)
	ON_BN_CLICKED(ID2Raton, OnBnClickedCmdraton)
END_MESSAGE_MAP()

//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización


BOOL CEd_LComandos::OnInitDialog()
{
	CDialog::OnInitDialog();
	lista.InsertColumn(0,TIP12,0,150);
	lista.InsertColumn(1,TIP13,0,170);
	lista.SetExtendedStyle(LVS_EX_FULLROWSELECT/* | LVS_EX_FLATSB*/ );
	return TRUE;
}

//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/


int CEd_LComandos::Insertar(CString nombre, UINT64 teclas, UCHAR teclasExt, bool ini)
{
	CString n_teclas;
	TCHAR n_tecla[32];
	UCHAR codigos[8];
	bool E0s[8];
	LONG tecla;
	int posicion;
	
	codigos[7]=(UCHAR)(( teclas&0xFF00000000000000)/0xFFFFFFFFFFFFFF);
	E0s[7]=((teclasExt&128)==128) ? true : false;
	codigos[6]=(UCHAR)(( teclas&0xFF000000000000)/0xFFFFFFFFFFFF);
	E0s[6]=((teclasExt&64)==64) ? true : false;
	codigos[5]=(UCHAR)(( teclas&0xFF0000000000)/0xFFFFFFFFFF);
	E0s[5]=((teclasExt&32)==32) ? true : false;
	codigos[4]=(UCHAR)(( teclas&0xFF00000000)/0xFFFFFFFF);
	E0s[4]=((teclasExt&16)==16) ? true : false;
	codigos[3]=(UCHAR)((teclas&0xFF000000)/0xFFFFFF);
	E0s[3]=((teclasExt&8)==8) ? true : false;
	codigos[2]=(UCHAR)(( teclas&0xFF0000)/0xFFFF);
	E0s[2]=((teclasExt&4)==4) ? true : false;
	codigos[1]=(UCHAR)(( teclas&0xFF00)/0xFF);
	E0s[1]=((teclasExt&2)==2) ? true : false;
	codigos[0]=(UCHAR)( teclas&0xFF);
	E0s[0]=((teclasExt&1)==1) ? true : false;

	posicion=lista.InsertItem(lista.GetItemCount(),nombre.GetBuffer());
	if(posicion==0) {
		codigosT.AddHead(teclas);
		codigosTExt.AddHead(teclasExt);
	} else {
		codigosT.InsertAfter(codigosT.FindIndex(posicion-1),teclas);
		codigosTExt.InsertAfter(codigosTExt.FindIndex(posicion-1),teclasExt);
	}
	for(UCHAR i=0;i<8;i++) {
		if(codigos[i]!=0) {
			tecla=(65536*MapVirtualKey(codigos[i],0))+(0x1000000*E0s[i]);
			GetKeyNameText(tecla,n_tecla,32);
			if(!n_teclas.IsEmpty()) n_teclas.Append(" + ");
			n_teclas.Append(n_tecla);
		}
	}
	lista.SetItemText(posicion,1,n_teclas.GetBuffer());

	// Reordenar Indices
	if(!ini) {
		for(UCHAR i=0;i<6;i++) {
			for(UCHAR j=0;j<104;j++) {
				if(editor->CogerIndice(i,j)>=(posicion+1))
					editor->PonerIndice(i,j,editor->CogerIndice(i,j)+1);
			}
		}
	}
	
	return posicion;
}

//-------------------------------------------------//

void CEd_LComandos::Borrar(void)
{
	int sel=lista.GetSelectionMark();

	codigosT.RemoveAt(codigosT.FindIndex(sel));
	codigosTExt.RemoveAt(codigosTExt.FindIndex(sel));
	lista.DeleteItem(sel);

	// Reordenar Indices

	for(UCHAR i=0;i<6;i++) {
		for(UCHAR j=0;j<104;j++) {
			if(editor->CogerIndice(i,j)==(sel+1))
				editor->PonerIndice(i,j,0);
			if(editor->CogerIndice(i,j)>(sel+1))
				editor->PonerIndice(i,j,editor->CogerIndice(i,j)-1);
		}
	}
	for(UCHAR i=0;i<6;i++) {
		if((editor->CogerGiraT(0,i)&32)!=32) {
			for(UCHAR j=73;j<78;j++) {
				if((editor->CogerIndice(i,j)>0)&&(editor->CogerIndice(i,j-1)==0)) {
					editor->PonerIndice(i,j-1,editor->CogerIndice(i,j));
					editor->PonerIndice(i,j,0);
				}
			}
		}
		if((editor->CogerGiraT(1,i)&32)!=32) {
			for(UCHAR j=79;j<84;j++) {
				if((editor->CogerIndice(i,j)>0)&&(editor->CogerIndice(i,j-1)==0)) {
					editor->PonerIndice(i,j-1,editor->CogerIndice(i,j));
					editor->PonerIndice(i,j,0);
				}
			}
		}
		if((editor->CogerGiraT(2,i)&32)!=32) {
			for(UCHAR j=85;j<94;j++) {
				if((editor->CogerIndice(i,j)>0)&&(editor->CogerIndice(i,j-1)==0)) {
					editor->PonerIndice(i,j-1,editor->CogerIndice(i,j));
					editor->PonerIndice(i,j,0);
				}
			}
		}
		if((editor->CogerGiraT(3,i)&32)!=32) {
			for(UCHAR j=95;j<104;j++) {
				if((editor->CogerIndice(i,j)>0)&&(editor->CogerIndice(i,j-1)==0)) {
					editor->PonerIndice(i,j-1,editor->CogerIndice(i,j));
					editor->PonerIndice(i,j,0);
				}
			}
		}
	}
}

//-------------------------------------------------//

void CEd_LComandos::PreEditar()
{
	int sel=lista.GetSelectionMark()+1;
	pIndicesAux = new bool[624];
	ZeroMemory(pIndicesAux,sizeof(bool)*624);
	for(UCHAR i=0;i<6;i++) {
		for(UCHAR j=0;j<104;j++) {
			if(editor->CogerIndice(i,j)==sel) {
				*(pIndicesAux+(i*104)+j)=true;
			} else {
				*(pIndicesAux+(i*104)+j)=false;
			}
		}
	}
}

//-------------------------------------------------//

void CEd_LComandos::PostEditar(int sel)
{
	sel++;
	for(UCHAR i=0;i<6;i++) {
		for(UCHAR j=0;j<104;j++) {
			if(*(pIndicesAux+(i*104)+j))
				editor->PonerIndice(i,j,sel);
		}
	}
	delete[] pIndicesAux;
}

//-------------------------------------------------//
//-------------------------------------------------//

UINT16 CEd_LComandos::NumComandos()
{
	return lista.GetItemCount();
}

void CEd_LComandos::CogerDatos(UINT16 idx,UINT64* teclas, UCHAR* teclasExt)
{
	*teclas=codigosT.GetAt(codigosT.FindIndex(idx));
	*teclasExt=codigosTExt.GetAt(codigosTExt.FindIndex(idx));
}

bool CEd_LComandos::CogerNombres(UINT16 idx, CString *texto)
{
	CString nombre=lista.GetItemText(idx,0);
	if(nombre.IsEmpty()) {
		return false;
	} else {
		texto->Empty();
		texto->Append(nombre.GetBuffer());
		return true;
	}
}


/*************************************************
******************* Eventos **********************
**************************************************/


void CEd_LComandos::OnClose()
{
	this->ShowWindow(SW_HIDE);
	editor->Visualizar();
}

//-------------------------------------------------//
//-------------------------------------------------//

void CEd_LComandos::OnBnClickedCmdborrar()
{
	if(lista.GetSelectedCount()>0)
		Borrar();
}

void CEd_LComandos::OnBnClickedCmdnuevo()
{
	UINT64 teclas;
	UCHAR teclasExt;
	CString nombre;
	CComando cmd(false,&nombre,&lista,&teclas,&teclasExt,this);
	if(cmd.DoModal()==IDOK) 
		Insertar(nombre,teclas,teclasExt);
}

void CEd_LComandos::OnBnClickedCmdeditar()
{
	if(lista.GetSelectedCount()>0) {
		int sel=lista.GetSelectionMark();
		UINT64 teclas=codigosT.GetAt(codigosT.FindIndex(sel));
		UCHAR teclasExt=codigosTExt.GetAt(codigosTExt.FindIndex(sel));
		if(((teclas<4) || (teclas>7)) && (teclas!=1) && (teclas!=2))
		{
			CString nombre=lista.GetItemText(sel,0);
			PreEditar();
			Borrar();
			CComando cmd(true,&nombre,&lista,&teclas,&teclasExt,this);
			cmd.DoModal();
			PostEditar(Insertar(nombre,teclas,teclasExt));
		}
	}
}

void CEd_LComandos::OnBnClickedCmdraton()
{
	bool estado[]={0,0,0,0,0,0};
	CString n_aux;

	for(UINT16 i=0;i<lista.GetItemCount();i++)
	{
		n_aux.Empty();
		n_aux=lista.GetItemText(i,0);
		if(n_aux.CompareNoCase(TIP28)==0) estado[0]=true;
		if(n_aux.CompareNoCase(TIP29)==0) estado[1]=true;
		if(n_aux.CompareNoCase(TIP30)==0) estado[2]=true;
		if(n_aux.CompareNoCase(TIP31)==0) estado[3]=true;
		if(n_aux.CompareNoCase(TIP32)==0) estado[4]=true;
		if(n_aux.CompareNoCase(TIP33)==0) estado[5]=true;
	}
	if(!estado[0]) Insertar(TIP28,0,0);
	if(!estado[1]) Insertar(TIP29,0,0);
	if(!estado[2]) Insertar(TIP30,0,0);
	if(!estado[3]) Insertar(TIP31,0,0);
	if(!estado[4]) Insertar(TIP32,0,0);
	if(!estado[5]) Insertar(TIP33,0,0);
}