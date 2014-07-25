/*****************************************************************************
	feram.h(For application of FeRAM)

	Copyright (C) 2008-2009 LAPIS Semiconductor Co., Ltd.
	All rights reserved.

	LAPIS Semiconductor shall not be liable for any direct, indirect, 
	consequential or incidental damages arising from using or modifying this 
	program.

    History
    2009.08.24  ver.1.00
	2014.07.25	ver.1.01 modified to make compatible to LAPIS FeRAM Development Board
******************************************************************************/

#ifndef _FERAM_H_
#define _FERAM_H_


/*##############\##############################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== Return value. ===*/
#define FERAM_R_OK					(0)   /*  Data write / read start processing succeeded */
#define FERAM_R_NG					(-1)

#define FERAM_R_PROCESS				(1)
#define FERAM_R_SUCCESS				(0)
#define FERAM_R_ERROR				(-1)

		
/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
void feram_init(void);
int feram_write(unsigned int address, unsigned char *data, unsigned int size);
int feram_read(unsigned int address, unsigned char *data, unsigned int size);
int feram_continue(void);
int eeprom_getStatus(void);
void feram_stop(void);

#endif /*_EEPROM_H_*/
