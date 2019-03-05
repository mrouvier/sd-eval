// Error.h
//
// Copyright (C) 2019  LIA - Avignon University (author: Mickael Rouvier)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include "Logging.h"

class AssertionFailureException : public std::exception {
  private:
    const char* _expression;
    const char* _file;
    const char* _func;
    int _line;
    std::string _message;
    std::string _report;

  public:
    AssertionFailureException(const char* __expression, const char* __func, const char* __file, int __line, const std::string& __message) : _expression(__expression),  _file(__file), _func(__func), _line(__line), _message(__message) {

      Logging ml(__func, __file, __line);
      ml.stream() <<  __message ;

    }

    virtual const char* what() const throw() {
      return _report.c_str();
    }

    const char* Expression() const throw() {
      return _expression;
    }

    const char* File() const throw() {
      return _file;
    }

    int Line() const throw() {
      return _line;
    }

    const char* Message() const throw() {
      return _message.c_str();
    }

    const char* Func() const throw() {
        return _func;
    }

    ~AssertionFailureException() throw() {

    }


};


#define SDEVAL_ASSERT(EXPRESSION, MESSAGE) if(!(EXPRESSION)) { throw AssertionFailureException(#EXPRESSION, __func__, __FILE__, __LINE__, MESSAGE); }


#endif
