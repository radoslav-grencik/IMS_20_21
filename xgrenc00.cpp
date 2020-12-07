#include "xgrenc00.hpp"

int Error(int errnum, string err)
{
    if (err.size())
        cerr << "ERROR: " << err << "." << endl;
    switch (errnum) {
    case BAD_OPTIONS:
        cerr << "Try './xgrenc00 -h' for more information." << endl;
        break;
    }
    return (errnum);
}

int ParseOpt(int argc, char** argv)
{
    int opt;
    while (true) {
        opt = getopt(argc, argv, "h");
        if (opt == -1)
            break;

        switch (opt) {
        case 'h':
            return PrintHelp();
        case '?':
            return Error(BAD_OPTIONS, "");
        }
    }

    if (optind < argc) {
        cerr << "Non-option arguments detected: ";
        while (optind < argc)
            cerr << argv[optind++] << " ";
        cerr << endl;
        return Error(BAD_OPTIONS, "");
    }

    return PrintHelp();
}

int PrintHelp()
{
    cout << "USAGE: ./xgrenc00 [-h]" << endl
         << endl
         << "OPTIONS:" << endl
         << "-h    Show this help." << endl;
    return EXIT_HELP;
}

int main(int argc, char** argv)
{
    int return_code;
    return_code = ParseOpt(argc, argv); // parse the command line arguments (options)
    switch (return_code) {
    case EXIT_SUCCESS:
        break;
    case EXIT_HELP: // "-h" option has been used => return EXIT_SUCCESS
        return EXIT_SUCCESS;
    default:
        return return_code;
    }

    return EXIT_SUCCESS;
}
