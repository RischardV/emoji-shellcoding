/*
 * (c) 2022-2022 Hadrien Barral
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cassert>
#include <cstring>
#include <chrono>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

#define KBLD "\x1b[1m"
#define KRED "\x1b[31m"
#define KGRN "\x1b[32m"
#define KYLW "\x1b[33m"
#define KRST "\x1b[0m"

typedef std::vector<uint8_t> Bytes;

std::ostream& operator<< (std::ostream& out, const Bytes& bytes) {
    out << '[';
    for (size_t i=0; i<bytes.size(); i++) {
        out << std::hex << std::setfill('0') << std::setw(2) << (int)bytes[i] <<  std::dec;
    }
    out << ']';
    return out;
}

static uint8_t hex2byte(char a, char b)
{
    uint8_t val = 0;
    if (a != '\0') {
        val += hex2byte('\0', a) << 4;
    }
    if ((b >= '0') && (b <= '9')) {
        val += b - '0';
    } else if ((b >= 'A') && (b <= 'F')) {
        val += b - 'A' + 10;
    } else if ((b >= 'a') && (b <= 'f')) {
        val += b - 'a' + 10;
    } else {
        std::cerr << "Invalid char '" << b << "' in hex2byte" << std::endl;
        exit(1);
    }
    return val;
}

static Bytes hex_to_bytes(const std::string &hex)
{
    Bytes bytes;
    assert(hex.size() % 2 == 0);
    size_t pos = 0;
    while((pos+1 < hex.size())) {
        bytes.push_back(hex2byte(hex[pos], hex[pos+1]));
        pos += 2;
    }
    return bytes;
}

static bool is_bytes_prefix(const Bytes &prefix, const Bytes &bytes)
{
    if (prefix.size() > bytes.size()) {
        return false;
    }
    for (size_t i=0; i<prefix.size(); i++) {
        if (prefix[i] != bytes[i]) {
            return false;
        }
    }
    return true;
}

static bool is_bytes_suffix(const Bytes &suffix, const Bytes &bytes)
{
    if (suffix.size() > bytes.size()) {
        return false;
    }
    for (size_t i=0; i<suffix.size(); i++) {
        if (suffix[i] != bytes[bytes.size()-suffix.size()+i]) {
            return false;
        }
    }
    return true;
}

class Sequence {
    Bytes bytes;

public:
    Sequence(const std::string &hex) {
        bytes = hex_to_bytes(hex);
    }
};

class Instruction {
    Bytes _bytes;
    std::string _text;

    std::string adjust_text(const Bytes &instr_bytes, const std::string &instr_text) {
        if ((instr_text.rfind("jal\t", 0) == 0) || instr_text.rfind("j\t", 0) == 0) {
            std::stringstream stream;
            bool neg;
            uint32_t offset;
            if (instr_bytes.size() == 2) {
                uint16_t instr16 = ((uint16_t)instr_bytes[1]) << 8 | instr_bytes[0];
                /*
                 * 15     13 12  2 1  0
                 *  [funct3] [imm] [op]
                 */
                uint16_t imm = (instr16 >> 2) & 0x7FFU;
                neg = (imm & (1U << 10)) != 0;
                offset = (((imm >> 1) & 0b111) << 1)
                       | (((imm >> 9) & 0b1  ) << 4)
                       | (((imm >> 0) & 0b1  ) << 5)
                       | (((imm >> 5) & 0b1  ) << 6)
                       | (((imm >> 4) & 0b1  ) << 7)
                       | (((imm >> 7) & 0b11 ) << 8)
                       | (((imm >> 6) & 0b1  ) << 10);
            } else if (instr_bytes.size() == 4) {
                uint32_t instr32 = (((uint32_t)instr_bytes[3]) << 24)
                                 | (((uint32_t)instr_bytes[2]) << 16)
                                 | (((uint32_t)instr_bytes[1]) <<  8)
                                 | (((uint32_t)instr_bytes[0]) <<  0);
                /*
                 *   31    30    21    20    19     12  11 7 6    0
                 * imm[20] imm[10:1] imm[11] imm[19:12]  rd  opcode
                 */
                neg = (instr32 & (1U << 31)) != 0;
                offset = (((instr32 >> 21) & 0x3FF) << 1)
                       | (((instr32 >> 20) & 0b1  ) << 11)
                       | (((instr32 >> 12) & 0xFF ) << 12);
                if (neg) {
                    offset = (1U << 20) - offset;
                }
            } else {
                assert(0);
            }
            stream << instr_text.substr(0, instr_text.find("0x"))
                   << (neg ? "-" : "+") << std::hex << "0x" << offset;
            return stream.str();
        }
        if ((instr_text.rfind("bnez\t", 0) == 0) || instr_text.rfind("beqz\t", 0) == 0) {
            std::stringstream stream;
            bool neg;
            uint32_t offset;
            if (instr_bytes.size() == 2) {
                uint16_t instr16 = ((uint16_t)instr_bytes[1]) << 8 | instr_bytes[0];
                neg = (instr16 & (1U << 12)) != 0;
                offset = (((instr16 >>  3) & 0b11) << 1)
                       | (((instr16 >> 10) & 0b11) << 3)
                       | (((instr16 >>  2) & 0b1 ) << 5)
                       | (((instr16 >>  5) & 0b11) << 6);
                if (neg) {
                    offset = (1U << 8) - offset;
                }
            } else if (instr_bytes.size() == 4) {
                /* Not handled yet */
                return instr_text;
            } else {
                assert(0);
            }
            stream << instr_text.substr(0, instr_text.find("0x"))
                   << (neg ? "-" : "+") << std::hex << "0x" << offset;
            return stream.str();
        }
        return instr_text;
    }

