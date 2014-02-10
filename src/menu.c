#include <pebble.h>
#include "menu.h"
#include "rules.h"
#include "game.h"
#include "gameover.h"

static uint32_t resources[] = {
	RESOURCE_ID_MENU_BG
};
static struct {
	Window *window;
	Layer *layer;
	TextLayer *textPlay;
	TextLayer *textRules;
	GBitmap *background;
	GPath *pointer;
} ui;
static int choice = 0, counter = 0, combo[] = {0,0,2,2,0,2,0,2,1};
static GFont font;
static GPathInfo arrow = {
	.num_points = 3,
	.points = (GPoint []) {{-7, -7}, {7, 0}, {-7, 7}}
};

static void update_callback(Layer *me, GContext* ctx) {
	graphics_draw_bitmap_in_rect(ctx, ui.background, GRect(0,0,144,168));
	graphics_context_set_fill_color(ctx, GColorBlack);
	gpath_draw_filled(ctx, ui.pointer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(ui.window);
  font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  GRect bounds = layer_get_frame(window_layer);
  ui.layer = layer_create(bounds);
  layer_set_update_proc(ui.layer, update_callback);
  ui.background = gbitmap_create_with_resource(resources[0]);

  ui.textPlay = text_layer_create((GRect) { .origin = { 70, 32 }, .size = { bounds.size.w, 40 } });
  ui.textRules = text_layer_create((GRect) { .origin = { 70, 50 }, .size = { bounds.size.w, 40 } });
  text_layer_set_text(ui.textPlay, "Play");
  text_layer_set_text(ui.textRules, "Rules");
  //text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(ui.layer, text_layer_get_layer(ui.textPlay));
  layer_add_child(ui.layer, text_layer_get_layer(ui.textRules));
 
  ui.pointer = gpath_create(&arrow);
  gpath_move_to(ui.pointer, GPoint(55,40));
  layer_add_child(window_layer, ui.layer);
}

static void window_unload(Window *window) {
  text_layer_destroy(ui.textPlay);
  text_layer_destroy(ui.textRules);
  gpath_destroy(ui.pointer);
  gbitmap_destroy(ui.background);
}

static void toggle_menu(void) {
	if(choice==0) {
		choice=1;
		gpath_move_to(ui.pointer, GPoint(55,60));
	} else {
		choice=0;
		gpath_move_to(ui.pointer, GPoint(55,40));
	}
	layer_mark_dirty(ui.layer);

}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(combo[counter] == 1) {
    counter=0;
    gameover_init(false);
    return;
  }
  if(choice==0) {
  	game_init();
  } else {
  	rules_init();
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(combo[counter] == 0) counter++;
  else counter = 0;
  toggle_menu();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(combo[counter] == 2) counter++;
  else counter = 0;
  toggle_menu();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void menu_init(void) {
	ui.window = window_create();
	window_set_fullscreen(ui.window, true);
	window_set_click_config_provider(ui.window, click_config_provider);
	window_set_window_handlers(ui.window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_stack_push(ui.window, true);
}
void menu_deinit(void) {
  window_destroy(ui.window);
}