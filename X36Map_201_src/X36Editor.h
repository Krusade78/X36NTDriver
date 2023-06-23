// X36Editor.h: Dialogo para editar mapas
// Definición

#pragma once

class CX36Editor : public CDialog
{
public:
	CX36Editor(CString archivoXMP,CWnd* pParent = NULL);   // Constructor
	virtual ~CX36Editor();

	UCHAR PosicionActual();
	UINT16 CogerIndice(UCHAR modo, UCHAR pos);
	void PonerIndice(UCHAR modo, UCHAR pos, UINT16 idx);
	bool CogerAutoRepeat(UCHAR modo, UCHAR pos);
	void PonerAutoRepeat(UCHAR modo, UCHAR pos, bool b);
	UCHAR CogerGiraT(UCHAR modo);
	UCHAR CogerGiraT(UCHAR eje, UCHAR modo);
	void PonerGiraT(UCHAR eje, UCHAR modo, UCHAR idx);
	UCHAR CogerBtDx(UCHAR pos);
	void PonerBtDx(UCHAR pos, UCHAR idx);
	UCHAR CogerPovDx(UCHAR pos);
	void PonerPovDx(UCHAR pos, UCHAR idx);
	UCHAR CogerAcelPed();
	void PonerAcelPed(UCHAR idx);
	bool ConModos();
	bool ConPinkie();
	void Visualizar();

protected:
	afx_msg BOOL Hints( UINT id, NMHDR* pTTTStruct, LRESULT *pResult );
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSalir();
	afx_msg void OnArchivoGuardarcomo();
	afx_msg void OnSobreSobreelx36map();
	afx_msg void OnTcnSelchangeTab2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeTab3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedPinkie();
	afx_msg void OnBnClickedModos();
	afx_msg void OnBnClickedGuardar();
	afx_msg void OnBnClickedResumen();
	afx_msg void OnBnClickedEditorcomandos();

	DECLARE_MESSAGE_MAP()
private:
	CToolBarCtrl barra;
	CStatic linea;
	CDialog* CCmds;
	CPropertySheet tab1;
	CPropertyPage* pagina0;
	CPropertyPage* pagina1;
	CPropertyPage* pagina2;
	UCHAR tpagina[3];
	CTabCtrl tab2;
	CTabCtrl tab3;
	CComboBox combox;
	CComboBox comboy;
	CComboBox comboz;
	CComboBox combor;

	CButton sPinkie;
	CButton sModo;

	bool autoRepeat[6][36];
	UINT16 indices[6][104];
	UCHAR botonesP[36];
	UCHAR povsP[36];
	UCHAR giraT[4][6];
	UCHAR ejes[5];

	CString archivoXMP;
	UCHAR tab2CurSel;
	UCHAR tab3CurSel;

	bool Salir();
	void ConfigurarBarra();
	void CargarArchivo();
	bool GuardarArchivo(bool UI=false);
	void CambiarTab3();
	void CambiarPagina(bool cargar, bool guardar);
	void CargarPagina(CPropertyPage** pgn, UCHAR modo);
	void GuardarPagina(CPropertyPage** pgn);
	UCHAR CogerModo();
};