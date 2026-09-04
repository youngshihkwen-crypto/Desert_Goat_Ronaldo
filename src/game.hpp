#pragma once
#include "pixel.hpp"
#include "infinite_maze.hpp"
#include "ida_star.hpp"
extern "C" int host_progress();
extern "C" int host_save_progress(int);
extern "C" void host_voice(int);
extern "C" void host_tone(int,int);
constexpr int COLS=91,ROWS=21,CELLS=COLS*ROWS;
constexpr int STAGE_COUNT=9,PROGRESS_MASK=(1<<STAGE_COUNT)-1,FINAL_STAGE=8,BOSS_HP=24;
constexpr int SKILL_COUNT=4,MAX_LIVES=3;
constexpr float MORGAN_END=1.2f,MORGAN_POPUP_END=1.0f,DAMAGE_FLASH_TIME=.32f,RAGE_FLASH_TIME=1.2f;
constexpr float WARNING_HOLD=2.5f,WARNING_END=2.8f,WARNING_MIN_READ=.9f;
struct Skill{const char* name;const char* hint;float recharge;};
static constexpr Skill skills[SKILL_COUNT]={{"ARMBAND SLAM","CLEAR / PUSH",14},{"RONADO","SPIN / DASH",18},{"PHONE SMASH","THROW / STUN",16},{"ELBOW STRIKE","FRONT / BREAK",12}};
static constexpr const char* bossSkills[STAGE_COUNT]={"","","","TRASH TALK","OVERTHINKING","SUBSTITUTION","FIVE FINGERS","CHOKEHOLD","AIRCRAFT CARRIER TURN"};
static constexpr int lateBossHp[6]={8,12,15,18,19,24};
static constexpr int lateWaveSize[6]={3,5,6,7,7,8};
static constexpr float lateWaveInterval[6]={10.f,8.6f,7.6f,6.8f,6.8f,6.f};
static constexpr int lateNearbyLimit[6]={14,20,24,28,30,32};
static constexpr float lateBossMove[6]={.48f,.41f,.36f,.32f,.336f,.30f};
static constexpr float lateRivalMove[6]={.34f,.30f,.27f,.24f,.26f,.24f};
static constexpr float lateRivalFire[6]={4.8f,4.1f,3.7f,3.3f,3.68f,3.4f};
static constexpr float lateBossFire[6]={3.6f,3.f,2.65f,2.3f,2.64f,2.4f};
static constexpr float lateBossSkill[6]={10.f,8.2f,6.8f,5.6f,6.f,5.f};
struct BossWarning{const char* what;const char* effect;const char* dodge;};
static constexpr BossWarning bossWarnings[STAGE_COUNT]={
 {"","",""},{"","",""},{"","",""},
 {"FIVE SHOTS ARE ABOUT TO SPREAD.","THE BALLS WILL AIM AT YOUR POSITION.","HOLD C TOWARD THE BOSS OR SIDESTEP."},
 {"A CROSS-SHAPED STRIKE IS LOCKED ON.","THE SHIELD CANNOT BLOCK THE FLOOR.","MOVE OUT OF BOTH MARKED LANES."},
 {"A SUBSTITUTION ORDER IS INCOMING.","ON HIT: BRIEF STUN AND -25% ENERGY.","FACE THE BOSS WITH C OR LEAVE THE RING."},
 {"FIVE PALM STRIKES ARE ABOUT TO LAND.","THE SHIELD CANNOT BLOCK THE FLOOR.","STEP BETWEEN THE FIVE MARKED CIRCLES."},
 {"MESSI IS REACHING FOR A CHOKEHOLD.","A CLOSE HIT WILL DAMAGE AND STUN YOU.","BACK AWAY OR FACE HIM WHILE HOLDING C."},
 {"MAGUIRE IS TURNING FOR A WIDE SWEEP.","A SPREAD OF FOOTBALLS WILL FOLLOW.","USE THE ARC GAP OR MOVE OUT OF THE RING."}
};
// Shared cinematic beats keep movement, the shout and completion in sync.
constexpr float OUTRO_RUN=4.4f,OUTRO_JUMP=6.35f,OUTRO_LAND=7.3f,OUTRO_SHOUT=7.42f,OUTRO_END=10.3f;
constexpr int INITIAL_RIVALS=1,MAX_ACTIVE_RIVALS=3;
constexpr float RIVAL_SPAWN_INTERVAL=9.f;
constexpr int DX[4]={0,1,0,-1},DZ[4]={-1,0,1,0};
struct Stage{const char* name;u32 seed;float pace,fire;int hp;const char* bio1;const char* bio2;};
static constexpr Stage stages[STAGE_COUNT]={
 {"TEN HAG",27183,.85f,.9f,2,"THE DISCIPLINARIAN","HIS PLAN. HIS RULES."},
 {"SIMEONE",61825,.75f,1.1f,3,"DEFENSIVE MASTERMIND","NEVER BACKS DOWN"},
 {"BOUNOU",90277,.65f,.75f,2,"THE LAST LINE","CALM UNDER PRESSURE"},
 {"CASSANO",33691,.82f,.95f,2,"THE LOUD CRITIC","NEVER SHORT OF WORDS"},
 {"GUARDIOLA",77813,.74f,1.f,3,"THE PASSING MASTER","EVERY MOVE A PLAN"},
 {"SANTOS",125407,.8f,.9f,3,"THE QUIET TACTICIAN","A TOUGH CALL TO MAKE"},
 {"PIQUE",191039,.72f,.95f,3,"THE TOWERING RIVAL","A GRIN. A CHALLENGE."},
 {"MESSI",250007,.7f,.9f,3,"THE LITTLE MAGICIAN","BIG PLAYS. NO FUSS."},
 {"HARRY MAGUIRE",390113,.8f,1.8f,2,"THE FINAL BOSS","THE ULTIMATE ROADBLOCK"}
};

