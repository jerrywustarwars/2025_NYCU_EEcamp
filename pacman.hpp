#pragma once
#include <bits/stdc++.h>
#include <windows.h>
using namespace std;

extern int dir[5][2]; 
extern bool gameRunning;

bool checkNotCollideWall(string grid[], int i, int j);
bool checkCollideGhost(int (*ghostPos)[2], int playerPos[2]);
int bestDir(string grid[], int curPos[2], int curDir, int target[2]);
template <int _N>
void moveGhost(string grid[], int (&ghostPos)[_N][2], int (&ghostDir)[_N],
                char ghostType[], int playerPos[2],
                int playerDir, bool scaredMode);
void eatGhost(string grid[], int playerPos[2], int (*ghostPos)[2],
                int (&ghostDir)[ghostNum], char (&ghostType)[ghostNum],
                int &score);

// 鍵 盤 管 理 器 結 構
struct KeyManager {
    char lastDirC;  // 最 後 按 下 的 方 向 鍵 字 符
    int lastDir;    // 最 後 按 下 的 方 向
    std::map<char, int> wasdToDir = {{'d', 0}, {'s', 1}, {'a', 2}, {'w', 3}};  // WASD 鍵 對 應  的 方 向
    bool start;  // 是 否 開 始
    bool newInput;  // 是 否 有 新 輸 入

    KeyManager() : lastDirC('w'), lastDir(4), start(true), newInput(false) {
        thread rK(&KeyManager::updateStatus, this);  // 啟 動 更 新 狀 態 的 線 程
        rK.detach();  // 分 離 線 程
    }

    ~KeyManager() {
        start = false;  // 停 止 更 新 狀 態
        INPUT inputs[3] = {};
        ZeroMemory(inputs, sizeof(inputs));  // 清 空 輸 入

        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = 'w';  // 模 擬 按 下 'w' 鍵

        inputs[2].type = INPUT_KEYBOARD;
        inputs[2].ki.wVk = 'w';
        inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;  // 模 擬 釋 放 'w' 鍵

        UINT unsent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));  // 發 送 輸 入
    }

    // 更 新 鍵 盤 狀 態
    void updateStatus() {
        char cur = 'w';
        lastDirC = cur;

        while (start) {
            cur = readKey();  // 讀 取 當 前 按 鍵
            newInput = true;
            if (cur == 'w' || cur == 'a' || cur == 's' || cur == 'd') {
                lastDirC = cur;
                lastDir = wasdToDir[lastDirC];  // 更 新 方 向
            }
        }
        return;
    }

    // 讀 取 按 鍵
    char readKey() {
        INPUT_RECORD inputRecord;
        DWORD written;
        HANDLE stdInH = GetStdHandle(STD_INPUT_HANDLE);
        while (true) {
            ReadConsoleInputA(stdInH, &inputRecord, 1, &written);  // 讀 取 控 制 台 輸 入
            if (inputRecord.EventType == KEY_EVENT &&
                inputRecord.Event.KeyEvent.bKeyDown)
                break;  // 如 果 是 按 鍵 事 件 且 按 鍵 被 按 下 ， 則 退 出 循 環
        }
        switch (inputRecord.Event.KeyEvent.wVirtualScanCode) {
            case 72:
                return 'w';  // 回 傳 'w'
            case 75:
                return 'a';  // 回 傳 'a'
            case 80:
                return 's';  // 回 傳 's'
            case 77:
                return 'd';  // 回 傳 'd'
            default:
                return inputRecord.Event.KeyEvent.uChar.AsciiChar;  // 回 傳 其 他 字 符
        }
    }

    // 獲 取 最 後 按 下 的 鍵
    int getLastKey() {
        return lastDir;
    }

    void anyKeyToContinue() {
        newInput = false;
        while (!newInput) {}
        return;
    }
};

