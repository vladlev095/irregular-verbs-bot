#include <iostream>
#include <vector>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <map>
#include <fstream>

#include <tgbot/tgbot.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Menu {
public:
    Menu() = default;
    ~Menu() = default;
    void addCommand(std::string name, std::string description) {
        TgBot::BotCommand::Ptr cmd(new TgBot::BotCommand);
        cmd->command = name;
        cmd->description = description;
        commands.push_back(cmd);
    }
    std::vector<TgBot::BotCommand::Ptr> getCommands() {
        return commands;
    }
private:
    std::vector<TgBot::BotCommand::Ptr> commands;
};

class InlineKeyboard {
public:
    InlineKeyboard() {
        ptr = TgBot::InlineKeyboardMarkup::Ptr(new TgBot::InlineKeyboardMarkup);
    }
    void addButton(std::string text, std::string callbackData) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text = text;
        button->callbackData = callbackData;
        row.push_back(button);
    }
    void addRow() {
        ptr->inlineKeyboard.push_back(row);
        row.clear();
    }
    TgBot::InlineKeyboardMarkup::Ptr getPtr() {
        return ptr;
    }
private:
    TgBot::InlineKeyboardMarkup::Ptr ptr;
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;
};

class jsonExtract {
    public:
    jsonExtract() {}
    private:
};

std::string groupName(const std::string& queryData) {
    for(int i = 0; i <= 10; ++i) {
        std::string groupName = "Group" + std::to_string(i);
        if(queryData == groupName) {
            return groupName;
        }
    }
    return {};
}

int main() {
    std::string token(getenv("TOKEN"));
    TgBot::Bot bot(token);

    //json
    std::ifstream file("../localdb/verbs.json");
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    json j = json::parse(file);
    json Group1 = json::array();
    Group1 = j["Group 1"]; 
    json Group2 = json::array();
    Group2 = j["Group 2"];
    std::map<std::string, json> groups = {{"Group1", Group1}, {"Group2", Group2}};

    //menu
    Menu menu;
    menu.addCommand("start", "Go back to the beginning");
    menu.addCommand("learn", "Choose a set of verbs and begin learning");
    menu.addCommand("test", "Check your progress");
    bot.getApi().setMyCommands(menu.getCommands());

    //inline keyboards
    InlineKeyboard startKeyboard;
    startKeyboard.addButton("Learn", "Learn");
    startKeyboard.addButton("Test", "Test");
    startKeyboard.addRow();
    InlineKeyboard groupsKeyboard;
    groupsKeyboard.addButton("Group 1", "Group1");
    groupsKeyboard.addButton("Group 2", "Group2");
    groupsKeyboard.addButton("Group 3", "Group3");
    groupsKeyboard.addButton("Group 4", "Group4");
    groupsKeyboard.addButton("Group 5", "Group5");
    groupsKeyboard.addRow();
    groupsKeyboard.addButton("Group 6", "Group6");
    groupsKeyboard.addButton("Group 7", "Group7");
    groupsKeyboard.addButton("Group 8", "Group8");
    groupsKeyboard.addButton("Group 9", "Group9");
    groupsKeyboard.addButton("Group 10", "Group10");
    groupsKeyboard.addRow();
    InlineKeyboard naviKeyboard;
    naviKeyboard.addButton("Next verb", "Next");
    naviKeyboard.addButton("Back to group list", "Back");
    naviKeyboard.addRow();

    //events
    bot.getEvents().onCommand("start", [&bot, &startKeyboard](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "This bot will help you learn and practice English irregular verbs. \
Choose what you want to do and let's get started!", nullptr, nullptr, startKeyboard.getPtr());
    });
    bot.getEvents().onCommand("learn", [&bot, &groupsKeyboard](TgBot::Message::Ptr message) {

        bot.getApi().sendMessage(message->chat->id, "It's easier to learn irregular verbs if you divide the ones that \
        rhyme into groups. Choose a group of verbs:", nullptr, nullptr, groupsKeyboard.getPtr());
    });
    bot.getEvents().onCallbackQuery([&bot, &groups, &groupsKeyboard, &naviKeyboard](TgBot::CallbackQuery::Ptr query) {
        std::string queryData = query->data;
        int verbCount = 0;
        if (StringTools::startsWith(queryData, "Learn")) {
            bot.getApi().sendMessage(query->message->chat->id, "It's easier to learn irregular verbs if you divide the ones that \
            rhyme into groups. Choose a group of verbs:", nullptr, nullptr, groupsKeyboard.getPtr());
        }
        else if(StringTools::startsWith(queryData, groupName(queryData))) {
            bot.getApi().sendMessage(query->message->chat->id, "Here is a group of rhyming verbs to learn:");
            bot.getApi().sendMessage(query->message->chat->id, groups.at(queryData)[verbCount].dump(4), nullptr, nullptr, naviKeyboard.getPtr());
            bot.getApi().sendMessage(query->message->chat->id, "Choose another group or use the menu to go back or test your knowledge.", nullptr, nullptr, groupsKeyboard.getPtr());
        }
    });

    //sigint + longpoll
    signal(SIGINT, [](int s) {
        std::cout << "SIGINT got\n";
        exit(0);
    });
    try {
        std::cout << "Bot username: " << bot.getApi().getMe()->username.c_str() << std::endl;
        bot.getApi().deleteWebhook();
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            std::cout << "Polling..." << std::endl;
            longPoll.start();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}