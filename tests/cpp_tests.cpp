#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../src/controller.hpp"
static int progress=0,voiceCount[3]={},tones=0,checks=0;
extern "C" int host_progress(){return progress;}
extern "C" int host_save_progress(int p){progress=p;return 1;}
extern "C" int host_load_map(char*,int){return 0;}
extern "C" int host_save_map(const char*,int){return 1;}
extern "C" void host_voice(int n){if(n>=0&&n<3)voiceCount[n]++;}
extern "C" void host_tone(int,int){tones++;}
static Game g;
static void check(bool b,const char* s){checks++;if(!b){std::fprintf(stderr,"FAIL: %s\n",s);std::exit(1);}}
static void advance(float s){for(int i=0;i<int(s*100+.5f);i++)g.update(.01f);}
static void capture(const char* name){g.render();char path[160];std::snprintf(path,sizeof path,"build/%s.rgba",name);auto f=std::fopen(path,"wb");check(f!=nullptr,"capture opens");std::fwrite(g.r.pixels,4,WIDTH*HEIGHT,f);std::fclose(f);}
static void tap(int key){g.key(key,true);g.key(key,false);}
static bool bypass(Room post,Room from,Room to){
 constexpr int R=6,W=R*2+1;bool seen[W*W]={};Room queue[W*W];int head=0,tail=0;queue[tail++]=from;seen[(from.z-post.z+R)*W+from.x-post.x+R]=true;
 while(head<tail){Room p=queue[head++];if(sameRoom(p,to))return true;for(int d=0;d<4;d++){Room n{p.x+DX[d],p.z+DZ[d]};int x=n.x-post.x+R,z=n.z-post.z+R;if(x<0||z<0||x>=W||z>=W||sameRoom(n,post)||seen[z*W+x]||!g.world.linked(p,n))continue;seen[z*W+x]=true;queue[tail++]=n;}}
 return false;
}
int main(){
 g.init();capture("title-start");check(g.titleLetters==0&&g.buttonCount==1,"initial cover only brand and music icon");
 tap(13);check(g.curtain<0,"premature input does not bypass lettering");advance(.8f);check(g.titleLetters==1,"first letter arrives");capture("title-letter");advance(2.25f);check(g.titleLetters==7&&tones==7,"seven letters each trigger a beat");capture("title");check(g.buttonCount==2,"ready title only entry hint and sound");
 tap(32);advance(.6f);check(g.state==TITLE&&g.curtain>0,"curtain is gradual");capture("curtain");advance(.65f);check(g.state==STAGE_SELECT,"curtain reveals stage selection");capture("stages");
 tap(39);tap(39);tap(39);check(g.stagePage==1,"right arrow navigates cards across stage pages");capture("stages-middle");g.action(13);g.action(13);check(g.stagePage==2,"mouse next page clamps");capture("stages-final");g.action(12);check(g.stagePage==1,"mouse previous page goes back");
 float previous=0;int closed=0,open=0,sideLinks=0;
 for(int s=0;s<STAGE_COUNT;s++){
  g.setupWorld(s);check(g.goalDistance()>previous,"difficulty distance increases");previous=g.goalDistance();check(g.goalDistance()>45&&g.goalDistance()<185,"compact trophy distance suits horde combat");Room goal=g.world.room(g.goalX+1,g.goalZ+1);check(g.world.exits(goal)==1,"trophy retains its final doorway");check(g.guardPostCount>=3,"compact route still has guard posts");InfiniteMaze repeat;repeat.seed=stages[s].seed;
  int detours=0;for(int i=0;i<g.guardPostCount;i++){Room post=g.guardPosts[i],child=goal;while(!sameRoom(g.world.parent(child),post)&&g.world.depth(child)>g.world.depth(post))child=g.world.parent(child);if(bypass(post,g.world.parent(post),child))detours++;}
  std::printf("Stage %d: %d/%d guard posts have a nearby bypass\n",s+1,detours,g.guardPostCount);check(detours*2>=g.guardPostCount,"at least half of posts can be bypassed locally");
  for(int z=-20;z<=20;z++)for(int x=-20;x<=20;x++){
   Room a{x,z},p=g.world.parent(a);check(sameRoom(p,repeat.parent(a)),"generation deterministic");if(x||z)check(g.world.depth(p)==g.world.depth(a)-1,"every parent moves toward origin");check(g.world.fits(x*4-1,z*4-1),"room center holds character");
   bool joined=g.world.linked(a,{x+1,z});check(joined==g.world.linked({x+1,z},a),"side links symmetric");if(joined&&!g.world.treeLinked(a,{x+1,z}))sideLinks++;check(g.world.segmentBlocked(x*4,z*4,(x+1)*4,z*4)!=joined,"ray blocked exactly at closed panel");check(g.world.fits(x*4+1,z*4-1)==joined,"body cannot straddle panel");joined?open++:closed++;
   Room next=g.world.next(a,{54,-46});check(g.world.linked(a,next),"route helper follows opening");
  }
 }
 check(closed>100&&open>100&&sideLinks>1000,"many real side links, with walls retained");g.cleared=0;g.state=STAGE_SELECT;g.stagePage=0;g.action(108);check(g.state==LOADOUT&&g.pendingStage==8,"all nine stages open without clears");g.state=STAGE_SELECT;
 g.action(100);capture("loadout");g.action(300);check(g.state==LOADOUT,"explicit skill required");g.action(200);g.action(300);check(g.state==INTRO,"selection starts intro");advance(1);capture("intro");check(voiceCount[1]==1,"conference voice once");g.action(7);check(g.state==PLAYING,"skip enters game");check(g.player.x==-1&&g.player.z==-1&&g.enemyCount>=1,"origin spawn and nearby guard posts");capture("world");
 g.key(87,true);int first=g.player.z;advance(.6f);check(g.player.z<first-2,"held W walks continuously");g.key(87,false);first=g.player.z;advance(.2f);check(g.player.z==first,"released W stops");tap(80);float time=g.worldTime;advance(1);check(g.worldTime==time,"pause freezes spawning");g.action(8);
 // Isolated actor fixture verifies that a guard is a real physical blocker.
 g.player.x=g.player.z=-1;g.enemyCount=1;g.enemies[0]=Unit{};g.enemies[0].x=3;g.enemies[0].z=-1;
 check(g.step(g.player,1)&&g.step(g.player,1),"player can approach guard");check(!g.step(g.player,1),"player cannot walk through living guard");g.enemies[0].dead=true;check(g.step(g.player,1),"defeating guard opens passage");
 g.start(0,0,false);check(g.trophyProgress()==0,"trophy bar starts empty");g.player.x=roundi((-1+g.goalX)*.5f);g.player.z=roundi((-1+g.goalZ)*.5f);check(absf(g.trophyProgress()-.5f)<.02f,"half straight-line distance means fifty percent");float half=g.trophyProgress();g.player.x=g.player.z=-10;check(g.trophyProgress()<half,"moving away reduces progress");g.player.x=g.goalX;g.player.z=g.goalZ;check(g.trophyProgress()==1,"arrival is one hundred percent");
 g.start(0,0,false);g.enemyCount=0;g.guardPostCount=0;g.waveClock=WAVE_INTERVAL-.1f;advance(.05f);check(g.enemyCount==0,"no wave before meter fills");advance(.08f);check(g.enemyCount==WAVE_SIZE&&g.waveNumber==1&&g.waveClock<.1f,"one full meter creates eight rivals together");
 int wx[WAVE_SIZE],wz[WAVE_SIZE];for(int i=0;i<WAVE_SIZE;i++){auto& e=g.enemies[i];wx[i]=e.x;wz[i]=e.z;Point p=g.project(e.x+1,.04f,e.z+1);check(e.spawnFlash>1&&p.x>=40&&p.x<=WIDTH-40&&p.y>100&&p.y<HEIGHT-80,"spawn warning appears inside viewport");g.hurt(e,10);check(e.hp==1,"warning enemy cannot be damaged");g.fire(e,true);}check(g.ballCount==0,"warning enemies cannot fire");capture("wave-warning");
 tap(80);time=g.waveClock;float flash=g.enemies[0].spawnFlash;advance(.3f);check(g.waveClock==time&&g.enemies[0].spawnFlash==flash,"pause freezes both wave bar and flashing");g.action(8);advance(.8f);for(int i=0;i<WAVE_SIZE;i++)check(g.enemies[i].x==wx[i]&&g.enemies[i].z==wz[i],"flashing enemies do not chase");advance(.7f);for(int i=0;i<WAVE_SIZE;i++)check(g.enemies[i].spawnFlash==0,"warning ends before active pursuit");check(g.idaCalls>0,"activated wave opponents run IDA star");
 g.enemyCount=MAX_NEARBY_RIVALS;for(int i=0;i<g.enemyCount;i++){g.enemies[i]=Unit{};g.enemies[i].x=g.player.x+6;g.enemies[i].z=g.player.z+i;}int wave=g.waveNumber;g.waveClock=WAVE_INTERVAL;g.updateReinforcements(.01f);check(g.waveNumber==wave&&g.waveClock==WAVE_INTERVAL,"crowded area holds full meter instead of piling on rivals");
 int widths[]={320,960,1600};for(int width:widths){WIDTH=width;HEIGHT=width==320?568:900;g.start(0,0,false);g.enemyCount=0;g.guardPostCount=0;check(g.launchWave(),"viewport-sized horde fits narrow and wide screens");}WIDTH=960;HEIGHT=720;g.follow(true);
 g.cleared=PROGRESS_MASK;g.state=STAGE_SELECT;g.stagePage=0;capture("stages-unlocked");for(int s=0;s<STAGE_COUNT;s++){check(g.start(s,s%SKILL_COUNT,false),"unlocked stage starts");g.charge=100;check(g.special(),"four freely available specials fire");check(g.charge==0&&!g.special(),"charge consumed once");}
 g.state=STAGE_SELECT;g.stagePage=2;capture("maguire-card");g.cleared=255;check(g.unlocked(FINAL_STAGE),"old eight-clear save unlocks Maguire");g.cleared=127;check(g.unlocked(FINAL_STAGE),"Maguire open regardless of previous clears");g.cleared=PROGRESS_MASK;
 g.start(FINAL_STAGE,3,false);g.enemyCount=g.guardPostCount=0;g.player.x=g.goalX;g.player.z=g.goalZ;g.updateBoss(.01f);g.update(.01f);check(g.state==PLAYING&&g.finalBoss()&&!g.bossDefeated&&!g.trophyUnlocked(),"undefeated Maguire keeps the trophy locked");int bossCount=g.enemyCount;g.updateBoss(.01f);check(g.enemyCount==bossCount,"only one final boss spawns");
 Room entrance=g.world.parent(g.world.room(g.goalX+1,g.goalZ+1));g.player.x=entrance.x*4-1;g.player.z=entrance.z*4-1;g.follow(true);advance(2);check(g.finalBoss()&&g.finalBoss()->hp==BOSS_HP,"boss has dedicated final-stage HP");
 for(int i=0;i<g.enemyCount;i++)if(g.enemies[i].boss)g.hurt(g.enemies[i],BOSS_HP/2);g.updateBoss(.01f);check(g.bossRage,"half health triggers phase two");g.bossTimer=0;g.updateBoss(.01f);check(g.bossWindup>0,"boss strike has visible warning");capture("maguire-boss");tap(80);float windup=g.bossWindup;advance(.5f);check(g.bossWindup==windup,"pause freezes boss telegraph");g.action(8);advance(WARNING_END+.05f);check(g.launchWave(),"boss duel keeps the reinforcement horde active");
 for(int i=0;i<g.enemyCount;i++)if(g.enemies[i].boss)g.hurt(g.enemies[i],100);check(g.bossDefeated&&!g.bossEncounterActive()&&g.bossWindup==0,"boss defeat cancels strike and opens trophy");g.player.x=g.goalX;g.player.z=g.goalZ;g.update(.01f);check(g.state==OUTRO,"boss defeated plus trophy reached starts celebration");
 g.start(0,0,false);g.beginOut(true);Camera saved=g.cam;capture("exit");g.cinema=1.7f;capture("fade");g.cinema=3.1f;capture("trophy");g.cinema=4.25f;capture("place");g.cinema=5.5f;capture("run");g.cinema=6.8f;capture("jump");g.cinema=7.9f;capture("land");check(g.lastArmCount==2,"close-up only two arms");check(g.cam.x==saved.x&&g.cam.y==saved.y,"render preserves camera");
 int spoken=voiceCount[2];g.cinema=0;advance(OUTRO_SHOUT-.05f);check(voiceCount[2]==spoken,"shout waits for landing");advance(.2f);check(voiceCount[2]==spoken+1,"shouts once");
 g.start(0,0,false);WIDTH=1600;HEIGHT=900;g.follow(true);capture("wide");g.state=TITLE;g.curtain=-1;g.render();for(int i=0;i<g.buttonCount;i++){auto b=g.buttons[i];check(b.x>=0&&b.y>=0&&b.x+b.w<=WIDTH&&b.y+b.h<=HEIGHT,"responsive controls fit");}WIDTH=960;HEIGHT=720;char n[16];number(n,-123);check(std::strcmp(n,"-123")==0,"negative coordinates readable");std::printf("PASS: %d C++ checks\n",checks);
}
