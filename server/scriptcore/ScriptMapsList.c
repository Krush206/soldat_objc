#include "ScriptMapsList.h"
#include <stdlib.h>
#include <string.h>

/* Internal map list structure */
typedef struct {
    char **maps;      /* array of map name strings */
    int count;        /* current number of maps */
    int capacity;     /* allocated capacity */
    int currentMapId; /* index of current map */
} MapList;

/* Singleton instance */
static MapList g_mapList = { NULL, 0, 0, -1 };

/* Helper: ensure capacity */
static int ensure_capacity(MapList *list, int needed) {
    if (list->capacity >= needed)
        return 0;
    int newCap = list->capacity == 0 ? 8 : list->capacity * 2;
    while (newCap < needed)
        newCap *= 2;
    char **newMaps = (char **)realloc(list->maps, newCap * sizeof(char *));
    if (!newMaps)
        return -1;
    list->maps = newMaps;
    list->capacity = newCap;
    return 0;
}

/* Add a map name */
void ScriptMapsList_AddMap(const char *name) {
    if (!name)
        return;
    if (ensure_capacity(&g_mapList, g_mapList.count + 1) != 0)
        return; /* allocation failure */
    g_mapList.maps[g_mapList.count] = strdup(name);
    if (!g_mapList.maps[g_mapList.count])
        return;
    g_mapList.count++;
}

/* Remove a map name */
void ScriptMapsList_RemoveMap(const char *name) {
    if (!name)
        return;
    for (int i = 0; i < g_mapList.count; ++i) {
        if (strcmp(g_mapList.maps[i], name) == 0) {
            free(g_mapList.maps[i]);
            /* shift remaining elements */
            for (int j = i; j < g_mapList.count - 1; ++j)
                g_mapList.maps[j] = g_mapList.maps[j + 1];
            g_mapList.count--;
            /* adjust currentMapId if needed */
            if (g_mapList.currentMapId >= g_mapList.count)
                g_mapList.currentMapId = g_mapList.count - 1;
            return;
        }
    }
}

/* Get map index by name */
int ScriptMapsList_GetMapIdByName(const char *name) {
    if (!name)
        return -1;
    for (int i = 0; i < g_mapList.count; ++i) {
        if (strcmp(g_mapList.maps[i], name) == 0)
            return i;
    }
    return -1;
}

/* Get map name by index */
const char *ScriptMapsList_GetMap(int index) {
    if (index < 0 || index >= g_mapList.count)
        return NULL;
    return g_mapList.maps[index];
}

/* Get current map index */
int ScriptMapsList_GetCurrentMapId(void) {
    return g_mapList.currentMapId;
}

/* Set current map index */
void ScriptMapsList_SetCurrentMapId(int newNum) {
    if (newNum >= 0 && newNum < g_mapList.count)
        g_mapList.currentMapId = newNum;
}

/* Get total maps count */
int ScriptMapsList_GetMapsCount(void) {
    return g_mapList.count;
}

/* Optional cleanup function for program termination */
void ScriptMapsList_Cleanup(void) {
    for (int i = 0; i < g_mapList.count; ++i)
        free(g_mapList.maps[i]);
    free(g_mapList.maps);
    g_mapList.maps = NULL;
    g_mapList.count = 0;
    g_mapList.capacity = 0;
    g_mapList.currentMapId = -1;
}