#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <string.h>
#include <cmath>
#include <cerrno>
#include <cstring>
#include <filesystem>

#define filename "dados.evta"

namespace fs = std::filesystem;
using namespace std;

string mode;
string inputFileName;

double bottomLeftX;
double bottomLeftY;
double height;
bool rotated;
bool inverted;

double bottomLeft1X;
double bottomLeft1Y;
double height1;
bool rotated1;
bool inverted1;

double bottomLeft2X;
double bottomLeft2Y;
double height2;
bool rotated2;
bool inverted2;

double bottomLeft3X;
double bottomLeft3Y;
double height3;
bool rotated3;
bool inverted3;

double sizePixel;
double detectorSizeX;
double detectorSizeY;
string energy;

double spacing_1_2;
double spacing_2_3;
double spacing_3_4;

// ------------------------------ AUXILIARY FUNCTIONS

// trim string from start
inline void ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
}

// trim string from end
inline void rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
}

// Converts energy values from ToT to KeV, usgin the ABCT files
void convert_energy(vector<vector<string>>& data) {
    cout << "Converting energy from ToT to KeV..." << endl;

    // read abct files
    vector<vector<double>> a_list, b_list, c_list, t_list;
    float a, b, c, t;
    ifstream fileA("ABCT_Files/Files_a.txt");
    string s;
    vector<double> temp;

    while(getline(fileA, s)) {
        stringstream ss(s);
        string token;
        while(getline(ss, token, ' ')) {
            temp.push_back(stof(token));
        }

        a_list.push_back(temp);
        temp.clear();
    }
    fileA.close();
    s = "";

    ifstream fileB("ABCT_Files/Files_b.txt");
    while(getline(fileB, s)) {
        stringstream ss(s);
        string token;
        while(getline(ss, token, ' ')) {
            temp.push_back(stof(token));
        }

        b_list.push_back(temp);
        temp.clear();
    }
    fileB.close();
    s = "";

    ifstream fileC("ABCT_Files/Files_c.txt");
    while(getline(fileC, s)) {
        stringstream ss(s);
        string token;
        while(getline(ss, token, ' ')) {
            temp.push_back(stof(token));
        }

        c_list.push_back(temp);
        temp.clear();
    }
    fileC.close();
    s = "";

    ifstream fileT("ABCT_Files/Files_t.txt");
    while(getline(fileT, s)) {
        stringstream ss(s);
        string token;
        while(getline(ss, token, ' ')) {
            temp.push_back(stof(token));
        }

        t_list.push_back(temp);
        temp.clear();
    }
    fileT.close();
    s = "";

    // replace ToT with Energy in KeV
    for(int i = 0; i < data.size(); i++){ 
        if(i == 0) {
            continue;
        }

        a = a_list[stoi(data[i][1])/256][stoi(data[i][1]) % 256];
        b = b_list[stoi(data[i][1])/256][stoi(data[i][1]) % 256];
        c = c_list[stoi(data[i][1])/256][stoi(data[i][1]) % 256];
        t = t_list[stoi(data[i][1])/256][stoi(data[i][1]) % 256];

        float e = ((t*a - b + stoi(data[i][3]))/(2*a)) + sqrt(pow((t*a - b + stoi(data[i][3]))/(2*a), 2) - (t * (stoi(data[i][3]) - b) - c)/a);   

        data[i][3] = to_string(e);
    }

    cout << "Conversion finished!" << endl;

}

