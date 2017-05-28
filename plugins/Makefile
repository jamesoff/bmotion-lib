#
# Compiler: g++
#

OTHERS = testplugin admincore core

default:
	@echo; \
	$(foreach dir,$(OTHERS),cd $(dir); make; cd ..;)

clean:
	@echo; \
	$(foreach dir,$(OTHERS),cd $(dir); make clean; cd ..;)
