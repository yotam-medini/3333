import { TableStatus, CallBackIdx } from "./consts";
import * as c from "./cs_consts";
console.log("S3333_C2S_NTBL=" + c.S3333_C2S_NTBL);

let E3333_OK = 0;

let win_hostname = window.location.hostname
if (win_hostname == "") { win_hostname = "localhost"; }
var host = "ws://" + win_hostname + ":" + "9090";
console.log("host="+host)
export let web_socket = new WebSocket(host);
console.log("web_socket: ", web_socket);

console.log("readyState=" + web_socket.readyState);
web_socket.onopen = function () {
  console.log("websocket.onopen");
  getTablesStatus();
};

// Callbacks:
let callbacks = []
for (let i = 0; i < CallBackIdx.ISIZE; ++i) {
  callbacks[i] = undefined;
}
export let set_callback = function (cbi: CallBackIdx, fn) {
  console.log("set_callback(" + cbi + ")");
  callbacks[cbi] = fn;
}

export let cb_updateTableStatus = undefined;


web_socket.onmessage = function (event) {
  console.log("event.data=" + event.data);
  let edata = JSON.parse(event.data);
  let rc = edata['rc'];
  console.log("edata="+edata+", rc="+rc);
  if (rc == E3333_OK) {
    let cmd = edata['cmd'];
    console.log("cmd="+cmd);
    let fn = undefined;
    let result = undefined;
    switch (cmd) {
      case c.E3333_S2C_TBLS:
        fn = callbacks[CallBackIdx.IClubTables];
        console.log("fn?=" + (fn !== undefined));
        result = edata['result'];
        console.log("result: ", result);
        if (fn !== undefined) {
          fn(result);
        }
        break;
      case c.E3333_S2C_NTBL:
        fn = callbacks[CallBackIdx.ITableStatus];
        console.log("fn?=" + (fn !== undefined));
        if (fn !== undefined) {
          fn(TableStatus.Own);
        }
        result = edata['result'];
        let table_name = result["table_name"];
        fn = callbacks[CallBackIdx.iSetNames];
        console.log("table_name="+table_name + " fn?=" + (fn !== undefined));
        fn(table_name, table_name);
        break;
      case c.E3333_S2C_GSTATE:
        console.log("c.E3333_S2C_GSTATE");
        result = edata['result'];
        let active = result['active'];
        fn = callbacks[CallBackIdx.ISetStates];
        fn(result['tstate'], result['gstate']);
        fn = callbacks[CallBackIdx.ISetCards];
        fn(active);
        callbacks[CallBackIdx.iSetDeck](result['deck']);
        callbacks[CallBackIdx.iSetGameState](result);
        break;
      case c.E3333_S2C_NOT_A_SET:
        console.log("E3333_S2C_NOT_A_SET");
        callbacks[CallBackIdx.INotASet]();
        break;
      default:
        console.log("Error: unsupported cmd: " + cmd);
    }
  }
}
console.log("web_socket: ", web_socket);

export function getTablesStatus() {
  console.log("web_socket: ", web_socket);
  web_socket.send("tbls");
}

export function sendClubRefresh() {
  let request = [c.S3333_C2S_TBLS];
  console.log("request="+request);
  web_socket.send(request);
}

export function sendNewTable(
    table_name: string,
    table_password: string,
    player_password: string ) {
  console.log("sendNewTable");
  let request = [
    c.S3333_C2S_NTBL, table_name, 3, table_password, player_password].join(" ")
  console.log("request="+request);
  web_socket.send(request);
}

export function sendNewGame() {
  web_socket.send(c.S3333_C2S_GNEW);
}  

export function sendTry3(caards_selected: number[3], gstate: number) {
  let request = [c.S3333_C2S_TRY3 + " " + gstate + " " + 
    caards_selected.join(" ")]
  console.log("request="+request);
  web_socket.send(request);
}

export function sendAdd3(gstate: number) {
  let request = [c.S3333_C2S_ADD3 + " " + gstate]
  console.log("request="+request);
  web_socket.send(request);
}

export function sendNoMore(gstate: number) {
  let request = [c.S3333_C2S_NMOR + " " + gstate]
  console.log("request="+request);
  web_socket.send(request);
}

export function sendPlayersRefresh() {
  let request = [c.S3333_C2S_PLRF];
  console.log("request="+request);
  web_socket.send(request);
}

export function joinTable(table_name: string) { // not yet
  let request = [c.S3333_C2S_JOIN + " " + gstate]
  console.log("request="+request);
  web_socket.send(request);
}
