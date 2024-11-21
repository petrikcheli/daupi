#include <iostream>
#include <portaudio.h>
#include <vector>
#include "opus.h"

#define SAMPLE_RATE 24000
#define FRAMES_PER_BUFFER 480
#define CHANNELS 1
#define MAX_PACKET_SIZE 4000
#define SECOND 3

struct Data{
    std::vector<float> buffer;
    //float* buffer = new float[100000];
    int frame_index = 0;
    int frame_play_index = 0;
    //int size;
};

int record_audio(
    const void* input_buffer, void* output_buffer, unsigned long frames_per_buffer,
    const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
    void* user_data
    ){
    Data *data = (Data *)user_data;
    float* in = (float*)input_buffer;
    (void)output_buffer;

    for(unsigned long i = 0; i < frames_per_buffer; ++i){
        data->buffer.push_back(in[i]);
    }
    data->frame_index++;
    return 0;
}

static int play_callback(const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags, void *userData) {

    Data *data = (Data *)userData;
    float *output = (float *)outputBuffer;
    size_t framesToCopy = framesPerBuffer;

    if (data->frame_index > 0) {
        std::copy(data->buffer.data()+(FRAMES_PER_BUFFER * CHANNELS)*data->frame_play_index,
                  data->buffer.data()+(FRAMES_PER_BUFFER * CHANNELS)*data->frame_play_index + FRAMES_PER_BUFFER*CHANNELS,
                  output);
    }
    data->frame_index--;
    data->frame_play_index++;

    //Pa_WriteStream(outputBuffer, inputBuffer, FRAMES_PER_BUFFER);

    return paContinue;
}

static void check_err(PaError err){
    if(err != paNoError){
        exit(EXIT_FAILURE);
    }
}

