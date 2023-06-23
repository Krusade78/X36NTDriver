// Prop_Rot_eje.cpp: Página de propiedades de ejes
// Implementación

#include "stdafx.h"
#include "x36editor.h"
#include "ed_lcomandos.h"
#include "Prop_Rot.h"

// Constructor de la Clase

IMPLEMENT_DYNAMIC(CProp_Rot, CPropertyPage)
CProp_Rot::CProp_Rot(CX36Editor *editor,CEd_LComandos *cmds,UCHAR modo)
	: CPropertyPage(IDD_X36MAP_EDITOR_EJES)
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

void CProp_Rot::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDComboInc, comboInc);
	DDX_Control(pDX, IDComboDec, comboDec);
	DDX_Control(pDX, IDComboIncPk, comboIncPk);
	DDX_Control(pDX, IDComboDecPk, comboDecPk);
	DDX_Control(pDX, IDListaCmds, listaCmds);
	DDX_Control(pDX, IDListaCmdsPk, listaCmdsPk);
	DDX_Control(pDX, IDListaPos, listaPos);
	DDX_Control(pDX, IDListaPosPk, listaPosPk);
	DDX_Control(pDX, IDIncremental, sIncremental);
	DDX_Control(pDX, IDPosiciones, sPosiciones);
	DDX_Control(pDX, IDIncrementalPk, sIncrementalPk);
	DDX_Control(pDX, IDPosicionesPk, sPosicionesPk);
	DDX_Control(pDX, IDSensibilidad, sensi);
	DDX_Control(pDX, IDSensibilidadPk, sensiPk);
}

BEGIN_MESSAGE_MAP(CProp_Rot, CPropertyPage)
	ON_BN_CLICKED(IDPosiciones, OnBnClickedPosiciones)
	ON_BN_CLICKED(IDIncremental, OnBnClickedIncremental)
	ON_BN_CLICKED(IDPosicionesPk, OnBnClickedPosicionesPk)
	ON_BN_CLICKED(IDIncrementalPk, OnBnClickedIncrementalPk)
	ON_BN_CLICKED(IDbBorrar, OnBnClickedbborrar)
	ON_BN_CLICKED(IDbBorrarPk, OnBnClickedbborrarpk)
	ON_BN_CLICKED(IDbInsertar, OnBnClickedbinsertar)
	ON_BN_CLICKED(IDbInsertarPk, OnBnClickedbinsertarpk)
END_MESSAGE_MAP()


//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización

BOOL CProp_Rot::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	comboInc.AddString(TIP3);
	comboIncPk.AddString(TIP3);
	comboDec.AddString(TIP3);
	comboDecPk.AddString(TIP3);
	sensi.SetRange(0,15);
	sensiPk.SetRange(0,15);
	// Cargar comandos
	{
	UINT16 i=0;
	CString nombres;
	while(cmds->CogerNombres(i,&nombres))
	{
		comboInc.AddString(nombres.GetBuffer());
		comboIncPk.AddString(nombres.GetBuffer());
		comboDec.AddString(nombres.GetBuffer());
		comboDecPk.AddString(nombres.GetBuffer());
		listaCmds.AddString(nombres.GetBuffer());
		listaCmdsPk.AddString(nombres.GetBuffer());
		i++;
	}
	}

	return TRUE;
}

//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/

