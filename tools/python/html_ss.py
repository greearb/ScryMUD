#!/usr/bin/env python
# $Id$
# Copyright (C) 2006, Edward Roper <edro+scrymud@wanfear.net>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

# I'm feeling lazy right now, so this is hard-coded.
source_dir = "/home/eroper/devel/ScryMUD/mud/grrmud/World"
dest_dir = "/home/eroper/devel/ScryMUD/tools/python/ss_help"


import sys, html_util
from scrymud import skills_spells, util
def mkContent(skill, skillcollection):
    ret_val = ""
    ret_val += "<div class=\"help_name\">%s</div>\n" %(skill.name)
    ret_val += "<p>Requires level: %d Mana cost: %d</p>\n" %(skill.min_level, skill.mana_cost)
    ret_val += "<div class=\"help_content\">"
    for p in util.paragraphs(skill.description):
        ret_val += "<p>%s</p>" %(p)
        ret_val += "</div>"
        ret_val += "<h3>Prerequisites</h3><div id=\"skill_prereqs\">"
        for id in skill.prereqs:
            name = skillcollection.getNum(int(id)).name.lower().replace(" ","_")
            ret_val += "<a href=\"/skills_spells/%s.html\">%s</a> " %(name,name)
        ret_val += "</div>"
        ret_val += "<h3>Enables</h3><div id=\"skill_enables\">"
        for id in skill.enables:
            name = skillcollection.getNum(int(id)).name.lower().replace(" ","_")
            ret_val += "<a href=\"/skills_spells/%s.html\">%s</a> " %(name,name)
        ret_val += "</div>"
    return(ret_val)

if ( __name__ == '__main__' ):
    try:
        config = html_util.cfg(sys.argv[1:])
    except html_util.cfg.cfgError, e:
        sys.stderr.write("Configuration error: %s\n" %(e))
        html_util.usage()
    Skills = skills_spells.SSCollection(config.source_dir)
    Skills.read()
    Pages = html_util.PageCollection(config.html_head,config.html_foot,config.output_dir)
    
    for skill in Skills:
        page = Pages.newPage()
        page.title = skill.name
        page.doc = mkContent(skill,Skills)
    Pages.writeIndex("index.html")
    Pages.writePages()
