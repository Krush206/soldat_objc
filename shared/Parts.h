/* Parts — C port of shared/Parts.pas */

#ifndef PARTS_H
#define PARTS_H

#include "Vector.h"
#include <stdbool.h>

#define NUM_PARTICLES 560
#define RKV 0.98

typedef struct Constraint {
    bool Active;
    int PartA, PartB;
    float Restlength;
} Constraint;

typedef struct ParticleSystem {
    bool Active[NUM_PARTICLES + 1];          /* Pascal 1..NUM_PARTICLES */
    TVector2 Pos[NUM_PARTICLES + 1];
    TVector2 Velocity[NUM_PARTICLES + 1];
    TVector2 OldPos[NUM_PARTICLES + 1];
    TVector2 Forces[NUM_PARTICLES + 1];
    float OneOverMass[NUM_PARTICLES + 1];
    float TimeStep;
    float Gravity, VDamping, EDamping;
    int ConstraintCount;
    int PartCount;
    Constraint Constraints[NUM_PARTICLES + 1];
} ParticleSystem;

void ParticleSystem_DoVerletTimeStep(ParticleSystem *self);
void ParticleSystem_DoVerletTimeStepFor(ParticleSystem *self, int i, int j);
void ParticleSystem_DoEulerTimeStep(ParticleSystem *self);
void ParticleSystem_DoEulerTimeStepFor(ParticleSystem *self, int i);
void ParticleSystem_CreatePart(ParticleSystem *self, TVector2 start,
    TVector2 vel, float mass, int num);
void ParticleSystem_MakeConstraint(ParticleSystem *self, int pa, int pb,
    float rest);
void ParticleSystem_Clone(ParticleSystem *self, ParticleSystem *other);
void ParticleSystem_LoadPOObject(ParticleSystem *self, const char *filename,
    float scale);
void ParticleSystem_StopAllParts(ParticleSystem *self);
void ParticleSystem_Destroy(ParticleSystem *self);
void ParticleSystem_SatisfyConstraints(ParticleSystem *self);

extern ParticleSystem SparkParts;
extern ParticleSystem SpriteParts;
extern ParticleSystem BulletParts;

#endif /* PARTS_H */