// CEditEsp.cpp: Crea un Control CEdit Especial
// para grabar las pulsaciones de teclas

// Definición

#pragma once

class CEditEsp : public CEdit
{
public:
	CEditEsp();

	UCHAR scanCode;
	bool teclaExtendida;
protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};
