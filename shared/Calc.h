/* Calc — C port of shared/Calc.pas.
 *
 * Pascal overloads taking vectors get a V suffix in C
 * (Distance -> Distance / DistanceV, SqrDist -> SqrDist / SqrDistV). */

#ifndef CALC_H
#define CALC_H

#include "Vector.h"

typedef struct TIntersectionResult {
    TVector2 Points[2];
    unsigned char NumIntersections;
} TIntersectionResult;

TIntersectionResult Calc_IsLineIntersectingCircle(TVector2 line1, TVector2 line2,
    TVector2 circleCenter, float radius);
int Calc_LineCircleCollision(TVector2 startPoint, TVector2 endPoint,
    TVector2 circleCenter, float radius, TVector2 *collisionPoint);
float Calc_PointLineDistance(TVector2 p1, TVector2 p2, TVector2 p3);
float Calc_Angle2Points(TVector2 p1, TVector2 p2);
float Calc_Distance(float x1, float y1, float x2, float y2);
float Calc_SqrDist(float x1, float y1, float x2, float y2);
float Calc_SqrDistV(TVector2 p1, TVector2 p2);
float Calc_DistanceV(TVector2 p1, TVector2 p2);
int Calc_GreaterPowerOf2(int n);
int Calc_RoundFair(float value);

#endif /* CALC_H */