#include <jansson.h>
#include "hack.h"
#include "color.h"
#include "date.h"
#include "display.h"
#include "dlb.h"
#include "inttypes.h"
#include "patchlevel.h"
#include "wintype.h"
#include "func_tab.h"
#include "permonst.h"

#include "json_io.h"
#include "json_player_selection.h"

void
json_init_nhwindows(int *argcp UNUSED, char **argv UNUSED)
{
    iflags.use_background_glyph = TRUE;
    json_send("init_nhwindows",
              json_pack("{s: [ssss], s: o}", "banner", COPYRIGHT_BANNER_A,
                        COPYRIGHT_BANNER_B, COPYRIGHT_BANNER_C,
                        COPYRIGHT_BANNER_D, "config", json_iflags()));
}

void
json_askname()
{
    json_send("askname", json_object());
    json_t *resp = json_recieve();
    const char *type = json_string_value(json_object_get(resp, "type"));
    if (type && !strcmp(type, "setname")
        && json_string_value(json_object_get(resp, "message"))) {
        strlcpy(plname, json_string_value(json_object_get(resp, "message")),
                PL_NSIZ);
    } else {
        strlcpy(plname, "InvalidNameJSON", PL_NSIZ);
    }
    json_decref(resp);
}

void
json_get_nh_event()
{
}

void
json_exit_nhwindows(const char *str)
{
    json_send("exit_nhwindows", json_string(str));
}

void
json_suspend_nhwindows()
{
    // not supported for now
}

void
json_resume_nhwindows()
{
    // not supported for now
}

const char *
window_type_to_string(int type)
{
    switch (type) {
    case NHW_MESSAGE:
        return "message";
    case NHW_MAP:
        return "map";
    case NHW_STATUS:
        return "status";
    case NHW_MENU:
        return "menu";
    case NHW_TEXT:
        return "text";
    default:
        return NULL;
    }
}

winid
json_create_nhwindow(int type)
{
    json_send("create_nhwindow",
              json_pack("{s:s}", "type", window_type_to_string(type)));
    json_t *resp = json_recieve();
    winid id = 0;
    if (json_unpack(resp, "{s: {s: i}}", "message", "id", &id) == -1) {
        panic("JSON error creating window");
    }
    json_decref(resp);
    return id;
}

void
json_clear_nhwindow(winid id)
{
    json_send("clear_nhwindow", json_pack("{s:i}", "id", id));
}

void
json_display_nhwindow(winid id, int blocking)
{
    json_send("display_nhwindow",
              json_pack("{s:i,s:b}", "id", id, "blocking", blocking));
    if (blocking) {
        json_decref(json_recieve());
    }
}

void
json_destroy_nhwindow(winid id)
{
    json_send("destroy_nhwindow", json_pack("{s:i}", "id", id));
}

void
json_curs(winid id, int x, int y)
{
    json_send("curs", json_pack("{s:i,s:i,s:i}", "id", id, "x", x, "y", y));
}

const char *
attr_to_string(int attr)
{
    switch (attr) {
    case ATR_NONE:
        return NULL;
    case ATR_ULINE:
        return "uline";
    case ATR_BOLD:
        return "bold";
    case ATR_BLINK:
        return "blink";
    case ATR_INVERSE:
        return "inverse";
    default:
        return NULL;
    }
}

void
json_putstr(winid id, int attr, const char *str)
{
    json_send("putstr", json_pack("{s:i,s:s?,s:s}", "id", id, "attr",
                                  attr_to_string(attr), "str", str));
}

void
json_display_file(const char *path, int complain)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        if (complain) {
            fprintf(stderr, "Can't find file %s", path);
        }
        return;
    }
    char *buffer = NULL;
    size_t len;
    ssize_t bytes_read = getdelim(&buffer, &len, '\0', file);
    if (bytes_read != -1) {
        json_send("display_file", json_pack("{s:s%}", "text", buffer, len));
    }
}

void
json_start_menu(winid id)
{
    json_send("start_menu", json_pack("{s:i}", "id", id));
}

