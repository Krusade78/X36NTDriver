// Prop_Rot_eje.h: Página de propiedades de ejes
// Definición

#pragma once
#include "Prop_Rot.h"


class CProp_Rot_eje : public CProp_Rot
{
	DECLARE_DYNAMIC(CProp_Rot_eje)

public:
	CProp_Rot_eje(CX36Editor *editor,CEd_LComandos *cmds,UCHAR modo);
	void GuardarDatos();

protected:
	CButton sAsignarPuerto;
	void CargarDatos();
	void ResetGUI();

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedAsigPuerto();

	DECLARE_MESSAGE_MAP()
};