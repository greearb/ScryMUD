#!/usr/bin/perl

# $Id$
# Takes ScryMUD .map files and produces (sloppy) SVG files.
# Copyright (C) 2004, Edward Roper <eroper@wanfear.com>

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

sub doheader {
   print MAPFILE<<ENDOUT;
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 20010904//EN"
"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">
<!-- Created with Inkscape (http://www.inkscape.org/) -->
<svg
   id="svg1"
   sodipodi:version="0.32"
   inkscape:version="0.37"
   width="210mm"
   height="297mm"
   sodipodi:docbase="/home/eroper/docs/ScryMUD/mapmaker"
   sodipodi:docname="mapmaker.svg"
   xmlns="http://www.w3.org/2000/svg"
   xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
   xmlns:sodipodi="http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
   xmlns:xlink="http://www.w3.org/1999/xlink">
  <defs
     id="defs3" />
  <sodipodi:namedview
     id="base"
     pagecolor="#ffffff"
     bordercolor="#666666"
     borderopacity="1.0"
     inkscape:pageopacity="0.0"
     inkscape:pageshadow="2"
     inkscape:zoom="1.38926677"
     inkscape:cx="234.346519"
     inkscape:cy="753.365732"
     inkscape:window-width="640"
     inkscape:window-height="501"
     inkscape:window-x="383"
     inkscape:window-y="121"
     showgrid="true"
     snaptogrid="true"
     gridspacingx="5mm"
     gridspacingy="5mm"
     gridtolerance="1px"
     vertgrid="false" />
ENDOUT
}

sub placebox {
my ($x,$y,$rname,$color) = @_;
print MAPFILE<<ENDOUT;
<rect
   style="font-size:12;fill:none;fill-opacity:0.75;fill-rule:evenodd;stroke:$color;stroke-width:1.875;stroke-dasharray:none;stroke-linejoin:miter;"
   id="$rname"
   width="40.0"
   height="40.0"
   x="$x"
   y="$y"
   ry="8.0"
   rx="8.0" />
ENDOUT
}

sub placeline {
   my($sx,$sy,$dx,$dy,$pname,$color) = @_;
   print MAPFILE<<ENDOUT;
  <path
     style="fill:none;fill-rule:evenodd;stroke:$color;stroke-opacity:1;stroke-width:1pt;stroke-linejoin:miter;stroke-linecap:round;fill-opacity:0.75;"
     d="M $sx $sy L $dx $dy"
     id="$pname" />
ENDOUT
}

sub placetext {
   my($x,$y,$text) = @_;
   print MAPFILE<<ENDOUT;
<text
  x="$x"
  y="$y"
  style="font-size:20.000000;font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;fill:#000000;fill-opacity:1.0000000;stroke:none;stroke-width:1.0000000pt;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1.0000000;font-family:Arial;text-anchor:start;writing-mode:lr"
  >
  $text
</text>
ENDOUT
}


$zone_num;
$zone_name;

$rnum = 0;
$pnum = 0;
$zpnum = 0;

@rooms;
@paths;
@zpaths;

$min_level = 0;
$max_level = 0;
$min_x = 0;
$min_y = 0;

while (<>) {
   chomp;
   if ( $_ =~ /^zone\s(\d+)\s([\w\s]+)$/ ) {
      $zone_num = $1;
      $zone_name = $2;
   } elsif ( $_ =~ /^room/ ) {
      ( $waste,
        $rooms[$rnum]{'rname'},
        $rooms[$rnum]{'qx'},
        $rooms[$rnum]{'qy'},
        $rooms[$rnum]{'qz'},
        $rooms[$rnum]{'collided'}
      ) = split;
#print "$_ || $rooms[$rnum]{'qz'}\n";
      $rnum++;
   } elsif ( $_ =~ /^path/ ) {
      ( $waste,
        $paths[$pnum]{'pname'},
        $paths[$pnum]{'sx'},
        $paths[$pnum]{'sy'},
        $paths[$pnum]{'sz'},
        $paths[$pnum]{'dx'},
        $paths[$pnum]{'dy'},
        $paths[$pnum]{'dz'},
        $paths[$pnum]{'stretched'}
      ) = split;
      $pnum++;
   } elsif ( $_ =~ /^zonepath/ ) {

      ( $waste,
        $zpaths[$zpnum]{'zone'},
        $zpaths[$zpnum]{'pname'},
        $zpaths[$zpnum]{'sx'},
        $zpaths[$zpnum]{'sy'},
        $zpaths[$zpnum]{'sz'},
        $zpaths[$zpnum]{'dx'},
        $zpaths[$zpnum]{'dy'},
        $zpaths[$zpnum]{'dz'}
      ) = split;
      $zpnum++;

   }#if zonepath
}#while there's more input

#figure out the minimum and maximum z-layer.
foreach $rm (@rooms) {

   if (int($rm->{'qz'}) > $max_level) {
      $max_level = $rm->{'qz'};
   } elsif (int($rm->{'qz'}) < $min_level) {
      $min_level = $rm->{'qz'};
   }

   if ($rm->{'qx'} < $min_x) {
      $min_x = $rm->{'qx'};
   }
   if ($rm->{'qy'} < $min_y) {
      $min_y = $rm->{'qy'};
   }

}

my $i;
for ($i=$min_level;$i<=$max_level;$i++) {
   my $fname;
   if ($zone_num eq "") {
      next;
   }
   if ( $i >= 0 ) {
      $fname = sprintf("zone_%03d-level%03d.svg", $zone_num, $i);
   } else {
      $fname = sprintf("zone_%03d-sublevel%03d.svg", $zone_num, abs($i));
   }
   open MAPFILE, ">$fname";
   
   doheader();

   foreach $rm (@rooms) {
      if ( $rm->{'qz'} == $i ) {
         if ( $rm->{'collided'} ) {
            $color = "red";
         } else {
            $color = "black";
         }
         placebox($rm->{'qx'}*45, $rm->{'qy'}*45, $rm->{'rname'}, $color);
#placenum($qx*45+2, $qy*45+20, $num++);
      }
   }

   foreach $pth (@paths) {
      if ( $pth->{'dz'} == $i ) {
         if ($pth->{'stretched'}) {
            $color = "red";
         } else {
            $color = "black";
         }
         $pth->{'sx'} = ( $pth->{'sx'}*45+20 );
         $pth->{'sy'} = ( $pth->{'sy'}*45+20 );
         $pth->{'dx'} = ( $pth->{'dx'}*45+20 );
         $pth->{'dy'} = ( $pth->{'dy'}*45+20 );
         placeline($pth->{'sx'},$pth->{'sy'},$pth->{'dx'},$pth->{'dy'},
               $pth->{'pname'}, $color);
      }
   }

   foreach $pth (@zpaths) {
      if ( $pth->{'dz'} == $i ) {
         $pth->{'sx'} = ( $pth->{'sx'}*45+20 );
         $pth->{'sy'} = ( $pth->{'sy'}*45+20 );
         $pth->{'dx'} = ( $pth->{'dx'}*45+20 );
         $pth->{'dy'} = ( $pth->{'dy'}*45+20 );
         placeline($pth->{'sx'},$pth->{'sy'},$pth->{'dx'},$pth->{'dy'},
               $pth->{'pname'}, "black");
         placetext($pth->{'dx'}+1, $pth->{'dy'}, $zone_num . ":" . $pth->{'zone'});
      }
   }

   placetext($min_x*45, $min_y*45-20, $zone_num . ":" . $zone_name .
         " layer: " .  $i);

   print MAPFILE "</svg>\n";
   close MAPFILE;
}#for()
