# This is all being put together in MySql i will also
# translate/change types so it will go easily into msql
# shortly.  
#
# If there is anything that needs to be changed please
# feel free to dig in.  As for the missing stuff, it will
# be taken care of as soon as possible.
#
# Aasen
#
# Database: dbScry
#--------------------------------------------------------

#
# Table structure for table 'tblZFlags'
#
CREATE TABLE tblZFlags (
  ZoneID int(11) DEFAULT '0' NOT NULL,
  isEnabled tinyint(1) DEFAULT '0' NOT NULL,
  isTotallyLoaded tinyint(1) DEFAULT '0' NOT NULL,
  canWriteMobs tinyint(1) DEFAULT '0' NOT NULL,
  canWriteRooms tinyint(1) DEFAULT '0' NOT NULL,
  canWriteDoors tinyint(1) DEFAULT '0' NOT NULL,
  PRIMARY KEY (ZoneID)
);

#
# Some of the bit fields need to be changed i beleve to integer
# to make them actual low bit high bit insted of boolean.
#


#
# Table structure for table 'tblZone'
#
CREATE TABLE tblZone (
  ZoneID int(11) DEFAULT '0' NOT NULL,
  Name varchar(50) DEFAULT '' NOT NULL,
  Description varchar(150) DEFAULT '' NOT NULL,
  BeginRoom int(11) DEFAULT '0' NOT NULL,
  RegenTickCycle int(11) DEFAULT '0' NOT NULL,
  EndRoom int(11) DEFAULT '0' NOT NULL,
  ROD timestamp(14),
  LMD timestamp(14),
  PRIMARY KEY (ZoneID)
);

#
# Did i miss any of the key elements?
#


#
# Table structure for table 'tblxOwner'
#
CREATE TABLE tblxOwner (
  OwnerID int(11) DEFAULT '0' NOT NULL,
  ZoneID int(11) DEFAULT '0' NOT NULL,
  UNIQUE UniqueIdx (OwnerID,ZoneID)
);

#
# Just a cross table for linking Owner/Player as Owners of a zone
# This will be changed to reflect the ID field in the player tbl.
#


