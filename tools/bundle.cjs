const fs=require('node:fs'),path=require('node:path');
const root=path.resolve(__dirname,'..');
const wasm=fs.readFileSync(path.join(root,'build/desert-goat.wasm'));
const adapter=fs.readFileSync(path.join(root,'src/browser.js'),'utf8');
fs.writeFileSync(path.join(root,'game.js'),'// GENERATED from C++ by build.ps1. Do not edit.\nconst WASM_BASE64='+JSON.stringify(wasm.toString('base64'))+';\n'+adapter);
console.log('C++ browser build: '+wasm.length+' bytes of WebAssembly.');
