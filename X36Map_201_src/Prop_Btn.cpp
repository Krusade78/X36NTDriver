// Prop_Btn.cpp: Página de propiedades de botones
// Implementación

#include "stdafx.h"
#include "x36editor.h"
#include "ed_lcomandos.h"
#include "Prop_Btn.h"

// Constructor de la Clase

IMPLEMENT_DYNAMIC(CProp_Btn, CPropertyPage)
CProp_Btn::CProp_Btn(CX36Editor *editor,CEd_LComandos *cmds,UCHAR modo)
	: CPropertyPage(IDD_X36MAP_EDITOR_BOTONES)
{
	this->editor = editor;
	this->cmds = cmds;
	this->m_pPSP->dwFlags |=PSP_USETITLE;
	pagina=editor->PosicionActual();
	this->modo=modo;
	if(editor->ConModos()) {
		switch(modo)
		{
			case 0:
				this->m_pPSP->pszTitle=BTN2;
				break;
			case 2:
				this->m_pPSP->pszTitle=BTN3;
				break;
			case 4:
				this->m_pPSP->pszTitle=BTN4;
		}
	} else {
		this->m_pPSP->pszTitle=BTN5;
	}
}

/*************************************************
*************** Control de Mensajes **************
**************************************************/

void CProp_Btn::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDComboApr, comboApr);
	DDX_Control(pDX, IDComboSol, comboSol);
	DDX_Control(pDX, IDComboAprPk, comboAprPk);
	DDX_Control(pDX, IDComboSolPk, comboSolPk);
	DDX_Control(pDX, IDAutoRepeat, sAutoRepeat);
	DDX_Control(pDX, IDAutoRepeatPk, sAutoRepeatPk);
}


BEGIN_MESSAGE_MAP(CProp_Btn, CPropertyPage)
END_MESSAGE_MAP()


//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización

BOOL CProp_Btn::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	comboApr.AddString(TIP3);
	comboAprPk.AddString(TIP3);
	comboSol.AddString(TIP3);
	comboSolPk.AddString(TIP3);
	{
	UINT16 i=0;
	CString nombres;
	while(cmds->CogerNombres(i,&nombres))
	{
		comboApr.AddString(nombres.GetBuffer());
		comboAprPk.AddString(nombres.GetBuffer());
		comboSol.AddString(nombres.GetBuffer());
		comboSolPk.AddString(nombres.GetBuffer());
		i++;
	}
	}

	return TRUE;
}

//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/

void CProp_Btn::CargarDatos()
{
	comboApr.SetCurSel(editor->CogerIndice(modo,pagina));
	comboSol.SetCurSel(editor->CogerIndice(modo,pagina+36));
	comboAprPk.SetCurSel(editor->CogerIndice(modo+1,pagina));
	comboSolPk.SetCurSel(editor->CogerIndice(modo+1,pagina+36));
	if(editor->CogerAutoRepeat(modo,pagina)) sAutoRepeat.SetCheck(BST_CHECKED);
	if(editor->CogerAutoRepeat(modo+1,pagina)) sAutoRepeatPk.SetCheck(BST_CHECKED);

	if( (!editor->ConPinkie() || ((pagina==5) && editor->ConPinkie()))
	||	(editor->ConModos() && ((pagina==24) || (pagina==25))) ) {
			GetDlgItem(IDLabelApretarPk)->EnableWindow(FALSE);
			GetDlgItem(IDLabelSoltarPk)->EnableWindow(FALSE);
			sAutoRepeatPk.EnableWindow(FALSE);
			comboAprPk.EnableWindow(FALSE);
			comboSolPk.EnableWindow(FALSE);
	}
	if(	(editor->ConModos() && ((pagina==24) || (pagina==25)))
	||	(editor->ConPinkie()&& (pagina==5)) ) {
			GetDlgItem(IDLabelApretar)->EnableWindow(FALSE);
			GetDlgItem(IDLabelSoltar)->EnableWindow(FALSE);
			sAutoRepeat.EnableWindow(FALSE);
			comboApr.EnableWindow(FALSE);
		comboSol.EnableWindow(FALSE);
	}

}

//-------------------------------------------------//

void CProp_Btn::GuardarDatos()
{
	editor->PonerIndice(modo,pagina,comboApr.GetCurSel());
	editor->PonerIndice(modo+1,pagina,comboAprPk.GetCurSel());
	editor->PonerIndice(modo,pagina+36,comboSol.GetCurSel());
	editor->PonerIndice(modo+1,pagina+36,comboSolPk.GetCurSel());
	if(sAutoRepeat.GetCheck()==BST_CHECKED) {
		editor->PonerAutoRepeat(modo,pagina,true);
	} else {
		editor->PonerAutoRepeat(modo,pagina,false);
	}
	if(sAutoRepeatPk.GetCheck()==BST_CHECKED) {
		editor->PonerAutoRepeat(modo+1,pagina,true);
	} else {
		editor->PonerAutoRepeat(modo+1,pagina,false);
	}
}

//-------------------------------------------------//

void CProp_Btn::ResetGUI()
{
	if(editor->ConPinkie()) {
		GetDlgItem(IDLabelApretarPk)->EnableWindow();
		GetDlgItem(IDLabelSoltarPk)->EnableWindow();
		sAutoRepeatPk.EnableWindow();
		comboAprPk.EnableWindow();
		comboSolPk.EnableWindow();
	}
	GetDlgItem(IDLabelApretar)->EnableWindow();
	GetDlgItem(IDLabelSoltar)->EnableWindow();
	sAutoRepeat.EnableWindow();
	comboApr.EnableWindow();
	comboSol.EnableWindow();
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/

BOOL CProp_Btn::OnKillActive()
{
	GuardarDatos();
	return CPropertyPage::OnKillActive();
}

BOOL CProp_Btn::OnSetActive()
{
	ResetGUI();
	CargarDatos();
	return CPropertyPage::OnSetActive();
}