const char *
color_to_string(int color)
{
    if (color == CLR_BLACK) {
        return "black";
    } else if (color == CLR_RED) {
        return "red";
    } else if (color == CLR_GREEN) {
        return "green";
    } else if (color == CLR_BROWN) {
        return "brown";
    } else if (color == CLR_BLUE) {
        return "blue";
    } else if (color == CLR_MAGENTA) {
        return "magenta";
    } else if (color == CLR_CYAN) {
        return "cyan";
    } else if (color == CLR_GRAY) {
        return "gray";
    } else if (color == NO_COLOR) {
        return "no_color";
    } else if (color == CLR_ORANGE) {
        return "orange";
    } else if (color == CLR_BRIGHT_GREEN) {
        return "bright_green";
    } else if (color == CLR_YELLOW) {
        return "yellow";
    } else if (color == CLR_BRIGHT_BLUE) {
        return "bright_blue";
    } else if (color == CLR_BRIGHT_MAGENTA) {
        return "bright_magenta";
    } else if (color == CLR_WHITE) {
        return "white";
    } else {
        return NULL;
    }
}

const char *cmap_to_string[MAXPCHARS] = { "stone",
                                          "vwall",
                                          "hwall",
                                          "tlcorn",
                                          "trcorn",
                                          "blcorn",
                                          "brcorn",
                                          "crwall",
                                          "tuwall",
                                          "tdwall",
                                          "tlwall",
                                          "trwall",
                                          "ndoor",
                                          "vodoor",
                                          "hodoor",
                                          "vcdoor",
                                          "hcdoor",
                                          "bars",
                                          "tree",
                                          "room",
                                          "darkroom",
                                          "corr",
                                          "litcorr",
                                          "upstair",
                                          "dnstair",
                                          "upladder",
                                          "dnladder",
                                          "altar",
                                          "grave",
                                          "throne",
                                          "sink",
                                          "fountain",
                                          "pool",
                                          "ice",
                                          "lava",
                                          "vodbridge",
                                          "hodbridge",
                                          "vcdbridge",
                                          "hcdbridge",
                                          "air",
                                          "cloud",
                                          "water",
                                          "arrow_trap",
                                          "dart_trap",
                                          "falling_rock_trap",
                                          "squeaky_board",
                                          "bear_trap",
                                          "land_mine",
                                          "rolling_boulder_trap",
                                          "sleeping_gas_trap",
                                          "rust_trap",
                                          "fire_trap",
                                          "pit",
                                          "spiked_pit",
                                          "hole",
                                          "trap_door",
                                          "teleportation_trap",
                                          "level_teleporter",
                                          "magic_portal",
                                          "web",
                                          "statue_trap",
                                          "magic_trap",
                                          "anti_magic_trap",
                                          "polymorph_trap",
                                          "vibrating_square",
                                          "hbeam",
                                          "lslant",
                                          "rslant",
                                          "digbeam",
                                          "flashbeam",
                                          "boomleft",
                                          "boomright",
                                          "ss1",
                                          "ss2",
                                          "ss3",
                                          "ss4",
                                          "poisoncloud",
                                          "goodpos",
                                          "sw_tl",
                                          "sw_tc",
                                          "sw_tr",
                                          "sw_ml",
                                          "sw_mr",
                                          "sw_bl",
                                          "sw_bc",
                                          "sw_br",
                                          "explode1",
                                          "explode2",
                                          "explode3",
                                          "explode4",
                                          "explode5",
                                          "explode6",
                                          "explode7",
                                          "explode8",
                                          "explode9" };

