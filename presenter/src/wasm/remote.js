var f;f||=typeof Module != 'undefined' ? Module : {};var aa=Object.assign({},f),k=(a,b)=>{throw b;},p="";"undefined"!=typeof document&&document.currentScript&&(p=document.currentScript.src);p=p.startsWith("blob:")?"":p.substr(0,p.replace(/[?#].*/,"").lastIndexOf("/")+1);var ba=f.print||console.log.bind(console),r=f.printErr||console.error.bind(console);Object.assign(f,aa);aa=null;f.quit&&(k=f.quit);var t;f.wasmBinary&&(t=f.wasmBinary);
var ca,u=!1,v,w,da,ea,x,y,A,B,fa=[],ha=[],ia=[],ja=[];function ka(){var a=f.preRun.shift();fa.unshift(a)}var C=0,D=null,E=null;function F(a){f.onAbort?.(a);a="Aborted("+a+")";r(a);u=!0;throw new WebAssembly.RuntimeError(a+". Build with -sASSERTIONS for more info.");}var la=a=>a.startsWith("data:application/octet-stream;base64,"),G;function ma(a){if(a==G&&t)return new Uint8Array(t);throw"both async and sync fetching of the wasm failed";}
function na(a){return t||"function"!=typeof fetch?Promise.resolve().then(()=>ma(a)):fetch(a,{credentials:"same-origin"}).then(b=>{if(!b.ok)throw`failed to load wasm binary file at '${a}'`;return b.arrayBuffer()}).catch(()=>ma(a))}function oa(a,b,c){return na(a).then(d=>WebAssembly.instantiate(d,b)).then(c,d=>{r(`failed to asynchronously prepare wasm: ${d}`);F(d)})}
function pa(a,b){var c=G;t||"function"!=typeof WebAssembly.instantiateStreaming||la(c)||"function"!=typeof fetch?oa(c,a,b):fetch(c,{credentials:"same-origin"}).then(d=>WebAssembly.instantiateStreaming(d,a).then(b,function(e){r(`wasm streaming compile failed: ${e}`);r("falling back to ArrayBuffer instantiation");return oa(c,a,b)}))}var qa={9532:a=>{a=H(a);console.log(a);window.electronAPI.sendMessageToPresenter(a)}};
function ra(a){this.name="ExitStatus";this.message=`Program terminated with exit(${a})`;this.status=a}var I=a=>{for(;0<a.length;)a.shift()(f)},sa=f.noExitRuntime||!0,ta=new TextDecoder("utf8"),H=(a,b)=>{if(!a)return"";b=a+b;for(var c=a;!(c>=b)&&w[c];)++c;return ta.decode(w.subarray(a,c))};f.UTF8ToString=H;class ua{constructor(a){this.ma=a-24}}var va=0,wa=0,J=[],K=[],xa,ya=a=>{var b=K[a];b||(a>=K.length&&(K.length=a+1),K[a]=b=xa.get(a));return b},L=0,N=[];
function za(){if(navigator.userActivation?navigator.userActivation.isActive:L&&Aa.Ca)for(var a=0;a<N.length;++a){var b=N[a];N.splice(a,1);--a;b.Oa(...b.Da)}}var O=[];function Ba(a){var b=O[a];b.target.removeEventListener(b.oa,b.qa,b.ua);O.splice(a,1)}function Ca(a){if(!a.target)return-4;if(a.wa)a.qa=function(c){++L;Aa=a;za();a.Aa(c);za();--L},a.target.addEventListener(a.oa,a.qa,a.ua),O.push(a);else for(var b=0;b<O.length;++b)O[b].target==a.target&&O[b].oa==a.oa&&Ba(b--);return 0}
var Aa,Da,Ea=[0,document,window],P=a=>{a=2<a?H(a):a;return Ea[a]||document.querySelector(a)},Q=(a,b,c)=>{var d=w;if(0<c){c=b+c-1;for(var e=0;e<a.length;++e){var g=a.charCodeAt(e);if(55296<=g&&57343>=g){var h=a.charCodeAt(++e);g=65536+((g&1023)<<10)|h&1023}if(127>=g){if(b>=c)break;d[b++]=g}else{if(2047>=g){if(b+1>=c)break;d[b++]=192|g>>6}else{if(65535>=g){if(b+2>=c)break;d[b++]=224|g>>12}else{if(b+3>=c)break;d[b++]=240|g>>18;d[b++]=128|g>>12&63}d[b++]=128|g>>6&63}d[b++]=128|g&63}}d[b]=0}},Ga=(a,b,
c,d,e,g)=>{Da||=Fa(176);a={target:P(a),oa:g,wa:d,Aa:h=>{var m=Da;B[m>>3]=h.timeStamp;var n=m>>2;x[n+2]=h.location;x[n+3]=h.ctrlKey;x[n+4]=h.shiftKey;x[n+5]=h.altKey;x[n+6]=h.metaKey;x[n+7]=h.repeat;x[n+8]=h.charCode;x[n+9]=h.keyCode;x[n+10]=h.which;Q(h.key||"",m+44,32);Q(h.code||"",m+76,32);Q(h.char||"",m+108,32);Q(h.locale||"",m+140,32);ya(d)(e,m,b)&&h.preventDefault()},ua:c};return Ca(a)},Ha=1,Ia=[],R=[],S=[],T=[],U=[],V=a=>{for(var b=Ha++,c=a.length;c<b;c++)a[c]=null;return b},Ka=(a,b,c,d)=>{for(var e=
0;e<a;e++){var g=W[c](),h=g&&V(d);g?(g.name=h,d[h]=g):Ja||=1282;x[b+4*e>>2]=h}},Ma=(a,b)=>{a.ma||(a.ma=a.getContext,a.getContext=function(d,e){e=a.ma(d,e);return"webgl"==d==e instanceof WebGLRenderingContext?e:null});var c=1<b.ra?a.getContext("webgl2",b):a.getContext("webgl",b);return c?La(c,b):0},La=(a,b)=>{var c=V(U);b={handle:c,attributes:b,version:b.ra,va:a};a.canvas&&(a.canvas.Ba=b);U[c]=b;return c},Ja,X,Na=["default","low-power","high-performance"],Oa=[null,[],[]],Pa=a=>"]"==a.slice(-1)&&a.lastIndexOf("["),
Qa=a=>{a-=5120;return 0==a?v:1==a?w:2==a?da:4==a?x:6==a?A:5==a||28922==a||28520==a||30779==a||30782==a?y:ea},Ra=a=>{var b=W.ya;if(b){var c=b.na[a];"number"==typeof c&&(b.na[a]=c=W.getUniformLocation(b,b.sa[a]+(0<c?`[${c}]`:"")));return c}Ja||=1282};function Sa(a){W.pixelStorei(37440,!0);W.texImage2D(3553,0,6408,6408,5121,a);W.pixelStorei(37440,!1)}
var W,Ta={d:(a,b,c,d)=>{F(`Assertion failed: ${H(a)}, at: `+[b?H(b):"unknown filename",c,d?H(d):"unknown function"])},s:(a,b,c)=>{var d=new ua(a);y[d.ma+16>>2]=0;y[d.ma+4>>2]=b;y[d.ma+8>>2]=c;va=a;wa++;throw va;},t:()=>{F("")},u:()=>1,x:(a,b,c)=>w.copyWithin(a,b,b+c),z:(a,b,c)=>{J.length=0;for(var d;d=w[b++];){var e=105!=d;e&=112!=d;c+=e&&c%8?4:0;J.push(112==d?y[c>>2]:105==d?x[c>>2]:B[c>>3]);c+=e?8:4}return qa[a](...J)},r:()=>devicePixelRatio,w:()=>performance.now(),b:()=>Math.random(),j:(a,b)=>{function c(d){ya(a)(d,
b)&&requestAnimationFrame(c)}return requestAnimationFrame(c)},v:()=>!1,p:(a,b,c)=>{a=P(a);if(!a)return-4;a.width=b;a.height=c;return 0},q:(a,b,c)=>{a=P(a);if(!a)return-4;a.style.width=b+"px";a.style.height=c+"px";return 0},T:(a,b,c,d)=>Ga(a,b,c,d,2,"keydown"),M:(a,b,c,d)=>Ga(a,b,c,d,3,"keyup"),U:a=>{throw H(a);},S:(a,b)=>{b>>=2;b={alpha:!!x[b],depth:!!x[b+1],stencil:!!x[b+2],antialias:!!x[b+3],premultipliedAlpha:!!x[b+4],preserveDrawingBuffer:!!x[b+5],powerPreference:Na[x[b+6]],failIfMajorPerformanceCaveat:!!x[b+
7],ra:x[b+8],La:x[b+9],Ga:x[b+10],za:x[b+11],Ma:x[b+12],Na:x[b+13]};a=P(a);return!a||b.za?0:Ma(a,b)},R:a=>{X=U[a];f.Ea=W=X?.va;return!a||W?0:-5},i:(a,b,c,d)=>{for(var e=0,g=0;g<c;g++){var h=y[b>>2],m=y[b+4>>2];b+=8;for(var n=0;n<m;n++){var l=w[h+n],q=Oa[a];if(0===l||10===l){for(l=0;q[l]&&!(NaN<=l);)++l;l=ta.decode(q.buffer?q.subarray(0,l):new Uint8Array(q.slice(0,l)));(1===a?ba:r)(l);q.length=0}else q.push(l)}e+=m}y[d>>2]=e;return 0},y:function(a,b,c){function d(){for(var h=g.getImageData(0,0,e.width,
e.height),m=new Uint32Array(h.data.buffer),n=h.width-1;0<n;--n)for(var l=n;l<m.length;l+=h.width)if(m[l])return n}32>=a&&(a=105);32>=b&&(b=105);b=String.fromCharCode(b);var e=document.createElement("canvas");e.height=c;e.width=3*c;var g=e.getContext("2d");g.font=c+"px Arial Unicode";g.fillText(b,0,e.height-7);c=d();g.fillText(String.fromCharCode(a)+b,0,e.height-7);return d()-c},l:(a,b)=>{W.attachShader(R[a],T[b])},P:(a,b,c)=>{W.bindAttribLocation(R[a],b,H(c))},I:(a,b)=>{35051==a?W.Fa=b:35052==a&&
(W.xa=b);W.bindBuffer(a,Ia[b])},c:(a,b)=>{W.bindTexture(a,S[b])},K:(a,b)=>W.blendFunc(a,b),H:(a,b,c,d)=>{2<=X.version?c&&b?W.bufferData(a,w,d,c,b):W.bufferData(a,b,d):W.bufferData(a,c?w.subarray(c,c+b):b,d)},C:a=>W.clear(a),D:(a,b,c,d)=>W.clearColor(a,b,c,d),m:a=>{W.compileShader(T[a])},Q:()=>{var a=V(R),b=W.createProgram();b.name=a;b.Ka=b.Ia=b.Ja=0;b.pa=1;R[a]=b;return a},o:a=>{var b=V(T);T[b]=W.createShader(a);return b},e:(a,b,c)=>{W.drawArrays(a,b,c)},L:a=>W.enable(a),F:a=>{W.enableVertexAttribArray(a)},
J:(a,b)=>{Ka(a,b,"createBuffer",Ia)},h:(a,b)=>{Ka(a,b,"createTexture",S)},k:(a,b)=>{b=H(b);if(a=R[a]){var c=a,d=c.na,e=c.ta,g;if(!d)for(c.na=d={},c.sa={},g=0;g<W.getProgramParameter(c,35718);++g){var h=W.getActiveUniform(c,g);var m=h.name;h=h.size;var n=Pa(m);n=0<n?m.slice(0,n):m;var l=c.pa;c.pa+=h;e[n]=[h,l];for(m=0;m<h;++m)d[l]=m,c.sa[l++]=n}c=a.na;d=0;e=b;g=Pa(b);0<g&&(d=parseInt(b.slice(g+1))>>>0,e=b.slice(0,g));if((e=a.ta[e])&&d<e[0]&&(d+=e[1],c[d]=c[d]||W.getUniformLocation(a,b)))return d}else Ja||=
1281;return-1},O:a=>{a=R[a];W.linkProgram(a);a.na=0;a.ta={}},n:(a,b,c,d)=>{for(var e="",g=0;g<b;++g)e+=H(y[c+4*g>>2],d?y[d+4*g>>2]:void 0);W.shaderSource(T[a],e)},E:(a,b,c,d,e,g,h,m,n)=>{if(2<=X.version){if(W.xa){W.texImage2D(a,b,c,d,e,g,h,m,n);return}if(n){var l=Qa(m);n>>>=31-Math.clz32(l.BYTES_PER_ELEMENT);W.texImage2D(a,b,c,d,e,g,h,m,l,n);return}}if(n){l=Qa(m);var q=e*(d*({5:3,6:4,8:2,29502:3,29504:4,26917:2,26918:2,29846:3,29847:4}[h-6402]||1)*l.BYTES_PER_ELEMENT+4-1&-4);n=l.subarray(n>>>31-Math.clz32(l.BYTES_PER_ELEMENT),
n+q>>>31-Math.clz32(l.BYTES_PER_ELEMENT))}else n=null;W.texImage2D(a,b,c,d,e,g,h,m,n)},a:(a,b,c)=>W.texParameteri(a,b,c),f:(a,b,c,d,e)=>{W.uniform4f(Ra(a),b,c,d,e)},g:(a,b,c,d)=>{2<=X.version?b&&W.uniformMatrix4fv(Ra(a),!!c,A,d>>2,16*b):(b=A.subarray(d>>2,d+64*b>>2),W.uniformMatrix4fv(Ra(a),!!c,b))},N:a=>{a=R[a];W.useProgram(a);W.ya=a},G:(a,b,c,d,e,g)=>{W.vertexAttribPointer(a,b,c,!!d,e,g)},B:function(a,b,c,d){var e=new Image;e.onload=function(){y[c>>2]=e.width;y[d>>2]=e.height;W.bindTexture(3553,
S[a]);Sa(e)};e.src=H(b)},A:function(a,b,c){var d=document.createElement("canvas");d.width=d.height=b;var e=d.getContext("2d");e.fillStyle="black";e.Ha="copy";e.globalAlpha=0;e.fillRect(0,0,d.width,d.height);e.globalAlpha=1;e.fillStyle="white";e.font=b+"px Arial Unicode";c&&(e.shadowColor="black",e.shadowOffsetX=2,e.shadowOffsetY=2,e.shadowBlur=3,e.strokeStyle="gray",e.strokeText(String.fromCharCode(a),0,d.height-7));e.fillText(String.fromCharCode(a),0,d.height-7);Sa(d)}},Y=function(){function a(c){Y=
c.exports;ca=Y.V;c=ca.buffer;f.HEAP8=v=new Int8Array(c);f.HEAP16=da=new Int16Array(c);f.HEAPU8=w=new Uint8Array(c);f.HEAPU16=ea=new Uint16Array(c);f.HEAP32=x=new Int32Array(c);f.HEAPU32=y=new Uint32Array(c);f.HEAPF32=A=new Float32Array(c);f.HEAPF64=B=new Float64Array(c);xa=Y.ia;ha.unshift(Y.W);C--;f.monitorRunDependencies?.(C);0==C&&(null!==D&&(clearInterval(D),D=null),E&&(c=E,E=null,c()));return Y}var b={a:Ta};C++;f.monitorRunDependencies?.(C);if(f.instantiateWasm)try{return f.instantiateWasm(b,
a)}catch(c){return r(`Module.instantiateWasm callback failed with error: ${c}`),!1}G||=la("remote.wasm")?"remote.wasm":f.locateFile?f.locateFile("remote.wasm",p):p+"remote.wasm";pa(b,function(c){a(c.instance)});return{}}(),Fa=a=>(Fa=Y.X)(a);f._free=a=>(f._free=Y.Y)(a);f._clear_screen=(a,b,c,d)=>(f._clear_screen=Y.Z)(a,b,c,d);f._fill_solid_rectangle=(a,b,c,d,e,g,h,m)=>(f._fill_solid_rectangle=Y._)(a,b,c,d,e,g,h,m);f._rand01=()=>(f._rand01=Y.$)();
f._fill_image=(a,b,c,d,e,g,h,m)=>(f._fill_image=Y.aa)(a,b,c,d,e,g,h,m);f._fill_text=(a,b,c,d,e,g,h,m,n,l)=>(f._fill_text=Y.ba)(a,b,c,d,e,g,h,m,n,l);f._send_presenter_msg=a=>(f._send_presenter_msg=Y.ca)(a);f._get_remote_mac=()=>(f._get_remote_mac=Y.da)();f._fake_presenter_state_dark=()=>(f._fake_presenter_state_dark=Y.ea)();f._init_webgl=(a,b)=>(f._init_webgl=Y.fa)(a,b);f._set_animation_frame_callback=a=>(f._set_animation_frame_callback=Y.ga)(a);
var Ua=f._main=(a,b)=>(Ua=f._main=Y.ha)(a,b),Va=a=>(Va=Y.ja)(a),Wa=a=>(Wa=Y.ka)(a),Xa=()=>(Xa=Y.la)();
f.ccall=(a,b,c,d)=>{var e={string:l=>{var q=0;if(null!==l&&void 0!==l&&0!==l){for(var z=q=0;z<l.length;++z){var M=l.charCodeAt(z);127>=M?q++:2047>=M?q+=2:55296<=M&&57343>=M?(q+=4,++z):q+=3}q+=1;z=Wa(q);Q(l,z,q);q=z}return q},array:l=>{var q=Wa(l.length);v.set(l,q);return q}};a=f["_"+a];var g=[],h=0;if(d)for(var m=0;m<d.length;m++){var n=e[c[m]];n?(0===h&&(h=Xa()),g[m]=n(d[m])):g[m]=d[m]}c=a(...g);return c=function(l){0!==h&&Va(h);return"string"===b?H(l):"boolean"===b?!!l:l}(c)};var Z;
E=function Ya(){Z||Za();Z||(E=Ya)};
function Za(){function a(){if(!Z&&(Z=!0,f.calledRun=!0,!u)){I(ha);I(ia);if(f.onRuntimeInitialized)f.onRuntimeInitialized();if($a){var b=Ua;try{var c=b(0,0);sa||(f.onExit?.(c),u=!0);k(c,new ra(c))}catch(d){d instanceof ra||"unwind"==d||k(1,d)}}if(f.postRun)for("function"==typeof f.postRun&&(f.postRun=[f.postRun]);f.postRun.length;)b=f.postRun.shift(),ja.unshift(b);I(ja)}}if(!(0<C)){if(f.preRun)for("function"==typeof f.preRun&&(f.preRun=[f.preRun]);f.preRun.length;)ka();I(fa);0<C||(f.setStatus?(f.setStatus("Running..."),
setTimeout(function(){setTimeout(function(){f.setStatus("")},1);a()},1)):a())}}if(f.preInit)for("function"==typeof f.preInit&&(f.preInit=[f.preInit]);0<f.preInit.length;)f.preInit.pop()();var $a=!0;f.noInitialRun&&($a=!1);Za();
