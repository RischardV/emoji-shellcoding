/*
 * (c) 2023-2023 Hadrien Barral
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
namespace fs = std::filesystem;

using u8 = uint8_t;

class Gadgets
{
public:
    uint64_t t2 = 1;
    uint64_t s1 = 1;
    uint64_t t0 = 9;
    uint64_t ra = 1;

    enum Reg {
        T0, T2, RA, A1, ZR, S1
    };

    class AbsoluteG {
    public:
        Reg reg;
        int64_t offset;
        size_t size_bytes;
        std::string text;

        AbsoluteG(Reg _reg, int64_t _offset, size_t _size_bytes, std::string _text) {
            reg = _reg;
            offset = _offset;
            size_bytes = _size_bytes;
            text = _text;
        }
    };

    class RelG {
    public:
        enum Op {
            ADD, ANDI
        };

        Op op;
        int64_t operand;
        Reg reg;
        size_t size_bytes;
        std::string text;

        RelG(Op _op, int64_t _operand, Reg _reg, size_t _size_bytes, std::string _text) {
            op = _op;
            operand = _operand;
            reg = _reg;
            size_bytes = _size_bytes;
            text = _text;
        }
    };

    std::vector<AbsoluteG> absolutes;
    std::vector<RelG> rel;

    uint64_t reg_to_val(Reg reg) const {
        uint64_t val;
        switch(reg) {
            case T0:
                val = t0;
                break;
            case T2:
                val = t2;
                break;
            case RA:
                val = ra;
                break;
            case S1:
                val = s1;
                break;
            case ZR:
                val = 0;
                break;
            default:
                assert(0);
        }
        return val;
    }

    std::pair<u8, AbsoluteG> solve(AbsoluteG ag) {
        uint64_t val;
        if (ag.reg == A1) {
            val = 0;
        } else {
            val = reg_to_val(ag.reg);
        }
        return std::make_pair(val + (uint64_t)ag.offset, ag);
    }

    size_t GAD_SZ_P6 = 6;
    size_t jumper_with_nonzero_a3_size = 10;
    size_t GAD_SZ_J4 = jumper_with_nonzero_a3_size + 4;
    size_t GAD_SZ_J10 = jumper_with_nonzero_a3_size + 10;
    size_t GAD_SZ_J32 = jumper_with_nonzero_a3_size + 32;
    size_t GAD_SZ_J100 = jumper_with_nonzero_a3_size + 100;
    size_t GAD_SZ_J130 = jumper_with_nonzero_a3_size + 130;
    size_t GAD_SZ_J132 = jumper_with_nonzero_a3_size + 132;

    Gadgets() {
        absolutes.push_back(AbsoluteG(T0, -1330, GAD_SZ_J10 , "k2_t0_plus_m1330_to_a1")); /* [9385e2ac] */
        absolutes.push_back(AbsoluteG(T0, -1458, GAD_SZ_J130, "k2_t0_plus_m1458_to_a1")); /* [9385e2a4] */
        absolutes.push_back(AbsoluteG(T0, -1554, GAD_SZ_J10 , "k2_t0_plus_m1554_to_a1")); /* [9385e29e] */
        absolutes.push_back(AbsoluteG(T0, -1586, GAD_SZ_J32 , "k2_t0_plus_m1586_to_a1")); /* [9385e29c] */
        absolutes.push_back(AbsoluteG(T0, -1602, GAD_SZ_J10 , "k2_t0_plus_m1602_to_a1")); /* [9385e29b] */
        absolutes.push_back(AbsoluteG(T0, -1618, GAD_SZ_J10 , "k2_t0_plus_m1618_to_a1")); /* [9385e29a] */
        absolutes.push_back(AbsoluteG(T0, -1634, GAD_SZ_J10 , "k2_t0_plus_m1634_to_a1")); /* [9385e299] */
        absolutes.push_back(AbsoluteG(T0, -1650, GAD_SZ_J10 , "k2_t0_plus_m1650_to_a1")); /* [9385e298] */
        absolutes.push_back(AbsoluteG(T0, -1794, GAD_SZ_J10 , "k2_t0_plus_m1794_to_a1")); /* [9385e28f] */
        absolutes.push_back(AbsoluteG(T0, -1938, GAD_SZ_J10 , "k2_t0_plus_m1938_to_a1")); /* [9385e286] */
        absolutes.push_back(AbsoluteG(T0, -1970, GAD_SZ_J100, "k2_t0_plus_m1970_to_a1")); /* [9385e284] */
        absolutes.push_back(AbsoluteG(T0, -2018, GAD_SZ_J32 , "k2_t0_plus_m2018_to_a1")); /* [9385e281] */
        absolutes.push_back(AbsoluteG(T2, -1874, GAD_SZ_J10 , "k2_t2_plus_m1874_to_a1")); /* [9385e38a] */
        absolutes.push_back(AbsoluteG(T2, -2034, GAD_SZ_J132, "k2_t2_plus_m2034_to_a1")); /* [9385e380] */
        absolutes.push_back(AbsoluteG(A1,     0, GAD_SZ_J4  , "k2_a1_to_0")); /* [9385e286] */

        rel.push_back(RelG(RelG::Op::ADD,    0, S1, GAD_SZ_J4, "incr_a1_by_s1"));
        rel.push_back(RelG(RelG::Op::ADD,    0, T0, GAD_SZ_J4, "incr_a1_by_t0"));
        rel.push_back(RelG(RelG::Op::ADD,    0, T2, GAD_SZ_P6, "incr_a1_by_t2"));
        rel.push_back(RelG(RelG::Op::ANDI,   3, ZR, GAD_SZ_J4, "k2_a1_andi_3"));
        rel.push_back(RelG(RelG::Op::ANDI,   4, ZR, GAD_SZ_J4, "k2_a1_andi_4"));
        rel.push_back(RelG(RelG::Op::ANDI,   6, ZR, GAD_SZ_J4, "k2_a1_andi_6"));
        rel.push_back(RelG(RelG::Op::ANDI,   9, ZR, GAD_SZ_J4, "k2_a1_andi_9"));
        rel.push_back(RelG(RelG::Op::ANDI, -23, ZR, GAD_SZ_J4, "k2_a1_andi_m23"));
        rel.push_back(RelG(RelG::Op::ANDI, -27, ZR, GAD_SZ_J4, "k2_a1_andi_m27"));
        rel.push_back(RelG(RelG::Op::ANDI, -28, ZR, GAD_SZ_J4, "k2_a1_andi_m28"));
        rel.push_back(RelG(RelG::Op::ANDI, -29, ZR, GAD_SZ_J4, "k2_a1_andi_m29"));
    }

    u8 compute(u8 i, const RelG &relg) const {
        u8 j;
        switch (relg.op) {
            case RelG::Op::ADD:
                j = i + reg_to_val(relg.reg) + relg.operand;
                break;
            case RelG::Op::ANDI:
                j =  i & (u8)relg.operand;
                break;
            default:
                assert(0);
        }
        return j;
    }
};

