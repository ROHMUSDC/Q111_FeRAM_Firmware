/*****************************************************************************
	feram.c(For application of FeRAM)

	Copyright (C) 2009 LAPIS Semiconductor Co., Ltd.
	All rights reserved.

	LAPIS Semiconductor shall not be liable for any direct, indirect, 
	consequential or incidental damages arising from using or modifying this 
	program.

    History
    2009.08.24  ver.1.00
    2012.11.09  ver.1.01 modified to add preprocessor for MCU without I2C module
	2014.07.25	ver.1.02 modified to make compatible to LAPIS FeRAM Development Board
******************************************************************************/
#if _FERAM_IF_TYPE == 1
	#ifndef _NO_I2C_MODE
		#include "feram_i2c.c"
	#endif	
#elif _FERAM_IF_TYPE == 2
	#include "feram_spi.c"
#else
	#error "The compilation switch "_FERAM_IF_TYPE" is illegal setting in FERAM module."
#endif
