all: SingleSwitchTopo.png
	
clean:
	-rm -f SingleSwitchTopo.png

SingleSwitchTopo.png:
	dot -Tpng SingleSwitchTopo.dot -o $@

.PHONY: all clean
.IGNORE:
