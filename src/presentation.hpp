#pragma once
#include "game.hpp"
inline float uiScale(){return minf(float(WIDTH)/960,float(HEIGHT)/720);}
inline float uiLeft(){return (WIDTH-960*uiScale())/2;}
inline float uiTop(){return (HEIGHT-720*uiScale())/2;}
static const char* ronHead[16]={".....KK....KK...","...KKDKKKKKDKK..","..KKDDKKKKKKKK..","..KKKKKKKKKKKK..","..KKSSSSSSSSKK..","..KSSLLLLLSSSK..","..KSSLLLLLLSSK..",".SSSKKSSSSKKSSS.",".SSSKLSSSSLKSSS.","..SSSLLSSLLSSS..","..SSSLLSSLLSSS..","...SSSSDDSSSS...","...SSLLLLLLSS...","....SSDKKDSS....",".....SSSSSS.....","......DDDD......"};
static const char* tenHead[16]={"......DDDD......","....DDLLLLDD....","...DLLWWLLLLD...","..DLLWWLLLLLLD..","..DLLLLLLLLLLD..","..DLLLLLLLLSSD..","..DSSLLLLLLSSD..",".SSSDDSSSSDDSSS.",".SSSKLSSSSLKSSS.","..SSSLLSSLLSSS..","..SDDLLSSLLDDS..","...DDSSDDSSDD...","...DDDLLLLDDD...","....DDDKKDDD....",".....DDDDDD.....","......DDDD......"};
static const char* sideHead[16]={".....KKKKK......","...KKKKKKKK.....","..KKKKKKKKKK....","..KKKKKKKKKK....","..KKSSSSSSSS....","..KKSSLLLLSS....","..KKSSLLLLSS....","..KSSSLLKKSS....","..KSSSLLLKSS....","..SSSLLSSSSSS...","..SSSLLSSSSSS...","...SSLLSSSS.....","...SSSSSDDS.....","....SSSSSS......",".....SSSS.......","......DD........"};
inline int palette(char c){switch(c){case 'K':return 23;case 'D':return 68;case 'G':return 136;case 'S':return 187;case 'L':return 221;case 'W':return 255;default:return -1;}}
inline void Game::actor(const Unit& u,float x,float y,float scale,int rival,int pose,float phase,bool angry){
 float ox=r.tx,oy=r.ty,os=r.scale;r.transform(ox+x*os,oy+y*os,os*scale*(rival==8?1.12f:rival==6?1.08f:rival==7?.94f:1));
 bool side=u.dir==1||u.dir==3,back=u.dir==0,mirror=u.dir==3;bool enemy=rival>=0;int kit=enemy?(rival==2?136:rival==7?235:rival==6?119:rival==3?102:68):255;
 float stride=pose==1?sine(phase*18)*5:sine(u.walk/.2f*PI)*4*(u.gait%2?1:-1),kick=sine(clamp(u.kick/.2f,0,1)*PI)*8;
 auto part=[&](float px,float py,float w,float h,int c){float drawX=mirror?-px-w:px;if(angry&&enemy&&rival>=7&&c>30){int red=mini(255,125+c/2),green=18+c/10,blue=15+c/14;r.colorRect(drawX,py,w,h,0xff000000u|u32(red)|u32(green)<<8|u32(blue)<<16);}else r.rect(drawX,py,w,h,c);};
 // One pose owns BOTH arms. Never layer celebration limbs on an idle body.
 lastArmCount=0;int legGap=pose==3?13:(side?4:8);
 part(-18,3,36,4,17);
 if(pose==8){part(-13,-14,12,7,enemy?85:221);part(3,-14,12,7,enemy?85:221);part(-16,-9,6,13,enemy?85:221);part(10,-9,6,13,enemy?85:221);part(-20,2,10,4,23);part(10,2,10,4,23);}
 else if(pose==2){part(-12,-18,8,8,221);part(4,-18,8,8,221);part(-16,-14,12,4,23);part(8,-14,12,4,23);}
 else{part(-legGap-3,-14-stride,6,16,enemy?85:221);part(legGap-3+(side?kick:0),-14+stride,6,16,enemy?85:221);part(-legGap-5,2-stride,10,4,23);part(legGap-3+(side?kick:0),2+stride,10,4,23);part(-legGap-5,2-stride,8,2,136);part(legGap-3+(side?kick:0),2+stride,8,2,136);}
 int width=side?18:28;part(-width/2,-36,width,24,23);part(-width/2+2,-36,width-4,20,kit);part(-width/2+2,-16,width-4,6,23);
 for(int arm=0;arm<2;arm++){int sign=arm?1:-1;lastArmCount++;
  if(pose==5||pose==7){float swing=sine(clamp(phase,0,1)*PI);if(arm){r.line(12,-32,22,-34-18*swing,kit,6);r.line(22,-34-18*swing,27+12*phase,-40-24*swing,187,6);}else{part(-18,-32,6,12,kit);part(-19,-22,7,8,187);}}
  else if(pose==6){if(arm){float thrust=sine(clamp(phase,0,1)*PI);r.line(10,-32,26+12*thrust,-28,kit,7);r.line(26+12*thrust,-28,18,-40,187,6);}else{part(-17,-34,6,10,kit);part(-15,-25,10,6,187);}}
  else if(pose==8){float talk=arm?sine(phase*8)*3:0;r.line(sign*12,-32,sign*20,-21-talk,kit,5);r.line(sign*20,-21-talk,sign*12,-18-talk,187,5);}
  else if(pose==9){part(sign<0?-16:10,-32,6,10,kit);part(sign<0?-12:4,-27,8,6,187);}
  else if(pose==10){r.line(sign*12,-32,sign*28,-30,kit,6);part(sign<0?-35:28,-33,7,6,187);}
  else if(pose==11){float reach=10+8*sine(phase*8);r.line(sign*12,-32,sign*16+reach,-37,kit,6);r.line(sign*16+reach,-37,sign*12+reach+9,-43,187,5);}
  else if(pose==12){if(arm){r.line(12,-32,25,-38,kit,6);r.line(25,-38,34,-43,187,5);}else{part(-18,-34,6,13,kit);part(-18,-22,6,7,187);}}
  else if(pose==13){r.line(sign*12,-32,sign*23,-39,kit,6);r.line(sign*23,-39,sign*19,-52,187,5);part(sign*19-4,-58,8,8,187);}
  else if(pose==14){r.line(sign*10,-32,sign*26,-39,kit,6);part(sign*26-5,-46,10,12,255);}
  else if(pose==3){part(sign<0?-23:14,-32,9,6,kit);part(sign<0?-28:21,-28,7,7,kit);part(sign<0?-30:24,-22,6,8,187);}
  else if(pose==4){float lift=clamp(phase,0,1),elbow=sign*(20-6*lift),hand=sign*(19-9*lift);r.line(sign*14,-33,elbow,-24-20*lift,kit,5);r.line(elbow,-24-20*lift,hand,-18-43*lift,187,5);}
  else if(pose==2){part(sign<0?-18:12,-46,6,15,kit);part(sign<0?-18:12,-54,6,8,187);}
  else if(side){float ax=arm?0:-9;part(ax,-33,6,12,kit);part(ax+(arm?stride:-stride),-22-kick*.3f,6,8,enemy&&rival==2?255:187);}
  else{float ax=sign<0?-18:12;part(ax,-34,6,12,kit);part(ax,-22+sign*stride-kick*.3f,6,8,enemy&&rival==2?255:187);}
 }
 if(enemy){
  if(rival==2||rival==6||rival==7||rival==8){if(rival==7){part(-9,-36,5,21,119);part(5,-36,5,21,119);}if(rival==8){part(-12,-36,24,19,205);part(-12,-36,24,3,23);}if(!side)text(r,rival==8?"5":rival==7?"10":rival==6?"3":"1",rival==7?-9:-4,-32,rival==7?1:2,23);}
  else if(rival==3){part(14,-32,3,14,187);part(10,-39,11,9,23);part(12,-39,7,2,187);}
  else if(rival==9){part(-4,-36,8,4,255);part(0,-32,2,12,187);part(10,-26,3,11,160);part(8,-31,7,7,23);part(9,-31,5,2,200);}
  else{if(!back){part(side?5:-4,-36,side?2:8,4,255);part(side?5:0,-32,2,10,187);}if(back)part(-2,-34,2,16,102);int bx=side?5:back?-26:16;part(bx,-28-kick,12,20,23);part(bx+2,-26-kick,8,16,221);part(bx+4,-24-kick,2,12,136);part(bx+2,-20-kick,8,2,136);}
 }
 else if(!side){text(r,"7",-4,-32,2,23);if(back)part(-6,-35,12,2,136);}
 int headShift=enemy&&phase!=0&&pose==0?roundi(sine(phase*9)*2):0;
 for(int row=0;row<16;row++)for(int col=0;col<16;col++){
  char c=(side?sideHead:enemy&&rival!=2?tenHead:ronHead)[row][col];
  if(rival==8){if(!side&&row>=4&&row<=13)c=col>=2&&col<=13?(row<7?'L':'S'):'.';if(!side&&row==8&&(col==5||col==10))c='K';if(!side&&row==12&&col>=6&&col<=9)c='D';}
  if(enemy&&rival>0&&rival!=4&&!side&&row<4){c=(col>=3&&col<=12)?(rival==5?'G':'K'):'.';if(row==0&&(col<5||col>10))c='.';}
  if(side&&enemy&&(rival==0||rival==4)&&row<8&&c=='K')c='S';if(side&&enemy&&rival!=2&&row>=11&&row<=14&&c=='S')c='D';
  if(enemy&&rival==5&&((row<5&&c=='K')||(row>=11&&c=='D')))c='G';
  if(enemy&&rival==7&&row>=10&&row<15&&c=='D')c='K';
  if(back){if(row<6&&!(enemy&&rival==0)&&(c=='S'||c=='L'||c=='W'))c='K';if(row>=6&&row<13)c=col>=2&&col<14?(enemy&&rival==0?'L':'K'):'.';if(row>=13)c=row<15?(col>=4&&col<12?'S':'.'):(col>=6&&col<10?'D':'.');}
  int color=palette(c);if(color>=0)part(-16+col*2+headShift,-66+row*2,2,2,u.hitFlash>0&&int(u.hitFlash*30)%2==1?255:color);
 }
 if(angry&&!side&&!back){part(-10,-52,4,2,23);part(-6,-50,4,2,23);part(6,-52,4,2,23);part(2,-50,4,2,23);part(18,-64,2,8,255);part(15,-61,8,2,255);}
 if(rival==9&&!side&&!back){part(-11,-62,22,4,150);part(-12,-51,10,6,23);part(2,-51,10,6,23);part(-2,-49,4,2,23);part(-9,-49,5,2,210);part(4,-49,5,2,210);}
 // Restore the original pixel face and its celebration close-up.
 if(!enemy&&scale>=2.8f&&!side&&!back){
  part(-11,-58,22,5,204);part(-9,-57,18,3,233);part(-11,-52,7,2,28);part(4,-52,7,2,28);
  part(-10,-49,6,3,248);part(4,-49,6,3,248);part(-7,-49,2,3,23);part(5,-49,2,3,23);
  part(-2,-49,3,8,233);part(1,-45,2,5,160);part(-3,-41,6,2,163);
  part(-12,-45,3,7,170);part(9,-45,3,7,170);part(-9,-38,18,2,202);
  if(pose!=3){part(-4,-38,8,1,90);part(-2,-37,4,1,238);}
 }
 if(pose==3&&!side&&!back){float mouth=clamp(phase,0,1);int mh=6+roundi(mouth*4);part(-4,-44,8,mh,96);part(-6,-42,12,mh-4,96);part(-2,-42,4,mh-3,12);part(-4,-40,8,mh-6,12);part(-2,-44+mh-2,4,1,180);}
 if(pose==8&&phase>0&&!side&&!back){part(-4,-40,8,2+roundi(absf(sine(phase*10))*3),23);}
 r.transform(ox,oy,os);
}
inline void Game::button(int id,int x,int y,int w,int h,const char* label,bool enabled,bool selected,int size){
 if(buttonCount<48){Button& b=buttons[buttonCount++];b.id=id;b.x=roundi(r.tx+x*r.scale);b.y=roundi(r.ty+y*r.scale);b.w=roundi(w*r.scale);b.h=roundi(h*r.scale);b.enabled=enabled;copystr(b.label,label,80);}
 if(id==4){copystr(buttons[buttonCount-1].label,muted?"Sound off - unmute":"Sound on - mute",80);float nx=x+w/2-8,ny=y+h/2-9;r.rect(nx+7,ny,3,15,muted?120:255);r.rect(nx+9,ny,9,3,muted?120:255);r.rect(nx,ny+12,9,6,muted?120:255);if(muted)r.line(nx-2,ny+20,nx+20,ny-2,190,2);return;}
 bool hover=enabled&&mouseX>=r.tx+x*r.scale&&mouseX<r.tx+(x+w)*r.scale&&mouseY>=r.ty+y*r.scale&&mouseY<r.ty+(y+h)*r.scale;r.rect(x,y,w,h,selected?235:hover?64:30);r.border(x,y,w,h,selected?255:enabled?145:65,2);while(size>1&&strlength(label)*6*size>w-12)size--;text(r,label,x+w/2,y+(h-7*size)/2,size,selected?17:enabled?255:110,true);
}
inline void Game::trophy(float x,float y,float scale){
 float ox=r.tx,oy=r.ty,os=r.scale;r.transform(ox+x*os,oy+y*os,os*scale);
 r.colorRect(-15,-4,30,5,0xff1b82c4u);r.colorRect(-10,-7,20,3,0xff50dfff);
 r.colorRect(-3,-17,6,10,0xff32bdf5u);r.colorRect(-10,-29,20,12,0xff30ccffu);r.colorRect(-8,-17,16,3,0xff26a4e6u);
 r.colorRect(-11,-31,22,3,0xff9bf5ffu);r.colorRect(-8,-28,4,8,0xffb5faffu);
 r.colorRect(-16,-29,5,3,0xff54dcffu);r.colorRect(-16,-26,3,9,0xff54dcffu);r.colorRect(-13,-19,5,3,0xff54dcffu);
 r.colorRect(11,-29,5,3,0xff54dcffu);r.colorRect(13,-26,3,9,0xff54dcffu);r.colorRect(8,-19,5,3,0xff54dcffu);
 for(int i=0;i<5;i++){float a=clock*1.7f+i*2*PI/5,rad=22+3*sine(clock*3+i);r.colorRect(cosine(a)*rad,-20+sine(a)*rad,2,2,0xffb7f7ffu);}
 r.transform(ox,oy,os);
}
inline void Game::drawWorld(bool actors){
 r.clip(0,0,WIDTH,HEIGHT);
 int zRadius=HEIGHT/44+8,xRadius=WIDTH/50+zRadius/3+8;
 int centerZ=(state==OUTRO||state==PAUSED&&pausedState==OUTRO)?outroHero.z:player.z;
 int centerX=(state==OUTRO||state==PAUSED&&pausedState==OUTRO)?outroHero.x:player.x;
 int z0=infinite?centerZ-zRadius:0,z1=infinite?centerZ+zRadius:ROWS-1;
 int x0=infinite?centerX-xRadius:0,x1=infinite?centerX+xRadius:COLS-1;
 for(int z=z0;z<=z1;z++)for(int x=x0;x<=x1;x++)if(!wallAt(x,z)){Point p[4]={project(x,0,z),project(x+1,0,z),project(x+1,0,z+1),project(x,0,z+1)};r.polygon(p,4,((x+z)&1)?32:29);}
 auto panel=[&](float ax,float az,float bx,float bz){Point a=project(ax,0,az),b=project(bx,0,bz),at=project(ax,1.35f,az),bt=project(bx,1.35f,bz);Point face[4]={a,b,bt,at};r.polygon(face,4,83);r.line(at.x,at.y,bt.x,bt.y,216,1);r.line(a.x,a.y,at.x,at.y,143,1);};
 for(int z=z0;z<=z1;z++){
  if(infinite){int rz=floorDiv(z+2,4);
   for(int rx=floorDiv(x0+2,4);rx<=floorDiv(x1+2,4)+1;rx++){
    int edge=rx*4-2;
    if(!world.linked({rx-1,rz},{rx,rz}))panel(edge,z,edge,z+1);
    if(z==rz*4-2&&!world.linked({rx,rz-1},{rx,rz}))panel(edge,z,edge+4,z);
   }
  }else for(int x=x0;x<=x1;x++)if(wallAt(x,z)){Point p[4]={project(x,1,z),project(x+1,1,z),project(x+1,1,z+1),project(x,1,z+1)};r.polygon(p,4,230);}
  if(actors){
   if(infinite&&goalZ==z){Point p=project(goalX+1,0,goalZ+1);r.rect(p.x-28,p.y-2,56,9,95);r.colorRect(p.x-28,p.y-4,56,2,0xff56d9ffu);trophy(p.x,p.y-8,1);}
   for(int i=0;i<enemyCount;i++)if(enemies[i].z==z&&!enemies[i].dead){Point p=project(enemies[i].x+1,.04f,z+1);if(enemies[i].spawnFlash<=0||int(enemies[i].spawnFlash*8)%2==0){Unit rival=enemies[i];if(rival.hitFlash>0){float recoil=sine(rival.hitFlash/.18f*PI)*6;p.x-=DX[rival.dir]*recoil;p.y-=DZ[rival.dir]*recoil;}int shotPose=rival.kick>0?(stage==0||stage==4||stage==5?12:stage==1?13:stage==2?14:stage==7?11:10):0;float phase=1-bossWindup/maxf(.01f,bossWindupTotal);if(rival.boss&&bossWindup>0&&stage==8)rival.dir=(bossAttackDir+int(phase*3))%4;actor(rival,p.x,p.y,rival.boss?1.65f:1,stage,rival.boss&&bossWindup>0?(stage==7?11:stage==6?2:0):shotPose,bossWindup>0?clock:0,rival.boss&&bossRage);}if(enemies[i].spawnFlash>0){r.border(p.x-20,p.y-4,40,9,210,1);text(r,"!",p.x,p.y-92,2,255,true);}}
   if(player.z==z&&(state!=PLAYING||dashLeft||specialAnim>0||shielding()||player.inv<=0||int(player.inv*10)%2==0)){Point p=project(player.x+1,.04f,z+1);Unit hero=player;int pose=shielding()?9:0;float phase=1-specialAnim;if(specialAnim>0){pose=animSkill==0?5:animSkill==1?10:animSkill==2?7:6;if(animSkill==1)hero.dir=int(phase*16)%4;}actor(hero,p.x,p.y,1,-1,pose,phase);}
   for(int i=0;i<ballCount;i++){auto& b=balls[i];if(int(__builtin_floorf(b.z))!=z)continue;Point p=project(b.x,.22f,b.z);int white=b.enemy?85:255,black=b.enemy?230:23;r.rect(p.x-5,p.y-7,10,14,white);r.rect(p.x-7,p.y-5,14,10,white);r.rect(p.x-3,p.y-3,6,6,black);}
  }
 }
 if(actors){
  if(bossWindup>0){const Unit* b=finalBoss();float cx=stage>=7&&b?b->x+1:bossTargetX,cz=stage>=7&&b?b->z+1:bossTargetZ;
   if(stage==4){for(int axis=0;axis<2;axis++)for(int side=-1;side<=1;side+=2){Point a=project(cx+(axis?side*1.15f:-5),.04f,cz+(axis?-5:side*1.15f)),end=project(cx+(axis?side*1.15f:5),.04f,cz+(axis?5:side*1.15f));r.colorLine(a.x,a.y,end.x,end.y,0xff2424ffu);}}
   else for(int circle=0;circle<(stage==6?5:1);circle++){float dx=circle==1?-3:circle==2?3:0,dz=circle==3?-3:circle==4?3:0,rad=stage==6?1.5f:stage==7?3.5f:stage==8?4.8f:2.7f;for(int j=0;j<64;j++){float a=j*2*PI/64;if(stage==8){float fx=cosine(a),fz=sine(a),front=fx*DX[bossAttackDir]+fz*DZ[bossAttackDir],side=fx*DZ[bossAttackDir]-fz*DX[bossAttackDir];if(front>absf(side)*1.6f)continue;}Point p=project(cx+dx+cosine(a)*rad,.04f,cz+dz+sine(a)*rad);r.colorRect(p.x-2,p.y-2,4,4,0xff2424ffu);}}
   Point p=project(cx,.1f,cz);text(r,stage==3?"BLOCK OR DODGE":stage==8?"TURNING!":stage==7?"KEEP AWAY!":"MOVE!",p.x,p.y-20,2,255,true);
   if(b){Point head=project(b->x+1,5.8f,b->z+1);float beat=1-bossWindup/bossWindupTotal;
    if(stage==3){for(int j=0;j<3;j++){float sx=head.x+(j-1)*76,sy=head.y-25-absf(sine(beat*6+j))*15;r.rect(sx-34,sy,68,21,235);text(r,j==1?"OVERRATED":"BLAH BLAH",sx,sy+6,1,23,true);r.rect(sx,sy+21,6,5,235);}}
    if(stage==4){r.rect(head.x-52,head.y-38,104,63,22);r.border(head.x-52,head.y-38,104,63,180);for(int j=0;j<5;j++){float sx=head.x-40+j*20,sy=head.y-23+(j%2)*25;text(r,j%2?"X":"O",sx,sy,1,255);if(j<4)r.line(sx+4,sy+4,sx+24,head.y-19+((j+1)%2)*25,130,1);}}
    if(stage==5){r.rect(head.x-51,head.y-38,102,55,23);r.border(head.x-51,head.y-38,102,55,210,2);text(r,"7  OUT",head.x,head.y-27,2,255,true);r.line(head.x-28,head.y+1,head.x+28,head.y+1,180,3);r.line(head.x+28,head.y+1,head.x+18,head.y-8,180,3);}
    if(stage==6){for(int j=0;j<5;j++){float dx=j==1?-3:j==2?3:0,dz=j==3?-3:j==4?3:0;Point palm=project(cx+dx,1.4f*(1-beat)+.2f,cz+dz);r.rect(palm.x-9,palm.y-9,18,17,230);for(int finger=0;finger<5;finger++)r.rect(palm.x-10+finger*5,palm.y-22+(finger==0||finger==4?5:0),4,15,230);}}
   }
  }
  if(shielding()||state==PAUSED&&keys[67]){float dx=DX[player.dir],dz=DZ[player.dir],x=player.x+1+dx*1.2f,z=player.z+1+dz*1.2f;Point a=project(x-dz*1.2f,.1f,z+dx*1.2f),b=project(x+dz*1.2f,.1f,z-dx*1.2f),c=project(x+dz*1.2f,2.4f,z-dx*1.2f),d=project(x-dz*1.2f,2.4f,z+dx*1.2f);Point face[4]={a,b,c,d};r.polygon(face,4,shieldFlash>0?235:85);r.line(a.x,a.y,b.x,b.y,255,3);r.line(b.x,b.y,c.x,c.y,255,3);r.line(c.x,c.y,d.x,d.y,255,3);r.line(d.x,d.y,a.x,a.y,255,3);r.line((a.x+b.x)/2,(a.y+b.y)/2,(c.x+d.x)/2,(c.y+d.y)/2,180,2);}
  if(healFlash>0){Point p=project(player.x+1,4.8f+(1.4f-healFlash)*.6f,player.z+1);text(r,fullHealNotice?"FULL HEAL":"+1 HP",p.x,p.y,2,255,true);}
  for(int i=0;i<effectCount;i++){auto& e=effects[i];bool phone=e.type==2;if(e.type==3){float reach=1+4*smooth(e.age/.35f);for(int j=-5;j<=5;j++){float side=j*.28f;Point p=project(e.x+e.tx*reach-e.tz*side,1.2f,e.z+e.tz*reach+e.tx*side);r.rect(p.x-4,p.y-3,8,6,roundi(255*(1-e.age/.7f)));}Point p=project(e.x+e.tx*2,3.3f,e.z+e.tz*2);text(r,"CRACK!",p.x,p.y,2,255,true);continue;}if(!phone||e.impacted){float phase=phone?(e.age-.4f)/.8f:e.age/e.duration,radius=(phone?3.7f:e.type==0?5:2.7f)*minf(1,phase*1.5f);for(int j=0;j<48;j++){float a=j*2*PI/48;Point p=project((phone?e.tx:e.x)+cosine(a)*radius,.1f,(phone?e.tz:e.z)+sine(a)*radius);r.rect(p.x-2,p.y-2,4,4,roundi(255*(1-phase)));}}
   if(!phone&&e.type==0&&e.age<.45f){Point p=project(e.x,maxf(0,1.6f-e.age*7),e.z);r.rect(p.x-16,p.y-16,32,26,255);r.border(p.x-16,p.y-16,32,26,23,4);text(r,"C",p.x-7,p.y-11,2,23);}
   if(phone&&!e.impacted){float t=minf(1,e.age/.4f);Point p=project(mix(e.x,e.tx,t),sine(t*PI)*2+.2f,mix(e.z,e.tz,t));r.rect(p.x-10,p.y-14,20,28,23);r.rect(p.x-6,p.y-10,12,20,255);r.rect(p.x-2,p.y+12,4,2,255);Point target=project(e.tx,0,e.tz);r.line(target.x-9,target.y,target.x+9,target.y,255);r.line(target.x,target.y-9,target.x,target.y+9,255);}
   if(phone&&e.impacted){for(int j=0;j<7;j++){float a=j*PI*2/7,d=(e.age-.4f)*4;Point p=project(e.tx+cosine(a)*d,.1f+maxf(0,.5f-d*.2f),e.tz+sine(a)*d);r.rect(p.x,p.y,5,8,210);r.line(p.x,p.y,p.x+5,p.y+8,23,2);}}
  }
  for(int i=0;i<particleCount;i++){auto& e=particles[i];Point p=project(e.x,e.y,e.z);r.rect(p.x-2,p.y-2,4,4,190);}
  for(int i=-1;i<enemyCount;i++){Unit& u=i<0?player:enemies[i];Point p=project(u.x+1,.04f,u.z+1);int labelY=u.boss?140:87;r.rect(p.x-46,p.y-labelY,92,13,17);text(r,i<0?"RONALDO":u.boss?stages[stage].name:stages[stage].name,p.x,p.y-labelY+3,1,255,true);
   if(u.boss){float hp=clamp(float(u.hp)/maxf(1.f,float(bossMaxHp())),0,1);r.rect(p.x-42,p.y-124,84,9,17);r.border(p.x-42,p.y-124,84,9,255,1);r.colorRect(p.x-39,p.y-121,78*hp,3,0xffff8a28u);if(hp>0)r.colorRect(p.x-39,p.y-121,78*hp,1,0xffffd080u);}
   if(u.stun>0)text(r,"* * *",p.x,p.y-labelY-14,1,255,true);if(u.seal>0){r.border(p.x-44,p.y-116,88,119,210,2);for(int bar=-2;bar<=2;bar++)r.line(p.x+bar*16,p.y-116,p.x+bar*16,p.y+3,200,2);char label[24]="SEALED ",seconds[8];number(seconds,maxi(1,int(u.seal+.99f)));append(label,seconds,24);append(label,"S",24);text(r,label,p.x,p.y-155,2,255,true);}}
 }
 r.clip(0,0,WIDTH,HEIGHT);
}

