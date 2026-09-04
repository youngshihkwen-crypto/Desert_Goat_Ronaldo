// Lossless QA captures of frames drawn by the C++ renderer.
const fs=require('node:fs'),zlib=require('node:zlib'),path=require('node:path');
function crc(data){let c=0xffffffff;for(const byte of data){c^=byte;for(let i=0;i<8;i++)c=(c>>>1)^((c&1)?0xedb88320:0);}return (c^0xffffffff)>>>0;}
function chunk(type,data){const t=Buffer.from(type),h=Buffer.alloc(4),c=Buffer.alloc(4);h.writeUInt32BE(data.length);c.writeUInt32BE(crc(Buffer.concat([t,data])));return Buffer.concat([h,t,data,c]);}
for(const name of ['warning-dark','warning-wide','morgan-sealed','morgan-unlocked','boss-demo-4','boss-demo-5','boss-demo-6','boss-demo-7','boss-demo-8','boss-demo-9','title-start','title-letter','title','curtain','stages','stages-unlocked','stages-middle','stages-final','loadout','intro','world','wave-warning','maguire-card','maguire-boss','horde-wave','four-skills','special-armband','special-ronado','special-phone','special-elbow','shield','damage-flash','morgan-impact','morgan-wide','boss-skill-4','boss-skill-5','boss-skill-6','boss-skill-7','boss-skill-8','boss-skill-9','exit','fade','trophy','place','run','jump','land','wide']){
 const raw=fs.readFileSync(path.join(__dirname,'../build',name+'.rgba'));
 const wide=name==='wide'||name==='morgan-wide'||name==='warning-wide';const width=wide?1600:960,height=wide?900:720;
 const rows=Buffer.alloc(height*(width*4+1));for(let y=0;y<height;y++)raw.copy(rows,y*(width*4+1)+1,y*width*4,(y+1)*width*4);
 const h=Buffer.alloc(13);h.writeUInt32BE(width);h.writeUInt32BE(height,4);h[8]=8;h[9]=6;
 const png=Buffer.concat([Buffer.from([137,80,78,71,13,10,26,10]),chunk('IHDR',h),chunk('IDAT',zlib.deflateSync(rows)),chunk('IEND',Buffer.alloc(0))]);
 fs.writeFileSync(path.join(__dirname,'../build',name+'.png'),png);
}
