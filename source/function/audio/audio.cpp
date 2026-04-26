#define _HAS_STD_BYTE 0
#define STB_VORBIS_HEADER_ONLY
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4018)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
#include "miniaudio/stb_vorbis.c"
#ifdef _WIN32
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"
#undef STB_VORBIS_HEADER_ONLY
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4018)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif
#include "miniaudio/stb_vorbis.c"
#ifdef _WIN32
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif
#include "function.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <random>
#include <chrono>
using namespace std;

static mutex audioMutex;
static random_device rd;
static mt19937 gen(rd());

static string randomPick(const vector<string>& sounds) {
    uniform_int_distribution<> dis(0, sounds.size() - 1);
    return sounds[dis(gen)];
}

struct AudioThread {
    thread t;
    shared_ptr<atomic<bool>> running;
};

static unordered_map<string, AudioThread> audioThreads;

static void playFunc(const string& filepath, shared_ptr<atomic<bool>> running) {
    ma_engine engine;
    if (ma_engine_init(nullptr, &engine) != MA_SUCCESS) return;

    ma_sound sound;
    if (ma_sound_init_from_file(&engine, filepath.c_str(), 0, nullptr, nullptr, &sound) != MA_SUCCESS) {
        ma_engine_uninit(&engine);
        return;
    }

    ma_sound_start(&sound);

    while (running->load() && ma_sound_is_playing(&sound))
        this_thread::sleep_for(chrono::milliseconds(100));

    ma_sound_stop(&sound);
    ma_sound_uninit(&sound);
    ma_engine_uninit(&engine);
}

static void playLoopFunc(const string& filePath, const vector<string>& sounds, shared_ptr<atomic<bool>> running) {
    ma_engine engine;
    if (ma_engine_init(nullptr, &engine) != MA_SUCCESS) return;

    while (running->load()) {
        string file = filePath + randomPick(sounds);
        ma_sound sound;
        if (ma_sound_init_from_file(&engine, file.c_str(), 0, nullptr, nullptr, &sound) != MA_SUCCESS) break;

        ma_sound_start(&sound);

        while (running->load() && ma_sound_is_playing(&sound))
            this_thread::sleep_for(chrono::milliseconds(100));

        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
    }

    ma_engine_uninit(&engine);
}

void Function::Audio::play(const string& threadName, const vector<string>& sounds, const string& filePath) {
    lock_guard<mutex> lock(audioMutex);

    auto it = audioThreads.find(threadName);
    if (it != audioThreads.end()) {
        it->second.running->store(false);
        if (it->second.t.joinable()) it->second.t.join();
        audioThreads.erase(it);
    }

    auto running = make_shared<atomic<bool>>(true);
    string file = filePath + randomPick(sounds);

    thread t(playFunc, file, running);
    audioThreads[threadName] = { std::move(t), running };
}

void Function::Audio::playL(const string& threadName, const vector<string>& sounds, const string& filePath) {
    lock_guard<mutex> lock(audioMutex);

    auto it = audioThreads.find(threadName);
    if (it != audioThreads.end()) {
        it->second.running->store(false);
        if (it->second.t.joinable()) it->second.t.join();
        audioThreads.erase(it);
    }

    auto running = make_shared<atomic<bool>>(true);
    thread t(playLoopFunc, filePath, sounds, running);
    audioThreads[threadName] = { std::move(t), running };
}

void Function::Audio::stop(const string& threadName) {
    lock_guard<mutex> lock(audioMutex);
    auto it = audioThreads.find(threadName);
    if (it != audioThreads.end()) {
        it->second.running->store(false);
        if (it->second.t.joinable()) it->second.t.join();
        audioThreads.erase(it);
    }
}

void Function::Audio::stop() {
    lock_guard<mutex> lock(audioMutex);
    for (auto& kv : audioThreads) {
        kv.second.running->store(false);
        if (kv.second.t.joinable()) kv.second.t.join();
    }
    audioThreads.clear();
}