#ifndef SCRIPTMAPSLIST_H
#define SCRIPTMAPSLIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Add a map name to the map list. */
void ScriptMapsList_AddMap(const char *name);

/* Remove a map name from the map list. */
void ScriptMapsList_RemoveMap(const char *name);

/* Return the index of a map by its name, or -1 if not found. */
int ScriptMapsList_GetMapIdByName(const char *name);

/* Return the map name at the given index, or NULL if index out of bounds. */
const char *ScriptMapsList_GetMap(int index);

/* Return the current map index. */
int ScriptMapsList_GetCurrentMapId(void);

/* Set the current map index if it is within valid range. */
void ScriptMapsList_SetCurrentMapId(int newNum);

/* Return the total number of maps in the list. */
int ScriptMapsList_GetMapsCount(void);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPTMAPSLIST_H */