void CProp_Rot::CargarDatos()
{
	bool inc,incPk;
	UCHAR pos=editor->PosicionActual();
	inc=((editor->CogerGiraT(modo)&32)==32)?true:false;
	incPk=((editor->CogerGiraT(modo+1)&32)==32)?true:false;

	if(inc) {
		comboInc.SetCurSel(editor->CogerIndice(modo,pos));
		comboDec.SetCurSel(editor->CogerIndice(modo,pos+1));
		sensi.SetPos(editor->CogerGiraT(modo)&31);

		sIncremental.SetCheck(BST_CHECKED);
		sPosiciones.SetCheck(BST_UNCHECKED);
		GetDlgItem(IDLabelIncrementar)->ShowWindow(SW_SHOW);
		GetDlgItem(IDLabelDecrementar)->ShowWindow(SW_SHOW);
		GetDlgItem(IDLabelSensibilidad)->ShowWindow(SW_SHOW);
		GetDlgItem(IDLabelMax)->ShowWindow(SW_SHOW);
		GetDlgItem(IDLabelMin)->ShowWindow(SW_SHOW);
		comboInc.ShowWindow(SW_SHOW);
		comboDec.ShowWindow(SW_SHOW);
		sensi.ShowWindow(SW_SHOW);
	} else {
		CString nombre;
		UCHAR k;
		if(pos<84) k=6; else k=10;
		for(UCHAR i=0;i<k;i++) {
			if(editor->CogerIndice(modo,pos+i)!=0) {
				listaCmds.GetText(editor->CogerIndice(modo,pos+i)-1,nombre);
				listaPos.AddString(nombre.GetBuffer());
			}
		}
		sIncremental.SetCheck(BST_UNCHECKED);
		sPosiciones.SetCheck(BST_CHECKED);
		GetDlgItem(IDLabelComandos)->ShowWindow(SW_SHOW);
		GetDlgItem(IDLabelPos)->ShowWindow(SW_SHOW);
		GetDlgItem(IDbInsertar)->ShowWindow(SW_SHOW);
		GetDlgItem(IDbBorrar)->ShowWindow(SW_SHOW);
		listaCmds.ShowWindow(SW_SHOW);
		listaPos.ShowWindow(SW_SHOW);
	}

	// Con Pinkie

	if(incPk) {
		comboIncPk.SetCurSel(editor->CogerIndice(modo+1,pos));
		comboDecPk.SetCurSel(editor->CogerIndice(modo+1,pos+1));
		sensiPk.SetPos(editor->CogerGiraT(modo+1)&31);

		sIncrementalPk.SetCheck(BST_CHECKED);
		sPosicionesPk.SetCheck(BST_UNCHECKED);
		GetDlgItem(IDLabelIncrementarPk)->ShowWindow(SW_SHOW);
		GetDlgItem(IDLabelDecrementarPk)->ShowWindow(SW_SHOW);
		GetDlgItem(IDLabelSensibilidadPk)->ShowWindow(SW_SHOW);
		GetDlgItem(IDLabelMaxPk)->ShowWindow(SW_SHOW);
		GetDlgItem(IDLabelMinPk)->ShowWindow(SW_SHOW);
		comboIncPk.ShowWindow(SW_SHOW);
		comboDecPk.ShowWindow(SW_SHOW);
		sensiPk.ShowWindow(SW_SHOW);
	} else {
		CString nombre;
		UCHAR k;
		if(pos<84) k=6; else k=10;
		for(UCHAR i=0;i<k;i++) {
			if(editor->CogerIndice(modo+1,pos+i)!=0) {
				listaCmds.GetText(editor->CogerIndice(modo+1,pos+i)-1,nombre);
				listaPosPk.AddString(nombre.GetBuffer());
			}
		}
		sIncrementalPk.SetCheck(BST_UNCHECKED);
		sPosicionesPk.SetCheck(BST_CHECKED);
		GetDlgItem(IDLabelComandosPk)->ShowWindow(SW_SHOW);
		GetDlgItem(IDLabelPosPk)->ShowWindow(SW_SHOW);
		GetDlgItem(IDbInsertarPk)->ShowWindow(SW_SHOW);
		GetDlgItem(IDbBorrarPk)->ShowWindow(SW_SHOW);
		listaCmdsPk.ShowWindow(SW_SHOW);
		listaPosPk.ShowWindow(SW_SHOW);
	}

	if(!editor->ConPinkie()) {
		sIncrementalPk.EnableWindow(FALSE);
		sPosicionesPk.EnableWindow(FALSE);
		GetDlgItem(IDbInsertarPk)->EnableWindow(FALSE);
		GetDlgItem(IDbBorrarPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelIncrementarPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelDecrementarPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelSensibilidadPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelMaxPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelMinPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelPosPk)->EnableWindow(FALSE);
		GetDlgItem(IDLabelComandosPk)->EnableWindow(FALSE);
		comboIncPk.EnableWindow(FALSE);
		comboDecPk.EnableWindow(FALSE);
		sensiPk.EnableWindow(FALSE);
		listaCmdsPk.EnableWindow(FALSE);
		listaPosPk.EnableWindow(FALSE);
	}
}

//-------------------------------------------------//

