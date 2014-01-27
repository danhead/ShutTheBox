#include <pebble.h>

static struct {
	Window *window;
	Layer *cityscape;
	Layer *moon;
	Layer *anim;
	Layer *text;
} ui;
static struct {
	GBitmap *cityscape;
	GBitmap *moon;
} images;
static GPoint firework;
static PropertyAnimation *moon_animation, *cityscape_animation, *text_animation;
static AppTimer *timer;
static GFont font_l;
static bool exploded;
static int duration, particles, gravity, speed;
static char wintext[12];

static struct {
	int d;
	int s;
	int delay;
	GPoint pos;
} points[256];

void firework_start(int x) {
	particles = 256;
	duration = 0;
	gravity = 20;
	speed = 30;
	exploded = false;
	firework.x = x;
	firework.y = 160;
}
void explosion_start(void) {
	exploded = true;
	duration = 0;
	int d = 0;
	for(int c=0;c<particles;c++) {
		if(d>360) d-=360;
		d+=(rand()%250)+1;
		points[c].d = d*180;
		points[c].s = (rand()%10)+10;
		points[c].delay = (rand()%250);
		points[c].pos = (GPoint){
			.x = firework.x,
			.y = firework.y
		};
	}
	vibes_long_pulse();
}
void firework_update(void) {
	if(!exploded) {
		speed=(int16_t)(speed - (int32_t)(speed*gravity/100));
		firework.y-=speed;
		if(firework.y<(rand()%30)+40) {
			explosion_start();
		}
	}
	if(exploded && duration < 2500) {
		for(int c=0;c<particles;c++) {
			if(duration>points[c].delay) {
				points[c].s = (int16_t)(points[c].s - (int32_t)(points[c].s*gravity/100));
				int x = (int16_t)(sin_lookup(points[c].d) * (int32_t)points[c].s / TRIG_MAX_RATIO);
				x = (int16_t)(x - (int32_t)(x*gravity/100));
				int y = (int16_t)(-cos_lookup(points[c].d) * (int32_t)points[c].s / TRIG_MAX_RATIO);
				y = (int16_t)(y - (int32_t)(y*gravity/100));
				points[c].pos = (GPoint){
					.x = x + points[c].pos.x,
					.y = y + points[c].pos.y
				};
			}
		}
	}
}

static void timer_callback(void *context) {
	const uint32_t timeout_ms = 50;
	duration += 50;
	if(duration>2500) {
		firework_start(rand()%80+30);
	}
	firework_update();
	layer_mark_dirty(ui.anim);
	timer = app_timer_register(timeout_ms, timer_callback, NULL);
}
static void cityscape_update_callback(Layer *me, GContext* ctx) {
	graphics_draw_bitmap_in_rect(ctx, images.cityscape, GRect(0,0,250,168));
}
static void moon_update_callback(Layer *me, GContext* ctx) {
	graphics_draw_bitmap_in_rect(ctx, images.moon, GRect(0,0,32,33));
}
static void anim_update_callback(Layer *me, GContext* ctx) {
	graphics_context_set_stroke_color(ctx, GColorWhite);
	if(!exploded) {
		for(int c=0;c<3;c++) {
			for(int d=0;d<6;d++) {
				graphics_draw_pixel(ctx, GPoint((firework.x+c),(firework.y+d)));
			}
		}
	} else {
		for(int c=0;c<particles;c++) {
			for(int i=0;i<2;i++) {
				for(int j=0;j<2;j++) {
					GPoint point = GPoint(points[c].pos.x+i, points[c].pos.y+j);
					graphics_draw_pixel(ctx, point);
				}
			}
		}
	}

}

static void text_update_callback(Layer *me, GContext* ctx) {
	graphics_context_set_text_color(ctx, GColorWhite);
	graphics_draw_text(ctx,
		wintext,
		font_l,
		GRect(0, 0, 144, 29),
		GTextOverflowModeWordWrap,
		GTextAlignmentCenter,
		NULL);
}


static void window_load(Window *window) {
	light_enable(true);
	Layer *window_layer = window_get_root_layer(ui.window);
	font_l = fonts_get_system_font(FONT_KEY_GOTHIC_28);
	images.cityscape = gbitmap_create_with_resource(RESOURCE_ID_CITYSCAPE);
	images.moon = gbitmap_create_with_resource(RESOURCE_ID_MOON);
	ui.cityscape = layer_create((GRect){
		.origin = { 0, 90 },
		.size = { 250, 78 }
	});
	ui.moon = layer_create((GRect){
		.origin = { 120, 10 },
		.size = { 32, 33 }
	});
	ui.anim = layer_create((GRect){
		.origin = { 0, 0 },
		.size = { 144, 168 }
	});
	ui.text = layer_create((GRect){
		.origin = { 0, 45 },
		.size = { 144, 30 }
	});
	layer_set_update_proc(ui.cityscape, cityscape_update_callback);
	layer_set_update_proc(ui.moon, moon_update_callback);
	layer_set_update_proc(ui.anim, anim_update_callback);
	layer_set_update_proc(ui.text, text_update_callback);
	layer_add_child(window_layer, ui.moon);
	layer_add_child(window_layer, ui.cityscape);
	layer_add_child(window_layer, ui.anim);
	layer_add_child(window_layer, ui.text);

	GRect from_rect = GRect(120, 10, 32, 33);
	GRect to_rect = GRect(100, 10, 32, 33);
  	moon_animation = property_animation_create_layer_frame(ui.moon, &from_rect, &to_rect);
  	animation_set_duration((Animation*) moon_animation, 5000);
  	animation_set_delay((Animation*) moon_animation, 1000);
  	animation_schedule((Animation*) moon_animation);

  	from_rect = GRect(0, 90, 250, 78);
	to_rect = GRect(-50, 90, 250, 78);
  	cityscape_animation = property_animation_create_layer_frame(ui.cityscape, &from_rect, &to_rect);
  	animation_set_duration((Animation*) cityscape_animation, 5000);
  	animation_set_delay((Animation*) cityscape_animation, 1000);
  	animation_schedule((Animation*) cityscape_animation);

  	from_rect = GRect(0, 45, 144, 30);
	to_rect = GRect(-144, 45, 144, 30);
  	text_animation = property_animation_create_layer_frame(ui.text, &from_rect, &to_rect);
  	animation_set_duration((Animation*) text_animation, 4000);
  	animation_set_delay((Animation*) text_animation, 1000);
  	animation_schedule((Animation*) text_animation);

}
static void window_unload(Window *window) {
	app_timer_cancel(timer);
	light_enable(false);
	gbitmap_destroy(images.cityscape);
	gbitmap_destroy(images.moon);
}
void gameover_init(bool legit) {
	if(legit) snprintf(wintext, 12, "You win!");
	else snprintf(wintext, 12, "You Cheat!");
	ui.window = window_create();
	window_set_fullscreen(ui.window, true);
	window_set_background_color(ui.window, GColorBlack);
	window_set_window_handlers(ui.window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});
	window_stack_push(ui.window, true);
	const uint32_t timeout_ms = 50;
	timer = app_timer_register(timeout_ms, timer_callback, NULL);
}
void gameover_deinit(void) {
	window_destroy(ui.window);
}