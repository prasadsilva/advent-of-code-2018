#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include <list>
#include <regex>
#include <limits>
#include <array>
#include <sstream>

namespace day19 {

  const bool trace_read = false;
  const bool trace1 = true;
  const bool trace2 = false;
  const bool enable_assertions = true;

  enum opcode_e {
    addr = 0,
    addi,
    mulr,
    muli,
    banr,
    bani,
    borr,
    bori,
    setr,
    seti,
    gtir,
    gtri,
    gtrr,
    eqir,
    eqri,
    eqrr,

    num_op_codes,
  };

  std::string opcode_str[] = {
    "addr",
    "addi",
    "mulr",
    "muli",
    "banr",
    "bani",
    "borr",
    "bori",
    "setr",
    "seti",
    "gtir",
    "gtri",
    "gtrr",
    "eqir",
    "eqri",
    "eqrr"
  };

  struct instruction_t {
    int opcode;
    long inputA;
    long inputB;
    long outputC;

    friend std::istream &operator>>(std::istream &in, instruction_t &instr) {
      std::string opstr;
      in >> opstr >> instr.inputA >> instr.inputB >> instr.outputC;
      instr.opcode = -1;
      for (int i = 0; i < opcode_e::num_op_codes; i++) {
        if (opstr == opcode_str[i]) {
          instr.opcode = i;
          break;
        }
      }
      assert(instr.opcode != -1);
      return in;
    }

    friend std::ostream &operator<<(std::ostream &out, const instruction_t &instr) {
      out << opcode_str[instr.opcode] << " " << instr.inputA << " " << instr.inputB << " " << instr.outputC;
      return out;
    }
  };

  struct program_t {
    std::vector<instruction_t> instructions;
    int ip_reg = -1;

    friend std::istream &operator>>(std::istream &in, program_t &program) {
      instruction_t instruction;
      std::string str;
      in >> str >> program.ip_reg;
      if (trace_read) std::cout << "#ip " << program.ip_reg << std::endl;
      getline(in, str);
      while (!in.eof()) {
        in >> instruction;
        program.instructions.push_back(instruction);
        if (trace_read) std::cout << instruction << std::endl;
      }
      return in;
    }

    friend std::ostream &operator<<(std::ostream &out, const program_t &program) {
      out << "#ip " << program.ip_reg << std::endl;
      for (auto &instruction : program.instructions) {
        out << instruction << std::endl;
      }
      return out;
    }
  };

  struct device_state_t {
    std::array<long, 6> registers{0};

    void clear() { registers = std::array<long, 6>{0}; }

    bool operator==(const device_state_t &other) const {
      return registers == other.registers;
    }

    friend std::ostream &operator<<(std::ostream &out, const device_state_t &ds) {
      out << "[" << ds.registers[0] << ", " << ds.registers[1] << ", " << ds.registers[2] << ", " << ds.registers[3]
          << ", " << ds.registers[4] << ", " << ds.registers[5] << "]";
      return out;
    }
  };

  struct device_t {
    device_state_t state;
    int instruction_pointer = 0;

    void run(const program_t &program, int limit = std::numeric_limits<int>::max(), bool reset_ip = true) {
      int count = 0;
      assert(program.ip_reg != -1);
      if (reset_ip) instruction_pointer = 0;
      std::cout << count << ", " << limit << std::endl;
      while (instruction_pointer < program.instructions.size() && count < limit) {
        // When the instruction pointer is bound to a register, its value is written to that register just before each instruction is executed,
        // and the value of that register is written back to the instruction pointer immediately after each instruction finishes execution.
        ///
        // Load instruction pointer to reg
        if (trace1) {
          std::cout << std::setw(16) << count << " ip: " << std::setw(4) << instruction_pointer << " | "
                    << std::setw(20) << state << "\t -> ";
        }
        state.registers[program.ip_reg] = instruction_pointer;
        auto &instruction_to_run = program.instructions[instruction_pointer];
        process(instruction_to_run);
        if (trace1) {
          std::cout << std::setw(16) << instruction_to_run << "\t -> " << std::setw(16) << state << std::endl;
        }
        instruction_pointer = state.registers[program.ip_reg];
        instruction_pointer++;
        count++;
      }
    }

