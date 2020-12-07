#include "xgrenc00.hpp"

int totalTons = 0;
int reusedTons = 0;
int recycledTons = 0;
int wasteTons = 0;
int incineratedTons = 0;
int decomposedTons = 0;
int Rreused = 0;
int Rincinerated = 0;
int Rwasted = 0;
double delay;
double recChange = 0;
double recSucChange = 0;
double RR; // % of recyclated plastic
double RS; // success rate of recyclation
int Y; // number of simulated years
double yearlyIncrease = 1.0;

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

class ArgumentParser {
public:
    long int year = -1;
    long int recycleRate = -1;
    long int recycleSuccess = -1;
    double yearIncrease = YEARINCREASENOTSET;

    int PrintHelp()
    {
        cout << "USAGE: ./xgrenc00 [-h]" << endl
             << endl
             << "OPTIONS:" << endl
             << "-h    Show this help." << endl;
        return EXIT_HELP;
    }

    int ParseOpt(int argc, char** argv)
    {
        int opt;
        while (true) {
            opt = getopt(argc, argv, "r:s:y:i:h");
            if (opt == -1)
                break;

            switch (opt) {
            case 'h':
                return PrintHelp();
            case 'r':
                recycleRate = strtol(optarg, NULL, 10);
                if (recycleRate > 62) // 30% of plastic will be used again and 8% will be incinerated
                    recycleRate = 62; // thus max percentage of recycled plastic is 62%
                break;
            case 's':
                recycleSuccess = strtol(optarg, NULL, 10);
                if (recycleSuccess > 80) // 20% of plastic will be used as fuel => max recyclation success rate is 80%
                    recycleSuccess = 80;
                break;
            case 'y':
                year = strtol(optarg, NULL, 10);
                break;
            case 'i':
                yearIncrease = strtod(optarg, NULL);
                break;
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

        return EXIT_SUCCESS;
    }
};

class Plastic : public Process {
    void Behavior()
    {
        double random = Random();
        bool wasted = false;
        totalTons++;
        if (random <= 0.3) { // plastic was reused
            reusedTons++;
        } else if ((random > 0.3) && (random <= (0.86 - recChange))) { // plastic waste end up in junkyard
            wasteTons++;
            wasted = true;
        } else if ((random > (0.86 - recChange)) && (random <= 0.92)) { // plastic was recycled
            recycledTons++;
            double recycledRandom = Random();
            if (recycledRandom <= 0.2) { // plastic waste was incinerated
                incineratedTons++;
                Rincinerated++;
            } else if ((recycledRandom > 0.2) && (recycledRandom <= (0.4 + recSucChange))) { // plastic was reused
                reusedTons++;
                Rreused++;
            } else if ((recycledRandom > (0.4 + recSucChange)) && (recycledRandom <= 1.0)) { // plastic waste end up in junkyard
                wasteTons++;
                Rwasted++;
                wasted = true;
            }
        } else if ((random > 0.92) && (random <= 1.0)) { // plastic waste was incinerated
            incineratedTons++;
        }
        if (wasted) {
            double decomposeRandom = Random();
            if (decomposeRandom <= 0.05) { // cigarette filters/small plastic waste
                Wait(Uniform(5 * DAYSINYEAR, 10 * DAYSINYEAR));
            } else if ((decomposeRandom > 0.05) && (decomposeRandom <= 0.80)) { // PET bottles/PET bottle caps/plastic containers/lids
                Wait(Exponential(450 * DAYSINYEAR));
            } else if ((decomposeRandom > 0.80) && (decomposeRandom <= 0.96)) { // plastic bags
                Wait(Exponential(20 * DAYSINYEAR));
            } else if ((decomposeRandom > 0.96) && (decomposeRandom <= 0.996)) { // plastic food containers
                Wait(Exponential(50 * DAYSINYEAR));
            } else if ((decomposeRandom > 0.996) && (decomposeRandom <= 1.0)) { // plastic straws
                Wait(Exponential(200 * DAYSINYEAR));
            }
            decomposedTons++;
        }
    }
};

class Production : public Event {
public:
    void Behavior()
    {
        (new Plastic)->Activate();
        Activate(Time + delay);
    }
};

void Experiment()
{
    double startingProduction = 359.0; // production of plastic in year 2018
    // Y - number of years to be simulated
    Init(0, DAYSINYEAR * Y);
    delay = DAYSINYEAR / startingProduction;
    (new Production)->Activate();
    Run();
    startingProduction *= yearlyIncrease; // yearly production increase

    double total = (double)totalTons / Y;
    double reused = (double)reusedTons / Y;
    double waste = (double)wasteTons / Y;
    double decomposed = (double)decomposedTons / Y;
    double incinerated = (double)incineratedTons / Y;
    double rec = (double)recycledTons / Y;
    double rr = (double)Rreused / Y;
    double ri = (double)Rincinerated / Y;
    double rw = (double)Rwasted / Y;
    double tmp1 = total;
    double tmp2 = reused;
    double tmp3 = waste;
    double tmp4 = decomposed;
    double tmp5 = incinerated;
    double tmp6 = rec;
    double tmp7 = rr;
    double tmp8 = ri;
    double tmp9 = rw;
    for (int i = 1; i < Y; i++) {
        tmp1 *= yearlyIncrease;
        tmp2 *= yearlyIncrease;
        tmp3 *= yearlyIncrease;
        tmp4 *= yearlyIncrease;
        tmp5 *= yearlyIncrease;
        tmp6 *= yearlyIncrease;
        tmp7 *= yearlyIncrease;
        tmp8 *= yearlyIncrease;
        tmp9 *= yearlyIncrease;

        total += tmp1;
        reused += tmp2;
        waste += tmp3;
        decomposed += tmp4;
        incinerated += tmp5;
        rec += tmp6;
        rr += tmp7;
        ri += tmp8;
        rw += tmp9;
    }

    int totalWordWaste = 5000; // 6300 * 0.8
    cout << "===================================================" << endl;
    cout << "== Recycling Rate :\t\t" << RR << "%" << endl;
    cout << "== Recycling Succes :\t\t" << RS << "%" << endl;
    cout << "== Years simulated :\t\t" << Y << endl;
    cout << "== Yearly de/increase in production :\t" << ((yearlyIncrease - 1) * 100) << "%" << endl;
    cout << "===================================================" << endl;

    cout << "Total Mtons produced:\t" << (int)total << "\t" << (double)(total / ((double)total / 100)) << "%" << endl;
    cout << "Reused Mtons\t\t" << (int)reused << "\t" << (double)(reused / ((double)total / 100)) << "%" << endl;
    cout << "Waste Mtons:\t\t" << (int)waste << "\t" << (double)(waste / ((double)total / 100)) << "%"
         << "\tof that:" << endl;
    cout << "\tDecomposed:\t" << (int)decomposed << "\t" << endl;

    totalWordWaste += waste;
    totalWordWaste -= decomposed;
    cout << "Incinerated Mtons:\t" << (int)incinerated << "\t" << (double)(incineratedTons / ((double)totalTons / 100)) << "%" << endl;
    cout << "Recycled Mtons:\t\t" << (int)rec << "\t(" << (double)(recycledTons / ((double)totalTons / 100)) << "%)"
         << "\tof that:" << endl;
    cout << "\tReused :\t" << (int)rr << "\t" << endl;
    cout << "\tIncinerated :\t" << (int)ri << "\t" << endl;
    cout << "\tWasted :\t" << (int)rw << "\t" << endl;
    cout << "Total world waste\t" << totalWordWaste << " milion tons\t" << endl;
    cout << "===================================================" << endl;
}

int main(int argc, char** argv)
{
    ArgumentParser argumentParser;
    int return_code;
    return_code = argumentParser.ParseOpt(argc, argv); // parse the command line arguments (options)
    switch (return_code) {
    case EXIT_SUCCESS:
        break;
    case EXIT_HELP: // "-h" option has been used => return EXIT_SUCCESS
        return EXIT_SUCCESS;
    default:
        return return_code;
    }

    if (argumentParser.recycleRate == -1)
        RR = 6.0;
    else
        RR = (double)argumentParser.recycleRate;

    if (argumentParser.recycleSuccess == -1)
        RS = 20.0;
    else
        RS = (double)argumentParser.recycleSuccess;

    if (argumentParser.year == -1)
        Y = 10;
    else
        Y = argumentParser.year;

    double recyclationPercentage = RR / 100.0;
    recChange = recyclationPercentage - 0.06;

    double recyclationSuccessPercentage = RS / 100.0;
    recSucChange = recyclationSuccessPercentage - 0.2;

    if (argumentParser.yearIncrease != YEARINCREASENOTSET)
        yearlyIncrease = 1.0 + (argumentParser.yearIncrease / 100);

    Experiment();

    return EXIT_SUCCESS;
}
