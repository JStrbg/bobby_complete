#include <QtWidgets/QVBoxLayout>
#include "getkeycode.h"
#include <QtSerialPort/QSerialport>
#include <windows.h>
#include <iostream>
#include <QString>
#include <QThread>
#include <QGridLayout>
#include <QBrush>
getKeyCode::getKeyCode(QWidget *parent)
: QWidget(parent) //Konstruktor
{
    setupUi();
}

getKeyCode::~getKeyCode() //Destruktor
{
}

void getKeyCode::setupUi() //Initierar objekt och placerar i fönstret
{

    setWindowTitle("Kartrobot");

    this->setAutoFillBackground(true);
    this->setStyleSheet("background-color: black; color : white;"); //Svart bakgrund

    centerLabel = new QLabel("Welcöm ver 2.5");
    klabel = new QLabel("K: " + QString::number(k));
    kdlabel = new QLabel("Kd: " + QString::number(kd));
    krlabel = new QLabel("Kr: " + QString::number(kr)); //Finns inte längre

    sensorfram = new QLabel("Fram: ");
    sensorbak = new QLabel("Bak: ");
    sensorvl = new QLabel("Vänster lång: ");
    sensorvs = new QLabel("Vänster kort: ");
    sensorhf = new QLabel("Höger fram: ");
    sensorhb = new QLabel("Höger bak: ");
    sensorh = new QLabel("Höger: ");
    pwmleft = new QLabel("PWM Vänster: ");
    pwmright = new QLabel("PWM Höger: ");
    sensorprog = new QLabel("Sensormodul prog: ");
    sensorfel = new QLabel("Sensormodul fel: ");
    styrprog = new QLabel("Styrmodul prog: ");
    styrfel = new QLabel("Styrmodul fel: ");
    comprog = new QLabel("Kommunikationsmodul prog: ");
    comfel = new QLabel("Kommunikationsmodul fel: ");
    connectedlabel = new QLabel("Connection: [_]");
    directionlabel = new QLabel("Riktning: Framåt");

    send = new QLabel(tr("Skickas: "));
    autoswitch = new QLabel("Manuell: [X]");

    connect(&serialport, SIGNAL(readyRead()), this, SLOT(readData())); //enabel interrupt på recieve

    //3 vertikala boxar i en grid, variabelnamn på elementen i griden stämmer inte längre då de flyttades för att använda plats mer optimalt
    QGridLayout *grid = new QGridLayout;
    QVBoxLayout *recieveboxsensor = new QVBoxLayout;
    QVBoxLayout *recieveboxstyr = new QVBoxLayout;
    QVBoxLayout *sendbox = new QVBoxLayout;
    QVBoxLayout *map_expl = new QVBoxLayout;

    //kartförklaring
    robotlabel = new QLabel("Robot:");
    vagglabel = new QLabel("Vägg:");
    map_expl->addWidget(robotlabel);
    map_expl->addWidget(vagglabel);

    //skickas
    sendbox->addWidget(centerLabel);
    sendbox->addWidget(send);
    sendbox->addWidget(directionlabel);
    sendbox->addWidget(klabel);
    sendbox->addWidget(krlabel);
    sendbox->addWidget(kdlabel);


    //mottaget
    recieveboxsensor->addWidget(sensorfram);
    recieveboxsensor->addWidget(sensorbak);
    recieveboxsensor->addWidget(sensorhf);
    recieveboxsensor->addWidget(sensorhb);
    recieveboxsensor->addWidget(sensorh);
    recieveboxsensor->addWidget(sensorvs);
    recieveboxsensor->addWidget(sensorvl);
    recieveboxsensor->addWidget(pwmleft);
    recieveboxsensor->addWidget(pwmright);


    recieveboxstyr->addWidget(autoswitch); // låt autoswitchen synas här för det fanns plats
    recieveboxstyr->addWidget(connectedlabel);
    recieveboxstyr->addWidget((new QLabel("")));//en filler så det ser finare ut

    recieveboxstyr->addWidget(styrprog);
    recieveboxstyr->addWidget(styrfel);
    recieveboxstyr->addWidget(sensorprog);
    recieveboxstyr->addWidget(sensorfel);
    recieveboxstyr->addWidget(comprog);
    recieveboxstyr->addWidget(comfel);


    //placera på grid-layouten
    grid->addItem(sendbox,1,0); //row, column
    grid->addItem(recieveboxstyr,1,1);
    grid->addItem(recieveboxsensor,1,2);
    grid->addItem(map_expl,1,3);

    // Justera positioner på grid-layouten
    grid->setColumnMinimumWidth(0,160);
    grid->setColumnMinimumWidth(1,300);
    grid->setColumnMinimumWidth(2,150);

    grid->setRowStretch(0, 1);
    grid->setColumnStretch(4,1);

    setLayout(grid);
    resize(1920,1008); //fönsterstorlek
    setWindowState(Qt::WindowMaximized); //maximerar fönster ändå

    //Startficka
    matrix[0][24] = true;
    matrix[0][26] = true;

    this->setFocus();
}

