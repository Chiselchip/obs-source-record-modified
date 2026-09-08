#pragma once
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
void source_record_status_init(void);
void source_record_status_shutdown(void);
void source_record_status_update(void *context, const char *name, bool active, bool paused);
void source_record_status_remove(void *context);
#ifdef __cplusplus
}
#endif
