#include "data_analysis.cpp"

int main()
{

    if(readConfig() == 1) {
        return 1;
    }
    cout << "Configuration file successfully read!" << endl;

    if(mode == "finger") {
        cout << "Processing finger data..." << endl;
        if(finger() == 1) {
            return 1;
        }
    } else if (mode == "quad") {
        cout << "Processing quad data..." << endl;
        if(quad() == 1) {
            return 1;
        }
    } else if (mode == "instrument") {
        cout << "Processing instrument data..." << endl;
        if(instrument() == 1) {
            return 1;
        }
    } else {
        cerr << "Wrong mode selected in config file!" << endl;
        return 1;
    }

    return 0;
}