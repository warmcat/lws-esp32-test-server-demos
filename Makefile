#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
PROJECT_NAME := lws-esp32-test-server-demos
SSL_CERT_PEM:=${PWD}/build/libwebsockets/libwebsockets-test-server
SSL_KEY_PEM:=${PWD}/build/libwebsockets/libwebsockets-test-server.key

EXTRA_COMPONENT_DIRS := components
LWS_IS_FACTORY_APPLICATION=0
export LWS_IS_FACTORY_APPLICATION

include $(IDF_PATH)/make/project.mk
include sdkconfig
include ${PWD}/components/libwebsockets/scripts/esp32.mk

CFLAGS+= -I$(PROJECT_PATH)/components/libwebsockets/plugins  -DLWS_IS_FACTORY_APPLICATION=$(LWS_IS_FACTORY_APPLICATION)

export IDF_PATH


