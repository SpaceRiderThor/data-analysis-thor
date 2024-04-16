#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <string.h>
#include <cmath>

using namespace std;


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
    ofstream outFile("dados.evta");

    //outFile.setf(ios::fixed);
    //outFile.precision(9);

    if (outFile.is_open()) {
        outFile << "Version 200\n";
        outFile << "Type EVTA\n\n";
        
        //cout << data.size();    

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
            outFile << "HT 8;" << (stoi(data[i][1]) % 256)*0.0055<< ";" << (stoi(data[i][1])/256)*0.0055 << ";" << 0 << ";" << stof(data[i][3]) << ";0.00275;0.00275;0;5\n"; //HT detectorID;x;y;z;energy;x_uncertainty;y_uncertainty;z_uncertainty;energy_uncertainty --- shift of half a pixel to the right

            cont++;
        }

        outFile << "EN";

        // Close the file
        outFile.close();
        cout << "Data has been written to output the file.\n";
    } else {
        cerr << "Error: Unable to write into output file.\n";
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

int main(int argc, char* argv[])
{
    fstream MyReadFile("Ba133_colimated1cm-calibrated.t3pa");
    //ifstream MyReadFile("Am241-gammaAlphas/Am241-test1-1.t3pa");
    //ifstream MyReadFile("Ba133-pointsource/Ba133-point-source-1.t3pa");
    //ifstream MyReadFile("Larix-measurement24_betatron_on_with_polarizer_60s_1Mev_13-03-merged.txt");
    //ifstream MyReadFile("Larix-measurement24_betatron_on_with_polarizer_60s_1Mev_13-03-6.t3pa");
    //ifstream MyReadFile("Larix-measurement21_betatron_on_no_polarizer_60s_1Mev_13-03-6.t3pa");

    vector<vector<string>> values; 

    int i = 0;
    string s;
    while (getline (MyReadFile, s)) { // && i < 200
        // Output the text from the file
        //cout << "Inicio da linha ";

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

        i++;
        //cout << endl;
    }

    MyReadFile.close();

    if (argc == 2 && strcmp(argv[1], "-c") == 0) {
        convert_energy(values);
    }

    create_output_file(values);

    return 0;
}

