#include <jansson.h>
#include "hack.h"
#include "flag.h"

#include "json_io.h"

json_t *
json_iflags()
{
    if (iflags.wc_align_message == 0) {
        iflags.wc_align_message = ALIGN_RIGHT;
    }
    if (iflags.wc_align_status == 0) {
        iflags.wc_align_status = ALIGN_BOTTOM;
    }
    return json_pack(
        "{"
        "s: i"   // align_message
        "s: i}", // align_status
        // "s: b"  // ascii_map
        // "s: b"  // color
        // "s: {"  // font
        // "s: {"  // map
        // "s: s?" // font
        // "s: i"  // size
        // "}"
        // "s: {"  // menu
        // "s: s?" // font
        // "s: i"  // size
        // "}"
        // "s: {"  // message
        // "s: s?" // font
        // "s: i"  // size
        // "}"
        // "s: {"  // status
        // "s: s?" // font
        // "s: i"  // size
        // "}"
        // "s: {"  // text
        // "s: s?" // font
        // "s: i"  // size
        // "}"
        // "}"
        // "s: b"  // hilite_pet
        // "s: i"  // player_selection
        // "s: b"  // popup_dialog
        // "s: b"  // splash_screen
        // "s: b"  // use_inverse
        // "s: {"  // windowcolors
        // "s: {"  // menu
        // "s: s?" // foreground
        // "s: s?" // background
        // "}"
        // "s: {"  // message
        // "s: s?" // foreground
        // "s: s?" // background
        // "}"
        // "s: {"  // status
        // "s: s?" // foreground
        // "s: s?" // background
        // "}"
        // "s: {"  // text
        // "s: s?" // foreground
        // "s: s?" // background
        // "}"
        // "}"
        // "s: b"   // mouse_support
        // "s: b"   // wraptext
        // "s: b"   // selectsaved
        // "s: b"   // hitpointbar
        // "s: i}", // windowborders
        "align_message", iflags.wc_align_message, "align_status",
        iflags.wc_align_status
        //"ascii_map", iflags.wc_ascii_map, "color",
        // iflags.wc_color, "font", "map", "font", iflags.wc_font_map,
        // "size", iflags.wc_fontsiz_map, "menu", "font",
        // iflags.wc_font_menu, "size", iflags.wc_fontsiz_menu, "message",
        // "font", iflags.wc_font_message, "size",
        // iflags.wc_fontsiz_message, "status", "font",
        // iflags.wc_font_status, "size", iflags.wc_fontsiz_status,
        // "text", "font", iflags.wc_font_text, "size",
        // iflags.wc_fontsiz_text, "hilite_pet", iflags.wc_hilite_pet,
        // "player_selection", iflags.wc_player_selection, "popup_dialog",
        // iflags.wc_popup_dialog, "splash_screen",
        // iflags.wc_splash_screen, "inverse", iflags.wc_inverse,
        // "windowcolors", "menu" "foreground", iflags.wc_foregrnd_menu,
        // "background", iflags.wc_backgrnd_menu, "message", "foreground",
        // iflags.wc_foregrnd_message, "background",
        // iflags.wc_backgrnd_message, "status", "foreground",
        // iflags.wc_foregrnd_status, "background",
        // iflags.wc_backgrnd_status, "text", "foreground",
        // iflags.wc_foregrnd_text, "background", iflags.wc_backgrnd_text,
        // "mouse_support", iflags.wc_mouse_support, "wraptext",
        // iflags.wc2_wraptext, "selectsaved", iflags.wc2_selectsaved,
        // "hitpointbar", iflags.wc2_hitpointbar, "windowborders",
        // iflags.wc2_windowborders
    );
}

void
json_send(const char *type, json_t *message)
{
    json_t *json =
        json_pack("{s: s?,s: o?}", "type", type, "message", message);
    json_dumpf(json, stdout, 0);
    printf("\n");
    json_decref(json);
}

json_t *
json_recieve()
{
    json_error_t error;
    json_t *json = json_loadf(stdin, JSON_DISABLE_EOF_CHECK, &error);
    if (!json) {
        panic("Failed to read JSON input: %s in %s %i:%i", error.text,
              error.source, error.line, error.column);
    }
    return json;
}
