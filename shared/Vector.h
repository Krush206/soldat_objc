/* Vector — Objective-C port of shared/Vector.pas (D3DX-style vector math
 * for SOLDAT). Structs + inline C functions. */

#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

typedef struct TVector2 {
	float X, Y;
} TVector2;

typedef struct TVector3 {
	float X, Y, Z;
} TVector3;

typedef TVector2 *PVector2;
typedef TVector3 *PVector3;

static inline TVector2 Vector2Make(float X, float Y)
{
	TVector2 r;
	r.X = X;
	r.Y = Y;
	return r;
}

static inline float Vec2Length(TVector2 v)
{
	return sqrtf(v.X * v.X + v.Y * v.Y);
}

static inline float Vec2Length2(TVector2 v)
{
	return v.X * v.X + v.Y * v.Y;
}

static inline float Vec2Dot(TVector2 v1, TVector2 v2)
{
	return v1.X * v2.X + v1.Y * v2.Y;
}

static inline TVector2 Vec2Add(TVector2 v1, TVector2 v2)
{
	return Vector2Make(v1.X + v2.X, v1.Y + v2.Y);
}

static inline TVector2 Vec2Subtract(TVector2 v1, TVector2 v2)
{
	return Vector2Make(v1.X - v2.X, v1.Y - v2.Y);
}

static inline PVector2 Vec2Scale(TVector2 *vOut, TVector2 v, float s)
{
	vOut->X = v.X * s;
	vOut->Y = v.Y * s;
	return vOut;
}

static inline PVector2 Vec2Normalize(TVector2 *vOut, TVector2 v)
{
	float len = Vec2Length(v);

	if (len < 0.001f && len > -0.001f) {
		vOut->X = 0;
		vOut->Y = 0;
	} else {
		vOut->X = v.X / len;
		vOut->Y = v.Y / len;
	}
	return vOut;
}

static inline float Vec3Length(TVector3 v)
{
	return sqrtf(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
}

#endif /* VECTOR_H */
