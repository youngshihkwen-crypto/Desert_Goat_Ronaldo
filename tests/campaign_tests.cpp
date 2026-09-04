#include <cstdio>
#include "../src/controller.hpp"
static int progress=0;
extern "C" int host_progress(){return progress;}
extern "C" int host_save_progress(int p){progress=p;return 1;}
extern "C" int host_load_map(char*,int){return 0;}
extern "C" int host_save_map(const char*,int){return 1;}
extern "C" void host_voice(int){}
extern "C" void host_tone(int,int){}
static Game g;
static IdaStar localRoute;
static constexpr int GRID=400;
static int roomDistance[GRID*GRID],roomQueue[GRID*GRID],leftRoom,topRoom,mapWidth,mapHeight;
static void makeRoute(){Room goal=g.world.room(g.goalX+1,g.goalZ+1);leftRoom=mini(0,goal.x)-12;topRoom=mini(0,goal.z)-12;mapWidth=int(absf(float(goal.x)))+25;mapHeight=int(absf(float(goal.z)))+25;for(int i=0;i<mapWidth*mapHeight;i++)roomDistance[i]=-1;int head=0,tail=0,end=(goal.z-topRoom)*mapWidth+goal.x-leftRoom;roomQueue[tail++]=end;roomDistance[end]=0;
 while(head<tail){int p=roomQueue[head++];Room a{p%mapWidth+leftRoom,p/mapWidth+topRoom};for(int d=0;d<4;d++){Room b{a.x+DX[d],a.z+DZ[d]};int x=b.x-leftRoom,z=b.z-topRoom;if(x<0||z<0||x>=mapWidth||z>=mapHeight||!g.world.linked(a,b))continue;int k=z*mapWidth+x;if(roomDistance[k]<0){roomDistance[k]=roomDistance[p]+1;roomQueue[tail++]=k;}}}
}
static Room routeNext(Room from,Room goal){Room result=g.world.next(from,goal);int best=100000;for(int d=0;d<4;d++){Room n{from.x+DX[d],from.z+DZ[d]};int x=n.x-leftRoom,z=n.z-topRoom;if(x<0||z<0||x>=mapWidth||z>=mapHeight||!g.world.linked(from,n))continue;int value=roomDistance[z*mapWidth+x];if(value<0)continue;int penalty=0;for(int i=0;i<g.enemyCount;i++)if(!g.enemies[i].dead&&length(float(g.enemies[i].x-(n.x*4-1)),float(g.enemies[i].z-(n.z*4-1)))<5)penalty+=2;value=value*10+mini(penalty,8);if(value<best){best=value;result=n;}}return result;}
static int evadeStrike(){
 struct Node{int x,z,first;};Node queue[289];bool seen[289]={};int head=0,tail=0;queue[tail++]={g.player.x,g.player.z,-1};seen[144]=true;
 while(head<tail){Node p=queue[head++];if(p.first>=0&&!g.bossThreatens(p.x+1,p.z+1))return p.first;
  for(int d=0;d<4;d++){int x=p.x+DX[d],z=p.z+DZ[d],rx=x-g.player.x+8,rz=z-g.player.z+8;if(rx<0||rz<0||rx>=17||rz>=17||seen[rz*17+rx]||g.blocked(x,z))continue;bool free=true;for(int i=0;i<g.enemyCount;i++)if(!g.enemies[i].dead&&g.enemies[i].spawnFlash<=0&&absf(float(x-g.enemies[i].x))<2&&absf(float(z-g.enemies[i].z))<2)free=false;if(!free)continue;seen[rz*17+rx]=true;queue[tail++]={x,z,p.first<0?d:p.first};}
 }return -1;
}
int main(){
 auto report=std::fopen("build/campaign-proof.json","wb");if(!report)return 1;std::fprintf(report,"[\n");
 // Normal-input horde campaign: armband, directional shield and Morgan rescue.
 // This proves a feasible combat
 // strategy, not that every loadout or slower reaction time must win.
 for(int campaign=0;campaign<1;campaign++){
  progress=0;g.cleared=0;g.muted=true;
  for(int level=0;level<STAGE_COUNT;level++){
   g.state=STAGE_SELECT;g.action(100+level);g.action(200);g.action(300);g.action(7);if(g.state!=PLAYING)return 2;makeRoute();
   char route[8192]={};int count=0;float elapsed=0,dodgeUntil=0;int keys[4]={87,68,83,65};const char* letters="wdsa";
   while((g.state==PLAYING||g.state==WARNING)&&elapsed<600&&count<8190){
    if(g.state==WARNING){g.update(.016f);elapsed+=.016f;continue;}
    Room a=g.world.room(g.player.x+1,g.player.z+1),b=g.world.room(g.goalX+1,g.goalZ+1);int tx=g.goalX,tz=g.goalZ;
    if(!sameRoom(a,b)){Room n=routeNext(a,b);tx=n.x*4-1;tz=n.z*4-1;if(n.x!=a.x&&g.player.z!=a.z*4-1){tx=g.player.x;tz=a.z*4-1;}else if(n.z!=a.z&&g.player.x!=a.x*4-1){tz=g.player.z;tx=a.x*4-1;}}
    int dir=tx!=g.player.x?(tx>g.player.x?1:3):tz!=g.player.z?(tz>g.player.z?2:0):-1;
    // Engage visible interceptors instead of blindly shooting toward the goal.
    float nearest=6;bool combat=false;
    for(int e=0;e<g.enemyCount;e++){auto& enemy=g.enemies[e];float dist=length(float(enemy.x-g.player.x),float(enemy.z-g.player.z));if(!enemy.dead&&enemy.spawnFlash<=0&&dist<nearest&&g.clearLine(g.player.x+1,g.player.z+1,enemy.x+1,enemy.z+1)&&(absf(float(enemy.x-g.player.x))<=1||absf(float(enemy.z-g.player.z))<=1)){int aim=absf(float(enemy.x-g.player.x))>=absf(float(enemy.z-g.player.z))?(enemy.x>g.player.x?1:3):(enemy.z>g.player.z?2:0);if(g.blocked(g.player.x+DX[aim],g.player.z+DZ[aim]))continue;dir=aim;nearest=dist;combat=true;}}
    for(int i=0;i<g.ballCount;i++){auto& ball=g.balls[i];if(!ball.enemy||ball.dead)continue;float dx=g.player.x+1-ball.x,dz=g.player.z+1-ball.z,along=dx*ball.dx+dz*ball.dz,cross=dx*ball.dz-dz*ball.dx;
     if(along<0||along>4||absf(cross)>.9f)continue;int choices[2]={absf(ball.dx)>.5f?0:1,absf(ball.dx)>.5f?2:3};
     for(int d:choices){int nx=g.player.x+DX[d],nz=g.player.z+DZ[d];bool free=!g.blocked(nx,nz);for(int e=0;e<g.enemyCount;e++)if(!g.enemies[e].dead&&absf(float(nx-g.enemies[e].x))<2&&absf(float(nz-g.enemies[e].z))<2)free=false;if(free){dir=d;combat=false;dodgeUntil=elapsed+2;break;}}
    }
    if(g.bossWindup>0){dodgeUntil=0;if(g.bossThreatens(g.player.x+1,g.player.z+1)){int escape=evadeStrike();if(escape>=0){dir=escape;combat=false;}}else{dir=g.player.dir;combat=true;}}
    if(dir<0&&g.bossEncounterActive()){const Unit* boss=g.finalBoss();int best=-1;float away=-1;for(int d=0;d<4;d++){int x=g.player.x+DX[d],z=g.player.z+DZ[d];if(g.blocked(x,z))continue;float distance=boss?length(float(x-boss->x),float(z-boss->z)):0;if(distance>away){away=distance;best=d;}}dir=best;}
    if(dir<0){std::fprintf(stderr,"No route direction\n");return 3;}
    if(!combat&&g.blocked(g.player.x+DX[dir],g.player.z+DZ[dir])){auto local=localRoute.find({g.player.x,g.player.z},{tx,tz},[&](Room,Room q){return !g.blocked(q.x,q.z);},10000);if(!local.complete||local.length<1){std::fprintf(stderr,"No local route stage %d at %d,%d\n",level+1,g.player.x,g.player.z);return 3;}dir=local.next.x!=g.player.x?(local.next.x>g.player.x?1:3):(local.next.z>g.player.z?2:0);}
    // Keep moving after dodging: the horde aims at the last visible position.
    g.key(67,combat);if(!combat||g.player.dir!=dir){g.key(keys[dir],true);g.key(keys[dir],false);route[count++]=letters[dir];}g.key(32,true);
    for(int e=0;e<g.enemyCount;e++)if(g.charge>=100&&length(g.player.x-g.enemies[e].x,g.player.z-g.enemies[e].z)<5){g.key(67,false);g.key(86,true);g.key(86,false);break;}
    if(g.superUnlocked()&&!g.superUsed&&(g.lives<=1||(g.bossCasts>0&&g.finalBoss()&&g.finalBoss()->hp<=12))){g.key(66,true);g.key(66,false);while(g.state==MORGAN){g.update(.016f);elapsed+=.016f;}}
    // Normal input only during play. No HP, position, enemy or cooldown writes.
    int frames=9;for(int f=0;f<frames&&g.state==PLAYING;f++){g.update(.016f);elapsed+=.016f;}
   }
   if(g.state!=OUTRO){std::fprintf(stderr,"FAILED campaign %d stage %d HP %d at %d,%d %.1fs boss HP %d\n",campaign,level+1,g.lives,g.player.x,g.player.z,elapsed,g.finalBoss()?g.finalBoss()->hp:0);return 4;}
   for(int f=0;f<700&&g.state==OUTRO;f++)g.update(.016f);while(g.state==MORGAN_UNLOCK)g.update(.016f);if(g.state!=RESULT||!g.won||progress!=((1<<(level+1))-1))return 5;
   if(g.kills<3||g.waveNumber<1||g.idaCompleted<1){std::fprintf(stderr,"Combat, waves or IDA not exercised\n");return 6;}
   // A lethal Morgan critical may skip rage; the threshold is checked separately.
   // Trophy victory does not require killing the big boss.
   std::printf("PASS %s stage %d %-13s HP=%d steps=%d time=%.1fs guards=%d kills=%d distance=%d IDA=%d/%d\n",skills[g.equipped].name,level+1,stages[level].name,g.lives,count,elapsed,g.encounters,g.kills,g.goalDistance(),g.idaCompleted,g.idaCalls);
   if(campaign==0)std::fprintf(report,"%s{\"stage\":%d,\"name\":\"%s\",\"skill\":\"%s\",\"hp\":%d,\"seconds\":%.2f,\"distance\":%d,\"guards\":%d,\"kills\":%d,\"waves\":%d,\"idaCompleted\":%d,\"idaCalls\":%d,\"bossDefeated\":%s,\"directionInputs\":\"%s\"}",level?",\n":"",level+1,stages[level].name,skills[g.equipped].name,g.lives,elapsed,g.goalDistance(),g.encounters,g.kills,g.waveNumber,g.idaCompleted,g.idaCalls,g.bossDefeated?"true":"false",route);
  }
 }
 std::fprintf(report,"\n]\n");std::fclose(report);return 0;
}
