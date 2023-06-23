/*--
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    x36.h

Funcion : funciones de E/S

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
typedef struct _PAQUETE_SAITEK
{
	UCHAR Modo;
	UCHAR Posicion;
	UINT64 Dato;
	UCHAR DatoExt;
} PAQUETE_SAITEK, *PPAQUETE_SAITEK;

class Cx36
{
public:
	Cx36(void);
	virtual ~Cx36(void);
	UCHAR ejes[4];

	void IniciarX36();
	BOOL IniciarServicio();
	void CargarMapa(bool ini=false);
	bool Recalibrar();
	void Descalibrar();
	void PonerPausa();
	void PonerSensibilidad();
private:
	UCHAR acelPed;

	bool AutoCarga();
	void CargarEstandar();
	BOOL IniciarDriver();
	bool ReordenarCalibracion(UCHAR* buf);
};