// 輸 出 遊 戲 網 格
void gridPrint(string grid[], string (&prevGrid)[HEIGHT]) {
    int n = HEIGHT, m = WIDTH;
    vector<vector<string>> gridDisplay(n, vector<string>(m, " "));
    vector<vector<string>> gridDisplayColor(n, vector<string>(m, ""));
    vector<vector<string>> gridDisplayThick(n, vector<string>(m, ""));

    // 輸 出 顏 色 的 函 數
    function<string(string, string, string)> outputColor =
        [&](string color, string thick, string output) {
            string ans = "\033[";
            std::map<string, string> colorGrid = {
                {"black", "0"},
                {"red", "196"},
                {"green", "46"},
                {"yellow", "11"},
                {"blue", "27"},
                {"purple", "105"},
                {"lightblue", "87"},
                {"white", "255"},
                {"pink", "219"},
                {"skin", "224"},
                {"orange", "214"}};
            std::map<string, string> thickGrid = {
                {"bold", ";1"},
                {"normal", ""},
                {"light", ";2"}};
            ans += "38;5;" + colorGrid[color] + thickGrid[thick] + "m";
            ans += output;
            ans += "\033[0m";
            return ans;
        };

    // 檢 查 上 方 是 否 有 牆 壁
    function<bool(int, int)> checkUp = [&](int x, int y) {
        if (0 <= x && x < n && 0 <= y && y < m)
            if (grid[x][y] == '#')
                return 1;  // 如 果 是 牆 壁 ， 回 傳 1
        return 0;  // 否 則 回 傳 0
    };

    // 填 充 網 格 顯 示
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (grid[i][j] == '#') {

                // 當 前 位 置 只 看 上 下 左 右 有 幾 個 #
                // 一 個 : ─ or │   // 兩 個 : ─ │ ┌ ┐ └ ┘
                // 三 個 : 看 缺 少 的 邊 對 馬 步 的 兩 個 角
                // 有 一 個 為 空 就 是 ├ ┤ ┬ ┴  都 是 滿 的 就 是 ─ │
                // 四 個 : 根 據 缺 角
                // 缺 一 個  ┌ ┐ └ ┘   缺 兩 個 相 鄰  ├ ┤ ┬ ┴   
                // 缺 兩 個 對 角  ┼    缺 三 個 以 上  ┼
                
                gridDisplayColor[i][j] = "blue";  // 牆 壁 顏 色
                gridDisplayThick[i][j] = "normal";  // 牆 壁 粗 細
                string type[16] = {"X", "─", "│", "╮", "─", "─", "╭", "┬", "│", "╯", "│", "┤", "╰", "┴", "├", "┼"};
                int next = checkUp(i + 1, j) + checkUp(i - 1, j) +
                           checkUp(i, j + 1) + checkUp(i, j - 1);
                if (next == 1 || next == 2) {
                    int iCheck = checkUp(i - 1, j) * 8 +
                                 checkUp(i, j + 1) * 4 +
                                 checkUp(i + 1, j) * 2 +
                                 checkUp(i, j - 1);
                    gridDisplay[i][j] = type[iCheck];  // 根 據 周 圍 牆 壁 的 數 量 選 擇 顯 示 的 字 符 
                } else if (next == 3) {
                    if (!checkUp(i + 1, j) &&
                        (checkUp(i - 1, j + 1) && checkUp(i - 1, j - 1)))
                        gridDisplay[i][j] = type[1];
                    else if (!checkUp(i - 1, j) &&
                             (checkUp(i + 1, j + 1) && checkUp(i + 1, j - 1)))
                        gridDisplay[i][j] = type[1];
                    else if (!checkUp(i, j + 1) &&
                             (checkUp(i - 1, j - 1) && checkUp(i + 1, j - 1)))
                        gridDisplay[i][j] = type[2];
                    else if (!checkUp(i, j - 1) &&
                             (checkUp(i - 1, j + 1) && checkUp(i + 1, j + 1)))
                        gridDisplay[i][j] = type[2];
                    else {
                        int iCheck = checkUp(i - 1, j) * 8 +
                                     checkUp(i, j + 1) * 4 +
                                     checkUp(i + 1, j) * 2 +
                                     checkUp(i, j - 1);
                        gridDisplay[i][j] = type[iCheck];
                    }
                } else if (next == 4) {
                    int angle = checkUp(i + 1, j + 1) + checkUp(i - 1, j + 1) +
                                checkUp(i + 1, j - 1) + checkUp(i - 1, j - 1);
                    if (angle == 4) {
                        gridDisplay[i][j] = " ";  // 如 果 四 周 都 有 牆 ， 顯 示 空 格
                    } else if (angle == 3) {
                        if (!checkUp(i + 1, j + 1))
                            gridDisplay[i][j] = type[6];  // ┌
                        else if (!checkUp(i - 1, j + 1))
                            gridDisplay[i][j] = type[12]; // └
                        else if (!checkUp(i + 1, j - 1))
                            gridDisplay[i][j] = type[3];  // ┐
                        else if (!checkUp(i - 1, j - 1))
                            gridDisplay[i][j] = type[9];  // ┘
                    } else if (angle == 2) {
                        if (!checkUp(i + 1, j + 1) && !checkUp(i + 1, j - 1))
                            gridDisplay[i][j] = type[13]; // ┴
                        else if (!checkUp(i - 1, j + 1) && !checkUp(i - 1, j - 1))
                            gridDisplay[i][j] = type[7];  // ┬
                        else if (!checkUp(i + 1, j + 1) && !checkUp(i - 1, j + 1))
                            gridDisplay[i][j] = type[11]; // ┤
                        else if (!checkUp(i + 1, j - 1) && !checkUp(i - 1, j - 1))
                            gridDisplay[i][j] = type[14]; // ├
                        else
                            gridDisplay[i][j] = type[15]; // ┼
                    } else
                        gridDisplay[i][j] = type[15]; // ┼
                }
            } else if (grid[i][j] == '0') {
                gridDisplayColor[i][j] = "yellow";  // pacman 顏 色
                gridDisplayThick[i][j] = "bold";  // pacman 粗 細
                gridDisplay[i][j] = "●";  // pacman 符 號
            } else if (grid[i][j] == '*') {
                gridDisplayColor[i][j] = "skin";  // 能 量 球 顏 色
                gridDisplayThick[i][j] = "normal";  // 能 量 球 粗 細 
                gridDisplay[i][j] = "○";  // 能 量 球 符 號
            } else if (grid[i][j] == '.') {
                gridDisplayColor[i][j] = "skin";  // 點 數 顏 色 
                gridDisplayThick[i][j] = "normal";  // 點 數 粗 細
                gridDisplay[i][j] = ".";  // 點 數 符 號 
            } else if (grid[i][j] == ' ') {
                gridDisplayColor[i][j] = "white";  // 空 白 顏 色
                gridDisplayThick[i][j] = "normal";  // 空 白 粗 細
                gridDisplay[i][j] = " ";  // 空 白 符 號
            } else if (isalpha(grid[i][j])) {
                if (grid[i][j] == 'G') {  // 嚇 人 的 鬼
                    gridDisplayColor[i][j] = "white";
                    gridDisplayThick[i][j] = "normal";
                    gridDisplay[i][j] = "ö";  // 嚇 人 的 鬼 符 號
                } else if (isupper(grid[i][j])) {
                    gridDisplayColor[i][j] = "purple";  // 鬼 的 顏 色
                    gridDisplayThick[i][j] = "normal";  // 鬼 的 粗 細
                    gridDisplay[i][j] = "ö";  // 鬼 符 號
                } else if (islower(grid[i][j])) {
                    string ghost[4] = {"red", "pink", "lightblue", "orange"};
                    gridDisplayColor[i][j] = ghost[grid[i][j] - 'a'];  // 小 鬼 的 顏 色
                    gridDisplayThick[i][j] = "normal";  // 小 鬼 的 粗 細
                    gridDisplay[i][j] = "∩";  // 小 鬼 符 號
                }
            }
        }
    }

    cout << "\033[H";  // 移 動 鼠 標 到 左 上 角
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (prevGrid[i][j] != grid[i][j]) {
                cout << "\033[" << (i + 1) << ";" << (j + 1) << "H";  // 移 動 光 標 到 指 定 位 置
                cout << outputColor(gridDisplayColor[i][j], "normal", gridDisplay[i][j]);  // 輸 出 顏 色 和 符 號 
            }
        }
    }
    cout << "\033[" << (n + 1) << ";1H";  // 移 動 鼠 標 到 下 一 行
    for (int i = 0; i < n; i++) {
        prevGrid[i] = grid[i];  // 更 新 前 一 個 網 格
    }
}

