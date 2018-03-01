/*
 * Example ESP32 app code using Libwebsockets
 *
 * Copyright (C) 2017 Andy Green <andy@warmcat.com>
 *
 * This file is made available under the Creative Commons CC0 1.0
 * Universal Public Domain Dedication.
 *
 * The person who associated a work with this deed has dedicated
 * the work to the public domain by waiving all of his or her rights
 * to the work worldwide under copyright law, including all related
 * and neighboring rights, to the extent allowed by law. You can copy,
 * modify, distribute and perform the work, even for commercial purposes,
 * all without asking permission.
 *
 * The test apps are intended to be adapted for use in your code, which
 * may be proprietary.	So unlike the library itself, they are licensed
 * Public Domain.
 *
 */
#include <libwebsockets.h>
#include <nvs_flash.h>

/*
 * Configuration for normal station website
 *
 * We implement the generic lws test server features using
 * generic plugin code from lws.  Normally these plugins
 * are dynamically loaded at runtime, but we use them by
 * statically including them.
 *
 * To customize for your own device, you would remove these
 * and put your own plugin include here
 */
#include "../components/libwebsockets/plugins/protocol_dumb_increment.c"
#include "../components/libwebsockets/plugins/protocol_lws_mirror.c"
#include "../components/libwebsockets/plugins/protocol_post_demo.c"
#include "../components/libwebsockets/plugins/protocol_lws_status.c"
#include "../components/libwebsockets/plugins/protocol_lws_meta.c"
#include <protocol_esp32_lws_reboot_to_factory.c>

static const struct lws_protocols protocols_station[] = {
	{
		"http-only",
		lws_callback_http_dummy,
		0,
		1024, 0, NULL, 900
	},
	LWS_PLUGIN_PROTOCOL_DUMB_INCREMENT, /* demo... */
	LWS_PLUGIN_PROTOCOL_MIRROR,	    /* replace with */
	LWS_PLUGIN_PROTOCOL_POST_DEMO,	    /* your own */
	LWS_PLUGIN_PROTOCOL_LWS_STATUS,	    /* plugin protocol */
	LWS_PLUGIN_PROTOCOL_ESPLWS_RTF,	/* helper protocol to allow reset to factory */
	LWS_PLUGIN_PROTOCOL_LWS_META,	    /* protocol multiplexer */
	{ NULL, NULL, 0, 0, 0, NULL, 0 } /* terminator */
};

static const struct lws_protocol_vhost_options pvo_headers = {
        NULL,
        NULL,
        "Keep-Alive:",
        "timeout=5, max=20",
};

/* reset to factory mount */
static const struct lws_http_mount mount_station_rtf = {
	.mountpoint		= "/esp32-rtf",
	.origin			= "esplws-rtf",
	.origin_protocol	= LWSMPRO_CALLBACK,
	.mountpoint_len		= 10,
};

/*
 * this makes a special URL "/formtest" which gets passed to
 * the "protocol-post-demo" plugin protocol for handling
 */
static const struct lws_http_mount mount_station_post = {
	.mount_next		= &mount_station_rtf,
	.mountpoint		= "/formtest",
	.origin			= "protocol-post-demo",
	.origin_protocol	= LWSMPRO_CALLBACK,
	.mountpoint_len		= 9,
};

/*
 * this serves "/station/..." in the romfs at "/" in the URL namespace
 */
static const struct lws_http_mount mount_station = {
        .mount_next		= &mount_station_post,
        .mountpoint		= "/",
        .origin			= "/station",
        .def			= "test.html",
        .origin_protocol	= LWSMPRO_FILE,
        .mountpoint_len		= 1,
};

/*
 * this serves "/secret/" in the romfs at "/secret" in the URL namespace
 * it requires basic auth, which for the demo is set to literally user / password
 * This is just demoing how to do basic auth.
 *
 * See below how the password is set
 */
static const struct lws_http_mount mount_station_needs_auth = {
        .mount_next		= &mount_station,
        .mountpoint		= "/secret",
        .origin			= "/secret",
        .def			= "index.html",
        .origin_protocol	= LWSMPRO_FILE,
        .mountpoint_len		= 7,

	.basic_auth_login_file	= "lwsdemoba", /* esp32 nvs realm to use */
};


esp_err_t event_handler(void *ctx, system_event_t *event)
{
	/* deal with your own user events here first */

	return lws_esp32_event_passthru(ctx, event);
}

/*
 * This is called when the user asks to "Identify physical device"
 * he is configuring, by pressing the Identify button on the AP
 * setup page for the device.
 *
 * It should do something device-specific that
 * makes it easy to identify which physical device is being
 * addressed, like flash an LED on the device on a timer for a
 * few seconds.
 */
void
lws_esp32_identify_physical_device(void)
{
	lwsl_notice("%s\n", __func__);
}

void lws_esp32_leds_timer_cb(TimerHandle_t th)
{
}


void app_main(void)
{
	static struct lws_context_creation_info info;
	struct lws_context *context;
	struct lws_vhost *vh;
	nvs_handle nvh;

	lws_esp32_set_creation_defaults(&info);

	info.port = 443;
	info.fd_limit_per_thread = 12;
	info.max_http_header_pool = 12;
	info.max_http_header_data = 512;
	info.pt_serv_buf_size = 900;
	info.keepalive_timeout = 5;
	info.simultaneous_ssl_restriction = 12;
	info.options = LWS_SERVER_OPTION_EXPLICIT_VHOSTS |
		       LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

	info.ssl_cert_filepath = "ap-cert.pem";
	info.ssl_private_key_filepath = "ap-key.pem";

	info.vhost_name = "station";
	info.protocols = protocols_station;
	info.mounts = &mount_station_needs_auth;
	info.headers = &pvo_headers;

	nvs_flash_init();
	lws_esp32_wlan_config();

	/*
	 * set the basic auth user:password used for /secret/... urls
	 * normally you'd just do this once at setup-time or if the
	 * password was changed.  If you don't use basic auth on your
	 * site, no need for this.
	 */
	if (!nvs_open("lwsdemoba", NVS_READWRITE, &nvh)) {
		nvs_set_str(nvh, "user", "password");
		nvs_commit(nvh);
		nvs_close(nvh);
	}

	ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL));

	lws_esp32_wlan_start_station();
	context = lws_esp32_init(&info, &vh);

	while (!lws_service(context, 50))
		taskYIELD();
}
