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

release: $(OBJS) ../hegemon/data/help/hegemon.bml
	rm -f ../hegemon/*.class
	cp *.class ../hegemon/

../hegemon/data/help/hegemon.bml: $(OBJS) Makefile ../hegemon/data/help/hegemon.ORIG
	echo "<color red>" > ../hegemon/data/help/hegemon.bml
	echo "Built on: `date`" >> ../hegemon/data/help/hegemon.bml
	echo "By: `whoami`" >> ../hegemon/data/help/hegemon.bml
	echo "On machine: `uname -mrspn`" >> ../hegemon/data/help/hegemon.bml
	echo "</color><color blue>" >> ../hegemon/data/help/hegemon.bml

	cat ../hegemon/data/help/hegemon.ORIG >> ../hegemon/data/help/hegemon.bml


all: tar jar release


tar: $(OBJS) ../hegemon/data/help/hegemon.bml
	rm -f heg.tar
	rm -f heg.tar.gz
	tar -cvf heg.tar *.class
	gzip heg.tar

jar: $(OBJS) ../hegemon/data/help/hegemon.bml
	rm -f heg.jar
	${JAR} -cvf heg.jar *.class

clean: 
	rm -f *.class ../hegemon/data/help/hegemon.bml
