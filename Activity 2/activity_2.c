#include "mbed.h"
#include "stdint.h" //This allows the use of integers of a known width
#include <string.h> //This allows the use of strcpy and strcat
#define LM75_REG_TEMP (0x00) // Temperature Register
#define LM75_REG_CONF (0x01) // Configuration Register
#define LM75_ADDR     (0x90) // LM75 address

#define LM75_REG_TOS (0x03) // TOS Register
#define LM75_REG_THYST (0x02) // THYST Register



I2C i2c(I2C_SDA, I2C_SCL);

// Don't actually know the order, should check
DigitalOut red(LED1); // Error indicator
DigitalOut blue(LED2); // Interrupt indicator
DigitalOut green(LED3); // Logging indicator

InterruptIn lm75_int(D7); // Make sure you have the OS line connected to D7

Serial pc(SERIAL_TX, SERIAL_RX);

int16_t i16;

int interrupt_interval = 200000; // In microseconds
int repeat_interval = 1000000; // In microseconds


// CIRCULAR BUFFER (variables and function predeclerations)
// Ported over from C++ project, could be cleaned up with a struct

#define ARRAY_SIZE 100

float buffer[ARRAY_SIZE];
int firstIndex = 0; // Index of first element
int lastIndex = ARRAY_SIZE-1; // Index of last element. To be overwritten.
int bufferLength = 0; // Buffer length. When length = size, buffer is full.
int size = -1; // Size of buffer. To be overwritten

void initialiseBuffer(int bufferSize, int initialise); // initialise is a bool, and should only be 1 or 0
void bufferAdd(float contents, int popIfFull);
int bufferPop();


// EMBEDDED LOGIC

// Alarm!
void displayAlarm() {
    // Generate string
    char displayChars[400];
    strcpy(displayChars, "Temperatures: ")

    float temp = bufferPop();
    while(temp != -1) {
        strcat(displayChars, temp);
        strcat(displayChars, " ")
        temp = bufferPop();
    }

    // Display string
    pc.printf(displayChars);

    // LED indicator
    while(1) {
        blue = !blue;
        wait_us(interrupt_interval);
    }

}

int main()
{
    // Initialise buffer
    initialiseBuffer(60, 0);

    char data_write[3];
    char data_read[3];

    /* Configure the Temperature sensor device STLM75:
        - Thermostat mode Interrupt
        - Fault tolerance: 0
        - Interrupt mode means that the line will trigger when you exceed TOS and stay triggered until a register is read - see data sheet
    */
    data_write[0] = LM75_REG_CONF;
    data_write[1] = 0x02;
    int status = i2c.write(LM75_ADDR, data_write, 2, 0);
    if(status != 0)
    { // Error
        while(1)
        {
            red = !red;
            wait_us(interrupt_interval));
        }
    }

    float tos=28; // TOS temperature
    float thyst=26; // THYST tempertuare

    // This section of code sets the TOS register
    data_write[0]=LM75_REG_TOS;
    i16 = (int16_t)(tos*256) & 0xFF80;
    data_write[1]=(i16 >> 8) & 0xff;
    data_write[2]=i16 & 0xff;
    i2c.write(LM75_ADDR, data_write, 3, 0);

    //This section of codes set the THYST register
    data_write[0]=LM75_REG_THYST;
    i16 = (int16_t)(thyst*256) & 0xFF80;
    data_write[1]=(i16 >> 8) & 0xff;
    data_write[2]=i16 & 0xff;
    i2c.write(LM75_ADDR, data_write, 3, 0);

    // This line attaches the interrupt.
    // The interrupt line is active low so we trigger on a falling edge
    lm75_int.fall(&displayAlarm);

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

        // Log result
        bufferAdd(temp, 1)
        green = !green;
        wait_us(repeat_interval);
    }

}


// CIRCULAR BUFFER (logic)

void initialiseBuffer(int bufferSize, int initialise) {
    // bufferSize: Size of buffer. Cannot be more than array size
    // initialise: If true, initialise a full buffer with initValue
    // initValue: Value to initialise with. Does nothing if initialise=false

    // Ensure that size is not larger than array
    size = (bufferSize <= ARRAY_SIZE) ? bufferSize : ARRAY_SIZE;

    for(int i=0; i < (ARRAY_SIZE); i++) {
        buffer[i] = 0;
    }

    if(initialise != 0) {
        lastIndex = size-1;
        bufferLength = size;
    }
}

// Add to buffer
void bufferAdd(float contents, int popIfFull) { // popIfFull is a bool, it should only be 1 or 0
    if(bufferLength == 0) {
        lastIndex = firstIndex;
        buffer[lastIndex] = contents;
        bufferLength++;
        return;
    }

    if(bufferLength < size) {
        lastIndex = (lastIndex + 1) % ARRAY_SIZE;
        bufferLength++;
        buffer[lastIndex] = contents;
    } else {
        if(popIfFull == 0) {
            pc.printf("Queue full, contents not added.");
        } else {
            bufferPop();
            bufferAdd(contents, popIfFull);
        }
    }
}

// Pop from buffer
float bufferPop() {
    if(bufferLength > 0) {
        int output = buffer[firstIndex];
        firstIndex = (firstIndex + 1) % ARRAY_SIZE;
        bufferLength--;

        return output;
    } else {
        pc.printf("Queue empty, nothing to pop")
        return -1;
    }
}