#include <iostream>
#include <unordered_map>
#include <sstream>
#include <exception>

#include <container-xml-serializer.hpp>

// g++ --std=gnu++11 test-xml-serialization.cpp  -g -I ../../src/include/core/container/


// struct member_description{
//    string type;
//    void * address;
// };
// 
// struct class_description{
//    string name;
//    // member name
//    unordered_map<string, member_description> members;
// };


#include <array>
// key is entity.

struct obj{
   string name;
   int typ;
};

struct test{
   obj o;
   array<obj, 5> my_array;
   string filename;
   int count;
};


void serialize(struct obj & t, stringstream & s, const string & name = "obj"){
   s << "<" << name << ">" << endl;
   s << "<name>";
   encodeXML(t.name, s);
   s << "</name>" << endl;
   s << "<typ>" << t.typ << "</typ>" << endl;
   s << "</" << name << ">" << endl;
}

void serialize(struct test & t, stringstream & s, const string & name = "test"){
   s << "<" << name << ">" << endl;
   serialize(t.o, s, "o");
   s << "</" << name << ">" << endl;
}


template <typename MYTYPE>
static void retrieveSimpleXMLTag(stringstream & s, const string & name, MYTYPE & typ){
   checkXMLTagBegin(s, name, name);
   devourWhitespace(s);
   try{
      s >> typ ;
   }catch(exception & e){
      throw XMLException("Error deserializing content of \"" + name + "\".");
   }
   checkXMLTagEnd(s, name, name);
}

template <>
void retrieveSimpleXMLTag(stringstream & s, const string & name, string & str){
   checkXMLTagBegin(s, name, name);
   decodeXML(str, s) ;
   checkXMLTagEnd(s, name, name);
}


void deserialize(struct obj & t, stringstream & s, const string & name = "obj"){
   checkXMLTagBegin(s, name, "obj");

   retrieveSimpleXMLTag(s, "name", t.name);
   retrieveSimpleXMLTag(s, "typ", t.typ);

   checkXMLTagEnd(s, name, "obj");
}

void deserialize(struct test & t, stringstream & s, const string & name = "test"){
   checkXMLTagBegin(s, name, "test");

   deserialize(t.o, s, "o");

   checkXMLTagEnd(s, name, "test");
}

template <typename TYPE>
void deserialize(TYPE & t, const string & str, const string & name){
   stringstream s(str);
   deserialize(t, s, name);
}

template <typename TYPE>
void deserialize(TYPE & t, const string & str){
   stringstream s(str);
   deserialize(t, s);
}


int main(){
   test t = {{"test with & empty <1>", 5}, {{{"test", 5}, {"test2", 2}}}, "testfilename", 3};
   string start = t.o.name;

   stringstream s;
   serialize(t, s);
   cout << s.str() << endl;

   t.o.name = "schuhe"; 
   t.o.typ = 4;
   cout << t.o.name << endl;

   deserialize(t, s);
   cout << t.o.name << endl;

   if (t.o.name != start || t.o.typ != 5){
      cerr << "ERR" << endl;
   }else{
      cerr << "OK" << endl;
   }

   string input = " <test> \n <o> \n <name>test with &amp; empty &lt;1&gt;</name> \n \t <typ>\t 5 </typ> </o> \t </test> \n";
   t = test();

   deserialize(t, input);
   cout << t.o.name << endl;

   if (t.o.name != start || t.o.typ != 5){
      cerr << "ERR" << endl;
   }else{
      cerr << "OK" << endl;
   }
}