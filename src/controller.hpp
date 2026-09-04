#pragma once
#include "presentation.hpp"
inline void Game::navigateMenu(int code){
 if(state==PLAYING||state==TITLE)return;keyboardFocus=true;render();
 int current=-1;for(int i=0;i<buttonCount;i++)if(buttons[i].id==focusedButton&&buttons[i].enabled)current=i;
 if(code==13){if(current>=0)action(buttons[current].id);return;}
 if(current<0){for(int i=0;i<buttonCount;i++)if(buttons[i].enabled){focusedButton=buttons[i].id;return;}return;}
 // Move between cards; crossing a row edge reveals the adjacent stage page.
 if(state==STAGE_SELECT&&focusedButton>=100&&focusedButton<100+STAGE_COUNT&&(code==37||code==39)){
  int next=focusedButton-100+(code==37?-1:1);if(next>=0&&next<STAGE_COUNT){stagePage=next/3;focusedButton=100+next;}return;
 }
 if(state==LOADOUT&&code==40&&focusedButton>=200&&focusedButton<204){focusedButton=pendingSkill>=0?300:2;return;}
 if(state==LOADOUT&&code==38&&(focusedButton==300||focusedButton==2)){focusedButton=200+(pendingSkill>=0?pendingSkill:0);return;}
 auto& from=buttons[current];float fx=from.x+from.w*.5f,fy=from.y+from.h*.5f,best=1e9f;int next=-1;
 for(int i=0;i<buttonCount;i++){auto& b=buttons[i];if(i==current||!b.enabled)continue;float dx=b.x+b.w*.5f-fx,dy=b.y+b.h*.5f-fy,along=code==37?-dx:code==39?dx:code==38?-dy:dy,cross=code==37||code==39?absf(dy):absf(dx);if(along<4)continue;float score=along+cross*3;if(score<best){best=score;next=i;}}
 if(next>=0)focusedButton=buttons[next].id;
}
inline void Game::pointer(int type,int x,int y){
 mouseX=x;mouseY=y;if(type!=0)return;keyboardFocus=false;
 if(state==TITLE){for(int i=0;i<buttonCount;i++){auto& b=buttons[i];if(b.id==4&&x>=b.x&&x<b.x+b.w&&y>=b.y&&y<b.y+b.h){action(4);return;}}if(titleTime>=TITLE_READY&&curtain<0)curtain=0;return;}
 for(int i=buttonCount-1;i>=0;i--){auto& b=buttons[i];if(x>=b.x&&x<b.x+b.w&&y>=b.y&&y<b.y+b.h){if(b.enabled)action(b.id);return;}}
}
inline void Game::action(int id){
 if(id==4){muted=!muted;if(muted)host_voice(0);return;}
 if(id==1&&state==TITLE){if(titleTime>=TITLE_READY&&curtain<0)curtain=0;return;}
 if(id==7){skip();return;}if(id==8&&state==PAUSED){state=pausedState;clearKeys();return;}
 if(state==WARNING){if(id==14)dismissWarning();return;}if(state==PAUSED&&pausedState==WARNING)return;
 if(state==INTRO||state==OUTRO||state==MORGAN||state==MORGAN_UNLOCK||state==BOSS_DEMO||state==PAUSED&&(pausedState==MORGAN||pausedState==MORGAN_UNLOCK||pausedState==BOSS_DEMO))return;
 if(id==5){special();return;}if(id==6){super();return;}
 if((id==12||id==13)&&(state==STAGE_SELECT||state==LOADOUT)){int& page=state==STAGE_SELECT?stagePage:skillPage;page=maxi(0,mini(state==STAGE_SELECT?(STAGE_COUNT-1)/3:0,page+(id==12?-1:1)));if(state==STAGE_SELECT){focusedButton=100+stagePage*3;focusState=state;}return;}
 if(id==1){if(state!=STAGE_SELECT&&state!=LOADOUT)returnState=state;state=STAGE_SELECT;clearKeys();return;}
 if(id==2){if(state==LOADOUT){state=STAGE_SELECT;pendingSkill=-1;}else if(state==STAGE_SELECT)state=returnState;clearKeys();return;}
 if(id>=100&&id<100+STAGE_COUNT&&state==STAGE_SELECT){choose(id-100);return;}
 if(id>=200&&id<200+SKILL_COUNT&&state==LOADOUT){pendingSkill=id-200;return;}
 if(id==300&&state==LOADOUT&&pendingSkill>=0&&pendingSkill<SKILL_COUNT){start(pendingStage,pendingSkill);return;}
 if(id==9&&state==RESULT){choose(stage);return;}if(id==10&&state==RESULT&&won&&stage<STAGE_COUNT-1){choose(stage+1);return;}
 if(id==11&&(state==RESULT||state==STAGE_SELECT)){returnState=state;beginOut(true);return;}
}
