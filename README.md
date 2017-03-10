lws-esp32
=========

## Status

 - Station mode provides the full lws test server on port 443 over htps, all working

 - ESP32 becomes an AP and serves dynamic websocket-based scan / config page,
   so you can set up your AP and password.  Initially comes up in this since
   the AP details are not set yet.

![lws AP mode config page](https://libwebsockets.org/lws-esp32-ap.png)

 - Integration to new projects hugely simplified by moving common lws support code
   into lws directly.  Non-library parts licensed are CC0.


## Customizing to your build setup

### 0) Set the right serial device in menuconfig

### 1) Patch esp-idf

You can either use the already-patched esp-idf at

$ git clone --init --recursive https://github.com/lws-team/lws-esp-idf

or you can apply this patch to your own esp-idf

```diff
    Apply this patch on esp-idf, then
    

diff --git a/.gitmodules b/.gitmodules
index 6a6af0e..897992d 100644
--- a/.gitmodules
+++ b/.gitmodules
@@ -13,3 +13,7 @@
 [submodule "components/coap/libcoap"]
        path = components/coap/libcoap
        url = https://github.com/obgm/libcoap.git
+[submodule "components/libwebsockets"]
+       path = components/libwebsockets
+       url = https://git@github.com/warmcat/libwebsockets
+       branch=master
diff --git a/components/libwebsockets b/components/libwebsockets
new file mode 160000
index 0000000..3a09c3b
--- /dev/null
+++ b/components/libwebsockets
@@ -0,0 +1 @@
+Subproject commit 102d40e6b6486076f37a46708604ec387834f16c
```

and then do

```
     $ git submodules update --recursive
```

from the esp-idf

### 2) Partitioning the device

Clear down the partitioning since we write a custom table and the bootloader
will choke if the OTA parts are not initialized like this

```
 $ make erase_flash
```

## Writing the ROMFS to flash

A ROMFS contains the files to be served, stored at a partition at the end
of the flash.

There's currently space for ~900KiB there.

The ROMFS is generated automatically from files in `./romfs-files` every make. 

Write it and update it in the flash like this:

```
 $ make all flash-romfs
```

## General build and flash

```
 $ make all flash monitor
```

## Using the AP config

 - connect your wifi to the ap "lws-config-...."

 - In a browser, go to https://192.168.4.1

 - set a serial number, like 1234

 - Select your normal AP from the list

 - Give the AP password and click the button

 - Your ESP32 resets into Station mode and associates with the AP

## Using the lws test apps

See what IP your ESP32 got from your AP, the visit it in your browser
using, eg https://192.168.2.249

If your dhcp server provides your dns, you can also reach the device
using lws-serial, eg, https://lws-1234

 - dumb increment should be updating at ~20Hz

 - mirror should let you draw in the canvas... open a second browser
   instance and they should be able to see each other's drawings

 - close testing should work

 - server info should reflect browsers open on the site dynamically

 - POST tests should pass the string and upload the file if one given

