#!/usr/bin/env python3.5
# 4tri game - server
# Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2011/July/03


import datetime
import json
import os
import pickle
import random
import socket
import sys
import time
import traceback

# sys.path = ['/home/yotam/src/3333/altasio'] + sys.path
import asyncio
# sys.path = ['/home/yotam/src/3333/altws'] + sys.path
import websockets

import consts


def safe_int(s, defret=-1):
    try:
        ret = int(s)
    except:
        ret = defret
    return ret;


def strnow():
    "Return current time as yyyy/mm/dd:HH:MM:SS"
    now = int(time.time())
    lta = time.localtime(now)
    s = "%d/%02d/%02d:%02d:%02d:%02d" % lta[0:6]
    return s


cards = 81*[None]
ci = 0;
for i in range(3):
    for j in range(3):
        for k in range(3):
            for l in range(3):
                cards[ci] = (i, j, k, l)
                ci += 1


def is_set(ci3):
    global cards
    [c0, c1, c2] = list(map(lambda ci: cards[ci], ci3))
    summod3 = 0
    dim = 0
    while dim < 4 and summod3 == 0:
        summod3 = (c0[dim] + c1[dim] + c2[dim]) % 3
        dim += 1
    return summod3 == 0


class GraceFuture(asyncio.Future):

    def __init__(self):
        asyncio.Future.__init__(self)

    def set_result_default(self, result):
        if not self.done():
            self.set_result(result)
        return self.result()

class Player:

    def __init__(self, ws, name, table, passcode):
        self.ws = ws
        self.name = name
        self.table = table
        self.passcode = passcode
        self.say = ""
        self.tcreated = int(time.time())
        self.time_last_action = self.tcreated
        self.time_left = -1
        self.new_game()
        self.log("Player %s, table=%s" % (name, table))

    def __str__(self):
        return "{Player: %s@%s}" % (self.name, self.table.name())

    def new_game(self):
        self.sets_found = 0
        self.bad_calls = 0
        self.add3s_good = 0
        self.add3s_bad = 0
        self.no_more_good = 0
        self.no_more_bad = 0


    def action_time(self):
        self.time_last_action = int(time.time())
        self.table.action_time(self.time_last_action)


    def simple_repr(self):
        return {
            'name': self.name,
            'say': self.say,
            'numbers': self.numbers(),
            'tcreated': self.tcreated,
            'taction': self.time_last_action
        }


    def numbers(self):
        return [self.sets_found,
                self.bad_calls,
                self.add3s_good + self.no_more_good,
                self.add3s_bad + self.no_more_bad]

    def is_owner(self):
        return self is self.table.players[0]


    def left(self):
        t = time.time()
        self.log("Player %s left t=%d" % (self.name, t))
        self.time_left = t
        self.ws = None
        # Should put a time to leave table and disconnect


    def log(self, msg):
        self.table.log(msg, tb_up=4)



