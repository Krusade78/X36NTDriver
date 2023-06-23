// Comando.h: Dialogo para crear/editar comandos
// Definición

#pragma once
#include "CEditEsp.h"



class CComando : public CDialog
{
public:
	CComando(bool ins,CString* nombre,CListCtrl* lista,UINT64* teclas,UCHAR* teclasExt,CWnd* pParent = NULL);   // Constructor
	virtual ~CComando();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancelar();
	afx_msg void OnBnClickedAceptar();
	afx_msg void OnBnClickedInstecla();
	afx_msg void OnBnClickedBotecla();

	DECLARE_MESSAGE_MAP()

private:
	void CargarListas();

	bool insertar;
	CString* pNombre;
	CListCtrl* pLista;
	UINT64* pTeclas;
	UCHAR* pTeclasExt;
	CEditEsp pulsacion;
	CListBox listaTeclas;
	CList <UCHAR,UCHAR&> codigos;
	CList <bool,bool&> codigosExt;
	CEdit nombre;
};
