// CEditEsp.cpp: Crea un Control CEdit Especial
// para grabar las pulsaciones de teclas

// Implementación

#include "stdafx.h"
#include "CEditEsp.h"

// Constructor de la Clase

CEditEsp::CEditEsp(): CEdit(){}


/*************************************************
*************** Control de Mensajes **************
**************************************************/


BEGIN_MESSAGE_MAP(CEditEsp, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYUP()
END_MESSAGE_MAP()


/*************************************************
******************** Métodos *********************
**************************************************/

void CEditEsp::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){}

void CEditEsp::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){}

void CEditEsp::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	OnKeyUp(nChar, nRepCnt, nFlags);
}

void CEditEsp::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
//	LONG tecla;
	CString n_tecla;

	teclaExtendida=(((nFlags&0x100)/0xFF)==1)?true:false;
	if((nFlags&0xFF)==MapVirtualKey(VK_RSHIFT,0)) {
		scanCode=(UCHAR)VK_RSHIFT;
	} else {
		scanCode=(UCHAR)nChar;
	}
	n_tecla=CHotKeyCtrl::GetKeyName(scanCode,teclaExtendida);
	this->SetWindowText(n_tecla.GetBuffer());
	if(nChar==VK_PAUSE || nChar==VK_SNAPSHOT)
		this->SetWindowText("");
}