class Strategy
{
public:
    size_t nb_bytes;
    size_t nb_gadgets;
    std::vector<std::string> gadgets;
};

void solve(u8 from, std::vector<Strategy> &to, const Gadgets &gadgets)
{
    /* Add the source */
    to[from].nb_bytes = 0;
    to[from].nb_gadgets = 0;
    to[from].gadgets.clear();

    size_t depth = 0;
    bool done = true;
    do {
        done = true;

        if (depth < 1000) {
            done = false;
        }

        for (int i=0; i<256; i++) {
            if (to[i].nb_bytes == depth) {
                for (auto rel: gadgets.rel) {
                    u8 j = gadgets.compute(i, rel);
                    size_t new_depth = to[i].nb_bytes + rel.size_bytes;
                    if (new_depth < to[j].nb_bytes) {
                        to[j].nb_bytes = to[i].nb_bytes + rel.size_bytes;
                        to[j].nb_gadgets = to[i].nb_gadgets + 1;
                        to[j].gadgets = to[i].gadgets;
                        to[j].gadgets.push_back(rel.text);
                        done = false;
                    }
                }
            }
        }

        depth++;
    } while (!done);
}

using Strategies = std::vector<std::vector<Strategy>>;

static Strategies solve_strategies(uint64_t t0_val, uint64_t s1_val)
{
    Strategies fromto;
    fromto.resize(256);
    for (int i=0; i<256; i++) {
        fromto[i].resize(256);
        for (int j=0; j<256; j++) {
            fromto[i][j].nb_bytes = 10000;
            fromto[i][j].nb_gadgets = 10000;
        }
    }

    Gadgets gadgets;
    gadgets.t0 = t0_val;
    gadgets.s1 = s1_val;

    std::vector<std::pair<u8, Gadgets::AbsoluteG>> start_abs;
    for (auto ag: gadgets.absolutes) {
        start_abs.push_back(gadgets.solve(ag));
    }

    for (int i=0; i<256; i++) {
        for (auto sval: start_abs) {
            if (sval.second.size_bytes < fromto[i][sval.first].nb_bytes) {
                fromto[i][sval.first].nb_bytes = sval.second.size_bytes;
                fromto[i][sval.first].nb_gadgets = 1;
                fromto[i][sval.first].gadgets.clear();
                fromto[i][sval.first].gadgets.push_back(sval.second.text);
            }
        }
    }

    for (int i=0; i<256; i++) {
        solve(i, fromto[i], gadgets);
    }

    return fromto;
}

