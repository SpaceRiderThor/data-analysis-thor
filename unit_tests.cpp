#include <gtest/gtest.h>
#include "data_analysis.cpp"


// convert_energy

class Convert_EnergyTests : public ::testing::Test {
    protected:
    void SetUp() override {
        
    }

    void TearDown() override {

    }
};




// readConfig

class ReadConfigTests : public ::testing::Test {
    protected:
    void SetUp() override {
        // backup original config if it exists
        if(ifstream("config.txt")) {
            ifstream src("config.txt");
            ofstream dst("config_backup.txt");
            dst << src.rdbuf();
        }

    }

    void TearDown() override {
        //restore original config
        if(ifstream("config_backup.txt")){
            ifstream src("config_backup.txt");
            ofstream dst("config.txt");
            dst << src.rdbuf();
            remove("config_backup.txt");
        } else {
            remove("config.txt");
        }
    }

    void createConfigFile(string content) {
        ofstream configFile("config.txt");
        configFile << content;
        configFile.close();
    }
};

TEST_F(ReadConfigTests, FileOpenFailure) { //exits method if file does not exists
    remove("config.txt");

    EXPECT_EQ(readConfig(), 1);
};


TEST_F(ReadConfigTests, ReadFinger) { //read correctly
    createConfigFile("mode finger\nbottomLeftCorner 0 0\nheight 10\nsizePixel 1\ndetectorSize 100 100\nenergy kev\ndataFile file.txt\nabct folder"); //create test file

    EXPECT_EQ(readConfig(), 0);

    EXPECT_EQ(mode, "finger");
    EXPECT_EQ(bottomLeftX, 0);
    EXPECT_EQ(bottomLeftY, 0);
    EXPECT_EQ(sizePixel, 1);
    EXPECT_EQ(detectorSizeX, 100);
    EXPECT_EQ(detectorSizeY, 100);
    EXPECT_EQ(energy, "kev");
    EXPECT_EQ(inputFileName, "file.txt");
    EXPECT_EQ(abctFolder, "folder");
};

TEST_F(ReadConfigTests, ReadQuad) {
    createConfigFile("mode quad\nbottomLeftCorner 0 0\nheight 10\nspaceBetween 1 2 3\nsizePixel 1\ndetectorSize 100 100\nenergy kev\ndataFile file.txt\nabct folder");

    EXPECT_EQ(readConfig(), 0);

    EXPECT_EQ(mode, "quad");
    EXPECT_EQ(bottomLeftX, 0);
    EXPECT_EQ(bottomLeftY, 0);
    EXPECT_EQ(height, 10);
    EXPECT_EQ(spacing_1_2, 1);
    EXPECT_EQ(spacing_2_3, 2);
    EXPECT_EQ(spacing_3_4, 3);
    EXPECT_EQ(sizePixel, 1);
    EXPECT_EQ(detectorSizeX, 100);
    EXPECT_EQ(detectorSizeY, 100);
    EXPECT_EQ(energy, "kev");
    EXPECT_EQ(inputFileName, "file.txt");
    EXPECT_EQ(abctFolder, "folder");
};

TEST_F(ReadConfigTests, ReadInstrument) {
    createConfigFile("mode instrument\nbottomLeftCorner0 0 0\nbottomLeftCorner1 1 1\nbottomLeftCorner2 2 2\nbottomLeftCorner3 3 3\nheight 10 11 12 13\nrotated 0 0 0 0\ninverted 0 0 0 0\nspaceBetween 1 2 3\nsizePixel 1\ndetectorSize 100 100\nenergy kev\ndataFile folder0\nabct folder1");

    EXPECT_EQ(readConfig(), 0);

    EXPECT_EQ(mode, "instrument");
    EXPECT_EQ(bottomLeftX, 0);
    EXPECT_EQ(bottomLeftY, 0);
    EXPECT_EQ(height, 10);
    EXPECT_EQ(bottomLeft1X, 1);
    EXPECT_EQ(bottomLeft1Y, 1);
    EXPECT_EQ(height1, 11);
    EXPECT_EQ(rotated1, false);
    EXPECT_EQ(inverted1, false);
    EXPECT_EQ(bottomLeft2X, 2);
    EXPECT_EQ(bottomLeft2Y, 2);
    EXPECT_EQ(height2, 12);
    EXPECT_EQ(rotated2, false);
    EXPECT_EQ(inverted2, false);
    EXPECT_EQ(bottomLeft3X, 3);
    EXPECT_EQ(bottomLeft3Y, 3);
    EXPECT_EQ(height3, 13);
    EXPECT_EQ(rotated3, false);
    EXPECT_EQ(inverted3, false);
    EXPECT_EQ(spacing_1_2, 1);
    EXPECT_EQ(spacing_2_3, 2);
    EXPECT_EQ(spacing_3_4, 3);
    EXPECT_EQ(sizePixel, 1);
    EXPECT_EQ(detectorSizeX, 100);
    EXPECT_EQ(detectorSizeY, 100);
    EXPECT_EQ(energy, "kev");
    EXPECT_EQ(inputFileName, "folder0");
    EXPECT_EQ(abctFolder, "folder1");
}

