#ifndef SCRIPT_FILE_API_H
#define SCRIPT_FILE_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Opaque forward declarations */
typedef struct Script Script;
typedef struct ScriptFileAPI ScriptFileAPI;
typedef struct ScriptFile ScriptFile;
typedef struct IniFile IniFile;
typedef struct StringList StringList;
typedef struct MemoryStream MemoryStream;
typedef struct StringStream StringStream;

/* ScriptFileAPI */
ScriptFileAPI *script_file_api_create(Script *script);
void script_file_api_destroy(ScriptFileAPI *api);

/* Properties */
uint8_t script_file_api_get_sandbox_level(const ScriptFileAPI *api);
bool script_file_api_get_allow_ini_edit(const ScriptFileAPI *api);
const char *script_file_api_get_data_folder(const ScriptFileAPI *api);

/* Access check – normalises the path and returns true if allowed.
 * The path buffer may be modified to the normalised relative path. */
bool script_file_api_check_access(ScriptFileAPI *api, char **path);

/* ScriptFile (obtained from api) */
ScriptFile *script_file_api_get_file(const ScriptFileAPI *api);

/* ScriptFile methods */
bool script_file_check_access(ScriptFile *file, const char *path);
MemoryStream *script_file_create_file_stream(ScriptFile *file);
MemoryStream *script_file_create_file_stream_from_file(ScriptFile *file, const char *path);
StringList *script_file_create_string_list(ScriptFile *file);
StringList *script_file_create_string_list_from_file(ScriptFile *file, const char *path);
IniFile *script_file_create_ini(ScriptFile *file, const char *path);
bool script_file_exists(ScriptFile *file, const char *path);
bool script_file_copy(ScriptFile *file, const char *source, const char *destination);
bool script_file_move(ScriptFile *file, const char *source, const char *destination);
bool script_file_delete(ScriptFile *file, const char *path);

/* IniFile */
IniFile *ini_file_create(const char *path, ScriptFileAPI *api);
void ini_file_destroy(IniFile *ini);
void ini_file_rename(IniFile *ini, const char *new_path, bool reload);
const char *ini_file_get_file_name(const IniFile *ini);
bool ini_file_get_case_sensitive(const IniFile *ini);
void ini_file_set_case_sensitive(IniFile *ini, bool value);
bool ini_file_get_strip_quotes(const IniFile *ini);
void ini_file_set_strip_quotes(IniFile *ini, bool value);
bool ini_file_get_cache_updates(const IniFile *ini);
void ini_file_set_cache_updates(IniFile *ini, bool value);

/* StringList */
StringList *string_list_create(ScriptFileAPI *api);
void string_list_destroy(StringList *list);
int string_list_add(StringList *list, const char *s);
void string_list_append(StringList *list, const char *s);
void string_list_add_strings(StringList *list, StringList *other);
void string_list_clear(StringList *list);
void string_list_delete(StringList *list, int index);
int string_list_index_of(StringList *list, const char *s);
void string_list_insert(StringList *list, int index, const char *s);
void string_list_load_from_file(StringList *list, const char *file_name);
void string_list_save_to_file(StringList *list, const char *file_name);
void string_list_begin_update(StringList *list);
void string_list_end_update(StringList *list);
bool string_list_equals(StringList *list, StringList *other);
void string_list_exchange(StringList *list, int index1, int index2);
int string_list_index_of_name(StringList *list, const char *name);
void string_list_load_from_stream(StringList *list, MemoryStream *stream);
void string_list_move(StringList *list, int cur_index, int new_index);
void string_list_save_to_stream(StringList *list, MemoryStream *stream);
void string_list_set_text(StringList *list, const char *text);
int string_list_add_object(StringList *list, const char *s, void *object);
const char *string_list_get_text(StringList *list);
int string_list_index_of_object(StringList *list, void *object);
void string_list_insert_object(StringList *list, int index, const char *s, void *object);
bool string_list_find(StringList *list, const char *s, int *index);
void string_list_sort(StringList *list);
int string_list_get_count(const StringList *list);
const char *string_list_get_text_property(const StringList *list);
void string_list_set_text_property(StringList *list, const char *text);
const char *string_list_get_comma_text(const StringList *list);
void string_list_set_comma_text(StringList *list, const char *comma_text);
const char *string_list_get_string(const StringList *list, int index);
void string_list_set_string(StringList *list, int index, const char *value);
void *string_list_get_object(const StringList *list, int index);
void string_list_set_object(StringList *list, int index, void *object);
const char *string_list_get_name(const StringList *list, int index);
const char *string_list_get_value(const StringList *list, const char *key);
void string_list_set_value(StringList *list, const char *key, const char *value);
int string_list_get_duplicates(const StringList *list);
void string_list_set_duplicates(StringList *list, int duplicates);
bool string_list_get_sorted(const StringList *list);
void string_list_set_sorted(StringList *list, bool sorted);

/* MemoryStream (used as TFileStream) */
MemoryStream *memory_stream_create(ScriptFileAPI *api);
void memory_stream_destroy(MemoryStream *stream);
void memory_stream_clear(MemoryStream *stream);
void memory_stream_load_from_stream(MemoryStream *dest, MemoryStream *src);
void memory_stream_load_from_file(MemoryStream *stream, const char *file_name);
void memory_stream_save_to_stream(MemoryStream *src, MemoryStream *dest);
void memory_stream_save_to_file(MemoryStream *stream, const char *file_name);
void memory_stream_set_size(MemoryStream *stream, long new_size);
long memory_stream_read(MemoryStream *stream, void *buffer, long count);
long memory_stream_write(MemoryStream *stream, const void *buffer, long count);
void memory_stream_read_buffer(MemoryStream *stream, void *buffer, long count);
void memory_stream_write_buffer(MemoryStream *stream, const void *buffer, long count);
int64_t memory_stream_get_position(const MemoryStream *stream);
void memory_stream_set_position(MemoryStream *stream, int64_t pos);
int64_t memory_stream_get_size(const MemoryStream *stream);
void memory_stream_set_size_int64(MemoryStream *stream, int64_t size);

/* StringStream */
StringStream *string_stream_create(void);
void string_stream_destroy(StringStream *stream);
long string_stream_read(StringStream *stream, void *buffer, long count);
const char *string_stream_read_string(StringStream *stream, long count);
int64_t string_stream_seek(StringStream *stream, int64_t offset, int origin);
long string_stream_write(StringStream *stream, const void *buffer, long count);
void string_stream_write_string(StringStream *stream, const char *str);
const char *string_stream_get_data_string(const StringStream *stream);

/* Helper: copy file (used by script_file_copy) */
bool copy_file(const char *source, const char *destination, bool fail_if_exists);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPT_FILE_API_H */