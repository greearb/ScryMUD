# Makefile for Hegemon project

# Dependiencias por mano:  javac no funciona bien

CUR_DATE=`date '+%y.%m.%d %h:%m:%s'`

JHOME=/usr/local/jdk
JBIN=${JHOME}/bin
JAVAC=${JBIN}/javac
JAR=${JBIN}/jar

CLASSPATH=${JHOME}/lib/classes.zip:./

%.class : %.java
	${JAVAC} -classpath ${CLASSPATH} -deprecation $<

OBJS=hegemon.class \
Action.class \
ActionDialog.class \
ActionFrame.class \
Alias.class \
AliasDialog.class \
AliasFrame.class \
BuildInfo.class \
BIStream.class \
BOStream.class \
ClientDisplay.class \
ColorChoicesFrame.class \
ColorNameException.class \
ColorSelectionManager.class \
ColorUtils.class \
CommandHistory.class \
ContextColorPair.class \
Context.class \
ConnectionManager.class \
HegemonDisplay.class \
HegemonDisplayProperties.class \
HegemonInputFilter.class \
HelpFrame.class \
HelpIndex.class \
ScrollComponentGraph.class \
ScrollComponentVector.class \
HegemonScrollCanvas.class \
HegemonScroll.class \
HegemonManager.class \
HostAddr.class \
HostDialog.class \
InputArea.class \
KeywordEditor.class \
LabeledCheckBoxGlob.class \
LabeledChoice.class \
LabeledTextArea.class \
LabeledTextBoxGlob.class \
LabeledTextField.class \
Log.class \
LogFrame.class \
MessageDialog.class \
MobScriptEditor.class \
NumCheckbox.class \
NumTextBox.class \
OLCDoor.class \
OLCMob.class \
OLCObject.class \
OLCRoom.class \
OlEditor.class \
OlcStore.class \
PathCellEditor.class \
PsoEditor.class \
Queue.class \
ScrollComponentQueue.class \
ScrollComponent.class \
ScrollComponentText.class \
ScrollComponentNewline.class \
SocketManager.class \
SocketReader.class \
SocketWriter.class \
StringUtils.class


SRCS=hegemon.java \
Action.java \
ActionDialog.java \
ActionFrame.java \
Alias.java \
AliasDialog.java \
AliasFrame.java \
BIStream.java \
BOStream.java \
ClientDisplay.java \
ColorChoicesFrame.java \
ColorNameException.java \
ColorSelectionManager.java \
ColorUtils.java \
CommandHistory.java \
ContextColorPair.java \
Context.java \
ConnectionManager.java \
HegemonDisplay.java \
HegemonDisplayProperties.java \
HegemonInputFilter.java \
HelpFrame.java \
HelpIndex.java \
ScrollComponentGraph.java \
ScrollComponentVector.java \
HegemonScrollCanvas.java \
HegemonScroll.java \
HegemonManager.java \
HostAddr.java \
HostDialog.java \
InputArea.java \
KeywordEditor.java \
LabeledCheckBoxGlob.java \
LabeledChoice.java \
LabeledTextArea.java \
LabeledTextBoxGlob.java \
LabeledTextField.java \
Log.java \
LogFrame.java \
MessageDialog.java \
MobScriptEditor.java \
NumCheckbox.java \
NumTextBox.java \
OLCDoor.java \
OLCMob.java \
OLCObject.java \
OLCRoom.java \
OlEditor.java \
OlcStore.java \
PathCellEditor.java \
PsoEditor.java \
Queue.java \
ScrollComponentQueue.java \
ScrollComponent.java \
ScrollComponentText.java \
ScrollComponentNewline.java \
SocketManager.java \
SocketReader.java \
SocketWriter.java \
StringUtils.java


release:  $(OBJS)
	rm -f ../hegemon/*.class
	cp *.class ../hegemon/


BuildInfo.java: $(SRCS) Makefile BuildInfo.prefix BuildInfo.suffix
	cat BuildInfo.prefix > BuildInfo.java
	echo "\"Built on:   `date` \\n\" +" >> BuildInfo.java
	echo "\"By:         `whoami` \\n\" +" >> BuildInfo.java
	echo "\"On machine: `uname -mrspn` \\n\" +" >> BuildInfo.java
	cat BuildInfo.suffix >> BuildInfo.java

all: tar jar release


heg.tar: $(OBJS)
	rm -f heg.tar
	rm -f heg.tar.gz
	tar -cvf heg.tar *.class
	gzip heg.tar

heg.jar: $(OBJS)
	rm -f heg.jar
	${JAR} -cvf heg.jar *.class

clean: 
	rm -f *.class

purge:  clean
	rm -f *~ *.flc
	rm -f heg.jar heg.tar heg.tar.gz