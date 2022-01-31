#ifndef VIRTUAL_MACHINE_GUI_HPP
#define VIRTUAL_MACHINE_GUI_HPP

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <QString>
#include <QWidget>
#include <QtWidgets>
#include "VMModel.hpp"

#define NUMROWS 35

// TODO define the GUI class

class VirtualMachineGUI: public QWidget{
	Q_OBJECT

public:
	VMModel vm;
	int pcIndex;
	void load(QString str);

	VirtualMachineGUI();
	~VirtualMachineGUI();
	void drawRegNames();
	void update();
	void stepOnce();
	void initRegNames();
	void runThread();

	//testing
	QLineEdit* getStatus() { return Status; }
	QPushButton* getStepButton() { return StepButton; }
	QPushButton* getRunButton() { return RunButton; }
	QPushButton* getBreakButton() { return BreakButton; }
	QStandardItem** getRegValues() { return regValues; }
	QStandardItemModel* getMemModel() { return memModel; }

	public slots:
	void stepHandler();
	void runHandler();
	void breakHandler();

private:
	QPlainTextEdit *A;
	QTableView *B;
	QTableView *C;
	QLineEdit *Status;
	QLabel *StatusLabel;
	QPushButton *StepButton;
	QPushButton *RunButton;
	QPushButton *BreakButton;

	QStandardItemModel *regModel;
	QStandardItemModel *memModel;

	QStandardItem* regValues[NUMROWS];
	QStandardItem** memValues;

	std::vector<std::string> regNames;

	//threading
	std::thread th1;
	bool run;
};


#endif
