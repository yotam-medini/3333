export enum TableStatus {
  None,
  Own,
  Join
};

export enum CallBackIdx {
  IClubTables,
  ITableStatus,
  iSetNames,
  iSetGameState,
  ISetStates,
  ISetCards,
  iSetDeck,
  INotASet,
  ISIZE
};

let dummy_player = [];
dummy_player["say"] = "";
dummy_player["numbers"] = [0, 0, 0, 0];
dummy_player["tcreated"] = 0;
dummy_player["tactive"] = 0;

export let game_state_empty = [];
game_state_empty["tstate"] = -1;
game_state_empty["gstate"] = -1;
game_state_empty["players"] = [dummy_player];
game_state_empty["deck"] = 81;
game_state_empty["active"] = [];


