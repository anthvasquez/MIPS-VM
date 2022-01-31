#include <iostream>
#include <iomanip>
#include <sstream>
#include <QtWidgets>
#include <QDebug>
#include <cmath>
#include "virtual_machine_gui.hpp"

// CONTROLLER AND VIEW

void VirtualMachineGUI::load(QString str)
{
	/*QStandardItemModel *model = new QStandardItemModel(4, 4);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
			model->setItem(row, column, item);
		}
	}*/
	VMModel vmm(str.toStdString());
	vm = vmm;

	if (vmm.status == 1)
	{
		Status->setText(QString::fromStdString("Error: File could not be opened."));
		return;
	}
	else if (vmm.status == 2)
	{
		Status->setText(QString::fromStdString("Error: Invalid Assembly Format (Parsing Error)"));
		return;
	}

	/* Convert infile to a big long string (char*) */
	std::ifstream fileText(str.toStdString());
	fileText.seekg(0, std::ios::end);
	int length = fileText.tellg();
	fileText.seekg(0, std::ios::beg);
	char* buffer = new char[length];
	fileText.read(buffer, length);
	fileText.close();
	A->setPlainText(QString::fromStdString(buffer));
	delete buffer;	

	StepButton->setDisabled(false);
	RunButton->setDisabled(false);
	QStandardItem** memLabels = new QStandardItem*[vm.virtualMem.size()];
	memValues = new QStandardItem*[vm.virtualMem.size()];
	stringstream response;
	for (size_t i = 0; i < vm.virtualMem.size(); i++)
	{
		response << "0x" << std::setfill('0') << std::setw(8) << std::hex << i;
		memLabels[i] = new QStandardItem(QString::fromStdString(response.str()));
		memValues[i] = new QStandardItem();
		memLabels[i]->setTextAlignment(Qt::AlignCenter);
		memValues[i]->setTextAlignment(Qt::AlignCenter);
		memModel->setItem(i, 0, memLabels[i]);
		response.str(std::string());
		response.clear();
	}

	update();
}

VirtualMachineGUI::VirtualMachineGUI() : pcIndex(0), run(false)
{
	QGridLayout *topLevelContainer = new QGridLayout();

	A = new QPlainTextEdit();
	A->setReadOnly(true);
	A->setObjectName("text");

	B = new QTableView();
	B->setObjectName("registers");
	B->verticalHeader()->hide();

	C = new QTableView();
	C->setObjectName("memory");
	C->verticalHeader()->hide();

	StatusLabel = new QLabel("Status");
	StatusLabel->setAlignment(Qt::AlignCenter);

	Status = new QLineEdit();
	Status->setObjectName("status");
	Status->setReadOnly(true);
	
	StepButton = new QPushButton("Step");
	StepButton->setObjectName("step");
	StepButton->setDisabled(true);

	RunButton = new QPushButton("Run");
	RunButton->setObjectName("run");
	RunButton->setDisabled(true);

	BreakButton = new QPushButton("Break");
	BreakButton->setObjectName("break");
	BreakButton->setDisabled(true);

	topLevelContainer->addWidget(A, 0, 0, 1, 3);
	topLevelContainer->addWidget(B, 0, 3, 1, 3);
	topLevelContainer->addWidget(C, 0, 6, 1, 3);
	topLevelContainer->addWidget(StatusLabel, 1, 0);
	topLevelContainer->addWidget(Status, 1, 1, 1, 8);
	topLevelContainer->addWidget(StepButton, 2, 0, 1, 3);
	topLevelContainer->addWidget(RunButton, 2, 3, 1, 3);
	topLevelContainer->addWidget(BreakButton, 2, 6, 1, 3);

	regModel = new QStandardItemModel();// (NUMREGISTERS, 3);
	QStringList regHeaders({ "Number", "Alias", "Value (Hex)" });
	regModel->setHorizontalHeaderLabels(regHeaders);
	B->setModel(regModel);

	memModel = new QStandardItemModel();
	QStringList memHeaders({ "Address (Hex)", "Value (Hex)" });
	memModel->setHorizontalHeaderLabels(memHeaders);
	C->setModel(memModel);

	initRegNames();
	for (size_t i = 0; i < NUMROWS; i++)
	{
		regValues[i] = new QStandardItem();
		regValues[i]->setTextAlignment(Qt::AlignCenter);
	}
	drawRegNames();

	connect(StepButton, SIGNAL(released()), this, SLOT(stepHandler()));
	connect(RunButton, SIGNAL(released()), this, SLOT(runHandler()));
	connect(BreakButton, SIGNAL(released()), this, SLOT(breakHandler()));

	// prepare to show
	setLayout(topLevelContainer);
	setWindowTitle("MIPS Virtual Machine");
}

VirtualMachineGUI::~VirtualMachineGUI()
{
	delete A;
	delete B;
	delete C;
	delete Status;
	delete StatusLabel;
	delete StepButton;
	delete RunButton;
	delete BreakButton;
	delete regModel;
	delete memModel;
}

