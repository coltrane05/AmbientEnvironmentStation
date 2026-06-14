#include "led_state_machine.h"
#include "gpio.h"
#include "register_macros.h"
#include "timx.h"
#include "usart.h"

static led_state_machine_t led_state_machine;

// State transition matrix for the state machine.
// Each row defines a transition from a current state to a next state based on 
// an event.
static led_state_trans_matrix_row_t LEDStateTransMatrix[] = {
    {ST_INIT, EV_BUTTON_PRESSED, ST_LED_SLOW},
    {ST_LED_OFF, EV_BUTTON_PRESSED, ST_LED_SLOW},
    {ST_LED_SLOW, EV_BUTTON_PRESSED, ST_LED_MEDIUM},
    {ST_LED_MEDIUM, EV_BUTTON_PRESSED, ST_LED_FAST},
    {ST_LED_FAST, EV_BUTTON_PRESSED, ST_LED_SOLID},
    {ST_LED_SOLID, EV_BUTTON_PRESSED, ST_LED_OFF}
};

// Array of function pointers corresponding to each state.
// Each function will be called when the state machine transitions to the
// corresponding state.
static led_state_function_row_t LEDStateFunctionArray[] = {
    {"ST_INIT", &led_init},
    {"ST_LED_OFF", &led_off},
    {"ST_LED_SLOW", &led_slow},
    {"ST_LED_MEDIUM", &led_medium},
    {"ST_LED_FAST", &led_fast},
    {"ST_LED_SOLID", &led_solid},
};

// Function definitions for each state.
// These functions will be called when the state machine transitions to the 
// corresponding state.
void led_init (void)
{
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_off (void)
{
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_slow (void)
{
    TIM2->ARR = 1499;
    SET_BIT(TIM2->EGR, 0);
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_medium (void)
{
    TIM2->ARR = 749;
    SET_BIT(TIM2->EGR, 0);
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_fast (void)
{
    TIM2->ARR = 249;
    SET_BIT(TIM2->EGR, 0);
    CLEAR_BIT(GPIOA->ODR, 5);
}

void led_solid (void)
{
    SET_BIT(GPIOA->ODR, 5);
}

// Function to run one iteration of the state machine based on the current state and an event.
void state_machine_run_iteration (led_event_t event)
{
    for (int i = 0; i < sizeof(LEDStateTransMatrix)/sizeof(LEDStateTransMatrix[0]); i++)
    {
        if (LEDStateTransMatrix[i].currState == led_state_machine.currState)
        {
            if ((LEDStateTransMatrix[i].event == event ))
            {
                led_state_machine.currState = LEDStateTransMatrix[i].nextState;

                (LEDStateFunctionArray[led_state_machine.currState].func)();
                usart2_print("Current State: ");
                usart2_print(LEDStateFunctionArray[led_state_machine.currState].name);
                usart2_print("\r\n");
                break;
            }
        }
    }
}

// Check if the led is in a blinking state
bool state_machine_is_blinking (void)
{
    if (led_state_machine.currState != ST_LED_OFF && led_state_machine.currState != ST_LED_SOLID)
    {
            return true;
    }
    else
    {
        return false;
    }
}

// Function to initialize the state machine. Sets the initial state and performs any necessary setup.
void state_machine_init ()
{
    led_state_machine.currState = ST_INIT;
    state_machine_run_iteration(EV_BUTTON_PRESSED);

    CLEAR_BIT(GPIOA->ODR, 5);
}