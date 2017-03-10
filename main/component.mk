#COMPONENT_EXTRA_CLEAN := logo.h
main.o: prepare

.PHONY: prepare
prepare:
	tail -n +2 $(SSL_CERT_PEM).pem | \
		head -n -1 | base64 -d - \
			> $(COMPONENT_PATH)/../romfs-files/ssl-cert.der
	tail -n +2 $(SSL_KEY_PEM).pem | \
		head -n -1 | base64 -d - \
			> $(COMPONENT_PATH)/../romfs-files/ssl-key.der
	genromfs -f romfs.img -d $(COMPONENT_PATH)/../romfs-files


