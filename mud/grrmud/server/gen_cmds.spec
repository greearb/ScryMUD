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
adjust_register ~
return adjust_register(i, &(cooked_strs[1]), j, pc);
alias ~
return add_crit_alias(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
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
alist zlist zlist ~
return zlist(i, j, pc); 
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
# Socials - A
ack socials ~
ack(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
afk socials ~
afk(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
agree socials ~
agree(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
announce socials ~
announce(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - A
auction yell *channels ~
NOP
anitre *anitre ~
NOP
avintre *avintre ~
NOP

# Mortal Commands - B
backstab bs ~
return backstab(i, &(cooked_strs[1]), pc);
balance ~
return balance(i, &(cooked_strs[1]), pc);
bash ~
return bash(i, &cooked_strs[1], pc);
beckon socials ~
beckon(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
beep ~
return beep(i, &(cooked_strs[1]), pc);
berserk ~
return berserk(pc);
blend ~
return blend(pc);
block ~
return block(i, &(cooked_strs[1]), pc);
body_slam bodyslam bodyslam ~
return body_slam(i, &(cooked_strs[1]), pc, was_ordered);
brief ~
return brief(pc);
buglist ~
return buglist(BT_BUGS, i, cooked_strs[1], j, cooked_strs[2], cooked_strs[3], pc);
butcher ~
return butcher(i, &(cooked_strs[1]), pc);
buy ~
return buy(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
# IMM Commands - B
# Socials - B
bah socials ~
bah(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
battlecry socials ~
battlecry(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
bite socials ~
bite(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
blush socials ~
blush(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
bonk socials ~
bonk(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
bounce socials ~
bounce(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
bow socials ~
bow(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
burp socials ~
burp(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - B
bug typo *bug ~
NOP

# Mortal Commands - C
cast ~
return cast(&(cooked_strs[1]), j, &(cooked_strs[2]), pc, was_ordered);
circle ~
return circle(i, &(cooked_strs[1]), pc);
claw ~
return claw(i, &(cooked_strs[1]), pc);
client ~
client_command( &(cooked_strs[1]), pc); return 0;
close ~
return close(i, &(cooked_strs[1]), pc);
color ansi color ~
return color((cooked_strs[1]), (cooked_strs[2]), pc);
concoct brew concoct ~
return concoct(pc, !was_ordered);
consider ~
return consider(i, &(cooked_strs[1]), pc);
construct ~
return construct(pc, !was_ordered);
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
# Socials - C
cackle socials ~
cackle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
cheer socials ~
cheer(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
chomp socials ~
chomp(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
chuckle socials ~
chuckle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
clap socials ~
clap(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
climb socials ~
climb(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
cough socials ~
cough(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
cower socials ~
cower(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
curse ~
curse(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
curtsey socials ~
curtsey(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
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

# Mortal Commands - D
down ~
return down(pc, *(pc.getCurRoom()), is_dead);
date ~
return date(pc);
deposit ~
return deposit(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
describe ~
return describe(pc);
disarm ~
return disarm(i, &(cooked_strs[1]), pc);
discuss ~
return discuss(&(cooked_strs[1]), j, &(cooked_strs[2]), pc);
donate ~
return donate(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
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
dvnum ~
return dvnum(i, &(cooked_strs[1]), pc);
dsys ~
return dsys(i, cooked_strs[1], cooked_strs[2], pc);
# Socials - D
dance socials ~
dance(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
disco socials ~
disco(j, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
disdain socials ~
disdain(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
doze socials ~
doze(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
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
exits ~
return exit(pc);
# IMM Commands - E
enslave ~
return enslave(i, &(cooked_strs[1]), pc);
exact_damage ~
return exact_damage(i, j, cooked_strs[1], pc, was_ordered);
# Socials - E
# Help - E
elf *elf ~
NOP
elves *elves ~
NOP
entity *entity ~
NOP

# Mortal Commands - F
fill ~
return fill(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
flee ~
return flee(pc, is_dead);
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
# Socials - F
flex socials ~
flex(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - F
faerie *faerie ~
NOP

# Mortal Commands - G
gemote *emote ~
NOP
get take get ~
return get(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
give ~
return give(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
go enter go ~
is_dead = FALSE; return go(i, &(cooked_strs[1]), pc, is_dead);
gossip *channels ~
NOP
group ~
return group(i, &(cooked_strs[1]), pc);
gs *channels ~
NOP
gt *channels ~
NOP
guard ~
return guard(i, &(cooked_strs[1]), pc);
# IMM Commands - G
gag ~
return gag(i, &(cooked_strs[1]), pc);
goto ~
return _goto(i, &(cooked_strs[1]), pc);
# Socials - G
giggle socials ~
giggle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
goo socials ~
goo(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
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
# Help - G

# Mortal Commands - H
help man help ~
return help(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
hide ~
return hide(pc);
hit kill attack murder hit ~
return hit(i, &(cooked_strs[1]), pc);
hold grab hold ~
return hold(i, &(cooked_strs[1]), pc);
hurl ~
return hurl(i, &(cooked_strs[1]), pc, was_ordered);
# IMM Commands - H
# Socials - H
hehe socials ~
hehe(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hmm socials ~
hmm(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hop socials ~
hop(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hug socials ~
hug(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hum socials ~
hum(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - H
houses *houses ~
NOP
humans *human ~
NOP


# Mortal Commands - I
inventory ~
return inventory(pc);
idea *idea ~
NOP
idealist buglist ~
return buglist(BT_IDEAS, i, cooked_strs[1], j, cooked_strs[2], cooked_strs[3], pc);
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
# Socials - I
insane socials ~
insane(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
itch socials ~
itch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - I
immrules *immrules ~
NOP
ironclads *ironclad ~
NOP

# Mortal Commands - J
junk ~
return junk(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
# IMM Commands - J
# Socials - J
jump socials ~
social_jump(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - J

# Mortal Commands - K
kick ~
return kick(i, &(cooked_strs[1]), pc);
# IMM Commands - K
# Socials - K
kiss socials ~
kiss(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - K
keywords *keyword ~
NOP

# Mortal Commands - L
look ls look ~
return look(i, &(cooked_strs[1]), pc, TRUE); /* ignore brief */
language languages ~
return language(cooked_strs[1], pc);
light wear ~
return light(i, &(cooked_strs[1]), pc);
list ~
return list_merchandise(i, &(cooked_strs[1]), pc);
lock ~
return lock(i, &(cooked_strs[1]), pc);
lore ~
return lore(i, &(cooked_strs[1]), pc);
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
# Socials - L
lag socials ~
lag(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
laugh lol socials ~
laugh(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
lick socials ~
lick(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
listen socials ~
listen(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
loser socials ~
loser(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - L
label_dummy label: add_mob_script ~
return -1; //should never get here

# Mortal Commands - M
meditate ~
return meditate(pc);
move *move ~
NOP
# IMM Commands - M
make_builder ~
return make_builder(i, &(cooked_strs[1]), pc);
make_pso ~
return make_pso(i, &(cooked_strs[1]), pc);
mclear ~
return mclear(i, pc);
mclone ~
return mclone(i, &(cooked_strs[1]), pc);
mlist ~
return  mlist(i, j, pc);
mload ~
return mload(i, &(cooked_strs[1]), pc);
mset ~
return mset(i, &(cooked_strs[1]), &(cooked_strs[2]), k, &(cooked_strs[3]), pc);
mstat ~
return mstat(i, &(cooked_strs[1]), pc);
mreload ~
return mreload(pc);
mvnum ~
return mvnum(i, &(cooked_strs[1]), pc);
# Socials - M
megawibble socials ~
megawibble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
muahaha socials ~
muahaha(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
mutter socials ~
mutter(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - M

# Mortal Commands - N
north ~
return north(pc, *(pc.getCurRoom()), is_dead);
northeast ne northeast ~
return northeast(pc, *(pc.getCurRoom()), is_dead);
northwest nw northwest ~
return northwest(pc, *(pc.getCurRoom()), is_dead);
nogossip ~
return nogossip(pc);
# IMM Commands - N
neighbor_echo other_room_echo rm_comm *rm_comm ~
NOP
# Socials - N
nestle socials ~
nestle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
nibble socials ~
nibble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
nod socials ~
nod(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
nog socials ~
nog(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - N

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
olist ~
return olist(i, j, pc);
oload ~
return oload(i, &(cooked_strs[1]), pc);
opurge ~
return opurge(i, &(cooked_strs[1]), pc);
ostat ~
return ostat(i, &(cooked_strs[1]), pc);
oreload ~
return oreload(pc);
oset ~
return oset(i, &(cooked_strs[1]), &(cooked_strs[2]), k, &(cooked_strs[3]), pc);
ovnum ~
return ovnum(i, &(cooked_strs[1]), pc);
# Socials - O
# Help - O
ogrue *ogrue ~
NOP

# Mortal Commands - P
page_break ~
return page_break(i, pc);
passwd ~
return passwd(&(cooked_strs[1]), &(cooked_strs[2]), &(cooked_strs[3]), pc);
pay ~
return pay(i, j, &(cooked_strs[1]), pc);
pemote *pemote ~
NOP
picklock ~
return picklock(i, &(cooked_strs[1]), pc);
post ~
return post(&(cooked_strs[1]), pc);
practice ~
return practice(&(cooked_strs[1]), j, &(cooked_strs[2]), pc);
prone ~
return prone(pc);
prompt ~
return prompt(&(cooked_strs[1]), pc);
put ~
return put(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
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
# Socials - P
panic socials ~
panic(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
pat socials ~
pat(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
peck socials ~
peck(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
pft socials ~
pft(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
pinch socials ~
pinch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
point socials ~
point(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
poke socials ~
poke(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
ponder socials ~
ponder(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
pout socials ~
pout(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
puke socials ~
puke(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
punt socials ~
punt(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - P

# Mortal Commands - Q
quaff ~
return quaff(i, &(cooked_strs[1]), pc);
quit logout log_out quit ~
return log_out(pc);
# IMM Commands - Q
# Socials - Q
# Help - Q

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
# Socials - R
rofl socials ~
rofl(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
roll ~
if ((i != 1) || (j != 1)) roll(i, j, pc); else roll(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - R
races *races ~
NOP
regeneration *regeneration ~
NOP
rocktrolls *rocktroll ~
NOP
rules *rules ~
NOP

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
scribe ~
return scribe(&(cooked_strs[1]), pc);
sell ~
return sell(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
shield ~
return shield(i, &(cooked_strs[1]), pc, was_ordered);
shoot ~
return shoot(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
shout *shout ~
NOP
sit ~
return sit(pc);
skin ~
return skin(i, &(cooked_strs[1]), pc);
sleep ~
return sleep(pc);
sneak ~
return sneak(pc);
split ~
return cmd_split(i, pc);
stand ~
return stand(pc);
steal ~
return steal(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
suicide ~
return suicide(&(cooked_strs[1]), pc);
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
set_ven_stop ~
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
# Socials - S
scold socials ~
scold(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
scratch socials ~
scratch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
scream socials ~
scream(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
shake socials ~
shake(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
shock socials ~
shock(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
shrug socials ~
shrug(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
shudder socials ~
shudder(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
sigh socials ~
sigh(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
silly socials ~
silly(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
slap socials ~
slap(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
smile socials ~
smile(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
smirk socials ~
smirk(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
smooch socials ~
smooch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
snap socials ~
snap(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
snicker socials ~
snicker(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
sob socials ~
sob(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
splash socials ~
splash(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
stare socials ~
stare(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
strut socials ~
strut(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
sweatdrop socials ~
sweatdrop(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
swoon socials ~
swoon(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
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

# Mortal Commands - T
tail ~
return tail(i, &(cooked_strs[1]), pc);
throw ~
return _throw(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
time ~
return pc.showTime();
title *title ~
NOP
toggle ~
return toggle_prompt(&(cooked_strs[1]), pc);
track ~
return track(i, &(cooked_strs[1]), pc);
trip ~
return trip(i, &(cooked_strs[1]), pc);
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
# Socials - T
tango socials ~
tango(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
thank socials ~
thank(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
thpppt socials ~
thpppt(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
tickle socials ~
tickle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
tremble socials ~
tremble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
trout socials ~
trout(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
twiddle socials ~
twiddle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
twirl socials ~
twirl(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - T
theme *theme ~
NOP
topics *topics ~
NOP

# Mortal Commands - U
up ~
return up(pc, *(pc.getCurRoom()), is_dead); 
ungroup ~
return pc.doUngroup(i, &(cooked_strs[1])); 
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
# Socials - U
# Help - U

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
visible ~
return visible(pc);
# Socials - V
# Help - V


# Mortal Commands - W
west ~
return west(pc, *(pc.getCurRoom()), is_dead);
wake ~
return wake(pc);
wear ~
return wear(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
where ~
return where(i, &(cooked_strs[1]), pc);
who ~
return who(pc);
wield ~
return wield(i, &(cooked_strs[1]), pc);
wimpy ~
return wimpy(i, pc);
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
# Socials - W
wave socials ~
wave(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
weep socials ~
weep(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
whistle socials ~
whistle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
whine socials ~
whine(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
whip socials ~
whip(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wibble socials ~
wibble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wiggle socials ~
wiggle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wiggletoes socials ~
wiggletoes(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wink socials ~
wink(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - W
wizhelp *wizhelp ~
NOP

# Mortal Commands - X
# IMM Commands - X
# Socials - X
# Help - X

# Mortal Commands - Y
yell *yell ~
NOP
# IMM Commands - Y
# Socials - Y
yawn socials ~
yawn(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
yuck socials ~
yuck(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
# Help - Y

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
# Socials - Z
# Help - Y
zones *zones ~
NOP


~
