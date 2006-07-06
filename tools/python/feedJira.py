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
Reads a configurable list of BC_BUGS, BC_IDEAS files and imports the issues
into a Jira (http://atlassian.com/) installation via. SOAP.
"""

# User tuneable ##############################################################

# Configure the username/password of the Jira account to use.
jira_user = 'mud'
jira_pass = 'tU0rinPb4m'

# This is discoverable in the Jira custom-field administration by
# "configuring" the room number custom field and observing the
# value in the customFieldId attribute of the GET URL.
custom_field_id_room_num = 10000

# URL to the Jira SOAP service. Don't forget to enable the service.
soap_url = 'http://bugs.scrymud.net/rpc/soap/jirasoapservice-v2?wsdl'

# sources is an array of tuples: ( Scry_World_Path, Jira_Project_ID )
sources = [ ( '/home/scrymud/muds/mud/grrmud/World', 'TST' ),
#            ( '/home/scrymud/muds/mud_dev/grrmud/World', 'TST'),
          ]

# Map ScryMUD(left-hand) status to Jira(right-hand) status
# This isn't used at the moment. I'm leaving it here, commented out
# for my own sanity in the near future.
#status_map = { 0:1, # open       -> open
#               1:1, # assigned   -> open
#               2:5, # retest     -> resolved
#               3:6, # closed     -> closed
#               4:1, # deferred   -> open
#               5:1, # all        -> open (have no idea what all means)
#               6:1, # none       -> open (have no idea what none means)
#               7:1, # high_state -> open (have no idea what high_state means)
#             }

# End user tuneable ##########################################################

# Notes on Jira-Enterprise defaults (I think they're default anyway )#########
# Issue-Types:
#     1: Bug
#     2: New Feature
#     3: Task
#     4: Improvement
#
# Priorities:
#     1: Blocker
#     2: Critical
#     3: Major
#     4: Minor
#     5: Trivial
#
# Resolutions:
#     1: Fixed
#     2: Won't Fix
#     3: Duplicate
#     4: Incomplete
#     5: Cannot Reproduce
#
# Statuses:
#     1: Open
#     2:
#     3: In Progress
#     4: Reopened
#     5: Resolved
#     6: Closed
##############################################################################

import SOAPpy, getpass, datetime
from scrymud.bugs import *

room_num_id = "customfield_%d" %(custom_field_id_room_num)

soap = SOAPpy.WSDL.Proxy(soap_url)
soap_auth = soap.login(jira_user, jira_pass)

for (bugPath, project) in sources:

    # This temporary list is probably overkill ;)
    bugFiles = list()
    bugFiles.append(bugPath + '/BC_BUGS')
    bugFiles.append(bugPath + '/BC_IDEAS')

    # For each "source" handle both "BC_BUGS" and "BC_IDEAS"
    for bugFile in bugFiles:
        myBugs = bugList(bugFile)

        # Set the type of issue based on the source file.
        if ( bugFile[-7:] == 'BC_BUGS' ):
            bugType = 1
        elif ( bugFile[-8:] == 'BC_IDEAS' ):
            bugType = 4
        else:
            raise ValueError, "unexpected bugFile; not BC_BUGS or BC_IDEAS"

        for bug in myBugs:
            new_bug = soap.createIssue(soap_auth, {
                                                   'project': project,
                                                   'type'   : str(bugType),
                                                   'summary': bug.summary,
                                                  } )
            # Add the bug comments, if applicable.
            for comment in bug.comments:
                soap.addComment(soap_auth, new_bug['key'], { 'body': "[%s]\n%s" %(comment.reporter, comment.report) })

            # Set the room number and status.
            soap.updateIssue(soap_auth, new_bug['key'], [ {'id':room_num_id, 'values':[str(bug.roomNum)] },
                                                          {'id':'description', 'values':[ "[%s]\n%s" %(bug.reporter, bug.summary) ] },
                                                          {'id':'issuetype', 'values':[str(bugType)] },
                                                          {'id':'priority', 'values':['4'] },
                                                        ] )


soap.logout(soap_auth)