static constexpr const char* arenaNames[STAGE_COUNT]={"THE BENCH","BUS PARK","DESERT WALL","PRESS BOX","PASSING GRID","CROSSROADS","FIVE FINGERS","FINAL LABYRINTH","SLABHEAD SHOWDOWN"};
static constexpr Room trophyRooms[STAGE_COUNT]={{12,8},{-15,10},{18,-12},{-21,-14},{24,16},{-27,18},{30,-20},{-33,-22},{36,24}};
enum State{TITLE,STAGE_SELECT,LOADOUT,INTRO,PLAYING,PAUSED,OUTRO,RESULT,RESERVED,MORGAN,WARNING,BOSS_DEMO,MORGAN_UNLOCK};
struct Unit{int x=1,z=9,dir=1,hp=2;float cool=0,think=.6f,inv=0,walk=0,kick=0,stun=0,spawnFlash=0,hitFlash=0,seal=0;int gait=0,id=0,homeX=0,homeZ=0;bool dead=false,guard=false,alert=false,boss=false;Room navFrom{},navGoal{},navNext{};bool navValid=false;};
struct Ball{float x,z,dx,dz,life;bool enemy,dead;int power=1;bool homing=false;};
struct Effect{int type;float x,z,tx,tz,age,duration;bool impacted;};
struct Particle{float x,z,y,vx,vz,vy,life;};
struct Button{int id,x,y,w,h,enabled;char label[80];};
struct Camera{float x=480,y=67,skew=-8,dy=22.2f;};
struct Game{
 Raster r;
 InfiniteMaze world;bool infinite=true;int goalX=23,goalZ=17;float worldTime=0,waveClock=0,waveRetry=0;int waveNumber=0;
 Room guardPosts[192];bool guardDeployed[192]={};int guardPostCount=0,encounters=0;
 IdaStar pathfinder;int searchAllowance=12000,idaCalls=0,idaCompleted=0,idaDeferred=0;
 bool bossSpawned=false,bossDefeated=false,bossRage=false;float bossTimer=3,bossWindup=0,bossTargetX=0,bossTargetZ=0,bossWindupTotal=1;int bossAttackDir=0,bossCasts=0;
 float warningTime=0;bool warningExiting=false;int warningCount=0;
 void beginWarning(){state=WARNING;warningTime=0;warningExiting=false;warningCount++;shake=12;tone(160,140);}
 void dismissWarning(){if(state!=WARNING||warningTime<WARNING_MIN_READ||warningExiting)return;warningExiting=true;warningTime=WARNING_HOLD;shake=12;tone(95,100);}
 void finishWarning(){if(state!=WARNING)return;state=PLAYING;if(!finalBoss())bossWindup=0;shake=maxf(shake,8);}
 bool bossEncounterActive()const{return infinite&&campaign&&stage>=3&&!bossDefeated;}
 int bossMaxHp()const{return stage>=3?lateBossHp[stage-3]:0;}
 const Unit* finalBoss()const{for(int i=0;i<enemyCount;i++)if(enemies[i].boss&&!enemies[i].dead)return &enemies[i];return nullptr;}
 int rivalLimit()const{return infinite?MAX_WORLD_RIVALS:MAX_ACTIVE_RIVALS;}
 int waveSize()const{int amount=stage<3?WAVE_SIZE:lateWaveSize[stage-3];return WIDTH<480?mini(4,amount):amount;}
 float waveInterval()const{return stage<3?WAVE_INTERVAL:lateWaveInterval[stage-3];}
 int nearbyLimit()const{return stage<3?MAX_NEARBY_RIVALS:lateNearbyLimit[stage-3];}
 float bossMovePace()const{float pace=lateBossMove[stage-3];return bossRage&&stage>=7?pace*(stage==FINAL_STAGE?.62f:.72f):pace;}
 float rivalMovePace()const{return stage<3?.24f:lateRivalMove[stage-3];}
 float rivalFirePace()const{return stage<3?3.4f:lateRivalFire[stage-3];}
 float bossFirePace()const{return bossRage?(stage==FINAL_STAGE?1.35f:1.7f):lateBossFire[stage-3];}
 float bossSkillPace()const{return bossRage?(stage==FINAL_STAGE?3.2f:3.8f):lateBossSkill[stage-3];}
 bool wallAt(int x,int z)const{return infinite?world.wall(x,z):x<0||x>=COLS||z<0||z>=ROWS||walls[index(x,z)];}
 int goalDistance()const{return roundi(length(float(goalX+1),float(goalZ+1)));}
 bool atTrophy()const{return infinite?(absf(float(player.x-goalX))<=2&&absf(float(player.z-goalZ))<=2&&!world.segmentBlocked(player.x+1,player.z+1,goalX+1,goalZ+1)):(player.x>=89&&player.z>=18);}
 bool trophyUnlocked()const{return !bossEncounterActive();}
 float trophyProgress()const{int gx=infinite?goalX:89,gz=infinite?goalZ:18;float start=length(float(gx-(infinite?-1:1)),float(gz-(infinite?-1:9)));if(atTrophy())return 1;return clamp(1-length(float(player.x-gx),float(player.z-gz))/maxf(1,start),0,.99f);}
 u8 walls[CELLS]={},defaults[CELLS]={};
 int field[CELLS]={},queue[CELLS]={};
 State state=TITLE,returnState=TITLE,pausedState=PLAYING;
 Unit player,enemies[MAX_WORLD_RIVALS];int enemyCount=0,nextEnemyId=1;
 Ball balls[256];int ballCount=0;Effect effects[48];int effectCount=0;
 Particle particles[256];int particleCount=0;
 Camera cam,outroCamera;Unit outroHero;
 Button buttons[48];int buttonCount=0;
 int focusedButton=-1;bool keyboardFocus=false;State focusState=TITLE;
 int stage=0,equipped=0,pendingStage=0,pendingSkill=-1,cleared=0,lives=3,kills=0,stagePage=0,skillPage=0,extraLinks=0;
 bool muted=false,campaign=true,won=false,storageOK=true;
 bool keys[256]={};int keyOrder[4]={},order=0;float moveTimer=0,spawnTimer=RIVAL_SPAWN_INTERVAL,charge=100,clock=0,cinema=0,notice=0,shake=0;
 bool spoken=false,replaying=false;int dashLeft=0,dashDir=1,dashHit[MAX_WORLD_RIVALS]={};float dashTimer=0;
 float specialAnim=0,shieldFlash=0,healFlash=0,superFlash=0,damageFlash=0,rageFlash=0;int animSkill=0,morganLine=0,rageFlashPulse=0;bool superUsed=false,fullHealNotice=false;Camera morganCamera;Unit morganHero;
 bool superUnlocked()const{return (cleared&7)==7;}
 bool shielding()const{return state==PLAYING&&keys[67]&&player.stun<=0;}
 bool shieldBlocks(float sourceX,float sourceZ){float dx=sourceX-player.x-1,dz=sourceZ-player.z-1,front=dx*DX[player.dir]+dz*DZ[player.dir],side=dx*DZ[player.dir]-dz*DX[player.dir];if(shielding()&&front>0&&front>=absf(side)){shieldFlash=.2f;tone(300,35);return true;}return false;}
 bool enemyHit(float sourceX,float sourceZ,bool directional=true){if(directional&&shieldBlocks(sourceX,sourceZ))return false;if(player.inv>0)return false;damage();return true;}
 void waveHeal(){if(lives<MAX_LIVES){lives++;healFlash=1.4f;fullHealNotice=false;}}
 int mouseX=0,mouseY=0;u32 random=27183;
 int lastArmCount=0;
 float titleTime=0,curtain=-1;int titleLetters=0;
 static constexpr float TITLE_READY=3.0f;
 float rnd(){random=random*1664525u+1013904223u;return float(random>>8)/16777216.f;}
 bool unlocked(int index)const{return index>=0&&index<STAGE_COUNT;}
 void speak(int id){if(!muted)host_voice(id);}void tone(int f,int ms){if(!muted)host_tone(f,ms);}
 int index(int x,int z)const{return z*COLS+x;}
 bool blocked(int x,int z,const u8* map=nullptr)const{if(!map&&infinite){return !world.fits(x,z);}if(!map)map=walls;if(x<0||z<0||x+2>COLS||z+2>ROWS)return true;for(int zz=z;zz<z+2;zz++)for(int xx=x;xx<x+2;xx++)if(map[index(xx,zz)])return true;return false;}
 bool pointBlocked(float x,float z)const{int xx=int(x)-(x<int(x)),zz=int(z)-(z<int(z));return wallAt(xx,zz);}
 void setupWorld(int level){
  world.seed=stages[level].seed;Room goal=world.trophyLeaf(trophyRooms[level]);world.protectedGoal=goal;world.protectGoal=true;goalX=goal.x*4-1;goalZ=goal.z*4-1;worldTime=waveClock=waveRetry=0;waveNumber=0;encounters=guardPostCount=0;idaCalls=idaCompleted=idaDeferred=0;bossSpawned=bossDefeated=bossRage=false;bossTimer=3;bossWindup=0;bossCasts=warningCount=0;warningTime=0;warningExiting=false;
  // More posts along the backbone; side links let players choose to bypass.
  // Deploy nearby posts once, before they enter the view; never teleport guards.
  Room p=world.parent(goal);while(world.depth(p)>=4&&guardPostCount<192){if(world.depth(p)%6==4){guardPosts[guardPostCount]=p;guardDeployed[guardPostCount++]=false;}p=world.parent(p);}
 }
 void deployGuards(){
  for(int i=0;i<guardPostCount;i++)if(!guardDeployed[i]){Room p=guardPosts[i];int x=p.x*4-1,z=p.z*4-1;if(length(float(x-player.x),float(z-player.z))>36)continue;int before=enemyCount;addRival(x,z);if(enemyCount>before){auto& e=enemies[enemyCount-1];e.guard=true;e.homeX=x;e.homeZ=z;e.dir=world.parent(p).x!=p.x?(p.x>0?3:1):(p.z>0?0:2);guardDeployed[i]=true;}}
 }
 void carve(int x,int z,int w,int h){for(int zz=z;zz<z+h;zz++)for(int xx=x;xx<x+w;xx++)walls[index(xx,zz)]=0;}
 void buildMap(u32 seed){
  random=seed;for(int i=0;i<CELLS;i++)walls[i]=1;for(int z=0;z<3;z++)for(int x=0;x<15;x++)carve(2+x*6,2+z*6,4,4);
  bool visited[45]={};int sx[45],sz[45],top=0,links=0;sx[0]=0;sz[0]=1;visited[15]=true;
  while(top>=0){int x=sx[top],z=sz[top],choices[4],n=0;for(int d=0;d<4;d++){int nx=x+DX[d],nz=z+DZ[d];if(nx>=0&&nx<15&&nz>=0&&nz<3&&!visited[nz*15+nx])choices[n++]=d;}if(!n){top--;continue;}int d=choices[mini(n-1,int(rnd()*n))],nx=x+DX[d],nz=z+DZ[d],width=links%3==0?2:4,offset=(4-width)/2;
   if(nx!=x)carve(2+mini(x,nx)*6+4,2+z*6+offset,2,width);else carve(2+x*6+offset,2+mini(z,nz)*6+4,width,2);
   links++;visited[nz*15+nx]=true;sx[++top]=nx;sz[top]=nz;
  }
  // Preserve the first three mazes. Later mazes gain genuine alternate loops;
  // stages 6-8 use offset, two-cell gaps that fit the full player collision box.
  int level=0;for(int i=0;i<STAGE_COUNT;i++)if(stages[i].seed==seed)level=i;
  int desired=level<3?0:level<5?level-1:5+(level-5)*2;extraLinks=0;
  for(int attempt=0;attempt<500&&extraLinks<desired;attempt++){
   int x=int(rnd()*14),z=int(rnd()*3);bool vertical=rnd()<.5f&&z<2;
   int cx=2+x*6+(vertical?0:4),cz=2+z*6+(vertical?4:0);bool closed=true;
   for(int zz=0;zz<(vertical?2:4);zz++)for(int xx=0;xx<(vertical?4:2);xx++)if(!walls[index(cx+xx,cz+zz)])closed=false;
   if(!closed)continue;int width=level>=5?2:4,offset=level>=5?(int(rnd()*2)*2):0;
   carve(cx+(vertical?offset:0),cz+(vertical?0:offset),vertical?width:2,vertical?2:width);extraLinks++;
  }
  carve(0,9,4,2);carve(88,16,2,4);carve(89,18,2,2);copybytes(defaults,walls,CELLS);
 }
 int distances(int x,int z,const u8* map=nullptr){for(int i=0;i<CELLS;i++)field[i]=-1;if(blocked(x,z,map))return 0;int count=1;queue[0]=index(x,z);field[queue[0]]=0;for(int i=0;i<count;i++){int cell=queue[i],cx=cell%COLS,cz=cell/COLS;for(int d=0;d<4;d++){int nx=cx+DX[d],nz=cz+DZ[d];if(!blocked(nx,nz,map)&&field[index(nx,nz)]<0){int p=index(nx,nz);field[p]=field[cell]+1;queue[count++]=p;}}}return count;}
 bool validMap(const u8* map){distances(1,9,map);return !blocked(1,9,map)&&!blocked(89,18,map)&&field[index(89,18)]>=0;}
 void clearKeys(){for(int i=0;i<256;i++)keys[i]=false;for(int i=0;i<4;i++)keyOrder[i]=0;moveTimer=0;}
 void follow(bool snap=false){float tx=WIDTH*.5f-((player.x+1-COLS/2.f)*25+(player.z+1-ROWS/2.f)*cam.skew),ty=HEIGHT*.52f-(player.z+1)*cam.dy;cam.x=mix(cam.x,tx,snap?1:.15f);cam.y=mix(cam.y,ty,snap?1:.15f);}
 Point project(float x,float y,float z)const{return{cam.x+(x-COLS/2.f)*25+(z-ROWS/2.f)*cam.skew,cam.y+z*cam.dy-y*20};}
 void init(){cleared=host_progress()&PROGRESS_MASK;buildMap(stages[0].seed);setupWorld(0);player=Unit{};player.x=player.z=-1;follow(true);state=TITLE;}
 void reset(){host_voice(0);cam.skew=-8;cam.dy=22.2f;kills=0;lives=MAX_LIVES;enemyCount=ballCount=particleCount=effectCount=0;nextEnemyId=1;player=Unit{};if(infinite){player.x=player.z=-1;setupWorld(stage);}player.inv=2;charge=100;spawnTimer=RIVAL_SPAWN_INTERVAL;notice=shake=cinema=0;dashLeft=0;specialAnim=shieldFlash=healFlash=superFlash=damageFlash=rageFlash=0;rageFlashPulse=0;superUsed=false;fullHealNotice=false;morganLine=0;spoken=false;replaying=false;clearKeys();state=PLAYING;follow(true);if(infinite)deployGuards();else spawn(INITIAL_RIVALS);}
 bool start(int level,int skill,bool intro=true){if(!unlocked(level)||skill<0||skill>=SKILL_COUNT)return false;stage=level;equipped=skill;campaign=true;infinite=true;buildMap(stages[stage].seed);reset();if(intro)state=level>=3?BOSS_DEMO:INTRO;return true;}
 void choose(int level){if(!unlocked(level))return;if(state!=STAGE_SELECT&&state!=LOADOUT)returnState=state;pendingStage=level;pendingSkill=-1;stagePage=level/3;skillPage=0;state=LOADOUT;clearKeys();}
 void finish(bool success){bool hadInterview=superUnlocked();state=RESULT;won=success;host_voice(0);clearKeys();dashLeft=0;shake=success?14:0;if(success&&campaign&&!replaying){cleared|=1<<stage;storageOK=host_save_progress(cleared)!=0;}cinema=0;
  if(success&&!hadInterview&&superUnlocked()){state=MORGAN_UNLOCK;morganCamera=cam;morganHero=player;morganLine=0;}
 }
 void beginOut(bool replay=false){
  outroHero=player;outroCamera=cam;
  // Replays also begin at the actual exit, without relocating the live player.
  if(replay){outroHero.x=infinite?goalX:89;outroHero.z=infinite?goalZ:18;outroHero.dir=1;outroHero.walk=outroHero.kick=0;
   outroCamera.x=WIDTH*.5f-((outroHero.x+1-COLS/2.f)*25+(outroHero.z+1-ROWS/2.f)*outroCamera.skew);
   outroCamera.y=HEIGHT*.52f-(outroHero.z+1)*outroCamera.dy;
  }
  state=OUTRO;cinema=0;spoken=false;replaying=replay;notice=0;effectCount=0;dashLeft=0;shake=12;clearKeys();
 }
 void skip(){if(state==WARNING){dismissWarning();}else if(state==MORGAN){finishMorgan();}else if(state==MORGAN_UNLOCK){host_voice(0);state=RESULT;cinema=0;clearKeys();}else if(state==INTRO||state==BOSS_DEMO){host_voice(0);state=PLAYING;clearKeys();follow(true);}else if(state==OUTRO){if(replaying){state=returnState;host_voice(0);replaying=false;}else finish(true);}}
 bool step(Unit& u,int d){int x=u.x+DX[d],z=u.z+DZ[d];u.dir=d;if(blocked(x,z))return false;
  if(infinite){if(&u!=&player&&absf(float(x-player.x))<2&&absf(float(z-player.z))<2)return false;for(int i=0;i<enemyCount;i++){auto& e=enemies[i];if(&e!=&u&&!e.dead&&e.spawnFlash<=0&&absf(float(x-e.x))<2&&absf(float(z-e.z))<2)return false;}}
  u.x=x;u.z=z;u.walk=.2f;u.gait++;return true;}
 void spawn(int amount){
  if(infinite){for(int n=0;n<amount;n++){for(int attempt=0;attempt<100;attempt++){int x=player.x+int(rnd()*41)-20,z=player.z+int(rnd()*41)-20;if(length(float(x-player.x),float(z-player.z))<12||blocked(x,z))continue;addRival(x,z);break;}}return;}
  distances(player.x,player.z);int candidates[CELLS],n=0;for(int i=0;i<CELLS;i++)if(field[i]>=12&&field[i]<=32&&length(float(i%COLS-player.x),float(i/COLS-player.z))>=8)candidates[n++]=i;
  for(int j=0;j<amount&&enemyCount<rivalLimit()&&n;j++){int best=-1;for(int attempt=0;attempt<n;attempt++){int c=candidates[int(rnd()*n)%n];bool free=true;for(int i=0;i<enemyCount;i++)if(absf(float(enemies[i].x-c%COLS))<3&&absf(float(enemies[i].z-c/COLS))<3)free=false;if(free){best=c;break;}}if(best<0)break;Unit u;u.x=best%COLS;u.z=best/COLS;u.dir=3;u.cool=1.6f;u.hp=stages[stage].hp;u.id=nextEnemyId++;enemies[enemyCount++]=u;}
 }
 void fire(Unit& u,bool enemy=false){if(enemy&&!u.boss){int active=0;for(int i=0;i<ballCount;i++)if(balls[i].enemy&&!balls[i].dead)active++;if(active>=8)return;}if(u.seal>0||u.stun>0||u.spawnFlash>0||u.cool>0||ballCount>=254||(u.boss&&bossWindup>0))return;u.cool=enemy?(infinite?(u.boss?bossFirePace():rivalFirePace()):stages[stage].fire):.28f;u.kick=.2f;balls[ballCount++]={float(u.x+1),float(u.z+1),float(DX[u.dir]),float(DZ[u.dir]),3,enemy,false};
  if(enemy&&u.boss&&bossRage)for(int side=-1;side<=1;side+=2){float dx=DX[u.dir]+DZ[u.dir]*.3f*side,dz=DZ[u.dir]-DX[u.dir]*.3f*side,n=length(dx,dz);balls[ballCount++]={float(u.x+1),float(u.z+1),dx/n,dz/n,3,true,false};}tone(enemy?100:185,35);}
 bool clearLine(float x,float z,float tx,float tz){if(infinite)return !world.segmentBlocked(x,z,tx,tz);int steps=maxi(1,int(length(tx-x,tz-z)*8));for(int i=1;i<steps;i++)if(pointBlocked(mix(x,tx,float(i)/steps),mix(z,tz,float(i)/steps)))return false;return true;}
 void burst(float x,float z,int count){for(int i=0;i<count&&particleCount<256;i++)particles[particleCount++]={x,z,.2f,(rnd()-.5f)*2,(rnd()-.5f)*2,1+rnd()*1.7f,.4f+rnd()*.3f};}
 void hurt(Unit& e,int damage,bool chargeHit=false){if(e.dead||e.spawnFlash>0||damage<=0)return;e.hp-=damage;e.hitFlash=.18f;shake=maxf(shake,e.boss?7:4);burst(e.x+1,e.z+1,8);if(chargeHit)charge=minf(100,charge+8);if(e.hp<=0){e.dead=true;kills++;if(e.boss){bossDefeated=true;bossWindup=0;shake=maxf(shake,12);tone(350,200);}}}
 void damage(){if(player.inv>0)return;lives--;player.inv=2;shake=7;burst(player.x+1,player.z+1,15);tone(55,160);if(lives<=0)finish(false);damageFlash=DAMAGE_FLASH_TIME;shake=maxf(shake,7);}
 void blast(float x,float z,float radius,int damage,float stun,bool push=false,bool dash=false){
  for(int i=0;i<enemyCount;i++){Unit& e=enemies[i];if(e.dead||e.spawnFlash>0||length(e.x+1-x,e.z+1-z)>radius||!clearLine(x,z,e.x+1,e.z+1))continue;
   bool hit=false;if(dash){for(int j=0;j<MAX_WORLD_RIVALS;j++)if(dashHit[j]==e.id)hit=true;if(hit)continue;for(int j=0;j<MAX_WORLD_RIVALS;j++)if(!dashHit[j]){dashHit[j]=e.id;break;}}
   hurt(e,damage);e.stun=e.boss?minf(.4f,stun):stun;if(push&&!e.dead&&!e.boss){float dx=e.x+1-x,dz=e.z+1-z;int d=absf(dx)>=absf(dz)?(dx>=0?1:3):(dz>=0?2:0);for(int t=0;t<2;t++){bool free=true;for(int k=0;k<enemyCount;k++)if(k!=i&&!enemies[k].dead&&absf(float(enemies[k].x-e.x-DX[d]))<2&&absf(float(enemies[k].z-e.z-DZ[d]))<2)free=false;if(!free||!step(e,d))break;}}
  }for(int i=0;i<ballCount;i++)if(balls[i].enemy&&length(balls[i].x-x,balls[i].z-z)<=radius&&clearLine(x,z,balls[i].x,balls[i].z))balls[i].dead=true;
 }
 void addRival(int x,int z){if(enemyCount>=rivalLimit()||blocked(x,z))return;for(int i=0;i<enemyCount;i++)if(!enemies[i].dead&&absf(float(enemies[i].x-x))<3&&absf(float(enemies[i].z-z))<3)return;Unit u;u.x=x;u.z=z;u.dir=3;u.cool=1.8f;u.hp=2;u.id=nextEnemyId++;enemies[enemyCount++]=u;}
 bool launchWave(){
  int nearby=0,amount=waveSize();for(int i=0;i<enemyCount;i++)if(!enemies[i].dead&&length(float(enemies[i].x-player.x),float(enemies[i].z-player.z))<48)nearby++;
  if(nearby+amount>nearbyLimit()||enemyCount+amount>rivalLimit())return false;
  Room positions[WAVE_SIZE];int count=0;float ui=minf(float(WIDTH)/960,float(HEIGHT)/720);int rx=maxi(14,WIDTH/50+HEIGHT/140),rz=maxi(14,HEIGHT/44);
  for(int attempt=0;attempt<2400&&count<amount;attempt++){
   int x=player.x+int(rnd()*(rx*2+1))-rx,z=player.z+int(rnd()*(rz*2+1))-rz;float distance=length(float(x-player.x),float(z-player.z));Point p=project(x+1,.04f,z+1);
   if(distance<6||distance>42||blocked(x,z)||p.x<40||p.x>WIDTH-40||p.y<56*ui+90||p.y>HEIGHT-80*ui-20)continue;
   bool free=true;for(int i=0;i<enemyCount;i++)if(!enemies[i].dead&&absf(float(x-enemies[i].x))<3&&absf(float(z-enemies[i].z))<3)free=false;
   for(int i=0;i<count;i++)if(length(float(x-positions[i].x),float(z-positions[i].z))<4)free=false;
   if(free)positions[count++]={x,z};
  }
  if(count<amount)return false; // Never create only part of a promised wave.
  for(int i=0;i<amount;i++){Room p=positions[i];addRival(p.x,p.z);auto& e=enemies[enemyCount-1];e.spawnFlash=SPAWN_WARNING;e.hp=1;}
  waveNumber++;waveHeal();tone(140,90);return true;
 }
 void updateReinforcements(float dt){
  for(int i=enemyCount-1;i>=0;i--)if(!enemies[i].boss&&(absf(float(enemies[i].x-player.x))>96||absf(float(enemies[i].z-player.z))>96))enemies[i]=enemies[--enemyCount];
  waveClock=minf(waveInterval(),waveClock+dt);waveRetry=maxf(0,waveRetry-dt);
  if(waveClock>=waveInterval()&&waveRetry<=0){if(launchWave())waveClock=0;else waveRetry=.5f;}
 }
 bool bossThreatens(float x,float z){
  const Unit* b=finalBoss();if(!b||!clearLine(b->x+1,b->z+1,x,z))return false;
  float dx=x-bossTargetX,dz=z-bossTargetZ;
  if(stage==4)return (absf(dx)<1.15f&&absf(dz)<5)||(absf(dz)<1.15f&&absf(dx)<5);
  if(stage==6){for(int i=0;i<5;i++){float px=i==0?0:i==1?-3:i==2?3:0,pz=i==3?-3:i==4?3:0;if(length(dx-px,dz-pz)<1.5f)return true;}return false;}
  if(stage==7)return length(x-b->x-1,z-b->z-1)<3.5f;
  if(stage==8){dx=x-b->x-1;dz=z-b->z-1;float front=dx*DX[bossAttackDir]+dz*DZ[bossAttackDir],side=dx*DZ[bossAttackDir]-dz*DX[bossAttackDir];return length(dx,dz)<4.8f&&!(front>0&&front>absf(side)*1.6f);}
  return length(dx,dz)<2.7f;
 }
 void bossVolley(Unit& b,int count,float spread){float dx=player.x-b.x,dz=player.z-b.z,n=maxf(.1f,length(dx,dz));dx/=n;dz/=n;for(int i=0;i<count&&ballCount<256;i++){float offset=(i-(count-1)*.5f)*spread,vx=dx-dz*offset,vz=dz+dx*offset,k=length(vx,vz);balls[ballCount++]={float(b.x+1),float(b.z+1),vx/k,vz/k,4,true,false};}}
 void updateBoss(float dt){
  if(state!=PLAYING||!bossEncounterActive())return;
  if(!bossSpawned&&length(float(player.x-goalX),float(player.z-goalZ))<=22&&enemyCount<rivalLimit()){
   bool free=true;for(int i=0;i<enemyCount;i++)if(!enemies[i].dead&&absf(float(enemies[i].x-goalX))<2&&absf(float(enemies[i].z-goalZ))<2)free=false;
   if(free){Unit boss;boss.x=goalX;boss.z=goalZ;boss.hp=bossMaxHp();boss.boss=true;boss.spawnFlash=1.8f;boss.id=nextEnemyId++;boss.dir=2;enemies[enemyCount++]=boss;bossSpawned=true;tone(60,180);}
  }
  Unit* boss=nullptr;for(int i=0;i<enemyCount;i++)if(enemies[i].boss&&!enemies[i].dead)boss=&enemies[i];if(!boss||boss->spawnFlash>0||boss->seal>0)return;
  if(stage>=7&&boss->hp*2<=bossMaxHp()&&!bossRage){bossRage=true;rageFlash=RAGE_FLASH_TIME;rageFlashPulse=0;bossTimer=maxf(bossTimer,RAGE_FLASH_TIME+.2f);shake=maxf(shake,12);tone(80,180);}
  if(boss->stun>0){bossWindup=0;bossTimer=maxf(bossTimer,1.5f);return;}
  if(bossWindup>0){bossWindup=maxf(0,bossWindup-dt);if(bossWindup<=0){
    bossCasts++;if(stage==3)bossVolley(*boss,5,.23f);
    else if(bossThreatens(player.x+1,player.z+1)){bool hit=enemyHit(boss->x+1,boss->z+1,stage!=4&&stage!=6);if(hit&&(stage==5||stage==7)){player.stun=stage==7?.8f:1.1f;clearKeys();if(stage==5)charge=maxf(0,charge-25);}}
    if(stage==8){boss->dir=(bossAttackDir+2)%4;bossVolley(*boss,bossRage?5:3,.3f);}addEffect(4,bossTargetX,bossTargetZ);burst(bossTargetX,bossTargetZ,30);shake=8;tone(50,150);
   }return;}
  bossTimer-=dt;if(bossTimer<=0&&length(float(player.x-boss->x),float(player.z-boss->z))<=(stage==7?4.5f:14)&&clearLine(boss->x+1,boss->z+1,player.x+1,player.z+1)){
   bossTargetX=player.x+1;bossTargetZ=player.z+1;bossAttackDir=boss->dir;bossWindupTotal=stage==7?1.15f:stage==8?(bossRage?1.5f:2.0f):1.6f;bossWindup=bossWindupTotal;bossTimer=bossSkillPace();beginWarning();
  }
 }
 int rivalDirection(Unit& e){
  if(!infinite){
   Room a{e.x,e.z},b{player.x,player.z};
   if(searchAllowance>0){auto result=pathfinder.find(a,b,[&](Room,Room q){return !blocked(q.x,q.z);},mini(searchAllowance,6000));searchAllowance-=result.nodes;idaCalls++;if(result.complete){idaCompleted++;if(result.length<=1)return -1;return result.next.x!=e.x?(result.next.x>e.x?1:3):(result.next.z>e.z?2:0);}idaDeferred++;}
   int best=-1,distance=10000;for(int d=0;d<4;d++){int x=e.x+DX[d],z=e.z+DZ[d];if(!blocked(x,z)&&field[index(x,z)]>=0&&field[index(x,z)]<distance){best=d;distance=field[index(x,z)];}}return distance>1?best:-1;
  }
  Room a=world.room(e.x+1,e.z+1),b=world.room(player.x+1,player.z+1);int tx=player.x,tz=player.z;
  if(!sameRoom(a,b)){
   Room n=world.next(a,b); // Safe connected fallback if this frame's budget runs out.
   if(e.navValid&&sameRoom(e.navFrom,a)&&sameRoom(e.navGoal,b))n=e.navNext;
   else if(searchAllowance>0){auto result=pathfinder.find(a,b,[&](Room p,Room q){return world.linked(p,q);},mini(searchAllowance,6000));searchAllowance-=result.nodes;idaCalls++;
    if(result.complete&&result.length>0){n=result.next;e.navFrom=a;e.navGoal=b;e.navNext=n;e.navValid=true;idaCompleted++;}else{e.navValid=false;idaDeferred++;}}
   tx=n.x*4-1;tz=n.z*4-1;if(n.x!=a.x&&e.z!=a.z*4-1){tx=e.x;tz=a.z*4-1;}else if(n.z!=a.z&&e.x!=a.x*4-1){tz=e.z;tx=a.x*4-1;}
  }
  int d=tx!=e.x?(tx>e.x?1:3):tz!=e.z?(tz>e.z?2:0):-1;
  if(d>=0&&!blocked(e.x+DX[d],e.z+DZ[d]))return d;
  // A 2x2 body can straddle a room doorway. Resolve its local turn using the
  // same IDA* on tile anchors instead of getting stuck on a corner panel.
  if(d>=0&&searchAllowance>0){auto local=pathfinder.find({e.x,e.z},{tx,tz},[&](Room,Room q){return !blocked(q.x,q.z);},mini(1000,searchAllowance));searchAllowance-=local.nodes;idaCalls++;if(local.complete&&local.length>0){idaCompleted++;return local.next.x!=e.x?(local.next.x>e.x?1:3):(local.next.z>e.z?2:0);}idaDeferred++;}
  return -1;
 }
 void addEffect(int type,float x,float z,float tx=0,float tz=0){if(effectCount<48)effects[effectCount++]={type,x,z,tx,tz,0,type==2?1.2f:type==0?1.0f:.7f,false};}
 bool special(){if(state!=PLAYING||shielding()||player.stun>0||charge<100||dashLeft||specialAnim>0)return false;charge=0;notice=1.4f;specialAnim=1;animSkill=equipped;shake=6;tone(240,120);float x=player.x+1,z=player.z+1;
  if(equipped==0){addEffect(0,x,z);blast(x,z,5,2,1.5f,true);player.inv=maxf(player.inv,.35f);}
  if(equipped==1){dashLeft=6;dashTimer=0;dashDir=player.dir;for(int& hit:dashHit)hit=0;player.inv=maxf(player.inv,.65f);}
  if(equipped==2){float tx=x,tz=z;for(int i=1;i<=6;i++){if(pointBlocked(x+DX[player.dir]*i,z+DZ[player.dir]*i)||!clearLine(x,z,x+DX[player.dir]*i,z+DZ[player.dir]*i))break;tx=x+DX[player.dir]*i;tz=z+DZ[player.dir]*i;}addEffect(2,x,z,tx,tz);}
  if(equipped==3){addEffect(3,x,z,float(DX[player.dir]),float(DZ[player.dir]));player.inv=maxf(player.inv,.7f);for(int i=0;i<enemyCount;i++){auto& e=enemies[i];float dx=e.x+1-x,dz=e.z+1-z,front=dx*DX[player.dir]+dz*DZ[player.dir],side=dx*DZ[player.dir]-dz*DX[player.dir];if(!e.dead&&e.spawnFlash<=0&&front>=0&&front<=4.5f&&absf(side)<=2.1f&&clearLine(x,z,e.x+1,e.z+1)){hurt(e,6);e.stun=e.boss?.4f:2;}}}
  return true;
 }
 bool super(){if(state!=PLAYING||!superUnlocked()||superUsed||player.stun>0)return false;superUsed=true;morganCamera=cam;morganHero=player;cinema=0;morganLine=0;state=MORGAN;dashLeft=0;specialAnim=0;shake=8;tone(90,80);return true;}
 void finishMorgan(){if(state!=MORGAN)return;host_voice(0);cam=morganCamera;state=PLAYING;lives=MAX_LIVES;healFlash=.9f;fullHealNotice=true;player.inv=maxf(player.inv,2);player.stun=0;superFlash=0;damageFlash=0;
  // Every ordinary rival currently visible is eliminated, including guards
  // behind panels and rivals still flashing into existence. The boss is sealed
  // instead of damaged, and the reinforcement meter restarts from empty.
  for(int i=0;i<enemyCount;i++){auto& e=enemies[i];if(e.dead)continue;if(e.boss){e.seal=2;bossWindup=0;bossTimer=maxf(bossTimer,2);continue;}Point p=project(e.x+1,.04f,e.z+1);bool visible=p.x>=-60&&p.x<=WIDTH+60&&p.y>=0&&p.y-160<=HEIGHT;if(visible){e.spawnFlash=0;hurt(e,e.hp);}}
  for(int i=0;i<ballCount;i++)if(balls[i].enemy){Point p=project(balls[i].x,.22f,balls[i].z);if(p.x>=-12&&p.x<=WIDTH+12&&p.y>=-12&&p.y<=HEIGHT+12)balls[i].dead=true;}
  waveClock=waveRetry=0;spawnTimer=RIVAL_SPAWN_INTERVAL;shake=0;tone(55,90);
 }
 void animate(Unit& u,float dt){u.cool-=dt;u.inv-=dt;u.walk=maxf(0,u.walk-dt);u.kick=maxf(0,u.kick-dt);u.stun=maxf(0,u.stun-dt);u.hitFlash=maxf(0,u.hitFlash-dt);}
 void update(float dt){
  if(state==WARNING){dt=clamp(dt,0,.05f);warningTime+=dt;shake=maxf(0,shake-dt*28);if(!warningExiting&&warningTime>=WARNING_HOLD)dismissWarning();if(warningTime>=WARNING_END)finishWarning();return;}
  if(state==PAUSED&&pausedState==WARNING)return;
  dt=clamp(dt,0,.05f);clock+=dt;if(state!=PAUSED){damageFlash=maxf(0,damageFlash-dt);shake=maxf(0,shake-dt*28);if(rageFlash>0){rageFlash=maxf(0,rageFlash-dt);int pulse=mini(2,int((RAGE_FLASH_TIME-rageFlash)/.4f));if(pulse>rageFlashPulse){rageFlashPulse=pulse;shake=maxf(shake,12);tone(80,110);}}}
  if(state==TITLE){titleTime+=dt;int letters=titleTime<.75f?0:mini(7,1+int((titleTime-.75f)/.25f));if(letters>titleLetters){titleLetters=letters;tone(100+letters*25,35);}if(curtain>=0){curtain+=dt;if(curtain>=1.2f){curtain=-1;state=STAGE_SELECT;stagePage=0;returnState=TITLE;clearKeys();}}return;}
  if(state==INTRO){cinema+=dt;if(stage==0&&cinema>=.45f&&!spoken){spoken=true;speak(1);}if(cinema>=(stage==0?7.4f:3.2f))skip();return;}
  if(state==BOSS_DEMO){cinema+=dt;if(cinema>=3.2f&&!spoken){spoken=true;shake=10;tone(65,150);}if(cinema>=7)skip();return;}
  if(state==MORGAN){cinema+=dt;if(cinema>=MORGAN_POPUP_END)finishMorgan();return;}
  if(state==MORGAN_UNLOCK){cinema+=dt;if(cinema>=MORGAN_END)skip();return;}
  if(state==OUTRO){cinema+=dt;if(cinema>=OUTRO_SHOUT&&!spoken){spoken=true;speak(2);shake=16;tone(90,180);}if(cinema>=OUTRO_END)skip();return;}if(state!=PLAYING)return;
  searchAllowance=12000;animate(player,dt);charge=minf(100,charge+100/skills[equipped].recharge*dt);notice=maxf(0,notice-dt);specialAnim=maxf(0,specialAnim-dt);shieldFlash=maxf(0,shieldFlash-dt);healFlash=maxf(0,healFlash-dt);superFlash=maxf(0,superFlash-dt);
  if(dashLeft){dashTimer-=dt;while(dashLeft&&dashTimer<=0){bool moved=step(player,dashDir);addEffect(1,player.x+1,player.z+1);blast(player.x+1,player.z+1,2.7f,3,.8f,false,true);dashTimer+=.065f;if(!moved)dashLeft=0;else dashLeft--;}}
  moveTimer-=dt;int direction=-1,latest=0;for(int i=0;i<4;i++)if(keyOrder[i]>latest){latest=keyOrder[i];direction=i;}if(direction>=0&&moveTimer<=0&&!dashLeft&&player.stun<=0){if(shielding())player.dir=direction;else step(player,direction);moveTimer=.13f;}if(atTrophy()&&trophyUnlocked()){beginOut();return;}if(keys[32])fire(player);
  for(int i=0;i<effectCount;i++){Effect& e=effects[i];e.age+=dt;if(e.type==2&&!e.impacted&&e.age>=.4f){e.impacted=true;blast(e.tx,e.tz,3.7f,4,2);burst(e.tx,e.tz,28);shake=9;tone(70,120);}}for(int i=effectCount-1;i>=0;i--)if(effects[i].age>=effects[i].duration)effects[i]=effects[--effectCount];
  if(infinite){worldTime+=dt;deployGuards();updateReinforcements(dt);updateBoss(dt);}else{spawnTimer-=dt;if(spawnTimer<=0){int before=enemyCount;spawn(1);if(enemyCount>before)waveHeal();spawnTimer=RIVAL_SPAWN_INTERVAL;}distances(player.x,player.z);}
  if(state!=PLAYING)return;
  for(int i=0;i<enemyCount;i++){Unit& e=enemies[i];if(e.dead)continue;if(e.seal>0){e.seal=maxf(0,e.seal-dt);continue;}if(e.spawnFlash>0){e.spawnFlash=maxf(0,e.spawnFlash-dt);if(e.spawnFlash<=0){bool occupied=absf(float(e.x-player.x))<2&&absf(float(e.z-player.z))<2;for(int j=0;j<enemyCount;j++)if(j!=i&&!enemies[j].dead&&enemies[j].spawnFlash<=0&&absf(float(e.x-enemies[j].x))<2&&absf(float(e.z-enemies[j].z))<2)occupied=true;if(occupied)e.spawnFlash=.15f;else{e.cool=1.2f;e.think=0;}}continue;}bool stunned=e.stun>0;animate(e,dt);if(stunned)continue;
   if(e.guard&&!e.alert){e.alert=true;e.cool=maxf(e.cool,1.6f);encounters++;}
   e.think-=dt;if(e.think<=0&&(!e.boss||bossWindup<=0)){e.think=(infinite?(e.boss?bossMovePace():rivalMovePace()):stages[stage].pace)+rnd()*.08f;int d=rivalDirection(e);if(d>=0&&!step(e,d)){
     // Keep a crowded doorway flowing: try a free side step, never phase through bodies.
     int sides[2]={(d+1)%4,(d+3)%4};if(length(float(e.x+DX[sides[0]]-player.x),float(e.z+DZ[sides[0]]-player.z))>length(float(e.x+DX[sides[1]]-player.x),float(e.z+DZ[sides[1]]-player.z))){int swap=sides[0];sides[0]=sides[1];sides[1]=swap;}for(int side:sides)if(step(e,side))break;
    }}
   float distance=length(float(player.x-e.x),float(player.z-e.z));if(distance<=12&&clearLine(e.x+1,e.z+1,player.x+1,player.z+1)){
    float dx=player.x-e.x,dz=player.z-e.z;e.dir=absf(dx)>=absf(dz)?(dx>0?1:3):(dz>0?2:0);int before=ballCount;fire(e,true);if(ballCount>before){balls[before].dx=dx/maxf(.1f,distance);balls[before].dz=dz/maxf(.1f,distance);if(!e.boss)e.cool=rivalFirePace()+rnd()*.8f;}
   }
  }
  for(int i=0;i<ballCount;i++){Ball& b=balls[i];if(b.dead)continue;if(b.homing){int target=-1;float nearest=12;for(int k=0;k<enemyCount;k++){auto& e=enemies[k];float distance=length(e.x+1-b.x,e.z+1-b.z);if(!e.dead&&distance<nearest&&clearLine(b.x,b.z,e.x+1,e.z+1)){nearest=distance;target=k;}}if(target>=0){float dx=enemies[target].x+1-b.x,dz=enemies[target].z+1-b.z,n=maxf(.01f,length(dx,dz));b.dx=mix(b.dx,dx/n,minf(1,dt*8));b.dz=mix(b.dz,dz/n,minf(1,dt*8));n=maxf(.01f,length(b.dx,b.dz));b.dx/=n;b.dz/=n;}}float speed=b.enemy?4.2f:7;float oldX=b.x,oldZ=b.z;b.x+=b.dx*speed*dt;b.z+=b.dz*speed*dt;b.life-=dt;if(b.life<=0||pointBlocked(b.x,b.z)||(infinite&&world.segmentBlocked(oldX,oldZ,b.x,b.z))){b.dead=true;continue;}if(b.enemy){if(b.x>player.x+.2f&&b.x<player.x+1.8f&&b.z>player.z+.2f&&b.z<player.z+1.8f){b.dead=true;enemyHit(player.x+1-b.dx*3,player.z+1-b.dz*3);}}else for(int j=0;j<enemyCount;j++){Unit& e=enemies[j];if(!b.dead&&!e.dead&&e.spawnFlash<=0&&b.x>=e.x&&b.x<=e.x+2&&b.z>=e.z&&b.z<=e.z+2){b.dead=true;hurt(e,b.power,true);}}}
  for(int i=ballCount-1;i>=0;i--)if(balls[i].dead)balls[i]=balls[--ballCount];for(int i=enemyCount-1;i>=0;i--)if(enemies[i].dead)enemies[i]=enemies[--enemyCount];
  for(int i=0;i<particleCount;i++){auto& p=particles[i];p.x+=p.vx*dt;p.z+=p.vz*dt;p.y+=p.vy*dt;p.vy-=5*dt;p.life-=dt;}for(int i=particleCount-1;i>=0;i--)if(particles[i].life<=0)particles[i]=particles[--particleCount];follow();
  if(state==PLAYING&&atTrophy()&&trophyUnlocked())beginOut();
 }
 int moveDirection(int key){switch(key){case 87:case 38:return 0;case 68:case 39:return 1;case 83:case 40:return 2;case 65:case 37:return 3;default:return -1;}}
 void key(int code,bool down){if(code<0||code>=256)return;bool repeat=keys[code];keys[code]=down;int d=moveDirection(code);if(!down){if(d>=0){int a[4]={87,68,83,65},b[4]={38,39,40,37};if(!keys[a[d]]&&!keys[b[d]])keyOrder[d]=0;}return;}if(repeat)return;
  if(code==77){muted=!muted;if(muted)host_voice(0);return;}
  if(state==TITLE){if(titleTime>=TITLE_READY&&curtain<0){curtain=0;clearKeys();}return;}
  if(code==80&&(state==PLAYING||state==INTRO||state==OUTRO||state==MORGAN||state==MORGAN_UNLOCK||state==BOSS_DEMO||state==WARNING||state==PAUSED)){if(state==PAUSED)state=pausedState;else{pausedState=state;state=PAUSED;host_voice(0);}clearKeys();return;}
  if(state==WARNING){if(code>=37&&code<=40||code==13){navigateMenu(code);}return;}
  if(state!=PLAYING&&(code>=37&&code<=40||code==13)){navigateMenu(code);return;}
  if(state==INTRO||state==OUTRO||state==MORGAN||state==MORGAN_UNLOCK||state==BOSS_DEMO){if(code==13||(code==32&&state!=MORGAN))skip();return;}
  if(state!=PLAYING)return;if(code==66){super();return;}if(code==67){dashLeft=0;player.walk=0;}if(code==86)special();if(code==32)fire(player);if(d>=0){keyOrder[d]=++order;if(player.stun<=0){if(shielding())player.dir=d;else if(!dashLeft)step(player,d);}moveTimer=.13f;}
 }
 void blur(){clearKeys();if(state==PLAYING||state==INTRO||state==OUTRO||state==MORGAN||state==MORGAN_UNLOCK||state==BOSS_DEMO||state==WARNING){pausedState=state;state=PAUSED;host_voice(0);}}
 void action(int id);
 void navigateMenu(int code);void drawFocus();
 void render();void screenShake();void drawWarning();void drawWorld(bool actors=true);void trophy(float x,float y,float scale=1);void drawIntro();void drawOutro();void drawMorgan();void drawBossDemo();
 void actor(const Unit& u,float x,float y,float scale=1,int rival=-1,int pose=0,float phase=0,bool angry=false);
 void button(int id,int x,int y,int w,int h,const char* label,bool enabled=true,bool selected=false,int size=2);
 void pointer(int type,int x,int y);
};
