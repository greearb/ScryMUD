#
# ScryMUD Server Code
# Copyright (C) 1998-1999  Ben Greear
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# To contact the Author, Ben Greear:  greear@cyberhighway.net, (preferred)
#                                     greearb@agcs.com
#


#  This file contains the command definitions for ScryMUD.
#  If you wish to add more, feel free.  However, the order
#  is somewhat important:  The entries will be searched from
#  bottom of this file to the top.  This allows you to specify
#  how the commands are matched, ie put south at the bottom of
#  all commands starting with 's', so that if the player enters
#  's', then she will go south, instead of, say, sitting.
#
#  This file does not gracefully handle comments yet, so DO NOT
#  intersperse comments between command definitions
#
#  The format is as follows:
#  [cmd_name] <cmd_name ...> <*>[help_key] ~
#  [actual code to call]
#
#  The entries below should make it more clear.  Deviate at your
#  own peril!!
#
#  NOTE 1:  a * in front of a help key means that the entry is ONLY
#           for help.  You must still put in something for the command,
#           but it will not be put into the code anywhere (I usually
#           just use NOP.
#
#  NOTE 2:  the code for a given command must return an integer.  The
#           normal syntax is:  0 means everything is OK, < 0 means
#           an error.
#
#  NOTE 3:  The commands/help keys must be terminated with a tilde (~).
#           It should be possible for them to span multiple lines.
#           At this time, multi-word commands are not supported.
#
#  NOTE 4:  If there is only one command followed by a tilde, then the
#           command is treated both as a command AND the help key for
#           that command.  Nice little short-hand...


#  NOTE on HELP ONLY entries:  Please put them at the end so they won't
#       interfere with other commands.