public:
    Instruction(void) {
        _text = "<empty>";
    }
    Instruction(const std::string &instr_hex, const std::string &instr_text) {
        _bytes = hex_to_bytes(instr_hex);
        _text = adjust_text(_bytes, instr_text);
    }
    Instruction(const Bytes &instr_bytes, const std::string &instr_text) {
        _bytes = instr_bytes;
        _text = adjust_text(_bytes, instr_text);
    }

    size_t size(void) const {
        return _bytes.size();
    }

    const Bytes &bytes(void) const {
        return _bytes;
    }

    const std::string &text(void) const {
        return _text;
    }

    bool is_jump(void) const {
        /* Well, this does the job... */
        return (this->_text[0] == 'j') || (this->_text[0] == 'b');
    }

    bool is_must_jump(void) const {
        /* Well, this does the job... */
        return (this->_text[0] == 'j') || ((this->_text[0] == 'b') && (this->_text[1] == '\t'));
    }
};

typedef std::vector<std::deque<Instruction>> BlockList;
class Block {
    static constexpr size_t max_size = 5;
public:
    Instruction goal;
    bool is_sequence;
    bool prefix[max_size];
    bool suffix[max_size];
    BlockList prev[max_size];
    BlockList next[max_size];

    Block(const Instruction &in_goal) {
        goal = in_goal;
        is_sequence = false;
        for (size_t i=0; i<max_size; i++) {
            prefix[i] = false;
            suffix[i] = false;
        }
    }

    bool is_good(size_t idx) const {
        if ((suffix[idx] || (prev[idx].size() > 0))
            && ((goal.is_jump() && prefix[idx]) || (next[idx].size() > 0))) {
            return true;
        }
        if (suffix[idx] && ((idx == goal.size()) || (goal.is_jump() && (idx > 0)))) {
            return true;
        }
        return false;
    }

    bool is_good(void) const {
        for (size_t idx=0; idx<=goal.size(); idx++) {
            if (is_good(idx)) {
                return true;
            }
        }
        return false;
    }
};

static std::map<Bytes, Sequence> sequences;
static std::map<Bytes, Instruction> instructions;

static void load_seq(void)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    const char *seq_file = "build/seq.txt";
    std::ifstream file(seq_file);
    if (!file) {
        std::cerr << "Could not open file '" << seq_file << "'" << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        //std::cerr << "XXX:" << line << "##" << std::endl;
        std::string hex = line.substr(0, line.find(" "));
        sequences.insert(std::pair<Bytes, Sequence>(hex_to_bytes(hex), Sequence(hex)));
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cerr << "Loaded " << sequences.size() << " sequences in "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "µs" << std::endl;
}

static void load_instr(bool use_64)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    const char *instr_file = use_64 ? "build/instr64.txt" : "build/instr32.txt";
    std::ifstream file(instr_file);
    if (!file) {
        std::cerr << "Could not open file '" << instr_file << "'" << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string hex = line.substr(0, line.find(" "));
        std::string text = line.substr(12);
        //std::cerr << "LI: L:" << line << " ## H:'" << hex << "' T'"<< text << "'" << std::endl;
        instructions.insert(std::pair<Bytes, Instruction>(hex_to_bytes(hex), Instruction(hex, text)));
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cerr << "Loaded " << instructions.size() << " instructions from '" << instr_file << "' in "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "µs" << std::endl;
}

