// X36Import.cpp: Importar configuraciones antiguas
// Implementación

#include "stdafx.h"
#include "x36import.h"

// Constructor y destructor de la Clase

CX36Import::CX36Import(CWnd* padre)
{
	ZeroMemory(giraT,24);
	ZeroMemory(indices,2*6*104);
	ZeroMemory(autoRepeat,sizeof(bool)*6*36);
	lista = new CListBox();
	lista->Create(LBS_STANDARD|WS_CHILD,CRect(0,0,0,0),padre,1);
	Iniciar();
}

CX36Import::~CX36Import()
{
	if(!bufferpos.IsEmpty()) bufferpos.RemoveAll();
	delete lista;
	lista=NULL;
}


/*************************************************
******************** Métodos *********************
**************************************************/


void CX36Import::Iniciar()
{
	TCHAR* dir=NULL;
	INT_PTR res;
	DWORD lg=GetCurrentDirectory(0,dir);

	dir=new TCHAR[lg];
	GetCurrentDirectory(lg,dir);

	CFileDialog dlg(TRUE,".x36",0,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"Saitek X36 (*.x36)|*.x36|X36Map (*.xmp)|*.xmp||");
	res=dlg.DoModal();
	SetCurrentDirectory(dir);
	if(res==IDOK) {
		Importar(dlg.GetPathName(),dlg.GetPathName().Left(dlg.GetPathName().GetLength()-dlg.GetFileName().GetLength()));
	}
	delete[] dir;
	dir=NULL;
}

//-------------------------------------------------//

void CX36Import::Importar(CString ruta,CString path)
{
	CString nombre;
	if(ruta.Right(3).CompareNoCase("x36")==0)
	{
		bool ok=ImportarCMM(ruta,path,&nombre);
		if(ok) ImportarX36(ruta,nombre);
	}
	ConvertirXMP(ruta);
}

//-------------------------------------------------//

bool CX36Import::ImportarCMM(CString ruta,CString path,CString* nombre)
{
	char buffer[202];
	UINT16 bufferaux;
	UINT16 pos;
	int posaux;
	CFile f,g;
	CStdioFile x36;
	CList <UINT64,UINT64&> teclas;
	UINT64 teclasaux;
	CList <UCHAR,UCHAR&> teclasExt;
	UCHAR teclasExtaux;
	bool raton=false;

	ZeroMemory(buffer,202);

	// Abrir X36 y leer nombre del fichero CMM asociado

	if(x36.Open(ruta.GetBuffer(),CFile::modeRead)!=0)
	{
		if(x36.Read(buffer,14)==14) {
			nombre->Append(buffer);
			nombre->Delete(0,2);
		} else {
			f.Close();
			MessageBox(NULL,MSG7,"Error",MB_ICONERROR);
			return false;
		}
		ZeroMemory(buffer,14);
		x36.SeekToBegin();
		for(UCHAR i=0;i<0x1b;i++) x36.ReadString(buffer,202);
		x36.ReadString(buffer,9);
		if(buffer[7]=='2') raton=true;
		x36.Close();
	} else {
		MessageBox(NULL,MSG17,"Error",MB_ICONERROR);
		return false;
	}
	path.Append(nombre->GetBuffer());
	nombre->Empty();

	// Abrir CMM

	if(f.Open(path.GetBuffer(),CFile::typeBinary|CFile::modeRead)==0)
	{
		MessageBox(NULL,MSG18,"Error",MB_ICONERROR);
		return false;
	}

	// Crear archivo XMP con el nombre del CMM (nombre del juego)

	ZeroMemory(buffer,202);
	if(f.Read(buffer,32)!=32) {
			f.Close();
			MessageBox(NULL,MSG7,"Error",MB_ICONERROR);
			return false;
	}
	nombre->Append(buffer);
	nombre->Append(".xmp");

	if(g.Open(nombre->GetBuffer(),CFile::typeBinary|CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive)==0) {
		MessageBox(NULL,MSG9,"Error",MB_ICONERROR);
		f.Close();
		return false;
	}
	g.SeekToBegin();

	// Leer/Escribir datos	

	f.Seek(30,CFile::current);
	pos=(UINT16)((f.GetLength()-0x3e)/0xe0);
	if(raton) pos+=5;
	g.Write(&pos,2);
	if(raton) pos-=5;
	for(UINT16 i=0;i<pos;i++) {
		if(f.Read(&bufferaux,2)!=2) {
			f.Close();
			g.Close();
			CFile::Remove(nombre->GetBuffer());
			MessageBox(NULL,MSG9,"Error",MB_ICONERROR);
			return false;
		}

		ZeroMemory(buffer,32);
		if(f.Read(buffer,20)!=20){
			f.Close();
			g.Close();
			CFile::Remove(nombre->GetBuffer());
			MessageBox(NULL,MSG9,"Error",MB_ICONERROR);
			return false;
		}
		posaux=lista->AddString(buffer);

		ZeroMemory(buffer,202);
		if(f.Read(buffer,202)!=202) {
			f.Close();
			g.Close();
			CFile::Remove(nombre->GetBuffer());
			MessageBox(NULL,MSG9,"Error",MB_ICONERROR);
			return false;
		}
		ConvertirTeclas(buffer,&teclasaux,&teclasExtaux);
		if(posaux==0) {
			teclas.AddHead(teclasaux);
			teclasExt.AddHead(teclasExtaux);
			bufferpos.AddHead(bufferaux);
		} else {
			teclas.InsertAfter(teclas.FindIndex(posaux-1),teclasaux);
			teclasExt.InsertAfter(teclasExt.FindIndex(posaux-1),teclasExtaux);
			bufferpos.InsertAfter(bufferpos.FindIndex(posaux-1),bufferaux);
		}
	}
	// Ratón
	if(raton) {
	UINT16 k; UCHAR cero=0; UINT64 cero64=0;
	pos+=5;
	for(UCHAR i=0;i<5;i++) 
	{
		switch(i)
		{
			case 0:
				posaux=lista->AddString(TIP28);
//				teclasaux=4;
				break;
			case 1:
				posaux=lista->AddString(TIP29);
//				teclasaux=5;
				break;
			case 2:
				posaux=lista->AddString(TIP30);
//				teclasaux=6;
				break;
			case 3:
				posaux=lista->AddString(TIP31);
//				teclasaux=7;
				break;
			case 4:
				posaux=lista->AddString(TIP32);
//				teclasaux=1;
		}
		k=0xFFFF-i;
		if(posaux==0) {
			teclas.AddHead(cero64);
			teclasExt.AddHead(cero);
			bufferpos.AddHead(k);
		} else {
			teclas.InsertAfter(teclas.FindIndex(posaux-1),cero64);
			teclasExt.InsertAfter(teclasExt.FindIndex(posaux-1),cero);
			bufferpos.InsertAfter(bufferpos.FindIndex(posaux-1),k);
		}
	}
	}
	f.Close();

	for(UINT16 i=0;i<pos;i++) {
		ZeroMemory(buffer,32);
		lista->GetText(i,buffer);
		g.Write(buffer,32);
		teclasaux=teclas.GetAt(teclas.FindIndex(i));
		teclasExtaux=teclasExt.GetAt(teclasExt.FindIndex(i));
		g.Write(&teclasaux,8);
		g.Write(&teclasExtaux,1);
	}
	g.Close();

	teclas.RemoveAll();
	teclasExt.RemoveAll();

	return true;
}