void CProp_Rot::GuardarDatos()
{
	UCHAR i=0,k,rot;

	if(pagina<84) k=6; else k=10;
	switch(pagina)
	{
		case 72: rot=0; break;
		case 78: rot=1; break;
		case 84: rot=2; break;
		case 94: rot=3; break;
	}

	if(sIncremental.GetCheck()==BST_CHECKED) {
		editor->PonerIndice(modo,pagina,comboInc.GetCurSel());
		editor->PonerIndice(modo,pagina+1,comboDec.GetCurSel());
		for(i=2;i<k;i++) editor->PonerIndice(modo,pagina+i,0);
		editor->PonerGiraT(rot,modo,(32|(UCHAR)sensi.GetPos()));
	} else {
		CString nombre;
		for(i=0;i<listaPos.GetCount();i++) {
			listaPos.GetText(i,nombre);
			editor->PonerIndice(modo,pagina+i,listaCmds.FindString(-1,nombre.GetBuffer())+1);
		}
		for(i=listaPos.GetCount();i<k;i++) editor->PonerIndice(modo,pagina+i,0);
		editor->PonerGiraT(rot,modo,listaPos.GetCount());
	}
	// Datos "Con Pinkie"
	if(sIncrementalPk.GetCheck()==BST_CHECKED) {
		editor->PonerIndice(modo+1,pagina,comboIncPk.GetCurSel());
		editor->PonerIndice(modo+1,pagina+1,comboDecPk.GetCurSel());
		for(i=2;i<k;i++) editor->PonerIndice(modo+1,pagina+i,0);
		editor->PonerGiraT(rot,modo+1,(32|(UCHAR)sensiPk.GetPos()));
	} else {
		CString nombre;
		for(i=0;i<listaPosPk.GetCount();i++) {
			listaPosPk.GetText(i,nombre);
			editor->PonerIndice(modo+1,pagina+i,listaCmds.FindString(-1,nombre.GetBuffer())+1);
		}
		for(i=listaPosPk.GetCount();i<k;i++) editor->PonerIndice(modo+1,pagina+i,0);
		editor->PonerGiraT(rot,modo+1,listaPosPk.GetCount());
	}
}

//-------------------------------------------------//

void CProp_Rot::ResetGUI()
{
	if(editor->ConPinkie()) {
		sIncrementalPk.EnableWindow();
		sPosicionesPk.EnableWindow();
		GetDlgItem(IDbInsertarPk)->EnableWindow();
		GetDlgItem(IDbBorrarPk)->EnableWindow();
		GetDlgItem(IDLabelIncrementarPk)->EnableWindow();
		GetDlgItem(IDLabelDecrementarPk)->EnableWindow();
		GetDlgItem(IDLabelSensibilidadPk)->EnableWindow();
		GetDlgItem(IDLabelMaxPk)->EnableWindow();
		GetDlgItem(IDLabelMinPk)->EnableWindow();
		GetDlgItem(IDLabelPosPk)->EnableWindow();
		GetDlgItem(IDLabelComandosPk)->EnableWindow();
		comboIncPk.EnableWindow();
		comboDecPk.EnableWindow();
		sensiPk.EnableWindow();
		listaCmdsPk.EnableWindow();
		listaPosPk.EnableWindow();
	}
	GetDlgItem(IDLabelComandos)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelPos)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelIncrementar)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelDecrementar)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelSensibilidad)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelMax)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelMin)->ShowWindow(SW_HIDE);
	GetDlgItem(IDbInsertar)->ShowWindow(SW_HIDE);
	GetDlgItem(IDbBorrar)->ShowWindow(SW_HIDE);
	listaCmds.ShowWindow(SW_HIDE);
	listaPos.ShowWindow(SW_HIDE);
	comboInc.ShowWindow(SW_HIDE);
	comboDec.ShowWindow(SW_HIDE);
	sensi.ShowWindow(SW_HIDE);

	GetDlgItem(IDbInsertarPk)->ShowWindow(SW_HIDE);
	GetDlgItem(IDbBorrarPk)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelIncrementarPk)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelDecrementarPk)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelSensibilidadPk)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelMaxPk)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelMinPk)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelPosPk)->ShowWindow(SW_HIDE);
	GetDlgItem(IDLabelComandosPk)->ShowWindow(SW_HIDE);
	comboIncPk.ShowWindow(SW_HIDE);
	comboDecPk.ShowWindow(SW_HIDE);
	sensiPk.ShowWindow(SW_HIDE);
	listaCmdsPk.ShowWindow(SW_HIDE);
	listaPosPk.ShowWindow(SW_HIDE);

	listaPos.ResetContent();
	listaPosPk.ResetContent();
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/

void CProp_Rot::OnBnClickedPosiciones()
{
	UCHAR rot;
	switch(pagina)
	{
		case 72: rot=0; break;
		case 78: rot=1; break;
		case 84: rot=2; break;
		case 94: rot=3; break;
	}
	if((editor->CogerGiraT(rot,modo)&32)!=32) {
		return;
	} else {
		editor->PonerGiraT(rot,modo,0);
	}

	if(pagina<84)
		for(UCHAR i=0;i<6;i++) editor->PonerIndice(modo,pagina+i,0);
	else
		for(UCHAR i=0;i<10;i++) editor->PonerIndice(modo,pagina+i,0);
	GuardarDatos();
	ResetGUI();
	CargarDatos();
}

