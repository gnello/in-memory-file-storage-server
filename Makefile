TARGETS = helpers data-structures

.PHONY: all clean helpers data-structures

all: $(TARGETS)

helpers:
	cd ./helpers && $(MAKE)

data-structures:
	cd ./data-structures && $(MAKE)

clean:
	cd ./helpers && $(MAKE) clean
	cd ./data-structures && $(MAKE) clean