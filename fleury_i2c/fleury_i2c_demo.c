/*
 * i2c_demo.c
 *
 * Created: 1/31/2012 3:30:01 PM
 *  Author: Josiah
 */ 
#ifdef FLEURY_I2C_DEMO

/* I2C clock in Hz */
#define SCL_CLOCK  100000L
#define F_CPU 8000000L

void main(void)
{
	i2c_init(I2C_CLOCK_SELECT(SCL_CLOCK,F_CPU))
	while(1)
	{
		
	}
	return;
}

#endif