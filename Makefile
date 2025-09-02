SRC_DIR		= src

all:
    cd $(SRC_DIR) && $(MAKE)
	cp d3m0n

clean:
    cd $(SRC_DIR) && $(MAKE) clean

.PHONY: all clean