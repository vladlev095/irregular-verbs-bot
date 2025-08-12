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

class InlineKeyboard {
public:
    InlineKeyboard(TgBot::InlineKeyboardMarkup::Ptr startKeyboard) {
        ptr = startKeyboard;
    }
    void addButton(std::string text, std::string callbackData) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text = text;
        button->callbackData = callbackData;
        row0.push_back(button);
    }
    void addRow() {
        ptr->inlineKeyboard.push_back(row0);
        row0.clear();
    }
private:
    TgBot::InlineKeyboardMarkup::Ptr ptr;
    std::vector<TgBot::InlineKeyboardButton::Ptr> row0;
};

std::string groupName(const std::string &queryData) {
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
    std::ifstream f("../localdb/verbs.json");
    if (!f.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    json j = json::parse(f);
    json Group1 = json::array();
    Group1 = j["Group 1"]; 
    json Group2 = json::array();
    Group2 = j["Group 2"];
    std::map<std::string, json> groups = { {"Group1", Group1}, {"Group2", Group2}};

    //menu
    std::vector<TgBot::BotCommand::Ptr> commands;
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "start";
    cmdArray->description = "Go back to the beginning";
    commands.push_back(cmdArray);
    cmdArray = TgBot::BotCommand::Ptr(new TgBot::BotCommand);
    cmdArray->command = "learn";
    cmdArray->description = "Choose a set of verbs and begin learning";
    commands.push_back(cmdArray);
    cmdArray = TgBot::BotCommand::Ptr(new TgBot::BotCommand);
    cmdArray->command = "test";
    cmdArray->description = "Check your progress";
    commands.push_back(cmdArray);
    bot.getApi().setMyCommands(commands);

    //inline keyboards
    TgBot::InlineKeyboardMarkup::Ptr startKeyboard(new TgBot::InlineKeyboardMarkup);
    InlineKeyboard skb(startKeyboard);
    skb.addButton("Learn", "Learn");
    skb.addButton("Test", "Test"); 
    skb.addRow();
    TgBot::InlineKeyboardMarkup::Ptr groupsKeyboard(new TgBot::InlineKeyboardMarkup);
    InlineKeyboard gkb(groupsKeyboard);
    gkb.addButton("Group 1", "Group1");
    gkb.addButton("Group 2", "Group2");
    gkb.addButton("Group 3", "Group3");
    gkb.addButton("Group 4", "Group4");
    gkb.addButton("Group 5", "Group5");
    gkb.addRow();
    gkb.addButton("Group 6", "Group6");
    gkb.addButton("Group 7", "Group7");
    gkb.addButton("Group 8", "Group8");
    gkb.addButton("Group 9", "Group9");
    gkb.addButton("Group 10", "Group10");
    gkb.addRow();
    TgBot::InlineKeyboardMarkup::Ptr naviKeyboard(new TgBot::InlineKeyboardMarkup);
    InlineKeyboard nkb(naviKeyboard);
    nkb.addButton("Next verb", "Next");
    nkb.addButton("Back to group list", "Back");
    nkb.addRow();

    //events
    bot.getEvents().onCommand("start", [&bot, &startKeyboard](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "This bot will help you learn and practice English irregular verbs. \
            Choose what you want to do and let's get started!", nullptr, nullptr, startKeyboard);
    });
    bot.getEvents().onCommand("learn", [&bot, &groupsKeyboard](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "It's easier to learn irregular verbs if you divide the ones that rhyme into groups. Choose a group of verbs:", nullptr, nullptr, groupsKeyboard);
    });
    bot.getEvents().onCallbackQuery([&bot, &groups, &groupsKeyboard](TgBot::CallbackQuery::Ptr query) {
        std::string queryData = query->data;
        if (StringTools::startsWith(queryData, "Learn")) {
            bot.getApi().sendMessage(query->message->chat->id, "It's easier to learn irregular verbs if you divide the ones that rhyme into groups. Choose a group of verbs:", nullptr, nullptr, groupsKeyboard);
        }
        else if(StringTools::startsWith(queryData, groupName(queryData))) {
            bot.getApi().sendMessage(query->message->chat->id, "Here is a group of rhyming verbs to learn:");
            for(int i = 0; i < groups.at(queryData).size(); i++) {
                bot.getApi().sendMessage(query->message->chat->id, groups.at(queryData)[i].dump(4), nullptr, nullptr);
            }
            bot.getApi().sendMessage(query->message->chat->id, "Choose another group or use the menu to go back or test your knowledge.", nullptr, nullptr, groupsKeyboard);
        }
    });

    //sigint + longpoll
    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
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