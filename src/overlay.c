#include <pebble.h>
static Layer *layer;
static GRect bounds;
static void update_callback(Layer *me, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx,
    GRect(0,0,bounds.size.w,bounds.size.h),
    8,
    GCornersAll);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx,
    GRect(3,3,bounds.size.w-6,bounds.size.h-6),
    8,
    GCornersAll);
}

void overlay_init(Layer *window_layer, GRect b) {
  bounds = b;
	layer = layer_create(b);
	layer_set_update_proc(layer, update_callback);
  layer_set_hidden(layer, true);
  layer_add_child(window_layer, layer);
}

void overlay_deinit(void) {
	layer_destroy(layer);
}
void overlay_show(void) {
  layer_set_hidden(layer, false);
}
void overlay_hide(void) {
  layer_set_hidden(layer, true);
}
bool overlay_is_visible(void) {
  return !layer_get_hidden(layer);
}
void overlay_click_up(void) {

}
void overlay_click_down(void) {

}
void overlay_click_select(void) {

}
void do_callback(void (*thecallback)(void)) {
	thecallback();
}