//-------------------------------------------------//

void CX36Import::ImportarX36(CString ruta,CString nombre)
{
	UINT16 botones[18];
	UINT16 giros[64];
	UCHAR puerto[11];
	CFile g;
	CStdioFile f;
	CString linea;
	UCHAR sel;
	bool modos,pinkie;

	// Abrir X36 

	if(f.Open(ruta.GetBuffer(),CFile::modeRead)==0)
	{
		MessageBox(NULL,MSG17,"Error",MB_ICONERROR);
		return;
	}

	// Abrir archivo XMP 

	if(g.Open(nombre.GetBuffer(),CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive)==0) {
		f.Close();
		MessageBox(NULL,MSG1,"Error",MB_ICONWARNING);
		return;
	}

	// Leer/Escribir datos	

try {
	f.ReadString(linea);
	for(UCHAR i=0;i<0x15;i++)
	{
		f.ReadString(linea);
		if(i<0x10) {
			if(linea.GetAt(8)=='1') {
				SacarBytes(botones,linea.Right(linea.GetLength()-16));
				if(linea.GetAt(10)!='0') {
					GuardaBoton(botones,i,true);
				} else {
					GuardaBoton(botones,i,false);
				}
			}
		} else {
			if(linea.GetAt(9)=='1') {
				SacarBytes(botones,linea.Right(linea.GetLength()-17));
				if(linea.GetAt(11)!='0') {
					GuardaBoton(botones,i,true);
				} else {
					GuardaBoton(botones,i,false);
				}
			}
		}
	}
	f.ReadString(linea);
	if(linea.GetAt(7)=='1') {
		pinkie=true;
	} else {
		pinkie=false;
		if(linea.GetAt(9)=='1') {
			SacarBytes(botones,linea.Right(linea.GetLength()-17));
			if(linea.GetAt(11)!='0') {
				GuardaBoton(botones,0x15,true);
			} else {
				GuardaBoton(botones,0x15,false);
			}
		}
	}
	{
	bool raton=false;
	for(UCHAR i=0x16;i<0x23;i++)
	{
		f.ReadString(linea);
		if((linea.GetAt(7)=='2') || (raton && (i==0x1e))) {
			raton=true;
			UINT16 k;
			ZeroMemory(botones,18*2);
			for(UINT16 j=0;j<(bufferpos.GetCount());j++) {
				switch(i)
				{
					case 0x1a:
						if(bufferpos.GetAt(bufferpos.FindIndex(j))==0xffff) {
							k=j+1;
							j=(UINT16)bufferpos.GetCount();
						}
						break;
					case 0x1c:
						if(bufferpos.GetAt(bufferpos.FindIndex(j))==0xfffe) {
							k=j+1;
							j=(UINT16)bufferpos.GetCount();
						}
						break;
					case 0x1d:
						if(bufferpos.GetAt(bufferpos.FindIndex(j))==0xfffd) {
							k=j+1;
							j=(UINT16)bufferpos.GetCount();
						}
						break;
					case 0x1b:
						if(bufferpos.GetAt(bufferpos.FindIndex(j))==0xfffc) {
							k=j+1;
							j=(UINT16)bufferpos.GetCount();
						}
						break;
					case 0x1e:
						if(bufferpos.GetAt(bufferpos.FindIndex(j))==0xfffb) {
							k=j+1;
							j=(UINT16)bufferpos.GetCount();
						}
				}
			}
			botones[0]=k; botones[2]=k;
			botones[6]=k; botones[8]=k;
			botones[12]=k; botones[14]=k;
			GuardaBoton(botones,i,true);
		} else {
			if(linea.GetAt(9)=='1') {
				SacarBytes(botones,linea.Right(linea.GetLength()-17));
				if(linea.GetAt(11)!='0') {
					GuardaBoton(botones,i,true);
				} else {
					GuardaBoton(botones,i,false);
				}
			}
		}
	}
	}
	f.ReadString(linea);
	if(linea.GetAt(7)=='3') {
		modos=true;
	} else {
		modos=false;
		if(linea.GetAt(9)=='1') {
			SacarBytes(botones,linea.Right(linea.GetLength()-17));
			if(linea.GetAt(11)!='0') {
				GuardaBoton(botones,0x23,true);
			} else {
				GuardaBoton(botones,0x23,false);
			}
		}
	}
	for(UCHAR i=0;i<4;i++)
	{
		f.ReadString(linea);
		if((linea.GetAt(9)=='5') || (linea.GetAt(9)=='6') || (linea.GetAt(9)=='7')) {
			SacarBytes(giros,linea.Right(linea.GetLength()-15));
			if(linea.GetAt(9)!='5') {
				GuardaGiro(giros,i,true);
			} else {
				GuardaGiro(giros,i,false);
			}
		}
	}
	f.ReadString(linea);
	SacarBytesPuerto(puerto,linea.Right(linea.GetLength()-7));
	f.Close();

	g.SeekToEnd();
	g.Write(puerto,9);
	sel=(modos*128)+(pinkie*64);
	g.Write(&sel,1);
	g.Write(giraT,24);
	g.Write(indices,sizeof(UINT16)*6*104);
	g.Write(autoRepeat,36*6);
	g.Close();
} catch(CFileException* e) {
	f.Close();
	g.Close();
	CFile::Remove(nombre.GetBuffer());
	MessageBox(NULL,MSG19,"Error",MB_ICONERROR);
}
}