// Reads the "config.txt" file, assigning the input values onte the respective variables
int readConfig(){
    cout << "Reading configuration file..." << endl;

    fstream configFile("config.txt");
    vector<vector<string>> configValues;
    if (configFile.is_open()){
        string s;
        while (getline(configFile, s)) {
            vector<string> line;
            stringstream ss(s);
            string token;
            while(getline(ss, token, ' ')) {
                ltrim(token);
                rtrim(token);

                line.push_back(token);
            }

            configValues.push_back(line);
            line.clear();
        }

        configFile.close();
    } else {
        cerr << "Unable to open config file: " << strerror(errno) << endl;
        return 1;
    }


    mode = configValues[0][1];

    if(mode == "finger") {
        bottomLeftX = stod(configValues[1][1]);
        bottomLeftY = stod(configValues[1][2]);
        height = stod(configValues[2][1]);
        sizePixel = stod(configValues[3][1]);
        detectorSizeX = stoi(configValues[4][1]);
        detectorSizeY = stoi(configValues[4][2]);
        if(configValues[5][1] == "kev" || configValues[5][1] == "tot") {
            energy = configValues[5][1];
        } else {
            cerr << "Wrong energy value in config file! Must be either \"kev\" or \"tot\"!" << endl;
            return 1;
        }
        inputFileName = configValues[6][1];

    } else if (mode == "quad") {
        bottomLeftX = stod(configValues[1][1]);
        bottomLeftY = stod(configValues[1][2]);
        height = stod(configValues[2][1]);
        spacing_1_2 = stod(configValues[3][1]);
        spacing_2_3 = stod(configValues[3][2]);
        spacing_3_4 = stod(configValues[3][3]);
        sizePixel = stod(configValues[4][1]);
        detectorSizeX = stod(configValues[5][1]);
        detectorSizeY = stod(configValues[5][2]);
        if(configValues[6][1] == "kev" || configValues[6][1] == "tot") {
            energy = configValues[6][1];
        } else {
            cerr << "Wrong energy value in config file! Must be either \"kev\" or \"tot\"!" << endl;
            return 1;
        }
        inputFileName = configValues[7][1];

    } else if (mode == "instrument") {
        bottomLeftX = stod(configValues[1][1]);
        bottomLeftY = stod(configValues[1][2]);
        height = stod(configValues[5][1]);
        if(stoi(configValues[6][1]) == 1) {
            rotated = true;
        } else { 
            rotated = false; 
        }
        if(stoi(configValues[7][1]) == 1) {
            inverted = true;
        } else {
            inverted = false;
        }

        bottomLeft1X = stod(configValues[2][1]);
        bottomLeft1Y = stod(configValues[2][2]);
        height1 = stod(configValues[5][2]);
        if (stoi(configValues[6][2]) == 1) {
            rotated1 = true;
        } else {
            rotated1 = false;
        }
        if (stoi(configValues[7][2]) == 1) {
            inverted1 = true;
        } else {
            inverted1 = false;
        }

        bottomLeft2X = stod(configValues[3][1]);
        bottomLeft2Y = stod(configValues[3][2]);
        height2 = stod(configValues[5][3]);
        if(stoi(configValues[6][3]) == 1) {
            rotated2 = true;
        } else {
            rotated2 = false;
        }
        if(stoi(configValues[7][3]) == 1) {
            inverted2 = true;
        } else {
            inverted2 = false;
        }

        bottomLeft3X = stod(configValues[4][1]);
        bottomLeft3Y = stod(configValues[4][2]);
        height3 = stod(configValues[5][4]);
        if(stoi(configValues[6][4]) == 1) {
            rotated3 = true;
        } else {
            rotated3 = false;
        }
        if(stoi(configValues[7][4]) == 1) {
            inverted3 == true;
        } else {
            inverted3 == false;
        }

        spacing_1_2 = stod(configValues[8][1]);
        spacing_2_3 = stod(configValues[8][2]);
        spacing_3_4 = stod(configValues[8][3]);

        sizePixel = stod(configValues[9][1]);
        detectorSizeX = stod(configValues[10][1]);
        detectorSizeY = stod(configValues[10][2]);
        if(configValues[11][1] == "kev" || configValues[11][1] == "tot") {
            energy = configValues[11][1];
        } else {
            cerr << "Wrong energy value in config file! Must be either \"kev\" or \"tot\"!" << endl;
            return 1;
        }
        inputFileName = configValues[12][1];

    } else {
        cerr << "Wrong mode selected in config file!" << endl;
        return 1;
    }

    for(int i = 0; i < configValues.size(); i++) {
        for(int j = 0; j < configValues[i].size(); j++) {
            cout << configValues[i][j] << " ";
        }
        cout << endl;
    }    

    return 0;
}

// -------------------------- FINGER 

