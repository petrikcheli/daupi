// #include "Audio/audio.h"
// #include "Client/client.h"
// #include <thread>

// void foo(Audio* audio){
//     audio->initialization_device();
//     audio->open_in_stream();
//     audio->open_out_stream();
//     Pa_Sleep(10 * 1000);
// }

// int main(){
//     setlocale(LC_ALL, "rus");
//     boost::asio::io_context io_context;
//     Audio* audio = new Audio();
//     Client* client = new Client(io_context, "127.0.0.1", 8989);
//     audio->signalAudioCaptured.connect(
//         [&client](std::queue<std::shared_ptr<std::vector<unsigned char>>>& q_voice) {
//             client->send_message(q_voice); }
//         );
//     client->signal_voice_arrived.connect(
//         [&audio](std::shared_ptr<std::vector<unsigned char>> ar){
//             audio->decoded_voice(ar);
//         }
//         );
//     std::thread client_thread([&]() {
//         audio->initialization_device();
//         audio->open_in_stream();
//         audio->open_out_stream();
//         Pa_Sleep(10 * 1000);
//     });
//     // std::thread thr1(foo, audio);

//     // thr1.join();
//     client_thread.join();
//     io_context.run();

//     return 0;
// }

#include "ui/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
