#include "mbed.h"
#include "stdint.h" //This allow the use of integers of a known width

#define LM75_REG_TEMP (0x00) // Temperature Register
#define LM75_REG_CONF (0x01) // Configuration Register
#define LM75_ADDR     (0x90) // LM75 address

I2C i2c(I2C_SDA, I2C_SCL);

DigitalOut red(LED1);

Serial pc(SERIAL_TX, SERIAL_RX);

int main()
{

        char data_write[2];
        char data_read[2];

        /* Configure the Temperature sensor device STLM75:
        - Thermostat mode Interrupt
        - Fault tolerance: 0
        */
        data_write[0] = LM75_REG_CONF;
        data_write[1] = 0x02;
        int status = i2c.write(LM75_ADDR, data_write, 2, 0);
        if (status != 0) { // Error
                while (1)
                {
                        red = !red;
                        wait(0.2);
                }
        }

        while (1)
        {
                // Read temperature register
                data_write[0] = LM75_REG_TEMP;
                i2c.write(LM75_ADDR, data_write, 1, 1); // no stop
                i2c.read(LM75_ADDR, data_read, 2, 0);

                // Calculate temperature value in Celcius
                int16_t i16 = (data_read[0] << 8) | data_read[1];
                // Read data as twos complement integer so sign is correct
                float temp = i16 / 256.0;

                // Display result
                pc.printf("Temperature = %.3f\r\n",temp);
                red = !red;
                wait(1.0);
        }

}