# Mortal Commands - A
abilities ~
return abilities(pc);
ack socials ~
ack(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
adjust_register ~
return adjust_register(i, &(cooked_strs[1]), j, pc);
afk afk ~
afk(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
agree socials ~
agree(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
alias ~
return add_crit_alias(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
announce socials ~
announce(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
assist ~
return assist(i, &(cooked_strs[1]), pc);
# IMM commands - A
add_casts_spell ~
return add_casts_spell(i, j, k, pc);
add_keyword ~
return add_keyword(pc);
add_kname ~
return add_kname(i, &(cooked_strs[1]), pc);
add_mname ~
return add_mname(i, &(cooked_strs[1]), pc);
add_mob_script ~
return add_mob_script(pc, i, (cooked_strs[1]), j, (cooked_strs[2]), k, l);
add_obj_script add_mob_script ~
return add_obj_script(pc, i, (cooked_strs[1]), j, (cooked_strs[2]), k, l);
add_path ~
return add_path(i, j, pc);
add_proc ~
return add_proc(i, pc);
add_room_script add_mob_script ~
return add_room_script(pc, i, (cooked_strs[1]), j, (cooked_strs[2]), k, l);
add_oname ~
return add_oname(i, &(cooked_strs[1]), pc);
add_perm_inv ~
return add_perm_inv(i, j, &(cooked_strs[1]), pc);
add_stat_affect ~
return add_stat_affect(i, j, k, pc);
add_zone ~
return addZone(i, pc);
adlist ~
return adlist(i, pc);
adsave ~
return adsave(i, pc);
affect_hp ~
return affect_crit_stat(STAT_HP, cooked_strs[2], i, &(cooked_strs[1]), j, k, pc.getCurRoom(), &pc);
affect_mana ~
return affect_crit_stat(STAT_MANA, cooked_strs[2], i, &(cooked_strs[1]), j, k, pc.getCurRoom(), &pc);
affect_mov ~
return affect_crit_stat(STAT_MOV, cooked_strs[2], i, &(cooked_strs[1]), j, k, pc.getCurRoom(), &pc);
align_greater_than ~
return align_greater_than(i, pc);
align_less_than ~
return align_less_than(i, pc);
alist zlist zlist ~
return zlist(i, j, pc);
always_pay ~
return always_pay(i, j, &(cooked_strs[1]), pc, was_ordered);
amlist ~
return amlist(i, j, pc);
amsave ~
return amsave(i, pc);
aolist ~
return aolist(i, j, pc);
aosave ~
return aosave(i, pc);
arlist ~
return arlist(i, j, pc);
area_mset ~
return area_mset(pc, &(cooked_strs[1]), &(cooked_strs[2]), j);

# Mortal Commands - B
bah socials ~
bah(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
balance ~
return balance(i, &(cooked_strs[1]), pc);
bash ~
return bash(i, &cooked_strs[1], pc);
battlecry socials ~
battlecry(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
beckon socials ~
beckon(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
beep ~
return beep(i, &(cooked_strs[1]), pc);
berserk ~
return berserk(pc);
# behead ~
# return behead(i, &(cooked_strs[1]), pc);
bite socials ~
bite(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
blend ~
return blend(pc);
blink socials ~
blink(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
block ~
return block(i, &(cooked_strs[1]), pc);
blush socials ~
blush(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
bonk socials ~
bonk(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
bounce socials ~
bounce(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
bow socials ~
bow(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
brief ~
return brief(pc);
buglist ~
return buglist(BT_BUGS, i, cooked_strs[1], j, cooked_strs[2], cooked_strs[3], pc);
burp socials ~
burp(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
butcher ~
return butcher(i, &(cooked_strs[1]), pc);
buy ~
return buy(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
body_slam bodyslam bodyslam ~
return body_slam(i, &(cooked_strs[1]), pc, was_ordered);
backstab bs ~
return backstab(i, &(cooked_strs[1]), pc);
# IMM Commands - B

# Mortal Commands - C
cast ~
return cast(&(cooked_strs[1]), j, &(cooked_strs[2]), pc, was_ordered);
cackle socials ~
cackle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
cheer socials ~
cheer(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
chomp socials ~
chomp(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
chuckle socials ~
chuckle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
circle ~
return circle(i, &(cooked_strs[1]), pc);
clap socials ~
clap(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
claw ~
return claw(i, &(cooked_strs[1]), pc);
client ~
client_command( &(cooked_strs[1]), pc); return 0;
climb socials ~
climb(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
close ~
return close(i, &(cooked_strs[1]), pc);
color ansi color ~
return color((cooked_strs[1]), (cooked_strs[2]), pc);
consider ~
return consider(i, &(cooked_strs[1]), pc);
concoct brew concoct ~
return concoct(pc, !was_ordered);
construct ~
return construct(pc, !was_ordered);
cough socials ~
cough(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
cower socials ~
cower(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
curse ~
curse(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
curtsey socials ~
curtsey(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - C
chance ~
return (chance( i, j, k));
ch_ddesc ~
return ch_ddesc(i, pc);
ch_kdesc ~
return ch_kdesc(i, pc);
ch_mdesc ~
return ch_mdesc(i, pc);
ch_mndesc ~
return ch_mndesc(i, &(cooked_strs[1]), pc);
ch_msdesc ~
return ch_msdesc(i, &(cooked_strs[1]), pc);
ch_odesc ~
return ch_odesc(i, pc);
ch_ondesc ~
return ch_ondesc(i, &(cooked_strs[1]), pc);
ch_osdesc ~
return ch_osdesc(i, &(cooked_strs[1]), pc);
ch_path_desc ~
return ch_path_desc(i, j, pc);
ch_rdesc ~
return ch_rdesc(pc);
ch_rname ~
return ch_rname(&(cooked_strs[1]), pc);
ch_zname ~
return ch_zname(i, &(cooked_strs[1]), pc);
clear_keyword ~
return clear_keyword(i, pc);
clear_mnames ~
return clear_mnames(i, pc);
clear_onames ~
return clear_onames(i, pc);
create_concoction create_conct ~
return create_concoction(i, j, k, l, m, n, pc);
create_construction create_constrct ~
return create_construction(i, j, k, l, m, n, pc);

# Mortal Commands - D
down ~
return down(pc, *(pc.getCurRoom()), is_dead);
dance socials ~
dance(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
date ~
return date(pc);
deposit ~
return deposit(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
describe ~
return describe(pc);
disarm ~
return disarm(i, &(cooked_strs[1]), pc);
disco socials ~
disco(j, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
discuss ~
return discuss(&(cooked_strs[1]), j, &(cooked_strs[2]), pc);
disdain socials ~
disdain(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
donate ~
return donate(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
doze socials ~
doze(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
drink ~
return drink(i, &(cooked_strs[1]), pc);
drop ~
return drop(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
# IMM Commands - D
dclear ~
return dclear(i, pc);
delete_door ~
return delete_door(i, &(cooked_strs[1]), pc);
dlist ~
return dlist(i, j, pc);
does_own comparators ~
return does_own(pc, i, j, k, l, m, n);
door_to ~
return door_to(i, j, &(cooked_strs[1]), pc);
dset ~
return dset(i, &(cooked_strs[1]), &(cooked_strs[2]), k, pc);
dstat ~
return dstat(i, &(cooked_strs[1]), pc);
duck socials ~
duck(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
dvnum ~
return dvnum(i, &(cooked_strs[1]), pc);
dsys ~
return dsys(i, cooked_strs[1], cooked_strs[2], pc);

# Mortal Commands - E
east ~
return east(pc, *(pc.getCurRoom()), is_dead);
earthmeld ~
return earthmeld(pc);
eat ~
return eat(i, &(cooked_strs[1]), pc);
emote *emote ~
NOP
empty ~
return empty(i, &(cooked_strs[1]), pc);
equipment eq ~
return show_eq(pc);
examine ~
return examine(i, &(cooked_strs[1]), pc);
eyebrow socials ~
eyebrow(j, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - E
enslave ~
return enslave(i, &(cooked_strs[1]), pc);
exact_damage ~
return exact_damage(i, j, cooked_strs[1], pc, was_ordered);

# Mortal Commands - F
fill ~
return fill(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
flee ~
return flee(pc, is_dead);
flex socials ~
flex(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
follow ~
return follow(i, &(cooked_strs[1]), pc);
# IMM Commands - F
flip_door ~
return flip_door(i, &(cooked_strs[1]), pc);
force *force ~
NOP
force_all *force_all ~
NOP
freeze ~
return freeze(i, &(cooked_strs[1]), pc);

# Mortal Commands - G
gemote *emote ~
NOP
get take get ~
return get(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
giggle socials ~
giggle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
give ~
return give(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
go enter exit go ~
is_dead = FALSE; return go(i, &(cooked_strs[1]), pc, is_dead);
# exits is here so that "exit" can be part of "go" (i.e. vehicles)
exits ~
return exit(pc);
goo socials ~
goo(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
gossip *channels ~
NOP
group ~
return group(i, &(cooked_strs[1]), pc);
greet socials ~
greet(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
grin socials ~
grin(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
groan socials ~
groan(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
growl socials ~
growl(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
grunt socials ~
grunt(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
grumble socials ~
grumble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
gs *channels ~
NOP
gt *channels ~
NOP
guard ~
return guard(i, &(cooked_strs[1]), pc);
# IMM Commands - G
gag ~
return gag(i, &(cooked_strs[1]), pc);
gain_exp ~
return scr_gain_exp(i, pc, was_ordered);
goto cd goto ~
return _goto(i, &(cooked_strs[1]), pc);

# Mortal Commands - H
hehe socials ~
hehe(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
help man help ~
return help(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
hide ~
return hide(pc);
hit kill attack murder hit ~
return hit(i, &(cooked_strs[1]), pc);
hmm socials ~
hmm(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hold grab hold ~
return hold(i, &(cooked_strs[1]), pc);
hop socials ~
hop(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hug socials ~
hug(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hum socials ~
hum(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hurl ~
return hurl(i, &(cooked_strs[1]), pc, was_ordered);
# IMM Commands - H

# Mortal Commands - I
inventory ~
return inventory(pc);
idea *idea ~
NOP
idealist buglist ~
return buglist(BT_IDEAS, i, cooked_strs[1], j, cooked_strs[2], cooked_strs[3], pc);
insane socials ~
insane(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
itch socials ~
itch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - I
is_equal_to comparators ~
return is_equal_to(i, cooked_strs[1], cooked_strs[2], pc);
is_greater_than comparators ~
return is_greater_than(i, cooked_strs[1], cooked_strs[2], pc);
is_in_posn comparators ~
return is_in_posn(cooked_strs[1], pc);
is_less_than comparators ~
return is_less_than(i, cooked_strs[1], cooked_strs[2], pc);
itrans ~
return itrans(i, &(cooked_strs[1]), j, pc);

# Mortal Commands - J
jump socials ~
social_jump(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
junk ~
return junk(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
# IMM Commands - J

# Mortal Commands - K
kick ~
return kick(i, &(cooked_strs[1]), pc);
kiss socials ~
kiss(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - K

# Mortal Commands - L
look ls look ~
return look(i, &(cooked_strs[1]), pc, TRUE); /* ignore brief */
lag socials ~
lag(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
language languages ~
return language(cooked_strs[1], pc);
laugh lol socials ~
laugh(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
lick socials ~
lick(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
light wear ~
return light(i, &(cooked_strs[1]), pc);
list ~
return list_merchandise(i, &(cooked_strs[1]), pc);
listen socials ~
listen(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
lock ~
return lock(i, &(cooked_strs[1]), pc);
lore ~
return lore(i, &(cooked_strs[1]), pc);
loser socials ~
loser(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - L
ldwho ~
return ldwho(pc);
list_obj_scripts ~
return list_obj_scripts(i, pc);
list_paths ~
return list_paths(i, pc);
list_scripts ~
return list_scripts(i, pc);
list_room_scripts ~
return list_room_scripts(i, pc);
list_site_bans ~
return list_site_bans(pc);
list_zones ~
return showZones(pc);
log_level ~
return log_level(i, pc);

# Mortal Commands - M
meditate ~
return meditate(pc);
megawibble socials ~
megawibble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
move *move ~
NOP
muahaha socials ~
muahaha(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
mutter socials ~
mutter(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - M
make_builder ~
return make_builder(i, &(cooked_strs[1]), pc);
make_maps ~
return make_maps(pc);
make_pso ~
return make_pso(i, &(cooked_strs[1]), pc);
mclear ~
return mclear(i, pc);
mclone ~
return mclone(i, &(cooked_strs[1]), pc);
mia_mobs ~
return mia_mobs(i, pc);
mia_objects ~
return mia_objects(i, pc);
mldsearch ~
return mldsearch(&(cooked_strs[1]), pc);
mlist ~
return  mlist(i, j, pc);
mload ~
return mload(i, &(cooked_strs[1]), pc);
msdsearch ~
return msdsearch(&(cooked_strs[1]), pc);
mset ~
return mset(i, &(cooked_strs[1]), &(cooked_strs[2]), k, &(cooked_strs[3]), pc);
mstat ~
return mstat(i, &(cooked_strs[1]), pc);
mreload ~
return mreload(pc);
mvnum ~
return mvnum(i, &(cooked_strs[1]), pc);

# Mortal Commands - N
north ~
return north(pc, *(pc.getCurRoom()), is_dead);
northeast ne northeast ~
return northeast(pc, *(pc.getCurRoom()), is_dead);
northwest nw northwest ~
return northwest(pc, *(pc.getCurRoom()), is_dead);
nestle socials ~
nestle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
nibble socials ~
nibble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
nod socials ~
nod(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
nog socials ~
nog(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
nogossip ~
return nogossip(pc);
# IMM Commands - N
neighbor_echo other_room_echo rm_comm *rm_comm ~
NOP

# Mortal Commands - O
offer ~
return offer(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
open ~
return open(i, &(cooked_strs[1]), pc);
order *order ~
NOP
osay *osay ~
NOP
# IMM Commands - O
oclear ~
return oclear(i, pc);
oclone ~
return oclone(i, &(cooked_strs[1]), pc);
olc ~
return handle_olc(pc, do_sub);
oldsearch ~
return oldsearch(&(cooked_strs[1]), pc);
olist ~
return olist(i, j, pc);
oload ~
return oload(i, &(cooked_strs[1]), pc);
opurge ~
return opurge(i, &(cooked_strs[1]), pc);
osdsearch ~
return osdsearch(&(cooked_strs[1]), pc);
ostat ~
return ostat(i, &(cooked_strs[1]), pc);
oreload ~
return oreload(pc);
oset ~
return oset(i, &(cooked_strs[1]), &(cooked_strs[2]), k, &(cooked_strs[3]), pc);
ovnum ~
return ovnum(i, &(cooked_strs[1]), pc);

# Mortal Commands - P
page_break ~
return page_break(i, pc);
panic socials ~
panic(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
passwd ~
return passwd(&(cooked_strs[1]), &(cooked_strs[2]), &(cooked_strs[3]), pc);
pat socials ~
pat(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
pay ~
return pay(i, j, &(cooked_strs[1]), pc);
peck socials ~
peck(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
pemote *pemote ~
NOP
pet socials ~
pet(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
pft socials ~
pft(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
picklock ~
return picklock(i, &(cooked_strs[1]), pc);
pinch socials ~
pinch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
point socials ~
point(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
poke socials ~
poke(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
ponder socials ~
ponder(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
post ~
return post(&(cooked_strs[1]), pc);
pout socials ~
pout(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
practice ~
return practice(&(cooked_strs[1]), j, &(cooked_strs[2]), pc);
prone ~
return prone(pc);
prompt ~
return prompt(&(cooked_strs[1]), pc);
puke socials ~
puke(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
put ~
return put(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
punt socials ~
punt(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - P
pause add_mob_script ~
return pause(i, pc, was_ordered);
pecho *add_mob_script ~
NOP
poofin *poofin ~
NOP
poofout *poofout ~
NOP
possess ~
return possess(i, &(cooked_strs[1]), pc);

# Mortal Commands - Q
quaff ~
return quaff(i, &(cooked_strs[1]), pc);
quit logout log_out quit ~
return log_out(pc);
# IMM Commands - Q

# Mortal Commands - R
read ~
return read(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
recite ~
return recite(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
remove ~
return remove(i, &(cooked_strs[1]), pc);
reply *reply ~
NOP
rescue ~
return rescue(i, &(cooked_strs[1]), pc);
rest ~
return rest(pc);
rofl socials ~
rofl(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
roll ~
if ((i != 1) || (j != 1)) roll(i, j, pc); else roll(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - R
rclear ~
return rclear(i, pc);
rclone ~
return rclone(i, &(cooked_strs[1]), j, pc);
rlist ~
return rlist(i, j, pc);
recho gecho zecho *gecho ~
NOP
reset_olc ~
return reset_olc(pc);
remort ~
return remort(i, &(cooked_strs[1]), &(cooked_strs[2]), &(cooked_strs[3]), pc);
rem_bow_proc rm_bow_proc rm_bow_proc ~
return rm_bow_proc(i, pc);
rem_casts_spell rm_casts_spell rm_casts_spell ~
return rm_casts_spell(i, j, pc);
rem_curse_proc rm_curse_proc rm_curse_proc ~
return rm_curse_proc(i, pc);
rem_discuss_proc rm_discuss_proc rm_discuss_proc ~
return rm_discuss_proc(i, pc);
rem_give_proc rm_give_proc rm_give_proc ~
return rm_give_proc(i, pc);
rem_keyword rm_keyword rm_keyword ~
return rm_keyword(i, pc);
rem_name rm_name rem_name ~
return rem_mname(i, &(cooked_strs[1]), pc);
rem_obj_script rm_obj_script rm_obj_script ~
return rem_obj_script(i, cooked_strs[1], j, pc);
rem_oname rm_oname rem_oname ~
return rem_oname(i, &(cooked_strs[1]), pc);
rem_path rm_path rem_path ~
return rem_path(i, j, pc);
rem_perm_inv rm_perm_inv rem_perm_inv ~
return rem_perm_inv(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
rem_room_script rm_room_script rm_room_script ~
return rem_room_script(i, cooked_strs[1], j, pc);
rem_script rm_script rm_script ~
return rem_script(i, (cooked_strs[1]), j, pc);
rem_stat_affect rm_stat_affect rm_stat_affect ~
return rm_stat_affect(i, j, pc);
rem_zone rm_zone rem_zone ~
return remZone(i, pc);
replace_door ~
return replace_door(i, &(cooked_strs[1]), pc);
rezone ~
return rezone(pc);
rinit ~
return rinit(i, j, pc);
rm_move rm_transport *rm_move ~
NOP
rm_move_all rm_transport_all *rm_move_all ~
NOP
rm_omove rm_otransport *rm_omove ~
NOP
rm_omove_all rm_otransport_all *rm_omove_all ~
NOP
rset ~
return rset(&(cooked_strs[1]), j, pc);
rstat ~
return rstat(i, pc);

# Mortal Commands - S
south ~
return south(pc, *(pc.getCurRoom()), is_dead);
southeast se southeast ~
return southeast(pc, *(pc.getCurRoom()), is_dead);
southwest sw southwest ~
return southwest(pc, *(pc.getCurRoom()), is_dead);
sacrifice ~
return sacrifice(&(cooked_strs[1]), pc);
save ~
return ::save(pc);
say talk *say ~
NOP
scan ~
return scan(pc);
score ~
return score(&(cooked_strs[1]), pc);
scold socials ~
scold(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
scratch socials ~
scratch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
scream socials ~
scream(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
scribe ~
return scribe(&(cooked_strs[1]), pc);
sell ~
return sell(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
shake socials ~
shake(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
shield ~
return shield(i, &(cooked_strs[1]), pc, was_ordered);
shock socials ~
shock(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
shoot ~
return shoot(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
shout *shout ~
NOP
shrug socials ~
shrug(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
shudder socials ~
shudder(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
sigh socials ~
sigh(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
silly socials ~
silly(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
sit ~
return sit(pc);
skin ~
return skin(i, &(cooked_strs[1]), pc);
slap socials ~
slap(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
sleep ~
return sleep(pc);
smile socials ~
smile(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
smirk socials ~
smirk(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
smooch socials ~
smooch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
snap socials ~
snap(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
sneak ~
return sneak(pc);
snicker socials ~
snicker(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
sob socials ~
sob(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
splash socials ~
splash(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
split ~
return cmd_split(i, pc);
stand ~
return stand(pc);
stare socials ~
stare(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
steal ~
return steal(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
strut socials ~
strut(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
suicide ~
return suicide(&(cooked_strs[1]), pc);
sweatdrop socials ~
sweatdrop(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
swoon socials ~
swoon(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - S
save_mob ~
return save_mob(i, &(cooked_strs[1]), pc);
save_obj ~
return save_obj(i, &(cooked_strs[1]), pc);
script_jump_false add_mob_script ~
return script_jump_false(cooked_strs, cooked_ints, pc, c_script_owner, r_script_owner, sanity);
script_jump_true add_mob_proc ~
return script_jump_true(cooked_strs, cooked_ints, pc, c_script_owner, r_script_owner, sanity);
see_all ~
return see_all(pc);
self *possess ~
NOP
set_path_dir ~
return set_path_dir(i, j, k, &(cooked_strs[1]), pc);
set_path_pointer ~
return set_path_pointer(i, j, pc);
set_veh_croom ~
return set_veh_croom(i, j, pc);
set_veh_stop ~ 
return  set_veh_stop(i, j, &(cooked_strs[1]), pc);
set_zflag ~
return set_zflag(i, &(cooked_strs[1]), pc);
show_zones ~
return showZones(pc);
shutdown ~
return shutdown(&(cooked_strs[1]), pc);
silent_junk add_mob_script ~
return silent_junk(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
siteban ~
return siteban(&(cooked_strs[1]), pc);
slay ~
return slay(i, &(cooked_strs[1]), pc);
slist ~
return slist(i, cooked_strs[1], j, pc);
snoop ~
return snoop(i, &(cooked_strs[1]), pc);
sockets ~
return sockets(pc);
stat_script ~
return stat_script(i, j, pc);
stat_keyword ~
return stat_keyword(i, pc);
stat_obj_script ~
return stat_obj_script(i, j, pc);
stat_path ~
return stat_path(i, j, pc);
stat_room_script ~
return stat_room_script(i, j, pc);

# Mortal Commands - T
tail ~
return tail(i, &(cooked_strs[1]), pc);
tango socials ~
tango(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
tap socials ~
tap(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
test ~
test(pc); return 0;
thank socials ~
thank(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
thpppt socials ~
thpppt(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
throw ~
return _throw(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
tickle socials ~
tickle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
time ~
return pc.showTime();
title *title ~
NOP
toggle ~
return toggle_prompt(&(cooked_strs[1]), pc);
track ~
return track(i, &(cooked_strs[1]), pc);
tremble socials ~
tremble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
trip ~
return trip(i, &(cooked_strs[1]), pc);
trout socials ~
trout(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
twiddle socials ~
twiddle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
twirl socials ~
twirl(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - T
teach ~
return teach(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
thaw ~
return thaw(i, &(cooked_strs[1]), pc);
tog_mflag ~
return tog_mflag(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
tog_oflag ~
return tog_oflag(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
tog_rflag ~
return tog_rflag(i, pc);
tog_veh_stop ~
return tog_veh_stop(i, j, pc);
tog_vflag ~
return tog_vflag(i, pc);
total_rezone ~
return total_rezone(pc);

# Mortal Commands - U
up ~
return up(pc, *(pc.getCurRoom()), is_dead); 
ungroup ~
return pc.doUngroup(i, &(cooked_strs[1])); 
unguard ~
return unguard(pc);
unlock ~
return unlock(i, &(cooked_strs[1]), pc); 
unwield remove ~
return remove(i, &(cooked_strs[1]), pc);
unpost ~
return unpost(i, &(cooked_strs[1]), pc); 
use ~
return use(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc); 
using_client ~
return using_client(pc); 
# IMM Commands - U
ungag ~
return ungag(i, &(cooked_strs[1]), pc); 
unsiteban ~
return unsiteban(&(cooked_strs[1]), pc); 
unsnoop ~
return unsnoop(pc); 
unpossess *unpossess ~
NOP
update_cig update_cur_in_game update_cig ~
return update_cur_in_game(pc); 
uptime ~
return uptime(pc); 

# Mortal Commands - V
value_add ~
return value_add(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
value_list ~
return value_list(i, &(cooked_strs[1]), pc);
value_rem ~
return value_rem(i, &(cooked_strs[1]), j, pc);
value_set ~
return value_set(i, &(cooked_strs[1]), j, k, l, pc);
# IMM Commands - V
verifydoors ~
return verifydoors(i, pc);
visible ~
return visible(pc);


# Mortal Commands - W
west ~
return west(pc, *(pc.getCurRoom()), is_dead);
wake ~
return wake(pc);
wave socials ~
wave(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wear ~
return wear(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
weep socials ~
weep(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
where ~
return where(i, &(cooked_strs[1]), pc);
whistle socials ~
whistle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
whimper socials ~
whimper(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
whine socials ~
whine(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
who ~
return who(pc);
whip socials ~
whip(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wibble socials ~
wibble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wield ~
return wield(i, &(cooked_strs[1]), pc);
wiggle socials ~
wiggle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wiggletoes socials ~
wiggletoes(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wimpy ~
return wimpy(i, pc);
wink socials ~
wink(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
withdraw ~
return withdraw(i, cooked_strs[1], j, cooked_strs[2], pc);
# IMM Commands - W
wc *channels ~
NOP
wizchat *channels ~
NOP
wizinvis ~
return wizinvis(pc);
write_world ~
return write_world(pc);
write_zone asave write_zone ~
return write_zone(i, pc);

# Mortal Commands - X
# IMM Commands - X

# Mortal Commands - Y
yawn socials ~
yawn(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
yell *yell ~
NOP
yuck socials ~
yuck(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# IMM Commands - Y

# Mortal Commands - Z
# IMM Commands - Z
zcreate ~
return zcreate(i, &(cooked_strs[1]), j, pc); 
zenable ~
return zenable(i, &(cooked_strs[1]), pc); 
zdisable ~
return zdisable(i, &(cooked_strs[1]), pc); 
zgoto ~
return zgoto(i, pc); 
zlist ~
return zlist(i, j, pc); 
zlock ~
return zlock(i, pc); 
zunlock ~
return zunlock(i, pc); 

# Help - A
auction yell *channels ~
NOP
anitre *anitre ~
NOP
avintre *avintre ~
NOP
# Help - B
bug typo *bug ~
NOP
# Help - C
channels tell ask gt gc *channels ~
NOP
classes warrior sage wizard ranger thief alchemist cleric bard *classes ~
NOP
commands *commands ~
NOP
comparators *comparators ~
NOP
credits *credits ~
NOP
# Help - D
darklings *darkling ~
NOP
deals *deals ~
NOP
derelicts *derelicts ~
NOP
dragons *dragon ~
NOP
drow *drow ~
NOP
dwarves dwarf *dwarf ~
NOP
# Help - E
elf *elf ~
NOP
elves *elves ~
NOP
entity *entity ~
NOP
# Help - F
faerie *faerie ~
NOP
# Help - G
# Help - H
houses *houses ~
NOP
humans *human ~
NOP
# Help - I
immrules *immrules ~
NOP
ironclads *ironclad ~
NOP
# Help - J
# Help - K
keywords *keyword ~
NOP
# Help - L
label_dummy label: add_mob_script ~
return -1; //should never get here
# Help - N
# Help - O
ogrue *ogrue ~
NOP
# Help - P
# Help - Q
# Help - R
races *races ~
NOP
regeneration *regeneration ~
NOP
rocktrolls *rocktroll ~
NOP
rules *rules ~
NOP
# Help - S
script_cmds push pull *script_cmds ~
NOP
smob *smob ~
NOP
shops *shops ~
NOP
skills *skills ~
NOP
socials *socials ~
NOP
sombre sombri *sombre ~
NOP
# Help - T
theme *theme ~
NOP
topics *topics ~
NOP
# Help - U
# Help - V
# Help - W
wizhelp *wizhelp ~
NOP
# Help - X
# Help - Y
# Help - Z
zones *zones ~
NOP
~