TEST_F(ReadConfigTests, IncompleteConfig) { 
    createConfigFile("mode finger\nbottomLeft 0 0\n");

    EXPECT_EQ(readConfig(), 1);
}


// create_output_file_finger

class CreateOutputFingerTests: public ::testing::Test {
    protected:
    vector<vector<string>> data;
 
    // mock data vector
    void SetUp() override {
        readConfig();

        vector<string> v1{ "0", "9", "442954", "8", "17", "0" };
        vector<string> v2{ "1", "265", "442956", "6", "24", "0" };
        vector<string> v3{ "2", "8", "442953", "30", "20", "0" };
        data.push_back(v1); 
        data.push_back(v2);
        data.push_back(v3);
    }

    void TearDown() override {

    } 

};

TEST_F(CreateOutputFingerTests, correctExecutionKev) {
    EXPECT_EQ(create_output_file_finger(data), 0);
};

TEST_F(CreateOutputFingerTests, correctExecutionTot) {
    energy = "tot";

    EXPECT_EQ(create_output_file_finger(data), 0);
};

TEST_F(CreateOutputFingerTests, wrongData) {
    vector<string> v{"0", "1", "a"};
    data.push_back(v);

    EXPECT_EQ(create_output_file_finger(data), 1);
};


// finger

class FingerTests : public ::testing::Test {
    protected:
    void SetUp() override {
        readConfig();
    }

    void TearDown() override {
    }
};

TEST_F(FingerTests, SuccessfulRun) {
    EXPECT_EQ(finger(), 0);
}

TEST_F(FingerTests, NoInputFile) {
    inputFileName = "no_file.txt";

    EXPECT_EQ(finger(), 1);
}

// output_beginning

class OutputBeginningTests : public ::testing::Test {
    protected:
    // remove pre-existing files
    void SetUp() override {
        remove(filename);
    }

    // remove files created during tests
    void TearDown() override {
        remove(filename);
    }
};

TEST_F(OutputBeginningTests, CorrectOutput) {
    output_beginning();

    ifstream f(filename);
    ASSERT_TRUE(f.is_open());

    string line;
    getline(f, line);
    EXPECT_EQ(line, "Version 200");

    getline(f, line);
    EXPECT_EQ(line, "Type EVTA");

    f.close();
}

TEST_F(OutputBeginningTests, HandlesFileOpenError) {
    // redirect stderr to a stringstream to capture the error message
    stringstream buffer;
    streambuf* old = cerr.rdbuf(buffer.rdbuf());

    // mock failure to open file
    ofstream testFile(filename);
    testFile.close();
    chmod(filename, 0000); // makes the file unreadable/unwritable by stripping it of its permissions

    output_beginning();

    string errorMessage = buffer.str();
    EXPECT_NE(errorMessage.find("Error: Unable to write headers into output file."), string::npos);

    // Restore permissions and original stderr        remove(filename);

}

// output_content



// output_end

class OutputEndTests : public ::testing::Test {
    protected:
    string testFilename = "testfile.txt";


    void SetUp() override {
        remove(testFilename.c_str());

        // create mock file w/ header and 2 hits
        ofstream testFile(testFilename);
        if(testFile.is_open()) {
            testFile << "Version 200\n";
            testFile << "Type EVTA\n\n";

            testFile << "SE\n";
            testFile << "ID 1\n";
            testFile << "TI 0.0123868313\n";
            testFile << "HT 8;1.7615;2.646;0;9;0.00275;0.00275;0;0.5\n";

            testFile << "SE\n";
            testFile << "ID 2\n";
            testFile << "TI 0.0123868156\n";
            testFile << "HT 8;4.572;2.6405;0;35;0.00275;0.00275;0;0.5\n";
        
            testFile.close();
        } else {
            cerr << "Error: unable to open test file in SetUp() of OutputEndTests suite.\n";
        }
    }

    // remove files created during tests
    void TearDown() override {
        remove(testFilename.c_str());
    }
};

TEST_F(OutputEndTests, CorrectOutput) {
    output_end("testfile.txt");

    ifstream f("testfile.txt");
    ASSERT_TRUE(f.is_open());

    string line;
    string lastLine;
    while (getline(f, line)) { // get last line from output file
        lastLine = line; 
    }
    EXPECT_EQ(lastLine, "EN");

    f.close();
}

TEST_F(OutputEndTests, HandleFileOpenError) {
    // redirect stderr to a stringstream to capture the error message
    stringstream buffer;
    streambuf* old = cerr.rdbuf(buffer.rdbuf());

    // mock failure to open file
    ofstream testFile("testfile.txt");
    testFile.close();
    chmod("testfile.txt", 0000); // makes the file unreadable/unwritable by stripping it of its permissions

    output_end("testfile.txt");

    string errorMessage = buffer.str();
    EXPECT_NE(errorMessage.find("Error: Unable to write footer into output file."), string::npos);

    // Restore permissions and original stderr
    chmod("testfile.txt", 0644); 
    std::cerr.rdbuf(old); 
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}