// OptAssign.cc
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



#include "OptAssign.h"

#include <limits.h>
#include <string.h>

namespace {
  inline int max(int a, int b)
  {
    return a > b ? a : b;
  }

  inline int min(int a, int b)
  {
    return a < b ? a : b;
  }
}


OptAssign::OptAssign(int n)
{
  max_match=0;
  nbElement=n;
  lx=new int[n];              
  ly=new int[n];
  xy=new int[n];              
  yx=new int[n];   
  S=new bool[n];
  T=new bool[n];        
  slack=new int[n];           
  slackx=new int[n];          
  prev=new int[n]; 
  cost=new int[n*n];
  clear_cost_values();
}

OptAssign::~OptAssign()
{
  delete [] xy;
  delete [] yx;
  delete [] lx;
  delete [] ly;
  delete [] S;
  delete [] T;
  delete [] slack;
  delete [] slackx;
  delete [] prev;
  delete [] cost;
}

void OptAssign::set_cost_value(int x, int y, int val)
{
  cost[x+(y*nbElement)]=val;
}

void OptAssign::clear_cost_values()
{
  memset(cost,0,sizeof(*cost)*nbElement*nbElement);  
}

int OptAssign::get_cost(int x,int y)
{
  return cost[x+(y*nbElement)];
}

void OptAssign::init_labels()
{
  for(int i=0;i<nbElement;i++){
    lx[i] = 0;
    ly[i] = 0;
  }
  for (int i = 0; i < nbElement; i++)
    for (int j = 0; j < nbElement; j++){
      lx[i] = max(lx[i], get_cost(i,j));
      xy[i]=-1;
      yx[i]=-1; 
    }
 
}
void OptAssign::update_labels()
{
  int delta = INT_MAX;
  for(int i=0;i<nbElement;i++)
    if(!T[i])
      delta = min(delta,slack[i]);

  for(int i = 0; i < nbElement; i++)            
    if(S[i])
      lx[i] -= delta;

  for(int i = 0; i < nbElement; i++)             
    if(T[i])
      ly[i] += delta;

  for(int i = 0; i < nbElement; i++)            
    if(!T[i])
      slack[i] -= delta;
}

void OptAssign::add_edge(int x, int prevx)
{
  S[x] = true;
  prev[x] = prevx;
  for(int i=0; i<nbElement; i++) {
    int newcost = lx[x] + ly[i] - get_cost(x,i);
    if(slack[i] > newcost) {
      slack[i] = newcost;
      slackx[i] = x;
    }
  }
}

void OptAssign::find_assignement()
{ 
  int *q = new int[nbElement];
  for(int max_match=0; max_match != nbElement; max_match++) {
    for(int i = 0; i < nbElement; i++) {
      T[i] = S[i] = false;
      prev[i] = -1;
    }

    int root=0; 
    int wr = 0;
    for(int x = 0; x < nbElement; x++)            
      if(xy[x] == -1) {
	root = x;
	q[wr++] = x;
	prev[x] = -2;
	S[x] = true;
	break;
      }
  
    for(int y = 0; y < nbElement; y++) {
      slack[y] = lx[root] + ly[y] - get_cost(root,y);
      slackx[y] = root;
    }

    int rd = 0;
    int found_x, found_y;
    for(;;) {
      while(rd<wr) {
	int x = q[rd++];
	for(int y=0; y<nbElement; y++) {
	  if(lx[x] + ly[y] == get_cost(x, y) && !T[y]) {
	    if(yx[y] == -1) {
	      found_x = x;
	      found_y = y;
	      goto done;
	      
	      T[y] = true;
	      q[wr++] = yx[y];
	      add_edge(yx[y], x);		
	    }
	  }
	}
	
	update_labels();
	wr = rd = 0;
	
	for(int y=0; y<nbElement; y++)
	  if(!T[y] && slack[y] == 0) {
	    if(yx[y] == -1) {
	      found_x = slackx[y];
	      found_y = y;
	      goto done;
	      
	    } else {
	      T[y]=true;
	      if(!S[yx[y]]) {
		q[wr++] = yx[y];
		add_edge(yx[y], slackx[y]);
	      }
	    }
	  }
      }
    }
    break;

  done:

    int j = found_y;
    for(int i=found_x; i!=-2; i=prev[i]) {
      int ty=xy[i];
      yx[j]=i;
      xy[i]=j;
      j = ty;
    }
  }

  delete[] q;
}

int *OptAssign::munker()
{
  int *res =new int[nbElement];
  init_labels(); 
  find_assignement();
  for(int i=0; i<nbElement; i++)
    res[i] = xy[i];
  return res;

}


