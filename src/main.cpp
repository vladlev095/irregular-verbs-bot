#include <iostream>
#include <vector>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
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

    //menu
    std::vector<TgBot::BotCommand::Ptr> commands;
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "start";
    cmdArray->description = "go back to the beginning";
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

    //events
    bot.getEvents().onCommand("start", [&bot, &startKeyboard](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "This bot will help you learn and practice English irregular verbs. \
            Choose what you want to do and let's get started!", nullptr, nullptr, startKeyboard);
    });
    bot.getEvents().onCallbackQuery([&bot, &groupsKeyboard](TgBot::CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "Learn")) {
            bot.getApi().sendMessage(query->message->chat->id, "learn callback", nullptr, nullptr, groupsKeyboard);
        }
    });
    bot.getEvents().onCommand("learn", [&bot, &groupsKeyboard](TgBot::Message::Ptr message) {
            bot.getApi().sendMessage(message->chat->id, "learn command", nullptr, nullptr, groupsKeyboard);
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