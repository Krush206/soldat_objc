#include "MyInterfacedObject.h"

/* The Pascal implementation has no internal state, so the C version is stateless.
 * QueryInterface always fails because no concrete interface GUIDs are exposed.
 * AddRef and Release return -1 to indicate that reference counting is not used,
 * matching the Pascal behaviour. */

HRESULT MyInterfacedObject_QueryInterface(const GUID *IID, void **Obj)
{
    (void)IID;   /* Unused in this minimal implementation */
    (void)Obj;   /* Unused */

    /* No supported interfaces are known at this level. */
    return E_NOINTERFACE;
}

int32_t MyInterfacedObject_AddRef(void)
{
    /* -1 signals that reference counting is disabled, exactly as in the Pascal code. */
    return -1;
}

int32_t MyInterfacedObject_Release(void)
{
    /* -1 signals that reference counting is disabled, exactly as in the Pascal code. */
    return -1;
}