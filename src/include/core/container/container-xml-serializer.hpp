#ifndef CONTAINER_XML_SERIALIZER_H
#define CONTAINER_XML_SERIALIZER_H

/* 
 * This include files contains functions for (de)serialization of XML basic types.
 */

#include <assert.h>
#include <string.h>
#include <string>
#include <inttypes.h>

#include <exception>

using namespace std;

namespace j_xml_serialization{

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

static string convertStringBuffer(stringstream & s){
	if (s.eof()){
		return "Buffer reached end of string.";
	}
	return "Buffer: " + s.str().substr(s.tellg());
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
      throw XMLException("Error deserializing content of string, parsed so far: \"" + str.str() + "\"."  + convertStringBuffer(s));
   }
   x = str.str();
}


static void devourWhitespace(stringstream & s){
   while(s.peek() == '\t' || s.peek() == ' ' ) {
      s.get();
   }
}


static string retrieveTag(stringstream & s, const string & tag, const string & obj,  bool end=false){
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
      throw XMLException("Error reached end of string in obj: " + obj + ", tag " + tag + " is not closed. " + convertStringBuffer(s));
   }
   return out.str();
}

static void checkTag(stringstream & s, const string & tag, const string & obj,  bool end=false){
	string out = retrieveTag(s, tag, obj, end);
   if (out != tag){
      throw XMLException("Error deserializing \"" + obj + "\", expected " + tag + " but got " + out + ". " + convertStringBuffer(s));
   }
}


static void checkXMLTagBegin(stringstream & s, const string & name, const string & obj){ 
   retrieveTag(s, name, obj, false);
}


static void checkXMLTagEnd(stringstream & s, const string & name, const string & obj){
   retrieveTag(s, name, obj, true);
}

template <typename MYTYPE>
void retrieveSimpleXMLTag(stringstream & s, const string & name, MYTYPE & typ){
   checkXMLTagBegin(s, name, name);
   devourWhitespace(s);
   try{
      s >> typ ;
   }catch(exception & e){
      throw XMLException("Error deserializing content of \"" + name + "\". " + convertStringBuffer(s));
   }
   checkXMLTagEnd(s, name, name);
}

template <>
void retrieveSimpleXMLTag(stringstream & s, const string & name, string & str){
   checkXMLTagBegin(s, name, name);
   decodeXML(str, s) ;
   checkXMLTagEnd(s, name, name);
}

template <typename TYPE>
static void storeSimpleXMLTag(stringstream & s, const string & name, TYPE & typ, int intent){
	for (int x = 0; x < intent; x++ ){
		s << "  ";
	}
	s << "<" << name << ">" << typ << "</" << name << ">" << endl;
}

template <>
void storeSimpleXMLTag(stringstream & s, const string & name, string & typ, int intent){
	for (int x = 0; x < intent; x++ ){
		s << "  ";
	}	
	s << "<" << name << ">";
	encodeXML(typ, s);
	s << "</" << name << ">" << endl;
}

static void storeTagBegin(stringstream & s, const string & name, int intent){
	for (int x = 0; x < intent; x++ ){
		s << "  ";
	}	
	s << "<" << name << ">" << endl;
}


static void storeTagEnd(stringstream & s, const string & name, int intent){
	for (int x = 0; x < intent; x++ ){
		s << "  ";
	}	
	s << "</" << name << ">" << endl;
}

} // end namespace


#endif