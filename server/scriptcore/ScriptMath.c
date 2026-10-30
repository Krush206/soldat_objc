#include "ScriptMath.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>

/* Constants */
double ScriptMath_GetE(void) {
    return 2.718281828459045;
}

double ScriptMath_GetPi(void) {
    return M_PI;
}

/* Trigonometric */
double ScriptMath_Sin(double a) {
    return sin(a);
}

double ScriptMath_Cos(double a) {
    return cos(a);
}

double ScriptMath_Tan(double a) {
    return tan(a);
}

double ScriptMath_Cotan(double a) {
    double s = sin(a);
    if (s == 0.0) return HUGE_VAL;
    return cos(a) / s;
}

double ScriptMath_ArcSin(double a) {
    return asin(a);
}

double ScriptMath_ArcCos(double a) {
    return acos(a);
}

double ScriptMath_ArcTan(double a) {
    return atan(a);
}

double ScriptMath_ArcCotan(double a) {
    if (a == 0.0) return M_PI_2;
    return atan(1.0 / a);
}

double ScriptMath_ArcTan2(double a, double b) {
    return atan2(a, b);
}

void ScriptMath_Sincos(double theta, double *sinus, double *cosinus) {
    if (sinus) *sinus = sin(theta);
    if (cosinus) *cosinus = cos(theta);
}

/* Power / Log */
double ScriptMath_Pow(double a, double b) {
    return pow(a, b);
}

double ScriptMath_LogN(double a, double b) {
    if (a <= 0.0 || b <= 0.0 || b == 1.0) return NAN;
    return log(a) / log(b);
}

double ScriptMath_Ln(double a) {
    return log(a);
}

double ScriptMath_Exp(double a) {
    return exp(a);
}

/* Min / Max / Abs */
double ScriptMath_Min(double a, double b) {
    return (a < b) ? a : b;
}

double ScriptMath_Max(double a, double b) {
    return (a > b) ? a : b;
}

double ScriptMath_Abs(double a) {
    return fabs(a);
}

/* Sign / NaN */
int ScriptMath_Sign(double a) {
    if (a > 0.0) return 1;
    if (a < 0.0) return -1;
    return 0;
}

int ScriptMath_IsNaN(double a) {
    return isnan(a);
}

/* Rounding */
int ScriptMath_Round(double a) {
    return (int) llround(a);
}

double ScriptMath_RoundTo(double a, int b) {
    /* Delphi's RoundTo(value, digit) where digit is power of ten exponent.
       Negative digit => round to fractional digits. */
    double factor = pow(10.0, -b);
    return round(a * factor) / factor;
}

/* Degree / Radian conversion */
double ScriptMath_DegToRad(double a) {
    return a * (M_PI / 180.0);
}

double ScriptMath_RadToDeg(double a) {
    return a * (180.0 / M_PI);
}

double ScriptMath_DegNormalize(double a) {
    double res = fmod(a, 360.0);
    if (res < 0.0) res += 360.0;
    return res;
}

/* Range checks */
int ScriptMath_InRange(double a, double b, double c) {
    double lo = (b < c) ? b : c;
    double hi = (b > c) ? b : c;
    return (a >= lo && a <= hi);
}

double ScriptMath_EnsureRange(double aValue, double aMin, double aMax) {
    if (aValue < aMin) return aMin;
    if (aValue > aMax) return aMax;
    return aValue;
}

/* Random */
double ScriptMath_Random(double min, double max) {
    double r = (double)rand() / ((double)RAND_MAX + 1.0);
    return min + r * (max - min);
}