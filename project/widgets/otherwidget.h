#ifndef OTHERWIDGET_H
#define OTHERWIDGET_H

#include <QLineEdit>
#include <QWidget>
#include <rtduinoconfig.h>

class HotkeyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    HotkeyLineEdit(QWidget *parent = nullptr);
    ~HotkeyLineEdit();

protected:
    virtual void keyPressEvent(QKeyEvent *);
};

namespace Ui {
class OtherWidget;
}

class OtherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OtherWidget(QWidget *parent = nullptr);
    ~OtherWidget();

    void loadUi();
    void refreshUi();
    void quitUi();

private slots:
    void on_checkBox_delete_stateChanged(int arg1);

    void on_checkBox_change_stateChanged(int arg1);

    void on_checkBox_insert_stateChanged(int arg1);

    void on_checkBox_add_stateChanged(int arg1);

    void on_checkBox_update_stateChanged(int arg1);

private:
    Ui::OtherWidget *ui;
    RTduinoConfig *rtduino;
    bool support_openssl;
};

#endif // OTHERWIDGET_H
