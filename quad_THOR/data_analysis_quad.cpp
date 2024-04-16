#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

#define filename "dados.evta"

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
int output_content(vector<vector<string>> data, int detectorId, int eventId){
    float offset; //1.408 -> detector length
    if(detectorId == 0) {
        offset = 0; 
    } else if (detectorId == 1) {
        offset = 1.408 + 0.1; //0.1 -> 1mm between 1st and 2nd detectors
    } else if (detectorId == 2) {
        offset = 2*1.408 + 0.1 + 0.5; //0.5 -> distance between 2nd and 3rd detector
    } else {
        offset = 3*1.408 + 0.1 + 0.5 + 0.1; //0.1 -> 1mm between 3rd and 4th detectors
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
            outFile << "HT 8;" << ((stoi(data[i][1]) % 256))*0.0055 + offset << ";" << ((stoi(data[i][1])/256))*0.0055 << ";" << 0 << ";" << stoi(data[i][3]) << ";" << 0.00275 << ";" << 0.00275 << ";" << 0 << ";" << 0.5 << "\n"; //HT detectorID;x;y;z;energy;x_uncertainty;y_uncertainty;z_uncertainty;energy_uncertainty --- shift of half a pixel to the right

        }

        cout << "The content has been written to the file.\n";
        outFile.close();
    } else {
        cerr << "Error: Unable to write content into output file.\n";
    }

    outFile.close();

    cout << "EVENTID: " << eventId << endl;
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

int main() {
    //TODO escrever os valores de cada ficheiro

    output_beginning();


    /*ifstream File1("Na22-pointSource/Na22-point-source-1.t3pa");
    ifstream File2("Na22-pointSource/Na22-point-source-2.t3pa");
    ifstream File3("Na22-pointSource/Na22-point-source-3.t3pa");
    ifstream File4("Na22-pointSource/Na22-point-source-4.t3pa");*/

    ifstream File1("Ba133_colimated1cm-calibrated.t3pa");
    ifstream File2("Ba133_colimated1cm-calibrated.t3pa");
    ifstream File3("Ba133_colimated1cm-calibrated.t3pa");
    ifstream File4("Ba133_colimated1cm-calibrated.t3pa");

    vector<ifstream> files;
    files.push_back(move(File1));
    files.push_back(move(File2));
    files.push_back(move(File3));
    files.push_back(move(File4));


    vector<vector<string>> values;
    string s;

    int eventId = 1;

    for(int i = 0; i < files.size(); i++) {
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

        eventId = output_content(values, i, eventId);
        values.clear();
    }

    output_end();
    

    return 0;
}