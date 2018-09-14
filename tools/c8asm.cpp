/* 
 * C8ASM
 * Chip-8 assembler.
 *
 * Stefan Wong 2018
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <getopt.h>
// Modules
#include "lexer.hpp"
#include "assembler.hpp"

typedef struct 
{
    std::string infile;
    std::string outfile;
    bool verbose;
} AsmOpts;

void init_opts(AsmOpts& opts)
{
    opts.infile  = "\0";
    opts.outfile = "\0";
    //opts.asm_file = "\0";
    //opts.dump_asm = false;
    opts.verbose = false;
}

int main(int argc, char *argv[])
{
    AsmOpts opts;
    //const char* const short_opts = "vhi:o:sS:";
    const char* const short_opts = "vhi:o:";
    const option long_opts[] = {};
    int argn = 0;
    int status;

    init_opts(opts);

    // get args 
    while(1)
    {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
        if(opt == -1)
            break;

        switch(opt)
        {
            case 'v':
                opts.verbose = true;
                break;

            case 'i':
                opts.infile = std::string(optarg);
                break;

            case 'o':
                opts.outfile = std::string(optarg);
                break;

            //case 's':
            //    opts.dump_asm = true;
            //    break;

            //case 'S':
            //    opts.asm_file = std::string(optarg);
            //    break;

            case 'h':
                std::cout << "TODO : write help text and print here" << std::endl;
                break;

            default:
                std::cerr << "Unknown option " << std::string(optarg) << "(arg " << argn << ")" << std::endl;
                exit(-1);
                break;
        }
        argn++;
    }

    // Check we have valid options 
    if(opts.infile == "\0")
    {
        std::cerr << "Invalid input filename" << std::endl;
        std::cerr << "Use -i <filename> to specify" << std::endl;
        exit(-1);
    }
    if(opts.outfile == "\0")
    {
        std::cerr << "Invalid output filename" << std::endl;
        std::cerr << "Use -o <filename> to specify" << std::endl;
        exit(-1);
    }

    // Get a lexer object 
    Lexer lexer;
    Assembler assembler;
    Program as_prog;

    //lexer.setVerbose(opts.verbose);
    //assembler.setVerbose(opts.verbose);
    status = lexer.loadFile(opts.infile);
    if(status < 0)
    {
        std::cerr << "ERROR unable to load file " << opts.infile << std::endl;
        exit(-1);
    }
    lexer.lex();
    // Assemble the source
    SourceInfo lex_source = lexer.getSourceInfo();
    if(lex_source.hasError())
    {
        std::cout << lex_source.dumpErrors();
        exit(-1);
    }
    assembler.loadSource(lex_source);
    assembler.assemble();
    as_prog = assembler.getProgram();

    status = as_prog.save(opts.outfile);
    if(status < 0)
    {
        std::cerr << "ERROR while writing program to " << opts.outfile << std::endl;
        exit(-1);
    }

    if(opts.verbose)
    {
        std::cout << "\t Dumping assembled output ... " << std::endl;
        std::cout << "----------------" << std::endl;
        std::cout << " ADDR    OPCODE " << std::endl;
        std::cout << "----------------" << std::endl;
        Instr instr;
        for(unsigned i = 0; i < as_prog.numInstr(); ++i)
        {
            instr = as_prog.get(i); 
            std::cout << "[" << std::right << std::setw(4) << std::setfill('0') << std::hex << instr.adr << "]   ";
            std::cout << "$" << std::right << std::setw(4) << std::setfill('0') << std::hex << instr.ins << std::endl;
        }
    }

    return 0;
}
