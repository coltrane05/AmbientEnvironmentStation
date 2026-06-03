#include "led_state_machine.h"
#include "gpio.h"
#include "register_macros.h"

static stateTransMatrixRow_t LEDStateTransMatrix[] = {
    {ST_INIT, EV_BUTTON_PRESSED, ST_LED_SLOW},
    {ST_LED_OFF, EV_BUTTON_PRESSED, ST_LED_SLOW},
    {ST_LED_SLOW, EV_BUTTON_PRESSED, ST_LED_MEDIUM},
    {ST_LED_MEDIUM, EV_BUTTON_PRESSED, ST_LED_FAST},
    {ST_LED_FAST, EV_BUTTON_PRESSED, ST_LED_SOLID},
    {ST_LED_SOLID, EV_BUTTON_PRESSED, ST_LED_OFF}
};

static stateFunctionRow_t LEDStateFunctionArray[] = {
    {"ST_INIT", &led_init},
    {"ST_LED_OFF", &led_off},
    {"ST_LED_SLOW", &led_slow},
    {"ST_LED_MEDIUM", &led_medium},
    {"ST_LED_FAST", &led_fast},
    {"ST_LED_SOLID", &led_solid},
};

void led_init(unsigned int *countdown_clicks) {
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_off(unsigned int *countdown_clicks) {
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_slow(unsigned int *countdown_clicks) {
    *countdown_clicks = 2000000;
}

void led_medium(unsigned int *countdown_clicks) {
    *countdown_clicks = 500000;
}

void led_fast(unsigned int *countdown_clicks) {
    *countdown_clicks = 100000;
}

void led_solid(unsigned int *countdown_clicks) {
    SET_BIT(GPIOA->ODR, 5);
}

void state_machine_run_iteration(stateMachine_t *stateMachine, event_t event, unsigned int *countdown_clicks) {
    for (int i = 0; i < sizeof(LEDStateTransMatrix)/sizeof(LEDStateTransMatrix[0]); i++) {
        if(LEDStateTransMatrix[i].currState == stateMachine->currState) {
            if((LEDStateTransMatrix[i].event == event )) {
                stateMachine->currState = LEDStateTransMatrix[i].nextState;

                (LEDStateFunctionArray[stateMachine->currState].func)(countdown_clicks);
                break;
            }
        }
    }
}

void state_machine_init(stateMachine_t * stateMachine) {
    stateMachine->currState = ST_LED_OFF;
}