// X36Editor.cpp: Dialogo para editar mapas
// Implementación

#include "stdafx.h"
#include "X36Editor.h"
#include "Ed_LComandos.h"
#include "Ed_Resumen.h"
#include "Prop_Btn_pov.h"
#include "Prop_Rot_eje.h"

// Constructor de la Clase

CX36Editor::CX36Editor(CString archivoXMP,CWnd* pParent /*=NULL*/)
	: CDialog(IDD_X36MAP_EDITOR, pParent)
{
	tab2CurSel=0;
	tab3CurSel=0;
	pagina0=NULL;
	pagina1=NULL;
	pagina2=NULL;
	this->archivoXMP=archivoXMP;
	CCmds = new CEd_LComandos(this);
	CCmds->Create(IDD_X36MAP_COMANDOS,this);
	tab1.Construct("Propiedades",this);
}

CX36Editor::~CX36Editor()
{
	if(CCmds) {
		CCmds->DestroyWindow();
		delete CCmds; CCmds=NULL;
	}
}


/*************************************************
*************** Control de Mensajes **************
**************************************************/

void CX36Editor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDTab2, tab2);
	DDX_Control(pDX, IDTab3, tab3);
	DDX_Control(pDX, IDEjeX, combox);
	DDX_Control(pDX, IDEjeY, comboy);
	DDX_Control(pDX, IDEjeZ, comboz);
	DDX_Control(pDX, IDEjeR, combor);
	DDX_Control(pDX, IDPinkie, sPinkie);
	DDX_Control(pDX, IDModos, sModo);
}


BEGIN_MESSAGE_MAP(CX36Editor, CDialog)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, Hints )
	ON_NOTIFY(TCN_SELCHANGE, IDTab2, OnTcnSelchangeTab2)
	ON_NOTIFY(TCN_SELCHANGE, IDTab3, OnTcnSelchangeTab3)
	ON_BN_CLICKED(IDPinkie, OnBnClickedPinkie)
	ON_BN_CLICKED(IDModos, OnBnClickedModos)
	ON_WM_CLOSE()
	ON_COMMAND(ID_ARCHIVO_GUARDAR, OnBnClickedGuardar)
	ON_COMMAND(ID_VER_COMANDOS, OnBnClickedEditorcomandos)
	ON_COMMAND(ID_VER_RESUMEN, OnBnClickedResumen)
	ON_COMMAND(ID_ARCHIVO_SALIR, OnSalir)
	ON_COMMAND(ID_SOBRE_SOBREELX36MAP, OnSobreSobreelx36map)
	ON_COMMAND(ID_ARCHIVO_GUARDARCOMO, OnArchivoGuardarcomo)
END_MESSAGE_MAP()

