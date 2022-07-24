#include "main.h"

int main(int argc, char *argv[])
{
    options_t options_old = args_parser(argc, argv);
    options_t options = (options_old.benchmark == 0) ? run(options_old) : benchmark(options_old);
    options.outputfile = (options.outputfile == NULL) ? filename_generator(options) : options.outputfile;
    write_file(options.outputfile, options.output_content, options.output_length);
    printf("EVERYTHING OK\n");
    return 0;
}
