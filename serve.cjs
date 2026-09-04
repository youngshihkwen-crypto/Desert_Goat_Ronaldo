const http = require('node:http');
const fs = require('node:fs');
const path = require('node:path');
const files = {'/':'index.html','/index.html':'index.html','/style.css':'style.css','/game.js':'game.js'};
http.createServer((req,res)=>{
 const file=files[new URL(req.url,'http://localhost').pathname];
 if(!file){res.writeHead(404);res.end();return}
 fs.readFile(path.join(__dirname,file),(error,data)=>{
  if(error){res.writeHead(500);res.end('Unable to load file');return}
  res.writeHead(200,{'Content-Type':file.endsWith('.js')?'text/javascript; charset=utf-8':file.endsWith('.css')?'text/css; charset=utf-8':'text/html; charset=utf-8','Cache-Control':'no-store'});res.end(data);
 });
}).listen(4173,'127.0.0.1',()=>console.log('Game: http://127.0.0.1:4173'));
