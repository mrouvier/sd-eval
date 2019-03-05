// OptAssign.h
//
// Copyright (C) 2013  LNE (author: Olivier Galibert)
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



#ifndef OPTASSIGN_H
#define OPTASSIGN_H

class OptAssign {
    public:
        int nbElement;
        int *cost;
        int *lx;
        int *ly;
        int max_match;
        int *xy;              
        int *yx;              
        bool *S;
        bool *T;        
        int *slack;           
        int *slackx;          
        int *prev; 

        OptAssign(int n);
        ~OptAssign();

        void clear_cost_values();
        void set_cost_value(int x, int y, int val);
        int *munker();

    private:
        int get_cost(int, int);
        void init_labels();
        void update_labels();
        void add_edge(int, int);
        void find_assignement();
};

#endif
