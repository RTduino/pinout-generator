#ifndef ADD_H
#define ADD_H

#include <QWidget>
#include <QString>
#include <QStringList>

namespace Ui {
class add;
}

class add : public QWidget
{
    Q_OBJECT

public:
    explicit add(QWidget *parent = nullptr);
    ~add();

    void display_add_edit(bool name, bool channel);

    void set_widget_info(QStringList &list);

private slots:
    void on_addbtn_clicked();

    void on_cancebtn_clicked();

    void on_funcbox_currentIndexChanged(const QString &arg1);

private:
    Ui::add *ui;

signals:
    void send_add_data(QStringList &pininfolist);
};

#endif // ADD_H
