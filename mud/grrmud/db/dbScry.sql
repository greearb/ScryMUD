# This is all being put together in MySql i will also
# translate/change types so it will go easily into msql
# when the main defs are finished.  
#
# If there is anything that needs to be changed please
# feel free to dig in.  As for the missing stuff, it will
# be taken care of as soon as possible.
#
# Aasen
#
# Database: dbScry
#--------------------------------------------------------
# Server version	3.22.25

#
# Table structure for table 'tblBanList'
#
CREATE TABLE tblBanList (
  BanID int(11) DEFAULT '0' NOT NULL,
  Site char(20) DEFAULT '' NOT NULL,
  Notes char(200) DEFAULT '' NOT NULL
);

#
# Dumping data for table 'tblBanList'
#


#
# Table structure for table 'tblCritter'
#
CREATE TABLE tblCritter (
  CritID int(11) DEFAULT '0' NOT NULL,
  Description text NOT NULL,
  Name varchar(50) DEFAULT '' NOT NULL,
  CritType int(11) DEFAULT '0' NOT NULL,
  PRIMARY KEY (CritID)
);

#
# Dumping data for table 'tblCritter'
#


#
# Table structure for table 'tblMudStart'
#
CREATE TABLE tblMudStart (
  ConfID int(11) DEFAULT '0' NOT NULL,
  ConfName char(100) DEFAULT '' NOT NULL,
  MaxObjNum int(11) DEFAULT '0' NOT NULL,
  MaxDoorNum int(11) DEFAULT '0' NOT NULL,
  MaxRoomNum int(11) DEFAULT '0' NOT NULL,
  MaxCritNum int(11) DEFAULT '0' NOT NULL,
  Year int(11) DEFAULT '0' NOT NULL,
  Day int(11) DEFAULT '0' NOT NULL,
  Hour int(11) DEFAULT '0' NOT NULL,
  BootTime int(11) DEFAULT '0' NOT NULL,
  DefaultPort int(11) DEFAULT '0' NOT NULL,
  MaxDescriptorsAvail int(11) DEFAULT '0' NOT NULL,
  MaxPlayers int(11) DEFAULT '0' NOT NULL,
  BootLoadModifier int(11) DEFAULT '0' NOT NULL,
  LoadModifier int(11) DEFAULT '0' NOT NULL,
  RegularLoadModifier int(11) DEFAULT '0' NOT NULL,
  OptUsec int(11) DEFAULT '0' NOT NULL,
  FirstRoom int(11) DEFAULT '0' NOT NULL,
  LastRoom int(11) DEFAULT '0' NOT NULL,
  ROD timestamp(14),
  LMD timestamp(14),
  PRIMARY KEY (ConfID)
);

#
# Dumping data for table 'tblMudStart'
#


#
# Table structure for table 'tblPC'
#
CREATE TABLE tblPC (
  PCID int(11) DEFAULT '0' NOT NULL,
  Name char(50) DEFAULT '' NOT NULL,
  password char(50) DEFAULT '' NOT NULL,
  birthDay int(11) DEFAULT '0' NOT NULL,
  birthYear int(11) DEFAULT '0' NOT NULL,
  rentYear int(11) DEFAULT '0' NOT NULL,
  rentDay int(11) DEFAULT '0' NOT NULL,
  linePerPage int(11) DEFAULT '0' NOT NULL,
  age int(11) DEFAULT '0' NOT NULL,
  thirst int(11) DEFAULT '0' NOT NULL,
  hunger int(11) DEFAULT '0' NOT NULL,
  drugged int(11) DEFAULT '0' NOT NULL,
  prompt int(11) DEFAULT '0' NOT NULL,
  poofin char(255) DEFAULT '' NOT NULL,
  poofout char(255) DEFAULT '' NOT NULL,
  pkCount int(11) DEFAULT '0' NOT NULL,
  diedCount int(11) DEFAULT '0' NOT NULL,
  questPoints int(11) DEFAULT '0' NOT NULL,
  lastLoginTime int(11) DEFAULT '0' NOT NULL,
  canBeBeeped int(11) DEFAULT '0' NOT NULL,
  totalTimeOnline int(11) DEFAULT '0' NOT NULL,
  PRIMARY KEY (PCID)
);

#
# Dumping data for table 'tblPC'
#


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
# Dumping data for table 'tblZFlags'
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
# Dumping data for table 'tblZone'
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
# Dumping data for table 'tblxOwner'
#


