#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void source_record_dock_init(void);
void source_record_dock_set_paused(bool is_paused);

#ifdef __cplusplus
}
#endif
