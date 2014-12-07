Here Am I
=======

This pebble watchapp is checking for your location and is displaying it on the screen. What it does at the moment:
- Get's your phone location
- Does a request to google maps API that's determining your location based on long / lat
- Displays a formatted address including: Road name, City name, ZIP Code, Country


### TODO

- gestures controls for refreshing location, etc
- implementing viber (vibration)
- implementing icons and menus
- implementing compass
- a counter to see how many times you've gone into some locations
- a "save location" function to  the locations you've been and want to save


### How it works

This Pebble application has for the moment only two files:
- `main_activity.c` - which handles the main activity and main window
- `API_calls.js` - which makes the API calls and get's the phone location.

###### Main activity
The `main_activity.c` includes the `pebble.h` header and defines the KEYS for our dictionary that we are using to transfer the data from the phone to Pebble. In the following, I will describe this file and it's content.

First of all, we are defining _3_ elements that we are using within the application. Those are one `Window` object and two `TextLayer` objects. Each of them, are defined as pointers of types `Window` and `TextLayer`:
```c
static Window *main_window; // Create a new window layer
static TextLayer *text_layer; // Create a new text layer
static TextLayer *api_data_text; // Create a new text layer
```
###### Main loop
As every `C` program, the Pebble WhatchApp has a `main()` function that is called when the program is executed. Our `main()` function consists into 3 function calls that are going to _init_ our variables, get the application into _it's running loop_ while the application is running and another function that _destroys_ our used variables to free up memory. The `main()` function is defined as follows:
```c
int main(void){
  init(); // Init everything
  app_event_loop(); // Loop everything (run app and take actions)
  deinit(); // When app is leaved, de-init everything to free memory
}
```
###### Application init
In our application, on the `init()` function, I am creating a new `Window` object and the two `TextLayer` objects, and configure them with their default values, and also I am adding some of their properties. Also, in the same `init()` function, I am initialising all the required event listeners for the `AppMessage` API from Pebble, to listen for any events that are sending or receiving data. The function definition, along with the description of the code is the following:
```c
// Init function
static void init(){
  // Create new window
  main_window = window_create(); // Instantiates the *window (pointer) with a new Window object
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
```

