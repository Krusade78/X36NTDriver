// Ed_Resumen.cpp: Dialogo para imprimir/ver la configuración
// Implementación

#include "stdafx.h"
#include "X36Editor.h"
#include "Ed_LComandos.h"
#include "Ed_Resumen.h"

// Constructor de la Clase

CEd_Resumen::CEd_Resumen(CWnd* editor,CEd_LComandos *cmds,CString archivo)
: CDialog(IDD_X36MAP_RESUMEN, NULL)
{
	this->editor=(CX36Editor*)editor;
	this->cmds=cmds;
	archivoXMP=archivo;
	archivoXMP.Delete(archivoXMP.GetLength()-4,4);
}


/*************************************************
*************** Control de Mensajes **************
**************************************************/

void CEd_Resumen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID4Lista1, lista[0]);
	DDX_Control(pDX, ID4Lista2, lista[1]);
	DDX_Control(pDX, ID4Lista3, lista[2]);
}

BEGIN_MESSAGE_MAP(CEd_Resumen, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, ID4Tab, OnTcnSelchangeTab)
	ON_WM_SIZE()
	ON_BN_CLICKED(ID4Imprimir,OnBnClickedImprimir)
END_MESSAGE_MAP()

//-------------------------------------------------//
//-------------------------------------------------//


//--------------> Inicialización


BOOL CEd_Resumen::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(ID4Imprimir))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON3));
	for(UCHAR i=0; i<3;i++) {
		lista[i].SetExtendedStyle(LVS_EX_GRIDLINES |LVS_EX_FULLROWSELECT /*| LVS_EX_FLATSB*/ );
		lista[i].InsertColumn(0,TIP19,0,120);
		lista[i].InsertColumn(1,TIP20,LVCFMT_CENTER,50);
		lista[i].InsertColumn(2,TIP21,0,150);
		lista[i].InsertColumn(3,TIP22,0,150);
		lista[i].InsertColumn(4,TIP23,0,150);
		lista[i].InsertColumn(5,TIP24,0,150);
	}
	if(editor->ConModos()) {
		((CTabCtrl*)GetDlgItem(ID4Tab))->InsertItem(0,BTN2);
		((CTabCtrl*)GetDlgItem(ID4Tab))->InsertItem(1,BTN3);
		((CTabCtrl*)GetDlgItem(ID4Tab))->InsertItem(2,BTN4);
		CargarBotones(0);CargarBotones(1);CargarBotones(2);
		CambiarItems(0);
		CambiarItems(1);
		CambiarItems(2);
		((CTabCtrl*)GetDlgItem(ID4Tab))->SetCurSel(1);
	} else {
		((CTabCtrl*)GetDlgItem(ID4Tab))->InsertItem(0,BTN5);
		CargarBotones(1);
		CambiarItems(1);
	}
	return TRUE;
}

//-------------------------------------------------//


/*************************************************
******************** Métodos *********************
**************************************************/

