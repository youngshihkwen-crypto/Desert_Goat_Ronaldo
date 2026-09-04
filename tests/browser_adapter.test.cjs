// Test the real Wasm platform adapter against a minimal DOM, without a browser driver.
const {test}=require('node:test'),assert=require('node:assert/strict'),fs=require('node:fs'),vm=require('node:vm'),path=require('node:path');
test('mouse targets survive shake frames and keyboard remains in sync with DOM focus',async()=>{
 const events={},frameQueue=[];let document;
 class Element{
  constructor(){this.children=[];this.style={};this.dataset={};this.textContent='';this.disabled=false;}
  setAttribute(){} addEventListener(){} getContext(){return {putImageData(){}};}
  getBoundingClientRect(){return {width:960,height:720};}
  focus(){document.activeElement=this;this.onfocus?.();}
  remove(){if(this.parent){const at=this.parent.children.indexOf(this);this.parent.children.splice(at,1);this.parent=null;}}
  insertBefore(el,before){el.remove();const at=before?this.children.indexOf(before):this.children.length;this.children.splice(at,0,el);el.parent=this;}
 }
 const ids=Object.fromEntries(['game','controls','status','loading'].map(id=>[id,new Element()]));
 document={getElementById:id=>ids[id],createElement:()=>new Element(),addEventListener(){},activeElement:null};
 const source=fs.readFileSync(path.join(__dirname,'../src/browser.js'),'utf8');
 const context={document,window:{addEventListener:(key,fn)=>events[key]=fn},console,WebAssembly,TextDecoder,TextEncoder,Uint8Array,Uint8ClampedArray,Int32Array,Map,Set,ImageData:class{},localStorage:{getItem:()=>null,setItem(){}},atob:s=>Buffer.from(s,'base64').toString('binary'),WASM_BASE64:fs.readFileSync(path.join(__dirname,'../build/desert-goat.wasm')).toString('base64'),requestAnimationFrame:fn=>frameQueue.push(fn)};
 vm.runInNewContext(source,context);for(let i=0;i<100&&!frameQueue.length;i++)await new Promise(resolve=>setTimeout(resolve,5));assert.ok(frameQueue.length);
 let now=0;const tick=(count=1)=>{for(let i=0;i<count;i++){now+=50;frameQueue.shift()(now);}};
 const key=name=>{const event={key:name,preventDefault(){}};events.keydown(event);events.keyup(event);tick();};
 const button=id=>ids.controls.children.find(el=>Number(el.dataset.action)===id);
 const click=id=>{const el=button(id);assert.ok(el&&!el.disabled);el.focus();el.onclick();};
 tick(66);key('Enter');tick(26);assert.equal(ids.status.textContent,'SELECT STAGE');
 click(13);assert.match(button(103).textContent,/CASSANO/);key('ArrowRight');key('Enter');assert.equal(ids.status.textContent,'CHOOSE SPECIAL');
 key('ArrowRight');key('Enter');key('ArrowDown');key('Enter');assert.equal(ids.status.textContent,'OVERTHINKING');
 click(7);assert.match(ids.status.textContent,/PLAYING STAGE 5/);click(5);const stages=button(1);tick(4);assert.equal(button(1),stages,'shake frames preserve the same click target');assert.equal(stages.parent,ids.controls);
 click(1);assert.equal(ids.status.textContent,'SELECT STAGE');
 const cards=ids.controls.children.map(el=>el.dataset.action);assert.equal(new Set(cards).size,cards.length,'no duplicated DOM actions');
 key('ArrowLeft');key('Enter');assert.equal(ids.status.textContent,'CHOOSE SPECIAL');
});
