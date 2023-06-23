// Prop_Rot_eje.cpp: Página de propiedades de ejes
// Implementación

#include "stdafx.h"
#include "x36editor.h"
#include "ed_lcomandos.h"
#include "Prop_Rot_eje.h"

// Constructor de la Clase

IMPLEMENT_DYNAMIC(CProp_Rot_eje, CProp_Rot)
CProp_Rot_eje::CProp_Rot_eje(CX36Editor *editor,CEd_LComandos *cmds,UCHAR modo)
	:CProp_Rot(editor,cmds,modo)
{}

/*************************************************
*************** Control de Mensajes **************
**************************************************/

void CProp_Rot_eje::DoDataExchange(CDataExchange* pDX)
{
	CProp_Rot::DoDataExchange(pDX);
	DDX_Control(pDX, IDAsigPuertoEj, sAsignarPuerto);
}

BEGIN_MESSAGE_MAP(CProp_Rot_eje, CProp_Rot)
	ON_BN_CLICKED(IDAsigPuertoEj, OnBnClickedAsigPuerto)
END_MESSAGE_MAP()


//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización

BOOL CProp_Rot_eje::OnInitDialog()
{
	CProp_Rot::OnInitDialog();

	GetDlgItem(IDAsigPuertoEj)->ShowWindow(SW_SHOW);

	return TRUE;
}

//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/

void CProp_Rot_eje::CargarDatos()
{
	CProp_Rot::CargarDatos();

	if((editor->CogerAcelPed()==0) || ((pagina==72)&&(editor->CogerAcelPed()==1)))
		sAsignarPuerto.SetCheck(BST_CHECKED);
	if((editor->CogerAcelPed()==0) || ((pagina==78)&&(editor->CogerAcelPed()==2)))
		sAsignarPuerto.SetCheck(BST_CHECKED);

	if(sAsignarPuerto.GetCheck()==BST_CHECKED) {
		GetDlgItem(IDLabelComandos)->EnableWindow(FALSE);
		GetDlgItem(IDLabelIncrementar)->EnableWindow(FALSE);
		GetDlgItem(IDLabelDecrementar)->EnableWindow(FALSE);
		GetDlgItem(IDLabelPos)->EnableWindow(FALSE);
		GetDlgItem(IDbInsertar)->EnableWindow(FALSE);
		GetDlgItem(IDbBorrar)->EnableWindow(FALSE);
		sIncremental.EnableWindow(FALSE);
		sPosiciones.EnableWindow(FALSE);
		comboInc.EnableWindow(FALSE);
		comboDec.EnableWindow(FALSE);
		listaCmds.EnableWindow(FALSE);
		listaPos.EnableWindow(FALSE);

		GetDlgItem(IDbInsertarPk)->EnableWindow(FALSE);
		GetDlgItem(IDbBorrarPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelIncrementarPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelDecrementarPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelPosPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelComandosPk)->EnableWindow(FALSE);
		sIncrementalPk.EnableWindow(FALSE);
		sPosicionesPk.EnableWindow(FALSE);
		comboIncPk.EnableWindow(FALSE);
		comboDecPk.EnableWindow(FALSE);
		listaCmdsPk.EnableWindow(FALSE);
		listaPosPk.EnableWindow(FALSE);
	}
}

//-------------------------------------------------//

void CProp_Rot_eje::GuardarDatos()
{
	CProp_Rot::GuardarDatos();

	if(pagina==72) {
		if(sAsignarPuerto.GetCheck()==BST_CHECKED) {
			if(editor->CogerAcelPed()==3) editor->PonerAcelPed(1);
			if(editor->CogerAcelPed()==2) editor->PonerAcelPed(0);
		} else {
			if(editor->CogerAcelPed()==0) editor->PonerAcelPed(2);
			if(editor->CogerAcelPed()==1) editor->PonerAcelPed(3);
		}
	} else {
		if(sAsignarPuerto.GetCheck()==BST_CHECKED) {
			if(editor->CogerAcelPed()==1) editor->PonerAcelPed(0);
			if(editor->CogerAcelPed()==3) editor->PonerAcelPed(2);
		} else {
			if(editor->CogerAcelPed()==0) editor->PonerAcelPed(1);
			if(editor->CogerAcelPed()==2) editor->PonerAcelPed(3);
		}
	}
}

//-------------------------------------------------//

void CProp_Rot_eje::ResetGUI()
{
	CProp_Rot::ResetGUI();
	sAsignarPuerto.SetCheck(BST_UNCHECKED);
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/

void CProp_Rot_eje::OnBnClickedAsigPuerto()
{
	GetDlgItem(IDLabelComandos)->EnableWindow();
	GetDlgItem(IDLabelIncrementar)->EnableWindow();
	GetDlgItem(IDLabelDecrementar)->EnableWindow();
	GetDlgItem(IDLabelPos)->EnableWindow();
	GetDlgItem(IDbInsertar)->EnableWindow();
	GetDlgItem(IDbBorrar)->EnableWindow();
	sIncremental.EnableWindow();
	sPosiciones.EnableWindow();
	comboInc.EnableWindow();
	comboDec.EnableWindow();
	listaCmds.EnableWindow();
	listaPos.EnableWindow();

	if(editor->ConPinkie()) {
		GetDlgItem(IDbInsertarPk)->EnableWindow();
		GetDlgItem(IDbBorrarPk)->EnableWindow();
		GetDlgItem(IDLabelIncrementarPk)->EnableWindow();
		GetDlgItem(IDLabelDecrementarPk)->EnableWindow();
		GetDlgItem(IDLabelPosPk)->EnableWindow();
		GetDlgItem(IDLabelComandosPk)->EnableWindow();
		sIncrementalPk.EnableWindow();
		sPosicionesPk.EnableWindow();
		comboIncPk.EnableWindow();
		comboDecPk.EnableWindow();
		listaCmdsPk.EnableWindow();
		listaPosPk.EnableWindow();
	}

	GuardarDatos();
	ResetGUI();
	CargarDatos();
}

BOOL CProp_Rot_eje::OnKillActive()
{
	GuardarDatos();
	return CPropertyPage::OnKillActive();
}

BOOL CProp_Rot_eje::OnSetActive()
{
	ResetGUI();
	CargarDatos();
	return CPropertyPage::OnSetActive();
}