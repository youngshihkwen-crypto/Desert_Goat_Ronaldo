const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { test } = require('node:test');
test('C++ WebAssembly loads, renders, selects a skill and handles pause', async () => {
  const bytes = fs.readFileSync(path.join(__dirname, '../build/desert-goat.wasm'));
  const { instance } = await WebAssembly.instantiate(bytes, { env: {
    host_progress:()=>0, host_save_progress:()=>1,
    host_load_map:()=>0, host_save_map:()=>1, host_voice:()=>{}, host_tone:()=>{}
  }});
  const api=instance.exports;
  api.__wasm_call_ctors?.(); api.game_init();
  assert.equal(api.game_state(),0);
  assert.equal(api.game_width(),960);assert.equal(api.game_height(),720);
  const pixels = new Uint32Array(api.memory.buffer,api.game_pixels(),960*720);
  assert.ok(new Set(pixels).size>=2,'software rasterizer produces a visible frame');
  assert.equal(pixels[0],0xff000000,'cover background is pure black');
  assert.equal(api.game_button_count(),1,'initial cover only exposes sound');
  for(let i=0;i<65;i++)api.game_tick(.05);
  assert.equal(api.game_button_count(),2,'entry hint appears after lettering');
  api.game_key(13,1);api.game_key(13,0);
  assert.equal(api.game_state(),0,'curtain is animated');
  for(let i=0;i<25;i++)api.game_tick(.05);
  assert.equal(api.game_state(),1);
  api.game_key(39,1);api.game_key(39,0);api.game_tick(0);
  api.game_key(37,1);api.game_key(37,0);api.game_tick(0);
  api.game_action(100);assert.equal(api.game_state(),2);
  api.game_action(300);assert.equal(api.game_state(),2);
  for(let skill=0;skill<4;skill++){
    const cards=Array.from({length:api.game_button_count()},(_,i)=>new Int32Array(api.memory.buffer,api.game_button(i),6));
    assert.equal(cards.find(b=>b[0]===200+skill)?.[5],1,'four skills available without clears');
  }
  api.game_action(200);api.game_action(300);assert.equal(api.game_state(),3);
  api.game_action(7);assert.equal(api.game_state(),4);
  api.game_key(86,1);api.game_key(86,0);api.game_tick(.016);
  api.game_blur();assert.equal(api.game_state(),5);
  api.game_action(8);assert.equal(api.game_state(),4);
  for(let i=0;i<api.game_button_count();i++){
    const button=new Int32Array(api.memory.buffer,api.game_button(i),6);
    assert.ok(button[1]>=0&&button[2]>=0&&button[1]+button[3]<=960&&button[2]+button[4]<=720);
  }
  api.game_resize(1600,900);assert.equal(api.game_width(),1600);assert.equal(api.game_height(),900);api.game_tick(.016);
  assert.equal(new Uint32Array(api.memory.buffer,api.game_pixels(),1600*900).length,1440000);
});
test('old three-clear save unlocks short Morgan super popup in Wasm', async()=>{
  const {instance}=await WebAssembly.instantiate(fs.readFileSync(path.join(__dirname,'../build/desert-goat.wasm')),{env:{host_progress:()=>7,host_save_progress:()=>1,host_load_map:()=>0,host_save_map:()=>1,host_voice:()=>{},host_tone:()=>{}}});
  const api=instance.exports;api.__wasm_call_ctors?.();api.game_init();
  for(let i=0;i<65;i++)api.game_tick(.05);api.game_action(1);for(let i=0;i<25;i++)api.game_tick(.05);
  api.game_action(103);api.game_action(203);api.game_action(300);api.game_action(7);
  assert.equal(api.game_state(),4);api.game_key(66,1);api.game_key(66,0);assert.equal(api.game_state(),9);
  api.game_tick(.05);api.game_blur();assert.equal(api.game_state(),5);api.game_action(8);assert.equal(api.game_state(),9);
  api.game_resize(1600,900);api.game_action(7);assert.equal(api.game_state(),4);api.game_action(6);assert.equal(api.game_state(),4,'cannot use Morgan twice');
});
