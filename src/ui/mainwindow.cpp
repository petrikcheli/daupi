#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Audio/audio.h"
#include "../Client/client.h"
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::call(const std::string& ip, const std::string& port)
{
    setlocale(LC_ALL, "rus");
    int int_port = std::stoi(port);
    boost::asio::io_context io_context;
    Audio* audio = new Audio();
    Client* client = new Client(io_context, ip, int_port);
    audio->signalAudioCaptured.connect(
        [&client](std::queue<std::shared_ptr<std::vector<unsigned char>>>& q_voice) {
            client->send_message(q_voice); }
        );
    client->signal_voice_arrived.connect(
        [&audio](std::shared_ptr<std::vector<unsigned char>> ar){
            audio->decoded_voice(ar);
        }
        );
    std::thread client_thread([&]() {
        audio->initialization_device();
        audio->open_in_stream();
        audio->open_out_stream();
        //Pa_Sleep(10 * 1000);
    });
    // std::thread thr1(foo, audio);

    // thr1.join();
    client_thread.join();
    io_context.run();
}

void MainWindow::on_connect_clicked()
{
    QString qStr_ip = ui->ip_address->text();
    std::string ip = qStr_ip.toStdString();

    QString qStr_port = ui->port->text();
    std::string port = qStr_port.toStdString();

    ui->ip_address->deleteLater();
    ui->port->deleteLater();
    ui->connect->deleteLater();
    this->call(ip, port);
}

