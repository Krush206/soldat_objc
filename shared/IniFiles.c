/* IniFiles — C port of the FreePascal IniFiles unit subset */
#include "IniFiles.h"
#include "Classes.h"
#include "SysUtils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct MemIniFile {
    char **sectionNames;
    StringList **sectionData;
    int sectionCount;
    int sectionCapacity;
};

MemIniFile *MemIniFile_Create(const char *filename) {
    (void)filename;
    MemIniFile *ini = calloc(1, sizeof(MemIniFile));
    if (!ini) return NULL;
    ini->sectionCapacity = 16;
    ini->sectionNames = calloc(ini->sectionCapacity, sizeof(char *));
    ini->sectionData = calloc(ini->sectionCapacity, sizeof(StringList *));
    return ini;
}

void MemIniFile_Free(MemIniFile *ini) {
    if (!ini) return;
    for (int i = 0; i < ini->sectionCount; i++) {
        free(ini->sectionNames[i]);
        if (ini->sectionData[i]) StringList_Free(ini->sectionData[i]);
    }
    free(ini->sectionNames);
    free(ini->sectionData);
    free(ini);
}

int MemIniFile_SectionCount(const MemIniFile *ini) {
    return ini ? ini->sectionCount : 0;
}

bool MemIniFile_SectionExists(const MemIniFile *ini, const char *section) {
    if (!ini || !section) return false;
    for (int i = 0; i < ini->sectionCount; i++) {
        if (ini->sectionNames[i] && strcmp(ini->sectionNames[i], section) == 0) return true;
    }
    return false;
}

void MemIniFile_ReadSectionValues(const MemIniFile *ini, const char *section, StringList *dest) {
    if (!ini || !section || !dest) return;
    for (int i = 0; i < ini->sectionCount; i++) {
        if (ini->sectionNames[i] && strcmp(ini->sectionNames[i], section) == 0) {
            if (ini->sectionData[i]) {
                for (int j = 0; j < StringList_Count(ini->sectionData[i]); j++) {
                    StringList_Add(dest, StringList_Get(ini->sectionData[i], j));
                }
            }
            break;
        }
    }
}

const char *MemIniFile_ReadString(const MemIniFile *ini, const char *section, const char *key, const char *def) {
    if (!ini || !section || !key) return def;
    for (int i = 0; i < ini->sectionCount; i++) {
        if (ini->sectionNames[i] && strcmp(ini->sectionNames[i], section) == 0) {
            if (ini->sectionData[i]) {
                const char *val = StringList_GetValue(ini->sectionData[i], key);
                if (val) return val;
            }
            break;
        }
    }
    return def;
}

int MemIniFile_ReadInteger(const MemIniFile *ini, const char *section, const char *key, int def) {
    const char *s = MemIniFile_ReadString(ini, section, key, NULL);
    if (s) return atoi(s);
    return def;
}

bool MemIniFile_ReadBool(const MemIniFile *ini, const char *section, const char *key, bool def) {
    const char *s = MemIniFile_ReadString(ini, section, key, NULL);
    if (s) {
        if (strcmp(s, "1") == 0 || strcmp(s, "true") == 0 || strcmp(s, "True") == 0) return true;
        if (strcmp(s, "0") == 0 || strcmp(s, "false") == 0 || strcmp(s, "False") == 0) return false;
    }
    return def;
}

void MemIniFile_WriteString(MemIniFile *ini, const char *section, const char *key, const char *value) {
    if (!ini || !section || !key) return;
    
    int sectionIdx = -1;
    for (int i = 0; i < ini->sectionCount; i++) {
        if (ini->sectionNames[i] && strcmp(ini->sectionNames[i], section) == 0) {
            sectionIdx = i;
            break;
        }
    }
    
    if (sectionIdx == -1) {
        if (ini->sectionCount >= ini->sectionCapacity) {
            ini->sectionCapacity *= 2;
            ini->sectionNames = realloc(ini->sectionNames, ini->sectionCapacity * sizeof(char *));
            ini->sectionData = realloc(ini->sectionData, ini->sectionCapacity * sizeof(StringList *));
        }
        sectionIdx = ini->sectionCount++;
        ini->sectionNames[sectionIdx] = strdup(section);
        ini->sectionData[sectionIdx] = StringList_Create();
    }
    
    StringList_SetValue(ini->sectionData[sectionIdx], value, key);
}

bool MemIniFile_UpdateFile(const MemIniFile *ini, const char *filename) {
    if (!ini || !filename) return false;
    FILE *f = fopen(filename, "w");
    if (!f) return false;
    
    for (int i = 0; i < ini->sectionCount; i++) {
        if (!ini->sectionNames[i]) continue;
        fprintf(f, "[%s]\n", ini->sectionNames[i]);
        if (ini->sectionData[i]) {
            for (int j = 0; j < StringList_Count(ini->sectionData[i]); j++) {
                const char *line = StringList_Get(ini->sectionData[i], j);
                if (line) fprintf(f, "%s\n", line);
            }
        }
        fprintf(f, "\n");
    }
    
    fclose(f);
    return true;
}