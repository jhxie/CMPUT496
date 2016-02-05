all: SingleSwitchTopo.png
	
clean:
	-rm -f SingleSwitchTopo.png

SingleSwitchTopo.png:
	circo -Tpng SingleSwitchTopo.dot -o $@

.PHONY: all clean
.IGNORE:
