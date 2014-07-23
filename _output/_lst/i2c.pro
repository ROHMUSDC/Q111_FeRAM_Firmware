extern	int i2c_init(unsigned char mode,unsigned short kHz,unsigned char syn);
extern	int i2c_startSend(unsigned char slave_adr,unsigned char *addr,unsigned int addr_size,unsigned char *data,unsigned int data_size,void (*func)(unsigned int size,unsigned char errStat));
extern	int i2c_startReceive(unsigned char slave_adr,unsigned char *addr,unsigned int addr_size,unsigned char *data,unsigned int data_size,void (*func)(unsigned int size,unsigned char errStat));
extern	int i2c_continue(void);
extern	void i2c_stop(void);
extern	int i2c_checkIRQ(void);
extern	void i2c_clearIRQ(void);
extern	unsigned int i2c_getTransSize(void);
