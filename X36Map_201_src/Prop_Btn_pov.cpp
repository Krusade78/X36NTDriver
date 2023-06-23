// Prop_Btn_pov.cpp: Página de propiedades de povs
// Implementación

#include "stdafx.h"
#include "x36editor.h"
#include "ed_lcomandos.h"
#include "Prop_Btn_pov.h"

// Constructor de la Clase

IMPLEMENT_DYNAMIC(CProp_Btn_pov, CProp_Btn_prt)
CProp_Btn_pov::CProp_Btn_pov(CX36Editor *editor,CEd_LComandos *cmds,UCHAR modo)
	:CProp_Btn_prt(editor,cmds,modo)
{}


/*************************************************
*************** Control de Mensajes **************
**************************************************/

void CProp_Btn_pov::DoDataExchange(CDataExchange* pDX)
{
	CProp_Btn_prt::DoDataExchange(pDX);
	DDX_Control(pDX, IDAsigPuertoPOV, sAsignarPuertoPOV);
	DDX_Control(pDX, IDComboPOV, comboPOV);
}

BEGIN_MESSAGE_MAP(CProp_Btn_pov, CProp_Btn_prt)
	ON_BN_CLICKED(IDAsigPuertoPOV, OnBnClickedAsigPuertoPOV)
END_MESSAGE_MAP()


//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización

BOOL CProp_Btn_pov::OnInitDialog()
{
	CProp_Btn_prt::OnInitDialog();

	GetDlgItem(IDAsigPuertoPOV)->ShowWindow(SW_SHOW);
	GetDlgItem(IDComboPOV)->ShowWindow(SW_SHOW);

	return TRUE;
}

//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/


void CProp_Btn_pov::CargarDatos()
{
	CProp_Btn_prt::CargarDatos();

	comboPOV.AddString("POV 1 Ar");
	comboPOV.AddString("POV 1 Ar-D");
	comboPOV.AddString("POV 1 D");
	comboPOV.AddString("POV 1 D-Ab");
	comboPOV.AddString("POV 1 Ab");
	comboPOV.AddString("POV 1 Ab-I");
	comboPOV.AddString("POV 1 I");
	comboPOV.AddString("POV 1 Ar-I");
	comboPOV.AddString("POV 2 Ar");
	comboPOV.AddString("POV 2 Ar-D");
	comboPOV.AddString("POV 2 D");
	comboPOV.AddString("POV 2 D-Ab");
	comboPOV.AddString("POV 2 Ab");
	comboPOV.AddString("POV 2 Ab-I");
	comboPOV.AddString("POV 2 I");
	comboPOV.AddString("POV 2 Ar-I");

	// Cargar Asignación de puerto POV

	if((editor->CogerPovDx(pagina)<=8) && (editor->CogerPovDx(pagina)>0)) {
		comboPOV.SetCurSel(editor->CogerPovDx(pagina)-1);
		comboPOV.EnableWindow();
		sAsignarPuertoPOV.SetCheck(BST_CHECKED);
	} else if (editor->CogerPovDx(pagina)>8) {
		comboPOV.SetCurSel(editor->CogerPovDx(pagina)-9);
		comboPOV.EnableWindow();
		sAsignarPuertoPOV.SetCheck(BST_CHECKED);
	}
}

void CProp_Btn_pov::GuardarDatos()
{
	CProp_Btn_prt::GuardarDatos();

	UCHAR bt;

	if(sAsignarPuertoPOV.GetCheck()==BST_CHECKED) {
		if(comboPOV.GetCurSel()<8) {
			bt=comboPOV.GetCurSel()+1;
		} else {
			bt=comboPOV.GetCurSel()+9;
		}
		editor->PonerPovDx(pagina,bt);
	} else {
		// Liberar Botón
		editor->PonerPovDx(pagina,0);
	}
}


void CProp_Btn_pov::ResetGUI()
{
	CProp_Btn_prt::ResetGUI();
	sAsignarPuertoPOV.EnableWindow();
	sAsignarPuertoPOV.SetCheck(BST_UNCHECKED);
	comboPOV.EnableWindow(FALSE);
    comboPOV.ResetContent();
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/


void CProp_Btn_pov::OnBnClickedAsigPuertoPOV()
{
	comboPOV.SetCurSel(0);
	GuardarDatos();
	ResetGUI();
	CargarDatos();
}

BOOL CProp_Btn_pov::OnKillActive()
{
	GuardarDatos();
	return CPropertyPage::OnKillActive();
}

BOOL CProp_Btn_pov::OnSetActive()
{
	ResetGUI();
	CargarDatos();
	return CPropertyPage::OnSetActive();
}
