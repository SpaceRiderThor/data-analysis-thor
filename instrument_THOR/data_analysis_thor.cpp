#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <cmath>
#include <cstring>

#define filename "dados.evta"

namespace fs = std::filesystem;
using namespace std;

/////////////////////////////////////////////////////////////////////////

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

//TODO func que recebe os valores de cada ficheiro e adiciona ao output
int output_content(vector<vector<string>> data, int detectorId, int eventId, float z){
    float offset; //1.408 -> detector length
    if(detectorId % 4 == 0) {
        offset = 0;
    } else if (detectorId % 4 == 1) {
        offset = 1.408 + 0.1; //0.1 -> 1mm between 1st and 2nd detector
    } else if (detectorId % 4 == 2){
        offset = 2*1.408 + 0.1 + 0.5; //0.5 -> 5mm between 2nd and 3rd detector
    } else {
        offset = 3*1.408 + 0.1 + 0.5 + 0.1; //1mm -> distance between 3rd and 4th detector
    }

    ofstream outFile;
    outFile.open(filename, ios_base::app);

    if(outFile.is_open()){
        for(int i = 0; i < data.size(); i++) {
            if(i == 0 || (stoi(data[i][1]) % 256) == 0 || (stoi(data[i][1]) % 256) == 255 || (stoi(data[i][1])/256) == 0 || (stoi(data[i][1])/256) == 255){
                continue;
            }

            outFile << "SE\n";
            outFile << "ID " << eventId << "\n";
            eventId++;

            double time = (stod(data[i][2])*25 - stod(data[i][4])*1.5625) * 1e-9;
            outFile << "TI " << time << "\n";
            outFile << "HT 8;" << (stoi(data[i][1]) % 256)*0.0055 + offset << ";" << (stoi(data[i][1])/256)*0.0055 + 1.546 << ";" << z << ";" << stoi(data[i][3]) << ";" << 0.00275 << ";" << 0.00275 << ";" << 0.00275 << ";" << 0.5 << ";\n"; //HT detectorID;x;y;z;energy;x_uncertainty;y_uncertainty;z_uncertainty;energy_uncertainty --- shift of half a pixel to the right

        }

        cout << "The content has been written to the file.\n";
        outFile.close();
    } else {
        cerr << "Error: Unable to write content into output file.\n";
    }

    outFile.close();

    //cout << "EVENTID: " << eventId << endl;
    return eventId;
}


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

void convert_energy(vector<vector<string>>& data) {
    // read abct files
    vector<vector<double>> a_list, b_list, c_list, t_list;
    float a, b, c, t;
    ifstream fileA("ABCT_Files/Files_a.txt");// , fileC("ABCT_Files/Files_c.txt"), fileT("ABCT_Files/Files_t.txt");
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

    //cout << setprecision(7) << a_list[10][11]; //[y][x]

    // replace ToT with Energy in KeV
    for(int i = 0; i < data.size(); i++){ //data.size()
        if(i == 0) {
            continue;
        }

        a = a_list[stoi(data[i][1])/256][stoi(data[i][1]) % 256];
        b = b_list[stoi(data[i][1])/256][stoi(data[i][1]) % 256];
        c = c_list[stoi(data[i][1])/256][stoi(data[i][1]) % 256];
        t = t_list[stoi(data[i][1])/256][stoi(data[i][1]) % 256];

        //cout << "a: " << a << " b: " << b << " c: " << c << " t: " << t << endl;

        float e = ((t*a - b + stoi(data[i][3]))/(2*a)) + sqrt(pow((t*a - b + stoi(data[i][3]))/(2*a), 2) - (t * (stoi(data[i][3]) - b) - c)/a);
                    
        //cout << "Index: " << data[i][0] << setprecision(7) << " energy: " << e << endl;

        data[i][3] = to_string(e);
    }

}

int main(int argc, char* argv[]) {

    output_beginning();

    vector<ifstream> files;
    /*string path = "full_detector_sim";
    for (const auto &entry : fs::directory_iterator(path)) {
        if (fs::is_regular_file(entry.path())) {
            cout << entry.path() << endl;
            ifstream file(entry.path());
            files.push_back(move(file));
        }
    }*/

    for(int i = 0; i < 16; i++) {
        ifstream file("Ba133_colimated1cm-calibrated.t3pa");
        files.push_back(move(file));
    }

    int eventId = 1;
    string s = "";
    for (int i = 0; i < files.size(); i++){
        //cout << files[i] << endl;

        vector<vector<string>> values;
        s = "";
        while (getline(files[i], s)) {
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

        if (argc == 2 && strcmp(argv[1], "-c") == 0) {
            convert_energy(values);
        }   

        if(i < 4) { //first (uppermost) quad
            eventId = output_content(values, i, eventId, 2.76);
        } else if (i >= 4 && i < 8) { //second
            eventId = output_content(values, i, eventId, 2.38);
        } else if (i >= 8 && i < 12) { //third
            eventId = output_content(values, i, eventId, 0.38);
        } else { // >= 12, fourth
            eventId = output_content(values, i, eventId, 0);
        }
        values.clear();

        cout << "File " << i << " done." << endl;
    }

    output_end();
    

    return 0;
}