json_t *
glyph_to_json(int glyph, int x, int y)
{
    if (!iflags.use_background_glyph) {
        panic("bkg off?");
    }
    if (glyph == NO_GLYPH) {
        return json_null();
    }

    int thechar;
    int color;
    unsigned int special;
    mapglyph(glyph, &thechar, &color, &special, x, y);
    json_t *special_json = json_array();
    if (special & MG_CORPSE) {
        json_array_append_new(special_json, json_string("corpse"));
    }
    if (special & MG_INVIS) {
        json_array_append_new(special_json, json_string("invis"));
    }
    if (special & MG_DETECT) {
        json_array_append_new(special_json, json_string("detect"));
    }
    if (special & MG_PET) {
        json_array_append_new(special_json, json_string("pet"));
    }
    if (special & MG_RIDDEN) {
        json_array_append_new(special_json, json_string("ridden"));
    }
    if (special & MG_STATUE) {
        json_array_append_new(special_json, json_string("statue"));
    }
    if (special & MG_OBJPILE) {
        json_array_append_new(special_json, json_string("objpile"));
    }
    json_t *ret =
        json_pack("{s:i,s:s#,s:s?,s:o}", "id", glyph, "char", &thechar, 1,
                  "color", color_to_string(color), "special", special_json);
    const char *type = 0;
    if (glyph_is_monster(glyph)) {
        type = "monster";
        struct permonst monst = mons[glyph_to_mon(glyph)];
        json_object_set_new(ret, "monster", json_string(monst.mname));
    } else if (glyph_is_body(glyph)) {
        type = "corpse";
    } else if (glyph_is_cmap(glyph)) {
        type = "cmap";
        json_object_set_new(
            ret, "symbol", json_string(cmap_to_string[glyph_to_cmap(glyph)]));
    }
    json_object_set_new(ret, "type", type ? json_string(type) : json_null());
    return ret;
}

// We can't just take raw identifiers from the client (if they're used as
// pointers, the client couldmake pad things happen) so we map them to IDs
// here and send those to the client.
struct menu_identifier {
    boolean used;
    anything identifier;
    winid winid;
};

// Hack: item 0 is always used
struct menu_identifier
    menu_identifiers[10000 * sizeof(struct menu_identifier)] = {
        { 1, { 0 }, -1 },
        { 0 }
    };

void
json_add_menu(winid id, int glyph, const anything *identifier,
              int accelerator, int groupacc, int attr, const char *str,
              int preselected)
{
    int identifier_id;
    if (identifier->a_int == 0) {
        identifier_id = 0;
    } else {
        for (identifier_id = 0; identifier_id < SIZE(menu_identifiers);
             identifier_id++) {
            if (!menu_identifiers[identifier_id].used) {
                menu_identifiers[identifier_id].used = TRUE;
                menu_identifiers[identifier_id].identifier = *identifier;
                menu_identifiers[identifier_id].winid = id;
                break;
            }
        }
        if (identifier_id == SIZE(menu_identifiers)) {
            impossible("Too many menu items");
            return;
        }
    }
    json_send("add_menu",
              json_pack("{s:i,s:o?,s:i,s:s#,s:s#,s:s?,s:s,s:b}", "id", id,
                        "glyph", glyph_to_json(glyph, 0, 0), "identifier",
                        identifier_id, "accelerator", &accelerator, 1,
                        "groupacc", &groupacc, 1, "attr",
                        attr_to_string(attr), "str", str, "preselected",
                        preselected));
}

void
json_end_menu(winid id, const char *prompt)
{
    json_send("end_menu",
              json_pack("{s:i,s:s?}", "id", id, "prompt", prompt));
}

int
json_select_menu(winid id, int how, menu_item **selected)
{
    json_send("select_menu",
              json_pack("{s:i,s:s}", "id", id, "how",
                        ((how == PICK_ANY)
                             ? "any"
                             : ((how == PICK_ONE) ? "one" : "none"))));
    json_t *resp = json_recieve();
    json_t *selected_options;
    *selected = NULL;
    if (json_unpack(resp, "{s: {s: o}}", "message", "selected",
                    &selected_options)
        == -1) {
        json_decref(resp);
        return 0;
    }
    if (json_array_size(selected_options) > 0) {
        *selected = malloc(
            json_array_size(selected_options)
            * sizeof(menu_item)); // Do I need to use a NetHack version?
        size_t idx;
        json_t *option;
        json_array_foreach(selected_options, idx, option)
        {
            int count = 0;
            int identifier_id;
            if (json_unpack(option, "{s: i, s:i}", "identifier",
                            &identifier_id, "count", &count)
                == -1) {
                json_decref(resp);
                *selected = NULL;
                return 0;
            };
            struct menu_identifier idstruct = menu_identifiers[identifier_id];
            if (idstruct.used == TRUE && idstruct.winid == id) {
                (*selected)[idx].item = idstruct.identifier;
            } else {
                json_decref(resp);
                *selected = NULL;
                return 0;
            }
            (*selected)[idx].count = count;
        }
    }
    int ret = json_array_size(selected_options);
    json_decref(resp);
    return ret;
}