static size_t encode(std::vector<u8> payload, const Strategies &fromto)
{
    size_t nb_bytes = 0;

    u8 val = 1;
    for (u8 c: payload) {
        nb_bytes += fromto[val][c].nb_bytes;
        val = c;
    }

    return nb_bytes;
}

static int tryhard(int argc, char *argv[])
{
    if (argc != 3) {
        std::cerr << "usage: " << ((argc>0) ? argv[0] : "<progname>") << " -t <payload_dir>" << std::endl;
        return 1;
    }

    std::map<std::string, std::vector<u8>> payloads;
    std::string path = argv[2];
    for (const auto &entry : fs::directory_iterator(path)) {
        std::ifstream file(entry.path(), std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            std::cerr << "Failed to read file '" << entry.path() << "'" << std::endl;
            return 1;
        }
        std::vector<unsigned char> ubuffer(buffer.begin(), buffer.end());
        payloads.insert(std::pair(entry.path().filename(), ubuffer));
    }

    for (uint64_t t0_val=1; t0_val<256; t0_val++) {
        for (uint64_t s1_val=1; s1_val<256; s1_val++) {
            Strategies fromto = solve_strategies(t0_val, s1_val);
            for (auto const& [key, payload]  : payloads) {
                size_t nb_bytes = encode(payload, fromto);
                std::cout << key.substr(8, 8) << " with t0:" << std::setw(3) << t0_val << " s1:"
                          << std::setw(3)  << s1_val << " -> " << std::setw(4) << nb_bytes << std::endl;
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if ((argc > 1) && strcmp(argv[1], "-t") == 0) {
        return tryhard(argc, argv);
    }

    if (argc != 4) {
        std::cerr << "usage: " << ((argc>0) ? argv[0] : "<progname>") << " <out_file> <t0_value> <s1_value>" << std::endl;
        return 1;
    }
    std::string out_filename = argv[1];

    std::istringstream iss_t0(argv[2]);
    uint64_t t0_val;
    if (!((iss_t0 >> t0_val) && iss_t0.eof())) {
        std::cerr << "Could not convert t0_value '" << argv[2] << "' to integer." << std::endl;
        return 1;
    }

    std::istringstream iss_s1(argv[3]);
    uint64_t s1_val;
    if (!((iss_s1 >> s1_val) && iss_s1.eof())) {
        std::cerr << "Could not convert t0_value '" << argv[3] << "' to integer." << std::endl;
        return 1;
    }

    Strategies fromto = solve_strategies(t0_val, s1_val);

    std::stringstream out_json;
    out_json << "{" << std::endl;
    for (int i=0; i<256; i++) {
        out_json << "  \"" << i << "\": [" << std::endl;
        for (int j=0; j<256; j++) {
            size_t nbg = fromto[i][j].nb_gadgets;
            assert(nbg < 100);
            out_json << "    { \"nb_gadgets\":" << nbg << ", \"nb_bytes\":"
                     << fromto[i][j].nb_bytes << ", \"gadgets\":[ ";
            for (const auto &gadget: fromto[i][j].gadgets) {
                out_json << "\"" << gadget << "\",";
            }
            out_json.seekp(-1,out_json.cur);
            out_json << " ] }" << ((j<255) ? "," : "") << std::endl;
        }
        out_json << "  ]" << ((i<255) ? "," : "") << std::endl;
    }
    out_json << "}" << std::endl;

    std::ofstream out_file;

    out_file.open(out_filename, std::ios::out);

    if (!out_file) {
        std::cerr << "Could not open output file '" << out_filename << "'" << std::endl;
        return 1;
    }

    out_file << out_json.str();
    out_file.close();
    return 0;
}
