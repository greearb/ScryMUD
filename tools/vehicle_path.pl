#!/usr/bin/perl

# $Id$

# Given a speedwalk list: e;n;n;n;se;se;e;e;e
# This script will spit out all the commands necessary to create a ScryMUD
# vehicle path. It will reverse the speedwalk commands given to automatically
# create the return path. This isn't always necessarily what you want. This
# script will automatically mark the starting and ending (after the path)
# rooms as destinations.

# Copyright (C) 2004, Edward Roper <eroper@wanfear.com>
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

$veh_num = -1;
$vehicle_name = "";

if ( $vehicle_num == -1 ) {
   die("You must set \$vehicle_num");
}
if ( $vehicle_name == "" ) {
   die("You must set \$vehicle_name");
}

%dirs = (
      "north" => 0,
      "northeast" => 1,
      "east" => 2,
      "southeast" => 3,
      "south" => 4,
      "southwest" => 5,
      "west" => 6,
      "northwest" => 7,
      "up" => 80,
      "down" => 90,
      "none" => 100,
      );

$prev_dir = "none";

while ( <> ) {
        chomp;

#@there = split /;/;

        $backward = $forward = $_;
        $forward =~ s/;/ /g;
        $backward =~ s/;/ /g;

        $forward =~ s/\bn\b/north/g;
        $backward =~ s/\bn\b/south/g;

        $forward =~ s/\bs\b/south/g;
        $backward =~ s/\bs\b/north/g;

        $forward =~ s/\be\b/east/g;
        $backward =~ s/\be\b/west/g;

        $forward =~ s/\bw\b/west/g;
        $backward =~ s/\bw\b/east/g;

        $forward =~ s/\bne\b/northeast/g;
        $backward =~ s/\bne\b/southwest/g;

        $forward =~ s/\bnw\b/northwest/g;
        $backward =~ s/\bnw\b/southeast/g;

        $forward =~ s/\bse\b/southeast/g;
        $backward =~ s/\bse\b/northwest/g;

        $forward =~ s/\bsw\b/southwest/g;
        $backward =~ s/\bsw\b/northeast/g;

        $forward =~ s/\bu\b/up/g;
        $backward =~ s/\bu\b/down/g;

        $forward =~ s/\bd\b/down/g;
        $backward =~ s/\bd\b/up/g;

        $backward = join(' ', reverse(split(/ /,$backward)));
	
	@there = split(/ /,$forward);
	@back = split(/ /,$backward);

	$total = $forward . " " . $backward;
	@total_path = split(/ /,$total);

	for($i=0;$i<=$#total_path;$i++) {
	print "add_path $veh_num $i\n";
        print "set_path $veh_num $i 1 $total_path[$i]\n";

        $ndir = $dirs{$total_path[$i]};
        $pdir = $dirs{$prev_dir};

#print "DEBUG: $prev_dir || $total_path[$i]\n";


        if ( $i == $#there+1 ) {
           $pdir = $dirs{'none'};
        }

        if ( $pdir == $dirs{'none'} ) {
        } elsif ( $ndir == $pdir ) {
           print "ch_path_desc $veh_num $i\n~\n";
        } elsif ( $ndir > 75 ) {
           print "The $vehicle_name heads $total_path[$i].\n~\n";
        } elsif ( (abs($ndir-$pdir) == 1) || (abs($ndir-$pdir) == 7) ) {
           print "ch_path_desc $veh_num $i\n";
           print "The $vehicle_name veers $total_path[$i].\n~\n";
        } else {
           print "ch_path_desc $veh_num $i\n";
           print "The $vehicle_name turns $total_path[$i].\n~\n";
        }

        $prev_dir = $total_path[$i];
        }#total path walker

print "set_veh_stop $veh_num 0 yes\n";
print "set_veh_stop $veh_num " . ($#there+1) . " yes\n";

}

