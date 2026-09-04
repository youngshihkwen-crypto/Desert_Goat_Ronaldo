// Platform adapter only. Rules, maps, menus, input interpretation, animation,
// drawing, typography and hit testing are compiled C++ in the embedded Wasm.
(() => {
  'use strict';
  const canvas=document.getElementById('game'), ctx=canvas.getContext('2d',{alpha:false});
  const controls=document.getElementById('controls'), status=document.getElementById('status');
  let api, audio, voice, image, buttonsSignature='', last=0;
  const utf8=new TextDecoder(), controlNodes=new Map();
  function readString(ptr,max=256){const bytes=new Uint8Array(api.memory.buffer,ptr,max);let end=bytes.indexOf(0);return utf8.decode(end<0?bytes:bytes.subarray(0,end));}
  function stopVoice(){voice=null;window.speechSynthesis?.cancel();}
  function speak(id){stopVoice();if(!id)return;if(!window.speechSynthesis||!window.SpeechSynthesisUtterance)return;const lines=['','Ronaldo is not in my plans.','Siuuuuuu!','So... who is the greatest?','I am number one, two and three in history.','The World Cup is not my dream.'];if(!lines[id])return;const line=new SpeechSynthesisUtterance(lines[id]);line.lang='en-US';line.rate=id===2?.65:1;line.pitch=id===3?.65:.85;line.volume=1;voice=line;window.speechSynthesis.speak(line);}
  function tone(freq,ms){try{audio??=new (window.AudioContext||window.webkitAudioContext)();audio.resume();const osc=audio.createOscillator(),gain=audio.createGain(),t=audio.currentTime;osc.type='triangle';osc.frequency.value=freq;gain.gain.setValueAtTime(.045,t);gain.gain.exponentialRampToValueAtTime(.001,t+ms/1000);osc.connect(gain).connect(audio.destination);osc.start(t);osc.stop(t+ms/1000);}catch{}}
  const imports={env:{
    host_progress(){try{const data=JSON.parse(localStorage.getItem('desert-goat-progress-v1'));return Array.from({length:9},(_,i)=>i).reduce((mask,i)=>mask|(data?.cleared?.[i]===true?1<<i:0),0);}catch{return 0;}},
    host_save_progress(mask){try{localStorage.setItem('desert-goat-progress-v1',JSON.stringify({cleared:Array.from({length:9},(_,i)=>i).map(i=>!!(mask&(1<<i)))}));return 1;}catch{return 0;}},
    host_voice:speak,host_tone:tone
  }};
  function updateControls(){
    const currentStatus=readString(api.game_status());if(status.textContent!==currentStatus)status.textContent=currentStatus;
    const list=[];for(let i=0;i<api.game_button_count();i++){const ptr=api.game_button(i),v=new Int32Array(api.memory.buffer,ptr,6);list.push({id:v[0],x:v[1],y:v[2],w:v[3],h:v[4],enabled:!!v[5],label:readString(ptr+24,80)});}
    const signature=JSON.stringify(list);if(signature===buttonsSignature)return;buttonsSignature=signature;
    const visible=list.filter(b=>b.label), ids=new Set(visible.map(b=>b.id));
    for(const [id,el] of controlNodes)if(!ids.has(id)){el.remove();controlNodes.delete(id);}
    // Keep real button nodes alive through shaking, animation and meter updates.
    // Rebuilding the whole overlay each frame detaches pointer/focus targets.
    visible.forEach((b,index)=>{
      let el=controlNodes.get(b.id);
      if(!el){el=document.createElement('button');el.dataset.action=b.id;
        el.onfocus=()=>{api.game_focus(b.id);};
        el.onclick=()=>{api.game_action(b.id);canvas.focus({preventScroll:true});paint();};
        controlNodes.set(b.id,el);
      }
      if(el.textContent!==b.label){el.textContent=b.label;el.setAttribute('aria-label',b.label);}
      el.disabled=!b.enabled;el.style.left=`${b.x/canvas.width*100}%`;el.style.top=`${b.y/canvas.height*100}%`;el.style.width=`${b.w/canvas.width*100}%`;el.style.height=`${b.h/canvas.height*100}%`;
      const position=controls.children[index];if(position!==el)controls.insertBefore(el,position||null);
    });
    status.textContent=readString(api.game_status());
  }
  function paint(){ctx.putImageData(image,0,0);updateControls();}
  function frame(time){const dt=last?Math.min((time-last)/1000,.05):0;last=time;api.game_tick(dt);paint();requestAnimationFrame(frame);}
  function point(e){const box=canvas.getBoundingClientRect();return [Math.round((e.clientX-box.left)*canvas.width/box.width),Math.round((e.clientY-box.top)*canvas.height/box.height)];}
  function resize(){if(!api)return;const box=canvas.getBoundingClientRect();api.game_resize(Math.round(box.width),Math.round(box.height));canvas.width=api.game_width();canvas.height=api.game_height();ctx.imageSmoothingEnabled=false;image=new ImageData(new Uint8ClampedArray(api.memory.buffer,api.game_pixels(),canvas.width*canvas.height*4),canvas.width,canvas.height);buttonsSignature='';paint();}
  window.addEventListener('resize',resize);
  canvas.addEventListener('pointerdown',e=>{if(!api||e.button!==0)return;canvas.focus({preventScroll:true});canvas.setPointerCapture(e.pointerId);api.game_pointer(0,...point(e));paint();});
  canvas.addEventListener('pointermove',e=>{if(api)api.game_pointer(1,...point(e));});
  for(const type of ['pointerup','pointercancel'])canvas.addEventListener(type,e=>{if(api){api.game_pointer(2,...point(e));paint();}});
  window.addEventListener('keydown',e=>{if(!api||e.ctrlKey||e.metaKey||e.altKey)return;let key=e.key.length===1?e.key.toUpperCase().charCodeAt(0):({ArrowUp:38,ArrowDown:40,ArrowLeft:37,ArrowRight:39,Enter:13,Escape:27})[e.key];if(key===undefined&&api.game_state()===0&&!['Shift','Control','Alt','Meta','Tab'].includes(e.key))key=13;if(key===undefined)return;if(key===13||(key>=37&&key<=40))canvas.focus({preventScroll:true});e.preventDefault();api.game_key(key,1);paint();});
  window.addEventListener('keyup',e=>{if(!api)return;const key=e.key.length===1?e.key.toUpperCase().charCodeAt(0):({ArrowUp:38,ArrowDown:40,ArrowLeft:37,ArrowRight:39,Enter:13,Escape:27})[e.key];if(key!==undefined)api.game_key(key,0);});
  window.addEventListener('blur',()=>{if(api){api.game_blur();paint();}});
  document.addEventListener('visibilitychange',()=>{if(document.hidden&&api){api.game_blur();last=0;}});
  const bytes=Uint8Array.from(atob(WASM_BASE64),c=>c.charCodeAt(0));
  WebAssembly.instantiate(bytes,imports).then(({instance})=>{api=instance.exports;api.__wasm_call_ctors?.();api.game_init();resize();document.getElementById('loading').hidden=true;requestAnimationFrame(frame);}).catch(error=>{document.getElementById('loading').textContent='Unable to load C++ game. Reload the page.';console.error(error);});
})();
