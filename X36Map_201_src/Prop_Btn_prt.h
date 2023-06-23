// Prop_Btn_prt.h: Página de propiedades de botones del puerto
// Definición

#pragma once
#include "Prop_Btn.h"


class CProp_Btn_prt : public CProp_Btn
{
	DECLARE_DYNAMIC(CProp_Btn_prt)

public:
	CProp_Btn_prt(CX36Editor *editor,CEd_LComandos *cmds,UCHAR modo);
	void GuardarDatos();

protected:
	CButton sAsignarPuerto;
	CComboBox comboPuerto;
	void CargarDatos();
	void ResetGUI();

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedAsigPuerto();

	DECLARE_MESSAGE_MAP()
};
