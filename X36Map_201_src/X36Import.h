// X36Import.h: Importar configuraciones antiguas
// Definición

#pragma once

class CX36Import
{
public:
	CX36Import(CWnd* padre);	// Constructor
	virtual ~CX36Import();		// Destructor

private:
	CList <UINT16,UINT16&> bufferpos;
	UINT16 indices[6][104];
	UCHAR botonesP[9];
	UCHAR giraT[4][6];
	bool autoRepeat[6][36];
	CListBox* lista;

	void Iniciar(void);
	void Importar(CString ruta,CString path);
	bool ImportarCMM(CString ruta,CString path,CString* nombre);
	void ImportarX36(CString ruta,CString nombre);
	void ConvertirTeclas(char datos[],UINT64* teclas,UCHAR* teclasExt);
	UINT16 ConvertirHex(char numero[]);
	UCHAR SacarCodigo(CString nombre,UCHAR* teclaExt,UCHAR i);
	void SacarBytes(UINT16 datos[],CString cadena);
	void SacarBytesPuerto(UCHAR datos[],CString cadena);
	void GuardaBoton(UINT16 bytes[],UCHAR pos,bool rep);
	void GuardaGiro(UINT16 bytes[],UCHAR pos,bool posic);
	void ConvertirXMP(CString ruta);
	void ConvertirPuerto(UCHAR* buffer);
	void ConvertirPOV(UCHAR* buffer,UCHAR pov1,UCHAR pov2);
};
