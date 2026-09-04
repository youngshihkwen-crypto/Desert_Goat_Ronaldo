#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../src/controller.hpp"
static int saved=0,checks=0;static Game g;
extern "C" int host_progress(){return saved;}
extern "C" int host_save_progress(int p){saved=p;return 1;}
extern "C" void host_voice(int){}
extern "C" void host_tone(int,int){}
static void check(bool ok,const char* label){checks++;if(!ok){std::fprintf(stderr,"FAIL menus/warnings: %s\n",label);std::exit(1);}}
static void advance(float seconds){for(int i=0;i<int(seconds*100+.5f);i++)g.update(.01f);}
static void tap(int code){g.key(code,true);g.key(code,false);}
static void click(int id){g.render();for(int i=0;i<g.buttonCount;i++){auto b=g.buttons[i];if(b.id==id){check(b.enabled,"clicked control enabled");g.pointer(0,b.x+b.w/2,b.y+b.h/2);return;}}check(false,"requested mouse control exists");}
static void capture(const char* name){g.render();char path[160];std::snprintf(path,sizeof path,"build/%s.rgba",name);auto f=std::fopen(path,"wb");check(f!=nullptr,"capture opens");std::fwrite(g.r.pixels,4,WIDTH*HEIGHT,f);std::fclose(f);}
static void bossFixture(int stage){g.cleared=7;g.start(stage,0,false);g.guardPostCount=g.enemyCount=g.ballCount=0;g.world.protectGoal=false;g.goalX=g.goalZ=199;g.player.x=0;g.player.z=-1;g.player.inv=0;g.bossSpawned=true;g.bossTimer=0;g.waveClock=-50;Unit b;b.x=-2;b.z=-1;b.hp=g.bossMaxHp();b.boss=true;b.think=b.cool=100;b.id=1;g.enemies[g.enemyCount++]=b;g.follow(true);}
int main(){
 g.init();g.state=STAGE_SELECT;g.stagePage=0;click(13);check(g.stagePage==1,"mouse next page works");click(13);check(g.stagePage==2,"mouse reaches last page");click(12);check(g.stagePage==1,"mouse previous page works");
 click(103);check(g.state==LOADOUT,"mouse card selects stage");tap(39);tap(13);check(g.pendingSkill==1,"arrows plus enter select a skill");tap(40);tap(13);check(g.state==BOSS_DEMO,"down and enter start selected stage");tap(80);check(g.state==PAUSED,"demo can pause");float cinema=g.cinema;advance(.5f);check(g.cinema==cinema,"demo pause freezes timeline");tap(13);check(g.state==BOSS_DEMO,"enter resumes focused pause button");click(7);check(g.state==PLAYING,"mouse skips demo into play");
 tap(80);g.render();for(int i=0;i<g.buttonCount;i++)check(g.buttons[i].id==8||g.buttons[i].id==4,"paused menu has no hidden HUD controls");click(8);check(g.state==PLAYING,"mouse resumes pause");
 g.finish(false);click(9);check(g.state==LOADOUT,"mouse retries from result");click(203);click(300);check(g.state==BOSS_DEMO,"mouse skill and enter work");
 for(int level=3;level<9;level++){
  g.start(level,0);check(g.state==BOSS_DEMO,"every late stage starts with skill demo");int enemies=g.enemyCount;float energy=g.charge,wave=g.waveClock;advance(4.3f);check(g.state==BOSS_DEMO&&g.enemyCount==enemies&&g.charge==energy&&g.waveClock==wave,"skill tutorial never mutates combat");char name[64];std::snprintf(name,sizeof name,"boss-demo-%d",level+1);capture(name);check(g.lastArmCount==2,"demo uses two-arm poses");advance(2.8f);check(g.state==PLAYING,"skill demo automatically enters play");
  bossFixture(level);g.bossWindup=0;g.updateBoss(.01f);check(g.state==WARNING&&g.warningCount==1&&g.shake>=12,"all special casts enter warning with shake");float x=g.enemies[0].x,z=g.enemies[0].z,windup=g.bossWindup,time=g.clock,world=g.worldTime,charge=g.charge;g.balls[g.ballCount++]={4,0,-1,0,3,true,false};float life=g.balls[0].life;int count=g.ballCount;
  g.key(87,true);g.key(32,true);g.key(86,true);g.key(66,true);advance(.8f);check(g.player.x==0&&g.player.z==-1&&g.enemies[0].x==x&&g.enemies[0].z==z&&g.balls[0].life==life&&g.ballCount==count&&g.bossWindup==windup&&g.clock==time&&g.worldTime==world&&g.charge==charge,"warning freezes map actors projectiles energy and all attacks");g.clearKeys();tap(13);check(!g.warningExiting,"minimum warning reading time enforced");
  tap(80);float warn=g.warningTime;advance(.8f);check(g.warningTime==warn,"P also pauses warning timeline");click(8);advance(.3f);check(g.state==WARNING,"warning remains readable");
  if(level==8){capture("warning-dark");g.shake=0;g.render();int detail=0;for(int py=200;py<530;py++)for(int px=0;px<95;px++)if((g.r.pixels[py*WIDTH+px]&0xffffff)!=0)detail++;check(detail>500,"dark overlay retains map detail outside popup");}
  click(14);check(g.warningExiting&&g.shake>=12,"mouse closes popup with exit shake");advance(.31f);check(g.state==PLAYING&&g.bossWindup==windup,"warning returns to play before telegraph countdown starts");g.update(.01f);check(g.bossWindup<windup,"telegraph resumes after warning");
  g.bossWindup=0;g.bossTimer=0;g.updateBoss(.01f);check(g.state==WARNING&&g.warningCount==2,"subsequent casts also use warning");advance(WARNING_END+.02f);check(g.state==PLAYING,"warning also continues automatically");
 }
 // RGB darkening must keep the red navigation and gold trophy colors, not grayscale.
 g.r.clear(0);g.r.pixels[0]=0xff2040f0u;g.r.fadeToBlack(.45f);u32 color=g.r.pixels[0];check((color&255)>((color>>8)&255)&&((color>>8)&255)>((color>>16)&255),"darkening preserves hue ordering");
 // Seal the major boss, wipe nearby regular rivals (including spawn flashes), preserve far rivals.
 bossFixture(8);g.bossTimer=20;g.enemies[0].cool=.1f;g.enemies[0].think=0;g.lives=1;
 Unit near;near.x=4;near.z=-1;near.hp=1000;near.spawnFlash=1;near.id=2;g.enemies[g.enemyCount++]=near;
 Unit far=near;far.x=30;far.spawnFlash=0;far.id=3;g.enemies[g.enemyCount++]=far;
 Unit visible=near;visible.x=10;visible.z=10;visible.spawnFlash=0;visible.id=4;g.enemies[g.enemyCount++]=visible;
 g.balls[g.ballCount++]={5,0,-1,0,3,true,false};g.balls[g.ballCount++]={31,0,-1,0,3,true,false};g.balls[g.ballCount++]={11,11,-1,0,3,true,false};g.bossWindup=1;g.waveClock=WAVE_INTERVAL;g.waveRetry=.4f;
 check(g.super(),"interview activates once unlocked");g.skip();check(g.enemies[0].seal==2&&g.enemies[0].hp==BOSS_HP&&!g.enemies[0].dead&&g.bossWindup==0,"interview seals without killing major boss");check(g.enemies[1].dead&&!g.enemies[2].dead&&g.balls[0].dead&&!g.balls[1].dead&&g.lives==3,"interview wipes nearby regardless HP or spawn state and heals");
 check(g.enemies[3].dead&&g.balls[2].dead,"interview clears visible rivals and shots beyond the old radius");check(g.waveClock==0&&g.waveRetry==0,"interview resets a full reinforcement meter");
 capture("morgan-sealed");g.enemyCount=1;g.ballCount=0;int sx=g.enemies[0].x,sz=g.enemies[0].z;advance(1);check(g.enemies[0].seal>.99f&&g.enemies[0].x==sx&&g.enemies[0].z==sz&&g.ballCount==0,"sealed boss does not move or fire");tap(80);float seal=g.enemies[0].seal;advance(.5f);check(g.enemies[0].seal==seal,"pause does not consume seal duration");tap(80);advance(.99f);check(g.enemies[0].seal>0&&g.enemies[0].x==sx,"seal lasts full two gameplay seconds");advance(.12f);check(g.enemies[0].seal==0&&(g.enemies[0].x!=sx||g.enemies[0].z!=sz||g.ballCount>0),"boss resumes pursuit or firing after two seconds");check(!g.super(),"interview still once per attempt");
 // First three clears unlock a non-combat premiere exactly once, including automatic playback.
 g.cleared=3;g.start(2,0,false);g.lives=1;g.finish(true);check(g.state==MORGAN_UNLOCK&&g.cleared==7&&saved==7&&!g.superUsed,"third clear starts unlock premiere");advance(.5f);capture("morgan-unlocked");g.skip();check(g.state==RESULT&&g.won&&!g.superUsed&&g.lives==1,"unlock premiere does not consume a charge or apply combat effects");g.start(2,0,false);g.finish(true);check(g.state==RESULT,"replaying a cleared stage never repeats unlock premiere");
 g.cleared=3;g.start(2,0,false);g.finish(true);advance(MORGAN_END+.1f);check(g.state==RESULT&&g.won,"unlock animation also finishes automatically");
 for(int level=3;level<9;level++){g.start(level,0,false);g.enemyCount=g.guardPostCount=0;Room entrance=g.world.parent(g.world.room(g.goalX+1,g.goalZ+1));g.player.x=entrance.x*4-1;g.player.z=entrance.z*4-1;g.updateBoss(.01f);check(g.finalBoss()!=nullptr,"boss spawned for trophy arrival fixture");int d=g.goalX!=g.player.x?(g.goalX>g.player.x?1:3):(g.goalZ>g.player.z?2:0);check(g.step(g.player,d)&&g.step(g.player,d),"can physically touch trophy platform even with boss at its center");g.update(.01f);check(g.state==PLAYING&&!g.bossDefeated&&!g.trophyUnlocked()&&g.trophyProgress()==1,"living boss keeps trophy locked at the platform");for(int i=0;i<g.enemyCount;i++)if(g.enemies[i].boss){g.enemies[i].spawnFlash=0;g.hurt(g.enemies[i],100);}g.update(.01f);check(g.state==OUTRO&&g.bossDefeated&&g.trophyUnlocked(),"killing boss at the platform unlocks victory");}
 WIDTH=1600;HEIGHT=900;bossFixture(8);g.updateBoss(.01f);advance(1.2f);capture("warning-wide");WIDTH=960;HEIGHT=720;
 std::printf("PASS: %d menu, warning, seal and victory checks\n",checks);
}
