#ifndef PININFOUI_H
#define PININFOUI_H

#include <QWidget>
#include <QString>
#include <QStringList>

namespace Ui {
class PininfoUI;
}

class PininfoUI : public QWidget
{
    Q_OBJECT

public:
    explicit PininfoUI(QWidget *parent = nullptr);
    ~PininfoUI();

    void show_name_or_channel(bool nameshow,bool channelshow);
    void show_change_ui_info(QStringList &list);
    void show_insert_ui_info();
    void show_add_ui_info();

private slots:
    void on_addbtn_clicked();

    void on_cancebtn_clicked();

    void on_funcbox_currentIndexChanged(const QString &arg1);

    void on_rttpinedit_returnPressed();

    void on_devnameedit_returnPressed();

    void on_channeledit_returnPressed();

private:
    Ui::PininfoUI *ui;
    QString curpinstate;
signals:
    void send_pininfo_data(QStringList &pininfolist);
};

#endif // PININFOUI_H
