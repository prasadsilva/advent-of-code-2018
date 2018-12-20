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

const bool trace_read = false;
const bool trace1 = false;
const bool trace2 = true;
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

struct instruction_t {
    int opcode;
    int inputA;
    int inputB;
    int outputC;

    friend std::istream & operator >> (std::istream &in, instruction_t &instr) {
        in >> instr.opcode >> instr.inputA >> instr.inputB >> instr.outputC;
        return in;
    }

    friend std::ostream & operator << (std::ostream &out, const instruction_t &instr) {
        out << instr.opcode << " " << instr.inputA << " " << instr.inputB << " " << instr.outputC;
        return out;
    }
};

struct program_t {
    std::vector<instruction_t> instructions;

    void remap_opcodes(const std::array<int, opcode_e::num_op_codes> &mapping) {
        for (auto &instruction : instructions) {
            instruction.opcode = mapping[instruction.opcode];
        }
    }

    friend std::istream & operator >> (std::istream &in, program_t &program) {
        instruction_t instruction;
        while (!in.eof()) {
            in >> instruction;
            program.instructions.push_back(instruction);
        }
        return in;
    }

    friend std::ostream & operator << (std::ostream &out, const program_t &program) {
        for (auto &instruction : program.instructions) {
            out << instruction << std::endl;
        }
        return out;
    }
};

struct device_state_t {
    std::array<int, 4> registers{0};

    void clear() { registers = std::array<int, 4>{0}; }

    bool operator == (const device_state_t &other) const {
        return registers == other.registers;
    }

    friend std::ostream & operator << (std::ostream &out, const device_state_t &ds) {
        out << "[" << ds.registers[0] << ", " << ds.registers[1] << ", " << ds.registers[2] << ", " << ds.registers[3] << "]";
        return out;
    }
};

struct device_t {
    device_state_t state;

    void clear() { state.clear(); }

    void run(const program_t &program) {
        clear();
        for (auto &instruction : program.instructions) {
            process(instruction);
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
                r[C] = A > r[B] ? 1 : 0;
                break; 
            }
            case gtri: { 
                // (greater-than register/immediate) sets register C to 1 if register A is greater than value B. Otherwise, register C is set to 0.
                r[C] = r[A] > B ? 1 : 0;
                break; 
            }
            case gtrr: { 
                // (greater-than register/register) sets register C to 1 if register A is greater than register B. Otherwise, register C is set to 0.
                r[C] = r[A] > r[B] ? 1 : 0;
                break; 
            }
            case eqir: { 
                // (equal immediate/register) sets register C to 1 if value A is equal to register B. Otherwise, register C is set to 0.
                r[C] = A == r[B] ? 1 : 0;
                break; 
            }
            case eqri: { 
                // (equal register/immediate) sets register C to 1 if register A is equal to value B. Otherwise, register C is set to 0.
                r[C] = r[A] == B ? 1 : 0;
                break; 
            }
            case eqrr: { 
                // (equal register/register) sets register C to 1 if register A is equal to register B. Otherwise, register C is set to 0.
                r[C] = r[A] == r[B] ? 1 : 0;
                break; 
            }
            default: {
                assert("Unknown opcode!");
            }
        }
    }
};

struct instruction_event_t {
    device_state_t before;
    instruction_t instruction;
    device_state_t after;
};

struct instruction_trace_t {
    instruction_t event_instruction;
    std::vector<int> candidate_opcodes;
};

struct instruction_tracer_t {
    device_t device;
    std::vector<instruction_trace_t> traces;

    void process_event(const instruction_event_t &e) {
        // Opcode is unknown. Generate traces
        instruction_trace_t trace;
        trace.event_instruction = e.instruction;
        if (trace1) std::cout << "Processing event instr " << e.instruction << std::endl;
        for (int candidate_opcode = 0; candidate_opcode < opcode_e::num_op_codes; candidate_opcode++) {
            device.clear();
            device.state = e.before;
            instruction_t candidate_test_instruction = e.instruction;
            candidate_test_instruction.opcode = candidate_opcode;
            device.process(candidate_test_instruction);
            if (device.state == e.after) {
                if (trace1) std::cout << "\tOpcode " << candidate_opcode << " produced expected output" << std::endl;
                trace.candidate_opcodes.push_back(candidate_opcode);
            }
        }
        traces.push_back(trace);
    }

    int get_samples_with_more_than_3_candidate_opcodes() {
        int result = 0;
        for (auto &t : traces) {
            if (t.candidate_opcodes.size() >= 3) {
                result++;
            }
        }
        return result;
    }

