/*****************************************************************************
	feram_i2c.h(For application of FeRAM)
	
	PURPOSE:	Self-sufficient header file holding all the macros, prototypes,
				and functions for I2C FeRAM products similar to PN MR44V064A in 
				order to be included within the "main.c" folder. These format the
				data according to the features of the MR44V064A 
				(such as Page Write Cycle) in order to be processed by the I2C
				functions ("i2c.h" & "i2c.c").

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
#include "i2c.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== For FeRAM MR44V064A ===*/
#define FERAM_SLAVE_ADDRESS		0x57         // Use must manually change the FeRAM address in order to be sent the correct device
#define FERAM_PAGE_SIZE			8192

/*=== Internal state ===*/
#define ST_STOP						(0)
#define ST_I2C_SEND_START			(1)
#define ST_I2C_SEND_EXEC			(2)
#define ST_I2C_RECEIVE_START		(3)
#define ST_I2C_RECEIVE_EXEC			(4)
#define ST_FERAM_WRITE_START		(5)
#define ST_FERAM_WRITE_EXEC			(6)

#define min(a,b)	(((a) < (b)) ? (a) : (b))

/*=== Return value. ===*/
#define FERAM_R_OK					(0)   /*  Data write / read start processing succeeded */
#define FERAM_R_NG					(-1)

#define FERAM_R_PROCESS				(1)
#define FERAM_R_SUCCESS				(0)
#define FERAM_R_ERROR				(-1)

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
void feram_init(void);
int feram_write(unsigned int address, unsigned char *data, unsigned int size);
int feram_read(unsigned int address, unsigned char *data, unsigned int size);
int feram_continue(void);
int eeprom_getStatus(void);
void feram_stop(void);

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
		
} 

/*******************************************************************************
	Routine Name:	feram_write
	Form:			int feram_write(unsigned int address, unsigned char *data, unsigned int size)
	Parameters:		unsigned int address : Address where writing is started
					unsigned char *data : Pointer in area where data is stored
					unsigned int size : Size of write data(unit is byte)
	Return value:	int
						FERAM_R_OK(=0)		: OK
						FERAM_R_NG(=-1)		: State error
	Description:	This function begins to write data to FERAM.
******************************************************************************/
int feram_write(unsigned int address, unsigned char *data, unsigned int size)
{
	if(_gsCtrlParam.internal_status != ST_STOP){
		return(FERAM_R_NG);
	}
	_gsCtrlParam.address[0] = (unsigned char)( address >> 8 );
	_gsCtrlParam.address[1] = (unsigned char)( address & 0x00FF );
	_gsCtrlParam.data = data;
	_gsCtrlParam.remain_size = size;
	_gsCtrlParam.total_size = 0;
	/* The size of writing is divided into page size or less. */
	_gsCtrlParam.proc_size = FERAM_PAGE_SIZE - (address % FERAM_PAGE_SIZE);// page max size
	_gsCtrlParam.proc_size = (unsigned int)min(_gsCtrlParam.proc_size, _gsCtrlParam.remain_size);
	_gsCtrlParam.result = FERAM_R_PROCESS;
	_gsCtrlParam.internal_status = ST_I2C_SEND_START;
	return FERAM_R_OK;
}


/*******************************************************************************
	Routine Name:	feram_read
	Form:			int feram_read(unsigned int address, unsigned char *data, unsigned int size);
	Parameters:		unsigned int address : Address where reading is started
					unsigned char *data : Pointer to area where data is stored
					unsigned int size : Size of read data(unit is byte)
	Return value:	int
						FERAM_R_OK(=0)		: OK
						FERAM_R_NG(=-1)		: State error
	Description:	This function begins to read data from FERAM.
******************************************************************************/
int feram_read(unsigned int address, unsigned char *data, unsigned int size)
{
	if(_gsCtrlParam.internal_status != ST_STOP){
		return(FERAM_R_NG);
	}
	_gsCtrlParam.address[0] = (unsigned char)( address >> 8 );
	_gsCtrlParam.address[1] = (unsigned char)( address & 0x00FF );
	_gsCtrlParam.data = data;
	_gsCtrlParam.remain_size = size;
	_gsCtrlParam.total_size = 0;
	_gsCtrlParam.proc_size = size;
	_gsCtrlParam.result = FERAM_R_PROCESS;
	_gsCtrlParam.internal_status = ST_I2C_RECEIVE_START;
	return FERAM_R_OK;
}