inline void Game::drawIntro(){
 float t=cinema;bool press=stage==0;float transition=press?4.2f:.3f,zoom=smooth((t-transition)/((press?7.4f:3.2f)-transition));
 if(press&&t<transition){for(int y=130;y<400;y+=54)for(int x=280;x<880;x+=100)r.rect(x,y,70,28,37);text(r,"PRESS ROOM",480,86,2,170,true);Unit u=player;u.dir=2;u.walk=u.kick=0;actor(u,610,508,4,0,0,t>.45f&&t<2.9f?t:0);r.rect(290,430,565,12,68);r.rect(290,442,565,150,37);r.rect(559,387,5,43,170);r.rect(543,382,23,8,170);r.rect(543,426,38,4,170);r.rect(470,471,202,36,255);text(r,"TEN HAG",571,481,2,23,true);if(t>=.45f)text(r,"\"RONALDO IS NOT IN MY PLANS.\"",480,133,3,255,true);
  if(t>=2.1f){float pop=smooth((t-2.1f)/.75f);actor(u,171,838-pop*245,3.8f,-1,0,0,true);r.rect(75,517,198,123,22);if(t>2.85f)text(r,"?!",174,295,4,255,true);}text(r,"FICTIONAL PARODY",480,610,1,119,true);
 }else{Point p=project(player.x+1,.04f,player.z+1);float s=mix(3.7f*uiScale(),1,zoom),tx=mix(WIDTH*.5f,p.x,zoom),ty=mix(uiTop()+565*uiScale(),p.y,zoom);r.transform(tx-p.x*s,ty-p.y*s,s);drawWorld(false);Unit u=player;u.dir=zoom<.65f?2:player.dir;actor(u,p.x,p.y,1,-1,0,0,press&&zoom<.5f);r.transform(uiLeft(),uiTop(),uiScale());text(r,stages[stage].name,480,86,3,255,true);text(r,arenaNames[stage],480,125,2,170,true);if(stage>=5)text(r,"NARROW GAPS CAN LEAD AROUND",480,565,2,190,true);text(r,skills[equipped].name,480,606,2,190,true);}
 button(7,768,589,170,36,"SKIP / ENTER",true,false,2);
}
inline void Game::drawOutro(){
 float t=cinema,u=uiScale(),cx=WIDTH*.5f,stageY=HEIGHT*.61f,finishY=HEIGHT*.84f;
 Unit hero=outroHero;hero.walk=hero.kick=0;
 float zoom=smooth(t/1.9f),size=mix(1,2.8f*u,zoom),x=cx,y=stageY;int pose=0;float posePhase=0;
 float cupX=cx+61.6f*u,cupY=stageY,cupScale=1.54f*u;
 if(t<OUTRO_RUN){
  Camera saved=cam;cam=outroCamera;Point p=project(outroHero.x+1,.04f,outroHero.z+1);
  x=mix(p.x,cx,zoom);y=mix(p.y,stageY,zoom);
  if(t<3.5f){r.transform(x-p.x*size,y-p.y*size,size);drawWorld(false);r.transform();r.fadeToBlack(smooth((t-.65f)/2.85f));}else r.clear(0);
  cam=saved;hero.dir=zoom<.35f?outroHero.dir:2;
  float lift=t<2.65f?smooth((t-1.8f)/.85f):1-smooth((t-3.35f)/.85f);
  if(t>=1.8f){hero.dir=2;pose=4;posePhase=lift;}
  cupX=x+mix(22,0,lift)*size;cupY=y-lift*size*66;cupScale=mix(size*.55f,1.3f*u,lift);
 }else{
  r.clear(0);hero.dir=2;
  if(t<OUTRO_JUMP){float run=(t-OUTRO_RUN)/(OUTRO_JUMP-OUTRO_RUN);x=cx+sine(run*PI*8)*4*u*sine(run*PI);y=mix(stageY,finishY,smooth(run))-absf(sine(run*PI*8))*9*u;size=mix(2.8f,4.2f,run)*u;pose=1;posePhase=t-OUTRO_RUN;}
  else if(t<OUTRO_LAND){float jump=(t-OUTRO_JUMP)/(OUTRO_LAND-OUTRO_JUMP);y=finishY-sine(jump*PI)*HEIGHT*.2f;size=mix(4.2f,5.4f,smooth(jump))*u;int turn[5]={2,1,0,3,2};hero.dir=turn[mini(4,int(smooth(jump)*5))];pose=2;}
  else{float land=t-OUTRO_LAND;x=cx+sine(land*55)*maxf(0,1-land*3)*4*u;y=finishY+maxf(0,1-land*4)*7*u;size=(5.4f+.35f*smooth(land/1.1f))*u;pose=3;posePhase=smooth(land/.28f);for(int i=0;i<28;i++){float angle=i*2*PI/28,rad=(45+minf(1,land*2)*145)*u;r.rect(cx+cosine(angle)*rad,finishY+8*u+sine(angle)*rad*.18f,6*u,3*u,roundi(190*maxf(0,1-land/1.4f)));}}
 }
 if(t>=OUTRO_RUN){r.rect(cupX-22*u,cupY,44*u,4*u,68);trophy(cupX,cupY,cupScale);}
 actor(hero,x,y,size,-1,pose,posePhase);
 if(t<OUTRO_RUN)trophy(cupX,cupY,cupScale);
 if(t>=OUTRO_SHOUT)text(r,"SIUUUUU!",cx,HEIGHT*.12f,maxi(2,roundi(9*u)),255,true);
 r.transform(uiLeft(),HEIGHT-720*u,u);button(7,768,669,170,34,"SKIP / ENTER",true,false,2);
 r.transform(uiLeft(),0,u);button(4,874,11,66,34,muted?"OFF":"SND");r.transform();
}
inline void Game::drawBossDemo(){
 r.transform();r.clear(0);float t=cinema,u=uiScale(),reveal=smooth(t/.8f),attack=smooth((t-3.2f)/1.4f);
 r.transform(uiLeft(),uiTop(),u);
 text(r,"KNOW YOUR RIVAL",480,64,2,160,true);text(r,stages[stage].name,480,104,4,255,true);
 text(r,bossSkills[stage],480,154,3,235,true);
 r.rect(80,413,800,4,70);for(int x=80;x<880;x+=40)r.rect(x,417,20,2,40);
 Unit boss,hero;boss.dir=2;hero.dir=3;int pose=stage==3?8:stage==4||stage==5?12:stage==6?2:stage==7?11:10;
 float bx=stage==7?mix(280,460,attack):280,hx=stage==7?mix(635,760,attack):635,hy=412;
 if(stage==8)boss.dir=(2+int(attack*2))%4;
 if(stage==4||stage==6)hy-=sine(attack*PI)*70;
 actor(boss,bx,410,3.0f*reveal,stage,pose,t);
 actor(hero,hx,hy,1.65f,-1,t>3.2f?stage==3||stage==5?9:1:0,t);
 if(stage==3){
  r.rect(130,203,300,41,28);text(r,t<3.2f?"BLAH BLAH BLAH":"FIVE-SHOT RANT!",280,218,2,255,true);
  if(t>3.2f)for(int i=-2;i<=2;i++){float p=clamp((t-3.2f)/1.2f,0,1);float x=mix(325,620,p),y=350+i*35*p;r.rect(x-5,y-5,10,10,230);}
 }else if(stage==4){
  r.rect(185,208,190,72,24);r.border(185,208,190,72,170,2);for(int i=0;i<5;i++){text(r,i%2?"X":"O",201+i*33,225+(i%2)*24,2);if(i<4)r.line(211+i*33,230+(i%2)*24,244+i*33,230+((i+1)%2)*24,110,2);}
  r.border(570,405,130,18,170,2);r.border(625,360,20,110,170,2);if(t>3.2f){r.rect(570,405,130,18,roundi(220*(1-attack)+35));r.rect(625,360,20,110,roundi(220*(1-attack)+35));}
 }else if(stage==5){
  r.rect(177,207,210,68,24);r.border(177,207,210,68,200,2);text(r,"7  OUT",282,226,4,255,true);
  if(t>3.2f){float x=mix(385,625,attack);r.rect(x-27,330,54,50,190);text(r,"OUT",x,346,2,23,true);r.line(620,355,650,355,255,4);}
 }else if(stage==6){
  for(int i=0;i<5;i++){float x=470+i*76,y=mix(265,423,smooth((t-3.2f-i*.12f)/.5f));r.border(x-24,415,48,12,95,1);if(t>2.5f){r.rect(x-12,y-17,24,22,230);for(int f=0;f<5;f++)r.rect(x-14+f*6,y-35+(f==0||f==4?6:0),5,23,230);}}
 }else if(stage==7){
  if(t>2.5f){text(r,"KEEP YOUR DISTANCE!",620,239,2,255,true);r.line(bx+48,312,bx+106,300,190,6);r.line(bx+106,300,bx+120,309,190,5);}
 }else if(stage==8){
  for(int i=0;i<48;i++){float a=i*PI*2/48;if(a<.5f||a>PI*2-.5f)continue;float rad=80+attack*150;r.rect(280+cosine(a)*rad,410+sine(a)*rad*.28f,5,5,t>3.2f?230:100);}
  text(r,"SAFE GAP",605,268,2,210,true);r.line(440,300,550,340,180,2);if(t>4.5f)for(int i=-1;i<=1;i++){float p=clamp((t-4.5f)/1.2f,0,1);r.rect(mix(330,600,p),350+i*40*p,10,10,230);}
 }
 r.rect(60,479,840,145,18);text(r,bossWarnings[stage].what,480,501,2,255,true);text(r,bossWarnings[stage].effect,480,533,2,170,true);text(r,bossWarnings[stage].dodge,480,579,2,255,true);
 text(r,"WARNING FREEZES COMBAT / THEN DODGE THE MARKED ATTACK",480,646,1,170,true);
 button(7,640,675,290,32,"ENTER STAGE / ENTER",true,false,2);button(4,874,11,66,34,"SND");r.transform();
}
inline void Game::drawMorgan(){
 if(state==MORGAN_UNLOCK||state==PAUSED&&pausedState==MORGAN_UNLOCK){r.transform();r.clear(0);r.transform(uiLeft(),uiTop(),uiScale());
  text(r,"MORGAN INTERVIEW UNLOCKED",480,282,3,255,true);text(r,"B / SUPER MOVE READY",480,345,2,210,true);text(r,"BIG BOSS SEALED FOR 2 SECONDS",480,384,2,180,true);
  button(7,640,675,290,32,"CONTINUE / ENTER");button(4,874,11,66,34,"SND");r.transform();return;}
 State saved=state;float savedShake=shake;state=PLAYING;shake=0;render();state=saved;shake=savedShake;buttonCount=0;
 r.transform();r.fadeToBlack(.48f);float u=uiScale(),pop=smooth(cinema/.12f);
 r.transform(uiLeft()+480*u*(1-pop),uiTop()+360*u*(1-pop),u*pop);
 r.rect(100,168,760,384,15);r.border(100,168,760,384,255,3);for(int x=106;x<854;x+=28){r.rect(x,174,16,5,190);r.rect(x,541,16,5,190);}
 text(r,"SUPER MOVE",480,204,5,255,true);text(r,"MORGAN INTERVIEW",480,269,3,235,true);
 r.rect(142,326,676,42,42);text(r,"CLEAR ALL VISIBLE RIVALS + SHOTS",480,340,2,255,true);
 text(r,"FULL HEAL / REINFORCEMENTS RESET",480,399,2,210,true);text(r,"BIG BOSS SEALED FOR 2 SECONDS",480,438,2,255,true);
 text(r,"ONCE PER STAGE",480,479,1,150,true);if(cinema>=.25f)button(7,300,507,360,32,"ACTIVATE / ENTER",true,false,2);
 r.transform(uiLeft(),0,u);button(4,874,11,66,34,"SND");r.transform();
}
inline void coverWord(Raster& r,int visibleLetters=7){
 // Original condensed wordmark: solid white, no outline or drop shadow.
 // Hand-built contours, not a stretched 5x7 font. Tall heavy stems and narrow
 // counters follow the reference's letter proportions. Parallel top/bottom
 // edges and outward-leaning letters form a bottom-heavy trapezoid.
 static u8 mask[480*158]={};static bool ready=false;
 if(!ready){
  // Coordinates are local 0..100 contours. S has a diagonal waist; U has
  // deep narrow counters and clipped lower corners, like condensed club type.
  static const Point sShape[]={{22,0},{100,0},{100,12},{45,12},{39,15},{39,35},{45,40},{87,51},{100,60},{100,88},{91,96},{78,100},{0,100},{0,86},{55,86},{61,82},{61,66},{55,61},{13,50},{0,41},{0,12},{8,5}};
  static const Point uShape[]={{0,0},{37,0},{37,86},{43,89},{58,89},{64,86},{64,0},{100,0},{100,91},{88,98},{73,100},{24,100},{9,97},{0,91}};
  static const Point iShape[]={{0,0},{100,0},{100,100},{0,100}};
  const char* word="SIUUUUU";int pen=5;
  for(int i=0;word[i];i++){
   int width=word[i]=='S'?60:word[i]=='I'?22:50;
   const Point* shape=word[i]=='S'?sShape:word[i]=='I'?iShape:uShape;int count=word[i]=='S'?22:word[i]=='I'?4:14;
   for(int y=0;y<143;y++){
    float spread=.82f+.28f*y/142.f;
    for(int x=0;x<480;x++){
     // Inverse mapping keeps the sloping solid stems free of raster gaps.
     float local=(x+.5f-240)/spread+207-pen;
     if(local<0||local>=width)continue;
     float px=local*100/width,py=(y+.5f)*100/143;bool inside=false;
     for(int a=0,b=count-1;a<count;b=a++){const Point& p=shape[a];const Point& q=shape[b];if((p.y>py)!=(q.y>py)&&px<(q.x-p.x)*(py-p.y)/(q.y-p.y)+p.x)inside=!inside;}
     if(inside)mask[(5+y)*480+x]=u8(i+1);
    }
   }pen+=width+12;
  }ready=true;
 }
 for(int y=0;y<158;y++)for(int x=0;x<480;x++)if(mask[y*480+x]&&mask[y*480+x]<=visibleLetters)r.colorRect(240+x,272+y,1,1,0xffffffffu);
}
inline void Game::drawWarning(){
 State saved=state;float savedShake=shake;state=PLAYING;shake=0;render();state=saved;shake=savedShake;buttonCount=0;
 r.transform();r.fadeToBlack(.45f);
 float u=uiScale(),pop=smooth(warningTime/.16f)*(1-smooth((warningTime-WARNING_HOLD)/.3f));
 r.transform(uiLeft()+480*u*(1-pop),uiTop()+360*u*(1-pop),u*pop);
 r.rect(100,172,760,372,15);r.border(100,172,760,372,235,3);
 for(int x=106;x<854;x+=28){r.rect(x,178,16,5,160);r.rect(x,533,16,5,160);}
 text(r,"WARNING",480,205,6,255,true);text(r,stages[stage].name,480,266,2,180,true);
 text(r,bossSkills[stage],480,304,3,255,true);
 text(r,bossWarnings[stage].what,480,355,2,230,true);text(r,bossWarnings[stage].effect,480,381,2,180,true);
 r.rect(136,416,688,38,42);text(r,bossWarnings[stage].dodge,480,429,2,255,true);
 if(pop>.95f)button(14,300,477,360,32,warningExiting?"GET READY":warningTime<WARNING_MIN_READ?"COMBAT FROZEN":"CONTINUE / ENTER",state==WARNING&&warningTime>=WARNING_MIN_READ&&!warningExiting,false,2);
 r.transform(uiLeft(),0,u);button(4,874,11,66,34,muted?"OFF":"SND");r.transform();
}
inline void Game::drawFocus(){
 if(state==TITLE||state==PLAYING)return;
 // Modal menus must never expose hidden HUD actions to the mouse or keyboard.
 for(int i=buttonCount-1;i>=0;i--){int id=buttons[i].id;bool allowed=id==4;
  if(state==PAUSED)allowed|=id==8;
  else if(state==STAGE_SELECT)allowed|=(id>=100&&id<109)||id==12||id==13||id==2||id==11;
  else if(state==LOADOUT)allowed|=(id>=200&&id<204)||id==300||id==2;
  else if(state==RESULT)allowed|=id==9||id==1||id==10||id==11;
  else if(state==WARNING)allowed|=id==14;
  else allowed|=id==7;
  if(!allowed){for(int j=i;j<buttonCount-1;j++)buttons[j]=buttons[j+1];buttonCount--;}
 }
 int fallback=state==STAGE_SELECT?100+stagePage*3:state==LOADOUT?200+(pendingSkill<0?0:pendingSkill):state==PAUSED?8:state==RESULT?9:state==WARNING?14:7;
 bool found=false;for(int i=0;i<buttonCount;i++)if(buttons[i].id==focusedButton&&buttons[i].enabled)found=true;
 if(focusState!=state||!found){focusedButton=fallback;focusState=state;}
 if(!keyboardFocus)return;r.transform();for(int i=0;i<buttonCount;i++){auto& b=buttons[i];if(b.id==focusedButton&&b.enabled){r.border(b.x+4,b.y+4,b.w-8,b.h-8,255,maxi(1,roundi(2*uiScale())));}}
}
inline void Game::screenShake(){
 if(damageFlash>0&&(state==PLAYING||state==RESULT||state==PAUSED&&pausedState==PLAYING))r.damageTint(damageFlash/DAMAGE_FLASH_TIME);
 if(shake<=0||state==PAUSED||state==INTRO||state==TITLE)return;
 float phase=state==WARNING?warningTime:clock,amount=minf(14,shake)*maxf(.6f,uiScale());int dx=roundi(sine(phase*137+1)*amount),dy=roundi(cosine(phase*113)*amount*.65f);r.shift(dx,dy);
 for(int i=0;i<buttonCount;i++){buttons[i].x+=dx;buttons[i].y+=dy;}
}
inline void Game::render(){
 r.clear(state==TITLE?0:18);buttonCount=0;float u=uiScale(),ox=uiLeft(),oy=uiTop();State visible=state==PAUSED?pausedState:state;
 if(state==TITLE){
  if(curtain>=0){state=STAGE_SELECT;render();state=TITLE;buttonCount=0;}
  u=minf(float(WIDTH)/680,float(HEIGHT)/720);ox=(WIDTH-960*u)/2;oy=(HEIGHT-720*u)/2;
  float rise=curtain>=0?smooth(curtain/1.2f)*HEIGHT:0;
  float since=titleTime-.75f,beat=since-int(maxf(0,since)/.25f)*.25f;
  float jolt=since>=0&&since<1.75f?maxf(0,1-beat/.16f)*6*u:0;
  float sx=sine(titleTime*127)*jolt,sy=cosine(titleTime*97)*jolt*.7f;
  r.transform();r.rect(0,-rise,WIDTH,HEIGHT,0);r.transform(ox+sx,oy+sy-rise,u);
  text(r,"DESERT GOAT",480,218,4,255,true);coverWord(r,titleLetters);
  if(titleTime>=TITLE_READY&&curtain<0){text(r,"PRESS ANY KEY TO ENTER",480,501,2,215,true);Button& b=buttons[buttonCount++];b.id=1;b.x=roundi(ox+230*u);b.y=roundi(oy+474*u);b.w=roundi(500*u);b.h=roundi(70*u);b.enabled=1;copystr(b.label,"Press any key to enter",80);}
  r.transform();button(4,WIDTH-48,12,32,32,muted?"OFF":"SND");return;
 }
 if(visible==WARNING){drawWarning();if(state==PAUSED){r.transform(ox,oy,u);r.rect(310,287,340,126,12);r.border(310,287,340,126,255);text(r,"PAUSED",480,306,4,255,true);button(8,352,365,256,34,"RESUME / P");}r.transform();drawFocus();screenShake();return;}
 if(visible==OUTRO||visible==MORGAN||visible==MORGAN_UNLOCK||visible==BOSS_DEMO){if(visible==BOSS_DEMO)drawBossDemo();else if(visible==MORGAN||visible==MORGAN_UNLOCK)drawMorgan();else drawOutro();if(state==PAUSED){r.transform(ox,oy,u);r.rect(317,295,326,120,22);r.border(317,295,326,120,255);text(r,"PAUSED",480,318,4,255,true);button(8,352,368,256,34,"RESUME / P");}r.transform();drawFocus();screenShake();return;}
 if(visible==INTRO){r.transform(ox,oy,u);drawIntro();r.transform();}
 else {drawWorld();
  if(infinite&&(state==PLAYING||state==PAUSED)){Point from=project(player.x+1,.1f,player.z+1),to=project(goalX+1,.1f,goalZ+1);r.colorLine(from.x,from.y,to.x,to.y,0xff2424ffu);}
 }
 r.transform();r.clip(0,0,WIDTH,HEIGHT);r.rect(0,0,WIDTH,56*u,22);
 if(superFlash>0&&state==PLAYING){r.rect(0,HEIGHT*.42f,WIDTH,75*u,superFlash>.85f?235:12);text(r,"SUPER CRITICAL!",WIDTH*.5f,HEIGHT*.42f+15*u,maxi(1,roundi(5*u)),superFlash>.85f?23:255,true);}
 r.transform(ox,0,u);text(r,"DESERT GOAT",24,20,2);bool cinematic=visible==INTRO;
 if(state==PLAYING)button(1,724,11,140,34,"STAGES");button(4,874,11,66,34,muted?"OFF":"SND");
 {
  r.transform();r.rect(0,HEIGHT-80*u,WIDTH,80*u,22);r.transform(ox,HEIGHT-720*u,u);
  text(r,"STAMINA",24,656,1,170);for(int i=0;i<lives;i++)r.rect(26+i*25,679,12,12,255);text(r,"HOLD C",150,656,1,170);text(r,"SHIELD",150,677,2);
  button(5,260,650,390,58,"",state==PLAYING&&charge>=100&&!dashLeft&&!shielding()&&player.stun<=0,charge>=100&&state==PLAYING);int c=charge>=100&&state==PLAYING?23:255;text(r,"V",275,670,3,c);text(r,skills[equipped].name,310,659,2,c);r.rect(310,681,320,4,90);r.rect(310,681,320*charge/100,4,c);text(r,state==PAUSED?"PAUSED / PRESS P":shielding()?"RELEASE C TO ATTACK":charge>=100?"READY / PRESS V":"CHARGING",310,692,1,c);for(int i=0;i<buttonCount;i++)if(buttons[i].id==5)copystr(buttons[i].label,"Use special attack (V)",80);
  r.rect(666,650,270,58,30);r.border(666,650,270,58,100,2);text(r,"REINFORCEMENTS",678,659,2,235);
  float spawnProgress=infinite?waveClock/waveInterval():1-spawnTimer/RIVAL_SPAWN_INTERVAL;r.rect(678,681,246,4,70);r.rect(678,681,246*clamp(spawnProgress,0,1),4,230);
  char wave[48];number(wave,infinite?waveSize():1);append(wave," RIVALS / ",48);char sec[12];number(sec,maxi(0,int((infinite?waveInterval()-waveClock:spawnTimer)+.99f)));append(wave,sec,48);append(wave,"S",48);text(r,infinite&&waveClock>=waveInterval()?"WAITING FOR SAFE SPACE":wave,678,692,1,190);
 }
 if(state==PLAYING||state==PAUSED){
  r.transform(ox,0,u);r.rect(240,66,480,43,12);char progressLabel[32]="TROPHY ",percentage[12];number(percentage,roundi(trophyProgress()*100));append(progressLabel,percentage,32);append(progressLabel,"%",32);text(r,progressLabel,480,73,2,245,true);r.rect(254,96,452,5,65);r.rect(254,96,452*trophyProgress(),5,235);
  if(bossEncounterActive()){const Unit* boss=finalBoss();r.rect(210,114,540,65,16);char label[80];copystr(label,stages[stage].name,80);append(label,bossRage?" / RAGE":" / BOSS",80);text(r,label,480,120,2,255,true);r.rect(224,141,512,5,55);r.rect(224,141,512*(boss?float(boss->hp)/bossMaxHp():1),5,bossRage?255:185);text(r,boss?"DEFEAT BOSS / TROPHY LOCKED":"BOSS INCOMING / TROPHY LOCKED",480,155,1,230,true);if(bossWindup>0)r.rect(224,171,512*(1-bossWindup/bossWindupTotal),3,255);}
  r.transform(ox,HEIGHT-720*u,u);char info[100]="X ",n[16];number(n,player.x);append(info,n,100);append(info," Z ",100);number(n,player.z);append(info,n,100);
  append(info,"   TROPHY ",100);number(n,roundi(length(float(player.x-goalX),float(player.z-goalZ))));append(info,n,100);append(info," TILES",100);r.rect(16,610,440,23,10);text(r,info,24,618,1,235);
  button(6,500,608,436,32,superUnlocked()?(superUsed?"B  MORGAN INTERVIEW / USED":"B  MORGAN INTERVIEW / READY"):"B  MORGAN / CLEAR STAGES 1-3",state==PLAYING&&superUnlocked()&&!superUsed,false,1);
  text(r,shielding()?"C  SHIELD UP / TURN WITH WASD":"HOLD C: SHIELD / FACING ATTACKS ONLY",24,637,1,shielding()?255:180);
  if(notice>0){r.transform(ox,0,u);int y=bossEncounterActive()?190:119;r.rect(290,y,380,40,17);text(r,skills[equipped].name,480,y+13,2,255,true);}
 }
 if(state==PLAYING&&rageFlash>0){float elapsed=RAGE_FLASH_TIME-rageFlash;int flash=int(elapsed/.2f);if(flash<6&&flash%2==0){r.transform();r.fadeToBlack(.18f);r.transform(ox,oy,u);r.colorRect(150,245,660,190,0xff1818a0u);r.colorRect(150,245,660,7,0xff3030ffu);r.colorRect(150,428,660,7,0xff3030ffu);r.colorRect(150,245,7,190,0xff3030ffu);r.colorRect(803,245,7,190,0xff3030ffu);for(int x=170;x<790;x+=38)r.colorRect(x,263,22,5,0xff2828d8u);text(r,"RAGE MODE",480,292,6,255,true);text(r,stages[stage].name,480,356,3,255,true);text(r,"HALF HP / SPEED UP",480,397,2,255,true);}}
 r.transform(ox,oy,u);
 
 if(state==PAUSED){r.rect(317,295,326,120,22);r.border(317,295,326,120,255);text(r,"PAUSED",480,318,4,255,true);button(8,352,368,256,34,"RESUME / P");}
 if(state==LOADOUT){r.rect(36,91,888,521,25);r.border(36,91,888,521,85);text(r,stages[pendingStage].name,480,115,2,180,true);text(r,"CHOOSE ONE SPECIAL",480,152,3,255,true);text(r,"ALL FOUR AVAILABLE / PRESS V IN GAME",480,188,1,180,true);
  for(int i=0;i<SKILL_COUNT;i++){int x=62+i*214;bool selected=pendingSkill==i;button(200+i,x,222,194,260,"",true,selected);int c=selected?23:255;Unit hero;hero.dir=i==3?1:2;int pose=i==0?5:i==1?10:i==2?7:6;actor(hero,x+97,350,1.4f,-1,pose,.4f);if(i==0){r.rect(x+125,251,26,19,c);text(r,"C",x+133,254,1,selected?235:23);}if(i==2){r.rect(x+133,253,14,22,c);r.rect(x+136,256,8,13,selected?235:23);}text(r,skills[i].name,x+97,370,2,c,true);text(r,skills[i].hint,x+97,400,1,c,true);char cd[32];number(cd,int(skills[i].recharge));append(cd,"S RECHARGE",32);text(r,cd,x+97,426,1,c,true);text(r,selected?"SELECTED":"SELECT",x+97,459,1,c,true);copystr(buttons[buttonCount-1].label,skills[i].name,80);}
  text(r,superUnlocked()?"BONUS: MORGAN INTERVIEW / B / ONCE PER STAGE":"MORGAN INTERVIEW UNLOCKS AFTER STAGES 1-3",480,505,1,190,true);button(2,80,550,260,40,"BACK TO STAGES");button(300,510,550,370,40,pendingSkill>=0?"ENTER STAGE":"CHOOSE A SPECIAL",pendingSkill>=0,pendingSkill>=0);
 }
 if(state==STAGE_SELECT){r.rect(36,91,888,521,30);r.border(36,91,888,521,85);text(r,"INFINITE MAZE",480,116,2,187,true);text(r,"SELECT STAGE",480,156,3,255,true);
  int page=stagePage;
  for(int slot=0;slot<3;slot++){int i=page*3+slot;if(i>=STAGE_COUNT)break;int x=60+slot*286;bool enabled=unlocked(i),selected=false;int id=100+i;
   button(id,x,215,266,264,"",enabled,selected);int c=selected?23:enabled?255:110;
   if(state==STAGE_SELECT){Unit u;u.dir=2;actor(u,x+133,352,1.7f,i);text(r,stages[i].name,x+133,366,3,c,true);text(r,stages[i].bio1,x+133,400,2,c,true);text(r,stages[i].bio2,x+133,423,2,c,true);}
   
   char lock[48]="CLEAR STAGE ",num[8];number(num,i,2);append(lock,num,48);append(lock," FIRST",48);
   const char* status=!enabled?lock:selected?"SELECTED":state==STAGE_SELECT?(cleared&(1<<i)?"CLEARED / REPLAY":"SELECT STAGE"):"SELECT";text(r,status,x+133,457,1,c,true);if(state==STAGE_SELECT){char distance[32]="TROPHY: ",n[8];InfiniteMaze preview;preview.seed=stages[i].seed;Room goal=preview.trophyLeaf(trophyRooms[i]);number(n,roundi(length(goal.x*4,goal.z*4)));append(distance,n,32);append(distance," TILES",32);text(r,distance,x+133,443,1,c,true);}
   copystr(buttons[buttonCount-1].label,stages[i].name,80);
   if(state==STAGE_SELECT){append(buttons[buttonCount-1].label,". ",80);append(buttons[buttonCount-1].label,stages[i].bio1,80);append(buttons[buttonCount-1].label,". ",80);append(buttons[buttonCount-1].label,stages[i].bio2,80);}
  }
  button(12,80,490,180,40,"< LEFT",page>0,false,2);button(13,700,490,180,40,"RIGHT >",page<(STAGE_COUNT-1)/3,false,2);char pageText[40]="PAGE ",pn[8];number(pn,page+1);append(pageText,pn,40);append(pageText," / 3",40);text(r,pageText,480,500,2,170,true);
  if(state==LOADOUT){button(2,80,550,260,40,"BACK TO STAGES");button(300,510,550,370,40,pendingSkill>=0?"ENTER STAGE":"CHOOSE A SPECIAL",pendingSkill>=0,pendingSkill>=0);}
  else{button(2,80,550,180,40,returnState==PLAYING||returnState==PAUSED?"RESUME":"BACK");button(11,445,550,435,40,"PREVIEW SIUUUUU");if(!storageOK)text(r,"STORAGE UNAVAILABLE - SESSION ONLY",480,509,1,170,true);}
 }
 if(state==RESULT){r.rect(164,156,632,414,30);r.border(164,156,632,414,85);text(r,won?"ESCAPE COMPLETE":"BACK TO THE BENCH",480,200,4,255,true);text(r,stages[stage].name,480,274,3,187,true);if(won)text(r,campaign&&cleared==PROGRESS_MASK?"ALL 9 TROPHIES COLLECTED":stage==2&&superUnlocked()?"MORGAN INTERVIEW UNLOCKED":"STAGE CLEARED / ALL RIVALS OPEN",480,324,2,255,true);button(9,194,391,270,48,"TRY AGAIN");button(1,494,391,270,48,"STAGES");if(won&&stage<STAGE_COUNT-1&&campaign)button(10,194,457,270,48,"NEXT STAGE");if(won)button(11,494,457,270,48,"REPLAY SIUUUUU");if(!storageOK)text(r,"PROGRESS SAVED FOR THIS SESSION ONLY",480,535,1,170,true);}

 r.transform();drawFocus();screenShake();
}
