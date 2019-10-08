#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lexer.h"

#include <QFile>
#include <QtWidgets>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(800, 800);
    this->setFixedSize(800, 800);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnFile_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, "Open file", QDir::homePath());
    ui->lineEdit->setText(fileName);
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "WARNING", "File not open");
    }
    QTextStream in(&file);
    QString textFromFile = in.readAll();
    ui->textFile->setText(textFromFile);
    file.close();
}

void MainWindow::on_btnStart_clicked()
{
    Lexer lexer(fileName);
    std::vector<Token> tokenList = lexer.getTokenList();

    ui->tableLex->setColumnCount(3);
    ui->tableLex->setRowCount(tokenList.size());
    ui->tableLex->setHorizontalHeaderLabels(QStringList() << "Line" << "Lexeme" << "Class");

    for (int i = 0; i < tokenList.size(); ++i)
    {
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(tokenList[i].lineNumber));
        item1->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *item2 = new QTableWidgetItem(QString::fromUtf8(tokenList[i].lexeme.c_str()));
        item2->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *item3 = new QTableWidgetItem(QString::fromUtf8(tokenList[i].tokenClass.c_str()));
        item3->setTextAlignment(Qt::AlignCenter);

        ui->tableLex->setItem(i, 0, item1);
        ui->tableLex->setItem(i, 1, item2);
        ui->tableLex->setItem(i, 2, item3);
    }
}


void MainWindow::on_btnClear_clicked()
{
    fileName.clear();
    ui->lineEdit->clear();
    ui->tableLex->clear();
    ui->textFile->clear();
}
