#include "color.h"
#include "hack.h"
#include <jansson.h>

#include "json_io.h"

json_t *
role_state()
{
    json_t *options = json_array();
    for (int i = 0; roles[i].name.m; i++) {
        json_array_append_new(
            options, json_pack("{s: s, s: i, s: b}", "name",
                               (flags.initgend == 1 && roles[i].name.f)
                                   ? roles[i].name.f
                                   : roles[i].name.m, // I don't like this…
                               "id", i, "available",
                               ok_role(i, flags.initrace, flags.initgend,
                                       flags.initalign)));
    }
    return json_pack("{s: o, s:i}", "options", options, "current",
                     flags.initrole);
}

json_t *
race_state()
{
    json_t *options = json_array();
    for (int i = 0; races[i].noun; i++) {
        json_array_append_new(
            options, json_pack("{s: s, s: i, s: b}", "name",
                               races[i].noun, // I don't like this…
                               "id", i, "available",
                               ok_race(flags.initrole, i, flags.initgend,
                                       flags.initalign)));
    }
    return json_pack("{s: o, s:i}", "options", options, "current",
                     flags.initrace);
}

json_t *
gend_state()
{
    json_t *options = json_array();
    for (int i = 0; i < 2; i++) { // Hack to avoid neuter
        json_array_append_new(
            options, json_pack("{s: s, s: i, s: b}", "name", genders[i].adj,
                               "id", i, "available",
                               ok_gend(flags.initrole, flags.initrace, i,
                                       flags.initalign)));
    }
    return json_pack("{s: o, s:i}", "options", options, "current",
                     flags.initgend);
}

json_t *
align_state()
{
    json_t *options = json_array();
    for (int i = 0; i < 3; i++) {
        json_array_append_new(
            options, json_pack("{s: s, s: i, s: b}", "name",
                               aligns[i].adj, // I don't like this…
                               "id", i, "available",
                               ok_align(flags.initrole, flags.initrace,
                                        flags.initgend, i)));
    }
    return json_pack("{s: o, s:i}", "options", options, "current",
                     flags.initalign);
}

int
count_races()
{
    int i = 0;
    for (i = 0; races[i].noun; i++)
        ;
    return i;
}

void
validate_role()
{
    if (!ok_role(flags.initrole, flags.initrace, flags.initgend,
                 flags.initalign)) {
        flags.initrole = ROLE_NONE;
    }
}
void
validate_race()
{
    if (!ok_race(flags.initrole, flags.initrace, flags.initgend,
                 flags.initalign)) {
        flags.initrace = ROLE_NONE;
    }
}
void
validate_gend()
{
    if (!ok_gend(flags.initrole, flags.initrace, flags.initgend,
                 flags.initalign)) {
        flags.initgend = ROLE_NONE;
    };
}
void
validate_align()
{
    if (!ok_align(flags.initrole, flags.initrace, flags.initgend,
                  flags.initalign)) {
        flags.initalign = ROLE_NONE;
    };
}

void
json_player_selection()
{
    int max_race = count_races();
    int max_gend = 2;
    int max_align = 3;

    while (TRUE) {
        rigid_role_checks();
        json_send("select_character",
                  json_pack("{s: o, s: o, s: o, s: o}", "role", role_state(),
                            "race", race_state(), "gend", gend_state(),
                            "align", align_state()));
        json_t *resp = json_recieve();
        const char *type = json_string_value(json_object_get(resp, "type"));
        if (!strcmp(type, "set_character_attribute")) {
            json_t *message = json_object_get(resp, "message");
            const char *attribute =
                json_string_value(json_object_get(message, "attribute"));
            int value = json_integer_value(json_object_get(message, "value"));
            if (!strcmp(attribute, "role")) {
                if (validrole(value) || value == ROLE_RANDOM) {
                    flags.initrole = value;
                    validate_gend();
                    validate_align();
                    validate_race();
                }
            }
            if (!strcmp(attribute, "race")) {
                if ((value >= 0 && value < max_race)
                    || value == ROLE_RANDOM) {
                    flags.initrace = value;
                    validate_gend();
                    validate_align();
                    validate_role();
                }
            }
            if (!strcmp(attribute, "gend")) {
                if ((value >= 0 && value < max_gend)
                    || value == ROLE_RANDOM) {
                    flags.initgend = value;
                    validate_align();
                    validate_race();
                    validate_role();
                }
            }
            if (!strcmp(attribute, "align")) {
                if ((value >= 0 && value < max_align)
                    || value == ROLE_RANDOM) {
                    flags.initalign = value;
                    validate_gend();
                    validate_race();
                    validate_role();
                }
            }
        }
        if (!strcmp(type, "done") && flags.initrole != ROLE_NONE
            && flags.initrace != ROLE_NONE && flags.initgend != ROLE_NONE
            && flags.initalign != ROLE_NONE) {
            break;
        }
        json_decref(resp);
    }
}
