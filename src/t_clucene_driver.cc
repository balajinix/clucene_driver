#include "clucene_driver.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {

  CLuceneDriver cl_driver;

  // index location
  std::cout << "Enter index location\n";
  std::string index_location;
  getline(std::cin, index_location);

  cl_driver.InitIndex(index_location, true);

  std::string str;
  std::cout << "Enter documents to index: ('end' to stop data entry)\n";
  std::string field_name = std::string("contents");
  while (getline(std::cin, str)) {
    if (str.compare("end") == 0)
      break;
    if (cl_driver.AddField(field_name, str) < 0) {
      std::cerr << "ERROR: could not add field to a new document\n";
    }
    if (cl_driver.AddDocument() < 0) {
      std::cerr << "ERROR: could not add document to index\n";
    }
  }
  cl_driver.CloseIndex();

  cl_driver.InitSearch(index_location);
  std::cout << "Enter query: ('quit' to stop)\n";
  while (getline(std::cin, str)) {
    if (str.compare("quit") == 0)
      break;
    if (cl_driver.Search(str, field_name) < 0) {
      std::cerr << "ERROR: could not search\n";
    }
  }
  cl_driver.CloseSearch();

  return 0;
}
