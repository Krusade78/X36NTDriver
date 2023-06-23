// Prop_Btn_pov.cpp: Página de propiedades de botones del puerto
// Implementación

#include "stdafx.h"
#include "x36editor.h"
#include "ed_lcomandos.h"
#include "Prop_Btn_prt.h"

// Constructor de la Clase

IMPLEMENT_DYNAMIC(CProp_Btn_prt, CProp_Btn)
CProp_Btn_prt::CProp_Btn_prt(CX36Editor *editor,CEd_LComandos *cmds,UCHAR modo)
	:CProp_Btn(editor,cmds,modo)
{}

/*************************************************
*************** Control de Mensajes **************
**************************************************/

void CProp_Btn_prt::DoDataExchange(CDataExchange* pDX)
{
	CProp_Btn::DoDataExchange(pDX);
	DDX_Control(pDX, IDAsigPuertoBt, sAsignarPuerto);
	DDX_Control(pDX, IDComboPuerto, comboPuerto);
}

BEGIN_MESSAGE_MAP(CProp_Btn_prt, CProp_Btn)
	ON_BN_CLICKED(IDAsigPuertoBt, OnBnClickedAsigPuerto)
END_MESSAGE_MAP()


//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización

BOOL CProp_Btn_prt::OnInitDialog()
{
	CProp_Btn::OnInitDialog();

	GetDlgItem(IDComboPuerto)->ShowWindow(SW_SHOW);
	GetDlgItem(IDAsigPuertoBt)->ShowWindow(SW_SHOW);

	return TRUE;
}

//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/

void CProp_Btn_prt::CargarDatos()
{
	CProp_Btn::CargarDatos();

	CString nombre;
	char n[3];

	for(UCHAR i=1;i<37;i++)
	{
		nombre=TIP4;
		nombre.Append(" ");
		nombre.Append(itoa(i,n,10));
		comboPuerto.AddString(nombre.GetBuffer());
	}

	// Cargar Asignación de puerto

	if(editor->CogerBtDx(pagina)>0)
	{
		comboPuerto.SetCurSel(editor->CogerBtDx(pagina)-1);
		comboPuerto.EnableWindow();
		sAsignarPuerto.SetCheck(BST_CHECKED);
	}
}

//-------------------------------------------------//

void CProp_Btn_prt::GuardarDatos()
{
	CProp_Btn::GuardarDatos();

	if(sAsignarPuerto.GetCheck()==BST_CHECKED) {
		editor->PonerBtDx(pagina,comboPuerto.GetCurSel()+1);
	} else {
		// Liberar Botón
		editor->PonerBtDx(pagina,0);
	}
}

//-------------------------------------------------//

void CProp_Btn_prt::ResetGUI()
{
	CProp_Btn::ResetGUI();
	sAsignarPuerto.SetCheck(BST_UNCHECKED);
	comboPuerto.EnableWindow(FALSE);
	comboPuerto.ResetContent();
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/

void CProp_Btn_prt::OnBnClickedAsigPuerto()
{
	comboPuerto.SetCurSel(0);
	GuardarDatos();
	ResetGUI();
	CargarDatos();
}

BOOL CProp_Btn_prt::OnKillActive()
{
	GuardarDatos();
	return CPropertyPage::OnKillActive();
}

BOOL CProp_Btn_prt::OnSetActive()
{
	ResetGUI();
	CargarDatos();
	return CPropertyPage::OnSetActive();
}
