/*--
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    x36.c

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


void CambiarEstado(PX36DEVICE_EXTENSION devExt,UCHAR ID,BOOLEAN dato)
{
	if(devExt->mapaB[ID])
	{
		UCHAR nID=devExt->mapaB[ID]-1;
		if(dato)
		{
			devExt->boton[nID/8]|=1<<(nID%8);
		} else {
			devExt->boton[nID/8]&=255-(1<<(nID%8));
		}
	}
    if(devExt->mapaS[ID])
	{
		if(dato)
		{
			devExt->seta[(devExt->mapaS[ID]>>4)]=devExt->mapaS[ID]&15;
		} else {
			devExt->seta[(devExt->mapaS[ID]>>4)]=0;
		}				
	}
}

void Convertir(UCHAR teclasExt, UCHAR* E0s)
{
        E0s[7]=((teclasExt&128)==128) ? TRUE : FALSE;
        E0s[6]=((teclasExt&64)==64) ? TRUE : FALSE;
        E0s[5]=((teclasExt&32)==32) ? TRUE : FALSE;
        E0s[4]=((teclasExt&16)==16) ? TRUE : FALSE;
        E0s[3]=((teclasExt&8)==8) ? TRUE : FALSE;
        E0s[2]=((teclasExt&4)==4) ? TRUE : FALSE;
        E0s[1]=((teclasExt&2)==2) ? TRUE : FALSE;
        E0s[0]=((teclasExt&1)==1) ? TRUE : FALSE;
}



void EnviarCodigo(PX36DEVICE_EXTENSION devExt,UCHAR posicion,PKEYBOARD_INPUT_DATA inData,PPOOL pool, UCHAR soltar)
{
	KEYBOARD_INPUT_DATA datos=*inData;
	KEYBOARD_INPUT_DATA inPaq[25];

	CHAR i=0,j=0;
	UCHAR npaqs=0,posicionAnt=255;
	UCHAR* codigos;
	BOOLEAN E0s[8];
	BOOLEAN saltar=FALSE;

	datos.Flags=0;
	for(i=0;i<25;i++) inPaq[i]=datos;

	if(soltar!=255) {			// soltar teclas anteriores
		if(soltar>35) {
			posicionAnt=posicion;
			posicion=soltar;
		}
	}

pulso:
	for(i=0;i<6;i++)
	{

		if(posicion==devExt->ratonPos[i])
		{
			if(devExt->ratonEv[i]!=NULL)
	            KeSetEvent(devExt->ratonEv[i],0,FALSE);
			return;
		}
		if(posicion==(devExt->ratonPos[i]+36))
		{
			if(i>3) {
				if(devExt->ratonEv[i+2]!=NULL) 
                    KeSetEvent(devExt->ratonEv[i+2],0,FALSE);
			} else {
				if(devExt->ratonEv[i]!=NULL)
					KeClearEvent(devExt->ratonEv[i]);
			}
			return;
		}
	}

	if(posicion>35 && posicion<72) {
		if(devExt->autoRepeatsOn[posicion-36]!=255)
		{
            BOOLEAN letra=FALSE;

			codigos=(UCHAR*)&devExt->teclas[devExt->autoRepeatsOn[posicion-36]][posicion-36];
			Convertir(devExt->teclasExt[devExt->autoRepeatsOn[posicion-36]][posicion-36],E0s);
			for(i=8;i>0;i--) {
				if(codigos[i-1]!=0) {
					if((codigos[i-1]!=devExt->combis[0])
						&&(codigos[i-1]!=devExt->combis[1])
						&&(codigos[i-1]!=devExt->combis[2])
						&&(codigos[i-1]!=devExt->combis[3])
						&&(codigos[i-1]!=devExt->combis[4])
						&&(codigos[i-1]!=devExt->combis[5])
						&&(codigos[i-1]!=devExt->combis[6])
						&&(codigos[i-1]!=devExt->combis[7]))
					{
						if(letra) i=0; else letra=TRUE;
					}
					if(i!=0)
					{
						inPaq[npaqs].MakeCode=codigos[i-1];
						if(E0s[i-1]) {
							inPaq[npaqs].Flags=KEY_E0|KEY_BREAK;
						} else {
							inPaq[npaqs].Flags=KEY_BREAK;
						}
						npaqs++;
					}
				}
			}
		}
	}

	if (devExt->teclas[devExt->stModo+devExt->stPinkie][posicion]!=0)
	{
        BOOLEAN vuelta=FALSE;

		codigos=(UCHAR*)&devExt->teclas[devExt->stModo+devExt->stPinkie][posicion];
		Convertir(devExt->teclasExt[devExt->stModo+devExt->stPinkie][posicion],E0s);

		for(i=0;i<8;i++) {
			if(codigos[i]==0)
			{
				i=8;
			} else {
				if(!vuelta)
				{
					inPaq[npaqs].MakeCode=codigos[i];
					if(E0s[i]) inPaq[npaqs].Flags=KEY_E0;
					npaqs++;
					if((codigos[i]!=devExt->combis[0])
						&&(codigos[i]!=devExt->combis[1])
						&&(codigos[i]!=devExt->combis[2])
						&&(codigos[i]!=devExt->combis[3])
						&&(codigos[i]!=devExt->combis[4])
						&&(codigos[i]!=devExt->combis[5])
						&&(codigos[i]!=devExt->combis[6])
						&&(codigos[i]!=devExt->combis[7]))
					{
                        vuelta=TRUE;
						i--;
					}
				} else {
					if(codigos[i+1]==0) {
						if(posicion<36)	{
							if(devExt->autoRepeat[devExt->stModo+devExt->stPinkie][posicion]) {
								devExt->autoRepeatsOn[posicion]=devExt->stModo+devExt->stPinkie;
                                vuelta=FALSE;
							} else {
								devExt->autoRepeatsOn[posicion]=255;
							}
						}
					}

					if(vuelta) {
                        vuelta=FALSE;
						for(j=i;j>-1;j--)
						{
							inPaq[npaqs].MakeCode=codigos[j];
							if(E0s[j]) {
								inPaq[npaqs].Flags=KEY_BREAK|KEY_E0;
							} else {
								inPaq[npaqs].Flags=KEY_BREAK;
							}
							npaqs++;
							if((codigos[j]!=devExt->combis[0])
								&&(codigos[j]!=devExt->combis[1])
								&&(codigos[j]!=devExt->combis[2])
								&&(codigos[j]!=devExt->combis[3])
								&&(codigos[j]!=devExt->combis[4])
								&&(codigos[j]!=devExt->combis[5])
								&&(codigos[j]!=devExt->combis[6])
								&&(codigos[j]!=devExt->combis[7]))
							{
								if(vuelta) j=-1; else vuelta=TRUE;
							}
						}
                        vuelta=FALSE;
					}
				}
			}
		}
	}
	
	if(!saltar)
	{
		if(posicionAnt!=255) {		// Ejecutar pulsacion actual
			posicion=posicionAnt;
			saltar=TRUE;
			goto pulso;
		} else {
			if(soltar<36)
			{
				posicion=soltar;
				saltar=TRUE;
				goto pulso;
			}
		}
	}

	if(npaqs>0)
	{
	if(pool->pool==NULL) {
		pool->pool=ExAllocatePool(NonPagedPool,npaqs*sizeof(KEYBOARD_INPUT_DATA));
		if(pool->pool!=NULL) {
				RtlCopyMemory(pool->pool,(PVOID)inPaq,npaqs*sizeof(KEYBOARD_INPUT_DATA));
				pool->tam=npaqs;
		}
	} else {
		PVOID auxPool=ExAllocatePool(NonPagedPool,(npaqs+pool->tam)*sizeof(KEYBOARD_INPUT_DATA));
		if(auxPool!=NULL) {
			RtlCopyMemory(auxPool,pool->pool,pool->tam*sizeof(KEYBOARD_INPUT_DATA));
			ExFreePool(pool->pool);
			RtlCopyMemory((PVOID)(((PKEYBOARD_INPUT_DATA)auxPool)+pool->tam),(PVOID)inPaq,npaqs*sizeof(KEYBOARD_INPUT_DATA));
			pool->tam+=npaqs;
			pool->pool=auxPool;
		}
	}
	}
}



void TraducirBoton(PX36DEVICE_EXTENSION devExt,PKEYBOARD_INPUT_DATA inData, PPOOL pool)
{
	switch (inData->MakeCode)
	{
//****************************** X36 F
//------------ Gatillo
		case 13:
            CambiarEstado(devExt,0,TRUE);
            EnviarCodigo(devExt,0,inData,pool,255);
			break;
		case 77:
            CambiarEstado(devExt,0,FALSE);
            EnviarCodigo(devExt,36,inData,pool,255);
			break;
//------------ Lanzar
		case 4:
            CambiarEstado(devExt,1,TRUE);
            EnviarCodigo(devExt,1,inData,pool,255);
			break;
		case 68:
            CambiarEstado(devExt,1,FALSE);
            EnviarCodigo(devExt,37,inData,pool,255);
			break;

//------------ Boton a
		case 2:
            CambiarEstado(devExt,2,TRUE);
            EnviarCodigo(devExt,2,inData,pool,255);
			break;
		case 66:
            CambiarEstado(devExt,2,FALSE);
            EnviarCodigo(devExt,38,inData,pool,255);
			break;
//----------- Boton b
		case 3:
            CambiarEstado(devExt,3,TRUE);
            EnviarCodigo(devExt,3,inData,pool,255);
			break;
		case 67:
            CambiarEstado(devExt,3,FALSE);
            EnviarCodigo(devExt,39,inData,pool,255);
			break;
//------------ Boton c
		case 1:
            CambiarEstado(devExt,4,TRUE);
            EnviarCodigo(devExt,4,inData,pool,255);
			break;
		case 65:
            CambiarEstado(devExt,4,FALSE);
            EnviarCodigo(devExt,40,inData,pool,255);
			break;

// ------------ Pinkie
		case 14:
			if (!devExt->pinkie) {
                CambiarEstado(devExt,5,TRUE);
                EnviarCodigo(devExt,5,inData,pool,255);
			} else {
				devExt->stPinkie=1;
				devExt->gPosAnt[0]=11;
				devExt->gPosAnt[1]=11;
				devExt->gPosAnt[2]=11;
				devExt->gPosAnt[3]=11;
			}
			break;
		case 78:
			if (!devExt->pinkie) {
                CambiarEstado(devExt,5,FALSE);
                EnviarCodigo(devExt,41,inData,pool,255);
			} else {
				devExt->stPinkie=0;
				devExt->gPosAnt[0]=11;
				devExt->gPosAnt[1]=11;
				devExt->gPosAnt[2]=11;
				devExt->gPosAnt[3]=11;
			}
			break;
//************ Seta 1
		case 5:
			devExt->stSeta1=devExt->stSeta1|1;
			if((devExt->stSeta1&4)) {
                CambiarEstado(devExt,8,FALSE);
                CambiarEstado(devExt,10,TRUE);
				EnviarCodigo(devExt,10,inData,pool,44);
			} else {
				if((devExt->stSeta1&8)) {
                    CambiarEstado(devExt,9,FALSE);
                    CambiarEstado(devExt,11,TRUE);
					EnviarCodigo(devExt,11,inData,pool,45);
				} else {
                    CambiarEstado(devExt,6,TRUE);
                    EnviarCodigo(devExt,6,inData,pool,255);
				}
			}
			break;
		case 69:
			devExt->stSeta1=devExt->stSeta1&254;
			if((devExt->stSeta1&4)) {
                CambiarEstado(devExt,10,FALSE);
                CambiarEstado(devExt,8,TRUE);
				EnviarCodigo(devExt,46,inData,pool,8);
			} else {
				if((devExt->stSeta1&8)) {
                    CambiarEstado(devExt,11,FALSE);
                    CambiarEstado(devExt,9,TRUE);
					EnviarCodigo(devExt,47,inData,pool,9);
				} else {
                    CambiarEstado(devExt,6,FALSE);
                    EnviarCodigo(devExt,42,inData,pool,255);
				}
			}
			break;
		case 7:
			devExt->stSeta1=devExt->stSeta1|2;
			if((devExt->stSeta1&4)) {
                CambiarEstado(devExt,8,FALSE);
                CambiarEstado(devExt,12,TRUE);
				EnviarCodigo(devExt,12,inData,pool,44);
			} else {
				if((devExt->stSeta1&8)) {
                    CambiarEstado(devExt,9,FALSE);
                    CambiarEstado(devExt,13,TRUE);
					EnviarCodigo(devExt,13,inData,pool,45);
				} else {
                    CambiarEstado(devExt,7,TRUE);
                    EnviarCodigo(devExt,7,inData,pool,255);
				}
			}
			break;
		case 71:
			devExt->stSeta1=devExt->stSeta1&253;
			if((devExt->stSeta1&4)) {
                CambiarEstado(devExt,12,FALSE);
                CambiarEstado(devExt,8,TRUE);
				EnviarCodigo(devExt,48,inData,pool,8);
			} else {
				if((devExt->stSeta1&8)) {
                    CambiarEstado(devExt,13,FALSE);
                    CambiarEstado(devExt,9,TRUE);
					EnviarCodigo(devExt,49,inData,pool,9);
				} else {
                    CambiarEstado(devExt,7,FALSE);
                    EnviarCodigo(devExt,43,inData,pool,255);
				}
			}
			break;
		case 8:
			devExt->stSeta1=devExt->stSeta1|4;
			if((devExt->stSeta1&1)) {
                CambiarEstado(devExt,6,FALSE);
                CambiarEstado(devExt,10,TRUE);
				EnviarCodigo(devExt,10,inData,pool,42);
			} else {
				if((devExt->stSeta1&2)) {
                    CambiarEstado(devExt,7,FALSE);
                    CambiarEstado(devExt,12,TRUE);
					EnviarCodigo(devExt,12,inData,pool,43);
				} else {
                    CambiarEstado(devExt,8,TRUE);
                    EnviarCodigo(devExt,8,inData,pool,255);
				}
			}
			break;
		case 72:
			devExt->stSeta1=devExt->stSeta1&251;
			if((devExt->stSeta1&1)) {
                CambiarEstado(devExt,10,FALSE);
                CambiarEstado(devExt,6,TRUE);
				EnviarCodigo(devExt,46,inData,pool,6);
			} else {
				if((devExt->stSeta1&2)) {
                    CambiarEstado(devExt,12,FALSE);
                    CambiarEstado(devExt,7,TRUE);
					EnviarCodigo(devExt,48,inData,pool,7);
				} else {
                    CambiarEstado(devExt,8,FALSE);
                    EnviarCodigo(devExt,44,inData,pool,255);
				}
			}
			break;
		case 6:
			devExt->stSeta1=devExt->stSeta1|8;
			if((devExt->stSeta1&1)) {
                CambiarEstado(devExt,6,FALSE);
                CambiarEstado(devExt,11,TRUE);
				EnviarCodigo(devExt,11,inData,pool,42);
			} else {
				if((devExt->stSeta1&2)) {
                    CambiarEstado(devExt,7,FALSE);
                    CambiarEstado(devExt,13,TRUE);
					EnviarCodigo(devExt,13,inData,pool,43);
				} else {
                    CambiarEstado(devExt,9,TRUE);
                    EnviarCodigo(devExt,9,inData,pool,255);
				}
			}
			break;
		case 70:
			devExt->stSeta1=devExt->stSeta1&247;
			if((devExt->stSeta1&1)) {
                CambiarEstado(devExt,11,FALSE);
                CambiarEstado(devExt,6,TRUE);
				EnviarCodigo(devExt,47,inData,pool,6);
			} else {
				if((devExt->stSeta1&2)) {
                    CambiarEstado(devExt,13,FALSE);
                    CambiarEstado(devExt,7,TRUE);
					EnviarCodigo(devExt,49,inData,pool,7);
				} else {
                    CambiarEstado(devExt,9,FALSE);
                    EnviarCodigo(devExt,45,inData,pool,255);
				}
			}
			break;
//----------------
//********** Seta 2
		case 9:
			devExt->stSeta2=devExt->stSeta2|16;
			if((devExt->stSeta2&64)) {
                CambiarEstado(devExt,16,FALSE);
                CambiarEstado(devExt,18,TRUE);
				EnviarCodigo(devExt,18,inData,pool,52);
			} else {
				if((devExt->stSeta2&128)) {
                    CambiarEstado(devExt,17,FALSE);
                    CambiarEstado(devExt,19,TRUE);
					EnviarCodigo(devExt,19,inData,pool,53);
				} else {
                    CambiarEstado(devExt,14,TRUE);
                    EnviarCodigo(devExt,14,inData,pool,255);
				}
			}
			break;
		case 73:
			devExt->stSeta2=devExt->stSeta2&239;
			if((devExt->stSeta2&64)) {
                CambiarEstado(devExt,18,FALSE);
                CambiarEstado(devExt,16,TRUE);
				EnviarCodigo(devExt,54,inData,pool,16);
			} else {
				if((devExt->stSeta2&128)) {
                    CambiarEstado(devExt,19,FALSE);
                    CambiarEstado(devExt,17,TRUE);
					EnviarCodigo(devExt,55,inData,pool,17);
				} else {
                    CambiarEstado(devExt,14,FALSE);
                    EnviarCodigo(devExt,50,inData,pool,255);
				}
			}
			break;
		case 11:
			devExt->stSeta2=devExt->stSeta2|32;
			if((devExt->stSeta2&64)) {
                CambiarEstado(devExt,16,FALSE);
                CambiarEstado(devExt,20,TRUE);
				EnviarCodigo(devExt,20,inData,pool,52);
			} else {
				if((devExt->stSeta2&128)) {
                    CambiarEstado(devExt,17,FALSE);
                    CambiarEstado(devExt,21,TRUE);
					EnviarCodigo(devExt,21,inData,pool,53);
				} else {
                    CambiarEstado(devExt,15,TRUE);
                    EnviarCodigo(devExt,15,inData,pool,255);
				}
			}
			break;
		case 75:
			devExt->stSeta2=devExt->stSeta2&223;
			if((devExt->stSeta2&64)) {
                CambiarEstado(devExt,20,FALSE);
                CambiarEstado(devExt,16,TRUE);
				EnviarCodigo(devExt,56,inData,pool,16);
			} else {
				if((devExt->stSeta2&128)) {
                    CambiarEstado(devExt,21,FALSE);
                    CambiarEstado(devExt,17,TRUE);
					EnviarCodigo(devExt,57,inData,pool,17);
				} else {
                    CambiarEstado(devExt,15,FALSE);
                    EnviarCodigo(devExt,51,inData,pool,255);
				}
			}
			break;
		case 12:
			devExt->stSeta2=devExt->stSeta2|64;
			if((devExt->stSeta2&16)) {
                CambiarEstado(devExt,14,FALSE);
                CambiarEstado(devExt,18,TRUE);
				EnviarCodigo(devExt,18,inData,pool,50);
			} else {
				if((devExt->stSeta2&32)) {
                    CambiarEstado(devExt,15,FALSE);
                    CambiarEstado(devExt,20,TRUE);
					EnviarCodigo(devExt,20,inData,pool,51);
				} else {
                    CambiarEstado(devExt,16,TRUE);
                    EnviarCodigo(devExt,16,inData,pool,255);
				}
			}
			break;
		case 76:
			devExt->stSeta2=devExt->stSeta2&191;
			if((devExt->stSeta2&16)) {
                CambiarEstado(devExt,18,FALSE);
                CambiarEstado(devExt,14,TRUE);
				EnviarCodigo(devExt,54,inData,pool,14);
			} else {
				if((devExt->stSeta2&32)) {
                    CambiarEstado(devExt,20,FALSE);
                    CambiarEstado(devExt,15,TRUE);
					EnviarCodigo(devExt,56,inData,pool,15);
				} else {
                    CambiarEstado(devExt,16,FALSE);
                    EnviarCodigo(devExt,52,inData,pool,255);
				}
			}
			break;
		case 10:
			devExt->stSeta2=devExt->stSeta2|128;
			if((devExt->stSeta2&16)) {
                CambiarEstado(devExt,14,FALSE);
                CambiarEstado(devExt,19,TRUE);
				EnviarCodigo(devExt,19,inData,pool,50);
			} else {
				if((devExt->stSeta2&32)) {
                    CambiarEstado(devExt,15,FALSE);
                    CambiarEstado(devExt,21,TRUE);
					EnviarCodigo(devExt,21,inData,pool,51);
				} else {
                    CambiarEstado(devExt,17,TRUE);
                    EnviarCodigo(devExt,17,inData,pool,255);
				}
			}
			break;
		case 74:
			devExt->stSeta2=devExt->stSeta2&127;
			if((devExt->stSeta2&16)) {
                CambiarEstado(devExt,19,FALSE);
                CambiarEstado(devExt,14,TRUE);
				EnviarCodigo(devExt,55,inData,pool,14);
			} else {
				if((devExt->stSeta2&32)) {
                    CambiarEstado(devExt,21,FALSE);
                    CambiarEstado(devExt,15,TRUE);
					EnviarCodigo(devExt,57,inData,pool,15);
				} else {
                    CambiarEstado(devExt,17,FALSE);
                    EnviarCodigo(devExt,53,inData,pool,255);
				}
			}
			break;
//----------------
//******************** X35 ********************
//------------ Boton d
		case 15:
            CambiarEstado(devExt,22,TRUE);
            EnviarCodigo(devExt,22,inData,pool,255);
			break;
		case 79:
            CambiarEstado(devExt,22,FALSE);
            EnviarCodigo(devExt,58,inData,pool,255);
			break;
//----------- Boton Raton
		case 16:
            CambiarEstado(devExt,23,TRUE);
            EnviarCodigo(devExt,23,inData,pool,255);
			break;
		case 80:
            CambiarEstado(devExt,23,FALSE);
            EnviarCodigo(devExt,59,inData,pool,255);
			break;
//----------- Modo Izquierda
		case 32:
			if (!devExt->modo) {
                CambiarEstado(devExt,24,TRUE);
                EnviarCodigo(devExt,24,inData,pool,255);
			} else {
				devExt->stModo=0;
				devExt->gPosAnt[0]=11;
				devExt->gPosAnt[1]=11;
				devExt->gPosAnt[2]=11;
				devExt->gPosAnt[3]=11;
			}
			break;
		case 96:
			if (!devExt->modo) {
                CambiarEstado(devExt,24,FALSE);
                EnviarCodigo(devExt,60,inData,pool,255);
			} else {
				devExt->stModo=2;
				devExt->gPosAnt[0]=11;
				devExt->gPosAnt[1]=11;
				devExt->gPosAnt[2]=11;
				devExt->gPosAnt[3]=11;
			}
			break;
//----------- Modo Derecha
		case 31:
			if (!devExt->modo) {
                CambiarEstado(devExt,25,TRUE);
                EnviarCodigo(devExt,25,inData,pool,255);
			} else {
				devExt->stModo=4;
				devExt->gPosAnt[0]=11;
				devExt->gPosAnt[1]=11;
				devExt->gPosAnt[2]=11;
				devExt->gPosAnt[3]=11;
			}
			break;
		case 95:
			if (!devExt->modo) {
                CambiarEstado(devExt,25,FALSE);
                EnviarCodigo(devExt,61,inData,pool,255);
			} else {
				devExt->stModo=2;
				devExt->gPosAnt[0]=11;
				devExt->gPosAnt[1]=11;
				devExt->gPosAnt[2]=11;
				devExt->gPosAnt[3]=11;
			}
			break;
//----------- Aux Izquierda
		case 30:
            CambiarEstado(devExt,26,TRUE);
            EnviarCodigo(devExt,26,inData,pool,255);
			break;
		case 94:
            CambiarEstado(devExt,26,FALSE);
            EnviarCodigo(devExt,62,inData,pool,255);
			break;
//----------- Aux Derecha
		case 29:
            CambiarEstado(devExt,27,TRUE);
            EnviarCodigo(devExt,27,inData,pool,255);
			break;
		case 93:
            CambiarEstado(devExt,27,FALSE);
            EnviarCodigo(devExt,63,inData,pool,255);
			break;
//*********** Seta Raton
		case 21:
            CambiarEstado(devExt,28,TRUE);
            EnviarCodigo(devExt,28,inData,pool,255);
			break;
		case 85:
            CambiarEstado(devExt,28,FALSE);
            EnviarCodigo(devExt,64,inData,pool,255);
			break;
		case 23:
            CambiarEstado(devExt,29,TRUE);
            EnviarCodigo(devExt,29,inData,pool,255);
			break;
		case 87:
            CambiarEstado(devExt,29,FALSE);
            EnviarCodigo(devExt,65,inData,pool,255);
			break;
		case 24:
            CambiarEstado(devExt,30,TRUE);
            EnviarCodigo(devExt,30,inData,pool,255);
			break;
		case 88:
            CambiarEstado(devExt,30,FALSE);
            EnviarCodigo(devExt,66,inData,pool,255);
			break;
		case 22:
            CambiarEstado(devExt,31,TRUE);
            EnviarCodigo(devExt,31,inData,pool,255);
			break;
		case 86:
            CambiarEstado(devExt,31,FALSE);
            EnviarCodigo(devExt,67,inData,pool,255);
			break;
//-------------
//********** Seta 3
		case 17:
            CambiarEstado(devExt,32,TRUE);
            EnviarCodigo(devExt,32,inData,pool,255);
			break;
		case 81:
            CambiarEstado(devExt,32,FALSE);
            EnviarCodigo(devExt,68,inData,pool,255);
			break;
		case 19:
            CambiarEstado(devExt,33,TRUE);
            EnviarCodigo(devExt,33,inData,pool,255);
			break;
		case 83:
            CambiarEstado(devExt,33,FALSE);
            EnviarCodigo(devExt,69,inData,pool,255);
			break;
		case 20:
            CambiarEstado(devExt,34,TRUE);
            EnviarCodigo(devExt,34,inData,pool,255);
			break;
		case 84:
            CambiarEstado(devExt,34,FALSE);
            EnviarCodigo(devExt,70,inData,pool,255);
			break;
		case 18:
            CambiarEstado(devExt,35,TRUE);
            EnviarCodigo(devExt,35,inData,pool,255);
			break;
		case 82:
            CambiarEstado(devExt,35,FALSE);
            EnviarCodigo(devExt,71,inData,pool,255);
//------------
	}
}



void TraducirGiratorio(PX36DEVICE_EXTENSION devExt,PKEYBOARD_INPUT_DATA inData,PPOOL pool)
{
    BOOLEAN incremental=FALSE;
	UCHAR posiciones=0;
	UCHAR m=devExt->stModo+devExt->stPinkie;
	UCHAR girNuevo=((UCHAR)devExt->gPosicion-48)+(((UCHAR)inData->MakeCode-48)*16);

	if (devExt->confRot[devExt->gEje][m]==0) {
		return;
	} else {
		posiciones=(UCHAR)(devExt->confRot[devExt->gEje][m]&0x1F);
        incremental=((devExt->confRot[devExt->gEje][m]&0x20)==0x20) ? TRUE : FALSE;
	}

	if (incremental) {
		if (girNuevo>devExt->gGirAnt[devExt->gEje]) {
			if(devExt->gGirAnt[devExt->gEje]<(255-posiciones)) {
				if (girNuevo>(devExt->gGirAnt[devExt->gEje]+posiciones)) {
					switch(devExt->gEje) {
						case 0 : EnviarCodigo(devExt,72,inData,pool,255); break;
                        case 1 : EnviarCodigo(devExt,78,inData,pool,255); break;
                        case 2 : EnviarCodigo(devExt,84,inData,pool,255); break;
                        case 3 : EnviarCodigo(devExt,94,inData,pool,255);
					}
					devExt->gGirAnt[devExt->gEje]=girNuevo;
				}
			}
		} else {
			if(devExt->gGirAnt[devExt->gEje]>posiciones) {
				if (girNuevo<(devExt->gGirAnt[devExt->gEje]-posiciones)) {
					switch(devExt->gEje) {
						case 0 : EnviarCodigo(devExt,73,inData,pool,255); break;
                        case 1 : EnviarCodigo(devExt,79,inData,pool,255); break;
                        case 2 : EnviarCodigo(devExt,85,inData,pool,255); break;
                        case 3 : EnviarCodigo(devExt,95,inData,pool,255);
					}
					devExt->gGirAnt[devExt->gEje]=girNuevo;
				}
			}
		}
	} else {
		UCHAR posActual=girNuevo/(256/(posiciones));
		if(posActual!=devExt->gPosAnt[devExt->gEje]) {
			devExt->gPosAnt[devExt->gEje]=posActual;
			switch(devExt->gEje) {
				case 0 : EnviarCodigo(devExt,72+posActual,inData,pool,255); break;
                case 1 : EnviarCodigo(devExt,78+posActual,inData,pool,255); break;
                case 2 : EnviarCodigo(devExt,84+posActual,inData,pool,255); break;
                case 3 : EnviarCodigo(devExt,94+posActual,inData,pool,255);
			}
		}
	}
}
