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

"""
This program generates a Jelly script for importing existing BC_BUGS
and BC_IDEAS files into Jira. This is really only intended to be run
once, for an initial import. Subsequent updates should be performed
using feedJira.py which utilizes Jira's SOAP interface.
"""

# [ Configuration ] ##########################################################

jira_project_id = "TST"

sources = { "/home/scrymud/muds/mud/grrmud/World/BC_BUGS":"Bug",
            "/home/scrymud/muds/mud_dev/grrmud/World/BC_BUGS":"Bug",
            "/home/scrymud/muds/mud/grrmud/World/BC_IDEAS":"New Feature",
            "/home/scrymud/muds/mud_dev/grrmud/World/BC_IDEAS":"New Feature",
          }

##############################################################################

tplIssue = """
<jira:CreateIssue project-key="%s" issueType="%s" summary="%s" issueKeyVar="key" issueIdVar="id" priority="%s" created="%s" updated="%s" description="%s" reporter="mud">
<jira:AddCustomFieldValue id="customfield_10000" value="%d"/>
</jira:CreateIssue>
"""

tplComment = """<jira:AddComment comment="%s" issue-key="${key}" created="%s" reporter="mud"/>"""

tplWorkflow = """<jira:TransitionWorkflow key="${key}" workflowAction="%s" resolution="Fixed"/>"""

def cheezyEscape(input):
    """Because I didn't have much luck figuring out how to make the various
    python XML generate XML trees."""
    
    output = input.replace("&",'&amp;')
    output = output.replace('"','&quot;')
    output = output.replace('\n','&#10;')
    output = output.replace('<','&lt;')
    output = output.replace('<','&gt;')
    output = output.replace('\'','&apos;')
    output = output.replace('[','&#91;')
    output = output.replace(']','&#93;')
    return(output)


from scrymud.bugs import *

if ( __name__ == '__main__' ):
    print """<JiraJelly xmlns:jira="jelly:com.atlassian.jira.jelly.JiraTagLib">"""

    for (filePath, fileType) in sources.iteritems():
        myBugs = bugList(filePath)
        for bug in myBugs:

            if ( bug.summary == '' ):
                sys.stderr.write("WARNING %s:bug-%d has no summary.\n" %(filePath, bug.bugNum))
                bug.summary = "[ScryBUG:%s:%d][no summary available]" %(filePath, bug.bugNum)

            print tplIssue %( jira_project_id, fileType, cheezyEscape(bug.summary[:254]), "Minor", time.strftime("%Y-%m-%d %H:%M:%S.0",bug.date),
                              time.strftime("%Y-%m-%d %H:%M:%S.0",bug.getLastUpdate()), cheezyEscape("[%s]\n%s"%(bug.reporter.strip(), bug.summary)), bug.roomNum )
            for comment in bug.comments:
                print tplComment %( cheezyEscape("[%s]\n%s"%(comment.reporter.strip(), comment.report)), time.strftime("%Y-%m-%d %H:%M:%S.0",comment.date))

            if ( bug.state == 2 ):
                print tplWorkflow %("Resolve issue")
            elif ( bug.state == 3):
                print tplWorkflow %("Close issue")

    print """</JiraJelly>"""
