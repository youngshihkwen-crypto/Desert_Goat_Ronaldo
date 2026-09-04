#include <cstdio>
#include <cstdlib>
#include "../src/ida_star.hpp"
static IdaStar search;
static void check(bool b,const char* why){if(!b){std::fprintf(stderr,"FAIL: %s\n",why);std::exit(1);}}
static constexpr int W=17,R=8;
static int distance[W*W];static Room queue[W*W];
int main(){int checked=0,pruned=0,iterations=0;
 for(int seed=1;seed<=8;seed++){InfiniteMaze maze;maze.seed=seed*27183;
  auto linked=[&](Room a,Room b){return b.x>=-R&&b.x<=R&&b.z>=-R&&b.z<=R&&maze.linked(a,b);};
  for(int target=0;target<10;target++){
   Room goal{int(maze.hash(target,7)%W)-R,int(maze.hash(target,9)%W)-R};for(int& d:distance)d=-1;int head=0,tail=0;queue[tail++]=goal;distance[(goal.z+R)*W+goal.x+R]=0;
   while(head<tail){Room a=queue[head++];Room neighbors[4]={{a.x+1,a.z},{a.x-1,a.z},{a.x,a.z+1},{a.x,a.z-1}};for(Room b:neighbors){if(!linked(a,b))continue;int p=(b.z+R)*W+b.x+R;if(distance[p]>=0)continue;distance[p]=distance[(a.z+R)*W+a.x+R]+1;queue[tail++]=b;}}
   for(int source=0;source<12;source++){Room start{int(maze.hash(source,2)%W)-R,int(maze.hash(source,4)%W)-R};int optimal=distance[(start.z+R)*W+start.x+R];auto result=search.find(start,goal,linked,200000);check(result.complete,"IDA* completes small reference maze");check(result.length==optimal,"IDA* agrees with independent BFS optimum");if(optimal>0){check(linked(start,result.next),"first step crosses an opening");check(distance[(result.next.z+R)*W+result.next.x+R]==optimal-1,"first step lies on shortest path");}pruned+=search.pruned;iterations+=search.thresholds;checked++;}
  }
 }
 InfiniteMaze maze;auto limited=search.find({0,0},{100,100},[&](Room a,Room b){return maze.linked(a,b);},5);check(!limited.complete&&limited.nodes<=5,"budget exhaustion is explicit, not mislabeled shortest path");
 auto same=search.find({0,0},{0,0},[&](Room a,Room b){return maze.linked(a,b);});check(same.complete&&same.length==0,"start equals target");check(pruned>0&&iterations>checked,"pruning and iterative deepening exercised");std::printf("PASS: IDA* matches BFS on %d cases, %d threshold searches, %d prunes\n",checked,iterations,pruned);
}
