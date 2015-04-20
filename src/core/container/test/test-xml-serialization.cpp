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


#include <vector>
// key is entity.

struct obj{
   string name;
   int typ;
};

struct test{
   obj o;
   vector<obj> my_array;
   string filename;
   int count;
};

namespace j_xml_serialization{

void serialize(struct obj & t, stringstream & s, int intent = 0, const string & name = "obj"){
   storeTagBegin(s, name, intent);
   
   storeSimpleXMLTag(s, "name", t.name, intent + 1); 
   storeSimpleXMLTag(s, "typ", t.typ, intent + 1);

   storeTagEnd(s, name, intent);
}

void serialize(struct test & t, stringstream & s, int intent = 0, const string & name = "test"){
   storeTagBegin(s, name, intent);
   serialize(t.o, s, intent + 1, "o");

   // serialize array:

   storeTagBegin(s, "my_array", intent + 1);
   for (obj & o: t.my_array) {
      serialize(o, s, intent + 2);
   }
   storeTagEnd(s, "my_array", intent + 1);

   storeSimpleXMLTag(s, "filename", t.filename, intent + 1);
   storeSimpleXMLTag(s, "count", t.count, intent + 1);

   storeTagEnd(s, name, intent);
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

   // deserialize array
   checkXMLTagBegin(s, name, "my_array");

   // now parse until we find my_array.
   while(true){
      string tag = retrieveTag(s, "obj", "test");
      if (tag == "/my_array"){
         break;
      }
      if (tag != "obj"){
         throw XMLException("Error expected obj as child in my_array of test but got " + tag + ". " + convertStringBuffer(s));
      }
      s.seekg(-5 , ios_base::cur);
      obj child; 
      deserialize(child, s);
      t.my_array.push_back(child);
   }

   retrieveSimpleXMLTag(s, "filename", t.filename);
   retrieveSimpleXMLTag(s, "count", t.count);

   checkXMLTagEnd(s, name, "test");
}

// I need to add this code to simplify parsing from strings.
template <typename TYPE>
static void deserialize(TYPE & t, const string & str, const string & name){
   stringstream s(str);
   deserialize(t, s, name);
}

template <typename TYPE>
static void deserialize(TYPE & t, const string & str){
   stringstream s(str);
   deserialize(t, s);
}
/////////////////////////////////////

}


int main(){
   test t = {{"test with & empty <1>", 5}, {{{"test", 5}, {"test2", 2}}}, "testfilename", 3};
   string start = t.o.name;

   stringstream s;
   j_xml_serialization::serialize(t, s);
   cout << s.str() << endl;

   t.o.name = "schuhe"; 
   t.o.typ = 4;
   cout << t.o.name << endl;

   j_xml_serialization::deserialize(t, s);
   cout << t.o.name << endl;

   if (t.o.name != start || t.o.typ != 5){
      cerr << "ERR" << endl;
   }else{
      cerr << "OK" << endl;
   }

   string input = "<test> \n   <o> \n     <name>test with &amp; empty &lt;1&gt;</name> \n     <typ>5</typ> \n   </o> \n   <my_array> \n     <obj> \n       <name>test</name> \n       <typ>5</typ> \n     </obj> \n     <obj> \n       <name>test2</name> \n       <typ>2</typ> \n     </obj> \n   </my_array> \n   <filename>testfilename</filename> \n   <count>3</count> \n </test>";
   t = test();

   j_xml_serialization::deserialize(t, input);
   cout << t.o.name << endl;

   if (t.o.name != start || t.o.typ != 5){
      cerr << "ERR" << endl;
   }else{
      cerr << "OK" << endl;
   }
}