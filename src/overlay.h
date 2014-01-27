#pragma once
#include <pebble.h>

void overlay_init(Layer *window_layer, GRect b);
void overlay_deinit(void);
void overlay_show(void);
void overlay_hide(void);
bool overlay_is_visible(void);
void overlay_click_up(void);
void overlay_click_down(void);
void overlay_click_select(void);
void do_callback(void (*thecallback)(void));