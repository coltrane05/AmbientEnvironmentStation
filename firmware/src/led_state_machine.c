#include "led_state_machine.h"
#include "gpio.h"
#include "register_macros.h"
#include "timx.h"

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

void state_machine_run_iteration(stateMachine_t *stateMachine, event_t event) {
    for (int i = 0; i < sizeof(LEDStateTransMatrix)/sizeof(LEDStateTransMatrix[0]); i++) {
        if(LEDStateTransMatrix[i].currState == stateMachine->currState) {
            if((LEDStateTransMatrix[i].event == event )) {
                stateMachine->currState = LEDStateTransMatrix[i].nextState;

                (LEDStateFunctionArray[stateMachine->currState].func)();
                break;
            }
        }
    }
}

void state_machine_init(stateMachine_t * stateMachine) {
    stateMachine->currState = ST_LED_OFF;
    CLEAR_BIT(GPIOA->ODR, 5);
}