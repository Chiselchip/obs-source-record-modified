#include <stddef.h>\n\n#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void source_record_get_status(char *buffer, size_t buffer_size);
void source_record_dock_create(void);
void source_record_dock_destroy(void);

#ifdef __cplusplus
}
#endif
