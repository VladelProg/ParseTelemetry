#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Парсер файлов целей");

    parseMng = new ParseManager();
    parseMng->moveToThread(&parse_thread);
    //parseMng->moveToThread(&snapshot_thread);


    connect(this, &MainWindow::sigInit, parseMng, &ParseManager::init);
    connect(parseMng, &ParseManager::setMaxProgressBar, this, &MainWindow::SlotsetMaxProgressBar);
    connect(parseMng, &ParseManager::SetNowStatus, this, &MainWindow::SlotSetNowValue);
    connect(parseMng, &ParseManager::SigCorruptedData, this, &MainWindow::SlotCorruptedData);
    //connect(&parse_thread, &QThread::started, parseMng, &ParseManager::init);
    connect(&parse_thread, &QThread::started, this, [this]{
        ui->statusLabel->setText(QString("Парсинг файла %1").arg(baseFileName));

        if (action ==  ActionType::PARSING)         {parseMng->init
                /*        emit sigInit*/(ActionType::PARSING, ui->lineEdit->text(), ui->savelineEdit->text());}
        if (action == ActionType::SHAPSHOT)
        {
            parseMng->generateSnapshot(ui->lineEdit->text(), ui->savelineEdit->text(), startRecordTime, ui->deltaLine->text().toInt(), ui->startSnapshot->text().toInt() * 10e9, ui->endSnapshot->text().toInt() * 10e9);
        }
     });

    connect(&snapshot_thread, &QThread::started, this, [this]{
        //ui->statusLabel->setText(QString("Парсинг файла %1").arg(baseFileName));

        //emit SiGenerateSnapshot(startRecordTime, ui->startSnapshot->text().toInt(), ui->endSnapshot->text().toInt());
    });

    //connect(&parse_thread, &QThread::finished, parseMng, &ParseManager::deleteLater);

    // Для снапшота
    connect(parseMng, &ParseManager::SigStartRecordTime, this, QOverload<int64_t>::of(&MainWindow::SlotStartRecordTime));
    //connect(ui->generateSnapshot, &QPushButton::clicked, this, &MainWindow::on_generateSnapshot_clicked);
    //connect(this, &MainWindow::SiGenerateSnapshot, parseMng, &ParseManager::generateSnapshot);


    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);

    ui->tabWidget->setTabVisible(1, false);
    ui->tabWidget->setTabText(0, "Парсинг");
    //ui->tabWidget->setTabText(1, "Снапшот");
}

void MainWindow::on_generateSnapshot_clicked()
{
//    if (ui->startSnapshot->text().isEmpty())
//    {
//        QMessageBox::warning(this, tr("Ошибка"),
//                                        QString("Введите время начала снапшота"),
//                                        QMessageBox::Ok);
//    }

//    else if (ui->endSnapshot->text().isEmpty())
//    {
//        QMessageBox::warning(this, tr("Ошибка"),
//                             QString("Введите время конца снапшота"),
//                             QMessageBox::Ok);
//    }

//    else
//    {
        action = ActionType::SHAPSHOT;
        parse_thread.start();
//    }

    //parse_thread.start();
}

void MainWindow::SlotStartRecordTime(int64_t time)
{
    startRecordTime = time;
    ui->startLabel->setText("Начало работы в " + QString::number(time) + " секунд");
}

// Возможно и так
void MainWindow::SlotStartRecordTime(double time)
{
    startRecordTime = time;
    ui->startLabel->setText("Начало работы в " + QString::number(time) + " секунд");
}

void MainWindow::SlotCorruptedData()
{
    QMessageBox::critical(this, tr("Кривой файл"),
                                    QString("Битые данные в формуляре целей (Network.<targets>"),
                                    QMessageBox::Ok);
}

void MainWindow::SlotsetMaxProgressBar(qint64 size)
{
    ui->progressBar->setMaximum(size);
}

void MainWindow::SlotSetNowValue(ActionType action, bool status, qint64 size)
{
    ui->progressBar->setValue(size);

    if (size == ui->progressBar->maximum())
    {
        if (action == ActionType::SHAPSHOT)
        {
            if (!status)
            {
                ui->statusLabel->setText(QString("Парсинг завершен"));
                QMessageBox::StandardButton reply = QMessageBox::information(this, tr("Успех"),
                                                                             QString("Парсинг завершен, идет сохранение результатов в %1").arg(ui->savelineEdit->text()),
                                                                             QMessageBox::Ok);
                ui->statusLabel->setText(QString("Идет формирование снапшота"));

                if (reply == QMessageBox::Ok){
                    qDebug() << "Идет формирование снапшота";
                }
            }
            else
            {
                parse_thread.quit();
            }
        }

        if (action == ActionType::PARSING)
        {
            if (!status)
            {
                ui->statusLabel->setText(QString("Парсинг завершен"));
                QMessageBox::StandardButton reply = QMessageBox::information(this, tr("Успех"),
                                                                             QString("Парсинг завершен, идет сохранение результатов в %1").arg(ui->savelineEdit->text()),
                                                                             QMessageBox::Ok);
                ui->statusLabel->setText(QString("Сохранение результатов..."));

                if (reply == QMessageBox::Ok){
                    qDebug() << "Идет сохранение";
                }
            }

            else
            {
                ui->statusLabel->setText(QString("Результаты сохранены"));
                QMessageBox::StandardButton reply = QMessageBox::information(this, tr("Успех"),
                                                                             QString("Результаты сохранены в %1").arg(ui->savelineEdit->text()),
                                                                             QMessageBox::Ok);
                parse_thread.quit();

                if (reply == QMessageBox::Ok){
                    qDebug() << "Результаты сохранены";
                }
            }
        }
    }
}


MainWindow::~MainWindow()
{
    delete parseMng;

    parse_thread.quit();
    parse_thread.wait();

//    snapshot_thread.quit();
//    snapshot_thread.wait();

    delete ui;

    disconnect(this, &MainWindow::sigInit, parseMng, &ParseManager::init);
    disconnect(parseMng, &ParseManager::setMaxProgressBar, this, &MainWindow::SlotsetMaxProgressBar);
    disconnect(parseMng, &ParseManager::SetNowStatus, this, &MainWindow::SlotSetNowValue);
    disconnect(parseMng, &ParseManager::SigCorruptedData, this, &MainWindow::SlotCorruptedData);
}

void MainWindow::on_saveFolderButton_clicked()
{
    QString getName = QFileDialog::getExistingDirectory(this, "Выбор папки", path_, QFileDialog::ShowDirsOnly);

    ui->savelineEdit->setText(getName + "/" + baseFileName);
}

void MainWindow::on_startParse_clicked()
{
    if (ui->lineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Ошибка"),
                                        QString("Выберите файл для парсинга"),
                                        QMessageBox::Ok);
    }

    else if (ui->savelineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Ошибка"),
                                        QString("Выберите папку для сохранения"),
                                        QMessageBox::Ok);
    }

    else
    {
        action = ActionType::PARSING;
        parse_thread.start();
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString getName = QFileDialog::getOpenFileName(this, "Выбор файла", "", "*.dat");
//    QStringList getNames = QFileDialog::getOpenFileNames(this, "Выбор файла", "", "*.dat");
//    QString namesList;

//    for (const QString& name : getNames)
//    {
//        namesList += (name + "; ");
//    } // Задел на сохранение листа файлов и одновременный парсинг сразу всех

    QFileInfo filePath(getName);

    path_ = filePath.canonicalPath();
    baseFileName = filePath.baseName();;

    ui->lineEdit->setText(getName);

    ui->progressBar->setMinimum(0);
    //ui->progressBar->setValue();
}