void
json_update_inventory()
{
    json_send("update_inventory_start", json_object());
    display_inventory(NULL, FALSE);
    json_send("update_inventory_end", json_object());
}

void
json_mark_synch()
{
}

void
json_wait_synch()
{
}

void
json_cliparound(int x, int y)
{
    json_send("cliparound", json_pack("{s:i,s:i}", "x", x, "y", y));
}

void
json_print_glyph(winid id, XCHAR_P x, XCHAR_P y, int glyph, int bkglyph)
{
    json_send("print_glyph",
              json_pack("{s:i,s:i,s:i,s:o,s:o,s:i}", "id", id, "x", x, "y", y,
                        "glyph", glyph_to_json(glyph, x, y), "bkglyph",
                        glyph_to_json(bkglyph, x, y), "_bkg", bkglyph));
}

void
json_raw_print(const char *str)
{
    json_send("raw_print", json_pack("{s:s}", "str", str));
}

void
json_raw_print_bold(const char *str)
{
    json_send("raw_print_bold", json_pack("{s:s}", "str", str));
}

int
json_nhgetch()
{
    // docs say something here about SAFERHANGUP. Maybe we can get away
    // without b/c users can't cause a HUP? need to investigate
    json_send("nhgetch", json_object());
    json_t *resp = json_recieve();
    int key;
    if (json_unpack(resp, "{s: {s: i}}", "message", "key", &key) == -1) {
        json_decref(resp);
        return '.';
    };
    json_decref(resp);
    return key;
}

int
json_nh_poskey(int *x, int *y, int *mod)
{
    // docs say something here about SAFERHANGUP. Maybe we can get away
    // without b/c users can't cause a HUP? need to investigate
    json_send("nh_poskey", json_object());
    json_t *resp = json_recieve();
    json_t *message;
    if (json_unpack(resp, "{s: o}", "message", &message) == -1) {
        json_decref(resp);
        return '.';
    }
    json_t *mouse;
    if ((mouse = json_object_get(message, "mouse"))) {
        json_unpack(resp, "{s:i,s:i,s:i}", "x", &x, "y", &y, "mod", &mod);
        json_decref(resp);
        return 0;
    } else {
        int key;
        json_unpack(resp, "{s: {s: i}}", "message", "key", &key);
        json_decref(resp);
        return key;
    }
}

void
json_nhbell()
{
    json_send("nhbell", json_object());
}

int
json_doprev_message()
{
    json_send("doprev_message", json_object());
    return 0; //???
}

void
json_number_pad()
{
}

void
json_delay_output()
{
    json_send("delay_output", json_object());
}

void
json_start_screen()
{
}

void
json_end_screen()
{
}

void
json_preference_update(const char *preference)
{
    json_send("preference_update",
              json_pack("{s:o,s:s}", "config", json_iflags(), "changed",
                        preference));
}

