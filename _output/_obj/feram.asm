;; Compile Options : /TML610111 /MS /near /LE /Ii2c /Iirq /Imain /Itbc /Itimer /Iuart /Icommon /Imath /Istdio /Istdlib /Istring /Iyvals /CT _output\_prn\feram.cal /SS 256 /SD /Oa /Ot /W 3 /Zg /Fa_output\_obj\ /Lv /Zs 
;; Version Number  : Ver.3.41.8
;; File Name       : feram.c

	type (ML610111) 
	model small, near
	$$NINITVAR segment data 2h #0h
	$$NINITTAB segment table 2h any
	$$feram_init$feram segment code 2h #0h
CVERSION 3.41.8
CGLOBAL 01H 03H 0000H "feram_init" 08H 02H 09H 00H 81H 04H 00H 00H 07H
CSTRUCTTAG 0000H 0000H 0001H 0007H 0000000EH "_Notag"
CSTRUCTMEM 42H 00000002H 00000000H "address" 05H 01H 02H 00H 00H 00H
CSTRUCTMEM 42H 00000002H 00000002H "data" 04H 03H 00H 00H 00H
CSTRUCTMEM 42H 00000002H 00000004H "remain_size" 02H 00H 01H
CSTRUCTMEM 42H 00000002H 00000006H "total_size" 02H 00H 01H
CSTRUCTMEM 42H 00000002H 00000008H "proc_size" 02H 00H 01H
CSTRUCTMEM 43H 00000002H 0000000AH "result" 02H 00H 01H
CSTRUCTMEM 43H 00000002H 0000000CH "internal_status" 02H 00H 01H
CSTRUCTTAG 0000H 0000H 0000H 0008H 00000001H "_Notag"
CSTRUCTMEM 52H 00000001H 00000000H "b0" 02H 00H 00H
CSTRUCTMEM 52H 00000001H 00000001H "b1" 02H 00H 00H
CSTRUCTMEM 52H 00000001H 00000002H "b2" 02H 00H 00H
CSTRUCTMEM 52H 00000001H 00000003H "b3" 02H 00H 00H
CSTRUCTMEM 52H 00000001H 00000004H "b4" 02H 00H 00H
CSTRUCTMEM 52H 00000001H 00000005H "b5" 02H 00H 00H
CSTRUCTMEM 52H 00000001H 00000006H "b6" 02H 00H 00H
CSTRUCTMEM 52H 00000001H 00000007H "b7" 02H 00H 00H
CTYPEDEF 0000H 0000H 43H "tFERAMCtrlParam" 04H 00H 05H 01H 00H
CTYPEDEF 0000H 0000H 43H "cbfI2c" 0AH 03H 00H 02H 00H 00H 00H 00H 00H 00H 07H
CTYPEDEF 0000H 0000H 43H "_BYTE_FIELD" 04H 00H 05H 00H 00H
CSGLOBAL 43H 000EH "_gsCtrlParam" 04H 00H 05H 01H 00H
CFILE 0001H 00000024H "main\\mcu.h"
CFILE 0002H 000007EEH "main\\ML610111.H"
CFILE 0003H 00000045H "i2c\\i2c.h"
CFILE 0004H 0000002BH "main\\feram.h"
CFILE 0000H 0000005DH "feram\\feram.c"

	rseg $$feram_init$feram
CFUNCTION 9

_feram_init	:
CBLOCK 9 1 76

;;{
CLINEA 0000H 0001H 004CH 0001H 0001H
;;*****************************************
;;	register/stack information
;;*****************************************
;;*****************************************

	push	lr
CBLOCK 9 2 76
CRET 0000H

;;	_gsCtrlParam.address[0] = 0x00;
CLINEA 0000H 0001H 0051H 0002H 0020H
	mov	r0,	#00h
	st	r0,	NEAR __gsCtrlParam

;;	_gsCtrlParam.address[1] = 0x00;
CLINEA 0000H 0001H 0052H 0002H 0020H
	st	r0,	NEAR __gsCtrlParam+01h

;;	_gsCtrlParam.data = (void *)0;
CLINEA 0000H 0001H 0053H 0002H 001FH
	mov	er0,	#0 
	st	er0,	NEAR __gsCtrlParam+02h

;;	_gsCtrlParam.remain_size = 0;
CLINEA 0000H 0001H 0054H 0002H 001EH
	st	er0,	NEAR __gsCtrlParam+04h

;;	_gsCtrlParam.total_size = 0;
CLINEA 0000H 0001H 0055H 0002H 001DH
	st	er0,	NEAR __gsCtrlParam+06h

;;	_gsCtrlParam.proc_size = 0;
CLINEA 0000H 0001H 0056H 0002H 001CH
	st	er0,	NEAR __gsCtrlParam+08h

;;	_gsCtrlParam.result = FERAM_R_SUCCESS;
CLINEA 0000H 0001H 0057H 0002H 0027H
	st	er0,	NEAR __gsCtrlParam+0ah

;;	_gsCtrlParam.internal_status = ST_STOP;
CLINEA 0000H 0001H 0058H 0002H 0028H
	st	er0,	NEAR __gsCtrlParam+0ch

;;	(void)i2c_init(I2C_MOD_FST, 500, I2C_SYN_OFF); //Question// How is 500 kHz the chosen value?
CLINEA 0000H 0001H 005BH 0002H 005DH
	push	r0
	mov	r2,	#0f4h
	mov	r3,	#01h
	mov	r0,	#01h
	bl	_i2c_init
	add	sp,	#2 
CBLOCKEND 9 2 93

;;} // NOT TESTED!!!!
CLINEA 0000H 0001H 005DH 0001H 0013H
	pop	pc
CBLOCKEND 9 1 93
CFUNCTIONEND 9

	public _feram_init
	extrn code near : _i2c_init
	extrn code near : _main

	rseg $$NINITTAB
	db	00h
	db	00h
	dw	00h
	dw	00h
	dw	00h
	dw	00h
	dw	00h
	dw	00h

	rseg $$NINITVAR
__gsCtrlParam :
	ds	0eh

	end