void VirtualMachineGUI::stepHandler()
{
	stepOnce();
	update();
}

void VirtualMachineGUI::stepOnce()
{
	if ((uint16_t)(pcIndex) >= vm.pc.size())
	{
		Status->setText("Program is finished running.");
		//run = false;
		StepButton->setDisabled(true);
		RunButton->setDisabled(true);
		//BreakButton->setDisabled(true);
		return;
	}
	Instruction current = vm.pc.at(pcIndex);
	int jump = vm.execWrapper(current);
	if (jump != -1)
		pcIndex = jump;
	else
		pcIndex++;
}

void VirtualMachineGUI::runHandler()
{
	StepButton->setDisabled(true);
	RunButton->setDisabled(true);
	BreakButton->setDisabled(false);
	B->setDisabled(true);
	C->setDisabled(true);
	run = true;
	th1 = std::thread(&VirtualMachineGUI::runThread, this);
}

void VirtualMachineGUI::breakHandler()
{
	//std::unique_lock<std::mutex> lock(mutex);
	run = false;
	//lock.unlock();
	if(th1.joinable())
		th1.join();
	B->setDisabled(false);
	C->setDisabled(false);
	update();
	BreakButton->setDisabled(true);
	RunButton->setDisabled(false);
	StepButton->setDisabled(false);
}

void VirtualMachineGUI::runThread()
{
		while (run)
		{
			stepOnce();
		}
}

/**
* @precondition: load() must have been called so that
*					memValues is initialized
*/
void VirtualMachineGUI::update()
{
	stringstream response;
	response << "0x" << std::setfill('0') << std::setw(8) << std::hex << pcIndex;
	regValues[0]->setText(QString::fromStdString(response.str()));
	regModel->setItem(0, 2, regValues[0]);
	response.str(std::string());
	response.clear();
	for (int i = 1; i < NUMROWS; i++)
	{
		response << "0x" << std::setfill('0') << std::setw(8) << std::hex << vm.regValues[regNames[i]];
		regValues[i]->setText(QString::fromStdString(response.str()));
		regModel->setItem(i, 2, regValues[i]);
		response.str(std::string());
		response.clear();
	}

	for (size_t i = 0; i < vm.virtualMem.size(); i++)
	{
		response << "0x" << std::setfill('0') << std::setw(8) << std::hex << (vm.virtualMem[i] & 0xFF);
		memValues[i]->setText(QString::fromStdString(response.str()));
		memModel->setItem(i, 1, memValues[i]);
		response.str(std::string());
		response.clear();
	}

	if ((uint16_t)(pcIndex) >= vm.pc.size())
		Status->setText("Program is finished running.");
	else
		Status->setText("Ok");
}

void VirtualMachineGUI::drawRegNames()
{
	QStandardItem* numericalRegs[NUMREGISTERS-2];	//take out hi and lo
	for (int i = 0; i < NUMREGISTERS-2; i++)
	{
		numericalRegs[i] = new QStandardItem(QString("$%0").arg(i));
		numericalRegs[i]->setTextAlignment(Qt::AlignCenter);
		regModel->setItem(i+3, 0, numericalRegs[i]);
	}

	QStandardItem* symbolicRegs[NUMROWS];	//non numeric
	for (size_t i = 0; i < regNames.size(); i++)
	{
		symbolicRegs[i] = new QStandardItem(QString::fromStdString(regNames[i]));
		symbolicRegs[i]->setTextAlignment(Qt::AlignCenter);
		regModel->setItem(i, 1, symbolicRegs[i]);
	}
}

void VirtualMachineGUI::initRegNames()
{
	regNames.push_back("$pc");
	regNames.push_back("$hi");
	regNames.push_back("$lo");
	regNames.push_back("$zero");
	regNames.push_back("$at");
	regNames.push_back("$v0");
	regNames.push_back("$v1");
	regNames.push_back("$a0");
	regNames.push_back("$a1");
	regNames.push_back("$a2");
	regNames.push_back("$a3");
	regNames.push_back("$t0");
	regNames.push_back("$t1");
	regNames.push_back("$t2");
	regNames.push_back("$t3");
	regNames.push_back("$t4");
	regNames.push_back("$t5");
	regNames.push_back("$t6");
	regNames.push_back("$t7");
	regNames.push_back("$s0");
	regNames.push_back("$s1");
	regNames.push_back("$s2");
	regNames.push_back("$s3");
	regNames.push_back("$s4");
	regNames.push_back("$s5");
	regNames.push_back("$s6");
	regNames.push_back("$s7");
	regNames.push_back("$t8");
	regNames.push_back("$t9");
	regNames.push_back("$k0");
	regNames.push_back("$k1");
	regNames.push_back("$gp");
	regNames.push_back("$sp");
	regNames.push_back("$fp");
	regNames.push_back("$ra");
}