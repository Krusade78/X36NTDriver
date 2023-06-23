// Prop_Btn_prt.h: Página de propiedades de botones
// Definición

#pragma once

class CProp_Btn : public CPropertyPage
{
	DECLARE_DYNAMIC(CProp_Btn)

public:
	CProp_Btn(CX36Editor *editor,CEd_LComandos *cmds,UCHAR modo);
	void GuardarDatos();

protected:
	UCHAR pagina,modo;
	CX36Editor* editor;
	CComboBox comboApr;
	CComboBox comboSol;
	CComboBox comboAprPk;
	CComboBox comboSolPk;
	CButton sAutoRepeat;
	CButton sAutoRepeatPk;
	void CargarDatos();
	void ResetGUI();

	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX o DDV
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();

	DECLARE_MESSAGE_MAP()
private:
	CEd_LComandos* cmds;
};