    friend std::istream & operator >> (std::istream &in, instruction_tracer_t &tracer) {
        while (!in.eof()) {
            std::string line;
            getline(in, line);
            if (line.empty()) continue;

            std::regex before_pattern(R"(Before: \[(\d+), (\d+), (\d+), (\d+)\])");
            std::smatch before_matches;
            if (!std::regex_search(line, before_matches, before_pattern)) {
                throw new std::invalid_argument("Cannot parse before state!");
            }
            instruction_event_t e;
            e.before.registers[0] = std::stoi(before_matches[1].str());
            e.before.registers[1] = std::stoi(before_matches[2].str());
            e.before.registers[2] = std::stoi(before_matches[3].str());
            e.before.registers[3] = std::stoi(before_matches[4].str());

            in >> e.instruction;
            // Consume newline
            getline(in, line);

            getline(in, line);
            std::regex after_pattern(R"(After:  \[(\d+), (\d+), (\d+), (\d+)\])");
            std::smatch after_matches;
            if (!std::regex_search(line, after_matches, after_pattern)) {
                throw new std::invalid_argument("Cannot parse after state!");
            }
            e.after.registers[0] = std::stoi(after_matches[1].str());
            e.after.registers[1] = std::stoi(after_matches[2].str());
            e.after.registers[2] = std::stoi(after_matches[3].str());
            e.after.registers[3] = std::stoi(after_matches[4].str());

            if (trace_read) {
                std::cout << "Before: " << e.before << std::endl;
                std::cout << e.instruction << std::endl;
                std::cout << "After: " << e.after << std::endl;
            }

            tracer.process_event(e);
        }

        return in;
    }
};

struct opcode_mapper_t {
    std::array<std::set<int>, opcode_e::num_op_codes> internal_to_trace_opcode_mapping;

    std::array<int, opcode_e::num_op_codes> process_traces(const std::vector<instruction_trace_t> &input) {
        std::fill(internal_to_trace_opcode_mapping.begin(), internal_to_trace_opcode_mapping.end(), std::set<int>());
        for (auto& trace : input) {
            auto trace_opcode = trace.event_instruction.opcode;
            for (auto &candidate_opcode : trace.candidate_opcodes) {
                internal_to_trace_opcode_mapping[candidate_opcode].insert(trace_opcode);
            }
        }
        if (trace2) {
            std::cout << "Instruction mapping (internal -> trace)" << std::endl;
            for (int i = 0; i < internal_to_trace_opcode_mapping.size(); i++) {
                std::cout << std::setw(2) << i << " -> ";
                for (auto trace_opcode : internal_to_trace_opcode_mapping[i]) {
                    std::cout << std::setw(2) << trace_opcode << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        std::array<int, opcode_e::num_op_codes> trace_to_internal_mapping;
        // Deduction loop
        for (int i = 0; i < opcode_e::num_op_codes; i++) {
            // Find internal opcode with a single trace opcode mapping
            int mapped_internal_opcode = -1;
            int trace_opcode = -1;
            for (int internal_opcode = 0; internal_opcode < opcode_e::num_op_codes; internal_opcode++) {
                auto &mapping = internal_to_trace_opcode_mapping[internal_opcode];
                if (mapping.size() == 1) {
                    mapped_internal_opcode = internal_opcode;
                    trace_opcode = *mapping.begin();
                    break;
                }
            }
            assert(trace_opcode != -1);
            trace_to_internal_mapping[trace_opcode] = mapped_internal_opcode;
            if (trace2) std::cout << "Mapping " << trace_opcode << " -> " << mapped_internal_opcode << std::endl;
            // Remove trace opcode from mappings
            for (auto &mapping : internal_to_trace_opcode_mapping) {
                mapping.erase(trace_opcode);
            }
        }

        return trace_to_internal_mapping;
    }
};

void read_day16_trace_data(instruction_tracer_t &outdata, const char* filepath) {
    std::ifstream input_stream(filepath);
    input_stream >> outdata;
}

void read_day16_program_data(program_t &outdata, const char* filepath) {
    std::ifstream input_stream(filepath);
    input_stream >> outdata;
}

namespace day16 {

    void problem1() {
        std::cout << "Day 16 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 16

        if (enable_assertions) {
            instruction_tracer_t tracer;
            read_day16_trace_data(tracer, "data/day16/problem1/test1_trace.txt");    
            assert(tracer.get_samples_with_more_than_3_candidate_opcodes() == 1);
        }

        instruction_tracer_t tracer;
        read_day16_trace_data(tracer, "data/day16/problem1/input_trace.txt");
        std::cout << "Result: " << tracer.get_samples_with_more_than_3_candidate_opcodes() << std::endl;

        #endif
    }

    void problem2() {
        std::cout << "Day 16 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 16

        instruction_tracer_t tracer;
        read_day16_trace_data(tracer, "data/day16/problem2/input_trace.txt");
        opcode_mapper_t opcode_mapper;
        auto opcode_mapping = opcode_mapper.process_traces(tracer.traces);
        // Read program and remap opcodes
        program_t program;
        read_day16_program_data(program, "data/day16/problem2/input_program.txt");
        program.remap_opcodes(opcode_mapping);
        // Run program in device
        device_t device;
        device.run(program);
        std::cout << "Result: " << device.state.registers[0] << std::endl;

        #endif
    }

}