#pragma once
#include "pixel.hpp"
// A connected tree backbone plus deterministic side links for escape loops.
// Rooms remain 4x4, separated by zero-footprint panels; the trophy retains
// one doorway, but routes leading to it may branch and rejoin freely.
inline int floorDiv(int n,int d){int q=n/d;return q-(n%d<0);}
struct Room{int x,z;};
inline bool sameRoom(Room a,Room b){return a.x==b.x&&a.z==b.z;}
struct InfiniteMaze{
 u32 seed=27183;
 Room protectedGoal{0,0};bool protectGoal=false;
 u32 hash(int x,int z)const{u32 h=seed^u32(x)*0x9e3779b9u^u32(z)*0x85ebca6bu;h^=h>>16;h*=0x7feb352du;h^=h>>15;h*=0x846ca68bu;return h^(h>>16);}
 Room room(int x,int z)const{return{floorDiv(x+2,4),floorDiv(z+2,4)};}
 Room parent(Room p)const{if(!p.x&&!p.z)return p;bool horizontal=p.x&&(p.z==0||(hash(p.x,p.z)&1));if(horizontal)p.x+=p.x>0?-1:1;else p.z+=p.z>0?-1:1;return p;}
 bool treeLinked(Room a,Room b)const{return !sameRoom(a,b)&&(sameRoom(parent(a),b)||sameRoom(parent(b),a));}
 bool linked(Room a,Room b)const{
  if(int(absf(float(a.x-b.x))+absf(float(a.z-b.z)))!=1)return false;
  if(treeLinked(a,b))return true;
  if(protectGoal&&(sameRoom(a,protectedGoal)||sameRoom(b,protectedGoal)))return false;
  // Canonical undirected edge: querying from either side yields the same wall.
  if(a.x>b.x||a.x==b.x&&a.z>b.z){Room t=a;a=b;b=t;}
  return hash(a.x*2+(a.x==b.x?1:0),a.z*2)%100<38;
 }
 int treeExits(Room p)const{int n=0;const Room neighbors[4]={{p.x+1,p.z},{p.x-1,p.z},{p.x,p.z+1},{p.x,p.z-1}};for(auto q:neighbors)if(treeLinked(p,q))n++;return n;}
 int exits(Room p)const{int n=0;const Room neighbors[4]={{p.x+1,p.z},{p.x-1,p.z},{p.x,p.z+1},{p.x,p.z-1}};for(auto q:neighbors)if(linked(p,q))n++;return n;}
 Room trophyLeaf(Room target)const{
  // Keep the goal's final doorway stable while opening loops elsewhere.
  for(int radius=0;radius<=24;radius++)for(int dz=-radius;dz<=radius;dz++)for(int dx=-radius;dx<=radius;dx++){
   if(maxi(int(absf(float(dx))),int(absf(float(dz))))!=radius)continue;
   Room p{target.x+dx,target.z+dz};if(treeExits(p)==1)return p;
  }
  return target;
 }
 bool wall(int,int)const{return false;} // Panels occupy boundaries, never cells.
 bool joinedTiles(int x,int z,int tx,int tz)const{Room a=room(x,z),b=room(tx,tz);return sameRoom(a,b)||linked(a,b);}
 bool fits(int x,int z)const{for(int i=0;i<2;i++)if(!joinedTiles(x,z+i,x+1,z+i)||!joinedTiles(x+i,z,x+i,z+1))return false;return true;}
 bool segmentBlocked(float x,float z,float tx,float tz)const{
  float dx=tx-x,dz=tz-z;
  if(absf(dx)>.00001f){int lo=floorDiv(int(minf(x,tx))-4,4)-1,hi=floorDiv(int(maxf(x,tx))+4,4)+1;for(int k=lo;k<=hi;k++){float at=k*4-2,t=(at-x)/dx;if(t<0||t>1)continue;float zz=z+dz*t;int rz=int(__builtin_floorf((zz+2)/4));if(!linked({k-1,rz},{k,rz}))return true;}}
  if(absf(dz)>.00001f){int lo=floorDiv(int(minf(z,tz))-4,4)-1,hi=floorDiv(int(maxf(z,tz))+4,4)+1;for(int k=lo;k<=hi;k++){float at=k*4-2,t=(at-z)/dz;if(t<0||t>1)continue;float xx=x+dx*t;int rx=int(__builtin_floorf((xx+2)/4));if(!linked({rx,k-1},{rx,k}))return true;}}
  return false;
 }
 int depth(Room p)const{return int(absf(float(p.x))+absf(float(p.z)));}
 Room next(Room from,Room to)const{
  if(sameRoom(from,to))return to;Room a=from,b=to,child=b;
  for(int step=0;step<2048&&!sameRoom(a,b);step++){
   if(depth(a)>=depth(b))a=parent(a);else{child=b;b=parent(b);}
  }
  return sameRoom(a,from)&&sameRoom(a,b)?child:parent(from);
 }
};
constexpr int MAX_WORLD_RIVALS=128,WAVE_SIZE=8,MAX_NEARBY_RIVALS=32;
constexpr float WAVE_INTERVAL=6,SPAWN_WARNING=1.4f;