//      -- Print a prompt made up of ques, choices and default.
// 		   Read a single character response that is contained in
// 		   choices or default.  If choices is NULL, all possible
// 		   inputs are accepted and returned.  This overrides
// 		   everything else.  The choices are expected to be in
// 		   lower case.  Entering ESC always maps to 'q', or 'n',
// 		   in that order, if present in choices, otherwise it maps
// 		   to default.  Entering any other quit character (SPACE,
// 		   RETURN, NEWLINE) maps to default.
// 		-- If the choices string contains ESC, then anything after
// 		   it is an acceptable response, but the ESC and whatever
// 		   follows is not included in the prompt.
// 		-- If the choices string contains a '#' then accept a count.
// 		   Place this value in the global "yn_number" and return '#'.
// 		-- This uses the top line in the tty window-port, other
// 		   ports might use a popup.
// 		-- If choices is NULL, all possible inputs are accepted and
// 		   returned, preserving case (upper or lower.) This means that
// 		   if the calling function needs an exact match, it must
// 		   handle user input correctness itself.
// 		-- ques should not be more than QBUFSZ-1 characters long.
char
json_yn_function(const char *ques, const char *choices, int def)
{
    json_send("yn_function",
              json_pack("{s:s,s:s?,s:s#}", "ques", ques, "choices", choices,
                        "default", &def, 1));
    json_t *resp = json_recieve();
    char *key_str;
    if (json_unpack(resp, "{s:{s:s}}", "message", "key", &key_str) == -1) {
        return def;
    }
    if (strlen(key_str) != 1) {
        return def;
    }
    char key = key_str[0];
    if (choices && (!strchr(choices, key) || (key == '\033'))) {
        return def;
    }
    json_t *number_json = json_object_get(resp, "number");
    if (choices && strchr(choices, '#') && (key == '#') && number_json
        && json_is_integer(number_json)) {
        yn_number = json_integer_value(number_json);
    } else if (choices && (key == '#')) {
        return def;
    }
    return key;
}

void
json_getlin(const char *ques, char *input)
{
    json_send("getlin", json_pack("{s:s}", "ques", ques));
    json_t *resp = json_recieve();
    char *input_unclipped;
    if (json_unpack(resp, "{s:{s:s}}", "message", "input", &input_unclipped)
        == -1) {
        strlcpy(input, "", BUFSZ);
        return;
    }
    strlcpy(input, input_unclipped, BUFSZ);
}

int
json_get_ext_cmd()
{
    json_t *commands = json_array();
    int extcmd_count;
    for (int i = 0; extcmdlist[i].key == '\0'; i++) {
        json_array_append_new(
            commands,
            json_pack("{s:i, s:s, s:s, s:b}", "id", i, "text",
                      extcmdlist[i].ef_txt, "desc", extcmdlist[i].ef_desc,
                      "autocomplete",
                      (extcmdlist[i].flags & AUTOCOMPLETE) == AUTOCOMPLETE));
        extcmd_count = i;
    }
    json_send("get_ext_cmd", json_pack("{s:o}", "commands", commands));
    json_t *resp = json_recieve();
    int command;
    if (json_unpack(resp, "{s:{s:s}}", "message", "id", &command) == -1) {
        return -1;
    }
    if (command >= extcmd_count) {
        return -1;
    }
    return command;
}

void
json_status_init()
{
    json_send("status_init", json_object());
}

void
json_status_enablefield(int fldindex UNUSED, const char *fldname,
                        const char *fieldfmt UNUSED, int enable)
{
    json_send("status_enablefield",
              json_pack("{s:s,s:b}", "fieldname", fldname, "enable", enable));
}

const char *status_field_names[MAXBLSTATS] = {
    "title",         "strength",
    "dexterity",     "constitution",
    "intelligence",  "wisdom",
    "charisma",      "alignment",
    "score",         "carrying-capacity",
    "gold",          "power",
    "power-max",     "experience-level",
    "armor-class",   "HD",
    "time",          "hunger",
    "hitpoints",     "hitpoints-max",
    "dungeon-level", "experience",
    "condition"
};

