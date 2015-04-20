#include <iostream>
#include <unordered_map>
#include <sstream>
#include <exception>

using namespace std;




class XMLException: public exception
{
public:
  string txt;

  XMLException(const string & what){
      txt = what;
  }

  virtual const char* what() const throw()
  {
    return txt.c_str();
  }
};

struct member_description{
   string type;
   void * address;
};

struct class_description{
   string name;
   // member name
   unordered_map<string, member_description> members;
};


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

static void encodeXML(string x, stringstream & s){
   for (char c: x){
      switch (c)
      {
      case '\"': s << "&quot;"; break;   
      case '&':  s << "&amp;";  break;   
      case '<':  s << "&lt;";   break;   
      case '>':  s << "&gt;";   break;
      default: 
         s << c;
      }
   }
}
static void decodeXML(string & x, stringstream & s){   
   stringstream str;
   try{
      while(true){
         char c;
         c = s.peek();
         if (c == '&'){
            // devour special chars.
            s.get();
            c = s.get();
            switch(c){
               case 'q':
                  s.get();
                  s.get();
                  s.get();
                  str << '\"';
                  break;
               case 'a':
                  s.get();
                  s.get();
                  str << '&';
                  break;
               case 'l':
                  s.get();
                  str << '<';
                  break;
               case 'g':
                  s.get();
                  str << '>';
                  break;
               default:
                  throw exception();
            }
            c = s.get();
            if (c != ';'){
               // should terminate with ;
               throw exception();
            }
            continue;
         }
         if (c == '<'){
            break;
         }
         str << c;
         s.get();
      }
      
   }catch (exception & e){
      throw XMLException("Error deserializing content of string, parsed so far: \"" + str.str() + "\".");
   }
   x = str.str();
}

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


static void devourWhitespace(stringstream & s){
   while(s.peek() == '\t' || s.peek() == ' ' ) {
      s.get();
   }
}


static void retrieveTag(stringstream & s, const string & tag, const string & obj,  bool end=false){
   devourWhitespace(s);
   char c;
   stringstream out;
   try{
      s >> c;
      if (c != '<') {
         throw exception();
      }
      if (end){
         s >> c;
         if (c != '/') {
            throw exception();
         }      
      }
      while(true){
         c = s.get();
         if (c == '>'){
            break;
         }
         out << c;
      }
      if (c != '>') {
         throw exception();
      }
   }catch(exception & e){
      throw XMLException("Error reached end of string, tag " + tag + " is not closed. Remaining buffer: " + s.str().substr(s.tellg()));
   }

   if (out.str() != tag){
      throw XMLException("Error deserializing \"" + obj + "\", expected " + tag + " but got " + out.str());
   }
}

static void checkXMLTagBegin(stringstream & s, const string & name, const string & obj){ 
   retrieveTag(s, name, obj, false);
}


static void checkXMLTagEnd(stringstream & s, const string & name, const string & obj){
   retrieveTag(s, name, obj, true);
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