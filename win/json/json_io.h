#pragma once

#include <jansson.h>

json_t *json_iflags();

void json_send(const char *type, json_t *message);
json_t *json_recieve();
