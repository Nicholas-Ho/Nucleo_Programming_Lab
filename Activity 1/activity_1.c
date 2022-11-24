// Note: The environment has not been set up for use with ARM libraries/interfacing with microcontrollers.
// Errors are expected. When complete, copy the code to Kiel Cloud Studio for debugging.

// Include the mbed.h and mbed_thread.h libraries for interfacing with the microprocessor.
#include "mbed.h"
#include "platform/mbed_thread.h"


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
const int sequence_interval = 500; // In milliseconds
void displaySequence(void);


// Activates LED depending on the integer supplied (only 1 to 3).
void activateLEDs() {
    led1 = (current_led == 1);
    led2 = (current_led == 2);
    led3 = (current_led == 3);
}

// Cycles through the LEDs. Activated by ticker.
void updateLEDs() {

    activateLEDs(current_led);
    
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

}

// Debounce callback
void onDebounceComplete() {
    button.rise(onButtonRelease)
}

// Display recorded sequence
void displaySequence() {
    // In this case we want the microprocessor to stop other functions and only display the sequence.
    // So, the "less optimal" thread_sleep approach will be used.

    cycle_ticker.detach(); // Detach the LED updating cycle to prevent interference

    for (int i=0; i < N, i++) {
        activateLEDs(sequence[i]);
        thread_sleep_for(sequence_interval)
    }

    // Reset and restore
    for (int i=0; i < N, i+=) {
        sequence[i] = NULL;
    }
    set_seq_ix = 0;
    current_led = 1;
    cycle_ticker.attach(updateLEDs, cycle_interval);

}

int main() {

    button.rise(onButtonRelease);
    cycle_ticker.attach(updateLEDs, cycle_interval);

}