__attribute__((unused))
static void print_block(const Block &block)
{
    const Instruction &goal = block.goal;
    const Bytes &bytes = goal.bytes();
    size_t goal_size = goal.size();

    std::cout << KBLD "PRINT BLOCK " << bytes << KRST << std::endl;

    if (block.is_sequence) {
        std::cout << "!!!SEQUENCE " << bytes << std::endl;
    }

    for (size_t i=0; i<=goal_size; i++) {
        Bytes buf_left = Bytes(bytes.begin(), bytes.begin() + i);
        Bytes buf_right = Bytes(bytes.begin() + i, bytes.end());
        const bool &prefix = block.prefix[i];
        const bool &suffix = block.suffix[i];
        const BlockList &prev = block.prev[i];
        const BlockList &next = block.next[i];

        if (!block.is_good(i)) {
            std::cout << KRED "FAIL FOR " KRST << bytes << " split as " << buf_left << " | " << buf_right
                      <<  "  suffix:" << suffix <<  "  prefix:" << prefix
                      <<  "  prev:" << prev.size() <<  "  next:" << next.size() << std::endl;
        } else {
            std::cout << KGRN "GOOD FOR " KRST << bytes << " split as " << buf_left << " | " << buf_right
                      <<  "  suffix:" << suffix <<  "  prefix:" << prefix
                      <<  "  prev:" << prev.size() <<  "  next:" << next.size() << std::endl;

            for (auto const& [key, seq] : sequences) {
                if ((i > 0) && is_bytes_suffix(buf_left, key)) {
                    std::cout << "%%%%%%S" << std::endl;
                    std::cout << "  " << Bytes(key.begin(), key.end() - buf_left.size())
                              << " <before land>" << std::endl;
                }
            }

            for (const auto &instrs: prev) {
                std::cout << "%%%%%%p" << std::endl;
                for (const auto &instr: instrs) {
                    std::cout << "  " << instr.bytes() << " " << instr.text() << std::endl;
                }
            }

            std::cout << "========" << std::endl;
            std::cout << "OUR GOAL " << bytes << " " << goal.text() << std::endl;
            std::cout << "========" << std::endl;

            if (!goal.is_jump()) {
                for (const auto &instrs: next) {
                    for (const auto &instr: instrs) {
                        std::cout << "  " << instr.bytes() << " " << instr.text() << std::endl;
                    }
                    std::cout << "%%%%%%n" << std::endl;
                }
            } else {
                std::cout << "jump..." << std::endl;
            }
        }
    }
}

static Bytes eat_instructions(const Bytes &old_goal, bool back, std::deque<Instruction> &eaten, size_t *p_eaten_size)
{
    Bytes goal = old_goal;
    bool forward = true;
    size_t eaten_size = 0;
    while(forward) {
        forward = false;
        for (size_t sz = 2; sz<=4; sz+=2) {
            if (goal.size() >= sz) {
                Bytes instr_bytes = back ?
                                    Bytes(goal.end() - sz, goal.end()) :
                                    Bytes(goal.begin(), goal.begin() + sz);
                auto it = instructions.find(instr_bytes);
                if (it != instructions.end()) {
                    const Instruction &instr = it->second;
                    //std::cout << "EA: " << goal << " " << instr.bytes() << " T'" << instr.text() << "'" << std::endl;
                    if (instr.is_jump() && !back) {
                        forward = true;
                        eaten.push_back(instr);
                        if (instr.size() < goal.size()) {
                            Bytes trail = Bytes(goal.begin() + instr.size(), goal.end());
                            eaten.push_back(Instruction(trail, "<after jump>"));
                        }
                        eaten_size = goal.size();
                        goal = Bytes();
                    } else if(instr.is_must_jump() && back) {
                        /* Bad instruction */
                    } else {
                        forward = true;
                        if (back) {
                            eaten.push_front(instr);
                        } else {
                            eaten.push_back(instr);
                        }
                        eaten_size += sz;
                        goal = back ?
                               Bytes(goal.begin(), goal.end() - sz):
                               Bytes(goal.begin() + sz, goal.end());
                    }
                }
            }
        }
    }
    *p_eaten_size = eaten_size;
    return goal;
}

