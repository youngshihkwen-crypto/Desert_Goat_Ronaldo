#pragma once
#include "infinite_maze.hpp"

// IDA*: iterative f=g+h thresholds, admissible Manhattan lower bound.
// One shared scratch workspace, no heap allocation and no per-enemy map copy.
struct IdaResult{Room next;int length=-1,nodes=0;bool complete=false;};
struct IdaStar{
 static constexpr int INF=100000000,MAX_DEPTH=256,TABLE_SIZE=8192;
 struct Entry{int x=0,z=0,g=0;u32 stamp=0;};
 Entry table[TABLE_SIZE];Room path[MAX_DEPTH+1],target,answer;
 u32 generation=0;int nodes=0,budget=0,answerLength=0,thresholds=0,pruned=0;bool aborted=false;
 static int heuristic(Room a,Room b){return int(absf(float(a.x-b.x))+absf(float(a.z-b.z)));}
 static u32 slot(Room p){return (u32(p.x)*0x9e3779b9u^u32(p.z)*0x85ebca6bu)&(TABLE_SIZE-1);}
 template<class Linked> int visit(Room p,int g,int bound,const Linked& linked){
  int f=g+heuristic(p,target);if(f>bound){pruned++;return f;}
  if(sameRoom(p,target)){answer=path[g?1:0];answerLength=g;return -1;}
  if(nodes>=budget||g>=MAX_DEPTH){aborted=true;return INF;}nodes++;
  Entry& memo=table[slot(p)];
  // Verify both coordinates: hash collisions must never prune another room.
  if(memo.stamp==generation&&memo.x==p.x&&memo.z==p.z&&memo.g<=g){pruned++;return INF;}
  memo={p.x,p.z,g,generation};
  Room next[4]={{p.x,p.z-1},{p.x+1,p.z},{p.x,p.z+1},{p.x-1,p.z}};
  int estimate[4];for(int i=0;i<4;i++)estimate[i]=heuristic(next[i],target);
  for(int i=0;i<4;i++)for(int j=i+1;j<4;j++)if(estimate[j]<estimate[i]){int h=estimate[i];estimate[i]=estimate[j];estimate[j]=h;Room t=next[i];next[i]=next[j];next[j]=t;}
  int minimum=INF;
  for(Room n:next){
   if(g&&sameRoom(n,path[g-1])){pruned++;continue;} // immediate reversal
   if(!linked(p,n))continue;bool cycle=false;for(int i=0;i<g;i++)if(sameRoom(path[i],n)){cycle=true;break;}
   if(cycle){pruned++;continue;}path[g+1]=n;int result=visit(n,g+1,bound,linked);
   if(result==-1)return -1;if(aborted)return INF;minimum=mini(minimum,result);
  }
  return minimum;
 }
 template<class Linked> IdaResult find(Room start,Room goal,const Linked& linked,int nodeBudget=6000){
  target=goal;nodes=0;budget=maxi(0,nodeBudget);aborted=false;thresholds=pruned=0;path[0]=start;answer=start;answerLength=0;
  int bound=heuristic(start,goal);
  while(bound<=MAX_DEPTH){
   if(++generation==0){for(auto& e:table)e.stamp=0;generation=1;}
   thresholds++;int result=visit(start,0,bound,linked);
   if(result==-1)return{answer,answerLength,nodes,true};
   if(aborted)return{start,-1,nodes,false};
   if(result==INF)return{start,-1,nodes,true};bound=result;
  }
  return{start,-1,nodes,false};
 }
};
