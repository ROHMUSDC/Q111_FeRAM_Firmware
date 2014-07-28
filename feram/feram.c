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
#include "mcu.h"

//#ifndef _NO_I2C_MODE // do not compile if  mcu  does not support I2C
#include "i2c.h"
#include "feram.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/* For FeRAM MR44V064A */
#define FERAM_SLAVE_ADDRESS		0x57         // Don't know what to do with changing address
#define FERAM_PAGE_SIZE			8192

/* Internal state */
#define ST_STOP						(0)
#define ST_I2C_SEND_START			(1)
#define ST_I2C_SEND_EXEC			(2)
#define ST_I2C_RECEIVE_START		(3)
#define ST_I2C_RECEIVE_EXEC			(4)
#define ST_FERAM_WRITE_START		(5)
#define ST_FERAM_WRITE_EXEC			(6)

#define min(a,b)	(((a) < (b)) ? (a) : (b))


/*=== control parameter for FERAM ===*/
typedef struct {
	unsigned char	address[2];			/* Starting address of reading or writing	*/
	unsigned char*	data;				/* Pointer to area where data is stored		*/
	unsigned int	remain_size;		/* Remainder data size						*/
	unsigned int	total_size;			/* Present data size						*/
	unsigned int	proc_size;			/* Processing data size						*/
	int				result;				/* State of FERAM module					*/
	int				internal_status;	/* State of FERAM module internal state	*/
} tFERAMCtrlParam;

/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
/*=== control information of FERAM module ===*/
static tFERAMCtrlParam _gsCtrlParam = {{0x00,0x00}, (void *)0, 0, 0, 0, FERAM_R_SUCCESS, ST_STOP};


/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
static void _i2cFin( unsigned int size, unsigned char errStat );

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/

/*******************************************************************************
	Routine Name:	feram_init
	Form:			void feram_init( void )
	Parameters:		none
	Return value:	none
	Description:	Initialization of FERAM module
******************************************************************************/
void feram_init(void)
{
	/*=== Setting of port ===*/
	// Unable to control WP because the end user has total control of the Write Protect feature
	
	/*=== Initialization of control information ===*/
	_gsCtrlParam.address[0] = 0x00;
	_gsCtrlParam.address[1] = 0x00;
	_gsCtrlParam.data = (void *)0;
	_gsCtrlParam.remain_size = 0;
	_gsCtrlParam.total_size = 0;
	_gsCtrlParam.proc_size = 0;
	_gsCtrlParam.result = FERAM_R_SUCCESS;
	_gsCtrlParam.internal_status = ST_STOP;
	
	/*=== Initialization of I2C driver ===*/
	(void)i2c_init(I2C_MOD_FST, 500, I2C_SYN_OFF); //Question// How is 500 kHz the chosen value?
		
} // NOT TESTED!!!!