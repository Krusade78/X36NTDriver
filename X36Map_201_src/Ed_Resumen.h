// Ed_Resumen.h: Dialogo para imprimir/ver la configuración
// Definición

#pragma once

class CEd_Resumen : public CDialog
{
public:
	CEd_Resumen(CWnd* editor,CEd_LComandos *cmds, CString archivo);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedImprimir();

	DECLARE_MESSAGE_MAP()

private:
	CX36Editor* editor;
	CEd_LComandos* cmds;
	CListCtrl lista[3];
	CString archivoXMP;

	void CambiarItems(UCHAR l);
	void CargarBotones(UCHAR l);
	void CogerDatos(UCHAR i, UCHAR l);
	void CogerDatosRot(UCHAR i, UCHAR l);
	void ImprimirPagina(CDC* cdc,CFont* font1,CFont* font2,CFont* font3,CFont* font4,UCHAR l);
};