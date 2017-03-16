#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
PROJECT_NAME := lws-esp32
SSL_CERT_PEM:=${PWD}/build/libwebsockets/libwebsockets-test-server
SSL_KEY_PEM:=${PWD}/build/libwebsockets/libwebsockets-test-server.key

EXTRA_COMPONENT_DIRS := components
LWS_IS_FACTORY_APPLICATION=0
export LWS_IS_FACTORY_APPLICATION

include $(IDF_PATH)/make/project.mk
include sdkconfig
include ${PWD}/components/libwebsockets/scripts/esp32.mk

CFLAGS+= -I$(COMPONENT_PATH)/../components/libwebsockets/plugins  -DLWS_IS_FACTORY_APPLICATION=$(LWS_IS_FACTORY_APPLICATION)

export IDF_PATH

$(COMPONENT_PATH)/../ssl-cert.der:
	tail -n +2 $(SSL_CERT_PEM).pem | \
		head -n -1 | base64 -d - \
			> $(COMPONENT_PATH)/../ssl-cert.der

$(COMPONENT_PATH)/../ssl-key.der:
	tail -n +2 $(SSL_KEY_PEM).pem | \
		head -n -1 | base64 -d - \
			> $(COMPONENT_PATH)/../ssl-key.der

