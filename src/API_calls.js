/**
* All this code is executed on the phone
* @author: Lamba Alexandru
* @contact: contact@alexandrulamba.com
*/

// Location success callback
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

// Location failed callback
function locationErrorCallback(error){ // Failed to get the location from the phone
  console.log("There was an error getting your location. See below"); // Log this problem
}

//Get's the current location
function getLocation(){
  navigator.geolocation.getCurrentPosition( // Get the location from the phone
    locationSuccessCallback, // Set callback for success
    locationErrorCallback, // Set callback for errors
    {timeout: 15000, maximumAge: 60000} // Set timeout and maximum age for this request
  );
}

// Add event listener to be ready
Pebble.addEventListener('ready', function(ev){ // Initialise the AppMessage event listener for "ready" state
  getLocation(); // When we are ready, request the location
});

// Add event listener for "appmessage"
Pebble.addEventListener('appmessage',function(ev){ // If we have any appmessage request from pebble
  getLocation();// Update the location again and send it to the pebble
});