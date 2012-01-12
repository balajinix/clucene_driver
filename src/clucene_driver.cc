#include "clucene_driver.h"

#include "../shared/config/repl_tchar.h"
#include "../shared/util/Misc.h"
#include "../shared/util/StringBuffer.h"

using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::store;
using namespace lucene::document;
using namespace lucene::queryParser;
using namespace lucene::search;

CLuceneDriver::CLuceneDriver() {
}

CLuceneDriver::~CLuceneDriver() {
  _lucene_shutdown(); //clears all static memory
}

int CLuceneDriver::InitIndex(const std::string& index_location, const bool clear_index) {

  m_writer = NULL;

  if (!clear_index && IndexReader::indexExists(index_location.c_str()) ){
    if (IndexReader::isLocked(index_location.c_str())){
      std::cout << "Index was locked... unlocking it.\n";
      IndexReader::unlock(index_location.c_str());
    }
    m_writer = _CLNEW IndexWriter(index_location.c_str(), &m_whitespace_analyzer, false);
  }else{
    m_writer = _CLNEW IndexWriter(index_location.c_str(), &m_whitespace_analyzer, true);
  }

  //writer->setInfoStream(&std::cout);

  // We can tell the writer to flush at certain occasions
  //writer->setRAMBufferSizeMB(0.5);
  //writer->setMaxBufferedDocs(3);

  // To bypass a possible exception (we have no idea what we will be indexing...)
  m_writer->setMaxFieldLength(0x7FFFFFFFL); // LUCENE_INT32_MAX_SHOULDBE
  
  // Turn this off to make indexing faster; we'll turn it on later before optimizing
  m_writer->setUseCompoundFile(false);

  // m_doc used in add field
  m_doc.clear();

  return 0;
}

int CLuceneDriver::AddField(const std::string& field_name, const std::string& value) {

  if (field_name.empty()) {
    std::cerr << "ERROR: invalid input\n";
    return -1;
  }

  if (value.empty()) {
    std::cerr << "WARNING: empty input\n";
    return 0;
  }

  StringBuffer string_buffer;
  TCHAR tbuf[1024];
  STRCPY_AtoT(tbuf, (char*) value.c_str(), value.length());
  tbuf[value.length()]=0; // suspect
  string_buffer.append(tbuf);
  STRCPY_AtoT(tbuf, (char*) field_name.c_str(), field_name.length());
  tbuf[field_name.length()]=0; // suspect

  m_doc.add( *_CLNEW Field(tbuf, string_buffer.getBuffer(), Field::STORE_YES | Field::INDEX_TOKENIZED) );

  return 1;
}

int CLuceneDriver::AddDocument() {
  m_writer->addDocument(&m_doc);
  m_doc.clear();
  return 1;
}

int CLuceneDriver::CloseIndex() {

  // Make the index use as little files as possible, and optimize it
  m_writer->setUseCompoundFile(true);
  m_writer->optimize();
  
  // Close and clean up
  m_writer->close();
  _CLLDELETE(m_writer);

  return 0;
}

int CLuceneDriver::InitSearch(const std::string& index_location) {

  m_reader = IndexReader::open(index_location.c_str());
  if (!m_reader) {
    std::cerr << "ERROR: could not open index for search\n";
    return -1;
  }

  return 0;
}

int CLuceneDriver::Search(const std::string& query_string, const std::string& field_name) {

  if (query_string.empty() || query_string.length() > 128) {
    std::cerr << "ERROR: invalid query string\n";
    return -1;
  }

  IndexReader* newreader = m_reader->reopen();
  if (newreader != m_reader){
    _CLLDELETE(m_reader);
    m_reader = newreader;
  }

  IndexSearcher searcher(m_reader);

  TCHAR tline[128];
  STRCPY_AtoT(tline, query_string.c_str(), query_string.length());
  tline[query_string.length()]=0;

  TCHAR tname[128];
  STRCPY_AtoT(tname, field_name.c_str(), field_name.length());
  tname[field_name.length()]=0;

  Query* query;
  try {
    query = QueryParser::parse(tline, tname, &m_standard_analyzer);
  } catch (...) {
    std::cout << "caught query parser exception\n" << std::endl;
  }

/*
  TCHAR* buf;
  buf = query->toString(_T("contents"));
  _tprintf(_T("Searching for: %s\n\n"), buf);
  _CLDELETE_LCARRAY(buf);
*/

  Hits* hits = searcher.search(query);

  std::cout << hits->length() << " result(s) found\n";

  char result[1024];
  for (size_t i=0; i < hits->length(); i++){
    lucene::document::Document* doc = &hits->doc(i);
    double score = hits->score(i);
    const TCHAR* tbuf = doc->get(_T("contents"));
    size_t tbuf_len = _tcslen(tbuf);
    STRCPY_TtoA(result, tbuf, tbuf_len);
    result[tbuf_len]=0;
    std::cout << i+1 << " " << result << " " << score << std::endl;
    // _tprintf(_T("%d. %s - %f\n"), i, doc->get(_T("contents")), hits->score(i));
  }

  _CLLDELETE(hits);
  _CLLDELETE(query);
  query = NULL;

  searcher.close();

  return 0;
}

int CLuceneDriver::CloseSearch() {
  m_reader->close();
  _CLLDELETE(m_reader);
  return 0;
}

