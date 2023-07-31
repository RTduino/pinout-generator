#ifndef PROJWIDGET_H
#define PROJWIDGET_H

#include <QWidget>

namespace Ui {
class ProjWidget;
}

class ProjWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProjWidget(QWidget *parent = nullptr);
    ~ProjWidget();

    QString getFileName(void);
    QString getFilePath(void);

    // page生命周期函数
    void loadUi();
    void refreshUi();
    void quitUi();
signals:

    void projSave(void);
    void projOpen(QString &file);

private slots:
    void on_comboBox_mcuserias_currentIndexChanged(const QString &arg1);

    void on_pushButton_save_clicked();

    void on_pushButton_path_clicked();

    void on_pushButton_open_clicked();

    void on_pushButton_image_clicked();

private:
    Ui::ProjWidget *ui;
    QString xmlfile;

    QString xmlfilePath;
    QString xmlfileName;
};

#endif // PROJWIDGET_H