void
add_condition(json_t *array, const char *name, long bitmask,
              unsigned long *colormasks)
{
    const char *color = NULL;
    if (colormasks[CLR_BLACK] & bitmask) {
        color = "black";
    }
    if (colormasks[CLR_RED] & bitmask) {
        color = "red";
    }
    if (colormasks[CLR_GREEN] & bitmask) {
        color = "green";
    }
    if (colormasks[CLR_BROWN] & bitmask) {
        color = "brown";
    }
    if (colormasks[CLR_BLUE] & bitmask) {
        color = "blue";
    }
    if (colormasks[CLR_MAGENTA] & bitmask) {
        color = "magenta";
    }
    if (colormasks[CLR_CYAN] & bitmask) {
        color = "cyan";
    }
    if (colormasks[CLR_GRAY] & bitmask) {
        color = "gray";
    }
    if (colormasks[NO_COLOR] & bitmask) {
        color = "no_color";
    }
    if (colormasks[CLR_ORANGE] & bitmask) {
        color = "orange";
    }
    if (colormasks[CLR_BRIGHT_GREEN] & bitmask) {
        color = "bright_green";
    }
    if (colormasks[CLR_YELLOW] & bitmask) {
        color = "yellow";
    }
    if (colormasks[CLR_BRIGHT_BLUE] & bitmask) {
        color = "bright_blue";
    }
    if (colormasks[CLR_BRIGHT_MAGENTA] & bitmask) {
        color = "bright_magenta";
    }
    if (colormasks[CLR_WHITE] & bitmask) {
        color = "white";
    }
    json_t *attributes = json_array();
    if (colormasks[HL_ATTCLR_DIM] & bitmask) {
        json_array_append_new(attributes, json_string("dim"));
    }
    if (colormasks[HL_ATTCLR_BLINK] & bitmask) {
        json_array_append_new(attributes, json_string("blink"));
    }
    if (colormasks[HL_ATTCLR_ULINE] & bitmask) {
        json_array_append_new(attributes, json_string("uline"));
    }
    if (colormasks[HL_ATTCLR_INVERSE] & bitmask) {
        json_array_append_new(attributes, json_string("inverse"));
    }
    if (colormasks[HL_ATTCLR_BOLD] & bitmask) {
        json_array_append_new(attributes, json_string("bold"));
    }
    json_array_append_new(array,
                          json_pack("{s:s,s:s?,s:o}", "name", name, "color",
                                    color, "attributes", attributes));
}

void
json_status_update(int fldindex, genericptr_t ptr, int chg UNUSED,
                   int percentage, int color, unsigned long *colormasks)
{
    const char *fieldname;
    if (fldindex < 0) {
        switch (fldindex) {
        case BL_CHARACTERISTICS:
            fieldname = "characteristics";
            break;
        case BL_RESET:
            fieldname = "reset";
            break;
        case BL_FLUSH:
            fieldname = "flush";
            break;
        }
    } else {
        fieldname = status_field_names[fldindex];
    }
    json_t *value = 0;
    if (fldindex == BL_CONDITION) {
        long bitmask = (long) ptr;
        value = json_array();
        if ((bitmask & BL_MASK_SLIME) == BL_MASK_SLIME) {
            add_condition(value, "slime", BL_MASK_SLIME, colormasks);
        }
        if ((bitmask & BL_MASK_STONE) == BL_MASK_STONE) {
            add_condition(value, "stone", BL_MASK_STONE, colormasks);
        }
        if ((bitmask & BL_MASK_STRNGL) == BL_MASK_STRNGL) {
            add_condition(value, "strngl", BL_MASK_STRNGL, colormasks);
        }
        if ((bitmask & BL_MASK_FOODPOIS) == BL_MASK_FOODPOIS) {
            add_condition(value, "foodpois", BL_MASK_FOODPOIS, colormasks);
        }
        if ((bitmask & BL_MASK_TERMILL) == BL_MASK_TERMILL) {
            add_condition(value, "termill", BL_MASK_TERMILL, colormasks);
        }
        if ((bitmask & BL_MASK_BLIND) == BL_MASK_BLIND) {
            add_condition(value, "blind", BL_MASK_BLIND, colormasks);
        }
        if ((bitmask & BL_MASK_DEAF) == BL_MASK_DEAF) {
            add_condition(value, "deaf", BL_MASK_DEAF, colormasks);
        }
        if ((bitmask & BL_MASK_STUN) == BL_MASK_STUN) {
            add_condition(value, "stun", BL_MASK_STUN, colormasks);
        }
        if ((bitmask & BL_MASK_CONF) == BL_MASK_CONF) {
            add_condition(value, "conf", BL_MASK_CONF, colormasks);
        }
        if ((bitmask & BL_MASK_HALLU) == BL_MASK_HALLU) {
            add_condition(value, "hallu", BL_MASK_HALLU, colormasks);
        }
        if ((bitmask & BL_MASK_LEV) == BL_MASK_LEV) {
            add_condition(value, "lev", BL_MASK_LEV, colormasks);
        }
        if ((bitmask & BL_MASK_FLY) == BL_MASK_FLY) {
            add_condition(value, "flying", BL_MASK_FLY, colormasks);
        }
        if ((bitmask & BL_MASK_RIDE) == BL_MASK_RIDE) {
            add_condition(value, "ride", BL_MASK_RIDE, colormasks);
        }
    } else if (ptr) {
        value = json_string(ptr);
    }
    const char *color_string = 0;
    const char *attribute = 0;
    if (fldindex != BL_CONDITION) {
        int real_color = color & 0x0FF;
        int attribute_bits = color >> 8;
        color_string = color_to_string(real_color);
        if (attribute_bits == HL_ATTCLR_DIM) {
            attribute = "dim";
        }
        if (attribute_bits == HL_ATTCLR_BLINK) {
            attribute = "blink";
        }
        if (attribute_bits == HL_ATTCLR_ULINE) {
            attribute = "uline";
        }
        if (attribute_bits == HL_ATTCLR_INVERSE) {
            attribute = "inverse";
        }
        if (attribute_bits == HL_ATTCLR_BOLD) {
            attribute = "bold";
        }
    }
    json_send("status_update",
              json_pack("{s:s,s:o?,s:i,s:s?,s:s?}", "fieldname", fieldname,
                        "value", value, "percentage", percentage, "color",
                        color_string, "attribute", attribute));
}

