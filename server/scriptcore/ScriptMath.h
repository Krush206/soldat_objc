#ifndef SCRIPTMATH_H
#define SCRIPTMATH_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */
double ScriptMath_GetE(void);
double ScriptMath_GetPi(void);

/* Trigonometric */
double ScriptMath_Sin(double a);
double ScriptMath_Cos(double a);
double ScriptMath_Tan(double a);
double ScriptMath_Cotan(double a);
double ScriptMath_ArcSin(double a);
double ScriptMath_ArcCos(double a);
double ScriptMath_ArcTan(double a);
double ScriptMath_ArcCotan(double a);
double ScriptMath_ArcTan2(double a, double b);
void   ScriptMath_Sincos(double theta, double *sinus, double *cosinus);

/* Power / Log */
double ScriptMath_Pow(double a, double b);
double ScriptMath_LogN(double a, double b);  /* log base b of a */
double ScriptMath_Ln(double a);
double ScriptMath_Exp(double a);

/* Min / Max / Abs */
double ScriptMath_Min(double a, double b);
double ScriptMath_Max(double a, double b);
double ScriptMath_Abs(double a);

/* Sign / NaN */
int    ScriptMath_Sign(double a);           /* returns -1, 0, 1 */
int    ScriptMath_IsNaN(double a);          /* boolean */

/* Rounding */
int    ScriptMath_Round(double a);          /* nearest integer */
double ScriptMath_RoundTo(double a, int b); /* b is TRoundToRange (int8) */

/* Degree / Radian conversion */
double ScriptMath_DegToRad(double a);
double ScriptMath_RadToDeg(double a);
double ScriptMath_DegNormalize(double a);

/* Range checks */
int    ScriptMath_InRange(double a, double b, double c);          /* a in [b,c] */
double ScriptMath_EnsureRange(double aValue, double aMin, double aMax);

/* Random */
double ScriptMath_Random(double min, double max);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPTMATH_H */