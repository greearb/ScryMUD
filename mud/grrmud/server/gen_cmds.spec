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


# case A
abilities ~
return abilities(pc);
ack socials ~
ack(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
add_kname ~
return add_kname(i, &(cooked_strs[1]), pc);
adjust_register ~
return adjust_register(i, &(cooked_strs[1]), j, pc);
add_keyword ~
return add_keyword(pc);
add_mname ~
return add_mname(i, &(cooked_strs[1]), pc);
add_mob_script ~
return add_mob_script(pc, i, (cooked_strs[1]), j, (cooked_strs[2]), k, l);
add_obj_script add_mob_script ~
return add_obj_script(pc, i, (cooked_strs[1]), j, (cooked_strs[2]), k, l);
add_room_script add_mob_script ~
return add_room_script(pc, i, (cooked_strs[1]), j, (cooked_strs[2]), k, l);
add_oname ~
return add_oname(i, &(cooked_strs[1]), pc);
add_stat_affect ~
return add_stat_affect(i, j, k, pc);
add_casts_spell ~
return add_casts_spell(i, j, k, pc);
add_zone ~
return addZone(i, pc);
adlist ~
return adlist(pc);
affect_hp ~
return affect_crit_stat(STAT_HP, cooked_strs[2], i, &(cooked_strs[1]), j, k, pc.getCurRoom(), &pc);
affect_mana ~
return affect_crit_stat(STAT_MANA, cooked_strs[2], i, &(cooked_strs[1]), j, k, pc.getCurRoom(), &pc);
affect_mov ~
return affect_crit_stat(STAT_MOV, cooked_strs[2], i, &(cooked_strs[1]), j, k, pc.getCurRoom(), &pc);
afk socials ~
afk(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
agree socials ~
agree(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
add_perm_inv ~
return add_perm_inv(i, &(cooked_strs[1]), j, pc);
assist ~
return assist(i, &(cooked_strs[1]), pc);
asave ~
return write_zone(i, pc);
alist zlist zlist ~
return zlist(i, j, pc); 
amlist ~
return amlist(i, j, pc);
add_proc ~
return add_proc(i, pc);
add_path ~
return add_path(i, j, pc);
amsave ~
return amsave(i, pc);
aolist ~
return aolist(i, j, pc);
aosave ~
return aosave(i, pc);
adsave ~
return adsave(i, pc);
arlist ~
return arlist(i, j, pc);
auction *channels ~
NOP
avintre *avintre ~
NOP
anitre *anitre ~
NOP

# case B
bug typo *bug ~
NOP
buglist ~
return buglist(BT_BUGS, i, cooked_strs[1], j, cooked_strs[2], cooked_strs[3], pc);
balance ~
return balance(i, &(cooked_strs[1]), pc);
backstab bs ~
return backstab(i, &(cooked_strs[1]), pc);
bash ~
return bash(i, &cooked_strs[1], pc);
beep ~
return beep(i, &(cooked_strs[1]), pc);
beckon socials ~
beckon(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
berserk ~
return berserk(pc);
block ~
return block(i, &(cooked_strs[1]), pc);
blush socials ~
blush(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
blend ~
return blend(pc);
bonk socials ~
bonk(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
body_slam bodyslam bodyslam ~
return body_slam(i, &(cooked_strs[1]), pc);
bounce socials ~
bounce(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
bow socials ~
bow(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
concoct ~
return concoct(pc);
burp socials ~
burp(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
butcher ~
return butcher(i, &(cooked_strs[1]), pc);
buy ~
return buy(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
brief ~
return brief(pc);


# case C
cast ~
return cast(&(cooked_strs[1]), j, &(cooked_strs[2]), pc, was_ordered);
cackle socials ~
cackle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
circle ~
return circle(i, &(cooked_strs[1]), pc);
clap socials ~
clap(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
claw ~
return claw(i, &(cooked_strs[1]), pc);
clear_keryword ~
return clear_keyword(i, pc);
clear_mnames ~
return clear_mnames(i, pc);
clear_onames ~
return clear_onames(i, pc);
close ~
return close(i, &(cooked_strs[1]), pc);
color ansi color ~
return color((cooked_strs[1]), (cooked_strs[2]), pc);
consider ~
return consider(i, &(cooked_strs[1]), pc);
cheer socials ~
cheer(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
chuckle socials ~
chuckle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
ch_mdesc ~
return ch_mdesc(i, pc);
ch_msdesc ~
return ch_msdesc(i, &(cooked_strs[1]), pc);
ch_mndesc ~
return ch_mndesc(i, &(cooked_strs[1]), pc);
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
ch_kdesc ~
return ch_kdesc(i, pc);
ch_ddesc ~
return ch_ddesc(i, pc);
ch_rname ~
return ch_rname(&(cooked_strs[1]), pc);
ch_zname ~
return ch_zname(i, &(cooked_strs[1]), pc);
construct ~
return construct(pc);
create_concoction create_conct ~
return create_concoction(i, j, k, l, m, n, pc);
create_construction create_constrct ~
return create_construction(i, j, k, l, m, n, pc);
curse ~
curse(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
curtsey socials ~
curtsey(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
credits *credits ~
NOP
commands *commands ~
NOP
classes *classes ~
NOP
channels tell ask gt gc *channels ~
NOP

# case D
down ~
return down(pc, *(pc.getCurRoom()), is_dead);
date ~
return date(pc);
dance socials ~
dance(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
deposit ~
return deposit(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
describe ~
return describe(pc);
delete_door ~
return delete_door(i, &(cooked_strs[1]), pc);
discuss ~
return discuss(&(cooked_strs[1]), j, &(cooked_strs[2]), pc);
disco socials ~
disco(j, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
disdain socials ~
disdain(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
disarm ~
return disarm(i, &(cooked_strs[1]), pc);
does_own comparators ~
return does_own(pc, i, j, k, l, m, n);
donate ~
return donate(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
drop ~
return drop(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
drink ~
return drink(i, &(cooked_strs[1]), pc);
dset ~
return dset(i, &(cooked_strs[1]), &(cooked_strs[2]), k, pc);
door_to ~
return door_to(i, j, &(cooked_strs[1]), pc);
dstat ~
return dstat(i, &(cooked_strs[1]), pc);
dsys ~
return dsys(i, cooked_strs[1], pc);
dclear ~
return dclear(i, pc);
dlist ~
return dlist(i, j, pc);
dvnum ~
return dvnum(i, &(cooked_strs[1]), pc);
drow *drow ~
NOP
dwarves dwarf *dwarf ~
NOP
dragons *dragon ~
NOP
darklings *darkling ~
NOP

# case E
east ~
return east(pc, *(pc.getCurRoom()), is_dead);
eat ~
return eat(i, &(cooked_strs[1]), pc);
earthmeld ~
return earthmeld(pc);
equipment ~
return show_eq(pc);
empty ~
return empty(i, &(cooked_strs[1]), pc);
enslave ~
return enslave(i, &(cooked_strs[1]), pc);
exits ~
return exit(pc);
examine ~
return examine(i, &(cooked_strs[1]), pc);
exact_damage ~
return exact_damage(i, j, cooked_strs[2], pc);
elf *elf ~
NOP
elves *elves ~
NOP
emote *emote ~
NOP
entity *entity ~
NOP

# case F
fill ~
return fill(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
follow ~
return follow(i, &(cooked_strs[1]), pc);
flee ~
return flee(pc, is_dead);
flex socials ~
flex(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
flip_door ~
return flip_door(i, &(cooked_strs[1]), pc);
freeze ~
return freeze(i, &(cooked_strs[1]), pc);
force *force ~
NOP
force_all *force_all ~
NOP
faerie *faerie ~
NOP

#case G
gag ~
return gag(i, &(cooked_strs[1]), pc);
get take get ~
return get(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
giggle socials ~
giggle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
give ~
return give(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
grin socials ~
grin(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
group ~
return group(i, &(cooked_strs[1]), pc);
growl socials ~
growl(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
grumble socials ~
grumble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
grunt socials ~
grunt(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
guard ~
return guard(i, &(cooked_strs[1]), pc);
go enter go ~
is_dead = FALSE; return go(i, &(cooked_strs[1]), pc, is_dead);
goto ~
return _goto(i, &(cooked_strs[1]), pc);
gecho *gecho ~
NOP
gossip *channels ~
NOP
gt *channels ~
NOP
gs *channels ~
NOP

# case H
hide ~
return hide(pc);
hit kill attack murder hit ~
return hit(i, &(cooked_strs[1]), pc);
help man help ~
return help(i, &(cooked_strs[1]), pc);
hehe socials ~
hehe(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hold ~
return hold(i, &(cooked_strs[1]), pc);
hop socials ~
hop(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hmm socials ~
hmm(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hug socials ~
hug(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hum socials ~
hum(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
hurl ~
return hurl(i, &(cooked_strs[1]), pc);
humans *human ~
NOP

# case I
inventory ~
return inventory(pc);
idealist buglist ~
return buglist(BT_IDEAS, i, cooked_strs[1], j, cooked_strs[2], cooked_strs[3], pc);
insane socials ~
insane(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
is_equal_to comparators ~
return is_equal_to(i, cooked_strs[1], cooked_strs[2], pc);
is_greater_than comparators ~
return is_greater_than(i, cooked_strs[1], cooked_strs[2], pc);
is_in_posn comparators ~
return is_in_posn(cooked_strs[1], pc);
is_less_than comparators ~
return is_less_than(i, cooked_strs[1], cooked_strs[2], pc);
itch socials ~
itch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
itrans ~
return itrans(i, &(cooked_strs[1]), j, pc);
idea *idea ~
NOP
ironclads *ironclad ~
NOP

#case K
kiss socials ~
kiss(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
kick ~
return kick(i, &(cooked_strs[1]), pc);
keywords *keyword ~
NOP

look ls look ~
return look(i, &(cooked_strs[1]), pc, TRUE); /* ignore brief */
lag socials ~
lag(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
laugh socials ~
laugh(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
label_dummy label: add_mob_script ~
return -1; //should never get here
lick socials ~
lick(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
list ~
return list_merchandise(i, &(cooked_strs[1]), pc);
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
listen socials ~
listen(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
lock ~
return lock(i, &(cooked_strs[1]), pc);
lore ~
return lore(i, &(cooked_strs[1]), pc);
log_level ~
return log_level(i, pc);
ldwho ~
return ldwho(pc);

# case M
make_builder ~
return make_builder(i, &(cooked_strs[1]), pc);
make_pso ~
return make_pso(i, &(cooked_strs[1]), pc);
meditate ~
return meditate(pc);
mstat ~
return mstat(i, &(cooked_strs[1]), pc);
mclone ~
return mclone(i, &(cooked_strs[1]), pc);
mclear ~
return mclear(i, pc);
mreload ~
return mreload(pc);
mlist ~
return  mlist(i, j, pc);
mload ~
return mload(i, &(cooked_strs[1]), pc);
mset ~
return mset(i, &(cooked_strs[1]), &(cooked_strs[2]), k, &(cooked_strs[3]), pc);
muahaha socials ~
muahaha(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
mvnum ~
return mvnum(i, &(cooked_strs[1]), pc);
move *move ~
NOP

# case N
north ~
return north(pc, *(pc.getCurRoom()), is_dead);
northeast ne northeast ~
return northeast(pc, *(pc.getCurRoom()), is_dead);
northwest nw northwest ~
return northwest(pc, *(pc.getCurRoom()), is_dead);
nog socials ~
nog(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
nogossip ~
return nogossip(pc);
nod socials ~
nod(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
neighbor_echo *add_mob_script ~
NOP

# case O
offer ~
return offer(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
open ~
return open(i, &(cooked_strs[1]), pc);
opurge ~
return opurge(i, &(cooked_strs[1]), pc);
ostat ~
return ostat(i, &(cooked_strs[1]), pc);
oclone ~
return oclone(i, &(cooked_strs[1]), pc);
oclear ~
return oclear(i, pc);
olist ~
return olist(i, j, pc);
oload ~
return oload(i, &(cooked_strs[1]), pc);
oreload ~
return oreload(pc);
oset ~
return oset(i, &(cooked_strs[1]), &(cooked_strs[2]), k, &(cooked_strs[3]), pc);
ovnum ~
return ovnum(i, &(cooked_strs[1]), pc);
olc ~
return handle_olc(pc, do_sub);
order *order ~
NOP
ogrue *ogrue ~
NOP

# case P
pat socials ~
pat(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
passwd ~
return passwd(&(cooked_strs[1]), &(cooked_strs[2]), &(cooked_strs[3]), pc);
page_break ~
return page_break(i, pc);
practice ~
return practice(&(cooked_strs[1]), j, &(cooked_strs[2]), pc);
pay ~
return pay(i, j, &(cooked_strs[1]), pc);
pause add_mob_script ~
return pause(i, pc);
pinch socials ~
pinch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
possess ~
return possess(i, &(cooked_strs[1]), pc);
prompt ~
return prompt(&(cooked_strs[1]), pc);
pft socials ~
pft(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
picklock ~
return picklock(i, &(cooked_strs[1]), pc);
poke socials ~
poke(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
ponder socials ~
ponder(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
post ~
return post(&(cooked_strs[1]), pc);
prone ~
return prone(pc);
puke socials ~
puke(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
put ~
return put(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
pemote *pemote ~
NOP
poofin *poofin ~
NOP
poofout *poofout ~
NOP
pecho *add_mob_script ~
NOP
pout *socials ~
NOP

# case Q
quaf ~
return quaf(i, &(cooked_strs[1]), pc);
quit logout log_out quit ~
return log_out(pc);

# case R
reset_olc ~
return reset_olc(pc);
remove ~
return remove(i, &(cooked_strs[1]), pc);
rest ~
return rest(pc);
recite ~
return recite(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
read ~
return read(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
rem_name rm_name rem_name ~
return rem_mname(i, &(cooked_strs[1]), pc);
rem_oname rm_oname rem_oname ~
return rem_oname(i, &(cooked_strs[1]), pc);
rem_zone rm_zone rem_zone ~
return remZone(i, pc);
rem_path rm_path rem_path ~
return rem_path(i, j, pc);
rem_keyword rm_keyword rm_keyword ~
return rm_keyword(i, pc);
rem_perm_inv rm_perm_inv rem_perm_inv ~
return rem_perm_inv(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
rescue ~
return rescue(i, &(cooked_strs[1]), pc);
rezone ~
return rezone(pc);
replace_door ~
return replace_door(i, &(cooked_strs[1]), pc);
rofl socials ~
rofl(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
rinit ~
return rinit(i, j, pc);
roll ~
if ((i != 1) || (j != 1)) roll(i, j, pc); else roll(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
rstat ~
return rstat(i, pc);
rset ~
return rset(&(cooked_strs[1]), j, pc);
rm_give_proc ~
return rm_give_proc(i, pc);
rm_discuss_proc ~
return rm_discuss_proc(i, pc);
rm_bow_proc ~
return rm_bow_proc(i, pc);
rm_curse_proc ~
return rm_curse_proc(i, pc);
rm_obj_script rem_obj_script rm_obj_script ~
return rem_obj_script(i, cooked_strs[1], j, pc);
rm_room_script rem_room_script rm_room_script ~
return rem_room_script(i, cooked_strs[1], j, pc);
rm_script rem_script rm_script ~
return rem_script(i, (cooked_strs[1]), j, pc);
rm_stat_affect rem_stat_affect rm_stat_affect ~
return rm_stat_affect(i, j, pc);
rm_casts_spell rem_casts_spell rm_casts_spell ~
return rm_casts_spell(i, j, pc);
rclone ~
return rclone(i, &(cooked_strs[1]), j, pc);
rlist ~
return rlist(i, j, pc);
rclear ~
return rclear(i, pc);
reply *reply ~
NOP
rocktrolls *rocktroll ~
NOP
races *races ~
NOP
regeneration *regeneration ~
NOP
recho *recho ~
NOP
rm_omove *rm_omove ~
NOP
rm_omove_all *rm_omove_all ~
NOP
rm_move *rm_move ~
NOP
rm_move_all *rm_move_all ~
NOP

# case S
south ~
return south(pc, *(pc.getCurRoom()), is_dead);
southeast se southeast ~
return southeast(pc, *(pc.getCurRoom()), is_dead);
southwest sw southwest ~
return southwest(pc, *(pc.getCurRoom()), is_dead);
save ~
return ::save(pc);
sacrifice junk junk ~
return junk(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
scan ~
return scan(pc);
set_path ~
return set_path_dir(i, j, k, &(cooked_strs[1]), pc);
set_ven_stop ~
return  set_veh_stop(i, j, &(cooked_strs[1]), pc);
set_path_pointer ~
return set_path_pointer(i, j, pc);
set_zflag ~
return set_zflag(i, &(cooked_strs[1]), pc);
save_mob ~
return save_mob(i, &(cooked_strs[1]), pc);
save_obj ~
return save_obj(i, &(cooked_strs[1]), pc);
score ~
return score(&(cooked_strs[1]), pc);
scold socials ~
scold(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
script_jump_true add_mob_proc ~
return script_jump_true(cooked_strs, cooked_ints, pc, c_script_owner, r_script_owner, sanity);
script_jump_false add_mob_script ~
return script_jump_false(cooked_strs, cooked_ints, pc, c_script_owner, r_script_owner, sanity);
scream socials ~
scream(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
scratch socials ~
scratch(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
scribe ~
return scribe(&(cooked_strs[1]), pc);
see_all ~
return see_all(pc);
shield ~
return shield(i, &(cooked_strs[1]), pc, was_ordered);
shrug socials ~
shrug(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
shake socials ~
shake(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
shoot ~
return shoot(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
show_zones ~
return showZones(pc);
sigh socials ~
sigh(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
silly socials ~
silly(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
silent_junk add_mob_script ~
return silent_junk(i, &(cooked_strs[1]), &(cooked_strs[2]), pc);
sit ~
return sit(pc);
siteban ~
return siteban(&(cooked_strs[1]), pc);
sell ~
return sell(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
skin ~
return skin(i, &(cooked_strs[1]), pc);
sleep ~
return sleep(pc);
slay ~
return slay(i, &(cooked_strs[1]), pc);
slist ~
return slist(i, cooked_strs[1], j, pc);
smirk socials ~
smirk(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
snicker socials ~
snicker(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
sneak ~
return sneak(pc);
snoop ~
return snoop(i, &(cooked_strs[1]), pc);
sob socials ~
sob(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
stat_script ~
return stat_script(i, j, pc);
splash socials ~
splash(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
stand ~
return stand(pc);
stat_keyword ~
return stat_keyword(i, pc);
stat_obj_script ~
return stat_obj_script(i, j, pc);
stat_path ~
return stat_path(i, j, pc);
stat_room_script ~
return stat_room_script(i, j, pc);
steal ~
return steal(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
sockets ~
return sockets(pc);
suicide ~
return suicide(&(cooked_strs[1]), pc);
shutdown ~
return shutdown(&(cooked_strs[1]), pc);
slap socials ~
slap(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
smile socials ~
smile(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
strut socials ~
strut(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
smob *smob ~
NOP
sombri *sombri ~
NOP
socials *socials ~
NOP
shout *shout ~
NOP
self *possess ~
NOP
say talk *say ~
NOP
skills *skills ~
NOP

# case T
tail ~
return tail(i, &(cooked_strs[1]), pc);
teach ~
return teach(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
throw ~
return _throw(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
thank socials ~
thank(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
time ~
return time(pc);
track ~
return track(i, &(cooked_strs[1]), pc);
twiddle socials ~
twiddle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
toggle ~
return toggle_prompt(&(cooked_strs[1]), pc);
tog_veh_stop ~
return tog_veh_stop(i, j, pc);
total_rezone ~
return total_rezone(pc);
tog_mflag ~
return tog_mflag(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
tog_oflag ~
return tog_oflag(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
tog_rflag ~
return tog_rflag(i, pc);
thaw ~
return thaw(i, &(cooked_strs[1]), pc);
topics *topics ~
NOP
title *title ~
NOP
theme *theme ~
NOP

# case U
up ~
return up(pc, *(pc.getCurRoom()), is_dead); 
unlock ~
return unlock(i, &(cooked_strs[1]), pc); 
unsiteban ~
return unsiteban(&(cooked_strs[1]), pc); 
unpost ~
return unpost(i, &(cooked_strs[1]), pc); 
ungag ~
return ungag(i, &(cooked_strs[1]), pc); 
ungroup ~
return pc.doUngroup(i, &(cooked_strs[1])); 
unsnoop ~
return unsnoop(pc); 
uptime ~
return uptime(pc); 
update_cig update_cur_in_game update_cig ~
return update_cur_in_game(pc); 
use ~
return use(i, &(cooked_strs[1]), pc); 
using_client ~
return using_client(pc); 
unpossess *unpossess ~
NOP

# case V
value_add ~
return value_add(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
value_list ~
return value_list(i, &(cooked_strs[1]), pc);
value_set ~
return value_set(i, &(cooked_strs[1]), j, k, l, pc);
value_rem ~
return value_rem(i, &(cooked_strs[1]), j, pc);
visible ~
return visible(pc);


# case W
west ~
return west(pc, *(pc.getCurRoom()), is_dead);
wear ~
return wear(i, &(cooked_strs[1]), j, &(cooked_strs[2]), pc);
weep socials ~
weep(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wake ~
return wake(pc);
wave socials ~
wave(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wink socials ~
wink(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wiggle socials ~
wiggle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
whistle socials ~
whistle(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
where ~
return where(i, &(cooked_strs[1]), pc);
who ~
return who(pc);
withdraw ~
return withdraw(i, cooked_strs[1], j, cooked_strs[2], pc);
wield ~
return wield(i, &(cooked_strs[1]), pc);
wibble socials ~
wibble(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
wimpy ~
return wimpy(i, pc);
wizchat *channels ~
NOP
wizhelp *wizhelp ~
NOP
wc *channels ~
NOP
wizinvis ~
return wizinvis(pc);
write_world ~
return write_world(pc);

# case X


# case Y
yawn socials ~
yawn(i, &(cooked_strs[1]), pc, (*(pc.getCurRoom()))); return 0;
yell *yell ~
NOP

# case Z
zlock ~
return zlock(i, pc); 
zlist ~
return zlist(i, j, pc); 
zcreate ~
return zcreate(i, &(cooked_strs[1]), j, pc); 
zenable ~
return zenable(i, &(cooked_strs[1]), pc); 
zdisable ~
return zdisable(i, &(cooked_strs[1]), pc); 
zgoto ~
return zgoto(i, pc); 
zunlock ~
return zunlock(i, pc); 
zecho *zecho ~
NOP
~