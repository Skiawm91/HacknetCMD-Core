// All GPT Let's GO
#define _HAS_STD_BYTE 0
#include "function.h"
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#elif __APPLE__
#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CoreFoundation.h>
#include <limits.h>
#include <cstdlib>
#endif
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <random>
#include <chrono>
#include <iostream>
using namespace std;

#ifdef _WIN32

struct AudioImpl {
    unordered_map<string, pair<thread, shared_ptr<atomic<bool>>>> audioThreads;
    mutex audioMutex;

    string randomPick(const vector<string>& sounds) {
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> dis(0, sounds.size() - 1);
        return sounds[dis(gen)];
    }
};

static AudioImpl gAudioImpl;

static void mciPlay(const string& alias, const string& file, bool loop) {
    string openCmd = "open \"" + file + "\" type mpegvideo alias " + alias;
    mciSendStringA(openCmd.c_str(), NULL, 0, NULL);

    string playCmd = "play " + alias;
    if (loop) playCmd += " repeat";
    mciSendStringA(playCmd.c_str(), NULL, 0, NULL);
}

static void mciStop(const string& alias) {
    string stopCmd = "stop " + alias;
    mciSendStringA(stopCmd.c_str(), NULL, 0, NULL);
    string closeCmd = "close " + alias;
    mciSendStringA(closeCmd.c_str(), NULL, 0, NULL);
}

// 單次播放
void Function::Audio::play(const string& threadName, const vector<string>& fileNames, const int type) {
    string filePath;
    if (type == 0) filePath = "assets/musics/";
    else if (type == 1) filePath = "assets/sounds/";

    lock_guard<mutex> lock(gAudioImpl.audioMutex);

    // 若該音軌存在，先關閉
    auto it = gAudioImpl.audioThreads.find(threadName);
    if (it != gAudioImpl.audioThreads.end()) {
        it->second.second->store(false);
        if (it->second.first.joinable()) it->second.first.join();
        gAudioImpl.audioThreads.erase(it);
    }

    auto running = make_shared<atomic<bool>>(true);

    thread t([=]() {
        string alias = "ch_" + threadName;
        string file = filePath + gAudioImpl.randomPick(fileNames);
        mciPlay(alias, file, false);
        while (running->load()) this_thread::sleep_for(chrono::milliseconds(100));
        mciStop(alias);
    });

    gAudioImpl.audioThreads[threadName] = { move(t), running };
}

// 迴圈播放
void Function::Audio::playL(const string& threadName, const vector<string>& fileNames, const int type) {
    string filePath;
    if (type == 0) filePath = "assets/musics/";
    else if (type == 1) filePath = "assets/sounds/";

    lock_guard<mutex> lock(gAudioImpl.audioMutex);

    // 若該音軌存在，先關閉
    auto it = gAudioImpl.audioThreads.find(threadName);
    if (it != gAudioImpl.audioThreads.end()) {
        it->second.second->store(false);
        if (it->second.first.joinable()) it->second.first.join();
        gAudioImpl.audioThreads.erase(it);
    }

    auto running = make_shared<atomic<bool>>(true);

    thread t([=]() {
        string alias = "ch_" + threadName;
        string file = filePath + gAudioImpl.randomPick(fileNames);
        mciPlay(alias, file, true);
        while (running->load()) this_thread::sleep_for(chrono::milliseconds(100));
        mciStop(alias);
    });

    gAudioImpl.audioThreads[threadName] = { move(t), running };
}

// 停止特定音軌
void Function::Audio::stop(const string& threadName) {
    lock_guard<mutex> lock(gAudioImpl.audioMutex);
    auto it = gAudioImpl.audioThreads.find(threadName);
    if (it != gAudioImpl.audioThreads.end()) {
        it->second.second->store(false);
        if (it->second.first.joinable()) it->second.first.join();
        string alias = "ch_" + threadName;
        mciStop(alias);
        gAudioImpl.audioThreads.erase(it);
    }
}

// 停止全部音軌
void Function::Audio::stop() {
    lock_guard<mutex> lock(gAudioImpl.audioMutex);
    for (auto& kv : gAudioImpl.audioThreads) {
        kv.second.second->store(false);
        if (kv.second.first.joinable()) kv.second.first.join();
        string alias = "ch_" + kv.first;
        mciStop(alias);
    }
    gAudioImpl.audioThreads.clear();
}

#elif __APPLE__

struct PlayerContext {
    ExtAudioFileRef audioFile = nullptr;
    AudioQueueRef queue = nullptr;
    UInt32 maxPacketSize = 0;
    UInt32 numPacketsToRead = 1024;
    AudioStreamBasicDescription clientFormat = {};
    shared_ptr<atomic<bool>> running; // ✅ 新增
};

struct MacAudioThread {
    thread t;
    shared_ptr<atomic<bool>> running;
};

static unordered_map<string, MacAudioThread> macAudioThreads;
static mutex macAudioMutex;
static random_device rd;
static mt19937 gen(rd());

static string randomPick(const vector<string>& sounds) {
    uniform_int_distribution<> dis(0, sounds.size() - 1);
    return sounds[dis(gen)];
}