static void make_block(const Bytes &goal, int depth, bool prev,
                       const std::deque<Instruction> &already_eaten, BlockList &list)
{
    std::string logname = prev ? "MBP" : "MBN";
    //std::cerr << logname << ":" << depth << " " << goal << std::endl;
    if (depth >= 3) {
        return;
    }
    if (goal.size() == 0) {
        //std::cerr << logname << " OK" << std::endl;
        if (already_eaten.size() > 0) {
            list.push_back(already_eaten);
        }
        return;
    }

    bool simple_search = true;

    for (auto const& [key, seq] : sequences) {
        /* For depth == 0, goal is part of an instruction */
        if (((depth > 0) && !simple_search) || (prev ? is_bytes_suffix(goal, key) : is_bytes_prefix(goal, key))) {
            Bytes new_goal;
            if (depth == 0) {
                new_goal = (prev ?
                            Bytes(key.begin(), key.end() - goal.size()):
                            Bytes(key.begin() + goal.size(), key.end()));
            } else {
                new_goal = goal;
                new_goal.insert(prev ? new_goal.begin() : new_goal.end(), key.begin(), key.end());
            }

            size_t eaten_size;
            auto eaten = already_eaten;
            Bytes eaten_goal = eat_instructions(new_goal, prev, eaten, &eaten_size);
            //std::cerr << "*" << logname << ":" << depth << " " << goal << " -> " << new_goal << " -> " << eaten_goal << std::endl;
            if (eaten_goal.size() < 4) { /* Else, we have not eaten enough to continue... */
                make_block(eaten_goal, depth+1, prev, eaten, list);
            }
        }
    }
}

static bool block_prefix(const Bytes &goal)
{
    for (auto const& [key, seq] : sequences) {
        if (is_bytes_prefix(goal, key)) {
            return true;
        }
    }
    return false;
}

static bool block_suffix(const Bytes &goal)
{
    for (auto const& [key, seq] : sequences) {
        if (is_bytes_suffix(goal, key)) {
            return true;
        }
    }
    return false;
}

static BlockList make_block_prev(const Bytes &goal)
{
    std::deque<Instruction> eaten;
    BlockList list;
    make_block(goal, 0, true, eaten, list);
    return list;
}

static BlockList make_block_next(const Bytes &goal)
{
    std::deque<Instruction> eaten;
    BlockList list;
    make_block(goal, 0, false, eaten, list);
    return list;
}

static Block make_block(const Instruction &instr) {
    Block block(instr);

    const Bytes &goal = instr.bytes();
    //std::cout << "Trying to find block for instruction " << goal << std::endl;
    if (sequences.find(goal) != sequences.end()) {
        std::cout << "Instruction " << goal << " is a sequence!" << std::endl;
        block.is_sequence = true;
        return block;
    }
    /* FIXME: Handle case where instr is contained in the middle on a single emoji */
    for (size_t i=0; i<=goal.size(); i++) {
        Bytes buf_left = Bytes(goal.begin(), goal.begin() + i);
        Bytes buf_right = Bytes(goal.begin() + i, goal.end());
        //std::cerr << "XXX: " << buf_left << " | " << buf_right << std::endl;
        std::deque<Instruction> eaten;
        block.prefix[i] = block_prefix(buf_right);
        block.suffix[i] = block_suffix(buf_left);
        block.prev[i] = make_block_prev(buf_left);
        block.next[i] = make_block_next(buf_right);
    }

    return block;
}

static void make_good_list(void) {
    for (auto const& [key, instr] : instructions) {
        Block block = make_block(instr);
        //print_block(block);
        if (block.is_good()) {
            bool need_prefix = true;
            for (size_t idx=0; idx<=block.goal.size(); idx++) {
                if (block.is_good(idx)) {
                    if ((idx == 0) || (block.prev[idx].size() > 0)) {
                        need_prefix = false;
                    }
                }
            }

            size_t text_len = instr.text().size();
            std::string text_pad = std::string(40-text_len, ' ');
            std::cout << instr.bytes() << "\t"
                      << instr.text() << text_pad << "\t"
                      << (need_prefix ? "%NP " : "")
                      << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    bool make_list = false;
    bool instr_64 = true;
    int goal_index = -1;

    for (int i=1; i<argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            make_list = true;
        } else if (strcmp(argv[i], "--32") == 0) {
            instr_64 = false;
        } else if (strcmp(argv[i], "--64") == 0) {
            instr_64 = true;
        } else {
            goal_index = i;
        }
    }

    if ((argc < 2) || (!make_list && (goal_index < 0))) {
        std::cerr << "usage: " << ((argc>0) ? argv[0] : "<progname>") << " -l | <hex_string>" << std::endl;
        exit(1);
    }

    load_seq();
    load_instr(instr_64);

    if (make_list) {
        make_good_list();
        return 0;
    }

    Bytes in_bytes = hex_to_bytes(argv[goal_index]);
    Instruction goal = Instruction(in_bytes, "<user>");
    if (instructions.find(in_bytes) != instructions.end()) {
        goal = instructions[in_bytes];
    } else {
        std::cerr << "Warning: could not find " << in_bytes << " in our list of acceptable instructions" << std::endl;
    }

    Block block = make_block(goal);
    print_block(block);

    return 0;
}