//-------------------------------------------------//

void CX36Import::SacarBytes(UINT16 datos[],CString cadena)
{
	char numero[4]={0,0,0,0};
	UCHAR j=0,k=0;
	for(UCHAR i=0;i<cadena.GetLength();i++)
	{
		if(cadena.GetAt(i)==',') {
			datos[k]=ConvertirHex(numero);
			if(datos[k]>0) {
				for(UINT16 l=0;l<(bufferpos.GetCount());l++) {
					if(bufferpos.GetAt(bufferpos.FindIndex(l))==datos[k]) {
						datos[k]=l+1;
						break;
					}
				}
			}
			k++;
			j=0;
			ZeroMemory(numero,4);
		} else {
			numero[j]=cadena.GetAt(i);
			j++;
		}
	}
}

//-------------------------------------------------//

void CX36Import::SacarBytesPuerto(UCHAR datos[],CString cadena)
{
	char numero[4]={0,0,0,0};
	UCHAR j=0,k=0;
	for(UCHAR i=0;i<cadena.GetLength();i++)
	{
		if(cadena.GetAt(i)==',') {
			datos[k]=(UCHAR)ConvertirHex(numero);
			k++;
			j=0;
			ZeroMemory(numero,4);
		} else {
			numero[j]=cadena.GetAt(i);
			j++;
			if((i+1)==cadena.GetLength()) {
				datos[k]=(UCHAR)atoi(numero);
			}
		}
	}
}

//-------------------------------------------------//

UINT16 CX36Import::ConvertirHex(char numero[])
{
	UINT32 k=1;
	UINT16 n=0;
	for(UCHAR i=5;i>0;i--){
		if(numero[i-1]!=0) {
			switch(numero[i-1])
			{
				case '0' : n+=0*k; k*=16; break;
				case '1' : n+=1*k; k*=16; break;
				case '2' : n+=2*k; k*=16; break;
				case '3' : n+=3*k; k*=16; break;
				case '4' : n+=4*k; k*=16; break;
				case '5' : n+=5*k; k*=16; break;
				case '6' : n+=6*k; k*=16; break;
				case '7' : n+=7*k; k*=16; break;
				case '8' : n+=8*k; k*=16; break;
				case '9' : n+=9*k; k*=16; break;
				case 'A' : n+=10*k; k*=16; break;
				case 'B' : n+=11*k; k*=16; break;
				case 'C' : n+=12*k; k*=16; break;
				case 'D' : n+=13*k; k*=16; break;
				case 'E' : n+=14*k; k*=16; break;
				case 'F' : n+=15*k; k*=16;
			}
		}
	}
	return n;
}

//-------------------------------------------------//

void CX36Import::ConvertirTeclas(char datos[],UINT64* teclas,UCHAR* teclasExt)
{
	char buffer[16];
	char j=0,k=0;
	CString nombre;

	ZeroMemory(buffer,16);
	ZeroMemory(teclas,8);
	*teclasExt=0;

	for(UCHAR i=0;i<202;i++)
	{
		if(datos[i]==0x20) {
			nombre.Empty();
			nombre.Append(buffer);
			*(((UCHAR*)teclas)+j)=SacarCodigo(nombre,teclasExt,j);
			j++;
			i+=2;
			k=0;
			ZeroMemory(buffer,16);
		} else {
			if(datos[i]!=0) {
				buffer[k]=datos[i];
				k++;
			}
		}
	}
}