    void process(const instruction_t &instruction) {
      auto &r = state.registers;
      auto A = instruction.inputA;
      auto B = instruction.inputB;
      auto C = instruction.outputC;
      switch (instruction.opcode) {
        case addr: {
          // (add register) stores into register C the result of adding register A and register B.
          r[C] = r[A] + r[B];
          break;
        }
        case addi: {
          // (add immediate) stores into register C the result of adding register A and value B.
          r[C] = r[A] + B;
          break;
        }
        case mulr: {
          // (multiply register) stores into register C the result of multiplying register A and register B.
          r[C] = r[A] * r[B];
          break;
        }
        case muli: {
          // (multiply immediate) stores into register C the result of multiplying register A and value B.
          r[C] = r[A] * B;
          break;
        }
        case banr: {
          // (bitwise AND register) stores into register C the result of the bitwise AND of register A and register B.
          r[C] = r[A] & r[B];
          break;
        }
        case bani: {
          // (bitwise AND immediate) stores into register C the result of the bitwise AND of register A and value B.
          r[C] = r[A] & B;
          break;
        }
        case borr: {
          // (bitwise OR register) stores into register C the result of the bitwise OR of register A and register B.
          r[C] = r[A] | r[B];
          break;
        }
        case bori: {
          // (bitwise OR immediate) stores into register C the result of the bitwise OR of register A and value B.
          r[C] = r[A] | B;
          break;
        }
        case setr: {
          // (set register) copies the contents of register A into register C. (Input B is ignored.)
          r[C] = r[A];
          break;
        }
        case seti: {
          // (set immediate) stores value A into register C. (Input B is ignored.)
          r[C] = A;
          break;
        }
        case gtir: {
          // (greater-than immediate/register) sets register C to 1 if value A is greater than register B. Otherwise, register C is set to 0.
          r[C] = (A > r[B]) ? 1 : 0;
          break;
        }
        case gtri: {
          // (greater-than register/immediate) sets register C to 1 if register A is greater than value B. Otherwise, register C is set to 0.
          r[C] = (r[A] > B) ? 1 : 0;
          break;
        }
        case gtrr: {
          // (greater-than register/register) sets register C to 1 if register A is greater than register B. Otherwise, register C is set to 0.
          r[C] = (r[A] > r[B]) ? 1 : 0;
          break;
        }
        case eqir: {
          // (equal immediate/register) sets register C to 1 if value A is equal to register B. Otherwise, register C is set to 0.
          r[C] = (A == r[B]) ? 1 : 0;
          break;
        }
        case eqri: {
          // (equal register/immediate) sets register C to 1 if register A is equal to value B. Otherwise, register C is set to 0.
          r[C] = (r[A] == B) ? 1 : 0;
          break;
        }
        case eqrr: {
          // (equal register/register) sets register C to 1 if register A is equal to register B. Otherwise, register C is set to 0.
          r[C] = (r[A] == r[B]) ? 1 : 0;
          break;
        }
        default: {
          assert("Unknown opcode!");
        }
      }
    }
  };

  void read_day19_program_data(program_t &outdata, const char *filepath) {
    std::ifstream input_stream(filepath);
    if (!input_stream.is_open()) {
      std::cerr << "Cannot open file " << filepath << "!" << std::endl;
    }
    input_stream >> outdata;
  }

  void problem1() {
    std::cout << "Day 19 - Problem 1" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 19

    if (enable_assertions) {
      program_t program;
      read_day19_program_data(program, "data/day19/problem1/test1.txt");
      // Run program in device
      device_t device;
      device.run(program);
      assert(device.state.registers[0] == 6);
    }

    // program_t program;
    // read_day19_program_data(program, "data/day19/problem1/input.txt");
    // // Run program in device
    // device_t device;
    // device.run(program);
    // std::cout << "Result: " << device.state.registers[0] << std::endl;

#endif
  }

  void problem2() {
    std::cout << "Day 19 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 19

    program_t program;
    read_day19_program_data(program, "data/day19/problem2/input.txt");
    // Run program in device
    device_t device;
    // Set the first register to 1
    device.state.registers[0] = 1;
    device.run(program, 1000);

    // Taking too long. Need to trace the execution to find patterns.
    // Alternatively can also trace the instructions in the program to identify the nested loops
    // - See data/day19/instr_trace.txt

    // Current state [0, 5, 10551403, 1, 0, 123]
    // Pattern: r[5] is incrementing. Let's try setting it to r[2] - 1
    device.state.registers = {0, 5, 10551403, 1, 0, 10551402};
    device.run(program, 1000, false);

    // Current state [1, 10, 10551403, 2, 0, 124]
    // Pattern: r[3] is incrementing. Let's try setting it to 128
    device.state.registers = {1, 10, 10551403, 128, 0, 124};
    device.run(program, 1000, false);

    // Current state [1, 10, 10551403, 128, 0, 249]
    // This is going nowhere. To jump out of the loop r[3] and r[5] need to multiply to 10551403
    // 1 * 10551403 = 10551403 (already occured in first break)
    // 19 * 555337 = 10551403
    // 555337 * 19 = 10551403
    // 10551403 * 1 = 10551403
    // Let's try setting r[3] and r[5] to the rest of the factors
    device.state.registers = {1, 10, 10551403, 19, 0, 555337};
    device.run(program, 1000, false);

    // Current state [20, 10, 10551403, 19, 0, 555462]
    // r[3] was added to r[0]
    // Let's try the next pair of factors
    device.state.registers = {20, 10, 10551403, 555337, 0, 19};
    device.run(program, 1000, false);

    // Current state [555357, 10, 10551403, 555337, 0, 144]
    // Let's try the last pair of factors
    device.state.registers = {555357, 10, 10551403, 10551403, 0, 1};
    device.run(program, 1000, false);

    // Current state [11106760, 10, 10551403, 10551403, 0, 126]
    // The main loop will exit if r[3] > r[2]. Let's make that happen
    device.state.registers = {11106760, 10, 10551403, 10551403, 0, 10551403};
    device.run(program, 1000, false);

    std::cout << "Result: " << device.state.registers[0] << std::endl;

#endif
  }

}
