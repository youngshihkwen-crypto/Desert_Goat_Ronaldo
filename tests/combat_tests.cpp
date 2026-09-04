#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../src/controller.hpp"
static int progress=0,checks=0,voices[6]={};
extern "C" int host_progress(){return progress;}
extern "C" int host_save_progress(int p){progress=p;return 1;}
extern "C" int host_load_map(char*,int){return 0;}
extern "C" int host_save_map(const char*,int){return 1;}
extern "C" void host_voice(int n){if(n>=0&&n<6)voices[n]++;}
extern "C" void host_tone(int,int){}
static Game g;
static void check(bool ok,const char* label){checks++;if(!ok){std::fprintf(stderr,"FAIL combat: %s\n",label);std::exit(1);}}
static void advance(float t){for(int i=0;i<int(t*100+.5f);i++)g.update(.01f);}
static void tap(int k){g.key(k,true);g.key(k,false);}
static void capture(const char* name){g.render();char path[128];std::snprintf(path,sizeof path,"build/%s.rgba",name);auto f=std::fopen(path,"wb");check(f!=nullptr,"capture opens");std::fwrite(g.r.pixels,4,WIDTH*HEIGHT,f);std::fclose(f);}
static void fixture(int skill=0){g.cleared=PROGRESS_MASK;check(g.start(0,skill,false),"fixture starts");g.infinite=false;g.campaign=false;for(auto& w:g.walls)w=0;g.enemyCount=g.guardPostCount=g.ballCount=g.effectCount=0;g.player.x=40;g.player.z=9;g.player.dir=1;g.player.inv=0;g.follow(true);}
static Unit& target(int x,int z,int hp=20){auto& e=g.enemies[g.enemyCount++];e=Unit{};e.x=x;e.z=z;e.hp=hp;e.think=e.cool=100;e.id=g.nextEnemyId++;return e;}
int main(){
 g.init();g.cleared=0;
 for(int skill=0;skill<4;skill++){check(g.start(0,skill,false),"all four skills available in first stage without clears");g.state=STAGE_SELECT;g.choose(0);g.action(200+skill);check(g.pendingSkill==skill,"all four loadout cards selectable");g.action(300);check(g.equipped==skill,"selected skill equipped");}
 check(!g.start(0,4,false)&&!g.start(0,-1,false),"removed and invalid skills rejected");
 g.state=STAGE_SELECT;g.choose(0);capture("four-skills");g.action(204);check(g.pendingSkill==-1,"removed card cannot select hidden skill");
 const char* shots[]={"special-armband","special-ronado","special-phone","special-elbow"};
 for(int skill=0;skill<4;skill++){fixture(skill);auto& e=target(43,9);check(g.special(),"special starts");check(g.charge==0&&!g.special(),"one keypress consumes meter once");advance(skill==2?.22f:.18f);capture(shots[skill]);check(g.specialAnim>0&&g.animSkill==skill&&g.lastArmCount==2,"dedicated animation with only two arms");advance(.9f);check(e.hp<20,"every skill damages a forward opponent");}
 for(int skill=0;skill<4;skill++){fixture(skill);g.stage=4;auto& boss=target(43,9,20);boss.boss=true;int before=boss.hp;check(g.special(),"special starts against boss");advance(1.1f);check(boss.hp<before,"every special reduces boss HP");if(skill==0)capture("boss-head-health-bar");}
 fixture();g.stage=7;auto& ragingBoss=target(43,9,9);ragingBoss.boss=true;g.bossRage=true;g.render();int rageRed=0;for(int i=0;i<WIDTH*HEIGHT;i++){u32 c=g.r.pixels[i];int red=c&255,green=(c>>8)&255,blue=(c>>16)&255;if(red>120&&red>green*2&&red>blue*2)rageRed++;}check(rageRed>200,"half-HP Messi and Maguire render as red rage bosses");capture("boss-rage-red");
 fixture(3);auto& front=target(43,9);auto& behind=target(37,9);g.special();check(front.hp==14&&behind.hp==20,"elbow is six damage in front only");
 fixture(2);target(44,9);g.walls[g.index(43,9)]=g.walls[g.index(43,10)]=1;g.special();advance(.5f);check(g.enemies[0].hp==20,"thrown phone respects walls and blast line of sight");
 fixture(1);g.walls[g.index(43,9)]=g.walls[g.index(43,10)]=1;g.special();advance(.6f);check(g.player.x<=41,"ronado stops at wall");
 fixture();int x=g.player.x,z=g.player.z;g.key(67,true);g.key(87,true);advance(.8f);check(g.player.x==x&&g.player.z==z&&g.player.dir==0,"held shield turns without moving");g.key(87,false);capture("shield");g.key(67,false);g.key(87,true);advance(.3f);check(g.player.z<z,"release shield restores held movement");g.key(87,false);
 for(int facing=0;facing<4;facing++)for(int side=0;side<4;side++){fixture();g.player.dir=facing;g.key(67,true);float sx=g.player.x+1+DX[side]*4,sz=g.player.z+1+DZ[side]*4;bool blocked=g.shieldBlocks(sx,sz);check(blocked==(facing==side),"shield only blocks facing direction");g.enemyHit(sx,sz);check(g.lives==(facing==side?3:2),"front hit blocked and sides/back hurt");}
 for(int side=0;side<4;side++){fixture();g.player.dir=1;g.key(67,true);g.balls[g.ballCount++]={float(g.player.x+1)+DX[side]*.7f,float(g.player.z+1)+DZ[side]*.7f,float(-DX[side]),float(-DZ[side]),1,true,false};g.update(.01f);check(g.lives==(side==1?3:2),"actual projectile collision honors directional shield");}
 fixture();g.key(67,true);check(!g.special()&&g.keys[67],"special cannot silently drop a held shield");g.enemyHit(g.player.x+5,g.player.z+1,false);check(g.lives==2,"ground attack bypasses directional shield");g.blur();check(!g.keys[67]&&g.state==PAUSED,"blur clears held shield");
 // Healing happens once at successful wave commitment, not while waiting/retrying.
 g.cleared=0;g.start(0,0,false);g.enemyCount=g.guardPostCount=0;g.lives=1;check(g.launchWave()&&g.lives==2,"new wave heals exactly one");g.enemyCount=0;check(g.launchWave()&&g.lives==3,"second wave heals to max");g.enemyCount=0;check(g.launchWave()&&g.lives==3,"wave cannot over-heal");g.lives=1;g.enemyCount=MAX_NEARBY_RIVALS;for(int i=0;i<g.enemyCount;i++){g.enemies[i]=Unit{};g.enemies[i].x=g.player.x+6;g.enemies[i].z=g.player.z+i;}check(!g.launchWave()&&g.lives==1,"failed wave does not heal");
 fixture();g.cleared=3;check(!g.super(),"interview locked before third clear");g.cleared=7;g.lives=1;g.charge=31;auto& victim=target(44,9);victim.hp=24;g.waveClock=8;g.spawnTimer=8;g.balls[g.ballCount++]={44,10,-1,0,3,true,false};float bx=g.balls[0].x,life=g.balls[0].life;
 g.key(87,true);int movingZ=g.player.z;check(g.super()&&g.state==MORGAN&&g.superUsed&&g.shake==8,"interview opens a short super-move popup with one shake");
 check(!victim.dead&&!g.balls[0].dead&&g.lives==1,"effect waits while the popup freezes combat");advance(.2f);capture("morgan-impact");
 check(g.state==MORGAN&&victim.x==44&&victim.hp==24&&g.balls[0].x==bx&&g.balls[0].life==life&&g.charge==31&&g.waveClock==8,"popup freezes world and keeps its warning frame");
 float popup=g.cinema;tap(80);advance(.3f);check(g.state==PAUSED&&g.cinema==popup,"popup can be paused");tap(80);advance(.85f);
 check(g.state==PLAYING&&victim.dead&&g.balls[0].dead&&g.lives==3&&g.waveClock<.1f&&g.spawnTimer>RIVAL_SPAWN_INTERVAL-.1f&&g.superFlash==0,"popup resolves screen clear, heal and wave reset");
 check(voices[3]==0&&voices[4]==0&&voices[5]==0,"popup has no dialogue");g.key(87,true);advance(.15f);check(g.player.z<movingZ,"movement works immediately after popup");g.key(87,false);check(!g.super(),"effect remains once per stage");
 fixture();g.cleared=7;g.lives=1;target(43,9);check(g.super(),"new attempt resets interview use");advance(.26f);tap(13);check(g.state==PLAYING&&g.superUsed&&g.lives==3&&g.enemies[0].dead,"skip resolves super exactly once");tap(66);check(g.state==PLAYING,"B unavailable after use");
 // Stages four through nine form a monotonic combat difficulty curve.
 for(int level=3;level<9;level++){g.stage=level;check(g.bossMaxHp()==lateBossHp[level-3],"late boss HP follows difficulty table");check(g.waveSize()==lateWaveSize[level-3],"late-stage wave follows tuned difficulty table");if(level>3){g.stage=level-1;int hp=g.bossMaxHp(),cap=g.nearbyLimit();float interval=g.waveInterval(),move=g.bossMovePace(),fire=g.bossFirePace(),skill=g.bossSkillPace();g.stage=level;if(level<=6)check(g.bossMaxHp()>hp&&g.nearbyLimit()>cap&&g.waveInterval()<interval&&g.bossMovePace()<move&&g.bossFirePace()<fire&&g.bossSkillPace()<skill,"stages five through seven increase every pressure axis");else check(g.bossMaxHp()>hp&&g.nearbyLimit()>cap&&g.waveInterval()<=interval,"rage stages retain higher capacity and HP without inflating their base phase");}}
 g.stage=3;check(g.bossMaxHp()==8&&g.waveSize()==3&&g.waveInterval()==10&&g.nearbyLimit()==14,"stage four is the gentle boss introduction");g.stage=4;check(g.bossMaxHp()==12&&g.waveSize()==5&&g.waveInterval()==8.6f&&g.nearbyLimit()==20,"stage five starts the raised middle-game pressure");g.stage=6;check(g.bossMaxHp()==18&&g.waveSize()==7&&g.waveInterval()==6.8f&&g.nearbyLimit()==28,"stage seven reaches the new middle-game peak");g.stage=8;check(g.bossMaxHp()==24&&g.waveSize()==8&&g.waveInterval()==6&&g.nearbyLimit()==32,"stage nine reaches full combat pressure");
 for(int level=6;level<9;level++){fixture();g.stage=level;g.campaign=true;g.infinite=true;g.world.seed=stages[level].seed;g.world.protectGoal=false;g.goalX=199;g.goalZ=199;auto& boss=target(-2,-1,g.bossMaxHp()/2);boss.boss=true;g.bossSpawned=true;g.bossRage=false;float move=g.bossMovePace(),fire=g.bossFirePace(),skill=g.bossSkillPace();g.updateBoss(.01f);if(level==6)check(!g.bossRage,"Pique keeps the established non-rage difficulty");else check(g.bossRage&&g.bossMovePace()<move&&g.bossFirePace()<fire&&g.bossSkillPace()<skill,"Messi and Maguire enrage at half HP with faster pursuit, shots and skills");}
 fixture();g.stage=7;g.campaign=true;g.infinite=true;g.world.seed=stages[7].seed;g.world.protectGoal=false;g.goalX=199;g.goalZ=199;auto& rageCueBoss=target(43,9,g.bossMaxHp()/2);rageCueBoss.boss=true;g.bossSpawned=true;g.bossRage=false;g.updateBoss(.01f);check(g.bossRage&&g.rageFlash==RAGE_FLASH_TIME&&g.rageFlashPulse==0,"half HP starts the three-flash rage window");g.bossTimer=100;g.shake=0;g.render();check(g.r.pixels[260*WIDTH+180]==0xff1818a0u,"rage warning window is red");capture("rage-window-red");advance(.21f);g.shake=0;g.render();check(g.r.pixels[260*WIDTH+180]!=0xff1818a0u,"rage window turns off between flashes");advance(.2f);check(g.rageFlashPulse==1&&g.shake>0,"second rage flash shakes");advance(.4f);check(g.rageFlashPulse==2&&g.shake>0,"third rage flash shakes");advance(.4f);check(g.rageFlash==0&&g.rageFlashPulse==2,"rage warning ends after exactly three flashes");
 // Every late boss owns a distinct telegraphed cast; no early-stage boss gate.
 for(int level=0;level<9;level++){g.cleared=PROGRESS_MASK;g.start(level,0,false);check(g.bossEncounterActive()==(level>=3),"boss gates only stages four onward");}
 for(int level=3;level<9;level++){
  fixture();g.stage=level;g.campaign=true;g.infinite=true;g.world.seed=stages[level].seed;g.world.protectGoal=false;g.goalX=199;g.goalZ=199;g.player.x=0;g.player.z=-1;g.player.dir=1;
  // Same open room keeps the geometry fixture independent of maze topology.
  auto& b=target(-2,-1,g.bossMaxHp());b.boss=true;g.bossSpawned=true;g.bossTimer=0;g.bossDefeated=g.bossRage=false;g.bossWindup=0;g.bossCasts=0;g.updateBoss(.01f);check(g.bossWindup>0,"all late bosses telegraph a special");
  g.follow(true);char name[48];std::snprintf(name,sizeof name,"boss-skill-%d",level+1);capture(name);check(g.state==WARNING,"each boss special freezes combat with warning");advance(WARNING_END+.02f);float windup=g.bossWindup;g.super();check(g.state==MORGAN&&b.seal==0,"interview popup delays its effect");g.skip();check(g.state==PLAYING&&b.seal==2&&g.bossWindup==0,"activating popup cancels cast and seals boss");b.hp=g.bossMaxHp();b.dead=false;b.stun=0;b.seal=0;g.bossDefeated=false;g.player.inv=0;g.lives=3;g.bossWindup=windup;g.bossTargetX=g.player.x+1;g.bossTargetZ=g.player.z+1;
  for(int i=0;i<230&&g.bossCasts==0;i++)g.updateBoss(.01f);check(g.bossCasts==1,"boss cast actually resolves");if(level==3)check(g.ballCount>=5,"Cassano fires five-shot voice wave");if(level==4||level==6)check(g.lives==2,"cross or five-finger ground attack damages");if(level==5)check(g.player.stun>0&&g.charge==75,"substitution stuns and drains charge");if(level==7)check(g.player.stun>0&&g.lives==2,"choke hits close player");if(level==8)check(g.ballCount>=3&&b.dir==(g.bossAttackDir+2)%4,"carrier completes turn and volley");
  g.bossWindup=1;g.hurt(b,100);check(!g.bossEncounterActive()&&g.bossWindup==0,"boss defeat cancels cast and opens goal");
 }
 // Free stage selection does not counterfeit clears or unlock the interview.
 g.cleared=0;for(int level=0;level<9;level++)check(g.start(level,0,false),"all stages open with a new save");check(!g.superUnlocked()&&!g.super(),"open stages do not bypass interview progression");
 g.start(0,0,false);g.guardPostCount=0;g.enemyCount=0;int batches=0;for(int i=0;i<4;i++)if(g.launchWave())batches++;check(batches>=2&&g.enemyCount>=16&&g.enemyCount<=32,"horde fills viewport without exceeding cap");advance(.06f);capture("horde-wave");
 g.start(0,0,false);g.guardPostCount=0;g.enemyCount=0;g.waveClock=-100;auto& pursuer=target(23,-1);pursuer.guard=true;pursuer.think=0;pursuer.homeX=pursuer.x;pursuer.homeZ=pursuer.z;advance(3);check(pursuer.alert&&g.idaCalls>0&&pursuer.gait>=8,"guard actively follows IDA through walls at fast pace");check(length(float(pursuer.x-pursuer.homeX),float(pursuer.z-pursuer.homeZ))>8,"guard leaves former eight-tile leash");
 fixture();auto& struck=target(44,9);g.hurt(struck,1);check(g.shake>=4&&struck.hitFlash>0,"every damaging hit shakes and flashes");float hitShake=g.shake;tap(80);advance(.2f);check(g.shake==hitShake,"pause freezes hit feedback");tap(80);advance(.3f);check(g.shake==0&&struck.hitFlash==0,"hit feedback decays quickly");
 g.beginOut();check(g.shake>=12,"reaching trophy triggers victory shake");g.cinema=OUTRO_SHOUT-.005f;g.spoken=false;g.update(.01f);check(g.shake>=16,"SIUUUUU landing triggers strong shake");g.skip();check(g.state==RESULT&&g.shake>=14,"result triggers final victory beat");
 // Physical C/V/B bindings replace Q/E/F; C remains a hold, never a toggle.
 fixture();tap(81);tap(69);tap(70);check(!g.shielding()&&g.charge==100&&!g.superUsed,"old Q E F bindings no longer activate abilities");
 g.key(67,true);check(g.shielding(),"C down raises shield immediately");tap(86);check(g.charge==100&&g.shielding(),"V cannot drop held C shield");g.key(67,false);check(!g.shielding(),"C up lowers shield immediately");
 tap(86);check(g.charge==0&&g.specialAnim>0,"V activates selected special");g.lives=1;tap(66);check(g.superUsed&&g.lives==1&&g.state==MORGAN,"B opens the super-move popup");advance(.26f);tap(13);check(g.lives==3&&g.state==PLAYING,"Enter activates the interview effect");
 // Every actual HP loss, including the final hit, gets one gray wash + jolt.
 fixture();g.damage();check(g.lives==2&&g.damageFlash==DAMAGE_FLASH_TIME&&g.shake>=7,"HP loss triggers gray wash and shake");capture("damage-flash");
 advance(.1f);float feedback=g.damageFlash;g.damage();check(g.lives==2&&g.damageFlash==feedback,"invulnerable hit does not retrigger damage feedback");
 tap(80);advance(.5f);check(g.damageFlash==feedback,"pause freezes feedback");tap(80);advance(.3f);check(g.damageFlash==0,"damage wash fades out quickly");
 g.key(87,true);int damageZ=g.player.z;g.player.inv=0;g.damage();advance(.15f);check(g.lives==1&&g.player.z<damageZ&&g.state==PLAYING,"second hit retriggers feedback without freezing movement");g.key(87,false);
 g.player.inv=0;g.damage();check(g.state==RESULT&&g.lives==0&&g.damageFlash>0&&g.shake>=7,"fatal hit still grays and shakes");advance(.4f);check(g.damageFlash==0,"fatal-hit gray wash fades on result screen");
 fixture();g.key(67,true);g.enemyHit(g.player.x+5,g.player.z+1);check(g.damageFlash==0,"blocked shield hit does not gray the screen");
 g.r.clear(0);g.r.pixels[0]=0xff2040f0u;g.r.damageTint(1);u32 tint=g.r.pixels[0];check((tint&255)<240&&int(tint&255)-int((tint>>16)&255)<80&&(tint&0xffffff)!=0,"damage wash dims and desaturates while preserving image");
 // Pixel translation must work in all directions without overwriting source rows.
 WIDTH=7;HEIGHT=5;for(int dx=-1;dx<=1;dx++)for(int dy=-1;dy<=1;dy++){for(int i=0;i<35;i++)g.r.pixels[i]=0xff000100u+i;g.r.shift(dx,dy);for(int y=0;y<5;y++)for(int x=0;x<7;x++){int sx=x-dx,sy=y-dy;u32 expected=sx>=0&&sx<7&&sy>=0&&sy<5?0xff000100u+sy*7+sx:0xff000000u;check(g.r.pixels[y*7+x]==expected,"screen shake translates pixels safely");}}WIDTH=960;HEIGHT=720;
 WIDTH=1600;HEIGHT=900;fixture();g.cleared=7;g.super();g.cinema=7;capture("morgan-wide");WIDTH=960;HEIGHT=720;
 std::printf("PASS: %d combat and cinematic checks\n",checks);
}