void CEd_Resumen::CargarBotones(UCHAR l)
{
	lista[l].InsertItem(0,BTN6);
	lista[l].InsertItem(1,BTN7);
	lista[l].InsertItem(2,BTN8);
	lista[l].InsertItem(3,BTN9);
	lista[l].InsertItem(4,BTN10);
	lista[l].InsertItem(5,"Pinkie");
	lista[l].InsertItem(6,BTN12);
	lista[l].InsertItem(7,BTN13);
	lista[l].InsertItem(8,BTN14);
	lista[l].InsertItem(9,BTN15);
	lista[l].InsertItem(10,BTN38);
	lista[l].InsertItem(11,BTN39);
	lista[l].InsertItem(12,BTN40);
	lista[l].InsertItem(13,BTN41);
	lista[l].InsertItem(14,BTN16);
	lista[l].InsertItem(15,BTN17);
	lista[l].InsertItem(16,BTN18);
	lista[l].InsertItem(17,BTN19);
	lista[l].InsertItem(18,BTN42);
	lista[l].InsertItem(19,BTN43);
	lista[l].InsertItem(20,BTN44);
	lista[l].InsertItem(21,BTN45);
	lista[l].InsertItem(22,BTN20);
	lista[l].InsertItem(23,BTN21);
	lista[l].InsertItem(24,BTN22);
	lista[l].InsertItem(25,BTN23);
	lista[l].InsertItem(26,BTN24);
	lista[l].InsertItem(27,BTN25);
	lista[l].InsertItem(28,BTN26);
	lista[l].InsertItem(29,BTN27);
	lista[l].InsertItem(30,BTN28);
	lista[l].InsertItem(31,BTN29);
	lista[l].InsertItem(32,BTN30);
	lista[l].InsertItem(33,BTN31);
	lista[l].InsertItem(34,BTN32);
	lista[l].InsertItem(35,BTN33);
	lista[l].InsertItem(36,BTN36);
	lista[l].InsertItem(37,"----> Pos 1 / Incr.");
	lista[l].InsertItem(38,"----> Pos 2 / Decr.");
	lista[l].InsertItem(39,"----> Pos 3");
	lista[l].InsertItem(40,"----> Pos 4");
	lista[l].InsertItem(41,"----> Pos 5");
	lista[l].InsertItem(42,"----> Pos 6");
	lista[l].InsertItem(43,BTN37);
	lista[l].InsertItem(44,"----> Pos. 1 / Incr.");
	lista[l].InsertItem(45,"----> Pos. 2 / Decr.");
	lista[l].InsertItem(46,"----> Pos. 3");
	lista[l].InsertItem(47,"----> Pos. 4");
	lista[l].InsertItem(48,"----> Pos. 5");
	lista[l].InsertItem(49,"----> Pos. 6");
	lista[l].InsertItem(50,BTN34);
	lista[l].InsertItem(51,"----> Pos. 1 / Incr.");
	lista[l].InsertItem(52,"----> Pos. 2 / Decr.");
	lista[l].InsertItem(53,"----> Pos. 3");
	lista[l].InsertItem(54,"----> Pos. 4");
	lista[l].InsertItem(55,"----> Pos. 5");
	lista[l].InsertItem(56,"----> Pos. 6");
	lista[l].InsertItem(57,"----> Pos. 7");
	lista[l].InsertItem(58,"----> Pos. 8");
	lista[l].InsertItem(59,"----> Pos. 9");
	lista[l].InsertItem(60,"----> Pos. 10");
	lista[l].InsertItem(61,BTN35);
	lista[l].InsertItem(62,"----> Pos. 1 / Incr.");
	lista[l].InsertItem(63,"----> Pos. 2 / Decr.");
	lista[l].InsertItem(64,"----> Pos. 3");
	lista[l].InsertItem(65,"----> Pos. 4");
	lista[l].InsertItem(66,"----> Pos. 5");
	lista[l].InsertItem(67,"----> Pos. 6");
	lista[l].InsertItem(68,"----> Pos. 7");
	lista[l].InsertItem(69,"----> Pos. 8");
	lista[l].InsertItem(70,"----> Pos. 9");
	lista[l].InsertItem(71,"----> Pos. 10");
}

//-------------------------------------------------//

void CEd_Resumen::CambiarItems(UCHAR l)
{
	UCHAR i;

	for(i=0;i<5;i++) CogerDatos(i,l);
	if(editor->ConPinkie()) {
		lista[l].SetItemText(5,2,TIP14);
		lista[l].SetItemText(5,3,TIP15);
	} else {
		CogerDatos(5,l);
	}
	for(i=6;i<24;i++) CogerDatos(i,l);
	if(editor->ConModos()) {
		lista[l].SetItemText(24,2,TIP16);
		lista[l].SetItemText(24,3,TIP17);
		lista[l].SetItemText(25,3,TIP17);
		lista[l].SetItemText(25,2,TIP18);
	} else {
		CogerDatos(24,l);
		CogerDatos(25,l);
	}
	for(i=26;i<36;i++) CogerDatos(i,l);
	if((editor->CogerAcelPed()==0) || (editor->CogerAcelPed()==1))	{
		lista[l].SetItemText(36,1,TIP25);
	} else {
		lista[l].SetItemText(36,1,"No");
		CogerDatosRot(36,l);
	}
	if((editor->CogerAcelPed()==0) || (editor->CogerAcelPed()==2))	{
		lista[l].SetItemText(43,1,TIP25);
	} else {
		lista[l].SetItemText(43,1,"No");
		CogerDatosRot(43,l);
	}
	CogerDatosRot(50,l);
	CogerDatosRot(61,l);
}

//-------------------------------------------------//