int main(){
    PaError err;
    Data data;
    err = Pa_Initialize();
    check_err(err);

    int count_device = Pa_GetDeviceCount();
    std::cout << "count devices - " << count_device << std::endl;
    if(count_device <= 0){
        std::cout << "no device" << std::endl;
        exit(EXIT_FAILURE);
    }

    int number_input_device = Pa_GetDefaultInputDevice();
    int number_output_device = Pa_GetDefaultOutputDevice();

    PaStream *in_stream;
    PaStream *out_stream;

    PaStreamParameters input_param;
    PaStreamParameters output_param;

    input_param.device = number_input_device;
    input_param.channelCount = CHANNELS;
    input_param.sampleFormat = paFloat32;
    input_param.suggestedLatency = Pa_GetDeviceInfo(input_param.device)->defaultLowInputLatency;
    input_param.hostApiSpecificStreamInfo = NULL;

    output_param.device = number_output_device;
    output_param.channelCount = CHANNELS;
    output_param.sampleFormat = paFloat32;
    output_param.suggestedLatency = Pa_GetDeviceInfo(output_param.device)->defaultLowOutputLatency;
    output_param.hostApiSpecificStreamInfo = NULL;

    std::cout << Pa_GetDeviceInfo(number_input_device)->name << std::endl;
    std::cout << Pa_GetDeviceInfo(number_output_device)->name << std::endl;

    err = Pa_OpenStream(
        &in_stream,
        &input_param,
        NULL,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paFramesPerBufferUnspecified,
        record_audio,
        &data
        );
    check_err(err);

    err = Pa_StartStream(in_stream);
    check_err(err);

    Pa_Sleep(SECOND * 1000);

    err = Pa_StopStream(in_stream);
    check_err(err);

    err = Pa_CloseStream(in_stream);
    check_err(err);

    //data.size = data.frame_index;
    char a = 'f';

    err = Pa_OpenStream(
        &out_stream,
        NULL,
        &output_param,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paFramesPerBufferUnspecified,
        play_callback,
        &data
        );
    check_err(err);

    err = Pa_StartStream(out_stream);
    check_err(err);

    Pa_Sleep(SECOND * 1000);

    err = Pa_StopStream(out_stream);
    check_err(err);

    int error;
    // Создание кодировщика OPUS
    OpusEncoder *encoder = opus_encoder_create(SAMPLE_RATE, CHANNELS, OPUS_APPLICATION_AUDIO, &error);
    if (error != OPUS_OK) {
        std::cerr << "failed open opus: " << opus_strerror(error) << std::endl;
        // delete[] pcm_in;
        // delete[] encoded_data;
        return -1;
    }

    size_t size_encoded_data = data.buffer.size()*sizeof(float)/sizeof(unsigned char);
    unsigned char* encoded_data = new unsigned char[size_encoded_data];

    std::vector<opus_int32> enc_data_size_frame;

    float* in_data = new float[FRAMES_PER_BUFFER];


    opus_int32 encoded_bytes_sum = 0;
    opus_int32 encoded_bytes = 0;
    for(int i,j = 0; i < data.buffer.size(); i += FRAMES_PER_BUFFER, ++j){
        //memcpy(in_data, data.buffer.data()+j*FRAMES_PER_BUFFER, FRAMES_PER_BUFFER);
        std::copy(data.buffer.data()+j*FRAMES_PER_BUFFER,
                  data.buffer.data()+j*FRAMES_PER_BUFFER+FRAMES_PER_BUFFER, in_data);

        encoded_bytes = opus_encode_float(encoder, in_data, FRAMES_PER_BUFFER,
                                          encoded_data+(encoded_bytes_sum), FRAMES_PER_BUFFER);

        enc_data_size_frame.push_back(encoded_bytes);
        encoded_bytes_sum += encoded_bytes;

        if (encoded_bytes < 0) {
            std::cerr << "Ошибка кодирования OPUS: " << opus_strerror(encoded_bytes) << std::endl;
            opus_encoder_destroy(encoder);
            return -1;
        }
    }


    std::cout << "size buffer: " << data.buffer.size() << std::endl;
    std::cout << "size encoded buffer: " << encoded_bytes_sum << std::endl;

    OpusDecoder *decoder = opus_decoder_create(SAMPLE_RATE, CHANNELS, &err);
    if (err != OPUS_OK) {
        std::cerr << "Failed to create OPUS decoder: " << opus_strerror(err) << std::endl;
    }

    //opus_decoder_ctl(decoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_MUSIC));

    float* decoded_buffer = new float[data.buffer.size()];

    size_t decoded_bytes_sum = 0;
    opus_int32 encoded_data_index = 0;


    for(int i = 0; i < enc_data_size_frame.size(); ++i){
        int decoded_samples = opus_decode_float(decoder, encoded_data+encoded_data_index,
                                                enc_data_size_frame[i],
                                                decoded_buffer+i*FRAMES_PER_BUFFER, FRAMES_PER_BUFFER, 0);
        decoded_bytes_sum += decoded_samples;
        encoded_data_index += enc_data_size_frame[i];
        if (decoded_samples < 0) {
            std::cerr << "error decoded OPUS: " << opus_strerror(decoded_samples) << std::endl;
            opus_decoder_destroy(decoder);
            opus_encoder_destroy(encoder);
            return -1;
        }
    }



    std::cout << "size decoded buffer: " << decoded_bytes_sum << std::endl;

    for(int i = 0; i < 100; ++i){
        std::cout << decoded_buffer[i] <<std::endl;
    }

    for(int i = 0; i < data.buffer.size(); ++i){
        data.buffer[i] = 0;
    }

    data.buffer.clear();

    data.buffer.resize(decoded_bytes_sum * CHANNELS); // Убедитесь, что data.buffer имеет нужный размер
    std::copy(decoded_buffer, decoded_buffer + decoded_bytes_sum * CHANNELS, data.buffer.begin());

    data.frame_index = data.buffer.size();
    data.frame_play_index = 0;

    err = Pa_StartStream(out_stream);
    check_err(err);

    Pa_Sleep(SECOND * 1000);

    err = Pa_StopStream(out_stream);
    check_err(err);

    err = Pa_CloseStream(out_stream);
    check_err(err);

    Pa_Terminate();


    delete[] encoded_data;

    return 0;
}
