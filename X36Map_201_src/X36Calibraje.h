// X36NuevoDlg.h: Dialogo para crear un mapa nuevo
// Definición

#pragma once


class CX36Calibraje : public CDialog
{
public:
	CX36Calibraje(CWnd* pParent = NULL);   // Constructor
	DECLARE_MESSAGE_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBx();
	afx_msg void OnBnClickedBy();
	afx_msg void OnBnClickedBz();
	afx_msg void OnBnClickedBr();
	afx_msg void OnBnClickedBorrar();


private:
	struct {LONG i; LONG c; LONG d;}Limite[4];
	LONG EjesPosAnt[4];
	UCHAR ZonaNula[4];
	UCHAR Escala_izq[4];
	UCHAR Escala_der[4];
	void LeerRegistro();
	bool IniciarDX();
	bool GuardarRegistro();
	void ReglasVirtuales(LONG Axis[]);
	UCHAR nEjes;
	CSliderCtrl reglaxr,reglaxv;
	CSliderCtrl reglayr,reglayv;
	CSliderCtrl reglazr,reglazv;
	CSliderCtrl reglarr,reglarv;

	void LeerValor(HKEY key,LPCTSTR valor,int nid);
	bool GuardarValor(HKEY key,LPCTSTR valor,int nid,bool bin);
	LONG LeerEdit(int nid);
	bool CX36Calibraje::Comprobar(
							  LPCSTR eje,
							  UCHAR e,
							  int izq,
							  int cen,
							  int der,
							  int nul,
							  int mul,
							  int div,
							  int rmi,
							  int rma,
							  CSliderCtrl* regla);
	bool ActualizarX();
	bool ActualizarY();
	bool ActualizarZ();
	bool ActualizarR();
};