void CEd_Resumen::CogerDatos(UCHAR i,UCHAR l)
{
	UCHAR pos=0,modo=2;

	if(editor->ConModos()) {
		if(l==0) modo=0;
		if(l==2) modo=4;
	}
	{
		char nombres[16][4]={"P11","P12","P13","P14","P15","P16","P17","P18",
							"P21","P22","P23","P24","P25","P26","P27","P28"};
		CString nombre;
		char n[3];
		if(editor->CogerPovDx(i)!=0)
		{
			if(editor->CogerPovDx(i)<9)
				nombre.Append(nombres[editor->CogerPovDx(i)-1]);
			else
				nombre.Append(nombres[editor->CogerPovDx(i)-9]);
		}
		if(editor->CogerBtDx(i)!=0) {
			nombre.Append(" B");
			nombre.Append(itoa(editor->CogerBtDx(i),n,10));
		}
		if(!nombre.IsEmpty())
			lista[l].SetItemText(i,1,nombre.GetBuffer());
	}
	{
		CString nombre;
		if(editor->CogerIndice(modo,i)!=0) {
			cmds->CogerNombres(editor->CogerIndice(modo,i)-1,&nombre);
			lista[l].SetItemText(i,2,nombre.GetBuffer());
		}
		if(editor->CogerIndice(modo,i+36)!=0) {
			cmds->CogerNombres(editor->CogerIndice(modo,i+36)-1,&nombre);
			lista[l].SetItemText(i,3,nombre.GetBuffer());
		}
		if((editor->CogerIndice((UCHAR)(modo+1),i)!=0) && editor->ConPinkie()) {
			cmds->CogerNombres(editor->CogerIndice((UCHAR)(modo+1),i)-1,&nombre);
			lista[l].SetItemText(i,4,nombre.GetBuffer());
		}
		if((editor->CogerIndice((UCHAR)(modo+1),i+36)!=0) && editor->ConPinkie()) {
			cmds->CogerNombres(editor->CogerIndice((UCHAR)(modo+1),i+36)-1,&nombre);
			lista[l].SetItemText(i,5,nombre.GetBuffer());
		}
	}
}

//-------------------------------------------------//

void CEd_Resumen::CogerDatosRot(UCHAR i, UCHAR l)
{
	UCHAR pos,modo=2,k,maxpos;
	CString nombre;

	switch(i)
	{
		case 36: pos=72; k=0; maxpos=6; break;
		case 43: pos=78; k=1; maxpos=6; break;
		case 50: pos=84; k=2; maxpos=10; break;
		case 61: pos=94; k=3; maxpos=10;
	}

	if(editor->ConModos()) {
		if(l==0) modo=0;
		if(l==2) modo=4;
	}

	if((editor->CogerGiraT(k,modo)&32)==32) {
		lista[l].SetItemText(i,2,TIP26);
	} else {
		if(editor->CogerGiraT(k,modo)>0) {
			lista[l].SetItemText(i,2,TIP27);
		}
	}
	if((editor->CogerGiraT(k,modo+1)&32)==32) {
		lista[l].SetItemText(i,4,TIP26);
	} else {
		if(editor->CogerGiraT(k,modo+1)>0) {
			lista[l].SetItemText(i,4,TIP27);
		}
	}
	i++;
	for(UCHAR j=pos;j<(pos+maxpos);j++)
	{
		if(editor->CogerIndice(modo,j)!=0) {
			cmds->CogerNombres(editor->CogerIndice(modo,j)-1,&nombre);
			lista[l].SetItemText(i+(j-pos),2,nombre.GetBuffer());
		}
		if((editor->CogerIndice((UCHAR)(modo+1),j)!=0) && editor->ConPinkie()) {
			cmds->CogerNombres(editor->CogerIndice((UCHAR)(modo+1),j)-1,&nombre);
			lista[l].SetItemText(i+(j-pos),4,nombre.GetBuffer());
		}
	}
}

//-------------------------------------------------//