//-------------------------------------------------//

void CX36Import::GuardaBoton(UINT16 bytes[],UCHAR pos,bool rep)
{
	UCHAR i;
	switch(pos)
	{
		case 1: i=4; break;
		case 4: i=1; break;
		case 5: i=6; break;
		case 6: i=11; break;
		case 7: i=9; break;
		case 8: i=13; break;
		case 9: i=7; break;
		case 10: i=12; break;
		case 11: i=8; break;
		case 12: i=10; break;
		case 13: i=14; break;
		case 14: i=19; break;
		case 15: i=17; break;
		case 16: i=21; break;
		case 17: i=15; break;
		case 18: i=20; break;
		case 19: i=16; break;
		case 20: i=18; break;
		case 21: i=5; break;
		case 22: i=32; break;
		case 23: i=35; break;
		case 24: i=33; break;
		case 25: i=34; break;
		case 26: i=28; break;
		case 27: i=31; break;
		case 28: i=29; break;
		case 29: i=30; break;
		case 30: i=23; break;
		case 31: i=22; break;
		case 32: i=27; break;
		case 33: i=26; break;
		case 34: i=25; break;
		case 35: i=24; break;
		default: i=pos;
	}

	indices[0][i]=bytes[0];
	if(!rep) indices[0][i+36]=bytes[1];
	autoRepeat[0][i]=rep;
	indices[1][i]=bytes[2];
	if(!rep) indices[1][i+36]=bytes[3];
	autoRepeat[1][i]=rep;
	indices[2][i]=bytes[6];
	if(!rep) indices[2][i+36]=bytes[7];
	autoRepeat[2][i]=rep;
	indices[3][i]=bytes[8];
	if(!rep) indices[3][i+36]=bytes[9];
	autoRepeat[3][i]=rep;
	indices[4][i]=bytes[12];
	if(!rep) indices[4][i+36]=bytes[13];
	autoRepeat[4][i]=rep;
	indices[5][i]=bytes[14];
	if(!rep) indices[5][i+36]=bytes[15];
	autoRepeat[5][i]=rep;
}

//-------------------------------------------------//

void CX36Import::GuardaGiro(UINT16 bytes[],UCHAR pos,bool posic)
{
	UCHAR i;
	switch(pos)
	{
		case 0: i=72; break;
		case 1: i=78; break;
		case 2: i=84; break;
		case 3: i=94;
	}
	if(!posic) {
		indices[0][i]=bytes[0];
		indices[0][i+1]=bytes[1];
		ZeroMemory(&indices[0][i+2],8);
		giraT[pos][0]=32;
		indices[1][i]=bytes[32];
		indices[1][i+1]=bytes[33];
		ZeroMemory(&indices[1][i+2],8);
		giraT[pos][1]=32;
		indices[2][i]=bytes[11];
		indices[2][i+1]=bytes[12];
		ZeroMemory(&indices[2][i+2],8);
		giraT[pos][2]=32;
		indices[3][i]=bytes[42];
		indices[3][i+1]=bytes[43];
		ZeroMemory(&indices[3][i+2],8);
		giraT[pos][3]=32;
		indices[4][i]=bytes[22];
		indices[4][i+1]=bytes[23];
		ZeroMemory(&indices[4][i+2],8);
		giraT[pos][4]=32;
		indices[5][i]=bytes[52];
		indices[5][i+1]=bytes[53];
		ZeroMemory(&indices[5][i+2],8);
		giraT[pos][5]=32;
	} else {
		UCHAR j,k;
		if(pos<2) k=6; else k=10;
		for(j=0;j<k;j++) {
			indices[0][i+j]=bytes[j];
			if((bytes[j]==0) && (giraT[pos][0]==0)) giraT[pos][0]=j;
			if((bytes[j]!=0)&&(j==(k-1))) giraT[pos][0]=j+1;
		}
		for(j=0;j<k;j++) {
			indices[1][i+j]=bytes[32+j];
			if((bytes[32+j]==0) && (giraT[pos][1]==0)) giraT[pos][1]=j;
			if((bytes[32+j]!=0)&&(j==(k-1))) giraT[pos][1]=j+1;
		}
		for(j=0;j<k;j++) {
			indices[2][i+j]=bytes[11+j];
			if((bytes[11+j]==0) && (giraT[pos][2]==0)) giraT[pos][2]=j;
			if((bytes[11+j]!=0)&&(j==(k-1))) giraT[pos][2]=j+1;
		}
		for(j=0;j<k;j++) {
			indices[3][i+j]=bytes[42+j];
			if((bytes[42+j]==0) && (giraT[pos][3]==0)) giraT[pos][3]=j;
			if((bytes[42+j]!=0)&&(j==(k-1))) giraT[pos][3]=j+1;
		}
		for(j=0;j<k;j++) {
			indices[4][i+j]=bytes[22+j];
			if((bytes[22+j]==0) && (giraT[pos][4]==0)) giraT[pos][4]=j;
			if((bytes[22+j]!=0)&&(j==(k-1))) giraT[pos][4]=j+1;
		}
		for(j=0;j<k;j++) {
			indices[5][i+j]=bytes[52+j];
			if((bytes[52+j]==0) && (giraT[pos][5]==0)) giraT[pos][5]=j;
			if((bytes[52+j]!=0)&&(j==(k-1))) giraT[pos][5]=j+1;
		}
	}
}

