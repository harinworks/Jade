#ifndef AMALGAMATED_BUILD
#include "../button_events.h"
#include "../ui.h"
#include "jade_assert.h"

static gui_activity_t* make_sign_identity_activity(const char* identity)
{
    JADE_ASSERT(identity);

    // third row, buttons
    btn_data_t hdrbtns[] = { { .txt = "=", .font = JADE_SYMBOLS_16x16_FONT, .ev_id = BTN_SIGNIDENTITY_REJECT },
        { .txt = "S", .font = VARIOUS_SYMBOLS_FONT, .ev_id = BTN_SIGNIDENTITY_ACCEPT } };

    gui_activity_t* const act = gui_make_activity();
    gui_view_node_t* const parent = add_title_bar(act, "Sign Identity", hdrbtns, 2, NULL);
    gui_view_node_t* node;

    gui_make_text(&node, identity, TFT_WHITE);
    gui_set_parent(node, parent);
    gui_set_padding(node, GUI_MARGIN_ALL_DIFFERENT, 24, 0, 0, 4);
    gui_set_align(node, GUI_ALIGN_LEFT, GUI_ALIGN_TOP);

    return act;
}

bool show_sign_identity_activity(const char* identity, const size_t identity_len)
{
    JADE_ASSERT(identity);
    JADE_ASSERT(identity_len < MAX_DISPLAY_MESSAGE_LEN);

    char display_str[MAX_DISPLAY_MESSAGE_LEN];
    int ret = snprintf(display_str, sizeof(display_str), "%.*s", identity_len, identity);
    JADE_ASSERT(ret > 0 && ret < sizeof(display_str));

    gui_activity_t* const act = make_sign_identity_activity(display_str);
    gui_set_current_activity(act);
    int32_t ev_id;

    while (true) {
        // In a debug unattended ci build, assume 'accept' button pressed after a short delay
#ifndef CONFIG_DEBUG_UNATTENDED_CI
        const bool ret = gui_activity_wait_event(act, GUI_BUTTON_EVENT, ESP_EVENT_ANY_ID, NULL, &ev_id, NULL, 0);
#else
        gui_activity_wait_event(act, GUI_BUTTON_EVENT, ESP_EVENT_ANY_ID, NULL, &ev_id, NULL,
            CONFIG_DEBUG_UNATTENDED_CI_TIMEOUT_MS / portTICK_PERIOD_MS);
        const bool ret = true;
        ev_id = BTN_SIGNIDENTITY_ACCEPT;
#endif

        // Check to see whether user accepted or declined
        if (ret) {
            switch (ev_id) {
            case BTN_SIGNIDENTITY_REJECT:
                return false;

            case BTN_SIGNIDENTITY_ACCEPT:
                return true;
            }
        }
    }
}
#endif // AMALGAMATED_BUILD