// 移 動 pacman
void movePacman(string grid[], int (&playerPos)[2], int &curDir, KeyManager &keyM) {
    int input = keyM.getLastKey();  // 獲 取 最 後 按 下 的 鍵
    if (checkNotCollideWall(grid, playerPos[0] + dir[input][0],
                             playerPos[1] + dir[input][1])) {
        playerPos[0] += dir[input][0];  // 更 新 pacman 位 置
        playerPos[1] += dir[input][1];
        curDir = input;  // 更 新 當 前 方 向
    } else {
        curDir = 4;  // 如 果 碰 到 牆 壁 ， 設 置 為 停 止
    }
    return;
}

// 重 新 生 成 鬼 的 位 置 
void respawnGhost(string grid[], int (&ghostPos)[2], int playerPos[2]) {
    int maxDistance = -1;
    int farX = -1, farY = -1;

    for (int x = 0; x < HEIGHT; x++) {
        for (int y = 0; y < WIDTH; y++) {
            if (grid[x][y] == '.' && (x != playerPos[0] || y != playerPos[1])) {
                int distance = (x - playerPos[0]) * (x - playerPos[0]) +
                               (y - playerPos[1]) * (y - playerPos[1]);
                if (distance > maxDistance) {
                    maxDistance = distance;
                    farX = x;
                    farY = y;  // 更 新 最 遠 位 置
                }
            }
        }
    }
    ghostPos[0] = farX;  // 設 置 鬼 的 位 置
    ghostPos[1] = farY;
}

