#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
PROJECT_NAME := lws-esp32
SSL_CERT_PEM:=${PWD}/build/libwebsockets/libwebsockets-test-server
SSL_KEY_PEM:=${PWD}/build/libwebsockets/libwebsockets-test-server.key

export SSL_CERT_PEM
export SSL_KEY_PEM

include $(IDF_PATH)/make/project.mk
include sdkconfig
# stick the romfs filesystem in its own romfs part
flash-romfs:
	$(IDF_PATH)/components/esptool_py/esptool/esptool.py \
		--chip esp32 \
		--port $(CONFIG_ESPTOOLPY_PORT) \
		--baud $(CONFIG_ESPTOOLPY_BAUD) \
		write_flash 0x310000 build/main/romfs.img


