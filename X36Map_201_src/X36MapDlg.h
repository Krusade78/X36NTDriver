// X36MapDlg.h: Dialogo principal de programa
// Definición

#pragma once

class CX36MapDlg : public CDialog
{
public:
	CX36MapDlg(CWnd* pParent = NULL);	// Constructor
	HANDLE hl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedActualizar();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnLbnSelcancelList1();
	afx_msg void OnBnClickedCargar();
	afx_msg void OnBnClickedCambiar();
	afx_msg void OnBnClickedNuevo();
	afx_msg void OnBnClickedEditar();
	afx_msg void OnBnClickedImportar();
	afx_msg void OnBnClickedCalibrar();
	afx_msg void OnBnClickedSeleccionar();
	afx_msg void OnBnClickedActivar();

	DECLARE_MESSAGE_MAP()

private:
	CSliderCtrl sensiDes,preciDes;
	CListBox listaMapas;
	CButton activar;
	void CargarDatosRaton(void);
	void CargarDatosAuto(void);
	void CambiarRaton(void);
	void CargarListaMapas(void);
	void CargarMapa(bool ini=false);
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