//-------------------------------------------------//

UCHAR CX36Import::SacarCodigo(CString nombre,UCHAR* teclaExt,UCHAR i)
{
	bool ext=false;
	UCHAR codigo=0;
	if(nombre.Compare("ESC")==0) codigo=(UCHAR)VK_ESCAPE;/*MapVirtualKey(VK_ESCAPE,0);*/
	if(nombre.Compare("F1")==0) codigo=(UCHAR)VK_F1;/*MapVirtualKey(VK_F1,0);*/
	if(nombre.Compare("F2")==0) codigo=(UCHAR)VK_F2;/*MapVirtualKey(VK_F2,0);*/
	if(nombre.Compare("F3")==0) codigo=(UCHAR)VK_F3;/*MapVirtualKey(VK_F3,0);*/
	if(nombre.Compare("F4")==0) codigo=(UCHAR)VK_F4;/*MapVirtualKey(VK_F4,0);*/
	if(nombre.Compare("F5")==0) codigo=(UCHAR)VK_F5;/*MapVirtualKey(VK_F5,0);*/
	if(nombre.Compare("F6")==0) codigo=(UCHAR)VK_F6;/*MapVirtualKey(VK_F6,0);*/
	if(nombre.Compare("F7")==0) codigo=(UCHAR)VK_F7;/*MapVirtualKey(VK_F7,0);*/
	if(nombre.Compare("F8")==0) codigo=(UCHAR)VK_F8;/*MapVirtualKey(VK_F8,0);*/
	if(nombre.Compare("F9")==0) codigo=(UCHAR)VK_F9;/*MapVirtualKey(VK_F9,0);*/
	if(nombre.Compare("F10")==0) codigo=(UCHAR)VK_F10;/*MapVirtualKey(VK_F10,0);*/
	if(nombre.Compare("F11")==0) codigo=(UCHAR)VK_F11;/*MapVirtualKey(VK_F11,0);*/
	if(nombre.Compare("F12")==0) codigo=(UCHAR)VK_F12;/*MapVirtualKey(VK_F12,0);*/
	if(nombre.Compare("SNAPSHOT")==0) {/*ext=true;*/codigo=(UCHAR)0;/*VK_SNAPSHOT;}MapVirtualKey(VK_SNAPSHOT,0);*/}
	if(nombre.Compare("SCROLL")==0) codigo=(UCHAR)VK_SCROLL;/*MapVirtualKey(VK_SCROLL,0);*/
	if(nombre.Compare("PAUSE")==0) {/*ext=true;*/codigo=(UCHAR)0;/*VK_PAUSE;}MapVirtualKey(VK_PAUSE,0);*/}
	if(nombre.Compare("`")==0) codigo=(UCHAR)VK_OEM_5;/*MapVirtualKey(VK_OEM_5,0);*/
	if(nombre.Compare("1")==0) codigo=(UCHAR)0x31;/*MapVirtualKey(0x31,0);*/
	if(nombre.Compare("2")==0) codigo=(UCHAR)0x32;/*MapVirtualKey(0x32,0);*/
	if(nombre.Compare("3")==0) codigo=(UCHAR)0x33;/*MapVirtualKey(0x33,0);*/
	if(nombre.Compare("4")==0) codigo=(UCHAR)0x34;/*MapVirtualKey(0x34,0);*/
	if(nombre.Compare("5")==0) codigo=(UCHAR)0x35;/*MapVirtualKey(0x35,0);*/
	if(nombre.Compare("6")==0) codigo=(UCHAR)0x36;/*MapVirtualKey(0x36,0);*/
	if(nombre.Compare("7")==0) codigo=(UCHAR)0x37;/*MapVirtualKey(0x37,0);*/
	if(nombre.Compare("8")==0) codigo=(UCHAR)0x38;/*MapVirtualKey(0x38,0);*/
	if(nombre.Compare("9")==0) codigo=(UCHAR)0x39;/*MapVirtualKey(0x39,0);*/
	if(nombre.Compare("0")==0) codigo=(UCHAR)0x30;/*MapVirtualKey(0x30,0);*/
	if(nombre.Compare("-")==0) codigo=(UCHAR)VK_OEM_4;/*MapVirtualKey(VK_OEM_4,0);*/
	if(nombre.Compare("=")==0) codigo=(UCHAR)VK_OEM_6;/*MapVirtualKey(VK_OEM_6,0);*/
	if(nombre.Compare("#")==0) codigo=(UCHAR)VK_OEM_2;/*MapVirtualKey(VK_OEM_2,0);*/
	if(nombre.Compare("BACK")==0) codigo=(UCHAR)VK_BACK;/*MapVirtualKey(VK_BACK,0);*/
	if(nombre.Compare("TAB")==0) codigo=(UCHAR)VK_TAB;/*MapVirtualKey(VK_TAB,0);*/
	if(nombre.Compare("Q")==0) codigo=(UCHAR)0x51;/*MapVirtualKey(0x51,0);*/
	if(nombre.Compare("W")==0) codigo=(UCHAR)0x57;/*MapVirtualKey(0x57,0);*/
	if(nombre.Compare("E")==0) codigo=(UCHAR)0x45;/*MapVirtualKey(0x45,0);*/
	if(nombre.Compare("R")==0) codigo=(UCHAR)0x52;/*MapVirtualKey(0x52,0);*/
	if(nombre.Compare("T")==0) codigo=(UCHAR)0x54;/*MapVirtualKey(0x54,0);*/
	if(nombre.Compare("Y")==0) codigo=(UCHAR)0x59;/*MapVirtualKey(0x59,0);*/
	if(nombre.Compare("U")==0) codigo=(UCHAR)0x55;/*MapVirtualKey(0x55,0);*/
	if(nombre.Compare("I")==0) codigo=(UCHAR)0x49;/*MapVirtualKey(0x49,0);*/
	if(nombre.Compare("O")==0) codigo=(UCHAR)0x4f;/*MapVirtualKey(0x4f,0);*/
	if(nombre.Compare("P")==0) codigo=(UCHAR)0x50;/*MapVirtualKey(0x50,0);*/
	if(nombre.Compare("[")==0) codigo=(UCHAR)VK_OEM_1;/*MapVirtualKey(VK_OEM_1,0);*/
	if(nombre.Compare("]")==0) codigo=(UCHAR)VK_OEM_PLUS;/*MapVirtualKey(VK_OEM_PLUS,0);*/
	if(nombre.Compare("RTN")==0) codigo=(UCHAR)VK_RETURN;/*MapVirtualKey(VK_RETURN,0);*/
	if(nombre.Compare("CAPS")==0) codigo=(UCHAR)VK_CAPITAL;/*MapVirtualKey(VK_CAPITAL,0);*/
	if(nombre.Compare("A")==0) codigo=(UCHAR)0x41;/*MapVirtualKey(0x41,0);*/
	if(nombre.Compare("S")==0) codigo=(UCHAR)0x53;/*MapVirtualKey(0x53,0);*/
	if(nombre.Compare("D")==0) codigo=(UCHAR)0x44;/*MapVirtualKey(0x44,0);*/
	if(nombre.Compare("F")==0) codigo=(UCHAR)0x46;/*MapVirtualKey(0x46,0);*/
	if(nombre.Compare("G")==0) codigo=(UCHAR)0x47;/*MapVirtualKey(0x47,0);*/
	if(nombre.Compare("H")==0) codigo=(UCHAR)0x48;/*MapVirtualKey(0x48,0);*/
	if(nombre.Compare("J")==0) codigo=(UCHAR)0x4a;/*MapVirtualKey(0x4a,0);*/
	if(nombre.Compare("K")==0) codigo=(UCHAR)0x4b;/*MapVirtualKey(0x4b,0);*/
	if(nombre.Compare("L")==0) codigo=(UCHAR)0x4c;/*MapVirtualKey(0x4c,0);*/
	if(nombre.Compare(";")==0) codigo=(UCHAR)VK_OEM_3;/*MapVirtualKey(VK_OEM_3,0);*/
	if(nombre.Compare("'")==0) codigo=(UCHAR)VK_OEM_7;/*MapVirtualKey(VK_OEM_7,0);*/
	if(nombre.Compare("SHIFT")==0) codigo=(UCHAR)VK_SHIFT;/*MapVirtualKey(VK_SHIFT,0);*/
	if(nombre.Compare("\\")==0) codigo=(UCHAR)VK_OEM_102;/*MapVirtualKey(VK_OEM_102,0);*/
	if(nombre.Compare("Z")==0) codigo=(UCHAR)0x5a;/*MapVirtualKey(0x5a,0);*/
	if(nombre.Compare("X")==0) codigo=(UCHAR)0x58;/*MapVirtualKey(0x58,0);*/
	if(nombre.Compare("C")==0) codigo=(UCHAR)0x43;/*MapVirtualKey(0x43,0);*/
	if(nombre.Compare("V")==0) codigo=(UCHAR)0x56;/*MapVirtualKey(0x56,0);*/
	if(nombre.Compare("B")==0) codigo=(UCHAR)0x42;/*MapVirtualKey(0x42,0);*/
	if(nombre.Compare("N")==0) codigo=(UCHAR)0x4e;/*MapVirtualKey(0x4e,0);*/
	if(nombre.Compare("M")==0) codigo=(UCHAR)0x4d;/*MapVirtualKey(0x4d,0);*/
	if(nombre.Compare(",")==0) codigo=(UCHAR)VK_OEM_COMMA;/*MapVirtualKey(VK_OEM_COMMA,0);*/
	if(nombre.Compare(".")==0) codigo=(UCHAR)VK_OEM_PERIOD;/*MapVirtualKey(VK_OEM_PERIOD,0);*/
	if(nombre.Compare("/")==0) codigo=(UCHAR)VK_OEM_MINUS;/*MapVirtualKey(VK_OEM_MINUS,0);*/
	if(nombre.Compare("CTRL")==0) codigo=(UCHAR)VK_CONTROL;/*MapVirtualKey(VK_CONTROL,0);*/
	if(nombre.Compare("ALT")==0) codigo=(UCHAR)VK_MENU;/*MapVirtualKey(VK_MENU,0);*/
	if(nombre.Compare("SPACE")==0) codigo=(UCHAR)VK_SPACE;/*MapVirtualKey(VK_SPACE,0);*/
	if(nombre.Compare("GREY-DELETE")==0) {ext=true;codigo=(UCHAR)VK_DELETE;/*MapVirtualKey(VK_DELETE,0);*/}
	if(nombre.Compare("GREY-END")==0) {ext=true;codigo=(UCHAR)VK_END;/*MapVirtualKey(VK_END,0);*/}
	if(nombre.Compare("GREY-PGDN")==0) {ext=true;codigo=(UCHAR)VK_NEXT;/*MapVirtualKey(VK_NEXT,0);*/}
	if(nombre.Compare("GREY-INSERT")==0) {ext=true;codigo=(UCHAR)VK_INSERT;/*MapVirtualKey(VK_INSERT,0);*/}
	if(nombre.Compare("GREY-HOME")==0) {ext=true;codigo=(UCHAR)VK_HOME;/*MapVirtualKey(VK_HOME,0);*/}
	if(nombre.Compare("GREY-PGUP")==0) {ext=true;codigo=(UCHAR)VK_PRIOR;/*MapVirtualKey(VK_PRIOR,0);*/}
	if(nombre.Compare("GREY-LEFT")==0) {ext=true;codigo=(UCHAR)VK_LEFT;/*MapVirtualKey(VK_LEFT,0);*/}
	if(nombre.Compare("GREY-RIGHT")==0) {ext=true;codigo=(UCHAR)VK_RIGHT;/*MapVirtualKey(VK_RIGHT,0);*/}
	if(nombre.Compare("GREY-UP")==0) {ext=true;codigo=(UCHAR)VK_UP;/*MapVirtualKey(VK_UP,0);*/}
	if(nombre.Compare("GREY-DOWN")==0) {ext=true;codigo=(UCHAR)VK_DOWN;/*MapVirtualKey(VK_DOWN,0);*/}
	if(nombre.Compare("DECIMAL")==0) codigo=(UCHAR)VK_DECIMAL;/*MapVirtualKey(VK_DECIMAL,0);*/
	if(nombre.Compare("GREY-RTN")==0) {ext=true;codigo=(UCHAR)VK_RETURN;/*MapVirtualKey(VK_RETURN,0);*/}
	if(nombre.Compare("ADD")==0) codigo=(UCHAR)VK_ADD;/*MapVirtualKey(VK_ADD,0);*/
	if(nombre.Compare("SUBTRACT")==0) codigo=(UCHAR)VK_SUBTRACT;/*MapVirtualKey(VK_SUBTRACT,0);*/
	if(nombre.Compare("MULTIPLY")==0) codigo=(UCHAR)VK_MULTIPLY;/*MapVirtualKey(VK_MULTIPLY,0);*/
	if(nombre.Compare("DIVIDE")==0) {ext=true;codigo=(UCHAR)VK_DIVIDE;/*MapVirtualKey(VK_DIVIDE,0);*/}
	if(nombre.Compare("NUMLOCK")==0) {ext=true;codigo=(UCHAR)VK_NUMLOCK;/*MapVirtualKey(VK_NUMLOCK,0);*/}
	if(nombre.Compare("0N")==0) codigo=(UCHAR)VK_NUMPAD0;/*MapVirtualKey(VK_NUMPAD0,0);*/
	if(nombre.Compare("1N")==0) codigo=(UCHAR)VK_NUMPAD1;/*MapVirtualKey(VK_NUMPAD1,0);*/
	if(nombre.Compare("2N")==0) codigo=(UCHAR)VK_NUMPAD2;/*MapVirtualKey(VK_NUMPAD2,0);*/
	if(nombre.Compare("3N")==0) codigo=(UCHAR)VK_NUMPAD3;/*MapVirtualKey(VK_NUMPAD3,0);*/
	if(nombre.Compare("4N")==0) codigo=(UCHAR)VK_NUMPAD4;/*MapVirtualKey(VK_NUMPAD4,0);*/
	if(nombre.Compare("5N")==0) codigo=(UCHAR)VK_NUMPAD5;/*MapVirtualKey(VK_NUMPAD5,0);*/
	if(nombre.Compare("6N")==0) codigo=(UCHAR)VK_NUMPAD6;/*MapVirtualKey(VK_NUMPAD6,0);*/
	if(nombre.Compare("7N")==0) codigo=(UCHAR)VK_NUMPAD7;/*MapVirtualKey(VK_NUMPAD7,0);*/
	if(nombre.Compare("8N")==0) codigo=(UCHAR)VK_NUMPAD8;/*MapVirtualKey(VK_NUMPAD8,0);*/
	if(nombre.Compare("9N")==0) codigo=(UCHAR)VK_NUMPAD9;/*MapVirtualKey(VK_NUMPAD9,0);*/
	if(ext) {
		switch(i) {
			case 0: *teclaExt=*teclaExt+1; break;
			case 1: *teclaExt=*teclaExt+2; break;
			case 2: *teclaExt=*teclaExt+4; break;
			case 3: *teclaExt=*teclaExt+8; break;
			case 4: *teclaExt=*teclaExt+16; break;
			case 5: *teclaExt=*teclaExt+32; break;
			case 6: *teclaExt=*teclaExt+64; break;
			case 7: *teclaExt=*teclaExt+128;
		}
	}
	return codigo;
}


