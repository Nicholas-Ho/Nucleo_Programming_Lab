// Note: The environment has not been set up for use with ARM libraries/interfacing with microcontrollers.
// Errors are expected. When complete, copy the code to Kiel Cloud Studio for debugging.

// Include the mbed.h and mbed_thread.h libraries for interfacing with the microprocessor.
#include "mbed.h"
#include "platform/mbed_thread.h"

// For debugging
Serial pc(SERIAL_TX, SERIAL_RX);
// bool updated = false;

// Button Interrupt
InterruptIn button(USER_BUTTON);

// Green LED
DigitalOut led1(LED1);

// Blue LED
DigitalOut led2(LED2);

// Red LED
DigitalOut led3(LED3);


// Debouncing
Timeout debounce_timeout;
const float debounce_interval = 0.3;
void onDebounceComplete(void);

// Update cycle ticker
Ticker cycle_ticker;
const float cycle_interval = 1;

// Recorded array of sequences of size N
const int N = 5;
int sequence[N];
int set_seq_ix = 0; // Index of sequence to set

// Remember the last LED activated. Initialise with Green LED.
int current_led = 1;

// Display settings
const int sequence_interval = 500000; // In microseconds
void displaySequence(void);


// Activates LED depending on the integer supplied (only 1 to 3).
// Only used by displaySequence and hence does NOT change current LED.
void activateLEDs(int led) {
    led1 = (led == 1);
    led2 = (led == 2);
    led3 = (led == 3);
}

// Overloaded. Activates current LED.
void activateLEDs() {
    led1 = (current_led == 1);
    led2 = (current_led == 2);
    led3 = (current_led == 3);
}

// Cycles through the LEDs. Activated by ticker.
void updateLEDs() {

    activateLEDs();
    
    current_led = (current_led % 3) + 1; // Cycles from 1 to 3
}

// Interrupt callback (on button release)
void onButtonRelease() {

    // Debouncing
    button.rise(NULL);
    debounce_timeout.attach(onDebounceComplete, debounce_interval);

    sequence[set_seq_ix] = current_led;
    set_seq_ix++;
    if (set_seq_ix == N) {
        displaySequence();
    }

    // updated = true; // For debugging

}

// Debounce callback
void onDebounceComplete() {
    button.rise(onButtonRelease);
}

// Display recorded sequence
void displaySequence() {
    // In this case we want the microprocessor to stop other functions and only display the sequence.
    // So, the "less optimal" wait_us approach will be used.

    cycle_ticker.detach(); // Detach the LED updating cycle to prevent interference
    debounce_timeout.detach(); // Detach debounce timeout to ensure button is not enabled midway through sequence.
    button.rise(NULL); // Prevent pressing of button to prevent interference

    for (int i=0; i < N; i++) {
        activateLEDs(sequence[i]);
        wait_us(sequence_interval);
    }

    // Reset and restore
    for (int i=0; i < N; i++) {
        sequence[i] = NULL;
    }
    set_seq_ix = 0;
    current_led = 1;
    updateLEDs();
    cycle_ticker.attach(updateLEDs, cycle_interval);
    button.rise(onButtonRelease);

}

int main() {

    pc.baud(9600);
    pc.printf("Initialised. \r\n");

    button.rise(onButtonRelease);
    cycle_ticker.attach(updateLEDs, cycle_interval);

    // while(true) {
    //     if(updated == true) {
    //         pc.printf("%d %d %d %d %d \r\n", sequence[0], sequence[1], sequence[2], sequence[3], sequence[4]);
    //         updated = false;
    //     }
    // }

}