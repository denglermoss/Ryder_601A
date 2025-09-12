#include "functions.h"
#include "config.h"
#include <cmath>

// Prefer our own PI rather than relying on M_PI from <cmath>
inline constexpr float PI = 3.14159265358979323846f;

// Return the pointer to the selected waveform function
wave_func_ptr Functions::get_function(int i) {
    switch (i) {
        case 0: return Functions::SIN;
        case 1: return Functions::T;
        case 2: return Functions::R;
        case 3: return Functions::P;
        case 4: return Functions::Q;
        case 5: return Functions::QRS;
        case 6: return Functions::PRT;
        case 7: return Functions::PQRST;
        default: return nullptr;
    }
}

// 1) Simple sine: sin(2 * pi * t)
float Functions::SIN(float t) {
    float angle = 2.0f * PI * t;
    return std::sin(angle);
}

// 2) T-wave-like: 0.8 * sin(pi * t + 5.5)^22
float Functions::T(float t) {
    float angle = PI * t + 5.5f;
    float s = std::sin(angle);
    return 0.8f * std::pow(s, 22.0f);
}

// 3) R-wave-like: 7 * sin(pi * t)^300
float Functions::R(float t) {
    float angle = PI * t;
    float s = std::sin(angle);
    return 7.0f * std::pow(s, 300.0f);
}

// 4) P-wave-like: 0.5 * sin(pi * t + 0.9)^16
float Functions::P(float t) {
    float angle = PI * t + 0.9f;
    float s = std::sin(angle);
    return 0.5f * std::pow(s, 16.0f);
}

// 5) Q-wave-like: -0.5 * sin(pi * t - 3)^10 - 0.2 * sin(pi * t + 5.5)^10
float Functions::Q(float t) {
    float angleBase = PI * t;
    float sNeg3 = std::sin(angleBase - 3.0f);
    float sPos55 = std::sin(angleBase + 5.5f);

    float leftTerm  = -0.5f * std::pow(sNeg3, 10.0f);
    float rightTerm = -0.2f * std::pow(sPos55, 10.0f);
    return leftTerm + rightTerm;
}

// 6) QRS: Combine Q + R in one pass to avoid extra calls
float Functions::QRS(float t) {
    float angleBase = PI * t;
    float s = std::sin(angleBase);
    float sNeg3 = std::sin(angleBase - 3.0f);
    float sPos55 = std::sin(angleBase + 5.5f);

    float Rval = 7.0f * std::pow(s, 300.0f);
    float Qval = -0.5f * std::pow(sNeg3, 10.0f)
               - 0.2f * std::pow(sPos55, 10.0f);
    return Qval + Rval;
}

// 7) PRT: T + R + P
float Functions::PRT(float t) {
    float angleBase = PI * t;
    float s = std::sin(angleBase); 
    float sPos55 = std::sin(angleBase + 5.5f); 
    float sPos09 = std::sin(angleBase + 0.9f); 

    float Rval = 7.0f * std::pow(s, 300.0f);
    float Tval = 0.8f * std::pow(sPos55, 22.0f);
    float Pval = 0.5f * std::pow(sPos09, 16.0f);

    return Rval + Tval + Pval;
}

// 8) PQRST: Q + R + T + P in a single pass
float Functions::PQRST(float t) {
    float angleBase = PI * t;

    float s = std::sin(angleBase);
    float sPos55 = std::sin(angleBase + 5.5f); 
    float sPos09 = std::sin(angleBase + 0.9f); 
    float sNeg3 = std::sin(angleBase - 3.0f); 

    float Rval = 7.0f * std::pow(s, 300.0f);
    float Tval = 0.8f * std::pow(sPos55, 22.0f);
    float Pval = 0.5f * std::pow(sPos09, 16.0f);
    float Qval = -0.5f * std::pow(sNeg3, 10.0f) - 0.2f * std::pow(sPos55, 10.0f);

    return Rval + Tval + Pval + Qval;
}
