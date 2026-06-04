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
    void (*func)(void);
} stateFunctionRow_t;

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