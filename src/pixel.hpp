#pragma once
// The complete rasterizer and font run in C++, including in the browser build.
using u8=unsigned char; using u32=unsigned int;
constexpr int MAX_WIDTH=2560,MAX_HEIGHT=1440;
inline int WIDTH=960,HEIGHT=720;
constexpr float PI=3.14159265359f;
inline float absf(float a){return a<0?-a:a;}
inline float minf(float a,float b){return a<b?a:b;}
inline float maxf(float a,float b){return a>b?a:b;}
inline float clamp(float v,float a,float b){return minf(b,maxf(a,v));}
inline int mini(int a,int b){return a<b?a:b;}
inline int maxi(int a,int b){return a>b?a:b;}
inline float mix(float a,float b,float t){return a+(b-a)*t;}
inline float smooth(float t){t=clamp(t,0,1);return t*t*(3-2*t);}
inline float sine(float x){x-=int(x/(2*PI))*2*PI;if(x>PI)x-=2*PI;if(x< -PI)x+=2*PI;float x2=x*x;return x*(1-x2/6+x2*x2/120-x2*x2*x2/5040+x2*x2*x2*x2/362880);}
inline float cosine(float x){return sine(x+PI/2);}
inline float length(float x,float y){return __builtin_sqrtf(x*x+y*y);}
inline int roundi(float x){return int(x+(x>=0?.5f:-.5f));}
inline int strlength(const char* s){int n=0;while(s[n])n++;return n;}
inline void copystr(char* dest,const char* src,int cap){int i=0;for(;i<cap-1&&src[i];i++)dest[i]=src[i];dest[i]=0;}
inline void number(char* out,int n,int digits=0){char tmp[16];int i=0;bool neg=n<0;u32 v=neg?0u-u32(n):u32(n);do{tmp[i++]=char('0'+v%10);v/=10;}while(v&&i<15);while(i<digits&&i<15)tmp[i++]='0';int j=0;if(neg)out[j++]='-';while(i)out[j++]=tmp[--i];out[j]=0;}
inline void append(char* dst,const char* src,int cap=128){int len=strlength(dst);copystr(dst+len,src,cap-len);}
inline void copybytes(void* dst,const void* src,int n){auto d=(u8*)dst;auto s=(const u8*)src;for(int i=0;i<n;i++)d[i]=s[i];}
struct Point{float x,y;};
struct Raster{
 u32 pixels[MAX_WIDTH*MAX_HEIGHT];
 int clipL=0,clipT=0,clipR=WIDTH,clipB=HEIGHT;
 float tx=0,ty=0,scale=1;
 void clip(int x,int y,int w,int h){clipL=maxi(0,x);clipT=maxi(0,y);clipR=mini(WIDTH,x+w);clipB=mini(HEIGHT,y+h);}
 void transform(float x=0,float y=0,float s=1){tx=x;ty=y;scale=s;}
 void rect(float x,float y,float w,float h,int gray){int l=roundi(tx+x*scale),t=roundi(ty+y*scale),r=roundi(tx+(x+w)*scale),b=roundi(ty+(y+h)*scale);l=maxi(l,clipL);t=maxi(t,clipT);r=mini(r,clipR);b=mini(b,clipB);u32 c=0xff000000u|u32(gray)*0x010101u;for(int yy=t;yy<b;yy++)for(int xx=l;xx<r;xx++)pixels[yy*WIDTH+xx]=c;}
 void border(float x,float y,float w,float h,int c,int size=2){rect(x,y,w,size,c);rect(x,y+h-size,w,size,c);rect(x,y,size,h,c);rect(x+w-size,y,size,h,c);}
 void colorPixel(int x,int y,u32 rgba){if(x>=clipL&&x<clipR&&y>=clipT&&y<clipB)pixels[y*WIDTH+x]=rgba;}
 void colorRect(float x,float y,float w,float h,u32 rgba){int l=maxi(clipL,roundi(tx+x*scale)),t=maxi(clipT,roundi(ty+y*scale)),rr=mini(clipR,roundi(tx+(x+w)*scale)),bb=mini(clipB,roundi(ty+(y+h)*scale));for(int yy=t;yy<bb;yy++)for(int xx=l;xx<rr;xx++)pixels[yy*WIDTH+xx]=rgba;}
 void colorLine(float x1,float y1,float x2,float y2,u32 rgba){
  float dx=x2-x1,dy=y2-y1,t0=0,t1=1;
  float p[4]={-dx,dx,-dy,dy},q[4]={x1-clipL,clipR-1-x1,y1-clipT,clipB-1-y1};
  for(int i=0;i<4;i++){if(absf(p[i])<.0001f){if(q[i]<0)return;}else{float t=q[i]/p[i];if(p[i]<0)t0=maxf(t0,t);else t1=minf(t1,t);}}if(t0>t1)return;
  float ax=x1+dx*t0,ay=y1+dy*t0,bx=x1+dx*t1,by=y1+dy*t1;int n=maxi(1,roundi(maxf(absf(bx-ax),absf(by-ay))));for(int i=0;i<=n;i++)colorPixel(roundi(mix(ax,bx,float(i)/n)),roundi(mix(ay,by,float(i)/n)),rgba);
 }
 void clear(int c){transform();clip(0,0,WIDTH,HEIGHT);rect(0,0,WIDTH,HEIGHT,c);}
 void shift(int dx,int dy){if(!dx&&!dy)return;int y=dy>0?HEIGHT-1:0,end=dy>0?-1:HEIGHT,step=dy>0?-1:1;for(;y!=end;y+=step){int sy=y-dy;for(int j=0;j<WIDTH;j++){int x=dx>0?WIDTH-1-j:j,sx=x-dx;pixels[y*WIDTH+x]=sx>=0&&sx<WIDTH&&sy>=0&&sy<HEIGHT?pixels[sy*WIDTH+sx]:0xff000000u;}}}
 void fadeToBlack(float amount){int keep=roundi((1-clamp(amount,0,1))*255);for(int i=0;i<WIDTH*HEIGHT;i++){u32 c=pixels[i];pixels[i]=0xff000000u|(((c&255)*keep/255))|((((c>>8)&255)*keep/255)<<8)|((((c>>16)&255)*keep/255)<<16);}}
 // Brief damage wash: desaturate and dim existing pixels, never hide the map.
 void damageTint(float amount){int a=roundi(clamp(amount,0,1)*210),keep=255-roundi(clamp(amount,0,1)*90);for(int i=0;i<WIDTH*HEIGHT;i++){u32 c=pixels[i];int red=c&255,green=(c>>8)&255,blue=(c>>16)&255,gray=(red*77+green*150+blue*29)/256;red=((red*(255-a)+gray*a)/255)*keep/255;green=((green*(255-a)+gray*a)/255)*keep/255;blue=((blue*(255-a)+gray*a)/255)*keep/255;pixels[i]=0xff000000u|u32(red)|u32(green)<<8|u32(blue)<<16;}}
 void line(float x1,float y1,float x2,float y2,int c,int thickness=2){int n=maxi(1,roundi(maxf(absf(x2-x1),absf(y2-y1))));for(int i=0;i<=n;i++){float t=float(i)/n;rect(mix(x1,x2,t),mix(y1,y2,t),thickness,thickness,c);}}
 void polygon(const Point* p,int n,int c){
  if(n<3||n>32)return;Point q[32];float lo=10000,hi=-10000;for(int i=0;i<n;i++){q[i]={tx+p[i].x*scale,ty+p[i].y*scale};lo=minf(lo,q[i].y);hi=maxf(hi,q[i].y);}
  u32 color=0xff000000u|u32(c)*0x010101u;
  for(int y=maxi(clipT,int(lo));y<mini(clipB,int(hi)+1);y++){float xs[32];int count=0;for(int i=0;i<n;i++){Point a=q[i],b=q[(i+1)%n];if((a.y<=y&&b.y>y)||(b.y<=y&&a.y>y))xs[count++]=a.x+(y-a.y)*(b.x-a.x)/(b.y-a.y);}
   for(int i=0;i<count;i++)for(int j=i+1;j<count;j++)if(xs[j]<xs[i]){float tmp=xs[i];xs[i]=xs[j];xs[j]=tmp;}
   for(int i=0;i+1<count;i+=2)for(int x=maxi(clipL,int(xs[i]));x<mini(clipR,int(xs[i+1])+1);x++)pixels[y*WIDTH+x]=color;
  }
 }
};
// Original 5x7 bitmap alphabet. No system fonts, font downloads or antialiasing.
static constexpr u8 alphabet[][7]={
 {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},{30,17,17,17,17,17,30},
 {31,16,16,30,16,16,31},{31,16,16,30,16,16,16},{14,17,16,23,17,17,15},{17,17,17,31,17,17,17},
 {31,4,4,4,4,4,31},{7,2,2,2,18,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
 {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},{30,17,17,30,16,16,16},
 {14,17,17,17,21,18,13},{30,17,17,30,20,18,17},{15,16,16,14,1,1,30},{31,4,4,4,4,4,4},
 {17,17,17,17,17,17,14},{17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
 {17,17,10,4,4,4,4},{31,1,2,4,8,16,31},
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{30,1,1,14,1,1,30},
 {2,6,10,18,31,2,2},{31,16,16,30,1,1,30},{14,16,16,30,17,17,14},{31,1,2,4,8,8,8},
 {14,17,17,14,17,17,14},{14,17,17,15,1,1,14}
};
inline const u8* glyph(char c){if(c>='a'&&c<='z')c-=32;if(c>='A'&&c<='Z')return alphabet[c-'A'];if(c>='0'&&c<='9')return alphabet[26+c-'0'];
 static const u8 empty[7]={0},exclaim[7]={4,4,4,4,4,0,4},question[7]={14,17,1,2,4,0,4},colon[7]={0,4,4,0,4,4,0},dash[7]={0,0,0,31,0,0,0},dot[7]={0,0,0,0,0,4,4},slash[7]={1,2,2,4,8,8,16},plus[7]={0,4,4,31,4,4,0},left[7]={14,8,8,8,8,8,14},right[7]={14,2,2,2,2,2,14},quote[7]={10,10,0,0,0,0,0},star[7]={0,21,14,31,14,21,0},gt[7]={16,8,4,2,4,8,16},lt[7]={1,2,4,8,4,2,1},pct[7]={17,2,4,8,16,17,0};
 switch(c){case '!':return exclaim;case '?':return question;case ':':return colon;case '-':return dash;case '.':case ',':return dot;case '/':return slash;case '+':return plus;case '[':case '(':return left;case ']':case ')':return right;case '"':case '\'':return quote;case '*':return star;case '>':return gt;case '<':return lt;case '%':return pct;default:return empty;}}
// GOAT Heavy: original 7x9 condensed bitmap family. Two-pixel stems, narrow
// counters and clipped corners echo the solid title lettering at UI sizes.
// Keep the existing 6*size advance and 7*size height so every menu still fits.
static constexpr u8 heavyAlphabet[][9]={
 {28,62,99,99,99,127,127,99,99}, // A
 {126,127,99,99,126,99,99,127,126}, // B
 {62,127,99,96,96,96,99,127,62}, // C
 {124,126,103,99,99,99,103,126,124}, // D
 {127,127,96,96,124,124,96,127,127}, // E
 {127,127,96,96,124,124,96,96,96}, // F
 {62,127,99,96,111,111,99,127,62}, // G
 {99,99,99,99,127,127,99,99,99}, // H
 {62,62,28,28,28,28,28,62,62}, // I
 {31,31,6,6,6,6,102,126,60}, // J
 {99,103,110,124,120,124,110,103,99}, // K
 {96,96,96,96,96,96,96,127,127}, // L
 {99,119,127,127,107,99,99,99,99}, // M
 {99,99,115,123,127,111,103,99,99}, // N
 {62,127,99,99,99,99,99,127,62}, // O
 {126,127,99,99,127,126,96,96,96}, // P
 {62,127,99,99,99,107,111,126,63}, // Q
 {126,127,99,99,126,124,110,103,99}, // R
 {62,127,99,112,62,7,99,127,62}, // S
 {127,127,28,28,28,28,28,28,28}, // T
 {99,99,99,99,99,99,99,127,62}, // U
 {99,99,99,99,99,99,54,62,28}, // V
 {99,99,99,99,107,127,127,119,99}, // W
 {99,99,54,62,28,62,54,99,99}, // X
 {99,99,99,54,62,28,28,28,28}, // Y
 {127,127,7,14,28,56,112,127,127}, // Z
 {62,127,99,103,107,115,99,127,62}, // 0
 {12,28,60,28,28,28,28,62,62}, // 1
 {62,127,99,7,14,28,56,127,127}, // 2
 {126,127,3,3,62,3,3,127,126}, // 3
 {14,30,54,102,102,127,127,6,6}, // 4
 {127,127,96,96,126,127,3,127,126}, // 5
 {62,127,96,96,126,127,99,127,62}, // 6
 {127,127,3,6,12,24,24,24,24}, // 7
 {62,127,99,99,62,99,99,127,62}, // 8
 {62,127,99,127,63,3,3,127,62} // 9
};
inline void text(Raster& r,const char* s,float x,float y,int size=2,int c=255,bool center=false){
 if(center)x-=(strlength(s)*6-1)*size/2.f;
 for(int i=0;s[i];i++){
  char ch=s[i];if(ch>='a'&&ch<='z')ch-=32;
  int id=ch>='A'&&ch<='Z'?ch-'A':ch>='0'&&ch<='9'?26+ch-'0':-1;
  if(id>=0){const u8* g=heavyAlphabet[id];float w=5.f*size/7,h=7.f*size/9;
   for(int row=0;row<9;row++)for(int col=0;col<7;col++)if(g[row]&(1<<(6-col)))r.rect(x+i*6*size+col*w,y+row*h,w,h,c);
  }else{const u8* g=glyph(ch);for(int row=0;row<7;row++)for(int col=0;col<5;col++)if(g[row]&(1<<(4-col)))r.rect(x+i*6*size+col*size,y+row*size,minf(size*1.35f,float((5-col)*size)),size,c);}
 }
}
