#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <string.h>
#include <cmath>
#include <cerrno>
#include <cstring>

using namespace std;

string inputFileName;
double bottomLeftX;
double bottomLeftY;
double height;
double sizePixel;
int numberPixelsX;
int numberPixelsY;
string energy;

// trim from start
inline void ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
}

// trim from end
inline void rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
}

void create_output_file(vector<vector<string>> data) {
    cout << "Writing data to output file..." << endl;

    ofstream outFile("dados.evta");

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
            outFile << "HT 8;" << (stoi(data[i][1]) % 256)*sizePixel + bottomLeftX << ";" << (stoi(data[i][1])/256)*sizePixel + bottomLeftY << ";" << height << ";" << stof(data[i][3]) << ";0.00275;0.00275;0;5\n"; //HT detectorID;x;y;z;energy;x_uncertainty;y_uncertainty;z_uncertainty;energy_uncertainty --- shift of half a pixel to the right

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

int readConfig(){
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

    /*for (int i = 0; i < configValues.size(); i++) {
        for (int j = 0; j < configValues[i].size(); j++){
            cout << configValues[i][j] << " ";
        }
        cout << endl;
    }*/

    bottomLeftX = stod(configValues[0][1]);
    bottomLeftY = stod(configValues[0][2]);
    height = stod(configValues[1][1]);
    sizePixel = stod(configValues[2][1]);
    numberPixelsX = stoi(configValues[3][1]);
    numberPixelsY = stoi(configValues[3][2]);
    if(configValues[4][1] == "kev" || configValues[4][1] == "tot") {
        energy = configValues[4][1];
    } else {
        cerr << "Wrong energy value in config file! Must be either \"kev\" or \"tot\"!" << endl;
        return 1;
    }
    inputFileName = configValues[5][1];

    return 0;
}

int main()
{
    //fstream MyReadFile("Ba133_3600s_cut.t3pa");
    //fstream MyReadFile("Ba133_3600s.t3pa");
    //fstream MyReadFile("Ba133_colimated1cm-calibrated.t3pa");
    //fstream MyReadFile("Larix-measurement24_betatron_on_with_polarizer_60s_1Mev_13-03-merged.t3pa");


    if(readConfig() == 1) {
        return 1;
    }

    cout << bottomLeftX << " " << bottomLeftY << endl;
    cout << height << endl;
    cout << sizePixel << endl;
    cout << numberPixelsX << " " << numberPixelsY << endl;
    cout << energy << endl;
    cout << inputFileName << endl;

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
        return 0;
    }
    
    if (energy == "tot") { 
        convert_energy(values); // if energy is in ToT instead of KeV
    }

    create_output_file(values);

    return 0;
}

