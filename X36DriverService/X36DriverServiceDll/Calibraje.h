/*--
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    Calibraje.h

Funcion : Funciones DirectX (clase)

Notes:

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

--*/

#pragma once

class CReorden
{
public:
	CReorden();
	virtual ~CReorden();
	bool Recalibrar(UCHAR* ejes, HINSTANCE hInst);
private:
	struct {LONG i; LONG c; LONG d; UCHAR n; UCHAR iz; UCHAR de;}datosEje[4];
	UCHAR nEjes;
	bool LeerRegistro();
	bool Comprobar(UCHAR eje);
	bool IniciarDX(HINSTANCE hInst);
	bool GuardarDatos(UCHAR* ejes);
};
