all:
	gcc -o w48d w48d.c -lwiringPi

build-rpi:
	/opt/cross-pi-gcc/bin/arm-linux-gnueabihf-gcc -o w48d w48d.c -lwiringPi -L/opt/cross-pi-libs/lib -I/opt/cross-pi-gcc/arm-linux-gnueabihf/include -I/opt/cross-pi-libs/include


clean:
	rm -f w48d *.deb

install:
	cp -f w48d /usr/sbin/w48d
	cp -f w48d.service /lib/systemd/system/w48d.service
	systemctl enable w48d
