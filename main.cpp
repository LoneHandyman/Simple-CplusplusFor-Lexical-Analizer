#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#define RW_FOR            "for"
#define TOKEN_OPEN        '('
#define TOKEN_CLOSED      ')'
#define TOKEN_SEPARATOR   ';'

//<for>: (<decl>;<comp>;<incr>);
//<decl>: <type><idl>=<idr>|<idl>=<idr>|e
//<type>: int|char|float|etc
//<idl>: no number
//<idr>: no limits
//<comp>: <idr><symbol><idr>
//<incr>: <idl>

//{"int","contador","=","5"}

std::map<std::string, bool> acceptedTypes;
std::map<std::string, bool> acceptedAssignSymbols;
std::map<std::string, bool> acceptedCompSymbols;
std::map<std::string, bool> acceptedFincrSymbols;//Fast increment
std::map<std::string, bool> acceptedAopSymbols;//Aritmetic operation
std::map<std::string, std::vector<std::vector<std::string>>> grammar;

void init(){
  grammar["<decl>"] = {{"<type>","<id>","=","<num>"},
                       {"<type>","<id>","=","<id>"},
                       {"<id>","=","<num>"},
                       {"<id>","=","<id>"},{}};
  grammar["<comp>"] = {{"<id>","<symbol_comp>","<num>"},
                       {"<id>","<symbol_comp>","<id>"},{}};
  grammar["<incr>"] = {{"<id>","<symbol_fincr>"},
                       {"<symbol_fincr>", "<id>"},
                       {"<id>","<symbol_aop>","<num>"},
                       {"<id>","<symbol_aop>","<id>"},{}};

  acceptedTypes["int"] = true;
  acceptedTypes["size_t"] = true;
  acceptedTypes["uint32_t"] = true;
  acceptedTypes["auto"] = true;

  acceptedCompSymbols["=="] = true;
  acceptedCompSymbols["!="] = true;
  acceptedCompSymbols["<="] = true;
  acceptedCompSymbols[">="] = true;
  acceptedCompSymbols["<"] = true;
  acceptedCompSymbols[">"] = true;

  acceptedAopSymbols["+="] = true;
  acceptedAopSymbols["-="] = true;

  acceptedFincrSymbols["++"] = true;
  acceptedFincrSymbols["--"] = true;
}

std::string clearSpacesAndTabs(const std::string& sentence){
  std::string cleanedSentence;
  for(char c : sentence){
    if(c != ' ' && c != '\t')
      cleanedSentence.push_back(c);
  }
  return cleanedSentence;
}

bool analyzeTerminalToken(const std::string& token, const std::string& value){
  if(token == "<type>"){
    return acceptedTypes.find(value) != acceptedTypes.end();
  }
  if(token == "<id>"){
    return value.length() && !std::isdigit(value[0]);
  }
  if(token == "="){
    return value == "=";
  }
  if(token == "<num>"){
    for(const char& possibleDigit : value){
      if(!std::isdigit(possibleDigit))
        return false;
    }
    return true;
  }
  if(token == "<symbol_comp>"){
    return acceptedCompSymbols.find(value) != acceptedCompSymbols.end();
  }
  if(token == "<symbol_fincr>"){
    return acceptedFincrSymbols.find(value) != acceptedFincrSymbols.end();
  }
  if(token == "<symbol_aop>"){
    return acceptedAopSymbols.find(value) != acceptedAopSymbols.end();
  }
  return false;
}

bool analyzeGrammarToken(const std::string& token, const std::vector<std::string>& possibleGramarStruct){
  bool ok = false;
  for(auto& possibleStruct : grammar[token]){
    ok = true;
    if(possibleGramarStruct.size() == possibleStruct.size()){
      for(std::size_t idx = 0; idx < possibleStruct.size(); ++idx)
        if(!analyzeTerminalToken(possibleStruct[idx], possibleGramarStruct[idx])){
          ok = false;
          break;
        }
      if(ok) return true; 
    }
    else
      ok = false;
    
  }
  return ok;
}

void splitInTokens(const std::string& sentence, std::vector<std::string>& container){
  std::stringstream sSentence(sentence);
  std::string singleToken;
  while(sSentence >> singleToken){
    container.push_back(singleToken);
  }
}

bool forBodyParser(const std::string& body){
  if(body.length() > 0){
    std::stringstream iosentence(body);
    std::string decl, comp, incr;
    bool ok = true;
    std::vector<std::string> grammarStruct;
    if(std::getline(iosentence, decl, TOKEN_SEPARATOR)){
      splitInTokens(decl, grammarStruct);
      ok = ok && analyzeGrammarToken("<decl>", grammarStruct);
    }
    grammarStruct.clear();
    if(std::getline(iosentence, comp, TOKEN_SEPARATOR)){
      splitInTokens(comp, grammarStruct);
      ok = ok && analyzeGrammarToken("<comp>", grammarStruct);
    }
    grammarStruct.clear();
    if(std::getline(iosentence, incr, TOKEN_SEPARATOR)){
      splitInTokens(incr, grammarStruct);
      ok = ok && analyzeGrammarToken("<incr>", grammarStruct);
    }
    grammarStruct.clear();
    return ok;
  }
  return false;
}

bool lexicalAnalyzerFor(const std::string& sentence){
  if(sentence.length() > 0){
    std::stringstream iosentence(sentence);
    std::string forRW, forBody;
    if(std::getline(iosentence, forRW, TOKEN_OPEN)){
      forRW = clearSpacesAndTabs(forRW);
      if(forRW == RW_FOR){
        if(std::getline(iosentence, forBody, TOKEN_CLOSED)){
          return forBodyParser(forBody);
        }
      }
    }
  }
  return false;
}

int main(){
  init();
  std::cout << "-- Lexical Analyzer --\nPlease give a C++ FOR sentence:\n-> ";
  std::string sentence;
  getline(std::cin, sentence);
  std::cout << ((lexicalAnalyzerFor(sentence))?"YES":"NO") <<std::endl;
  return EXIT_SUCCESS;
}