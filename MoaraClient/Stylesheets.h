#pragma once
#include <QString>

const QString buttonStyle = R"(
    QPushButton {
        background-color: white;
        border: none;
        color: black;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        margin: 4px 2px;
        transition-duration: 0.4s;
        cursor: pointer;
        border-radius: 15px;
    }
    QPushButton:hover {
        background-color: #ff7a19; 
        color: white;
    }
)";

const QString pushedButtonStyle = R"(
    QPushButton {
        background-color: #ff7a19;
        border: none;
        color: white;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        margin: 4px 2px;
        transition-duration: 0.4s;
        cursor: pointer;
        border-radius: 15px;
    })";

const QString windowBackground = "background-color: rgb(255, 237, 171);";