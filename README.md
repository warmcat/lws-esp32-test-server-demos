lws-esp32-test-server-demos
===========================

## For use with lws-esp32-factory

This relies on setup capability provided by

https://github.com/warmcat/lws-esp32-factory

which runs from the "factory" partition on ESP32.  This app is
designed to run from the 2.9MB OTA partition.

## Build

This was built and tested againt esp-idf "v3", at 969f1bb9be1ce02f37115359111852a9bd2c9197.

Clone and bring in the lws submodule (it's unpatched lws master)

```
  $ git clone git@github.com:warmcat/lws-esp32-test-server-demos.git
  $ git submodule update --init --recursive
```

```
 $ make flash_ota ; make monitor
```

## Using the lws test apps

See what IP your ESP32 got from your AP, the visit it in your browser
using, eg https://192.168.2.249

If your dhcp server provides your dns, you can also reach the device
using lws-serial, eg, https://lws-1234 or https://lws-1234.local

 - dumb increment should be updating at ~20Hz

 - mirror should let you draw in the canvas... open a second browser
   instance and they should be able to see each other's drawings

 - close testing should work

 - server info should reflect browsers open on the site dynamically

 - POST tests should pass the string and upload the file if one given

 - The button at the bottom should reset you into the setup / factory app

 - The "War and Peace" demo should load the 4MB text from the 1.2MB gzipped zip
   file directly, using gzipped data to the browser.
