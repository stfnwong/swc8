/* 
 * C8DIS
 * Chip-8 disassembler.
 *
 * Stefan Wong 2018
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <getopt.h>
// Modules
#include "disassembler.hpp"
#include "source.hpp"

typedef struct 
{
    std::string infile;
    std::string outfile;
    bool verbose;
}  DisOpts;

void init_opts( DisOpts& opts)
{
    opts.infile  = "\0";
    opts.outfile = "\0";
    opts.verbose = false;
}

int main(int argc, char *argv[])
{
     DisOpts opts;
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


    // Get a lexer object 
    Disassembler dis;
    Program as_prog;

    dis.setVerbose(opts.verbose);
    status = dis.load(opts.infile);
    if(status < 0)
    {
        std::cerr << "ERROR unable to load file " << opts.infile << std::endl;
        exit(-1);
    }
    dis.disassemble();
    // Assemble the source
    SourceInfo dis_source = dis.getSourceInfo();
    //if(dis_source.hasError())
    //{
    //    std::cout << lex_source.dumpErrors();
    //    exit(-1);
    //}

    for(unsigned line = 0; line < dis_source.getNumLines(); ++line)
        std::cout << dis_source.getStr(line);

    if(opts.outfile != "\0")
    {
        std::ofstream ofile;
        try{
            ofile.open(opts.outfile);
        }
        catch(std::ios_base::failure& e) {
            std::cerr << e.what() << std::endl;
            exit(-1);
        }

        for(unsigned int line = 0; line < dis_source.getNumLines(); ++line)
            ofile << dis_source.getStr(line) << std::endl;

        ofile.close();
    }

    return 0;
}
