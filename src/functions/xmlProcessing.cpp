#include <string>
#include <vector>
#include <pugixml.hpp>

using namespace std;

class FeederXmlProcessing {
    int recCount = 0;
    int expectedCount = 0;
    vector<string> requiredAttrs = {"Track ID", "Track Number", "Year", "Name", "Album Artist", "Album", "Genre", "Date Added"};
    vector<string> ucwordsAttrs = {"Album Artist", "Album", "Genre"};
}