// Outputs the data onto the output file, using the correct .evta format
void create_output_file_finger(vector<vector<string>> data) {
    cout << "Writing data to output file..." << endl;

    ofstream outFile(filename);

    if (outFile.is_open()) {
        outFile << "Version 200\n";
        outFile << "Type EVTA\n\n";
        
        int cont = 1;
        for(int i = 0; i < data.size(); i++) {
            if(i == 0 || (stoi(data[i][1]) % 256) == 0 || (stoi(data[i][1]) % 256) == 255 || (stoi(data[i][1])/256) == 0 || (stoi(data[i][1])/256) == 255){
                continue;
            }

            if(stoi(data[i][1]) == 50904) {
                continue;
            }

            outFile << "SE\n";
            outFile << "ID " << cont << "\n";

            double time = (stod(data[i][2])*25 - stod(data[i][4])*1.5625) * 1e-9;
            outFile << "TI " << std::setprecision(9) << time << "\n";
            outFile << "HT 8;" << (stoi(data[i][1]) % 256)*sizePixel + bottomLeftX << ";" << (stoi(data[i][1])/256)*sizePixel + bottomLeftY << ";" << height << ";" << stof(data[i][3]) << ";" << sizePixel/2 << ";" << sizePixel/2 << ";0;0.5\n"; //HT detectorID;x;y;z;energy;x_uncertainty;y_uncertainty;z_uncertainty;energy_uncertainty --- shift of half a pixel to the right

            cont++;
        }

        outFile << "EN";

        // Close the file
        outFile.close();
        cout << "Data has been written!\n";
    } else {
        cerr << "Error: Unable to write into output file.\n";
    }
}

// Reads the values from the input file, calls convertion function (if necessary) and output function
int finger() {
    fstream MyReadFile(inputFileName);

    vector<vector<string>> values; 
    if(MyReadFile.is_open()) {
        string s;
        while (getline (MyReadFile, s)) { 
            // Output the text from the file

            vector<string> t;
            stringstream ss(s);
            string token;
            while(getline(ss, token, '\t')) {
                ltrim(token);
                rtrim(token);

                t.push_back(token);
            }

            values.push_back(t);
            t.clear();

        }

        MyReadFile.close();
    } else {
        cerr << "Unable to open file: " << strerror(errno) << endl;
        return 1;
    }
    
    if (energy == "tot") { 
        convert_energy(values); // if energy is in ToT instead of KeV
    }

    create_output_file_finger(values);

    return 0;
}


// --------------------------------- QUAD

// Writes the header on the ouput file
void output_beginning(){
    ofstream outFile(filename);

    if(outFile.is_open()) {
        outFile << "Version 200\n";
        outFile << "Type EVTA\n\n";

        outFile.close();
        cout << "The headers have been written to the file.\n";
    } else {
        cerr << "Error: Unable to write headers into output file.\n";
    }
}

//Writes the content body on the output file
int output_content(vector<vector<string>> data, int detectorId, int eventId, double bLeftX, double bLeftY, double z, bool rot, bool inv){
    float offset; //detextorSizeX -> detector length
    if(detectorId % 4 == 0) {
        offset = 0; 
    } else if (detectorId % 4 == 1) {
        offset = detectorSizeX + spacing_1_2; //0.1 -> 1mm between 1st and 2nd detectors
    } else if (detectorId % 4 == 2) {
        offset = 2*detectorSizeX + spacing_1_2 + spacing_2_3; //0.5 -> distance between 2nd and 3rd detector
    } else {
        offset = 3*detectorSizeX + spacing_1_2 + spacing_2_3 + spacing_3_4; //0.1 -> 1mm between 3rd and 4th detectors
    }

    ofstream outFile;
    outFile.open(filename, ios_base::app);

    if(outFile.is_open()){
        for(int i = 0; i < data.size(); i++) {
            if(i == 0 || (stoi(data[i][1]) % 256) == 0 || (stoi(data[i][1]) % 256) == 255 || (stoi(data[i][1])/256) == 0 || (stoi(data[i][1])/256) == 255){
                continue;
            }

            if(stoi(data[i][1]) == 50904) {
                continue;
            }

            outFile << "SE\n";
            outFile << "ID " << eventId << "\n";
            eventId++;

            double time = (stod(data[i][2])*25 - stod(data[i][4])*1.5625) * 1e-9;
            outFile << "TI " << std::setprecision(9) << time << "\n";

            if(rot == false || inv == false) {
                outFile << "HT 8;" << ((stoi(data[i][1]) % 256))*sizePixel + bLeftX + offset << ";" << ((stoi(data[i][1])/256))*sizePixel + bLeftY << ";" << z << ";" << stoi(data[i][3]) << ";" << sizePixel/2 << ";" << sizePixel/2 << ";" << 0 << ";" << 0.5 << "\n"; //HT detectorID;x;y;z;energy;x_uncertainty;y_uncertainty;z_uncertainty;energy_uncertainty --- shift of half a pixel to the right
            } else {
                outFile << "HT 8;" << 1.408 - (((stoi(data[i][1]) % 256))*sizePixel) + bLeftX + offset << ";" << 1.408 - (((stoi(data[i][1])/256))*sizePixel )+ bLeftY << ";" << z << ";" << stoi(data[i][3]) << ";" << sizePixel/2 << ";" << sizePixel/2 << ";" << 0 << ";" << 0.5 << "\n"; //HT detectorID;x;y;z;energy;x_uncertainty;y_uncertainty;z_uncertainty;energy_uncertainty --- shift of half a pixel to the right
            }

        }

        cout << "File number " << detectorId+1 << " processed." << endl;
        outFile.close();
    } else {
        cerr << "Error: Unable to write content into output file.\n";
    }

    outFile.close();

    return eventId;
}

