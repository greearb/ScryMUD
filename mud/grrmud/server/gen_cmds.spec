#
# ScryMUD Server Code
# Copyright (C) 1998  Ben Greear
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
#  [cmd_name] <*>[help_key]
#  [actual code to call]
#
#  The entries below should make it more clear.  Deviate at your
#  own peril!!

avintre *avintre ~
NOP
anitre *anitre ~
NOP
abilities ~
return abilities(pc);
ack socials ~
ack(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
add_kname ~
return add_kname(i, &str2, pc);
adjust_register ~
return adjust_register(i, &str2, j, pc);
add_keyword ~
return add_keyword(pc);
add_mname ~
return add_mname(i, &str2, pc);
add_mob_script ~
return add_mob_script(pc, i, str2, j, str3, k, l);
add_room_script add_mob_script ~
return add_room_script(pc, i, str2, j, str3, k, l);
add_oname ~
return add_oname(i, &str2, pc);
add_stat_affect ~
return add_stat_affect(i, j, k, pc);
add_casts_spell ~
return add_casts_spell(i, j, k, pc);
add_zone ~
return addZone(i, pc);
adlist ~
return adlist(pc);
afk socials ~
afk(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
agree socials ~
agree(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
add_perm_inv ~
return add_perm_inv(i, &str2, j, pc);
assist ~
return assist(i, &str2, pc);
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

bug typo *bug ~
NOP
backstab bs ~
return backstab(i, &str2, pc);
bash ~
return bash(i, &cooked_strs[1], pc);
beep ~
return beep(i, &str2, pc);
beckon socials ~
beckon(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
berserk ~
return berserk(pc);
blush socials ~
blush(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
blend ~
return blend(pc);
bonk socials ~
bonk(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
body_slam bodyslam bodyslam ~
return body_slam(i, &str2, pc);
bounce socials ~
bounce(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
bow socials ~
bow(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
concoct ~
return concoct(pc);
burp socials ~
burp(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
butcher ~
return butcher(i, &str2, pc);
buy ~
return buy(i, &str2, j, &str3, pc);
brief ~
return brief(pc);

credits *credits ~
NOP
commands *commands ~
NOP
classes *classes ~
NOP
channels gt gc *channels ~
NOP
cast ~
return cast(&str2, j, &str3, pc);
cackle socials ~
cackle(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
circle ~
return circle(i, &str2, pc);
clap socials ~
clap(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
claw ~
return claw(i, &str2, pc);
clear_keryword ~
return clear_keyword(i, pc);
clear_mnames ~
return clear_mnames(i, pc);
clear_onames ~
return clear_onames(i, pc);
close ~
return close(i, &str2, pc);
color ~
return color(str2, str3, pc);
consider ~
return consider(i, &str2, pc);
cheer socials ~
cheer(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
chuckle socials ~
chuckle(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
ch_mdesc ~
return ch_mdesc(i, pc);
ch_msdesc ~
return ch_msdesc(i, &str2, pc);
ch_mndesc ~
return ch_mndesc(i, &str2, pc);
ch_odesc ~
return ch_odesc(i, pc);
ch_ondesc ~
return ch_ondesc(i, &str2, pc);
ch_osdesc ~
return ch_osdesc(i, &str2, pc);
ch_path_desc ~
return ch_path_desc(i, j, pc);
ch_rdesc ~
return ch_rdesc(pc);
ch_kdesc ~
return ch_kdesc(i, pc);
ch_ddesc ~
return ch_ddesc(i, pc);
ch_rname ~
return ch_rname(&str2, pc);
ch_zname ~
return ch_zname(i, &str2, pc);
construct ~
return construct(pc);
create_concoction create_concoct ~
return create_concoction(i, j, k, l, m, n, pc);
create_construction create_constrct ~
return create_construction(i, j, k, l, m, n, pc);
curse ~
curse(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
curtsey socials ~
curtsey(i, &str2, pc, (*(pc.getCurRoom()))); return 0;

drow *drow ~
NOP
dwarves dwarf *dwarf ~
NOP
dragons *dragon ~
NOP
darklings *darkling ~
NOP
down ~
return down(pc, *(pc.getCurRoom()), is_dead);
date ~
return date(pc);
dance socials ~
dance(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
describe ~
return describe(pc);
delete_door ~
return delete_door(i, &str2, pc);
discuss ~
return discuss(&str2, j, &str3, pc);
disco socials ~
disco(j, &str2, pc, (*(pc.getCurRoom()))); return 0;
disdain socials ~
disdain(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
disarm ~
return disarm(i, &str2, pc);
donate ~
return donate(i, &str2, &str3, pc);
drop ~
return drop(i, &str2, &str3, pc);
drink ~
return drink(i, &str2, pc);
dset ~
return dset(i, &str2, &str3, k, pc);
door_to ~
return door_to(i, j, &str2, pc);
dstat ~
return dstat(i, &str2, pc);
dsys ~
return dsys(pc);
dclear ~
return dclear(i, pc);
dlist ~
return dlist(i, j, pc);
dvnum ~
return dvnum(i, &str2, pc);

east ~
return east(pc, *(pc.getCurRoom()), is_dead);
eat ~
return eat(i, &str2, pc);
earthmeld ~
return earthmeld(pc);
equipment ~
return show_eq(pc);
empty ~
return empty(i, &str2, pc);
enslave ~
return enslave(i, &str2, pc);
exits ~
return exit(pc);
examine ~
return examine(i, &str2, pc);
elf *elf ~
NOP
elves *elves ~
NOP
emote *emote ~
NOP
entity *entity ~
NOP

faerie *faerie ~
NOP
fill ~
return fill(i, &str2, j, &str3, pc);
follow ~
return follow(i, &str2, pc);
flee ~
return flee(pc, is_dead);
flex socials ~
flex(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
flip_door ~
return flip_door(i, &str2, pc);
freeze ~
return freeze(i, &str2, pc);
gag ~
return gag(i, &str2, pc);
get take get ~
return get(i, &str2, j, &str3, pc);
giggle socials ~
giggle(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
give ~
return give(i, &str2, j, &str3, pc);
grin socials ~
grin(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
group ~
return group(i, &str2, pc);
growl socials ~
growl(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
grumble socials ~
grumble(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
grunt socials ~
grunt(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
guard ~
return guard(i, &str2, pc);
go enter go ~
is_dead = FALSE; return go(i, &str2, pc, is_dead);
goto ~
return _goto(i, &str2, pc);

hide ~
return hide(pc);
hit kill attack murder hit ~
return hit(i, &str2, pc);
help ~
return help(i, &str2, pc);
hehe socials ~
hehe(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
hold ~
return hold(i, &str2, pc);
hop socials ~
hop(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
hmm socials ~
hmm(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
hug socials ~
hug(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
hum socials ~
hum(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
hurl ~
return hurl(i, &str2, pc);
humans *human ~
NOP

ironclads *ironclad ~
NOP
inventory ~
return inventory(pc);
insane socials ~
insane(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
itch socials ~
itch(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
itrans ~
return itrans(i, &str2, j, pc);
junk sacrifice junk ~
return junk(i, &str2, &str3, pc);

keywords *keyword ~
NOP
kiss socials ~
kiss(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
kick ~
return kick(i, &str2, pc);

look ~
return look(i, &str2, pc);
lag socials ~
lag(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
laugh socials ~
laugh(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
label_dummy label: add_mob_script ~
return -1; //should never get here
lick socials ~
lick(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
list ~
return list_merchandise(i, &str2, pc);
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
listen(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
lock ~
return lock(i, &str2, pc);
lore ~
return lore(i, &str2, pc);
log_level ~
return log_level(i, pc);
ldwho ~
return ldwho(pc);

make_builder ~
return make_builder(i, &str2, pc);
make_pso ~
return make_pso(i, &str2, pc);
meditate ~
return meditate(pc);
mstat ~
return mstat(i, &str2, pc);
mclone ~
return mclone(i, &str2, pc);
mclear ~
return mclear(i, pc);
mreload ~
return mreload(pc);
mlist ~
return  mlist(i, j, pc);
mload ~
return mload(i, &str2, pc);
mset ~
return mset(i, &str2, &str3, k, &str4, pc);
muahaha socials ~
muahaha(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
mvnum ~
return mvnum(i, &str2, pc);

north ~
return north(pc, *(pc.getCurRoom()), is_dead);
northeast ne northeast ~
return northeast(pc, *(pc.getCurRoom()), is_dead);
northwest nw northwest ~
return northwest(pc, *(pc.getCurRoom()), is_dead);
nogossip ~
return nogossip(pc);
nod socials ~
nod(i, &str2, pc, (*(pc.getCurRoom()))); return 0;

ogrue *ogrue ~
NOP
offer ~
return offer(i, &str2, j, &str3, pc);
open ~
return open(i, &str2, pc);
opurge ~
return opurge(i, &str2, pc);
ostat ~
return ostat(i, &str2, pc);
oclone ~
return oclone(i, &str2, pc);
oclear ~
return oclear(i, pc);
olist ~
return olist(i, j, pc);
oload ~
return oload(i, &str2, pc);
oreload ~
return oreload(pc);
oset ~
return oset(i, &str2, &str3, k, &str4, pc);
ovnum ~
return ovnum(i, &str2, pc);
olc ~
return handle_olc(pc, do_sub);

pat socials ~
pat(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
passwd ~
return passwd(&str2, &str3, &str4, pc);
page_break ~
return page_break(i, pc);
practice ~
return practice(&str2, j, &str3, pc);
pay ~
return pay(i, j, &str2, pc);
pause add_mob_script ~
return pause(i, pc);
pinch socials ~
pinch(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
possesss ~
return possess(i, &str2, pc);
prompt ~
return prompt(&str2, pc);
pft socials ~
pft(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
picklock ~
return picklock(i, &str2, pc);
poke socials ~
poke(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
ponder socials ~
ponder(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
post ~
return post(&str2, pc);
prone ~
return prone(pc);
puke socials ~
puke(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
put ~
return put(i, &str2, j, &str3, pc);

quaf ~
return quaf(i, &str2, pc);
quit logout log_out quit ~
return log_out(pc);

rocktrolls *rocktroll ~
NOP
races *races ~
NOP
regeneration *regeneration ~
NOP
reset_olc ~
return reset_olc(pc);
remove ~
return remove(i, &str2, pc);
rest ~
return rest(pc);
recite ~
return recite(i, &str2, j, &str3, pc);
read ~
return read(i, &str2, pc);
rem_name rm_name rem_name ~
return rem_mname(i, &str2, pc);
rem_oname rm_oname rem_oname ~
return rem_oname(i, &str2, pc);
rem_zone rm_zone rem_zone ~
return remZone(i, pc);
rem_path rm_path rem_path ~
return rem_path(i, j, pc);
rem_keyword rm_keyword rm_keyword ~
return rm_keyword(i, pc);
rem_perm_inv rm_perm_inv rem_perm_inv ~
return rem_perm_inv(i, &str2, j, &str3, pc);
rescue ~
return rescue(i, &str2, pc);
rezone ~
return rezone(pc);
replace_door ~
return replace_door(i, &str2, pc);
rofl socials ~
rofl(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
rinit ~
return rinit(i, j, pc);
roll ~
if ((i != 1) || (j != 1)) roll(i, j, pc); else roll(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
rstat ~
return rstat(i, pc);
rset ~
return rset(&str2, j, pc);
rm_give_proc ~
return rm_give_proc(i, pc);
rm_discuss_proc ~
return rm_discuss_proc(i, pc);
rm_bow_proc ~
return rm_bow_proc(i, pc);
rm_curse_proc ~
return rm_curse_proc(i, pc);
rm_room_script rem_room_script rm_room_script ~
return rem_room_script(i, str2, j, pc);
rm_script rem_script rm_script ~
return rem_script(i, str2, j, pc);
rm_stat_affect rem_stat_affect rm_stat_affect ~
return rm_stat_affect(i, j, pc);
rm_casts_spell rem_casts_spell rm_casts_spell ~
return rm_casts_spell(i, j, pc);
rclone ~
return rclone(i, &str2, j, pc);
rlist ~
return rlist(i, j, pc);
rclear ~
return rclear(i, pc);

south ~
return south(pc, *(pc.getCurRoom()), is_dead);
southeast se southeast ~
return southeast(pc, *(pc.getCurRoom()), is_dead);
southwest sw southwest ~
return southwest(pc, *(pc.getCurRoom()), is_dead);
save ~
return ::save(pc);
scan ~
return scan(pc);
set_path ~
return set_path_dir(i, j, k, &str2, pc);
set_ven_stop ~
return  set_veh_stop(i, j, &str2, pc);
set_path_pointer ~
return set_path_pointer(i, j, pc);
save_mob ~
return save_mob(i, &str2, pc);
save_obj ~
return save_obj(i, &str2, pc);
score ~
return score(&str2, pc);
scold socials ~
scold(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
script_jump_true add_mob_proc ~
return script_jump_true(cooked_strs, cooked_ints, pc, c_script_owner, r_script_owner);
script_jump_false add_mob_script ~
return script_jump_false(cooked_strs, cooked_ints, pc, c_script_owner, r_script_owner);
scream socials ~
scream(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
scratch socials ~
scratch(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
scribe ~
return scribe(&str2, pc);
see_all ~
return see_all(pc);
shield ~
return shield(i, &str2, pc);
shrug socials ~
shrug(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
shake socials ~
shake(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
shoot ~
return shoot(i, &str2, j, &str3, pc);
show_zones ~
return showZones(pc);
sigh socials ~
sigh(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
silly socials ~
silly(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
silent_junk add_mob_script ~
return silent_junk(i, &str2, &str3, pc);
sit ~
return sit(pc);
siteban ~
return siteban(&str2, pc);
sell ~
return sell(i, &str2, j, &str3, pc);
skin ~
return skin(i, &str2, pc);
sleep ~
return sleep(pc);
slay ~
return slay(i, &str2, pc);
slist ~
return slist(i, j, pc);
smirk socials ~
smirk(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
snicker socials ~
snicker(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
sneak ~
return sneak(pc);
snoop ~
return snoop(i, &str2, pc);
sob socials ~
sob(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
stat_script ~
return stat_script(i, j, pc);
splash socials ~
splash(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
stand ~
return stand(pc);
stat_keyword ~
return stat_keyword(i, pc);
stat_path ~
return stat_path(i, j, pc);
stat_room_script ~
return stat_room_script(i, j, pc);
steal ~
return steal(i, &str2, j, &str3, pc);
sockets ~
return sockets(pc);
suicide ~
return suicide(&str2, pc);
shutdown ~
return shutdown(&str2, pc);
slap socials ~
slap(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
smile socials ~
smile(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
strut socials ~
strut(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
smob *smob ~
NOP
sombri *sombri ~
NOP
socials *socials ~
NOP

tail ~
return tail(i, &str2, pc);
teach ~
return teach(i, &str2, j, &str3, pc);
throw ~
return _throw(i, &str2, j, &str3, pc);
thank socials ~
thank(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
time ~
return time(pc);
track ~
return track(i, &str2, pc);
twiddle socials ~
twiddle(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
toggle ~
return toggle_prompt(&str2, pc);
tog_veh_stop ~
return tog_veh_stop(i, j, pc);
total_rezone ~
return total_rezone(pc);
tog_mflag ~
return tog_mflag(i, &str2, j, &str3, pc);
tog_oflag ~
return tog_oflag(i, &str2, j, &str3, pc);
tog_rflag ~
return tog_rflag(i, pc);
tog_zflag ~
return tog_zflag(i, pc);
thaw ~
return thaw(i, &str2, pc);
topics *topics ~
NOP

up ~
return up(pc, *(pc.getCurRoom()), is_dead); 
unlock ~
return unlock(i, &str2, pc); 
unsiteban ~
return unsiteban(&str2, pc); 
unpost ~
return unpost(i, &str2, pc); 
ungag ~
return ungag(i, &str2, pc); 
ungroup ~
return pc.doUngroup(i, &str2); 
unsnoop ~
return unsnoop(pc); 
uptime ~
return uptime(pc); 
update_cig update_cur_in_game update_cig ~
return update_cur_in_game(pc); 
use ~
return use(i, &str2, pc); 
using_client ~
return using_client(pc); 

value_add ~
return value_add(i, &str2, j, &str3, pc);
value_list ~
return value_list(i, &str2, pc);
value_set ~
return value_set(i, &str2, j, k, l, pc);
value_rem ~
return value_rem(i, &str2, j, pc);
visible ~
return visible(pc);


west ~
return west(pc, *(pc.getCurRoom()), is_dead);
wear ~
return wear(i, &str2, j, &str3, pc);
weep socials ~
weep(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
wake ~
return wake(pc);
wave socials ~
wave(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
wink socials ~
wink(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
wiggle socials ~
wiggle(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
whistle socials ~
whistle(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
where ~
return where(i, &str2, pc);
who ~
return who(pc);
wield ~
return wield(i, &str2, pc);
wibble socials ~
wibble(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
wimpy ~
return wimpy(i, pc);
wizinvis ~
return wizinvis(pc);
write_world ~
return write_world(pc);
yawn socials ~
yawn(i, &str2, pc, (*(pc.getCurRoom()))); return 0;
zlock ~
return zlock(i, pc); 
zlist ~
return zlist(i, j, pc); 
zcreate ~
return zcreate(i, &str2, j, pc); 
zenable ~
return zenable(i, &str2, pc); 
zdisable ~
return zdisable(i, &str2, pc); 
zgoto ~
return zgoto(i, pc); 
zunlock ~
return zunlock(i, pc); 
~