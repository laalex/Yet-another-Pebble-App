// Location success callback
function locationSuccessCallback(pos){
  // Build the long/lat coords
  var lat = pos.coords.latitude;
  var lng = pos.coords.longitude;
  // Get the location name
  var current_address;
  var req_url = "http://maps.googleapis.com/maps/api/geocode/json?latlng="+lat+","+lng+"&sensor=false";
  // Return the JSON data
  var request = new XMLHttpRequest();
  request.open('GET', req_url, true);
  request.onload = function(e){
    if (request.readyState == 4 && request.status == 200) {
      var response = JSON.parse(request.responseText);
      // Build the current address
      current_address = response.results[0].formatted_address;
      var dictionary = {
        "KEY_APISTRING": current_address
      };
      Pebble.sendAppMessage(dictionary, function(){console.log("Location sent");}, function(){console.log("failed");});
    }
  };
  request.send(null);

  
}

// Location failed callback
function locationErrorCallback(error){
  console.log("There was an error getting your location. See below");
  console.log(error);
}

//Get's the current location
function getLocation(){
  navigator.geolocation.getCurrentPosition(
    locationSuccessCallback,
    locationErrorCallback,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Add event listener to be ready
Pebble.addEventListener('ready', function(ev){
  getLocation();
});

// Add event listener for "appmessage"
Pebble.addEventListener('appmessage',function(ev){
  getLocation();// This will get the updates later on
});