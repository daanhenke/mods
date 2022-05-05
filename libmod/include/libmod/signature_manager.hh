#pragma once

typedef struct
{

} sig_mgr_ctx_t;

sig_mgr_ctx_t ctx;

void sig_add_offset(const char* name, const char* signature, const char* module_name);
