// Prop_Btn_pov.h: Página de propiedades de povs
// Definición

#pragma once
#include "Prop_Btn_prt.h"


class CProp_Btn_pov : public CProp_Btn_prt
{
	DECLARE_DYNAMIC(CProp_Btn_pov)

public:
	CProp_Btn_pov(CX36Editor *editor,CEd_LComandos *cmds,UCHAR modo);
	void GuardarDatos();

protected:
	void CargarDatos();
	void ResetGUI();

	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX o DDV
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedAsigPuertoPOV();

	DECLARE_MESSAGE_MAP()
private:
	CButton sAsignarPuertoPOV;
	CComboBox comboPOV;
};
