#include <pebble.h>
#include "rules.h"

static struct {
	Window *window;
	ScrollLayer *layer;
	TextLayer *textTitle;
	TextLayer *textRules;
} ui;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(ui.window);
  GRect bounds = layer_get_bounds(window_layer);
  ui.layer = scroll_layer_create(bounds);

  ui.textTitle = text_layer_create(GRect(0,0,144,35));
  text_layer_set_text(ui.textTitle,"Rules");
  text_layer_set_font(ui.textTitle,fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  scroll_layer_add_child(ui.layer,text_layer_get_layer(ui.textTitle));

  ui.textRules = text_layer_create(GRect(0,35,144,365));
  text_layer_set_text(ui.textRules,"The objective of the game is to close all 9 tiles and shut the box.\n\nRoll two dice and close any combination of tiles that equal the sum of the rolled dice.\n\nYou must use the sum of the dice each turn, if you cannot the game is over. Once the sum total of all the open tiles equals 6 or less, you have the choice of using one die instead of two.\n\n\nWritten by Daniel Head.\nme@danielhead.com\n\n\nV1.0.1");
  text_layer_set_overflow_mode(ui.textRules,GTextOverflowModeWordWrap);
  text_layer_set_size(ui.textRules,GSize(144,365));
  scroll_layer_add_child(ui.layer,text_layer_get_layer(ui.textRules));
  
  layer_add_child(window_layer,scroll_layer_get_layer(ui.layer));  
  scroll_layer_set_click_config_onto_window(ui.layer,ui.window);
  scroll_layer_set_content_size(ui.layer,GSize(144,400));


}

static void window_unload(Window *window) {
	scroll_layer_destroy(ui.layer);
	text_layer_destroy(ui.textTitle);
	text_layer_destroy(ui.textRules);
}

void rules_init(void) {
	ui.window = window_create();
	window_set_fullscreen(ui.window,true);
	window_set_window_handlers(ui.window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_stack_push(ui.window, true);
}
void rules_deinit(void) {
  window_destroy(ui.window);
}