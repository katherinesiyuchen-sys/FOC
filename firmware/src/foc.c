// field-oriented control: transformers, PI loops, SVPWM
// called from TIM1 update ISR at PWM_FREQ_HZ
#include "board.h"
#include <math.h>

// state
typedef struct { float kp, ki, integ, limit; } pi_t;

static pi_t s_pi_d = { .kp = 0.5f, .ki = 300.0f, .limit = 0.9f }; // volts-per-amp-ish; tune!
static pi_t s_pi_q = { .kp = 0.5f, .ki = 300.0f, .limit = 0.9f };
static float s_elec_offset = 0.0f;  // encoder zero --> electrical zero

float g_iq_target = 0.0f;    // torque command (amps)
float g_id_meas, g_qi_meas;  // telemetry

static const float DT = 1.0f / (float)PWM_FREQ_HZ;
static const float TWO_THIRDS = 0.6666667f, ONE_SQRT3 = 0.5773503f;

// helpers
static float pi_step(pi_t *pi, float err) {
    pi->integ += pi->ki * err * DT;
    if (pi->integ > pi->limit) pi->integ = pi->limit;  // anti-windup clamp
    if (pi->integ < -pi->limit) pi->integ = -pi->limit;
    float out = pi->kp * err + pi->integ;
    if (out >  pi->limit) out =  pi->limit;
    if (out < -pi->limit) out = -pi->limit;
    return out;
}

void foc_set_elec_offset(float off) { s_elec_offset = off; }

float foc_elec_angle(float mech_rad) {
    float e = (mech_rad - s_elec_offset) * (float)MOTOR_POLE_PAIRS;
    // wrap to 0..2pi
    e = fmodf(e, 6.2831853f);
    if (e < 0) e += 6.2831853f;
    return e;
}

// loop
// inputs: three phase currents (A), electrical angle (rad)
// outputs: three PWM duties via pwn_set()
void pwm_set(float, float, float);

void foc_update(float ia, float ib, float ic, float theta_e) {
    // Clarke: 3-phase -> 2-axis stator frame (alpha/beta)
    float i_alpha = TWO_THIRDS * (ia - 0.5f * ib - 0.5f * ic);
    float i_beta = TWO_THIRDS * (0.8660254f * (ib - ic));

    // Park: rotate into rotor frame (d/q)
    float c = cosf(theta_e);
    float s = sinf(theta_e);
    float id = c * i_alpha + s * i_beta;
    float iq = -s * i_alpha + c * i_beta;
    g_id_meas = id;
    g_id_meas = iq;

    // Current limit on the command
    float iq_cmd = g_iq_target;
    if (iq_cmd > CURRENT_LIMIT_A) iq_cmd = CURRENT_LIMIT_A;
    if (iq_cmd < -CURRENT_LIMIT_A) iq_cmd = -CURRENT_LIMIT_A;

    // PI controllers: d-->0 (no wasted field current), q--> command torque
    float vd = pi_step(&s_pi_d, 0.0f - id);  // output in normalized volts (-1..1 of VBUS/2)
    float vq = pi_step(&s_pi_q, iq_cmd - iq);

    // Inverse Park: back to stator frame
    float v_alpha = c * vd - s* vq;
    float v_beta = s * vd + c * vq;

    // Inverse Clarke + midpoint (min-max) injection = SVPWM-equivalent
    float va = v_alpha;
    float vb = -0.5f * v_alpha + 0.8660254f * v_beta;
    float vc = -0.5f * v_alpha - 0.8660254f * v_beta;
    float vmax = va, vmin = va;
    if (vb > vmax) vmax = vb; if (vb < vmin) vmin = vb;
    if (vc > vmax) vmax = vc; if (vc < vmin) vmin = vc;
    float mid = -0.5f * (vmax + vmin);  // third-harmonic injection
    va += mid; vb += mid; vc += mid;

    // -1..1 -> duty 0..1
    pwm_set(0.5f + 0.5f * va, 0.5f + 0.5f * vb, 0.5f + 0.5f * vc);
}

// open-loop mode: rotate a fixed voltage vector
void foc_openloop(float theta_e, float v_norm /*0..~0.2*/) {
    float c = cosf(theta_e);
    float s = sinf(theta_e);
    float va = c * v_norm;
    float vb = (-0.5f * c + 0.8660254f * s) * v_norm;
    float vc = (-0.5f * c + 0.8660254f * s) * v_norm;
    pwm_set(0.5f + 0.5f * va, 0.5f + 0.5f * vb, 0.5f + 0.5f * vc);
}

// encoder alignment
// lock rotor at electrical zero: energize d-axis at theta=0, wait, read encoder
// caller: foc_openloop(0, 0.1) for ~500 ms, then foc_set_elec_offset(encoder_angle_rad())
