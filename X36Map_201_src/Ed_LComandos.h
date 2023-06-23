// Ed_LComandos.h: Dialogo de lista de comandos
// Definición

#pragma once


class CEd_LComandos : public CDialog
{
public:
	CEd_LComandos(CX36Editor* editor);  // Constructor
	virtual ~CEd_LComandos();			// Destructor

	int Insertar(CString nombre, UINT64 teclas, UCHAR teclasExt, bool ini=false);
	void CogerDatos(UINT16 idx,UINT64* teclas, UCHAR* teclasExt);
	bool CogerNombres(UINT16 idx, CString* texto);
	UINT16 NumComandos();

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnClose();
	afx_msg void OnBnClickedCmdborrar();
	afx_msg void OnBnClickedCmdnuevo();
	afx_msg void OnBnClickedCmdeditar();
	afx_msg void OnBnClickedCmdraton();

	DECLARE_MESSAGE_MAP()

private:
	CX36Editor* editor;

	CList <UINT64,UINT64&> codigosT;
	CList <UCHAR,UCHAR&> codigosTExt;
	CListCtrl lista;
	bool* pIndicesAux;
	bool* pCombosAux;

	void Borrar();
	void PreEditar();
	void PostEditar(int sel);
};