void
json_status_finish()
{
    json_send("status_finish", json_object());
}

struct window_procs json_procs = {
    "json",
    (WC_ALIGN_MESSAGE | WC_ALIGN_STATUS | WC_ASCII_MAP | WC_COLOR
     | WC_FONT_MAP | WC_FONT_MENU | WC_FONT_MESSAGE | WC_FONT_STATUS
     | WC_FONT_TEXT | WC_FONTSIZ_MAP | WC_FONTSIZ_MENU | WC_FONTSIZ_MESSAGE
     | WC_FONTSIZ_STATUS | WC_FONTSIZ_TEXT | WC_HILITE_PET
     | WC_PLAYER_SELECTION | WC_POPUP_DIALOG | WC_SPLASH_SCREEN | WC_INVERSE
     | WC_WINDOWCOLORS | WC_MOUSE_SUPPORT),
    (WC2_WRAPTEXT | WC2_SELECTSAVED | WC2_HITPOINTBAR | WC2_WINDOWBORDERS),
    json_init_nhwindows,
    json_player_selection,
    json_askname,
    json_get_nh_event,
    json_exit_nhwindows,
    json_suspend_nhwindows,
    json_resume_nhwindows,
    json_create_nhwindow,
    json_clear_nhwindow,
    json_display_nhwindow,
    json_destroy_nhwindow,
    json_curs,
    json_putstr,
    genl_putmixed,
    json_display_file,
    json_start_menu,
    json_add_menu,
    json_end_menu,
    json_select_menu,
    genl_message_menu,
    json_update_inventory,
    json_mark_synch,
    json_wait_synch,
#ifdef CLIPPING
    json_cliparound,
#endif
#ifdef POSITIONBAR
    donull,
#endif
    json_print_glyph,
    json_raw_print,
    json_raw_print_bold,
    json_nhgetch,
    json_nh_poskey,
    json_nhbell,
    json_doprev_message,
    json_yn_function,
    json_getlin,
    json_get_ext_cmd,
    json_number_pad,
    json_delay_output,
#ifdef CHANGE_COLOR
    json_change_color,
#ifdef MAC
    json_change_background,
    json_set_font_name
#endif
        json_get_color_string,
#endif
    json_start_screen,
    json_end_screen,
    genl_outrip,
    json_preference_update,
    genl_getmsghistory,
    genl_putmsghistory,
    json_status_init,
    json_status_finish,
    json_status_enablefield,
    json_status_update,
    genl_can_suspend_no, // Until we figure out how this works
};
