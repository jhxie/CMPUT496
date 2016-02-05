all: SingleSwitchTopo.png
	
clean:
	-rm -f SingleSwitchTopo.png

SingleSwitchTopo.png:
	dot -Tpng -Gsize=9,15\! -Gdpi=100 SingleSwitchTopo.dot -o $@

.PHONY: all clean
.IGNORE:
