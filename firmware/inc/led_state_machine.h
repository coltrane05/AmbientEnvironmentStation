#ifndef LED_STATE_MACHINE_H
#define LED_STATE_MACHINE_H

#include <stdbool.h>

typedef enum
{
    ST_INIT,
    ST_LED_OFF,
    ST_LED_SLOW,
    ST_LED_MEDIUM,
    ST_LED_FAST,
    ST_LED_SOLID
} led_state_t;

// Event types for the state machine.
// Only one event is needed for this simple state machine.
typedef enum
{
    EV_NONE,
    EV_BUTTON_PRESSED
} led_event_t;

// A sturcture to define a row in the state transition matrix.
typedef struct
{
    led_state_t currState;
    led_event_t event;
    led_state_t nextState;
} led_state_trans_matrix_row_t;

// A structure to define a row in the state function array.
typedef struct
{
    const char * name;
    void (*func)(void);
} led_state_function_row_t;

// The state machine structure. It holds the current state of the machine.
typedef struct
{
    led_state_t currState;
} led_state_machine_t;

void led_init (void);
void led_off (void);
void led_slow (void);
void led_medium (void);
void led_fast (void);
void led_solid (void);

void state_machine_run_iteration (led_event_t event);
bool state_machine_is_blinking (void);
void state_machine_init ();

#endif