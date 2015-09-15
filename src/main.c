#include <pebble.h>

static Window *s_main_window;

// These display the actual information
static TextLayer *s_week_year_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_day_layer;
static TextLayer *s_bluetooth_layer;

// These create some padding for the status bar text
static Layer *s_week_year_space_layer;
static Layer *s_battery_space_layer;
static Layer *s_day_space_layer;
static Layer *s_bluetooth_space_layer;

// Remember if bluetooth handler call is initial
static bool bluetooth_handler_init = true;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Declare buffers to hold the formatted strings
  static char s_week_year_buffer[12];
  static char s_time_buffer[6];
  static char s_date_buffer[13];
  static char s_day_buffer[10];
  
  // Format time to strings
  strftime(s_week_year_buffer, sizeof(s_week_year_buffer), "Week %V '%y", tick_time);
  strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", tick_time);
  strftime(s_date_buffer, sizeof(s_date_buffer), "%e %B", tick_time);
  strftime(s_day_buffer, sizeof(s_day_buffer), "%A", tick_time);

  // Add some leet if necessary
  if (strcmp(s_time_buffer, "13:37") == 0) {
    vibes_short_pulse();
    strncpy(s_time_buffer, "LEET!", sizeof(s_time_buffer));
  }

  // Update the display
  text_layer_set_text(s_week_year_layer, s_week_year_buffer);
  text_layer_set_text(s_time_layer, s_time_buffer);
  text_layer_set_text(s_date_layer, s_date_buffer);
  text_layer_set_text(s_day_layer, s_day_buffer);
}

static void battery_handler(BatteryChargeState charge_state) {
  // Declare buffer to hold the formatted string
  static char s_battery_buffer[6];
  
  // Format the status to string
  if (charge_state.is_charging) {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "+%d%%", charge_state.charge_percent);
  } else {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", charge_state.charge_percent);
  }

  // Update the display
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}

void bluetooth_handler(bool connected) {
  if (bluetooth_handler_init) {
    bluetooth_handler_init = false;
  } else {
    // Vibrate briefly twice on bluetooth status change
    vibes_double_pulse();
  }

  // Update the display
  text_layer_set_text(s_bluetooth_layer, connected ? "BT on" : "BT off");
}

static void space_layer_draw(Layer *layer, GContext *ctx) {
  // Fill the whole space layer using black
  // This is the status bar background color
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

static void add_space_layers(Layer *root_layer) {
  // Create space for week/year layer
  s_week_year_space_layer = layer_create(GRect(0, 0, 4, 19));
  layer_set_update_proc(s_week_year_space_layer, space_layer_draw);
  layer_add_child(root_layer, s_week_year_space_layer);
  
  // Create space for battery layer
  s_battery_space_layer = layer_create(GRect(140, 0, 4, 19));
  layer_set_update_proc(s_battery_space_layer, space_layer_draw);
  layer_add_child(root_layer, s_battery_space_layer);
  
  // Create space for day layer
  s_day_space_layer = layer_create(GRect(0, 149, 4, 19));
  layer_set_update_proc(s_day_space_layer, space_layer_draw);
  layer_add_child(root_layer, s_day_space_layer);
  
  // Create space for bluetooth layer
  s_bluetooth_space_layer = layer_create(GRect(140, 149, 4, 19));
  layer_set_update_proc(s_bluetooth_space_layer, space_layer_draw);
  layer_add_child(root_layer, s_bluetooth_space_layer);
}

static void main_window_load(Window *window) {
  // Get the root layer for all new layers
  Layer *root_layer = window_get_root_layer(window);

  // Get the fonts to be used
  GFont gothic_14_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  GFont bitham_42_light_font = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
  GFont gothic_24_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  
  // Create week/year layer
  s_week_year_layer = text_layer_create(GRect(4, 0, 107, 19));
  text_layer_set_background_color(s_week_year_layer, GColorBlack);
  text_layer_set_text_color(s_week_year_layer, GColorWhite);
  text_layer_set_font(s_week_year_layer, gothic_14_font);
  layer_add_child(root_layer, text_layer_get_layer(s_week_year_layer));

  // Create battery layer
  s_battery_layer = text_layer_create(GRect(111, 0, 30, 19));
  text_layer_set_background_color(s_battery_layer, GColorBlack);
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_font(s_battery_layer, gothic_14_font);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  layer_add_child(root_layer, text_layer_get_layer(s_battery_layer));

  // Create time layer
  s_time_layer = text_layer_create(GRect(0, 41, 144, 50));
  text_layer_set_font(s_time_layer, bitham_42_light_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(s_time_layer));

  // Create date layer
  s_date_layer = text_layer_create(GRect(0, 91, 144, 32));
  text_layer_set_font(s_date_layer, gothic_24_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(s_date_layer));

  // Create day layer
  s_day_layer = text_layer_create(GRect(4, 149, 107, 19));
  text_layer_set_background_color(s_day_layer, GColorBlack);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_font(s_day_layer, gothic_14_font);
  layer_add_child(root_layer, text_layer_get_layer(s_day_layer));

  // Create bluetooth layer
  s_bluetooth_layer = text_layer_create(GRect(111, 149, 29, 19));
  text_layer_set_background_color(s_bluetooth_layer, GColorBlack);
  text_layer_set_text_color(s_bluetooth_layer, GColorWhite);
  text_layer_set_font(s_bluetooth_layer, gothic_14_font);
  text_layer_set_text_alignment(s_bluetooth_layer, GTextAlignmentRight);
  layer_add_child(root_layer, text_layer_get_layer(s_bluetooth_layer));

  // Add the space layers
  add_space_layers(root_layer);
  
  // Initial update for time
  time_t temp_time = time(NULL); 
  struct tm *tick_time = localtime(&temp_time);
  tick_handler(tick_time, MINUTE_UNIT);
  // Initial update for battery
  BatteryChargeState charge_state = battery_state_service_peek();
  battery_handler(charge_state);
  // Initial update for bluetooth
  bool connected = bluetooth_connection_service_peek();
  bluetooth_handler(connected);
}

static void main_window_unload(Window *window) {
  // Destroy all space layers to free resources
  layer_destroy(s_week_year_space_layer);
  layer_destroy(s_battery_space_layer);
  layer_destroy(s_day_space_layer);
  layer_destroy(s_bluetooth_space_layer);
  
  // Destroy all layers to free resources
  text_layer_destroy(s_week_year_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_bluetooth_layer);
}

static void init() {
  // Create a new window
  s_main_window = window_create();

  // Set the load and unload handlers for the new window
  // These will create and destroy the layers which will display information
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Push the new window on the stack, using animation
  window_stack_push(s_main_window, true);

  // Subscribe to various events (time change, battery change, bluetooth change)
  // This will be used to update the display immediately on change
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  bluetooth_connection_service_subscribe(bluetooth_handler);
}

static void deinit() {
  // Unsubscribe from various events to free resources
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();

  // Destroy the window to free resources
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
