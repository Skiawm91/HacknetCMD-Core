#pragma once
class UserInterface {
public:
    UserInterface() : settings(this) {}
    void Home();
    void Login();
    void Settings(const bool isPlaying = false);
private:
    bool back = false;
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