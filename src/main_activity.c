/**
* All this code is executed on the phone
* @author: Lamba Alexandru
* @contact: contact@alexandrulamba.com
*/

#include <pebble.h>
#define KEY_APISTRING 0 //The name of the key that is returned by the PebbleJS Kit
  
// Var declaration
static Window *main_window; // Create a new window layer
static TextLayer *text_layer; // Create a new text layer
static TextLayer *api_data_text; // Create a new text layer

// Window load - handler
static void main_window_load(Window *window){
  // Create a new text layer and add it's properties
  text_layer = text_layer_create(GRect(0, 0, 144, 28));
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_text(text_layer, "You're at");
  
  // Create a new text layer and add it's properties
  api_data_text = text_layer_create(GRect(0, 28, 144, 140));
  text_layer_set_background_color(api_data_text, GColorBlack);
  text_layer_set_text_color(api_data_text, GColorWhite);
  text_layer_set_text_alignment(api_data_text, GTextAlignmentCenter);
  text_layer_set_text(api_data_text, "Loading...");
  
  // Add this text layer to the window as a child
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
  // Add the api text layer to the window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(api_data_text));
}

// Window unload
static void main_window_unload(Window *window){
  // Destroy the text layer to free up space
  text_layer_destroy(text_layer);
  // Destroy the text layer to free up space
  text_layer_destroy(api_data_text);
}


// AppMessage API -- callback handlers
static void inbox_received_callback(DictionaryIterator *iterator, void *context){
  Tuple *t = dict_read_first(iterator); // Get first dictionary value
  // Create a new buffer for the location string
  static char location_buffer[200]; // Buffer length should be enough to catch the entire string
  // Iterate troughout the dictionary values
  while(t != NULL){
    switch(t->key){ // Switch the key that we receive
      case KEY_APISTRING: // If we identify the key
        // Include the information from the key as a STRING into our buffer
        snprintf(location_buffer, sizeof(location_buffer), "%s", t->value->cstring);
      break;
      default: // The default case means we haven't indentified the string
        APP_LOG(APP_LOG_LEVEL_WARNING, "Key %d is not recognized", (int)t->key); // Log the error
      break;
    }
    // Read the next key in the dictionary if available
    t = dict_read_next(iterator);
  }
  // Add the location_buffer to the clock watchface
  APP_LOG(APP_LOG_LEVEL_DEBUG, "The value of location is: %s", location_buffer);
  // Update the text layer so we can see the address at where we are
  text_layer_set_text(api_data_text, location_buffer);
  
}
static void inbox_dropped_callback(AppMessageResult reason, void *context){
  // TO DO
  // Handle the dropped inbox message and log the reason
}
static void outbox_sent_callback(DictionaryIterator *iterator, void *context){
  // TO DO
}
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context){
  // TO DO
}


// Init function
static void init(){
  // Create new window
  main_window = window_create();
  // Set the handlers
  window_set_window_handlers(main_window, (WindowHandlers){
    .load = main_window_load, // Set the handler for window load
    .unload = main_window_unload // Set the handler for window unload
  });
  
  // Push the window to the stack
  window_stack_push(main_window, true); // Push the window animated
  
  // Init the callbacks for AppMessage API
  app_message_register_inbox_received(inbox_received_callback); // Init callback
  app_message_register_inbox_dropped(inbox_dropped_callback); // Init callback
  app_message_register_outbox_failed(outbox_failed_callback); // Init callback
  app_message_register_outbox_sent(outbox_sent_callback); // Init callback
  
  // Start app message
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

// De-init the application (destructor)
static void deinit(){
  // Destroy the main window to free memory on the Pebble watch
  window_destroy(main_window);
}

// Main loop
int main(void){
  init(); // Init everything
  app_event_loop(); // Loop everything (run app and take actions)
  deinit(); // When app is leaved, de-init everything to free memory
}