//------------------ XMP Antiguos ---------------------/


void CX36Import::ConvertirXMP(CString ruta)
{
	CFile f,g;
	UINT16 pos;
	UCHAR buffer[36];
	UCHAR puerto[10];
	ZeroMemory(buffer,36);
	bool fcerrado=false;
	
	if(f.Open(ruta.GetBuffer(),CFile::modeRead)==0)
	{
		MessageBox(NULL,MSG1,"Error",MB_ICONWARNING);
		return;
	}
	ruta.Delete(ruta.GetLength()-3,3);
	ruta.Append("tmp");
	if(g.Open(ruta.GetBuffer(),CFile::modeReadWrite|CFile::modeCreate|CFile::shareExclusive)==0)
	{
		f.Close();
		MessageBox(NULL,MSG1,"Error - tmp",MB_ICONWARNING);
		return;
	}
try
{
	g.Write(buffer,6);
	g.Write(buffer,36);
	g.Write(buffer,36);
	g.Write(buffer,24);
	f.Read(&pos,2);
	g.Write(&pos,2);
	for(UINT16 i=0;i<pos;i++)
	{
		f.Read(buffer,32);
		g.Write(buffer,32);
		f.Read(buffer,9);
		g.Write(buffer,9);
	}
	f.Read(puerto,10);
	f.Read(buffer,24);
	g.Seek(36+36+6,0);
	g.Write(buffer,24);
	g.SeekToEnd();
	for(UCHAR i=0;i<6;i++)
	{
		for(UCHAR j=0;j<104;j++)
		{
			f.Read(buffer,2);
			g.Write(buffer,2);
		}
	}
	f.Read(buffer,36); g.Write(buffer,36);
	f.Read(buffer,36); g.Write(buffer,36);
	f.Read(buffer,36); g.Write(buffer,36);
	f.Read(buffer,36); g.Write(buffer,36);
	f.Read(buffer,36); g.Write(buffer,36);
	f.Read(buffer,36); g.Write(buffer,36);

	f.Close();
	fcerrado=true;

	ConvertirPuerto(puerto);
	for(UCHAR i=0;i<6;i++)
	{
		if(puerto[i]!=255)
		{
			g.Seek(6+puerto[i],0);
			i++;
			g.Write(&i,1);
			i--;
		}
	}

	ConvertirPOV(buffer,puerto[6],puerto[7]);
	g.Seek(6+36,0);
	g.Write(buffer,36);

	buffer[0]=puerto[9];
	buffer[1]=puerto[8];
	buffer[2]=0; buffer[3]=1; buffer[4]=2; buffer[5]=3;
	g.SeekToBegin();
	g.Write(buffer,6);

	g.Flush();
	g.Close();

	{
		CString rutan;
		ruta.Delete(ruta.GetLength()-3,3);
		ruta.Append("xmp");
		CFile::Remove(ruta.GetBuffer());

		ruta.Delete(ruta.GetLength()-3,3);
		rutan=ruta.GetBuffer();
		rutan.Append("xmp");
		ruta.Append("tmp");
		CFile::Rename(ruta.GetBuffer(),rutan.GetBuffer());
	}
} catch(CFileException* e) {
	if(!fcerrado) f.Close();
	g.Close();
	CFile::Remove(ruta.GetBuffer());
	MessageBox(NULL,MSG29,"Error",MB_ICONERROR);
}

}