void CEd_Resumen::ImprimirPagina(CDC* cdc,CFont* font1,CFont* font2,CFont* font3,CFont* font4,UCHAR l)
{
	CString texto;

	cdc->StartPage();

	cdc->SetMapMode(MM_LOMETRIC);
	int ox=(2100-cdc->GetWindowExt().cx)/2;

	// Titulo
	cdc->SetTextAlign(TA_CENTER);
	cdc->SelectObject(font1);
	cdc->TextOut(1050-ox,-40,archivoXMP);
	cdc->SelectObject(font2);
	if(editor->ConModos()) {
		switch(l) 
		{
			case 0:
				texto=BTN2;
				cdc->TextOut(1050-ox,-110,texto); break;
			case 1:
				texto=BTN3;
				cdc->TextOut(1050-ox,-110,texto); break;
			case 2:
				texto=BTN4;
				cdc->TextOut(1050-ox,-110,texto);
		}
	} else {
		texto=BTN5;
		cdc->TextOut(1050-ox,-110,texto);
	}

	// Tabla
	ox=(100-(2100-cdc->GetWindowExt().cx))/2;
	cdc->SelectObject(font3);

	texto=TIP19;
	cdc->TextOut(150+ox,-185,texto);
	texto=TIP20;
	cdc->TextOut(350+ox,-185,texto);
	texto=TIP21;
	cdc->TextOut(600+ox,-185,texto);
	texto=TIP22;
	cdc->TextOut(1000+ox,-185,texto);
	texto=TIP23;
	cdc->TextOut(1400+ox,-185,texto);
	texto=TIP24;
	cdc->TextOut(1800+ox,-185,texto);
	cdc->SetTextAlign(TA_NOUPDATECP);

	cdc->MoveTo(ox,-228);
	cdc->LineTo(ox+2000,-228);
	cdc->MoveTo(ox,-228);
	cdc->LineTo(ox,-2752);
	cdc->MoveTo(300+ox,-232);
	cdc->LineTo(300+ox,-2752);
	cdc->MoveTo(400+ox,-232);
	cdc->LineTo(400+ox,-2752);
	cdc->MoveTo(800+ox,-232);
	cdc->LineTo(800+ox,-2752);
	cdc->MoveTo(1200+ox,-232);
	cdc->LineTo(1200+ox,-2752);
	cdc->MoveTo(1600+ox,-232);
	cdc->LineTo(1600+ox,-2752);
	cdc->MoveTo(2000+ox,-232);
	cdc->LineTo(2000+ox,-2752);

	cdc->SelectObject(font4);
	for(UCHAR i=0;i<lista[l].GetItemCount();i++)
	{
		texto=lista[l].GetItemText(i,0);
		cdc->TextOut(30+ox,(i*-35)-236,texto);
		cdc->SetTextAlign(TA_CENTER);
		texto=lista[l].GetItemText(i,1);
		cdc->TextOut(350+ox,(i*-35)-236,texto);
		cdc->SetTextAlign(TA_NOUPDATECP);
		texto=lista[l].GetItemText(i,2);
		cdc->TextOut(420+ox,(i*-35)-236,texto);
		texto=lista[l].GetItemText(i,3);
		cdc->TextOut(820+ox,(i*-35)-236,texto);
		texto=lista[l].GetItemText(i,4);
		cdc->TextOut(1220+ox,(i*-35)-236,texto);
		texto=lista[l].GetItemText(i,5);
		cdc->TextOut(1620+ox,(i*-35)-236,texto);
		cdc->MoveTo(ox,(i*-35)-232);
		cdc->LineTo(ox+2000,(i*-35)-232);
	}
	cdc->MoveTo(ox,-2520-232);
	cdc->LineTo(ox+2000,-2520-232);

	cdc->EndPage();
}

//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/

void CEd_Resumen::OnSize(UINT nType, int cx, int cy)
{
	RECT dim;
	CDialog::OnSize(nType, cx, cy);
	if(lista[1]) {
		lista[0].GetWindowRect(&dim);
		lista[0].MoveWindow(11,40,cx-22,cy-51);
		lista[1].GetWindowRect(&dim);
		lista[1].MoveWindow(11,40,cx-22,cy-51);
		lista[2].GetWindowRect(&dim);
		lista[2].MoveWindow(11,40,cx-22,cy-51);
	}
}

void CEd_Resumen::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	lista[0].ShowWindow(SW_HIDE);
	lista[1].ShowWindow(SW_HIDE);
	lista[2].ShowWindow(SW_HIDE);
	lista[((CTabCtrl*)GetDlgItem(ID4Tab))->GetCurSel()].ShowWindow(SW_SHOW);
	*pResult = 0;
}

void CEd_Resumen::OnBnClickedImprimir()
{
	CPrintDialog imp(FALSE,PD_ALLPAGES|PD_NOWARNING|PD_HIDEPRINTTOFILE|PD_USEDEVMODECOPIES);
	if(imp.DoModal()==IDOK){
		HDC hdc=imp.GetPrinterDC();
		CDC* cdc=CDC::FromHandle(hdc);
		DOCINFO doc;
			doc.cbSize=sizeof(DOCINFO);
			doc.lpszDocName="Documento de X36Map\0";
			doc.lpszOutput=NULL;
			doc.lpszDatatype="raw";
			doc.fwType=0;
		CFont font1,font2,font3,font4;
		font1.CreatePointFont(280,"Times New Roman",cdc);
		font2.CreatePointFont(160,"Times New Roman",cdc);
		font3.CreateFont(
			40,0,0,0,
			FW_BOLD,FALSE,FALSE,0,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS,
			"Times New Roman");
		font4.CreatePointFont(100,"Times New Roman",cdc);

		cdc->StartDoc(&doc);
		
		for(int i=0;i<imp.GetCopies();i++)
		{
			if(imp.PrintSelection()) {
				if(editor->ConModos()) {
					ImprimirPagina(cdc,&font1,&font2,&font3,&font4,(UCHAR)((CTabCtrl*)GetDlgItem(ID4Tab))->GetCurSel());
				} else {
					ImprimirPagina(cdc,&font1,&font2,&font3,&font4,1);
				}
			} else {
				if(editor->ConModos()) {
					ImprimirPagina(cdc,&font1,&font2,&font3,&font4,0);
					ImprimirPagina(cdc,&font1,&font2,&font3,&font4,1);
					ImprimirPagina(cdc,&font1,&font2,&font3,&font4,2);
				} else {
					ImprimirPagina(cdc,&font1,&font2,&font3,&font4,1);
				}
			}
		}
		cdc->EndDoc();
		cdc->DeleteDC();
	}
}
