ifneq ($(words $(CURDIR)),1)
  $(error Unsupported: GNU Make cannot build in directories containing spaces: '$(CURDIR)')
endif

CFLAGS := "-Wall -Werror -Wextra --std=c++17 -O2 -g"
SIMFLAGS := -O2 --cc --autoflush --assert -sv -Mdir bin -CFLAGS $(CFLAGS)
TFLAG := --trace
LIBS := -lpthread -lgtest

.PHONY: Vtop run clean info

default: Vtop

Vtop:
	verilator $(SIMFLAGS) top.sv --exe sim_main.cpp $(TFLAG)
	make -j -C bin -f Vtop.mk Vtop LDFLAGS="$(LIBS)"

run:
	./bin/Vtop

clean:
	rm -rf bin

info:
	verilator -V
