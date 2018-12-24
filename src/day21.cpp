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

namespace day21 {

  const bool trace_read = false;
  const bool trace1 = false;
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
    int instruction_count = 0;

    int break_on_ip = -1;

    void run(const program_t &program, int limit = std::numeric_limits<int>::max(), bool reset_ip = true) {
      int local_count = 0;
      assert(program.ip_reg != -1);
      if (reset_ip) instruction_pointer = 0;
      if (trace1) std::cout << local_count << ", " << limit << std::endl;
      while (instruction_pointer < program.instructions.size() && local_count < limit) {
        // When the instruction pointer is bound to a register, its value is written to that register just before each instruction is executed,
        // and the value of that register is written back to the instruction pointer immediately after each instruction finishes execution.
        ///
        // Load instruction pointer to reg
        if (trace1) {
          std::cout << std::setw(16) << local_count << " ip: " << std::setw(4) << instruction_pointer << " | "
                    << std::setw(20) << state << "\t -> ";
        }
        state.registers[program.ip_reg] = instruction_pointer;
        auto &instruction_to_run = program.instructions[instruction_pointer];
        process(instruction_to_run);
        if (trace1) {
          std::cout << std::setw(16) << instruction_to_run << "\t -> " << std::setw(16) << state << std::endl;
        }

        if (break_on_ip == instruction_pointer) {
          if (trace1) std::cout << "Breakpoint reached" << std::endl;

          instruction_pointer = state.registers[program.ip_reg];
          instruction_pointer++;
          local_count++;
          instruction_count++;
          break;
        }

        instruction_pointer = state.registers[program.ip_reg];
        instruction_pointer++;
        local_count++;
        instruction_count++;
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

  void read_day21_program_data(program_t &outdata, const char *filepath) {
    std::ifstream input_stream(filepath);
    if (!input_stream.is_open()) {
      std::cerr << "Cannot open file " << filepath << "!" << std::endl;
    }
    input_stream >> outdata;
  }

  void problem1() {
    std::cout << "Day 21 - Problem 1" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 21

    program_t program;
    read_day21_program_data(program, "data/day21/problem1/input.txt");
    // Run program in device
    device_t device;
    // Set the first register to 1
    device.state.registers[0] = 1;
    device.run(program, 3000);

    // [1, 140, 65536, 699230, 0, 21]
    // Instruction 19 multiplies 256 * r[2] to compare with r[3]
    device.state.registers = {1, 255, 65536, 699230, 0, 21};
    device.run(program, 1000, false);

    // Instruction 28 is reached at +48 instructions
    // The equality needs to succeed for the program to halt
    // The r[3] value is 6132825

    std::cout << "Result: " << 6132825 << std::endl;
#endif
  }

  void problem2() {
    std::cout << "Day 21 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 21

    program_t program;
    read_day21_program_data(program, "data/day21/problem2/input.txt");
    // Run program in device
    device_t device;
    // Set the first register to 1
    device.state.registers[0] = 0;

    // Instruction 28 is the key. Let's break on it
    device.break_on_ip = 28;

    // Looking for r[3] values.. (regardless of r[0] value)
    // 0  :   6132825        1
    // 1  :   4997788       93
    // 2  :   5305955       77
    // 3  :   2217896       81
    // 4  :   1614224       33
    // 5  :    744298       25
    // 6  :   3396776       11
    // 7  :  10662635
    // 8  :  11050722
    // 9  :  10662405
    // 10 :   4893776
    // 11 :   5155715
    // 12 :   7508735
    // 13 :  14236206
    // 14 :    457589
    // 15 :   9537596
    // 16 :  13858373
    // 17 :   1139140
    // 18 :  16736942
    // 19 :   6014911
    // 20 :   4363187
    // 21 :   1019955
    // 22 :  15013068
    // 23 :    172093
    // 24 :   9030294
    // 25 :   3249101
    // 26 :   8115995

    // This doesn't seem to have an obvious pattern. Brute force check all r3 values?

    // Everytime the program breaks on instr 28, output the r[3] value and break on the first repeating value
    std::set<long> found;
    long last_unique_r3 = 0;
    for (long i = 0; i < 20000; i++) {
      device.run(program, 10000000, false);
      assert(device.instruction_pointer == 29);
      auto r3 = device.state.registers[3];
      if (found.count(r3) > 0) {
        std::cout << "[" << i << "] Found repeating r3 value : " << r3 << std::endl;
        break;
      }
      std::cout << "r[3] = " << device.state.registers[3] << std::endl;
      last_unique_r3 = r3;
      found.insert(r3);
    }

    std::cout << "Result: " << last_unique_r3 << std::endl;

#endif
  }

}
