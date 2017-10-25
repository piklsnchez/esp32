deps_config := \
	/home/chris/git/esp-idf/components/app_trace/Kconfig \
	/home/chris/git/esp-idf/components/aws_iot/Kconfig \
	/home/chris/git/esp-idf/components/bt/Kconfig \
	/home/chris/git/esp-idf/components/esp32/Kconfig \
	/home/chris/git/esp-idf/components/ethernet/Kconfig \
	/home/chris/git/esp-idf/components/fatfs/Kconfig \
	/home/chris/git/esp-idf/components/freertos/Kconfig \
	/home/chris/git/esp-idf/components/heap/Kconfig \
	/home/chris/git/esp-idf/components/log/Kconfig \
	/home/chris/git/esp-idf/components/lwip/Kconfig \
	/home/chris/git/esp-idf/components/mbedtls/Kconfig \
	/home/chris/git/esp-idf/components/openssl/Kconfig \
	/home/chris/git/esp-idf/components/pthread/Kconfig \
	/home/chris/git/esp-idf/components/spi_flash/Kconfig \
	/home/chris/git/esp-idf/components/spiffs/Kconfig \
	/home/chris/git/esp-idf/components/tcpip_adapter/Kconfig \
	/home/chris/git/esp-idf/components/wear_levelling/Kconfig \
	/home/chris/git/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/chris/git/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/chris/git/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/chris/git/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
