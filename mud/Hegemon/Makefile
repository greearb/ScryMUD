# Makefile for Hegemon project

# Dependiencias por mano:  javac no funciona bien

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
ScrollComponentVector.class \
HegemonScrollCanvas.class \
HegemonScroll.class \
HegemonManager.class \
HostAddr.class \
HostDialog.class \
InputArea.class \
KeywordEditor.class \
LabeledCheckBoxGlob.class \
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
Queue.class \
ScrollComponentQueue.class \
ScrollComponent.class \
ScrollComponentText.class \
ScrollComponentNewline.class \
SocketManager.class \
SocketReader.class \
SocketWriter.class \
StringUtils.class

release: $(OBJS)
	rm -f ../hegemon/*.class
	cp *.class ../hegemon/

all: tar jar release


tar: $(OBJS)
	rm -f heg.tar
	rm -f heg.tar.gz
	tar -cvf heg.tar *.class
	gzip heg.tar

jar: $(OBJS)
	rm -f heg.jar
	${JAR} -cvf heg.jar *.class

clean: 
	rm -f *.class
