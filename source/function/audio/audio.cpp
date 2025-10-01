// Mac audio play/stop by GPT
#define _HAS_STD_BYTE 0
#include "function.h"
#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <iostream>
#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CoreFoundation.h>
#include <atomic>
#include <thread>
#include <chrono>
#endif
#include <string>
using namespace std;

#ifdef _WIN32
void Function::Audio::play(const string& soundFile) {
    PlaySoundA(("assets/musics/" + soundFile).c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}
void Function::Audio::stop() {
    PlaySoundA(NULL, NULL, 0);
}
#elif __APPLE__
static atomic<bool> keepPlaying(false);
static thread playerThread;

struct PlayerContext {
    ExtAudioFileRef audioFile = nullptr;
    AudioQueueRef queue = nullptr;
    UInt32 maxPacketSize = 0;
    UInt32 numPacketsToRead = 1024;
    AudioStreamBasicDescription clientFormat = {};
};

static void AQOutputCallback(void* inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
    PlayerContext* ctx = (PlayerContext*)inUserData;
    if (!keepPlaying.load()) {
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

static void playerFunc(string filepath) {
    PlayerContext ctx = {};
    CFURLRef url = CFURLCreateFromFileSystemRepresentation(nullptr, (const UInt8*)filepath.c_str(), filepath.length(), false);
    if (!url) {
        cerr << "Invalid file URL\n";
        return;
    }
    if (ExtAudioFileOpenURL(url, &ctx.audioFile) != noErr) {
        CFRelease(url);
        cerr << "Failed to open audio file\n";
        return;
    }
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
    while (keepPlaying.load())
        this_thread::sleep_for(chrono::milliseconds(100));
    AudioQueueStop(ctx.queue, true);
    AudioQueueDispose(ctx.queue, true);
    ExtAudioFileDispose(ctx.audioFile);
}

void Function::Audio::play(const string& soundFile) {
    if (keepPlaying.load()) return;
    keepPlaying = true;
    playerThread = thread(playerFunc, "assets/musics/" + soundFile);
}

void Function::Audio::stop() {
    if (!keepPlaying.load()) return;
    keepPlaying = false;
    if (playerThread.joinable()) playerThread.join();
}
#endif