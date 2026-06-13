#include "led_state_machine.h"
#include "gpio.h"
#include "register_macros.h"
#include "timx.h"
#include "usart.h"

stateMachine_t stateMachine;

// State transition matrix for the state machine.
// Each row defines a transition from a current state to a next state based on an event.
static stateTransMatrixRow_t LEDStateTransMatrix[] = {
    {ST_INIT, EV_BUTTON_PRESSED, ST_LED_SLOW},
    {ST_LED_OFF, EV_BUTTON_PRESSED, ST_LED_SLOW},
    {ST_LED_SLOW, EV_BUTTON_PRESSED, ST_LED_MEDIUM},
    {ST_LED_MEDIUM, EV_BUTTON_PRESSED, ST_LED_FAST},
    {ST_LED_FAST, EV_BUTTON_PRESSED, ST_LED_SOLID},
    {ST_LED_SOLID, EV_BUTTON_PRESSED, ST_LED_OFF}
};

// Array of function pointers corresponding to each state.
// Each function will be called when the state machine transitions to the corresponding state.
static stateFunctionRow_t LEDStateFunctionArray[] = {
    {"ST_INIT", &led_init},
    {"ST_LED_OFF", &led_off},
    {"ST_LED_SLOW", &led_slow},
    {"ST_LED_MEDIUM", &led_medium},
    {"ST_LED_FAST", &led_fast},
    {"ST_LED_SOLID", &led_solid},
};

// Function definitions for each state.
// These functions will be called when the state machine transitions to the corresponding state.
void led_init(void) {
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_off(void) {
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_slow(void) {
    TIM2->ARR = 1499;
    SET_BIT(TIM2->EGR, 0);
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_medium(void) {
    TIM2->ARR = 749;
    SET_BIT(TIM2->EGR, 0);
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_fast(void) {
    TIM2->ARR = 249;
    SET_BIT(TIM2->EGR, 0);
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_solid(void) {
    SET_BIT(GPIOA->ODR, 5);
}

// Function to run one iteration of the state machine based on the current state and an event.
void state_machine_run_iteration(event_t event) {
    for (int i = 0; i < sizeof(LEDStateTransMatrix)/sizeof(LEDStateTransMatrix[0]); i++) {
        if(LEDStateTransMatrix[i].currState == stateMachine.currState) {
            if((LEDStateTransMatrix[i].event == event )) {
                stateMachine.currState = LEDStateTransMatrix[i].nextState;

                (LEDStateFunctionArray[stateMachine.currState].func)();
                usart2_print("Current State: ");
                usart2_print(LEDStateFunctionArray[stateMachine.currState].name);
                usart2_print("\r\n");
                break;
            }
        }
    }
}

bool state_machine_is_blinking(void) {
    if (stateMachine.currState != ST_LED_OFF && stateMachine.currState != ST_LED_SOLID) {
            return true;
    }
    else {
        return false;
    }
}

// Function to initialize the state machine. Sets the initial state and performs any necessary setup.
void state_machine_init() {
    stateMachine.currState = ST_INIT;
    state_machine_run_iteration(EV_BUTTON_PRESSED);

    CLEAR_BIT(GPIOA->ODR, 5);
}