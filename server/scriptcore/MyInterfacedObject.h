#ifndef MYINTERFACEDOBJECT_H
#define MYINTERFACEDOBJECT_H

#include <stdint.h>

/* Basic COM-like definitions */
typedef int32_t HRESULT;

typedef struct _GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} GUID;

/* Standard HRESULT values */
#define S_OK                 ((HRESULT)0x00000000L)
#define E_NOINTERFACE        ((HRESULT)0x80004002L)

/* Forward declaration of the object (no data members in the Pascal version) */
typedef struct MyInterfacedObject MyInterfacedObject;

/* Interface function declarations */
HRESULT MyInterfacedObject_QueryInterface(const GUID *IID, void **Obj);
int32_t MyInterfacedObject_AddRef(void);
int32_t MyInterfacedObject_Release(void);

#endif /* MYINTERFACEDOBJECT_H */