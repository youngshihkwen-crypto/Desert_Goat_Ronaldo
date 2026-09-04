#include "controller.hpp"
#ifdef __wasm__
// Freestanding C++ needs no SDK, C runtime, downloaded framework or JS game engine.
extern "C" void* memset(void* dst,int value,__SIZE_TYPE__ n){auto p=(u8*)dst;for(__SIZE_TYPE__ i=0;i<n;i++)p[i]=(u8)value;return dst;}
extern "C" void* memcpy(void* dst,const void* src,__SIZE_TYPE__ n){auto d=(u8*)dst;auto s=(const u8*)src;for(__SIZE_TYPE__ i=0;i<n;i++)d[i]=s[i];return dst;}
#endif
Game game;
extern "C" {
void game_init(){game.init();game.render();}
void game_resize(int width,int height){float fit=minf(1,minf(float(MAX_WIDTH)/maxi(1,width),float(MAX_HEIGHT)/maxi(1,height)));WIDTH=maxi(240,roundi(width*fit));HEIGHT=maxi(200,roundi(height*fit));game.r.clear(0);game.follow(true);game.render();}
void game_tick(float dt){game.update(dt);game.render();}
void game_key(int key,int down){game.key(key,down!=0);}
void game_pointer(int type,int x,int y){game.pointer(type,x,y);}
void game_action(int id){game.keyboardFocus=false;game.action(id);game.render();}
void game_focus(int id){for(int i=0;i<game.buttonCount;i++)if(game.buttons[i].id==id&&game.buttons[i].enabled){game.focusedButton=id;game.focusState=game.state;game.keyboardFocus=true;break;}game.render();}
void game_blur(){game.blur();game.render();}
u32* game_pixels(){return game.r.pixels;}
int game_width(){return WIDTH;}int game_height(){return HEIGHT;}
int game_button_count(){return game.buttonCount;}
const Button* game_button(int i){return i>=0&&i<game.buttonCount?&game.buttons[i]:nullptr;}
int game_state(){return game.state;}
const char* game_status(){
 static char status[128];
 if(game.state==WARNING||game.state==PAUSED&&game.pausedState==WARNING){copystr(status,game.state==WARNING?"WARNING: ":"PAUSED WARNING: ",128);append(status,stages[game.stage].name);append(status," - ");append(status,bossSkills[game.stage]);append(status,". ");append(status,bossWarnings[game.stage].dodge);return status;}
 if(game.state==PLAYING||game.state==PAUSED){copystr(status,game.state==PAUSED?"PAUSED":"PLAYING",128);char n[16];append(status," STAGE ");number(n,game.stage+1);append(status,n);append(status," X ");number(n,game.player.x);append(status,n);append(status," Z ");number(n,game.player.z);append(status,n);append(status," HP ");number(n,game.lives);append(status,n);return status;}
 switch(game.state){case TITLE:return "TITLE";case STAGE_SELECT:return "SELECT STAGE";case LOADOUT:return "CHOOSE SPECIAL";case INTRO:return "INTRO";case BOSS_DEMO:return bossSkills[game.stage];case MORGAN_UNLOCK:return "MORGAN INTERVIEW UNLOCKED";case MORGAN:return "MORGAN INTERVIEW - FICTIONAL PARODY";case OUTRO:return "CELEBRATION";case RESULT:return game.won?"ESCAPE COMPLETE":"BACK TO THE BENCH";default:return "DESERT GOAT";}
}
}