void CProp_Rot::OnBnClickedIncremental()
{
	UCHAR rot;
	switch(pagina)
	{
		case 72: rot=0; break;
		case 78: rot=1; break;
		case 84: rot=2; break;
		case 94: rot=3; break;
	}

	if((editor->CogerGiraT(rot,modo)&32)==32) {
		return;
	} else {
		editor->PonerGiraT(rot,modo,32);
	}

	if(pagina<84)
		for(UCHAR i=0;i<6;i++) editor->PonerIndice(modo,pagina+i,0);
	else
		for(UCHAR i=0;i<10;i++) editor->PonerIndice(modo,pagina+i,0);
	comboDec.SetCurSel(0);
	comboInc.SetCurSel(0);
	GuardarDatos();
	ResetGUI();
	CargarDatos();
}

void CProp_Rot::OnBnClickedPosicionesPk()
{
	UCHAR rot;
	switch(pagina)
	{
		case 72: rot=0; break;
		case 78: rot=1; break;
		case 84: rot=2; break;
		case 94: rot=3; break;
	}

	if((editor->CogerGiraT(rot,modo+1)&32)!=32) {
		return;
	} else {
		editor->PonerGiraT(rot,modo+1,0);
	}

	if(pagina<84)
		for(UCHAR i=0;i<6;i++) editor->PonerIndice(modo+1,pagina+i,0);
	else
		for(UCHAR i=0;i<10;i++) editor->PonerIndice(modo+1,pagina+i,0);
	GuardarDatos();
	ResetGUI();
	CargarDatos();
}

void CProp_Rot::OnBnClickedIncrementalPk()
{
	UCHAR rot;
	switch(pagina)
	{
		case 72: rot=0; break;
		case 78: rot=1; break;
		case 84: rot=2; break;
		case 94: rot=3; break;
	}

	if((editor->CogerGiraT(rot,modo+1)&32)==32) {
		return;
	} else {
		editor->PonerGiraT(rot,modo+1,32);
	}

	if(pagina<84)
		for(UCHAR i=0;i<6;i++) editor->PonerIndice(modo+1,pagina+i,0);
	else
		for(UCHAR i=0;i<10;i++) editor->PonerIndice(modo+1,pagina+i,0);
	comboDecPk.SetCurSel(0);
	comboIncPk.SetCurSel(0);
	GuardarDatos();
	ResetGUI();
	CargarDatos();}

void CProp_Rot::OnBnClickedbborrar()
{
	if(listaPos.GetCurSel()>=0)
		listaPos.DeleteString(listaPos.GetCurSel());
}

void CProp_Rot::OnBnClickedbborrarpk()
{
	if(listaPosPk.GetCurSel()>=0)
		listaPosPk.DeleteString(listaPosPk.GetCurSel());
}

void CProp_Rot::OnBnClickedbinsertar()
{
	if(listaCmds.GetCurSel()>=0) {
		UCHAR k;
		if(pagina<84) k=6; else k=10;
		if(listaPos.GetCount()<=k) {
			CString nombre;
			listaCmds.GetText(listaCmds.GetCurSel(),nombre);
			listaPos.AddString(nombre.GetBuffer());
		} else {
			if(k==10) {
				MessageBox(MSG11,MSG12,MB_ICONWARNING);
			} else {
				MessageBox(MSG20,MSG12,MB_ICONWARNING);
			}
		}
	}
}

void CProp_Rot::OnBnClickedbinsertarpk()
{
	if(listaCmdsPk.GetCurSel()>=0) {
		UCHAR k;
		if(pagina<84) k=6; else k=10;
		if(listaPosPk.GetCount()<=k) {
			CString nombre;
			listaCmdsPk.GetText(listaCmdsPk.GetCurSel(),nombre);
			listaPosPk.AddString(nombre.GetBuffer());
		} else {
			if(k==10) {
				MessageBox(MSG11,MSG12,MB_ICONWARNING);
			} else {
				MessageBox(MSG20,MSG12,MB_ICONWARNING);
			}
		}
	}
}


BOOL CProp_Rot::OnKillActive()
{
	GuardarDatos();
	return CPropertyPage::OnKillActive();
}


BOOL CProp_Rot::OnSetActive()
{
	ResetGUI();
	CargarDatos();
	return CPropertyPage::OnSetActive();
}
