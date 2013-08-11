#ifndef __SIOX_STDEXCEPTION_HPP
#define __SIOX_STDEXCEPTION_HPP

#include <exception>

class NotFoundError: public exception
{
public:	
  string msg;

  NotFoundError(){
    msg = "Error Attribute/Value NotFound";
  }

  NotFoundError(const string & msg){
  	this->msg = msg;
  }

  virtual const char* what() const throw()
  {
    return msg.c_str();
  }
};


class IllegalStateError: public exception
{
public:	
  string msg;

  IllegalStateError(const string & msg){
  	this->msg = msg;
  }

  virtual const char* what() const throw()
  {
    return msg.c_str();
  }
};


class IllegalArgumentError: public exception
{
public:	
  string msg;

  IllegalArgumentError(const string & msg){
  	this->msg = msg;
  }

  virtual const char* what() const throw()
  {
    return msg.c_str();
  }
};



#endif