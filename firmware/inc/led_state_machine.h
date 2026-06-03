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

typedef enum {
    EV_NONE,
    EV_BUTTON_PRESSED
} event_t;

typedef struct {
    state_t currState;
    event_t event;
    state_t nextState;
} stateTransMatrixRow_t;

typedef struct {
    const char * name;
    void (*func)(unsigned int *countdown_clicks);
} stateFunctionRow_t;

typedef struct {
    state_t currState;
} stateMachine_t;

void led_init(unsigned int *countdown_clicks);
void led_off(unsigned int *countdown_clicks);
void led_slow(unsigned int *countdown_clicks);
void led_medium(unsigned int *countdown_clicks);
void led_fast(unsigned int *countdown_clicks);
void led_solid(unsigned int *countdown_clicks);

void state_machine_run_iteration(stateMachine_t *stateMachine, event_t event, unsigned int *countdown_clicks);
void state_machine_init(stateMachine_t *stateMachine);

#endif