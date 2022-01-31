#include <thread>
#include <chrono>
#include <QTest>
#include <QPlainTextEdit>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>

#include "virtual_machine_gui.hpp"
#include "test_config.hpp"

class VirtualMachineGUITest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();
  void testThreading();
  void testBadFilename();
  void testParseError();
  void testLabels();

  // TODO: implement additional tests here
  
private:

  VirtualMachineGUI widget;
};

// this section just verifies the object names and API
void VirtualMachineGUITest::initTestCase(){

  widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test00.asm"));

  auto textWidget = widget.findChild<QPlainTextEdit *>("text");
  QVERIFY2(textWidget, "Could not find QPLainText widget");

  auto registerViewWidget = widget.findChild<QTableView *>("registers");
  QVERIFY2(registerViewWidget, "Could not find QTableView widget for registers");

  auto memoryViewWidget = widget.findChild<QTableView *>("memory");
  QVERIFY2(memoryViewWidget, "Could not find QTableView widget for memory");

  auto statusViewWidget = widget.findChild<QLineEdit *>("status");
  QVERIFY2(statusViewWidget, "Could not find QLineEdit widget for status");

  auto stepButtonWidget = widget.findChild<QPushButton *>("step");
  QVERIFY2(stepButtonWidget, "Could not find QTableView widget for memory");
}

void VirtualMachineGUITest::testThreading()
{
	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test00.asm"));
	QTest::mouseClick(widget.getRunButton(), Qt::LeftButton);
	QVERIFY(widget.getBreakButton()->isEnabled());
	QTest::mouseClick(widget.getBreakButton(), Qt::LeftButton);
	QCOMPARE(widget.getRegValues()[0]->text(), QString::fromStdString("0x00000000"));
}

void VirtualMachineGUITest::testBadFilename()
{
	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test00.as"));
	QCOMPARE(widget.getStatus()->text(), QString::fromStdString("Error: File could not be opened."));
}

void VirtualMachineGUITest::testParseError()
{
	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/testParseError.asm"));
	QCOMPARE(widget.getStatus()->text(), QString::fromStdString("Error: Invalid Assembly Format (Parsing Error)"));
}

void VirtualMachineGUITest::testLabels()
{
	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test01.asm"));
	QTest::mouseClick(widget.getStepButton(), Qt::LeftButton);
	QTest::mouseClick(widget.getRunButton(), Qt::LeftButton);
	QVERIFY(widget.getBreakButton()->isEnabled());
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	QTest::mouseClick(widget.getBreakButton(), Qt::LeftButton);
	QCOMPARE(widget.getRegValues()[13]->text(), QString::fromStdString("0x00000001"));
	QVERIFY(widget.getStatus()->text() == "Program is finished running.");
}

QTEST_MAIN(VirtualMachineGUITest)
#include "virtual_machine_gui_test.moc"