// Writes the end/footer of the output file
void output_end(){
    ofstream outFile;
    outFile.open(filename, ios_base::app);

    if(outFile.is_open()){
        outFile << "EN";
        outFile.close();
        cout << "Footer has been written to the file.\n";
    } else {
        cerr << "Error: Unable to write footer into output file.\n";
    }
}

// Reads input files (must be in alphabetical order), converts energy (if necessary) and calls output functions
int quad(){
    output_beginning();

    //get files from folder
    vector<string> files;
    for (const auto &entry : fs::directory_iterator(inputFileName)) { 
        if (fs::is_regular_file(entry.path())) {
            files.push_back(entry.path().string());
        }
    }

    sort(files.begin(), files.end());

    vector<vector<string>> values;
    string s;

    int eventId = 1;

    for(int i = 0; i < files.size(); i++) {
        s = "";
        fstream MyReadFile(files[i]);

        while (getline(MyReadFile, s)) {
            vector<string> t;
            stringstream ss(s);
            string token;
            while(getline(ss, token, '\t')) {
                ltrim(token);
                rtrim(token);

                t.push_back(token);
            }

            values.push_back(t);
            t.clear();
        }

        if (energy == "tot") {
            convert_energy(values);
        }

        eventId = output_content(values, i, eventId, bottomLeftX, bottomLeftY, height, false, false);
        values.clear();
    }

    output_end();

    return 0;
}


// ------------------------------- INSTRUMENT

// Reads data files (in aphabetical order), converts the energy if necessary and calls output functions
int instrument(){
    output_beginning();

    //get files from folder
    vector<string> files;
    for (const auto &entry : fs::directory_iterator(inputFileName)) { 
        if (fs::is_regular_file(entry.path())) {
            files.push_back(entry.path().string());
        }
    }
    sort(files.begin(), files.end());


    int eventId = 1;
    string s = "";
    for (int i = 0; i < files.size(); i++){
        cout << "Current file: " << files[i] << endl;


        fstream MyReadFile(files[i]);
        vector<vector<string>> values;
        s = "";
        while (getline(MyReadFile, s)) {
            vector<string> t;
            stringstream ss(s);
            string token;
            while(getline(ss, token, '\t')) {
                ltrim(token);
                rtrim(token);

                t.push_back(token);
            }

            values.push_back(t);
            t.clear();
        }

        if (energy == "tot") {
            convert_energy(values);
        }   

        if(i < 4) { //first (uppermost) quad
            eventId = output_content(values, i, eventId, bottomLeftX, bottomLeftY, height, rotated, inverted);
        } else if (i >= 4 && i < 8) { //second
            eventId = output_content(values, i, eventId, bottomLeft1X, bottomLeft1Y, height1, rotated1, inverted1);
        } else if (i >= 8 && i < 12) { //third
            eventId = output_content(values, i, eventId, bottomLeft2X, bottomLeft2Y, height2, rotated2, inverted2);
        } else { // >= 12, fourth
            eventId = output_content(values, i, eventId, bottomLeft3X, bottomLeft3Y, height3, rotated3, inverted3);
        }
        values.clear();

    }

    output_end();

    return 0;
}

int main()
{

    if(readConfig() == 1) {
        return 1;
    }
    cout << "Configuration file successfully read!";

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