class Table:


    def __init__(self, server, passcode):
        self.server = server
        self.passcode = passcode
        self.tstate = 0;
        self.gstate = 0;
        self.game_active = False
        self.players = [] # owner will bw the first
        self.cards_init()
        self.tcreated = int(time.time())
        self.time_last_action = self.tcreated

    def __str__(self):
        return "<Table: %s>" % self.name()

    def name(self):
        p0name = self.players[0].name if len(self.players) > 0 else ""
        return p0name


    def close(self):
        for player in self.players:
            player.table = None


    def leave(self, player):
        self.players.remove(player)
        self.state_bump()


    def cards_init(self):
        N_CARDS = os.getenv("N_CARDS")
        n_cards = 81 if N_CARDS is None else int(N_CARDS)
        self.cards_deck = list(range(n_cards)) # Indices to cards 
        self.cards_active = [] # Indices to cards


    def join(self, ws, player_name, passcode, upass):
        self.log("name=%sm passcode=%s, upass=%s" %
                 (player_name, passcode, upass))
        ret = consts.E3333_OK
        player = None
        if not self.passcode in ("", passcode):
            self.log("self.passcode=%s, passcode=%s" %
                (self.passcode, passcode))
            ret = consts.E3333_TABLE_NOT_AUTHORIZED
        else:
            player = self.player_by_name(player_name)
            if player is None:
                if len(self.players) >= self.server.maxplayers:
                    ret = consts.E3333_TABLE_FULL
                else:
                    player = Player(ws, player_name, self, upass)
                    self.players.append(player)
                    self.state_bump()
            else: # re-connect
                if not player.passcode in ("", upass):
                    ret = consts.E3333_NAME_USED
                else:
                    self.log("reconnect player=%s, s_client: old=%s new=%s" %
                             (player_name, player.ws, ws))
                    self.server.player_close_current_connection(player)
                    player.ws = ws
        self.log("player_name=%s, passcode=%s, upass=%s, ret=%d" %
            (player_name, passcode, upass, ret))
        return (ret, player);


    def owner_left_get(self):
        return self.players[0].time_left


    def player_say(self, player_name, say):
        player = self.player_by_name(player_name)
        if player is not None:
            if player.say != say:
                player.say = say
                self.state_bump();


    def new_game(self):
        self.log("")
        self.game_state_bump()
        self.cards_init()
        for player in self.players:
            player.new_game()
        self.deal_cards_append(12)
        self.game_active = True


    def deal_card(self):
        di = random.randint(0, len(self.cards_deck) - 1)
        # TEMPORARY - An easy trivial: 'first-3 set'.
        # easy = 81 - len(self.cards_deck)
        # if easy < 3:
        #     di = easy
        ci = self.cards_deck[di]
        last = self.cards_deck.pop()
        if di < len(self.cards_deck):
            self.cards_deck[di] = last
        # self.log("di=%d, ci=%d" % (di, ci))
        return ci


    def deal_cards_append(self, n):
        self.game_state_bump()
        for i in range(n):
            self.cards_active.append(self.deal_card())


    def deal_cards_to(self, to3):
        self.game_state_bump()
        to3.sort()
        for i in (2, 1, 0):
            self.cards_active[to3[i]] = self.deal_card()


    def active_popi(self, ai):
        last = self.cards_active.pop()
        if ai < len(self.cards_active):
            self.cards_active[ai] = last


    def active_pop3(self, a3i):
        a3i.sort()
        for i in (2, 1, 0):
            self.active_popi(a3i[i])


    def add3(self, gstate, player):
        self.log("gstate=%d, self.gstate=%d" % (gstate, self.gstate))
        ret = consts.E3333_OK
        if gstate != self.gstate:
            ret = consts.E3333_COLLISION
        else:
            player.action_time();
            if self.active_has_set():
                player.add3s_bad += 1
                ret = consts.E3333_FALSE_NONE
            else:
                player.add3s_good += 1
            self.deal_cards_append(3)
        return ret


    def try3(self, gstate, player, a3i):
        self.log("gstate=%d, player.name=%s, a3i=%s" %
                 (gstate, player.name, str(a3i)))
        ret = consts.E3333_OK
        if gstate != self.gstate:
            ret = consts.E3333_COLLISION
        else:
            player.action_time();
            c3i = list(map(lambda ai: self.cards_active[ai], a3i))
            good = is_set(c3i)
            self.log("good?=%d" % good)
            if is_set(c3i):
                player.sets_found += 1
                if len(self.cards_active) <= 12 and len(self.cards_deck) >= 3:
                    self.deal_cards_to(a3i)
                else:
                    self.active_pop3(a3i)
                self.game_state_bump()
            else:
                ret = consts.E3333_NOT_A_SET
                player.bad_calls += 1
                self.state_bump()
        self.log("ret=%d" % ret)
        return ret;


    def no_more(self, gstate, player):
        self.log("gstate=%d, player_name=%s" % (gstate, player.name))
        ret = consts.E3333_OK
        if gstate != self.gstate:
            ret = consts.E3333_COLLISION
        else:
            player.action_time();
            if len(self.cards_deck) == 0 and not self.active_has_set():
                self.game_active = False
                player.no_more_good += 1
                self.game_state_bump()
            else:
                ret = consts.E3333_YES_MORE
                player.no_more_bad += 1
                self.state_bump()
        self.log("ret=%d" % ret)
        return ret;


    def player_by_name(self, name):
        player = None
        i = 0
        while i < len(self.players) and player is None:
            if self.players[i].name == name:
                player = self.players[i]
            i += 1
        return player


    def owner(self):
        return self.players[0]

    #def passcode(self):
    #    return self.owner().passcode


    def active_has_set(self):
        # Naive implementation
        found = False
        na = len(self.cards_active)
        i = 0
        c3i = [None, None, None]
        while i < na and not found:
            c3i[0] = self.cards_active[i]
            j = i + 1
            while j < na and not found:
                c3i[1] = self.cards_active[j]
                k = j + 1
                while k < na and not found:
                    c3i[2] = self.cards_active[k]
                    found = is_set(c3i)
                    k += 1
                j += 1
            i += 1
        self.log("found=%d" % found);
        return found


    def state_bump(self):
        self.tstate += 1
        self.log("tstate=%d" % self.tstate)


    def game_state_bump(self):
        self.time_last_action = int(time.time())
        self.state_bump()
        self.gstate += 1


    def get_state(self):
        d = {
            "tstate": self.tstate,
            "gstate": self.gstate,
            "gactive": self.game_active,
            "players": list(map(lambda p: p.simple_repr(), self.players)),
            "deck": len(self.cards_deck),
            "active": self.cards_active
        }
        return d


    def action_time(self, t):
        self.time_last_action = t


    def log(self, msg, tb_up=3):
        if self.server is None:
            sys.stderr.write("%s\n" % msg)
        else:
            self.server.log(msg, tb_up=tb_up)