void CX36Import::ConvertirPuerto(UCHAR* buffer)
{
	for(UCHAR i=0;i<6;i++)
	{
		switch(buffer[i])
		{
			case 0: buffer[i]=255; break;
			case 13: buffer[i]=0; break;
			case 4: buffer[i]=1; break;
			case 1: buffer[i]=4; break;
			case 5: buffer[i]=6; break;
			case 6: buffer[i]=9; break;
			case 9: buffer[i]=14; break;
			case 11: buffer[i]=15; break;
			case 12: buffer[i]=16; break;
			case 10: buffer[i]=17; break;
			case 15: buffer[i]=22; break;
			case 16: buffer[i]=23; break;
			case 21: buffer[i]=28; break;
			case 23: buffer[i]=29; break;
			case 24: buffer[i]=30; break;
			case 22: buffer[i]=31; break;
			case 17: buffer[i]=32; break;
			case 19: buffer[i]=33; break;
			case 20: buffer[i]=34; break;
			case 18: buffer[i]=35; break;
			default: break;
		}
	}
}

void CX36Import::ConvertirPOV(UCHAR* buffer,UCHAR pov1,UCHAR pov2)
{
	ZeroMemory(buffer,36);
	const p[4][4]={{6,7,8,9},{14,15,16,17},{28,29,30,31},{32,33,34,35}};
	if(pov1>0) {
		buffer[p[pov1-1][0]]=1;
		buffer[p[pov1-1][1]]=5;
		buffer[p[pov1-1][2]]=7;
		buffer[p[pov1-1][3]]=3;
	}
	if(pov2>0) {
		buffer[p[pov1-1][0]]=17;
		buffer[p[pov1-1][1]]=21;
		buffer[p[pov1-1][2]]=23;
		buffer[p[pov1-1][3]]=19;
	}
}