static void AQOutputCallback(void* inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
    PlayerContext* ctx = (PlayerContext*)inUserData;
    if (!ctx->running || !ctx->running->load()) { // ✅ 直接用
        AudioQueueStop(ctx->queue, false);
        return;
    }
    UInt32 numPackets = ctx->numPacketsToRead;
    AudioBufferList bufferList;
    bufferList.mNumberBuffers = 1;
    bufferList.mBuffers[0].mData = inBuffer->mAudioData;
    bufferList.mBuffers[0].mDataByteSize = ctx->maxPacketSize * numPackets;
    bufferList.mBuffers[0].mNumberChannels = ctx->clientFormat.mChannelsPerFrame;
    OSStatus status = ExtAudioFileRead(ctx->audioFile, &numPackets, &bufferList);
    if (status != noErr || numPackets == 0) {
        ExtAudioFileSeek(ctx->audioFile, 0);
        numPackets = ctx->numPacketsToRead;
        bufferList.mBuffers[0].mData = inBuffer->mAudioData;
        bufferList.mBuffers[0].mDataByteSize = ctx->maxPacketSize * numPackets;
        status = ExtAudioFileRead(ctx->audioFile, &numPackets, &bufferList);
        if (status != noErr || numPackets == 0) {
            inBuffer->mAudioDataByteSize = 0;
            AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, nullptr);
            return;
        }
    }
    inBuffer->mAudioDataByteSize = bufferList.mBuffers[0].mDataByteSize;
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, nullptr);
}

static void playerFunc(string filepath, shared_ptr<atomic<bool>> running) {
    PlayerContext ctx = {};
    ctx.running = running; // ✅ 加上這行
    char fullpath[PATH_MAX];
    if (!realpath(filepath.c_str(), fullpath)) return;
    CFURLRef url = CFURLCreateFromFileSystemRepresentation(nullptr, (const UInt8*)fullpath, strlen(fullpath), false);
    if (!url) return;
    if (ExtAudioFileOpenURL(url, &ctx.audioFile) != noErr) { CFRelease(url); return; }
    CFRelease(url);

    AudioStreamBasicDescription fileFormat;
    UInt32 size = sizeof(fileFormat);
    ExtAudioFileGetProperty(ctx.audioFile, kExtAudioFileProperty_FileDataFormat, &size, &fileFormat);

    ctx.clientFormat.mSampleRate = fileFormat.mSampleRate;
    ctx.clientFormat.mFormatID = kAudioFormatLinearPCM;
    ctx.clientFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
    ctx.clientFormat.mFramesPerPacket = 1;
    ctx.clientFormat.mChannelsPerFrame = fileFormat.mChannelsPerFrame;
    ctx.clientFormat.mBitsPerChannel = 16;
    ctx.clientFormat.mBytesPerPacket = 2 * fileFormat.mChannelsPerFrame;
    ctx.clientFormat.mBytesPerFrame = 2 * fileFormat.mChannelsPerFrame;

    ExtAudioFileSetProperty(ctx.audioFile, kExtAudioFileProperty_ClientDataFormat, sizeof(ctx.clientFormat), &ctx.clientFormat);
    AudioQueueNewOutput(&ctx.clientFormat, AQOutputCallback, &ctx, nullptr, nullptr, 0, &ctx.queue);

    ctx.maxPacketSize = ctx.clientFormat.mBytesPerPacket;
    UInt32 bufferByteSize = ctx.maxPacketSize * ctx.numPacketsToRead;

    for (int i = 0; i < 3; ++i) {
        AudioQueueBufferRef buffer;
        AudioQueueAllocateBuffer(ctx.queue, bufferByteSize, &buffer);
        AQOutputCallback(&ctx, ctx.queue, buffer);
    }

    AudioQueueStart(ctx.queue, nullptr);
    while (running->load()) this_thread::sleep_for(chrono::milliseconds(100));

    AudioQueueStop(ctx.queue, true);
    AudioQueueDispose(ctx.queue, true);
    ExtAudioFileDispose(ctx.audioFile);
}

void Function::Audio::play(const string& threadName, const vector<string>& sounds) {
    lock_guard<mutex> lock(macAudioMutex);

    auto it = macAudioThreads.find(threadName);
    if (it != macAudioThreads.end()) {
        it->second.running->store(false);
        if (it->second.t.joinable()) it->second.t.join();
        macAudioThreads.erase(it);
    }

    auto running = make_shared<atomic<bool>>(true);
    string file = "assets/musics/" + randomPick(sounds);

    thread t(playerFunc, file, running);

    macAudioThreads[threadName] = { std::move(t), running };
}

void Function::Audio::playL(const string& threadName, const vector<string>& sounds) {
    lock_guard<mutex> lock(macAudioMutex);

    auto it = macAudioThreads.find(threadName);
    if (it != macAudioThreads.end()) {
        it->second.running->store(false);
        if (it->second.t.joinable()) it->second.t.join();
        macAudioThreads.erase(it);
    }

    auto running = make_shared<atomic<bool>>(true);

    thread t([sounds, running]() {
        while (running->load()) {
            string file = "assets/musics/" + randomPick(sounds);
            playerFunc(file, running);
        }
    });

    macAudioThreads[threadName] = { std::move(t), running };
}

void Function::Audio::stop(const string& threadName) {
    lock_guard<mutex> lock(macAudioMutex);
    auto it = macAudioThreads.find(threadName);
    if (it != macAudioThreads.end()) {
        it->second.running->store(false);
        if (it->second.t.joinable()) it->second.t.join();
        macAudioThreads.erase(it);
    }
}

// 停止所有線程
void Function::Audio::stop() {
    lock_guard<mutex> lock(macAudioMutex);
    for (auto& kv : macAudioThreads) {
        kv.second.running->store(false);
        if (kv.second.t.joinable()) kv.second.t.join();
    }
    macAudioThreads.clear();
}


#endif