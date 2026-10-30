/* IniFiles — C port of the FreePascal IniFiles unit subset */

#ifndef INIFILES_H
#define INIFILES_H

#include "Classes.h"

typedef struct MemIniFile MemIniFile;

MemIniFile *MemIniFile_Create(const char *filename);
void MemIniFile_Free(MemIniFile *ini);
int MemIniFile_SectionCount(const MemIniFile *ini);
bool MemIniFile_SectionExists(const MemIniFile *ini, const char *section);
void MemIniFile_ReadSectionValues(const MemIniFile *ini, const char *section, StringList *dest);
const char *MemIniFile_ReadString(const MemIniFile *ini, const char *section, const char *key, const char *def);
int MemIniFile_ReadInteger(const MemIniFile *ini, const char *section, const char *key, int def);
bool MemIniFile_ReadBool(const MemIniFile *ini, const char *section, const char *key, bool def);
void MemIniFile_WriteString(MemIniFile *ini, const char *section, const char *key, const char *value);
bool MemIniFile_UpdateFile(const MemIniFile *ini, const char *filename);

#endif /* INIFILES_H */