// 更 新 遊 戲 狀 態
void update(string grid[], int playerPos[2], int (*ghostPos)[2],
            int ghostDir[], char ghostType[],
            string (&pointsGrid)[HEIGHT], int &pointsCnt, string (&display)[HEIGHT],
            int &score, bool &scaredMode, int &scaredTime) {
    int n = HEIGHT, m = WIDTH;
    for (int i = 0; i < n; i++) {
        string row = "";
        row.append(WIDTH, ' ');
        display[i] = row;  // 初 始 化 顯 示
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (grid[i][j] == '#') {
                display[i][j] = '#';  // 牆 壁
            }
            if (pointsGrid[i][j] == '.') {
                display[i][j] = '.';  // 點 數
            }
            if (pointsGrid[i][j] == '*') {
                display[i][j] = '*';  // 能 量 球
            }
        }
    }
    if (pointsGrid[playerPos[0]][playerPos[1]] == '.') {
        pointsGrid[playerPos[0]][playerPos[1]] = ' ';  // 吃 掉 點 數
        score += 10;  // 正 常 得 分
        pointsCnt--;  // 減 少 點 數 計 數
    } else if (pointsGrid[playerPos[0]][playerPos[1]] == '*') {
        pointsGrid[playerPos[0]][playerPos[1]] = ' ';  // 吃 掉 能 量 球
        score += 50;  // 能 量 球 得 分
        scaredMode = true;  // 啟 動 驚 嚇 模 式
        scaredTime = 20;  // 設 置 驚 嚇 時 間 為 20 ， 實 際 時 間 0.5*20 為 10 秒
        // 將 鬼 的 類 型 改 為 大 寫
        for (int i = 0; i < ghostNum; i++) {
            ghostType[i] = toupper(ghostType[i]);
        }
    }
    // 減 少 驚 嚇 時 間
    if (scaredMode) {
        scaredTime--;
        if (scaredTime <= 0) {
            scaredMode = false;  // 停 止 驚 嚇 模 式
            // 將 鬼 的 類 型 改 回 小 寫
            for (int i = 0; i < ghostNum; i++) {
                ghostType[i] = tolower(ghostType[i]);
            }
        }
    }
    display[playerPos[0]][playerPos[1]] = '0';  // pacman 位 置
    for (int i = 0; i < ghostNum; i++) {
        // 如 果 剩 下 3 秒 ， 改 變 鬼 的 顏 色
        if (scaredMode && scaredTime <= 6) {
            display[ghostPos[i][0]][ghostPos[i][1]] = 'G';  // 在 驚 嚇 模 式 下 用 'G' 表 示 鬼
        } else {
            display[ghostPos[i][0]][ghostPos[i][1]] = ghostType[i];  // 正 常 鬼 顯 示
        }
    }
}

#ifndef __VScode
BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
        gameRunning = false;
        return TRUE;
    }
    return FALSE;
}
#endif
