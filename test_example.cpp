#include <gtest/gtest.h>
#include "data_analysis.cpp"

/*int add(int a, int b) {
    return a + b;
}

// Example test case
TEST(AdditionTest, HandlesPositiveInput) {
    EXPECT_EQ(add(1, 2), 3);
}*/

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

TEST(OutputBeginningTests, CorrectOutput) {
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}