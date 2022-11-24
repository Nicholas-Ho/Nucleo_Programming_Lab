// Note: The environment has not been set up for use with ARM libraries/interfacing with microcontrollers.
// Errors are expected. When complete, copy the code to Kiel Cloud Studio for debugging.

// Include the mbed.h library for interfacing with the microprocessor
#include "mbed.h"

Serial pc(SERIAL_TX, SERIAL_RX);


float f_1(float x)
{
        return(x-x*x);
}

float first_derivative( float (*f)(float), float x)
{
        float h = 0.001;
        float dfdx;
        dfdx = ( f(x+h) - f(x-h) ) / (2*h);
        return dfdx;
}

float second_derivative( float (*f)(float), float x)
{
        float h = 0.001;
        float d2fdx2;
        d2fdx2 = ( f(x-h) - 2 * f(x) + f(x+h) ) / (h*h);
        return d2fdx2;
}


int main() {
        pc.baud(9600);
        float x=1.;

        pc.printf("Function Pointer test program. \r\n");

        pc.printf("Function value: f(%f)=%f \r\n", x, f_1(x));

        pc.printf("First derivative: f(%f)=%f \r\n", first_derivative(f_1, x));

        pc.printf("Second derivative: %f \r\n", second_derivative(f_1, x));
}