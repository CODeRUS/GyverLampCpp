default_target: all

all:
        platformio -f -c qtcreator run $(ARGS)

clean:
        platformio -f -c qtcreator run --target clean $(ARGS)

buildfs:
        platformio -f -c qtcreator run --target buildfs $(ARGS)

upload:
        platformio -f -c qtcreator run --target upload $(ARGS)

uploadfs:
        platformio -f -c qtcreator run --target uploadfs $(ARGS)

