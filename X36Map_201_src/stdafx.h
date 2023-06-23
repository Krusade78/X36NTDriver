#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Excluir material rara vez utilizado de encabezados de Windows
#endif

#define _WIN32_WINNT 0x0501
#define WINVER 0x0501
#define _WIN32_IE 0x600

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// Algunos constructores CString serán explícitos
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // Componentes principales y estándar de MFC
#include <afxcmn.h>			// Compatibilidad MFC para controles comunes de Windows
#include <afxtempl.h>
#include <afxdlgs.h>

#include "IdiomasMsg.h"
#include "resource.h"

