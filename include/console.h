#pragma once
#include <string>
#include <vector>
#include <iostream>

class Console {
public:
    // 跨平台統一 Buffer 紀錄結構
    struct BufferRecord {
        int x = -1;
        int y = -1;
        std::string text;
        std::string fg;
        std::string bg;
        bool noBack = false;
        bool isInline = false;
    };

    // --- Fluent API Builders ---
    class PrintBuilder {
    public:
        PrintBuilder(Console* c, const std::string& text = "")
            : parent(c), text(text) {}
        ~PrintBuilder();

        PrintBuilder& save();

    private:
        Console* parent;
        std::string text;
        bool shouldSave = false;
    };

    class PrintAtBuilder {
    public:
        PrintAtBuilder(Console* c, int x, int y, const std::string& text = "")
            : parent(c), x(x), y(y), text(text) {}
        ~PrintAtBuilder();

        PrintAtBuilder& noBack();
        PrintAtBuilder& save();
        PrintAtBuilder& noEraseEOL();

    private:
        Console* parent;
        int x, y;
        std::string text;
        bool isNoBack = false;
        bool shouldSave = false;
        bool isNoEraseEOL = false;
    };

    // API 主入口
    PrintBuilder print(const std::string& text = "");
    PrintBuilder println(const std::string& text = "");
    PrintAtBuilder printAt(int x, int y, const std::string& text = "");

    void bufferRestore(int maxY = 0);
    void clear(bool clearSaved = false);
    void resetColor();

    // ⭐️ ColorSetter 升級：加入 ~ColorSetter 與 isSingleLine 標記
    class ColorSetter {
    private:
        Console* parent;
        bool isBg;
        bool isFill; // 控制析構時是否 fillScreenBg (預設: isBg 的值)
        bool isOnce; // 是否只用一次
        std::string hex;
        std::string backup;

    public:
        ColorSetter(Console* p, bool bg, const std::string& h, const std::string& b)
            : parent(p), isBg(bg), isFill(bg), isOnce(false), hex(h), backup(b) {}

        // 鏈式調用方法
        ColorSetter& noFill();
        ColorSetter& once();

        ~ColorSetter();
    };

    ColorSetter setColor(const std::string& hexColor);
    ColorSetter setColorBg(const std::string& hexColor);

    void applyFg(const std::string& hex);
    void applyBg(const std::string& hex);
    void fillScreenBg(const std::string& hex); // ⭐️ 刷滿全螢幕底色的專用函式

    struct Cursor {
        void show();
        void hide();
    };
    Cursor cursor;

    // Helper 供 Builder 存取
    std::string getFg() const { return globalFgColor; }
    std::string getBg() const { return globalBgColor; }
    void addRecord(const BufferRecord& rec) { savedBuffer.push_back(rec); }

    void resize(int width, int height);
private:
    std::string globalFgColor = "FFFFFF";
    std::string globalBgColor = "000000";
    bool onceFg = false;
    bool onceBg = false;
    std::string onceFgBackup;
    std::string onceBgBackup;

    std::vector<BufferRecord> savedBuffer; // ⭐️ 全平台共用
};