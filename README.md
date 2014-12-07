Here Am I
=======

This pebble watchapp is checking for your location and is displaying it on the screen. What it does at the moment:
- Get's your phone location
- Does a request to google maps API that's determining your location based on long / lat
- Displays a formatted address including: Road name, City name, ZIP Code, Country

How it works?
=======

In this section I will explain how the pebble application works and exemplify with bits of code so you can better understand how to create pebble apps


Bugs / TODO
=======

- currently the address date buffer is only 32 bits long and it trims the address (bug)
