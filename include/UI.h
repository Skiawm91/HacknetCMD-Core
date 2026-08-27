#pragma once
class UserInterface {
public:
    UserInterface() : settings(this) {}
    void Home();
    void Login();
    void Settings(const bool isPlaying = false);
private:
    bool back = false;
    bool mode = 0; // 0: General, 1: VT100 Configuration
    struct SettingsOptions {
    public:
        SettingsOptions(UserInterface* p) : parent(p) {}
        void General(const bool &isPlaying);
        void VT100Configuration();
    private:
        UserInterface* parent;
    };
    SettingsOptions settings;
};