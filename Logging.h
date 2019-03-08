// Logging.h
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



#ifndef __SDEVAL_LOGGING_H__
#define __SDEVAL_LOGGING_H__

#include <iostream>
#include <sstream>
#include <ctime>


class Logging {
  private:
    std::ostringstream _out;
    const char* _func;
    const char* _file;
    int _line;

  public:
    Logging(const char* __func, const char* __file, int __line) : _func(__func), _file(__file), _line(__line) {

    }

    //LOG (nnet3-train[5.5.194~1-1dcd]:PrintStatsForThisPhase():nnet-training.cc:309) Average objective function for 'output' for minibatches 2950-2959 is -2.33515 over 1280 frames.
    ~Logging() {
      std::cerr<< "LOG ("<< _func << ":" << _file << ":" << _line << ") " << _out.str() <<std::endl;
    }


  std::ostream& stream() {
    return _out;
  }
};


#define SDEVAL_LOG Logging(__func__, __FILE__, __LINE__).stream()



#endif
