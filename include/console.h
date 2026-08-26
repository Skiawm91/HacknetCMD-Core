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

    private:
        Console* parent;
        int x, y;
        std::string text;
        bool isNoBack = false;
        bool shouldSave = false;
    };

    // API 主入口
    PrintBuilder print(const std::string& text = "");
    PrintBuilder println(const std::string& text = "");
    PrintAtBuilder printAt(int x, int y, const std::string& text = "");

    void bufferRestore(int maxY = 0);
    void clear(bool clearSaved = false);
    void resetColor();

    // ⭐️ ColorSetter 升級：加入 ~ColorSetter 與 isSingleLine 標記
    struct ColorSetter {
    public:
        ColorSetter(Console* c, bool isBg, const std::string& hex, const std::string& backup)
            : parent(c), isBg(isBg), hex(hex), backup(backup), isSingleLine(false) {}
        
        ~ColorSetter(); // RAII: 鏈式呼叫結束時判斷是否刷滿全螢幕

        void singleLine();
    private:
        Console* parent;
        bool isBg;
        std::string hex;
        std::string backup;
        bool isSingleLine = false; // ⭐️ 是否為單行標記
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
    bool singleLineFg = false;
    bool singleLineBg = false;
    std::string singleLineFgBackup;
    std::string singleLineBgBackup;

    std::vector<BufferRecord> savedBuffer; // ⭐️ 全平台共用
};