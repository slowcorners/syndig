#include <stdint.h>

#include "envelope.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

static uint8_t  attack  = 30;
static uint8_t  decay   = 50;
static float    sustain = 0.3;
static uint8_t  release = 127;

typedef enum {
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE,
	OFF
} state;

typedef struct {
	state state;
	float value;
	float velocity;
} envelope;

static envelope env[POLYPHONY];

void init_envelopes() {
	for (lane_id lane = 0; lane < POLYPHONY; lane++) {
		env[lane].state    = OFF;
		env[lane].value    = 0;
		env[lane].velocity = 0;
	}
}

void trigger_envelope(lane_id lane, float new_velocity) {
	env[lane].state    = ATTACK;
	env[lane].velocity = new_velocity;
}

void release_envelope(lane_id lane) {
	env[lane].state = RELEASE;
}

float envelope_nextval(lane_id lane) {

	envelope *e = &env[lane];
	
	if (e->state == OFF) {
		return 0;
	}

	if (e->state == ATTACK) {
		if (attack == 0 || e->value >= e->velocity) {
			e->value = e->velocity;
			e->state = DECAY;
		} else {
			e->value = MIN( e->value + ((MAX_MIDI - attack * e->velocity) / (MAX_MIDI * 100)), e->velocity);
		}
	}
	if (e->state == DECAY) {
		if (decay == 0 || e->value <= sustain * e->velocity) {
			e->value = sustain * e->velocity;
			e->state = SUSTAIN;
		} else {
			e->value = MAX( e->value - ((MAX_MIDI - decay * e->velocity) / (MAX_MIDI * 100)), sustain * e->velocity);
		}
	}
	if (e->state == RELEASE) {
		if (release == 0 || e->value <= 0) {
			e->value = 0;
			e->state = OFF;
		} else {
			e->value = MAX( e->value - ((MAX_MIDI - release * e->velocity) / (MAX_MIDI * 100)), 0);
		}
	}

	return e->value;
}

bool envelope_is_running(lane_id lane) {
	return env[lane].state != OFF;
}

bool  envelope_is_in_release(lane_id lane) {
	return env[lane].state == RELEASE;
}