void getKeyCode::paintEvent(QPaintEvent *) //Måla-ut-rutor-i-matris rutin, kan tvingas med update()
{

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::white);
    QRect rect = QRect(210,30,1500,750);
    painter.drawRect(rect); // måla kartgränser

    //Måla förklaring
    painter.setPen(Qt::black);
    rect = QRect(720,890,30,30);//x, -y
    painter.drawRect(rect);
    painter.fillRect(rect,QBrush(Qt::red));
    rect = QRect(720,975,30,30);
    painter.drawRect(rect);
    painter.fillRect(rect,QBrush(Qt::white));

    //måla väggar, robot och rutnät
    painter.setPen(Qt::white);
      for(int y = 0; y < 25; y++)
      {
          for(int x = 0; x < 50; x++)
          {
              rect = QRect(210+x*30,30+y*30,30,30); //(x,y,w,h) x-> y!^
              painter.drawRect(rect);
              if (matrix[24-y][x])
              {

                 painter.fillRect(rect, QBrush(Qt::white)); //fyll väggar
              }
              if ((24-y) == robo_y && x == robo_x) //Fyll i robot
                painter.fillRect(rect,QBrush(Qt::red));
          }
      }

}

void getKeyCode::readData()
{
    QByteArray rec;
    rec = serialport.readAll();

    if(keys[5]) // Ta emot robotens kartmatris, först y sen check sen x för varje vägg
    {
        if((quint8)rec[0] == 255) // klarsignal
        {
            update();
            keys[5] = 0;
            centerLabel->setText("Kartöverföring klar!");
        }
        else if(first)
        {
            ycount = (quint8)rec[0];
            first = false;
        }
        else if(!first)
        {
            xcount = (quint8)rec[0];
            matrix[ycount][xcount] = true;
            first = true;
        }
        if((quint8)rec[0] != 255)
        {
        update();
        QByteArray ready;
        ready[0] = (quint8)33; // vi valde detta bitmönster för att säga 'redo för ny'
        serialport.write(ready,1);
        serialport.waitForBytesWritten(20);
        }
    }
    else //Om vi inte bett om kartan, antar vi att sensorvärden och robotposition är vad som kommer
    {
        switch(sens_count)
        {
        case 0:
            sensorhf->setText("Högerfram: " + QString::number((quint8)rec[0]));
            break;
        case 1:
            sensorhb->setText("Högerbak: " + QString::number((quint8)rec[0]));
            break;
        case 2:
            sensorvs->setText("Vänster kort: " + QString::number((quint8)rec[0]));
            break;
        case 3:
            sensorbak->setText("Bak: " + QString::number((quint8)rec[0]));
            break;
        case 4:
            sensorh->setText("Höger: " + QString::number((quint8)rec[0]));
            break;
        case 5:
            sensorvl->setText("Vänster: " + QString::number((quint8)rec[0]));
            break;
        case 6:
            //lagra lägre delen av lidarn
            lidar = (quint8)rec[0];
            break;
        case 7:
            lidar = lidar + (quint8)rec[0]*256; //addera med övre delen
            sensorfram->setText("Fram: " + QString::number(lidar));
            break;
        case 8:
            pwmright->setText("PWM Höger: " + QString::number((quint8)rec[0]));
            break;
        case 9:
            pwmleft->setText("PWM Vänster : " + QString::number((quint8)rec[0]));
        case 10:
            sensorprog->setText("Sensormodul prog: " + QString::number((quint8)rec[0]));
            break;
        case 11:
            sensorfel->setText("Sensormodul fel: " + QString::number((quint8)rec[0]));
            break;
        case 12:
            styrprog->setText("Styrmodul prog: " + QString::number((quint8)rec[0]));
            break;
        case 13:
            centerLabel->setText(QString::number((quint8)rec[0])); // Denna är just nu en odefinierad intern signal hos kom-modulen
            break;
        case 14:
            styrfel->setText("Styrmodul fel: " + QString::number((quint8)rec[0]));

            break;
        case 15:
            comprog->setText("Kommunikationsmodul prog: " + QString::number((quint8)rec[0]));
            break;
        case 16:
            comfel->setText("Kommunikationsmodul fel: " +QString::number((quint8)rec[0]));
            break;
        case 17:
            if((quint8)rec[0] < 2)
                rotated_left = (quint8)rec[0];
            else
                centerLabel->setText("Felaktig rotation mottagen"); //felmeddelande
            break;
            //Här kommer robotens y sen x
        case 18:
            if((quint8)rec[0] < 25) //sanity checks
                robo_y = (quint8)rec[0];
            else
            {
                centerLabel->setText("Felaktig koordinat mottagen");
                robo_y = 0;
            }

            break;
        case 19:
            if((quint8)rec[0] < 50)
            {
                robo_x = (quint8)rec[0];
                if(robo_x == 25 && robo_y == 0)
                    state = 1;
                //tryck in väggar med logik, riktning och rotationssignal
                switch(direction)
                {
                case 0: //riktning fram
                    if(robo_x + 1 < 50)
                    {
                    if(!rotated_left){ // rot höger
                        for(int y = old_robo_y+1 ; y < robo_y; y++)
                        {
                            matrix[y][robo_x+1] = true; //fyll i väggar
                        }
                    }
                    else //rot vänster
                    {
                        for(int y = old_robo_y+1; y <= robo_y; y++)
                        {
                            matrix[y][robo_x+1] = true;
                        }
                        if(robo_y+1 < 25)
                            matrix[robo_y+1][robo_x] = true;
                    }
                    }
                    //Fyll i körväg
                    for(int y = old_robo_y;y <= robo_y; y++)
                    {
                        driven[y][robo_x] = true;
                    }


                    break;
                case 1: //riktning höger
                    if(robo_y-1 >= 0){
                    if(!rotated_left)
                    {
                        for(int x = old_robo_x + 1; x < robo_x; x++)
                        {
                            matrix[robo_y-1][x] = true;
                        }
                    }
                    else
                    {
                        for(int x = old_robo_x + 1; x <= robo_x;x++)
                        {
                            matrix[robo_y-1][x] = true;
                        }

                        matrix[robo_y][robo_x+1] = true;

                    }
                    }
                    for(int x = old_robo_x;x <= robo_x; x++)
                    {
                        driven[robo_y][x] = true;
                    }
                    break;
                case 2: //riktning bakåt
                    if(!rotated_left) //roterade höger
                    {
                        for(int y = robo_y+1; y < old_robo_y; y++)
                        {
                            matrix[y][robo_x-1] = true;
                        }
                    }
                    else //roterade vänster
                    {
                        for(int y = robo_y; y < old_robo_y; y++)
                        {
                            matrix[y][robo_x-1] = true;
                        }
                        if(robo_y-1>=0)
                            matrix[robo_y-1][robo_x] = true;
                    }
                    for(int y = robo_y;y <= old_robo_y; y++)
                    {
                        driven[y][robo_x] = true;
                    }
                    break;
                case 3: //riktning vänster
                    if(!rotated_left) //Roterade höger
                    {
                        for(int x = robo_x + 1; x < old_robo_x; x++ )
                        {
                            matrix[robo_y+1][x] = true;
                        }

                    }
                    else //roterade vänster
                    {
                        for(int x = robo_x; x < old_robo_x;x++)
                        {
                            matrix[robo_y+1][x] = true;
                        }
                        matrix[robo_y][robo_x-1] = true;
                    }
                    for(int x = robo_x;x <= old_robo_x; x++)
                    {
                        driven[robo_y][x] = true;
                    }
                    break;
                default:
                    centerLabel->setText("Karterror");
                    break;

                }

                old_robo_x = robo_x;
                old_robo_y = robo_y;
            }

            else
                centerLabel->setText("Felaktig koordinat mottagen");

            //Se till att körväg inte satts till vägg
            for(int x = 0; x<50;x++)
            {
                for(int y = 0; y<25;y++)
                {
                    if(driven[y][x])
                        matrix[y][x] = false;
                }
            }

            break;

        case 20: //Riktning
            direction = (quint8)rec[0];
            switch(direction)
            {
                case 0:
                directionlabel->setText("Riktning: Framåt");
                break;
            case 1:
                directionlabel->setText("Riktning: Höger");
                break;
            case 2:
                directionlabel->setText("Riktning: Bakåt");
                break;
            case 3:
                directionlabel->setText("Riktning: Vänster");
                break;
            default:
                directionlabel->setText("RIKTNING ERROR");
                break;

            }
            break;


        default: //skriv ut fel om något gått snett
            centerLabel->setText("fail mottagningsindex: " + QString::number((quint8)rec[0]));
            break;
        }

        sens_count++; //öka index så nästa byte hamnar rätt

            QByteArray ready;
            ready[0] = (quint8)33; //Skicka redo för ny byte signal
            serialport.write(ready,1);
            serialport.waitForBytesWritten(20); //väntar max 20ms

            if(sens_count > 20){ //Array mottagen, återställ räknare och tvinga ommålning
                sens_count = 0;
                update(); //Tvinga ommålning
            }
    }
}
void getKeyCode::keyPressEvent(QKeyEvent *event) //Triggas varje gång en tangentbordsknapp trycks
{
    switch(event->key()){
    case Qt::Key_1: //Kr är obsolete
        if (kr > 0)
        {
            kr--;
            kr_changed = true;
        }
        else
            return;
        break;
    case Qt::Key_2:
        if (kr < 63)
        {
            kr++;
            kr_changed = true;
        }
        else
            return;
        break;
    case Qt::Key_A: //Starta autonomt läge och rensa tidigare inställningar
        centerLabel->setText("Autonomt läge");
        autoswitch->setText("Manuell: [_]");
        for(int i = 1;i<8;i++)
        {
            keys[i] = 0;
        }
        keys[0] = 1;

        sens_count = 0;
        //rensa kartan och sätt x- y till start
        robo_x = 25;
        robo_y = 0;
        old_robo_x = 25;
        old_robo_y = 0;
        direction = 0;
        //reset direction
        directionlabel->setText("Riktning: Framåt");
        for(int x = 0; x < 50; x++)
        {
            for(int y = 0; y < 25; y++)
            {
                matrix[y][x] = false;
                driven[y][x] = false;
            }
        }
        state = 0;
        matrix[0][24] = true;
        matrix[0][26] = true;
        update();
        break;
    case Qt::Key_M: //Sätt manuellt läge
        centerLabel->setText("Manuellt läge");
        autoswitch->setText("Manuell: [X]");
        for(int i = 0;i<8;i++)
        {
            keys[i] = 0;
        }
        keys[4] = 1;
        sensortimer.stop();
        break;
    case Qt::Key_Down:
        keys[0] = 1;
        break;
    case Qt::Key_Right:
        keys[3] = 1;
        break;
    case Qt::Key_Left:
        keys[2] = 1;
        break;
    case Qt::Key_Up:
        keys[0] = 1;
        keys[1] = 1;
        break;
    case Qt::Key_U:
        centerLabel->setText("Överför karta");
        keys[5] = 1;
        sens_count = 0;
        keys[0] = 0;
        keys[4] = 1;
        for(int x = 0; x < 50; x++) //rensa matris
        {
            for(int y = 0; y < 25; y++)
            {
                matrix[y][x] = false;
                driven[y][x] = false;
            }
        }


        break;
    case Qt::Key_H:
        if (kd < 63)
        {
            kd++;
            kd_changed = true;
        }
        else
            return;
        break;
    case Qt::Key_G:
        if (kd > 0)
        {
            kd--;
            kd_changed = true;
        }
        else
            return;
        break;
    case Qt::Key_K:
        if (k < 63){
            k++;
            k_changed = true;
        }
        else
            return;
        break;
    case Qt::Key_J:
        if(k > 0){
            k--;
            k_changed = true;
        }
        else
            return;
        break;
    case Qt::Key_C:
        //Anslut
        if(!serialport.isOpen())
        {
           for(int i = 0;i<8;i++)
           {
               keys[i] = 0;
           }
           keys[4] = 1;
           //Inställningar för COM
        serialport.setPortName("COM6");
        serialport.setBaudRate(BAUD_115200);
        serialport.setDataBits(QSerialPort::Data8);
        serialport.setParity(QSerialPort::NoParity);
        serialport.setStopBits(QSerialPort::OneStop);
        serialport.setFlowControl(QSerialPort::HardwareControl);

        if(!serialport.open(QIODevice::ReadWrite)) //FÖrsök öppna com-porten
        {
           centerLabel->setText("UnCOMfirmed, connection failed"); //skriv ut om anslutningen misslyckades
        }
        else
        {
           centerLabel->setText("COMfirmed"); //SKrivs ut om anslutningen lyckades
           connectedlabel->setText("Connection: [X]");
        }
       }
        break;
    case Qt::Key_D:
        if(serialport.isOpen()) //Koppla ifrån
         {
               serialport.close();
               centerLabel->setText("COMpleted");
               connectedlabel->setText("Connection: [_]");
         }
        break;
    default:
            return;
        break;

    }
if(serialport.isOpen() && event->key() == Qt::Key_U)
{
    //skicka reset_sens_counter sync signal till kom-modul
    QByteArray fku;
    fku[0] = (quint8)255;
    serialport.write(fku,1);
    serialport.waitForBytesWritten(20);
    send->setText("Skickat: " + QString::number((quint8)fku[0],2));
}
    //uppdatera k
    if(serialport.isOpen() && k_changed && !kd_changed && !kr_changed)
    {
        klabel->setText("K: " + (QString::number(k)));
        QByteArray ss;
        ss[0] = 128 + k; //Bitmönster för ändring av K
        serialport.write(ss,1); //size = 1
        serialport.waitForBytesWritten(100); //väntar max 100ms eller tills det är färdigskrivet
        send->setText("Skickat: " + QString::number((quint8)ss[0],2));
        k_changed = false;
    }
    //uppdatera styrbyte
    else if(serialport.isOpen() && !k_changed && !kd_changed && !kr_changed)
    { //Skapa bitmönster och skicka styrbyte
        char s = keys[0] + keys[1]*2 + keys[2]*4 + keys[3]*8 + keys[4]*16 + keys[5]*32 + keys[6]*64 + keys[7]*128 + keys[8]*256; //Skapa bitmönster
        QByteArray ss;
        ss[0] = s;

        serialport.write(ss,1); //size = 1
        serialport.waitForBytesWritten(100); //Väntar max 100ms på att skrivningen ska bli klar
        send->setText("Skickat: " + QString::number((quint8)ss[0],2));
    }
    //uppdatera kd
    else if(serialport.isOpen() && !k_changed && kd_changed && !kr_changed)
    {
        kdlabel->setText("Kd: " + (QString::number(kd)));
        QByteArray ss;
        ss[0] = 64 + kd;
        serialport.write(ss,1); //size = 1
        serialport.waitForBytesWritten(100);
        send->setText("Skickat: " + QString::number((quint8)ss[0],2));
        kd_changed = false;
    }
    //uppdatera kr, används inte längre
    else if(serialport.isOpen() && !k_changed && !kd_changed && kr_changed)
    {
        krlabel->setText("Kr: " + (QString::number(kr)));
        QByteArray ss;
        ss[0] = 192 + kr;
        serialport.write(ss,1); //size = 1
        serialport.waitForBytesWritten(100); //väntar max 100ms eller tills det är färdigskrivet
        send->setText("Skickat: " + QString::number((quint8)ss[0],2));
        kr_changed = false;
    }
}

void getKeyCode::keyReleaseEvent(QKeyEvent *event) // Kallas på när en knapp släpps, funkar i övrigt som KeyPressEvent
{

    switch(event->key()){
        case Qt::Key_Down:
            keys[0] = 0;
            break;
        case Qt::Key_Right:
            keys[3] = 0;
            break;
        case Qt::Key_Left:
            keys[2] = 0;
            break;
        case Qt::Key_Up:
            keys[0] = 0;
            keys[1] = 0;

            break;
        default:
            return;
        break;
    }
    if(serialport.isOpen() && !k_changed && !kd_changed)
    {
        char s = keys[0] + keys[1]*2 + keys[2]*4 + keys[3]*8 + keys[4]*16 + keys[5]*32 + keys[6]*64 + keys[7]*128 + keys[8]*256;
        QByteArray ss;
        ss[0] = s;
        serialport.write(ss,1); //size = 1
        serialport.waitForBytesWritten(100); //Väntar for-evah
        send->setText("Skickat: " + QString::number((quint8)ss[0],2));
    }
}
