/* SyncObjs — C port of the FreePascal SyncObjs unit subset */

#ifndef SYNCOBJS_H
#define SYNCOBJS_H

#include <pthread.h>
#include <stdbool.h>

typedef struct CriticalSection CriticalSection;

CriticalSection *CriticalSection_Create(void);
void CriticalSection_Free(CriticalSection *cs);
void CriticalSection_Acquire(CriticalSection *cs);
void CriticalSection_Release(CriticalSection *cs);
bool CriticalSection_TryAcquire(CriticalSection *cs);

#endif /* SYNCOBJS_H */