class AppBase:


    def __init__(self, argv, logname):
        self.argv = argv
        self.rc = 0
        self.helped = False
        self.debug = False
        self.flog = None
        try:
            os.stat("/tmp/43333.debug")
            self.debug = True
        except:
            pass
        if self.debug:
            self.flog = open(logname, "a")
            try:
                os.chmod(logname, 0o666)
            except:
                pass


    def ok(self):
        return self.rc == 0

    def mayrun(self):
        return self.ok() and not self.helped

    def error(self, msg):
        self.log(msg)
        if self.ok():
            self.rc = 1
            sys.stderr.write("%s\n" % msg)
            self.usage()


    def log(self, msg, tb_up=2):
        if self.debug:
            scaller = ""
            tb = traceback.extract_stack()
            if len(tb) >= tb_up:
                e = tb[-tb_up]
                fn = e[0].split("/")[-1]
                scaller = "%s[%s:%d<%s>]" % (self.prelog(), fn, e[1], e[2])
            self.flog.write("%s %s: %s\n" % (strnow(), scaller, msg))
            sys.stderr.write("%s: %s\n" % (scaller, msg))
            self.flog.flush()


    def prelog(self):
        return ""


class NameTPassUPass:

    def __init__(self, args_tail, log):
        """ Possible args_tail:
        <name> 0
        <name> 1 <tpass>
        <name> 2 <upass>
        <name> 3 <tpass> <upass>
        """
        log("args_tail=%s" % str(args_tail))
        self.rc = consts.E3333_OK
        self.name = ""
        self.tpass = ""
        self.upass = ""
        if len(args_tail) < 2:
            self.rc = consts.E3333_BAD_COMMAND
        else:
            self.name = args_tail[0]
            ai = 1
            pw_flags = safe_int(args_tail[ai], 0);
            log("pw_flags=0x%x" % pw_flags)
            if not 0 <= pw_flags <= 3:
                self.rc = consts.E3333_BAD_COMMAND
            else:
                na_expected = 2 + [0, 1, 1, 2][pw_flags];
                if len(args_tail) != na_expected:
                    self.rc = consts.E3333_BAD_COMMAND
            if self.rc == consts.E3333_OK:
                if (pw_flags & 0x1) == 0x1:
                    ai += 1
                    self.tpass = args_tail[ai]
                if (pw_flags & 0x2) == 0x2:
                    ai += 1
                    self.upass = args_tail[ai]
        log("name=%s, tpass=%s, upass=%s." %
            (self.name, self.tpass, self.upass))


class Client:

    def __init__(self, ws):
        self.ws = ws
        self.time = time.time()
        self.alive = True
        self.future = GraceFuture()
        self.player = None

    async def produce(self):
        await self.future
        result = self.future.result()
        self.future = GraceFuture()
        return result

    def future_state(self):
        f = self.future
        return "future: @%s cancelled=%s, done=%s, state=%s" % (
            hex(id(f)), f.cancelled(), f.done(), f._state)

    def pre_send(self, message):
        self.future.set_result_default([]).append(message)

    def json_pre_send(self, message):
        self.pre_send(json.dumps(message))


