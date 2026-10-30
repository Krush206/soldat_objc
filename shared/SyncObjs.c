/* SyncObjs — C port of the FreePascal SyncObjs unit subset */
#include "SyncObjs.h"
#include <pthread.h>
#include <stdlib.h>

struct CriticalSection {
    pthread_mutex_t _mutex;
};

CriticalSection *CriticalSection_Create(void) {
    CriticalSection *cs = calloc(1, sizeof(CriticalSection));
    if (!cs) return NULL;
    pthread_mutex_init(&cs->_mutex, NULL);
    return cs;
}

void CriticalSection_Free(CriticalSection *cs) {
    if (!cs) return;
    pthread_mutex_destroy(&cs->_mutex);
    free(cs);
}

void CriticalSection_Acquire(CriticalSection *cs) {
    if (cs) pthread_mutex_lock(&cs->_mutex);
}

void CriticalSection_Release(CriticalSection *cs) {
    if (cs) pthread_mutex_unlock(&cs->_mutex);
}

bool CriticalSection_TryAcquire(CriticalSection *cs) {
    if (!cs) return false;
    return pthread_mutex_trylock(&cs->_mutex) == 0;
}