#!/usr/bin/perl
# $Id$

# This script can be used to archive away pfiles for inactive players. You
# most certainly want to look this over before running it.

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

die("It's not safe to run me without reading and understanding me!\n");

my $dir = "./mud/grrmud/Pfiles";
my $archive_dir = "./pfile-archives";
my @pfiles;
my $player_level;
my $last_line;
my $remort;
my %players;

#get a list of all the pfiles.
opendir(PFILES, $dir) || die("Couldn't open $dir: $!");
@pfiles = grep { -f "$dir/$_" } readdir(PFILES);
closedir PFILES;

printf("%-7s %5s %-30s %9s %13s %s\n",
      "remort", "level", "name", "days away", "time()", "last login");

#read each pfile for player level, last login and remort status.
foreach $p (@pfiles) {
   $level = -1;
   $llogin = -1;
   $remort = "";

   open(PFILE, "$dir/$p") || die ("Couldn't open $dir/$p: $!");

   while ( <PFILE> ) {
      chomp;
      if ($_ =~ /^\w+\s+~\s+names\s+\d+\.\d+\.\d+\.\d+\s+__LEVEL__\s+(\d+)$/) {
         $level = $1;
      }
      if ($_ =~ /^((-?\d+)\s+){6}bday/) {
         @tmp = split;
         $llogin = $tmp[4];
         if ( $last_line =~ /\s?30\s/ ) {
            $remort = "remort";
         }
         last;
      }
      $last_line = $_;
   }#while <PFILE>

   close(PFILE);
   my $tdiff = time() - $llogin;
   my $dstr = scalar localtime($llogin);
   $days = int($tdiff/(60 * 60 * 24));
   printf("%-7s %5s %-30s %9s %13s %s\n",
         $remort, $level, $p, $days, $llogin, $dstr);

   $players{$p}->{'remort'} = $remort;
   $players{$p}->{'level'} = $level;
   $players{$p}->{'days'} = $days;
   $players{$p}->{'llogin'} = $llogin;

}#foreach player file

die("It's not safe to run me without reading and understanding me!\n");

foreach $p_key (keys %players) {
   #don't move remorts
   if ( $players{$p_key}->{'remort'} != "" ) {
      next;
   }

   if ( 
         ( $players{$p_key}->{'days'} > 30 ) &&
         ( $players{$p_key}->{'level'} == 1 ) 
      ) {
      print STDERR "Moving non-rmt level==1 gone>30 days: $dir/$p_key --> $archive_dir/$p_key\n";

      rename("$dir/$p_key", "$archive_dir/$p_key") ||
         warn("move $dir/$p_key $archive_dir/$p_key failed: $!\n");
   } elsif (
         ( $players{$p_key}->{'days'} > 730 ) &&
         ( $players{$p_key}->{'level'} <= 10 ) 
         ) {
      print STDERR "Moving non-rmt level<=10 gone>730 days: $dir/$p_key --> $archive_dir/$p_key\n";

      rename("$dir/$p_key", "$archive_dir/$p_key") ||
         warn("move $dir/$p_key $archive_dir/$p_key failed: $!\n");
   }
}#foreach
