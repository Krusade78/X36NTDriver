// Pro_Rot.h: Dialogo para página de propiedades rotatorios
// Definición

#pragma once

class CProp_Rot : public CPropertyPage
{
	DECLARE_DYNAMIC(CProp_Rot)

public:
	CProp_Rot(CX36Editor *editor,CEd_LComandos *cmds,UCHAR modo);
	void GuardarDatos();

protected:
	UCHAR pagina,modo;
	CX36Editor* editor;
	CComboBox comboInc;
	CComboBox comboDec;
	CComboBox comboIncPk;
	CComboBox comboDecPk;
	CListBox listaCmds;
	CListBox listaCmdsPk;
	CListBox listaPos;
	CListBox listaPosPk;
	CButton sPosiciones;
	CButton sIncremental;
	CButton sPosicionesPk;
	CButton sIncrementalPk;
	CSliderCtrl sensi;
	CSliderCtrl sensiPk;
	void CargarDatos();
	void ResetGUI();


	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedPosiciones();
	afx_msg void OnBnClickedIncremental();
	afx_msg void OnBnClickedPosicionesPk();
	afx_msg void OnBnClickedIncrementalPk();
	afx_msg void OnBnClickedbborrar();
	afx_msg void OnBnClickedbborrarpk();
	afx_msg void OnBnClickedbinsertar();
	afx_msg void OnBnClickedbinsertarpk();

	DECLARE_MESSAGE_MAP()
private:
	CEd_LComandos* cmds;
};
