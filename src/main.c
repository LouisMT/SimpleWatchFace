#include <pebble.h>

// The main window
static Window *s_main_window;

// The text layers
// These display the actual information
static TextLayer *s_week_year_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_day_layer;
static TextLayer *s_bluetooth_layer;

// The space layers
// These create some padding for the status bar text
static Layer *s_week_year_space_layer;
static Layer *s_battery_space_layer;
static Layer *s_day_space_layer;
static Layer *s_bluetooth_space_layer;

// The buffers
// These contain the text for the text layers
static char s_week_year_buffer[15];
static char s_battery_buffer[5];
static char s_time_buffer[6];
static char s_date_buffer[13];
static char s_day_buffer[10];
static char s_bluetooth_buffer[7];

static void update_time() {
  // Get current time
  time_t temp_time = time(NULL); 
  struct tm *tick_time = localtime(&temp_time);

  // Format time to strings
  strftime(s_week_year_buffer, sizeof(s_week_year_buffer), "Week %V / %Y", tick_time);
  strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", tick_time);
  strftime(s_date_buffer, sizeof(s_date_buffer), "%e %B", tick_time);
  strftime(s_day_buffer, sizeof(s_day_buffer), "%A", tick_time);

  // Add some leet if necessary
  if (strcmp(s_time_buffer, "13:37") == 0) {
    strncpy(s_time_buffer, "LEET!", sizeof(s_time_buffer));
  }

  // Update the display
  text_layer_set_text(s_week_year_layer, s_week_year_buffer);
  text_layer_set_text(s_time_layer, s_time_buffer);
  text_layer_set_text(s_date_layer, s_date_buffer);
  text_layer_set_text(s_day_layer, s_day_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void update_battery() {
  // Get the battery status
  BatteryChargeState charge_state = battery_state_service_peek();

  // Format the status to string
  if (charge_state.is_charging) {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "CHRG");
  } else {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", charge_state.charge_percent);
  }

  // Update the display
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}

static void battery_handler(BatteryChargeState charge_state) {
  update_battery();
}

static void update_bluetooth() {
  // Get the bluetooth status, and format the status to string
  if (bluetooth_connection_service_peek()) {
    strncpy(s_bluetooth_buffer, "BT on", sizeof(s_bluetooth_buffer));
  } else {
    strncpy(s_bluetooth_buffer, "BT off", sizeof(s_bluetooth_buffer));
  }

  // Update the display
  text_layer_set_text(s_bluetooth_layer, s_bluetooth_buffer);
}

void bluetooth_handler(bool connected) {
  // Vibrate briefly twice on bluetooth status change
  vibes_double_pulse();
  update_bluetooth();
}

static void space_layer_draw(Layer *layer, GContext *ctx) {
  // Fill the whole space layer using black
  // This is the status bar background color
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

static void add_space_layers(Window *window) {
  // Create space for week/year layer
  s_week_year_space_layer = layer_create(GRect(0, 0, 4, 19));
  layer_set_update_proc(s_week_year_space_layer, space_layer_draw);
  layer_add_child(window_get_root_layer(window), s_week_year_space_layer);
  
  // Create space for battery layer
  s_battery_space_layer = layer_create(GRect(140, 0, 4, 19));
  layer_set_update_proc(s_battery_space_layer, space_layer_draw);
  layer_add_child(window_get_root_layer(window), s_battery_space_layer);
  
  // Create space for day layer
  s_day_space_layer = layer_create(GRect(0, 149, 4, 19));
  layer_set_update_proc(s_day_space_layer, space_layer_draw);
  layer_add_child(window_get_root_layer(window), s_day_space_layer);
  
  // Create space for bluetooth layer
  s_bluetooth_space_layer = layer_create(GRect(140, 149, 4, 19));
  layer_set_update_proc(s_bluetooth_space_layer, space_layer_draw);
  layer_add_child(window_get_root_layer(window), s_bluetooth_space_layer);
}

static void main_window_load(Window *window) {
  // Create week/year layer
  s_week_year_layer = text_layer_create(GRect(4, 0, 107, 19));
  text_layer_set_background_color(s_week_year_layer, GColorBlack);
  text_layer_set_text_color(s_week_year_layer, GColorWhite);
  text_layer_set_font(s_week_year_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_week_year_layer));

  // Create battery layer
  s_battery_layer = text_layer_create(GRect(111, 0, 29, 19));
  text_layer_set_background_color(s_battery_layer, GColorBlack);
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));

  // Create time layer
  s_time_layer = text_layer_create(GRect(0, 41, 144, 50));
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

  // Create date layer
  s_date_layer = text_layer_create(GRect(0, 91, 144, 32));
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));

  // Create day layer
  s_day_layer = text_layer_create(GRect(4, 149, 107, 19));
  text_layer_set_background_color(s_day_layer, GColorBlack);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));

  // Create bluetooth layer
  s_bluetooth_layer = text_layer_create(GRect(111, 149, 29, 19));
  text_layer_set_background_color(s_bluetooth_layer, GColorBlack);
  text_layer_set_text_color(s_bluetooth_layer, GColorWhite);
  text_layer_set_font(s_bluetooth_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_bluetooth_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bluetooth_layer));

  // Add the space layers
  add_space_layers(window);
  
  // Initial update for display
  update_time();
  update_battery();
  update_bluetooth();
}

static void main_window_unload(Window *window) {
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

int main(void) {
  init();
  app_event_loop();
  deinit();
}
