#include <iostream>
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vtop.h"

#define CLK_PERIOD 4000

Vtop *top_;  // module instantiation
vluint64_t main_time = 0; // simulation time

// verilator bull shit
double sc_time_stamp() {
  return ::main_time;
}

class Trace {
public:
  Trace(Vtop* top, const char* filename) {
    Verilated::traceEverOn(true);
    tp_ = new VerilatedVcdC;
    top->trace(tp_, 99);  // trace 99 levels of hierarchy
    tp_->spTrace()->set_time_resolution("ps");
    tp_->spTrace()->set_time_unit("ps");
    tp_->open(filename);
  }

  virtual ~Trace() {
    tp_->close();
    delete tp_;
  }

  void dump() {
    tp_->dump(main_time);
  }

  void exit() {
    tp_->flush();
  }

private:
  VerilatedVcdC *tp_;
};

class Clock {
public:
  Clock(vluint32_t period, vluint32_t offset = 0, bool val = false) {
    period_ = period;
    val_ = val;
    now_ = offset % (period/2);
  }

  vluint32_t next_edge() {
    return abs(period_/2 - now_);
  }

  void advance(vluint32_t step = 0) {
    now_ += step;
    if (now_ == period_/2) {
      val_ = !val_;
      now_ = 0;
    }
  }

  bool state() {
    return val_;
  }

private:
  vluint32_t period_ = 0;
  vluint32_t now_ = 0;
  bool val_ = false;
};

Clock *clk_;
Trace *trace_;

void init() {
  clk_ = new Clock(CLK_PERIOD);
  top_->clk = clk_->state();
  top_->d = 0;

  // eval and dump waveform
  top_->eval();
  trace_->dump();
}

vluint32_t step;

void tick() {
  step = clk_->next_edge();
  clk_->advance(step);
  top_->clk = clk_->state();
  top_->eval();

  if (clk_->state()) {
    step += 1; // pseudo output delay
    clk_->advance(1);
    top_->d = !top_->d;
    top_->eval();
  }

  main_time += step;
  trace_->dump();
  trace_->exit();
}

int main(int argc, char** argv) {
  Verilated::commandArgs(argc, argv); // remember args

  top_ = new Vtop;
  trace_ = new Trace(top_, "wave.vcd");

  init();
  for (int i = 0; i < 30; i++) {
    tick();
  }

  top_->final(); // done simulating
  delete top_; // destroy model
  trace_->exit();

  exit(0);
}
