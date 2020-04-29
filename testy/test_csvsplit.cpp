#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "binstream.hpp"
#include "mmap.hpp"

using namespace std;

//https://ideone.com/cyVOkZ
// split explicitly with ',' as delimeter
// won't quite handle quoted entries
// This was a little experiment
vector<string> splitCSV(string line)
{
    const regex re{"((?:[^\\\\,]|\\\\.)*?)(?:,|$)"};
    return {sregex_token_iterator(line.begin(), line.end(), re, 1), sregex_token_iterator()};
}



class CSVSplitter 
{

public:
    // This next field calculator will split fields strictly
    // on the delimeter.  it will NOT honor delimeters
    // which are embedded in '"' fields.

    // So, if the input has a quoted field with an embedded delimeter
    // it will be split across that embedded delimeter
    static string nextField(BinStream &bs, const char delim)
    {
        string s;
    
        //printf("nextField: %zd - %zd (%c)\n", bs.tell(), bs.size(), bs.peekOctet());

        if (bs.tell() == bs.size())
        {
            bs.skip(1);
            return s;
        }

        // trim leading whitespace
        while (!bs.isEOF() && isspace(bs.peekOctet())) {
            bs.skip(1);
        }

        while (!bs.isEOF()) {
            char c = bs.readOctet();

            // check for the delimeter
            if (c == delim){
                // break out of the loop if we found it
                break;
            }

            // append a character to the string
            s.append(1, c);
        }

        return s;
    }

    static vector<string> SplitFields(char *buff, size_t bufflen, const char delimeter=',')
    {
        vector<string> res;
        BinStream bs(buff, bufflen);

        while (!bs.isEOF()) {
            auto field = CSVSplitter::nextField(bs, delimeter);
            res.push_back(field);
        }

        return res;
    }
};


void test_fields()
{
    char * s = "this, is some comma, separated stuff,,";
    BinStream bs(s, strlen(s));

    while (!bs.isEOF()) {
        auto field = CSVSplitter::nextField(bs, ',');
        //printf("field: %s\n", field.c_str());
    }
}

/*
countriesAndTerritories,
location,
continent,
population_year,
population
*/
void print_covid_locations()
{
    mmap m = mmap::create(".\\data\\locations.csv");
    BinStream bs = BinStream(m.data(), m.size());
    if (!bs.isValid())
        return ;

    char buff[1024];
    int bufflen = 1024;
    int line = 1;

    printf(
"struct CovidLocation {\n"
"  string name;\n"
"  int year;\n"
"  int population;\n"
"};\n\n");

    printf("CovidLocation locations[] = {\n");
    while (!bs.isEOF()) {
        size_t n = bs.readLine(buff, bufflen);
        auto fields = CSVSplitter::SplitFields(buff, n);

        printf("{\"%s\",%s,%s},\n", fields[1].c_str(), fields[3].c_str(), fields[4].c_str());
    }
    printf("};\n");

}

void test_file()
{
    mmap m = mmap::create(".\\data\\locations.csv", GENERIC_READ);
    BinStream bs = BinStream(m.data(), m.size());
    if (!bs.isValid())
        return ;

    char buff[1024];
    int bufflen = 1024;
    int line = 1;

    while (!bs.isEOF()) {
        size_t n = bs.readLine(buff, bufflen);

        auto fields = CSVSplitter::SplitFields(buff, n);

        printf("%d[%zd]: %s\n", line, n, buff);

        // print the fields
        for (int i=0; i< fields.size(); i++) {
            printf("  - %s\n", fields[i].c_str());
        }
        //for (const auto& i : fields) cout << "  - " << i << endl;

        line = line + 1;
    }
}

void main()
{
    //test_file();
    //test_fields();
    print_covid_locations();
}