default_target: all

all:
        platformio run $(ARGS)
        platformio run --target buildfs $(ARGS)

clean:
        platformio run --target clean $(ARGS)

buildfs:
        platformio run --target buildfs $(ARGS)

upload:
        platformio run --target upload $(ARGS)

uploadfs:
        platformio run --target uploadfs $(ARGS)

