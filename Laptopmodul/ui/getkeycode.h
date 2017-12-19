#ifndef GETKEYCODE_H
#define GETKEYCODE_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtGui/QKeyEvent>
#include <QtSerialPort/QSerialport>
#include <QObject>
#include <QTimer>
#include <QPainter>

class getKeyCode : public QWidget
{
    Q_OBJECT

    public:
        getKeyCode(QWidget *parent = 0);
        ~getKeyCode();
        void setupUi();

    protected:
        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);
        void paintEvent(QPaintEvent *);

    private:

        QLabel *centerLabel;
        QLabel *klabel;
        QLabel *sensorfram;
        QLabel *sensorbak;
        QLabel *sensorvl;
        QLabel *sensorvs;
        QLabel *sensorhf;
        QLabel *sensorhb;
        QLabel *sensorh;
        QLabel *pwmleft;
        QLabel *pwmright;
        QLabel *sensorprog;
        QLabel *sensorfel;
        QLabel *styrprog;
        QLabel *styrfel;
        QLabel *comprog;
        QLabel *comfel;
        QLabel *kdlabel;
        QLabel *krlabel;
        QLabel *robotlabel;
        QLabel *vagglabel;
        QLabel *autoswitch;
        QLabel *connectedlabel;
        QLabel *directionlabel;
        QSerialPort serialport;
        int k = 30;
        int kr = 0;
        int kd = 30;
        bool keys[8] = {0,0,0,1,0,0,0,0};
        QLabel *send;
        bool k_changed = false;
        bool kd_changed = false;
        bool kr_changed = false;
        bool first = true;
        int sens_count = 0;
        quint16 lidar = 0;
        int direction = 0;
        bool driven[25][50] = {{false}};

        bool matrix[25][50] = {{false}}; // Y-X
        int xcount = 0;
        int ycount = 0;
        int robo_y = 0;
        int robo_x = 25;
        int old_robo_y = 0;
        int old_robo_x = 25;
        int rotated_left = 0;

public slots://interrupt-funktioner
        void readData();
};

#endif // GETKEYCODE_H