/*******************************************************************************
	Routine Name:	feram_continue
	Form:			int feram_continue(void)
	Parameters:		none
	Return value:	status
						FERAM_R_PROCESS(1)
						FERAM_R_SUCCESS(0)
						FERAM_R_ERROR(-1)
	Description:	Continuous processing of FERAM operation
******************************************************************************/
int feram_continue(void)
{
	int err = I2C_R_TRANS_START_OK;

	switch( _gsCtrlParam.internal_status ){
		case ST_I2C_SEND_START:
			if( _gsCtrlParam.remain_size != 0 ){
				_gsCtrlParam.internal_status = ST_I2C_SEND_EXEC;
				err = i2c_startSend((unsigned char)FERAM_SLAVE_ADDRESS, 
							&_gsCtrlParam.address[0], (unsigned int)2,
							_gsCtrlParam.data, _gsCtrlParam.proc_size,
							(cbfI2c)_i2cFin);
			}
			else{
				_gsCtrlParam.result = FERAM_R_SUCCESS;
				_gsCtrlParam.internal_status = ST_STOP;
			}
			break;

		case ST_FERAM_WRITE_START:
			_gsCtrlParam.internal_status = ST_FERAM_WRITE_EXEC;
			err = i2c_startReceive((unsigned char)FERAM_SLAVE_ADDRESS, 
						&_gsCtrlParam.address[0], (unsigned int)2,
						(unsigned char*)0, (unsigned int)0,
						(cbfI2c)_i2cFin);
			break;

		case ST_I2C_RECEIVE_START:
			_gsCtrlParam.internal_status = ST_I2C_RECEIVE_EXEC;
			err = i2c_startReceive((unsigned char)FERAM_SLAVE_ADDRESS, 
						&_gsCtrlParam.address[0], (unsigned int)2,
						_gsCtrlParam.data, _gsCtrlParam.proc_size,
						(cbfI2c)_i2cFin);
			break;

		case ST_STOP:
		case ST_I2C_SEND_EXEC:
		case ST_I2C_RECEIVE_EXEC:
		case ST_FERAM_WRITE_EXEC:
		default:
			break;
	}
	if(err != I2C_R_TRANS_START_OK){
		_gsCtrlParam.result = FERAM_R_ERROR;
		_gsCtrlParam.internal_status = ST_STOP;
	}
	return(_gsCtrlParam.result);
}

/*******************************************************************************
	Routine Name:	feram_stop
	Form:			void feram_stop( void )
	Parameters:		none
	Return value:	none
	Description:	Stop processing of FERAM operation
******************************************************************************/
void feram_stop( void )
{
	i2c_stop();
	/*=== Initialization of control information ===*/
	_gsCtrlParam.address[0] = 0x00;
	_gsCtrlParam.address[1] = 0x00;
	_gsCtrlParam.data = (void *)0;
	_gsCtrlParam.remain_size = 0;
	_gsCtrlParam.total_size = 0;
	_gsCtrlParam.proc_size = 0;
	_gsCtrlParam.result = FERAM_R_SUCCESS;
	_gsCtrlParam.internal_status = ST_STOP;
}

/*******************************************************************************
	Routine Name:	feram_getStatus
	Form:			int feram_getStatus(void)
	Parameters:		none
	Return value:	status
						FERAM_R_PROCESS(= 1)
						FERAM_R_SUCCESS(= 0)
						FERAM_R_ERROR(= -1)
	Description:	get status
******************************************************************************/
int feram_getStatus(void)
{
	return _gsCtrlParam.result;
}


/*############################################################################*/
/*#                              Subroutine                                  #*/
/*############################################################################*/
/*******************************************************************************
	Routine Name:	_i2cFin
	Form:			static void _i2cFin( unsigned int size, unsigned char errStat )
	Parameters:		none
	Return value:	none
	Description:	i2c handler.
******************************************************************************/
static void _i2cFin( unsigned int size, unsigned char errStat )
{
	unsigned int address;

	switch( _gsCtrlParam.internal_status ){
		case ST_I2C_SEND_EXEC:
			if(errStat == I2C_R_OK){
				_gsCtrlParam.total_size += size;
				_gsCtrlParam.remain_size -= _gsCtrlParam.proc_size;
				_gsCtrlParam.data += _gsCtrlParam.proc_size;
				address = (((unsigned int)_gsCtrlParam.address[0]<<8) | ((unsigned int)_gsCtrlParam.address[1]));
				address += _gsCtrlParam.proc_size;
				_gsCtrlParam.address[0] = (unsigned char)( address >> 8 );
				_gsCtrlParam.address[1] = (unsigned char)( address & 0x00FF );
				_gsCtrlParam.proc_size = min((unsigned int)FERAM_PAGE_SIZE, _gsCtrlParam.remain_size);
				_gsCtrlParam.internal_status = ST_FERAM_WRITE_START;
			}
			else{
				_gsCtrlParam.result = FERAM_R_ERROR;
				_gsCtrlParam.internal_status = ST_STOP;
			}
			break;

		case ST_I2C_RECEIVE_EXEC:
			if(errStat == I2C_R_OK){
				_gsCtrlParam.total_size += size;
				_gsCtrlParam.remain_size -= _gsCtrlParam.proc_size;
				_gsCtrlParam.data += _gsCtrlParam.proc_size;
				address = (((unsigned int)_gsCtrlParam.address[0]<<8) | ((unsigned int)_gsCtrlParam.address[1]));
				address += _gsCtrlParam.proc_size;
				_gsCtrlParam.address[0] = (unsigned char)( address >> 8 );
				_gsCtrlParam.address[1] = (unsigned char)( address & 0x00FF );
				_gsCtrlParam.proc_size = min((unsigned int)FERAM_PAGE_SIZE, _gsCtrlParam.remain_size);
				_gsCtrlParam.result = FERAM_R_SUCCESS;
			}
			else{
				_gsCtrlParam.result = FERAM_R_ERROR;
			}
			_gsCtrlParam.internal_status = ST_STOP;
			break;

		case ST_FERAM_WRITE_EXEC:
			if(errStat == 0){
				_gsCtrlParam.internal_status = ST_I2C_SEND_START;
			}
			else{
				_gsCtrlParam.internal_status = ST_FERAM_WRITE_START;
			}
			break;

		case ST_I2C_SEND_START:
		case ST_FERAM_WRITE_START:
		case ST_I2C_RECEIVE_START:
		case ST_STOP:
		default:
			break;
	}
}