build-firmwere:
	./hardware/firmware/scripts/build.sh

menuconfig-firmwere:
	./hardware/firmware/scripts/menuconfig.sh

cleanup-firmwere:
	./hardware/firmware/scripts/cleanup.sh

flash-firmware:
	./hardware/firmware/scripts/flash.sh

test-firmware:
	python3 ./hardware/firmware/src/tests/test.py

run-app-dev:
	cargo tauri dev