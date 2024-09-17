main: src/main.c
	gcc src/main.c -O2 -fPIC -shared -o libvtime.so
	gcc src/setration.c -O2 -o setration
