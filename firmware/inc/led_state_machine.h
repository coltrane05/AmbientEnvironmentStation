#ifndef LED_STATE_MACHINE_H
#define LED_STATE_MACHINE_H

typedef enum {
    ST_INIT,
    ST_LED_OFF,
    ST_LED_SLOW,
    ST_LED_MEDIUM,
    ST_LED_FAST,
    ST_LED_SOLID,
} state_t;

// Event types for the state machine.
// Only one event is needed for this simple state machine.
typedef enum {
    EV_NONE,
    EV_BUTTON_PRESSED
} event_t;

// A sturcture to define a row in the state transition matrix.
typedef struct {
    state_t currState;
    event_t event;
    state_t nextState;
} stateTransMatrixRow_t;

// A structure to define a row in the state function array.
typedef struct {
    const char * name;
    void (*func)(void);
} stateFunctionRow_t;

// The state machine structure. It holds the current state of the machine.
typedef struct {
    state_t currState;
} stateMachine_t;

void led_init(void);
void led_off(void);
void led_slow(void);
void led_medium(void);
void led_fast(void);
void led_solid(void);

void state_machine_run_iteration(stateMachine_t *stateMachine, event_t event);
void state_machine_init(stateMachine_t *stateMachine);

#endif