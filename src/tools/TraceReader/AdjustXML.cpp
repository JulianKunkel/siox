#include <sstream>

namespace tools{
   namespace XML{
      string extractNextTag(stringstream & s, bool & isFinish){
         stringstream out;
         while(s.good()){
            char c = s.get();
            if(c == '<'){
               break;
            }
         }
         if (s.good()){
            isFinish = (s.peek() == '/');
            if (isFinish) {
               s.get();
            }
         }else{
            isFinish = false;
            return "";
         }
         while(s.good()){
            char c = s.get();
            if(c == '>'){
               break;
            }
            out << c;
         }
         return out.str();
      }


      string decodeXML(stringstream & s){   
         stringstream str;
         char c;
         while((c = s.peek()) > 0){
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
                     assert(0);
               }
               c = s.get();
               if (c != ';'){
                  // should terminate with ;
                  assert(0);
               }
               continue;
            }
            if (c == '<'){
               break;
            }
            str << c;
            s.get();
         }
         return str.str();
      }

      string extractText(stringstream & s){
         stringstream out;
         // remove leading whitespace
         while(s.good()){
            char c = s.peek();
            if (! ( c == ' ' || c == '\t' || c == '\n') || c == '<'){
               break;
            }
            s.get();
         }

         while(s.good()){
            char c = s.peek();
            if(c == '<'){
               break;
            }
            out << c;
            s.get();
         }

         return decodeXML(out);
      }
   }
}