Instead of instantiating all the elements that are used within the `Window` object, I have attached handlers for when the `Window` loads (is constructed) or unloads (is destructed). This means, that when the window is loaded, I am instantiating all the `TextLayer` objects along with their properties. The handler for instantiating the `Window` is called `main_window_load` and it's defined as follows:
```c
static void main_window_load(Window *window){ // Get our pointer to the Window object
  // Create a new text layer and add it's properties
  text_layer = text_layer_create(GRect(0, 0, 144, 28)); // Create a new rectangle shape that starts at 0,0 and has the width of 144PX and height of 28PX
  text_layer_set_background_color(text_layer, GColorBlack); // Set the background color to Black
  text_layer_set_text_color(text_layer, GColorWhite); // Set the text color to White
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter); // Set the text alignment to center
  text_layer_set_text(text_layer, "You're at"); // This is the default value for the string contained in this TextLayer
  
  // Create a new text layer and add it's properties
  api_data_text = text_layer_create(GRect(0, 28, 144, 140));
  text_layer_set_background_color(api_data_text, GColorBlack);
  text_layer_set_text_color(api_data_text, GColorWhite);
  text_layer_set_text_alignment(api_data_text, GTextAlignmentCenter);
  text_layer_set_text(api_data_text, "Loading...");
  
  // Add this text layer to the window as a child
  // As we have the pointer to the window, we can get it's root layer with window_get_root_layer(pointer)
  // and we are using layer_add_child to add a new child into that window's layer and display it
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
  // Add the api text layer to the window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(api_data_text));
}
```
The prototype of those functions related to the UI can be found within the Pebble documentation at [Official Pebble Documentation > PEBBLE C > UI](http://developer.getpebble.com/docs/c/group___u_i.html). Every other details about what's happening in that block, can be readed and understood by the comments on each line. 

###### Application deinit
When a `Window` object is destroyed (when the application closed), our handler for destroying the `Window` object is going to destroy those objects to free up the memory used by the application on the Pebble OS. Thus meaning we need to remove the two layers we've added within this window, in it's window unload handler, as follows:
```c
static void main_window_unload(Window *window){
  // Destroy the text layer to free up space
  text_layer_destroy(text_layer);
  // Destroy the text layer to free up space
  text_layer_destroy(api_data_text);
}
```
The above code clears the memory that was allocated on Pebble. 

###### How does the Pebble app get the data from the phone?
Eearlier in this description, I've told you that the Pebble WhatchApp is getting the Phone's location and does an AJAX request to get the location name based on the _latitude and longitude_ determined by the phone. In our `init()` function we had the following code that was subscribing some events related to `AppMessage` API:
```c
// Init the callbacks for AppMessage API
app_message_register_inbox_received(inbox_received_callback); // Listen for information received from the phone
app_message_register_inbox_dropped(inbox_dropped_callback); // Listen for information received from the phone but discarded
app_message_register_outbox_failed(outbox_failed_callback); // Listen for events that send information to the phone but were failed
app_message_register_outbox_sent(outbox_sent_callback); // Listen for events that send information to the phone with success
  
// Start app message
app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
```
###### Watch to phone and phone to watch communication
The `app_message_open` actually opens the connection and attached all those event listeners. At the moment of calling those function, every event triggered is handled by one of the 4 event listeners we have instantiated later on. The only event listener implemented at the moment is the `app_message_register_inbox_received()` and it's handled by the function called `inbox_received_callback`, defined and explained as follows:
```c
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
```
This handler get's the dictionary key defined by `#define KEY_APISTRING 0` onto our application. It get's the dictionary from the Phone, searches for our defined keys, and when it finds `KEY_APISTRING` into the dictionary, it loads it's content as a _string_ within our buffer _var_ created in this function. After the dictionary is read entirely and the _while_ loop finishes, then we are updating the required `TextLayer` with the new content (the address at where we are located). 

All of this happens within the `app_event_loop()` call from our `main()` function. When we leave the application, the `deinit()` function is called. In here, we remove the `Window` object that we have created in the earlier steps:

```c
static void deinit(){
  // Destroy the main window to free memory on the Pebble watch
  window_destroy(main_window);
}
```

###### Now, how does the phone send us all this information, and how everything is executed between the Pebble WhatchApp and the Phone?
The JavaScript file `API_calls.js` handles all those steps and comunicates with our `c` app. First, usng the `Pebble` object from the `PebbleJS KIT`, we are setting up an event listener for when the connection between the watch and the phone is ready:
```javascript
Pebble.addEventListener('ready', function(ev){ // Initialise the AppMessage event listener for "ready" state
  getLocation(); // When we are ready, request the location
});
```
When this event is ready, we are calling a function named `geoLocation()` that get's our location from the phone as follows:
```javascript
function getLocation(){
  navigator.geolocation.getCurrentPosition( // Get the location from the phone
    locationSuccessCallback, // Set callback for success
    locationErrorCallback, // Set callback for errors
    {timeout: 15000, maximumAge: 60000} // Set timeout and maximum age for this request
  );
}
```
To make the function more readable, and easy to maintain, instead of using inline functions for the callbacks of `navigator.geolocation.getCurrentPosition` I have created some handler functions that are taking care of the required callbacks: `locationSuccessCallback` and `locationErrorCallback` - their names are very intuitive and I am pretty sure you know what they are doing. Focusing on the `locationSuccessCallback` (which means we have the location!) we are doing some more processing in this function like: getting the result from this callback, making an ajax call to _Google Maps API_ to get our location name. The function is defined as follows:
```javascript
function locationSuccessCallback(pos){
  // Build the long/lat coords
  var lat = pos.coords.latitude;
  var lng = pos.coords.longitude;
  // Get the location name
  var current_address;
  var req_url = "http://maps.googleapis.com/maps/api/geocode/json?latlng="+lat+","+lng+"&sensor=false";
  // Return the JSON data
  var request = new XMLHttpRequest(); // Create new AJAX request
  request.open('GET', req_url, true); // Open connection to the req_url
  request.onload = function(e){ // Check the AJAX STATE
    if (request.readyState == 4 && request.status == 200) { // IF ajax finished and response is OK (200)
      var response = JSON.parse(request.responseText); // Parse the response from the API
      // Build the current address
      current_address = response.results[0].formatted_address; // Get the current address
      var dictionary = { // Create a dictionary that we send to the pebble app
        "KEY_APISTRING": current_address
      };
      // Initialise a request to the pebble application
      // and send our dictionary (the propper data)
      Pebble.sendAppMessage(dictionary, function(){console.log("Location sent");}, function(){console.log("failed");});
    }
  };
  // Finish tue ajax request
  request.send(null);
  
}
```
###### The callback explained
The callback sends us a parameter with the _longitude and latitude_. We are using those two coordinates to make a request to Google Maps API via an Ajax call. When the call succeeds, we parse the response and create a new object (dictionary) where we put our _KEY defined in the C app_ as being the key of the object, and the parsed content from google as being the VALUE of that key in the object. Then, we push this information to the watch by calling:
```javascript
Pebble.sendAppMessage(dictionary, function(){console.log("Location sent");}, function(){console.log("failed");});
```
This function call, sends our dictionary to our `C` app handler: 
```c
static void inbox_received_callback(DictionaryIterator *iterator, void *context)
```
This handler has been explained on top: Pushes the new value of the key to the TextLayer and displays it on the watch screen.
