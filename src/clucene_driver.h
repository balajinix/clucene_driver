#ifndef _CLUCENE_DRIVER_H_
#define _CLUCENE_DRIVER_H_

#include <iostream>
#include <string>

#include "CLucene/StdHeader.h"
#include "CLucene/clucene-config.h"
#include "CLucene.h"

class CLuceneDriver {

 public:
  CLuceneDriver();
  ~CLuceneDriver();
  int InitIndex(const std::string& index_location, const bool clear_index);
  int AddField(const std::string& name, const std::string& value);
  int AddDocument();
  int CloseIndex();
  int InitSearch(const std::string& index_location);
  int Search(const std::string& query_string, const std::string& field_name);
  int CloseSearch();

 private:
  lucene::document::Document m_doc;
  lucene::index::IndexWriter* m_writer;
  lucene::analysis::WhitespaceAnalyzer m_whitespace_analyzer;
  lucene::index::IndexReader* m_reader;
  lucene::analysis::standard::StandardAnalyzer m_standard_analyzer;
};

#endif // _CLUCENE_DRIVER_H_