class Server(AppBase):

    default_host = 'localhost'
    default_maxtables = 24
    default_maxplayers = 4
    default_expire = 300
    leave_grace = 60

    def usage(self):
        self.helped = True
        sys.stderr.write(
"""
Usage:                   # [Default]
  %s
  [-h | -help | --help]  # This message
  [-host <n>]            # [%s]
  [-maxtables <n>]       # [%d]
  [-maxplayers <n>]      # [%d]
  [-expire <n>]          # [%d] seconds of no action of at least 2 players.
  [-pidfn <fn>]          # [None] File to record my PID
"""[1:] %
            (self.argv[0],
             self.__class__.default_host,
             self.__class__.default_maxtables,
             self.__class__.default_maxplayers,
             self.__class__.default_expire))


    def __init__(self, argv):
        super().__init__(argv, "/tmp/43333-server.log")
        self.log("")
        self.argv = argv

        # self.ws2a = {}
        self.ra_to_client = {} # remote-address -> client mapping

        # Tables
        self.name2table = {}

        self.maxtables = self.__class__.default_maxtables
        self.maxplayers = self.__class__.default_maxplayers
        self.expire = self.__class__.default_expire
        self.pidfn = None
        self.host = self.__class__.default_host

        self.command_handlers = {
            consts.S3333_C2S_GNEW: self.new_game,
            consts.S3333_C2S_TBLS: self.tables_status,
            consts.S3333_C2S_NTBL: self.new_table,
            consts.S3333_C2S_TRY3: self.try3,
            consts.S3333_C2S_ADD3: self.add3,
            consts.S3333_C2S_NMOR: self.no_more,
            consts.S3333_C2S_JOIN: self.table_join,
            consts.S3333_C2S_CLOS: self.table_close,
        }

        ai = 1
        while ai < len(argv) and self.mayrun():
            opt = argv[ai]
            ai += 1
            if opt in ('-h', '-help', '--help'):
                self.usage()
            elif opt == '-host':
                self.host = argv[ai]; ai += 1;
            elif opt == '-maxtables':
                self.maxtables = int(argv[ai]); ai += 1;
            elif opt == '-maxplayers':
                self.maxplayers = int(argv[ai]); ai += 1;
            elif opt == '-expire':
                self.expire = int(argv[ai]); ai += 1;
            elif opt == '-pidfn':
                self.pidfn = argv[ai]; ai += 1;
            else:
                self.error("Unsupported option: '%s'" % opt)
        self.log("mayrun=%s" % self.mayrun())

    def ws2client(self, ws):
        return self.ra_to_client[ws.remote_address]

    def ws2player(self, ws):
        return self.ws2client(ws).player

    # @ # asyncio.coroutine
    def ws_table(self, ws):
        p = self.ws2player(ws)
        self.log("@=%s, player=%s" % (str(ws.remote_address), p))
        table = None if p is None else p.table
        self.log("table=%s" % table)
        return table

    def make_s2c_command(self, op, error_code, value=None):
        cmd = { 'cmd': op, 'rc': error_code, 'result': value }
        self.log("cmd=%s" % str(cmd))
        return json.dumps(cmd)


    def client_error_response(self, ws, error_code, value=None):
        return self.make_s2c_command(None, error_code, value)

    def client_error_send(self,client, error_code, value=None):
        cmd = self.make_s2c_command(None, error_code, value)
        client.pre_send(cmd)

    def ws_error_send(self, ws, error_code, value=None):
        client = self.ra_to_client[ws.remote_address]
        self.client_error_send(client, error_code, value)

    def refresh_players(self, table):
        self.log("#(players)=%d" % len(table.players));
        tstate = table.get_state()
        s2c_command = self.make_s2c_command(consts.E3333_S2C_GSTATE,
            consts.E3333_OK, tstate)
        self.log("s2c_command=%s" % s2c_command)
        # dstate =  json.dumps(table.get_state());
        for p in table.players:
            if p.ws is None:
                self.log("p.name=%s ws=None" % p.name)
            else:
                client = self.ws2client(p.ws)
                client.pre_send(s2c_command)
        return s2c_command

    def set_announce(self, table, a3i):
        self.log("");
        tstate =  table.get_state()
        c2s_command = self.make_s2c_command(consts.E3333_S2C_SET_FOUND,
            consts.E3333_OK, a3i)
        for p in table.players:
            client = self.ws2client(p.ws)
            client.pre_send(c2s_command)


    def introduce(self, client):
        self.log("");
        client.pre_send(self.tables_status())

    def ws_recv(self, ws):
        try:
            ret = ws.recv()
        except Exception as e:
            ret = None
            self.log("ra=%s, %s" % (str(ws.remote_address), str(e)))
        return ret

    async def ws_handler(self, ws, path):
        self.log("path=%s" % str(path))
        ra = ws.remote_address
        self.ra_to_client[ra] = client = Client(ws)

        producer_task = asyncio.ensure_future(client.produce())
        listener_task = asyncio.ensure_future(self.ws_recv(ws))  # ws.recv()
        self.log("dir(ws)=%s" % str(dir(ws)))
        self.introduce(client)

        loop_count = 0
        while client.alive:
            loop_count += 1
            self.log("loop_count=%d, await async.wait, lt=%s, pt=%s" %
                     (loop_count, str(listener_task), str(producer_task)))
            done, pending = await asyncio.wait(
                [listener_task, producer_task],
                return_when=asyncio.FIRST_COMPLETED)
            self.log("loop_count=%d, #(done)=%d P?=%s, L?=%s" %
                     (loop_count, len(done),
                      producer_task in done, listener_task in done))

            if producer_task in done:
                messages = producer_task.result()
                if ws.open:
                    for message in messages:
                        await ws.send(message)
                    producer_task = asyncio.ensure_future(client.produce())
                else:
                    client.alive = False

            if listener_task in done:
                try:
                    message = listener_task.result()
                except Exception as e:
                    self.warn("listener (%s) %s" % (str(ra), str(e)))
                    message = None
                if message is None:
                    client.alive = False
                else:
                    self.ws_message_handle(ws, message)
                    listener_task = asyncio.ensure_future(self.ws_recv(ws))

        player = client.player
        self.log("client dead: ra=%s, player=%s" % (str(ra), player))
        if player is not None:
            table = player.table
            if player.is_owner():
                self.log("delete table player=%s" % player)
                for peer in table.players[1:]:
                    cmd = self.make_s2c_command(consts.E3333_S2C_TABLE_CLOSED,
                                                consts.E3333_OK, None)
                    pclient = self.ws2client(peer.ws)
                    pclient.pre_send(cmd)
                    pclient.player = None
                del self.name2table[player.name]
                self.refresh_tables_status([client])
            else:
                self.log("leave player=%s, %s" % (player, table))
                table.leave(player)
                self.refresh_players(table)
                
        del self.ra_to_client[ra]
        # del client

    def ws_message_handle(self, ws, message):
        self.log("ws=%s, message=%s" % (str(ws.remote_address), message))
        cmd_args = message.split()
        cmd0 = (cmd_args + [None])[0]
        handler_bad = lambda clnt, args: consts.E3333_BAD_COMMAND
        h = self.command_handlers.get(cmd0, handler_bad)
        self.log("cnd0=%s, h=%s" % (cmd0, h))
        # yield from h(websocket, cmd_args)
        response = h(ws, cmd_args)
        self.log("response=%s" % response)
        if response is not None:
            client = self.ra_to_client[ws.remote_address]
            client.pre_send(response)

    async def periodic_clean(self):
        self.log("")
        loop_count = 0
        while True and loop_count < 10:
            self.log("loop_count=%d" % loop_count)
            await asyncio.sleep(300)
            loop_count += 1

    def run(self):
        sys.stderr.write("Server.run\n")
        self.log("")
        start_server = websockets.serve(self.ws_handler, self.host,
                                        consts.CS3333_PORT)
        loop = asyncio.get_event_loop()
        tasks = [self.periodic_clean(), start_server]
        self.log("#(tasks)=%d" % len(tasks))
        loop.run_until_complete(asyncio.wait(tasks))
        self.log("Calling run_forever")
        loop.run_forever()

    def ts_to_ymdhms_compact(self, ts):
        now = int(time.time())
        lgma = list(map(time.gmtime, ts))
        lgma_now = lgma + [time.gmtime(now)]
        fmt = "%d/%02d/%02d %02d:%02d:%02d"
        set_years = set(map(lambda gma: gma[0], lgma_now))
        if len(set_years) == 1:
            fmt = fmt[3:]
            set_months = set(map(lambda gma: gma[1], lgma_now))
            if len(set_months) == 1:
                fmt = fmt[5:]
                set_mdays = set(map(lambda gma: gma[2], lgma_now))
                if len(set_mdays) == 1:
                    fmt = fmt[5:]
        b = 6 - fmt.count('%')
        ret = list(map(lambda gma: fmt % gma[b:6], lgma))
        return ret

    def tables_status(self, ws=None, cmd_args=None):
        self.log("")
        names = sorted(self.name2table)
        result = []
        for name in names:
            table = self.name2table[name]
            nopw = int(table.passcode == "")
            tblinf = [name, len(table.players), nopw, table.tcreated,
                      table.time_last_action, table.owner_left_get()]
            result.append(tblinf)

        # Display times in compact way
        tcreated_dc = self.ts_to_ymdhms_compact(map(lambda r: r[3], result))
        tlast_acttion = self.ts_to_ymdhms_compact(map(lambda r: r[4], result))
        for i in range(len(result)):
            r = result[i]
            r[3] = tcreated_dc[i]
            r[4] = tlast_acttion[i]
            result[i] = tuple(r)

        s2c_cmd = self.make_s2c_command(consts.E3333_S2C_TBLS,
            consts.E3333_OK, result)
        return s2c_cmd


    def refresh_tables_status(self, clients_exclude=[]):
        cmd = self.tables_status()
        for client in self.ra_to_client.values():
            if client not in clients_exclude:
                client.pre_send(cmd)
        

    def new_table(self, ws, cmd_args):
        self.log("cmd_args=%s" % str(cmd_args))
        rc = consts.E3333_OK
        response = ""
        if len(cmd_args) < 2:
            rc = consts.E3333_BAD_COMMAND
        else:
            ntu = NameTPassUPass(cmd_args[1:], self.log)
            rc = ntu.rc
            if rc == consts.E3333_OK:
                self.log("owner=%s, passcode=%s, upass=%s" % (
                    ntu.name, ntu.tpass, ntu.upass))
                (rc, table) = self.table_allocate(ws,
                    ntu.name, ntu.tpass, ntu.upass)
                self.log("rc=%d" % rc)
                if rc == consts.E3333_OK: # and not table is None:
                    ra = ws.remote_address
                    response = self.make_s2c_command(consts.E3333_S2C_NTBL,
                        consts.E3333_OK)
        self.log("rc=%d, response=%s" % (rc, response))
        if rc == consts.E3333_OK:
            self.refresh_tables_status()
        else:
            response = self.client_error_response(ws, rc)
        return response


    def table_close(self, ws, cmd_args):
        self.log("")
        player = ws.player
        table = player.table if player else None
        self.log("player=%s, table=%s" % (player, table))
        if not table is None:
            if player.is_owner():
                table.close()
                self.log("delete the table")
                del self.name2table[player.name]
            else:
                self.log("Leave the table")
                table.leave(player)
                self.refresh_players(table)
            player.table = None


    def table_join(self, ws, cmd_args):
        self.log("cmd_args=%s" % str(cmd_args))
        client = self.ws2client(ws)
        table = None
        rc = consts.E3333_OK
        if len(cmd_args) < 3:
            rc = consts.E3333_BAD_COMMAND
        else:
            table_name = cmd_args[1]
            table = self.name2table.get(table_name, None)
            if table is None:
                self.log("No such table")
                rc = consts.E3333_NO_TABLE
            else:
                ntu = NameTPassUPass(cmd_args[2:], self.log)
                rc = ntu.rc
                if rc == consts.E3333_OK:
                    (rc, player) = table.join(
                        ws, ntu.name, ntu.tpass, ntu.upass)
                    if rc == consts.E3333_OK:
                        client.player = player
                        s2c_cmd = self.make_s2c_command(
                            consts.E3333_S2C_JOIN, consts.E3333_OK)
                        self.ws2client(ws).pre_send(s2c_cmd)
                        self.refresh_players(table)
        if rc != consts.E3333_OK:
            self.client_error_send(client, rc)


    def player_remove(self, player):
        self.log("player=%s" % player)


    def player_left(self, player):
        self.log("player=%s" % player)
        player.left()
        # Need refresh...
        self.main_loop.add_timeout(
            datetime.timedelta(seconds=self.__class__.leave_grace),
            lambda: self.player_remove(player))

    def player_close_current_connection(self, player):
        self.log("player=%s" % player)
        if not player.ws is None:
            s2c_cmd = self.make_s2c_command(
                consts.E3333_S2C_CONNECTION_TAKEN, consts.E3333_OK)
            player.ws.write_message(s2c_cmd)
            del player.ws
            player.ws = None

    def new_game(self, ws, cmd_args):
        self.log("")
        response = ""
        ret = consts.E3333_OK
        table = self.ws_table(ws)
        self.log("table=%s" % table)
        if table is None:
            response = self.client_error_response(ws,
                                                  consts.E3333_NO_TABLE)
        else:
            table.new_game()
            self.log("call refresh_players")
            response = self.refresh_players(table)
        self.log("response=%s" % response)
        return response


    def add3(self, ws, cmd_args):
        self.log("")
        client = self.ws2client(ws)
        player = client.player
        table = player.table if player else None
        if len(cmd_args) != 2:
            self.client_error_send(client, consts.E3333_BAD_COMMAND)
        if table is None:
            self.client_error_send(client, consts.E3333_NO_TABLE)
        else:
            gstate = safe_int(cmd_args[1])
            ret = table.add3(gstate, player)
            self.log("ret=%d" % ret)
            if ret != consts.E3333_OK:
                self.ws_error_send(ws, ret)
            if ret != consts.E3333_COLLISION:
                self.refresh_players(table)


    def try3(self, ws, cmd_args):
        self.log("")
        player = self.ws2player(ws)
        table = player.table if player else None
        self.log("player=%s, table=%s" % (player, table))
        if len(cmd_args) != 5:
            self.ws_error_send(ws, consts.E3333_BAD_COMMAND)
        elif table is None:
            self.ws_error_send(ws, consts.E3333_NO_TABLE)
        else:
            gstate = safe_int(cmd_args[1])
            a3i = list(map(safe_int, cmd_args[2:]))
            self.log("table.name=%s, gstate=%d, player=%s, a3i=%s" %
                     (table.name(), gstate, player, a3i))
            ret = table.try3(gstate, player, a3i)
            self.log("ret=%d" % ret);
            if ret != consts.E3333_OK:
                self.ws_error_send(ws, ret)
            if ret != consts.E3333_COLLISION:
                if ret == consts.E3333_OK:
                    self.set_announce(table, a3i)
                self.refresh_players(table)


    def no_more(self, ws, cmd_args):
        self.log("")
        client = self.ws2client(ws)
        player = client.player
        table = player.table if player else None
        if len(cmd_args) != 2:
            self.client_error_send(client, consts.E3333_BAD_COMMAND)
        elif table is None:
            self.client_error_send(client, consts.E3333_NO_TABLE)
        if table is None:
            ret = consts.E3333_NO_TABLE
        else:
            gstate = safe_int(cmd_args[1])
            self.log("gstate=%d, player=%s" % (gstate, player))
            ret = table.no_more(gstate, player)
            if ret != consts.E3333_OK:
                self.client_error_send(client, ret)
            if ret != consts.E3333_COLLISION:
                self.refresh_players(table)


    def table_allocate(self, ws, owner_name, passcode, upass):
        self.log("owner_name=%s, passcode=%s" % (owner_name, passcode))
        ret = consts.E3333_OK
        table = None
        if len(self.name2table) > self.maxtables:
            ret = self.expire_old_table()
        if ret == consts.E3333_OK:
            if owner_name in self.name2table.keys():
                ret = consts.E3333_NAME_USED
        if ret == consts.E3333_OK:
            table = Table(self, passcode)
            owner = Player(ws, owner_name, table, upass)
            table.players = [owner]
            client = self.ra_to_client[ws.remote_address]
            client.player = owner
            self.name2table[owner_name] = table
        self.log("ret=%d" % ret)
        return (ret, table)


    def expire_old_table(self):
        ret = consts.E3333_OK
        toble_old = None
        now = int(time.time())
        age_max = -1
        for (id, table) in self.id2table.items():
            age = now - table.time_last_action
            if age < age_max:
                age = age_max
                table_old = table
        if age_max >= self.expire:
            self.log("Table expired: name=%s, age=%d" % (table.name(), age_max))
            self.table_free(table)
        else:
            ret = consts.E3333_CLUB_FULL
        self.log("ret=%d" % ret)
        return ret


    def table_free(self, table):
        self.log("name=%s" % (table.name()))
        del self.name2table[table.name()]
        del table

    def warn(self, msg):
        sys.stderr.write("Warning: %s\n" % msg)
        self.log(msg, tb_up=3)


if __name__ == "__main__":
    p = Server(sys.argv)
    if p.mayrun():
        p.run()
    sys.exit(p.rc)
