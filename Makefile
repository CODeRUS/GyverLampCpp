default_target: all

all:
	platformio -f -c qtcreator run

clean:
	platformio -f -c qtcreator run --target clean

buildfs:
	platformio -f -c qtcreator run --target buildfs

upload:
	platformio -f -c qtcreator run --target upload

uploadfs:
	platformio -f -c qtcreator run --target uploadfs