BOOL CX36Editor::Hints( UINT id, NMHDR* pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;

	switch(pTTTStruct->idFrom)
	{
		case ID_ARCHIVO_GUARDAR:
			pTTT->lpszText=TIP1;
			return TRUE;
		case ID_VER_COMANDOS:
			pTTT->lpszText=TIP2;
			return TRUE;
		case ID_VER_RESUMEN:
			pTTT->lpszText=TIP0;
			return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización

BOOL CX36Editor::OnInitDialog()
{
	CDialog::OnInitDialog();
	EnableToolTips(TRUE);
	combox.AddString(TIP5);
	combox.AddString(TIP6);
	combox.AddString(BTN36);
	combox.AddString(BTN37);
	comboy.AddString(TIP5);
	comboy.AddString(TIP6);
	comboy.AddString(BTN36);
	comboy.AddString(BTN37);
	comboz.AddString(TIP5);
	comboz.AddString(TIP6);
	comboz.AddString(BTN36);
	comboz.AddString(BTN37);
	combor.AddString(TIP5);
	combor.AddString(TIP6);
	combor.AddString(BTN36);
	combor.AddString(BTN37);
	archivoXMP.Insert(0,"Editor - ");
	this->SetWindowText(archivoXMP.GetBuffer());
	archivoXMP.Delete(0,9);
	CargarArchivo();
	SetIcon(AfxGetApp()->LoadIcon(IDI_Principal),TRUE);
	SetIcon(AfxGetApp()->LoadIcon(IDI_Principal),FALSE);
	tab2.InsertItem(0,"Joystick (X36)");
	tab2.InsertItem(1,BTN1);
	CSize tam;
		tam.cx=175;
		tam.cy=14;
	tab3.SetItemSize(tam);
	CambiarTab3();
	CambiarPagina(true,false);
	tab1.Create(this,WS_CHILD,WS_EX_CONTROLPARENT);
	tab1.SetWindowPos(this,7,53,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);
	ConfigurarBarra();
	return TRUE;
}

//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/

//------------- Configurar la barra de herremientas

void CX36Editor::ConfigurarBarra()
{
	TBBUTTON bt[4];

	bt[0].iBitmap=0;
	bt[1].iBitmap=1;
	bt[2].iBitmap=2;
	bt[3].iBitmap=-1;

	bt[0].iString=-1;
	bt[1].iString=-1;
	bt[2].iString=-1;
	bt[3].iString=-1;

	bt[0].fsStyle=TBSTYLE_BUTTON;
	bt[1].fsStyle=bt[0].fsStyle;
	bt[2].fsStyle=bt[0].fsStyle;
	bt[3].fsStyle=TBSTYLE_SEP;

	bt[0].fsState=TBSTATE_ENABLED;
	bt[1].fsState=bt[0].fsState;
	bt[2].fsState=bt[0].fsState;
	bt[3].fsState=bt[0].fsState;

	bt[0].idCommand=ID_VER_COMANDOS;
	bt[1].idCommand=ID_ARCHIVO_GUARDAR;
	bt[2].idCommand=ID_VER_RESUMEN;
	bt[3].idCommand=0;

	barra.Create(TBSTYLE_TRANSPARENT|WS_CHILD|WS_VISIBLE|TBSTYLE_FLAT|TBSTYLE_TOOLTIPS,(CRect)NULL,this,0);
	barra.SetBitmapSize(CSize(16,16));
	barra.SetButtonSize(CSize(32,32));
    barra.AddBitmap(3,IDB_TOOLBAR);
	barra.AddButtons(3,&bt[0]);
	barra.InsertButton(1,&bt[3]);
	barra.InsertButton(0,&bt[3]);
	barra.AutoSize();
	linea.Create(NULL,WS_CHILD|WS_VISIBLE|SS_ETCHEDHORZ ,CRect(0,35,560,1),this);
}

//------------- Cargar y Guardar .XMP

void CX36Editor::CargarArchivo()
{
	CFile f;
	UINT16 n_cmds=0;
	UINT64 teclas=0;
	UCHAR teclasExt=0;
	TCHAR desc[32];
	CString stDesc;
	UCHAR sel;

	if(f.Open(archivoXMP.GetBuffer(),CFile::typeBinary|CFile::modeRead)!=0){
		if(f.GetLength()<1568) goto mal;
		if(f.Read(&sel,1)!=1) goto mal;
		sModo.SetCheck((sel&128)/127);
		sPinkie.SetCheck((sel&64)/63);
		if(f.Read(&ejes,5)!=5) goto mal;
		combox.SetCurSel(ejes[1]);
		comboy.SetCurSel(ejes[2]);
		comboz.SetCurSel(ejes[3]);
		combor.SetCurSel(ejes[4]);
		if(f.Read(botonesP,36)!=36) goto mal;
		if(f.Read(povsP,36)!=36) goto mal;
		if(f.Read(giraT,24)!=24) goto mal;

		if(f.Read(&n_cmds,2)!=2) goto mal;
		for(UINT16 i=0;i<n_cmds;i++) {
			if(f.Read(desc,32)!=32) goto mal;
			stDesc.Empty();
			stDesc.Append(desc);
			if(f.Read(&teclas,8)!=8) goto mal;
			if(f.Read(&teclasExt,1)!=1) goto mal;
			((CEd_LComandos*)CCmds)->Insertar(stDesc,teclas,teclasExt,true);
		}
		if(f.Read(indices,1248)!=1248) goto mal;
		if(f.Read(autoRepeat,36*6)!=(36*6)) goto mal;
		f.Close();
	} else {
		MessageBox(MSG1,"Error",MB_ICONERROR);
		OnCancel();
	}
	return;
mal:
	f.Close();
	MessageBox(MSG1,"Error",MB_ICONERROR);
	OnOK();
}

bool CX36Editor::GuardarArchivo(bool UI)
{
	CFile f;
	UINT16 n_cmds=((CEd_LComandos*)CCmds)->NumComandos();
	UINT64 teclas=0;
	UCHAR teclasExt=0;
	TCHAR desc[32];
	CString nombre;

	UCHAR sel=(sModo.GetCheck()*128)+(sPinkie.GetCheck()*64);

	CambiarPagina(false,true);

	if(f.Open(archivoXMP.GetBuffer(),CFile::modeCreate|CFile::typeBinary|CFile::modeWrite|CFile::shareExclusive)!=0){
		f.Write(&sel,1);
		ejes[1]=combox.GetCurSel();
		ejes[2]=comboy.GetCurSel();
		ejes[3]=comboz.GetCurSel();
		ejes[4]=combor.GetCurSel();
		f.Write(ejes,5);
		f.Write(botonesP,36);
		f.Write(povsP,36);
		f.Write(giraT,24);
		f.Write(&n_cmds,2);
		for(UINT16 i=0;i<n_cmds;i++) {
			ZeroMemory(desc,32);
			((CEd_LComandos*)CCmds)->CogerNombres(i,&nombre);
			lstrcpy(desc,nombre);
			f.Write(desc,32);
			((CEd_LComandos*)CCmds)->CogerDatos(i,&teclas,&teclasExt);
			f.Write(&teclas,sizeof(UINT64));
			f.Write(&teclasExt,1);
		}
		f.Write(indices,sizeof(UINT16)*6*104);
		f.Write(autoRepeat,36*6);
		f.Close();
	} else {
		MessageBox(MSG9,"Error",MB_ICONERROR);
		return false;
	}
	if(UI) MessageBox(MSG10,"OK",MB_ICONINFORMATION);
	return true;
}

//-------------------------------------------------------/

bool CX36Editor::Salir()
{
	int resp=MessageBox(MSG22,TIP1,MB_ICONQUESTION|MB_YESNOCANCEL);

	switch(resp)
	{
		case IDYES:
			return GuardarArchivo();
		case IDNO:
			return true;
	}
	return false;
}

//-------------------------------------------------------/


// Cambiar botones al Tab 3

void CX36Editor::CambiarTab3()
{
	tab3.DeleteAllItems();
	if(tab2.GetCurSel()==0) {
		tab3.InsertItem(0,BTN6);
		tab3.InsertItem(1,BTN7);
		tab3.InsertItem(2,BTN8);
		tab3.InsertItem(3,BTN9);
		tab3.InsertItem(4,BTN10);
		tab3.InsertItem(5,"Pinkie");
		tab3.InsertItem(6,BTN12);
		tab3.InsertItem(7,BTN13);
		tab3.InsertItem(8,BTN14);
		tab3.InsertItem(9,BTN15);
		tab3.InsertItem(10,BTN38);
		tab3.InsertItem(11,BTN39);
		tab3.InsertItem(12,BTN40);
		tab3.InsertItem(13,BTN41);
		tab3.InsertItem(14,BTN16);
		tab3.InsertItem(15,BTN17);
		tab3.InsertItem(16,BTN18);
		tab3.InsertItem(17,BTN19);
		tab3.InsertItem(18,BTN42);
		tab3.InsertItem(19,BTN43);
		tab3.InsertItem(20,BTN44);
		tab3.InsertItem(21,BTN45);
	} else {
		tab3.InsertItem(0,BTN20);
		tab3.InsertItem(1,BTN21);
		tab3.InsertItem(2,BTN22);
		tab3.InsertItem(3,BTN23);
		tab3.InsertItem(4,BTN24);
		tab3.InsertItem(5,BTN25);
		tab3.InsertItem(6,BTN26);
		tab3.InsertItem(7,BTN27);
		tab3.InsertItem(8,BTN28);
		tab3.InsertItem(9,BTN29);
		tab3.InsertItem(10,BTN30);
		tab3.InsertItem(11,BTN31);
		tab3.InsertItem(12,BTN32);
		tab3.InsertItem(13,BTN33);
		tab3.InsertItem(14,BTN36);
		tab3.InsertItem(15,BTN37);
		tab3.InsertItem(16,BTN34);
		tab3.InsertItem(17,BTN35);
	}
}


void CX36Editor::CambiarPagina(bool cargar, bool guardar)
{
	if(guardar) {
		switch(tab1.GetActiveIndex())
		{
			case 0:
				GuardarPagina(&pagina0);
				break;
			case 1:
				GuardarPagina(&pagina1);
				break;
			case 2:
				GuardarPagina(&pagina2);
		}
	}

	if(cargar) {
		int tab_seleccionado=tab1.GetActiveIndex();

		if(pagina2!=NULL) {
			tab1.RemovePage(pagina2);
			delete pagina2;
			pagina2=NULL;
		}
		if(pagina1!=NULL) {
			tab1.RemovePage(pagina1);
			delete pagina1;
			pagina1=NULL;
		}
		if(pagina0!=NULL) {
			tab1.RemovePage(pagina0);
			delete pagina0;
			pagina0=NULL;
		}

		if(sModo.GetCheck()==BST_CHECKED) {
			CargarPagina(&pagina0,0);
			CargarPagina(&pagina1,2);
			CargarPagina(&pagina2,4);
			tab1.AddPage(pagina0);
			tab1.AddPage(pagina1);
			tab1.AddPage(pagina2);
		} else {
			CargarPagina(&pagina0,2);
			tab1.AddPage(pagina0);
		}

		tab1.SetActivePage(tab_seleccionado);
		tab2CurSel=tab2.GetCurSel();
		tab3CurSel=tab3.GetCurSel();
	}

}

void CX36Editor::CargarPagina(CPropertyPage** pgn, UCHAR modo)
{
	if(tab2.GetCurSel()==0) {
//		if(tab3.GetCurSel()==5) {
//			*pgn = new CProp_Btn(this,(CEd_LComandos*)CCmds,modo);
//		} else {
//			if (tab3.GetCurSel()>5) {
				*pgn = new CProp_Btn_pov(this,(CEd_LComandos*)CCmds,modo);
//			} else {
//				*pgn = new CProp_Btn_prt(this,(CEd_LComandos*)CCmds,modo);
//			}
//		}
	} else {
		switch (tab3.GetCurSel())
		{
/*			case 0:
				*pgn = new CProp_Btn_prt(this,(CEd_LComandos*)CCmds,modo);
				break;
			case 1:
				*pgn = new CProp_Btn_prt(this,(CEd_LComandos*)CCmds,modo);
				break;
*/			case 14:
				*pgn = new CProp_Rot_eje(this,(CEd_LComandos*)CCmds,modo);
				break;
			case 15:
				*pgn = new CProp_Rot_eje(this,(CEd_LComandos*)CCmds,modo);
				break;
			case 16:
				*pgn = new CProp_Rot(this,(CEd_LComandos*)CCmds,modo);
				break;
			case 17:
				*pgn = new CProp_Rot(this,(CEd_LComandos*)CCmds,modo);
				break;
			default :
//				if(tab3.GetCurSel()<6)
//					*pgn = new CProp_Btn(this,(CEd_LComandos*)CCmds,modo);
//				else
					*pgn = new CProp_Btn_pov(this,(CEd_LComandos*)CCmds,modo);
		}
	}
}


void CX36Editor::GuardarPagina(CPropertyPage** pgn)
{
	if(tab2CurSel==0) {
		if(tab3CurSel==5) {
			((CProp_Btn*)*pgn)->GuardarDatos();
		} else {
			if (tab3CurSel>5) {
				((CProp_Btn_pov*)*pgn)->GuardarDatos();
			} else {
				((CProp_Btn_prt*)*pgn)->GuardarDatos();
			}
		}
	} else {
		switch (tab3CurSel)
		{
			case 0:
				((CProp_Btn_prt*)*pgn)->GuardarDatos();
				break;
			case 1:
				((CProp_Btn_prt*)*pgn)->GuardarDatos();
				break;
			case 14:
				((CProp_Rot_eje*)*pgn)->GuardarDatos();
				break;
			case 15:
				((CProp_Rot_eje*)*pgn)->GuardarDatos();
				break;
			case 16:
				((CProp_Rot*)*pgn)->GuardarDatos();
				break;
			case 17:
				((CProp_Rot*)*pgn)->GuardarDatos();
				break;
			default :
				if(tab3CurSel<6)
					((CProp_Btn*)*pgn)->GuardarDatos();
				else
					((CProp_Btn_pov*)*pgn)->GuardarDatos();
		}
	}
}

//-------------------------------------------------------/

UCHAR CX36Editor::CogerModo()
{
	if(sModo.GetCheck()==BST_CHECKED) {
		switch(tab1.GetActiveIndex())
		{
			case 0: return 0;
			case 1: return 2;
			case 2: return 4;
		}
	} else {
		return 2;
	}
}


UCHAR CX36Editor::PosicionActual()
{
	if(tab2.GetCurSel()==0) {
		return tab3.GetCurSel();
	} else {
		switch (tab3.GetCurSel())
		{
			case 14:
				return 72;
				break;
			case 15:
				return 78;
				break;
			case 16:
				return 84;
				break;
			case 17:
				return 94;
				break;
			default :
				return (tab3.GetCurSel()+22);
		}
	}
}


UINT16 CX36Editor::CogerIndice(UCHAR modo, UCHAR pos)
{
	return indices[modo][pos];
}
void CX36Editor::PonerIndice(UCHAR modo, UCHAR pos, UINT16 idx)
{
	indices[modo][pos]=idx;
}


bool CX36Editor::CogerAutoRepeat(UCHAR modo, UCHAR pos)
{
	return autoRepeat[modo][pos];
}
void CX36Editor::PonerAutoRepeat(UCHAR modo, UCHAR pos, bool b)
{
	autoRepeat[modo][pos]=b;
}


UCHAR CX36Editor::CogerGiraT(UCHAR modo)
{
	UCHAR g;
	switch(PosicionActual())
	{
		case 72: g=0; break;
		case 78: g=1; break;
		case 84: g=2; break;
		case 94: g=3;
	}

	return giraT[g][modo];
}
UCHAR CX36Editor::CogerGiraT(UCHAR eje, UCHAR modo)
{
	return giraT[eje][modo];
}
void CX36Editor::PonerGiraT(UCHAR eje, UCHAR modo, UCHAR idx)
{
	giraT[eje][modo]=idx;
}


UCHAR CX36Editor::CogerBtDx(UCHAR pos)
{
	return botonesP[pos];
}
void CX36Editor::PonerBtDx(UCHAR pos, UCHAR idx)
{
	botonesP[pos]=idx;
}
UCHAR CX36Editor::CogerPovDx(UCHAR pos)
{
	return povsP[pos];
}
void CX36Editor::PonerPovDx(UCHAR pos, UCHAR idx)
{
	povsP[pos]=idx;
}
UCHAR CX36Editor::CogerAcelPed()
{
	return ejes[0];
}
void CX36Editor::PonerAcelPed(UCHAR idx)
{
	ejes[0]=idx;
}


bool CX36Editor::ConModos()
{
	if(sModo.GetCheck()==BST_CHECKED)
        return true;
	else
		return false;
}


bool CX36Editor::ConPinkie()
{
	if(sPinkie.GetCheck()==BST_CHECKED)
        return true;
	else
		return false;
}


void CX36Editor::Visualizar()
{
	CambiarPagina(true,false);
	this->ShowWindow(SW_SHOW);
}


//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/


void CX36Editor::OnTcnSelchangeTab2(NMHDR *pNMHDR, LRESULT *pResult)
{
	CambiarTab3();
	CambiarPagina(true,false);
	*pResult = 0;
}
void CX36Editor::OnTcnSelchangeTab3(NMHDR *pNMHDR, LRESULT *pResult)
{
	CambiarPagina(true,false);
	int a=tab3.GetCurSel();
	*pResult = 0;
}
void CX36Editor::OnBnClickedPinkie()
{
	CambiarPagina(true,false);
}
void CX36Editor::OnBnClickedModos()
{
	CambiarPagina(true,false);
}
void CX36Editor::OnBnClickedGuardar()
{
	GuardarArchivo(true);
}
void CX36Editor::OnBnClickedResumen()
{
	CambiarPagina(false,true);
	CEd_Resumen res(this,(CEd_LComandos*)CCmds,archivoXMP);
	res.DoModal();
}
void CX36Editor::OnBnClickedEditorcomandos()
{
	CambiarPagina(false,true);
	this->ShowWindow(SW_HIDE);
	CCmds->ShowWindow(SW_SHOW);
}

void CX36Editor::OnClose()
{
	if(Salir())	CDialog::OnClose();
}

void CX36Editor::OnSalir()
{
	if(Salir())	this->EndDialog(1);
}

void CX36Editor::OnSobreSobreelx36map()
{
	CDialog sobre(IDD_X36MAP_ACERCADE,this);
	sobre.DoModal();
}

void CX36Editor::OnArchivoGuardarcomo()
{
	INT_PTR res;
	CString xmpactual;
	CFileDialog dlg(FALSE,".xmp",0,OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,"X36Map (*.xmp)|*.xmp||",this);
	res=dlg.DoModal();
	if(res==IDOK) {
		xmpactual=archivoXMP.GetBuffer();
		archivoXMP=dlg.GetPathName();
		GuardarArchivo(true);
		archivoXMP=xmpactual.GetBuffer();
	}
}
