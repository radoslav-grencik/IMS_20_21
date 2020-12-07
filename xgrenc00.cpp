#include "xgrenc00.hpp"

unsigned long long int totalTons = 0;
unsigned long long int reusedTons = 0;
unsigned long long int recycledTons = 0;

unsigned long long int wasteTons = 0;
unsigned long long int oceanWasteTons = 0;
unsigned long long int landWasteTons = 0;

unsigned long long int incineratedTons = 0;
unsigned long long int decomposedTons = 0;

unsigned long long int Rreused = 0;
unsigned long long int Rincinerated = 0;
unsigned long long int Rwasted = 0;

double delay;
double recChange = 0;
double recSucChange = 0;

double RR; // % of recyclated plastic
double RS; // success rate of recyclation
unsigned long long int Y; // number of simulated years
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

int ArgumentParser::PrintHelp()
{
    cout << "USAGE: ./xgrenc00 [-h]" << endl
         << endl
         << "OPTIONS:" << endl
         << "-h    Show this help." << endl;
    return EXIT_HELP;
}

int ArgumentParser::ParseOpt(int argc, char** argv)
{
    int opt;
    while (true) {
        opt = getopt(argc, argv, "hr:s:y:i:");
        if (opt == -1)
            break;

        switch (opt) {
        case 'h':
            return PrintHelp();
        case 'r':
            static bool opt_r = false;
            if (opt_r == false) {
                opt_r = true;
                recycleRate = strtol(optarg, NULL, 10);
                if (recycleRate > 62) // 30% of plastic will be used again and 8% will be incinerated
                    recycleRate = 62; // thus max percentage of recycled plastic is 62%
                if (recycleRate < 0)
                    recycleRate = 0;
            } else
                return Error(BAD_OPTIONS, "Option '-r' set more than once");
            break;
        case 's':
            static bool opt_s = false;
            if (opt_s == false) {
                opt_s = true;
                recycleSuccess = strtol(optarg, NULL, 10);
                if (recycleSuccess > 80) // 20% of plastic will be used as fuel => max recyclation success rate is 80%
                    recycleSuccess = 80;
                if (recycleSuccess < 0)
                    recycleSuccess = 0;
            } else
                return Error(BAD_OPTIONS, "Option '-s' set more than once");
            break;
        case 'y':
            static bool opt_y = false;
            if (opt_y == false) {
                opt_y = true;
                year = strtol(optarg, NULL, 10);
                if (year < 1)
                    year = 1;
            } else
                return Error(BAD_OPTIONS, "Option '-y' set more than once");
            break;
        case 'i':
            static bool opt_i = false;
            if (opt_i == false) {
                opt_i = true;
                yearIncrease = strtod(optarg, NULL);
            } else
                return Error(BAD_OPTIONS, "Option '-i' set more than once");
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

void Plastic::Behavior()
{
    double random = Random();
    bool wasted = false;
    totalTons++;
    if (random <= 0.3) { // plastic was reused
        reusedTons++;
    } else if ((random > 0.3) && (random <= (0.86 - recChange))) { // plastic waste end up in junkyard
        double wasteRandom = Random();
        if (wasteRandom <= 0.0451)
            oceanWasteTons++;
        else if ((random > 0.0451) && (random <= 1.0))
            landWasteTons++;
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
            double wasteRandom = Random();
            if (wasteRandom <= 0.0447)
                oceanWasteTons++;
            else if ((random > 0.0447) && (random <= 1.0))
                landWasteTons++;
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
            Wait(Uniform(5 * DAYS_IN_YEAR, 10 * DAYS_IN_YEAR));
        } else if ((decomposeRandom > 0.05) && (decomposeRandom <= 0.80)) { // PET bottles/PET bottle caps/plastic containers/lids
            Wait(Exponential(450 * DAYS_IN_YEAR));
        } else if ((decomposeRandom > 0.80) && (decomposeRandom <= 0.96)) { // plastic bags
            Wait(Exponential(20 * DAYS_IN_YEAR));
        } else if ((decomposeRandom > 0.96) && (decomposeRandom <= 0.996)) { // plastic food containers
            Wait(Exponential(50 * DAYS_IN_YEAR));
        } else if ((decomposeRandom > 0.996) && (decomposeRandom <= 1.0)) { // plastic straws
            Wait(Exponential(200 * DAYS_IN_YEAR));
        }
        decomposedTons++;
    }
}

void Production::Behavior()
{
    (new Plastic)->Activate();
    Activate(Time + delay);
}

void Experiment()
{
    double startingProduction = 359.0; // production of plastic in year 2018
    // Y - number of years to be simulated
    Init(0, DAYS_IN_YEAR * Y);
    delay = DAYS_IN_YEAR / startingProduction;
    (new Production)->Activate();
    Run();
    startingProduction *= yearlyIncrease; // yearly production increase

    double total = (double)totalTons / Y;
    double reused = (double)reusedTons / Y;
    double waste = (double)wasteTons / Y;
    double oceanWaste = (double)oceanWasteTons / Y;
    double landWaste = (double)landWasteTons / Y;
    double decomposed = (double)decomposedTons / Y;
    double incinerated = (double)incineratedTons / Y;
    double rec = (double)recycledTons / Y;
    double rr = (double)Rreused / Y;
    double ri = (double)Rincinerated / Y;
    double rw = (double)Rwasted / Y;

    double tmp1 = total;
    double tmp2 = reused;
    double tmp3 = waste;
    double tmp10 = oceanWaste;
    double tmp11 = landWaste;
    double tmp4 = decomposed;
    double tmp5 = incinerated;
    double tmp6 = rec;
    double tmp7 = rr;
    double tmp8 = ri;
    double tmp9 = rw;

    for (unsigned int i = 1; i < Y; i++) {
        tmp1 *= yearlyIncrease;
        tmp2 *= yearlyIncrease;
        tmp3 *= yearlyIncrease;
        tmp10 *= yearlyIncrease;
        tmp11 *= yearlyIncrease;
        tmp4 *= yearlyIncrease;
        tmp5 *= yearlyIncrease;
        tmp6 *= yearlyIncrease;
        tmp7 *= yearlyIncrease;
        tmp8 *= yearlyIncrease;
        tmp9 *= yearlyIncrease;

        total += tmp1;
        reused += tmp2;
        waste += tmp3;
        oceanWaste += tmp10;
        landWaste += tmp11;
        decomposed += tmp4;
        incinerated += tmp5;
        rec += tmp6;
        rr += tmp7;
        ri += tmp8;
        rw += tmp9;
    }

    long double totalWordWaste = 5000; // 6300 * 0.8
    waste -= decomposed;
    landWaste -= decomposed * landWaste / waste;
    oceanWaste -= decomposed * oceanWaste / waste;
    totalWordWaste += waste;
    if (totalWordWaste < 0)
        totalWordWaste = 0;

    cout << fixed << setprecision(2)
         << "========================================================" << endl
         << "== Recycling Rate:\t\t\t" << RR << "%" << endl
         << "== Recycling Succes:\t\t\t" << RS << "%" << endl
         << "== Years simulated:\t\t\t" << Y << endl
         << "== Yearly de/increase in production:\t" << ((yearlyIncrease - 1) * 100) << "%" << endl
         << "========================================================" << endl
         << "Total Mtons produced:\t";
    cout << setw(12) << total << "\t" << total / total * 100 << "%" << endl
         << "Reused Mtons:\t\t";
    cout << setw(12) << reused << "\t" << reused / total * 100 << "%" << endl
         << "Incinerated Mtons:\t";
    cout << setw(12) << incinerated << "\t" << incineratedTons / total * 100 << "%" << endl
         << "--------------------------------------------------------" << endl
         << "Recycled Mtons:\t\t";
    cout << setw(12) << rec << "\t(" << recycledTons / total * 100 << "%)\tof that:" << endl
         << "\t- Reused:\t";
    cout << setw(12) << rr << endl
         << "\t- Incinerated:\t";
    cout << setw(12) << ri << endl
         << "\t- Wasted:\t";
    cout << setw(12) << rw << endl
         << "--------------------------------------------------------" << endl
         << "Decomposed Mtons:\t";
    cout << setw(12) << decomposed << "\t" << decomposed / total * 100 << "%" << endl
         << "Waste Mtons:\t\t";
    cout << setw(12) << waste << "\t\tof that:" << endl
         << "\t- Land waste:\t";
    cout << setw(12) << landWaste << "\t" << landWaste / total * 100 << "%" << endl
         << "\t- Ocean waste:\t";
    cout << setw(12) << oceanWaste << "\t" << oceanWaste / total * 100 << "%" << endl
         << "Total world waste Mtons:";
    cout << setw(12) << totalWordWaste << endl
         << "========================================================" << endl;
}

int main(int argc, char** argv)
{
    ArgumentParser argumentParser;
    int return_code = argumentParser.ParseOpt(argc, argv); // parse the command line arguments (options)
    switch (return_code) {
    case EXIT_SUCCESS:
        break;
    case EXIT_HELP: // "-h" option has been used => return EXIT_SUCCESS
        return EXIT_SUCCESS;
    default:
        return return_code;
    }

    if (argumentParser.recycleRate == NOT_SET)
        RR = 6.0;
    else
        RR = (double)argumentParser.recycleRate;

    if (argumentParser.recycleSuccess == NOT_SET)
        RS = 20.0;
    else
        RS = (double)argumentParser.recycleSuccess;

    if (argumentParser.year == NOT_SET)
        Y = 10;
    else
        Y = argumentParser.year;

    double recyclationPercentage = RR / 100.0;
    recChange = recyclationPercentage - 0.06;

    double recyclationSuccessPercentage = RS / 100.0;
    recSucChange = recyclationSuccessPercentage - 0.2;

    if (argumentParser.yearIncrease != YEAR_INCREASE_NOT_SET)
        yearlyIncrease = 1.0 + (argumentParser.yearIncrease / 100);

    Experiment();

    return EXIT_SUCCESS;
}
