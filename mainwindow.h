#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "parsemanager.h"
#include <QMainWindow>
#include <QThread>
#include "structs.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ParseManager* parseMng;
    QThread parse_thread;
    QThread snapshot_thread;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString baseFileName;
    QString path_;
    ActionType action;

    int64_t startRecordTime;
    //Services::DataService* _dataService = nullptr;
public slots:
    void on_pushButton_clicked();
    void on_startParse_clicked();
    void on_saveFolderButton_clicked();
    void SlotsetMaxProgressBar(qint64 size);
    void SlotSetNowValue(ActionType action, bool status, qint64 size);
    void SlotCorruptedData();

    // Для снапшота
    void SlotStartRecordTime(int64_t time);
    void SlotStartRecordTime(double time);
    void on_generateSnapshot_clicked();

signals:
    void sigInit(ActionType action, QString name, QString saveFolder);
    void SiGenerateSnapshot(int64_t startRecordTime, int64_t startSnapshotTime, int64_t endSnapShotTime);

public:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
