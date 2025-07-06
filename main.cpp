#include <bits/stdc++.h>
#include <windows.h>
#define HEIGHT 15
#define WIDTH  66   // 定 義 遊 戲 區 域 的 高 度 和 寬 度
#define powerNum 4
#define ghostNum 4  // 能 量 球 和 鬼 的 數 量
#include "pacman.hpp"
//#define __VScode // 若 使 用 VScode 請 取 消 註 解 
using namespace std;

int dir[5][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {0, 0}};  // d,s,a,w,stop
bool gameRunning = true;



// 檢 查 是 否 與 牆 壁 碰 撞
bool checkNotCollideWall(string grid[], int i, int j) {
    int n = HEIGHT, m = WIDTH;
    if (/*question 1*/)
        return false;  // 如 果 超 出 邊 界 或 碰 到 牆 壁 ， 回 傳 false
    return true;  // 否 則 回 傳 true
}

// 檢 查 是 否 與 鬼 碰 撞 
bool checkCollideGhost(int (*ghostPos)[2], int playerPos[2]) {
    for (int i = 0; i < ghostNum; i++) {
        if (/*question 2*/) {
            return true;  // 如 果 pacman 與 鬼 的 座 標 相 同 ， 回 傳true
        }
    }
    return false;  // 否 則 回 傳 false
}

// 計 算 最 佳 方 向
int bestDir(string grid[], int curPos[2], int curDir, int target[2]) {
    int n = HEIGHT, m = WIDTH;

    int ans = 4;  // 預 設 為 停 止
    int minDis = INT16_MAX;  // 最 小 距 離 初 始 化 為 最 大 值
    for (int i = 0; i < 4; i++) {
        if (i == (curDir + 2) % 4)
            continue;  // 不 考 慮 反 方 向

        int newI = curPos[0] + dir[i][0]; // 新 的 i 座 標
        int newJ = curPos[1] + dir[i][1]; //新 的 j 座 標
        int dis = /*question 3*/ ;
        if (dis < minDis && checkNotCollideWall(grid, newI, newJ)) {
            ans = i;  // 更 新 最 佳 方 向
            minDis = dis;  // 更 新 最 小 距 離
        }
    }
    return ans;  // 回 傳 最 佳 方 向
}


// 移 動 鬼
template <int _N>
void moveGhost(string grid[], int (&ghostPos)[_N][2], int (&ghostDir)[_N],
                char ghostType[], int playerPos[2],
                int playerDir, bool scaredMode) {
    for (int i = 0; i < ghostNum; i++) {
        int legalRoute = 0;  // 合 法 路 徑 計 數
        int possibleDir[3];  // 存 儲 可 能 的 方 向
        for (int j = 0; j < 4; j++) {
            if ((checkNotCollideWall(grid, ghostPos[i][0] + dir[j][0],
                                      ghostPos[i][1] + dir[j][1])) &&
                (j != (ghostDir[i] + 2) % 4)) {
                possibleDir[legalRoute] = j;  // 存 儲 有 效 方 向
                legalRoute++;
            }
        }
        legalRoute++;

        if (!scaredMode) {  // 如 果 不 是 驚 嚇 模 式
            if (legalRoute == 1) {
                ghostDir[i] = (ghostDir[i] + 2) % 4;  // 反 向 移 動
            } else if (legalRoute == 2) {
                ghostDir[i] = possibleDir[0];  // 隨 機 選 擇 一 個 方 向
            } else if (legalRoute == 3 || legalRoute == 4) {
                int target[2] = {};
                if (ghostType[i] == 'a') {  // 紅 鬼 ：目 標 pacman 位 置
                    /*question 4*/
                } else if (ghostType[i] == 'b') {  // 粉 紅 鬼 ：目 標 pacman 位 置 4 格 前
                    /*question 5*/
                } else if (ghostType[i] == 'c') {  // 淺 藍 鬼 ：目 標 與 紅 色 鬼 對 稱 中 心 在 pacman 前 方 2 格
                    int indexOfRed = 0;
                    for (int j = 0; j < ghostNum; j++) {
                        if (ghostType[j] == 'a') {
                            indexOfRed = j;
                            break;
                        }
                    }
                    int center[2];
                    center[0] = playerPos[0] + dir[playerDir][0] * 2;
                    center[1] = playerPos[1] + dir[playerDir][1] * 2;
                    /*question 6*/
                } else if (ghostType[i] == 'd') {  // 橙 鬼 ：保 持 4 格 距 離
                    double a, b;
                    a = ghostPos[i][0] - playerPos[0];
                    b = ghostPos[i][1] - playerPos[1];
                    target[0] = round(playerPos[0] + a * 4 / sqrt(a * a + b * b));
                    target[1] = round(playerPos[1] + b * 4 / sqrt(a * a + b * b));
                }
                ghostDir[i] = bestDir(grid, ghostPos[i], ghostDir[i], target);  // 計 算 最 佳 方 向
            }
        } else {  // 如 果 是 驚 嚇 模 式
            if (isupper(ghostType[i])) {
                if (legalRoute == 1) {
                    ghostDir[i] = (ghostDir[i] + 2) % 4;  // 反 向 移 動
                } else if (legalRoute == 2) {  // 不 回 頭
                    ghostDir[i] = possibleDir[0];
                } else if (legalRoute == 3 || legalRoute == 4) {  // 不 回 頭
                    ghostDir[i] = possibleDir[rand() % (legalRoute - 1)];
                }
            }
            if (islower(ghostType[i])) {  // 復 活 先 不 動
                ghostDir[i] = 4;
            }
        }
        if (checkNotCollideWall(grid, ghostPos[i][0] + dir[ghostDir[i]][0],
                                 ghostPos[i][1] + dir[ghostDir[i]][1])) {
            ghostPos[i][0] += dir[ghostDir[i]][0];  // 根 據 方 向 移 動
            ghostPos[i][1] += dir[ghostDir[i]][1];
        }
    }
}

// 吃 掉 鬼
void eatGhost(string grid[], int playerPos[2], int (*ghostPos)[2],
               int (&ghostDir)[ghostNum], char (&ghostType)[ghostNum],
               int &score) {
    for (int i = 0; i < ghostNum; i++) {
        if (ghostPos[i][0] == playerPos[0] && ghostPos[i][1] == playerPos[1]) {
            score += 200;  // 吃 掉 鬼 的 得 分
            /*question 7*/
            ghostDir[i] = 4;  // 設 置 鬼 的 方 向 為 停 止
            ghostType[i] = tolower(ghostType[i]);  // 將 鬼 的 類 型 改 為 小 寫
        }
    }
}

int main() {
    srand(time(0));  // 設 置 隨 機 數 種 子
    
#ifndef __VScode
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    system("chcp 65001");
#endif
    string initGrid[HEIGHT] = {
        "#################################################################",
        "#...............................................................#",
        "#..###...#...##...##.....#####....#......#...#######...#######..#",
        "#..#.#...#....##.##.....##...##...#......#...#.........#........#",
        "#..#.###.#.....###......#.........#......#...#######...#######..#",
        "#..#...#.#......#.......##...##...##....##...#.........#........#",
        "#..#...###......#........#####.....######....#######...#######..#",
        "#...............................0...............................#",
        "#...#####.....#####....##....##...########...#######...#######..#",
        "#..##...##...##   ##...###..###...#      #.........#...#........#",
        "#..#.........#######...#.####.#...########...#######...#######..#",
        "#..##...##...#.....#...#..##..#...#..........#...............#..#",
        "#...#####....#.....#...#......#...#..........#######...#######..#",
        "#...............................................................#",
        "#################################################################"};
    int pointsCnt = 0;

    int playerPos[2];  // pacman 位 置
    int playerDir = 3;  // 初 始 方 向 為 'w'

    int ghostPos[ghostNum][2];  // 鬼 的 位 置
    int ghostDir[ghostNum] = {4, 4, 4, 4};  // 鬼 的 方 向
    char ghostType[ghostNum];  // 鬼 的 類 型

    int score = 0;  // 分 數
    int scaredTime = 0;  // 驚 嚇 時 間
    bool scaredMode = false;  // 是 否 在 驚 嚇 模 式

    string pointsGrid[HEIGHT];  // 點 數 網 格
    string gridDisplay[HEIGHT];  // 顯 示 網 格
    string prevGridDisplay[HEIGHT];  // 前 一 個 顯 示 網 格

    for (int i = 0; i < HEIGHT; i++) {
        string row = "";
        row.append(WIDTH, ' ');
        pointsGrid[i] = row;  // 初 始 化 點 數 網 格
        gridDisplay[i] = row;  // 初 始 化 顯 示 網 格
        prevGridDisplay[i] = row;  // 初 始 化 前 一 個 顯 示 網 格
    }

    for (int i = 0; i < HEIGHT; i++) {  // 設 定 pacman 位 置 的 初 始 狀 態
        for (int j = 0; j < WIDTH; j++) {
            char c = initGrid[i][j];
            if (c == '0') {
                /*question 8*/
            }
        }
    }

    // 隨 機 放 置 能 量 球
    for (int p = 0; p < powerNum; p++) {
        int x, y;
        x = rand() % HEIGHT;  // 隨 機 列
        y = rand() % WIDTH;  // 隨 機 行
        while (/*question 9*/){ // 確 保 能 量 球 放 原 本 是 點 數 的 位 置 上
            x = rand() % HEIGHT;  // 隨 機 列
            y = rand() % WIDTH;  // 隨 機 行
        }
        initGrid[x][y] = '*';  // 放 置 能 量 球
    }

    // 隨 機 放 置 鬼
    for (int i = 0; i < ghostNum; i++) {
        int x, y;
        do {
            x = rand() % HEIGHT;  // 隨 機 列
            y = rand() % WIDTH;  // 隨 機 行
        } while (!(initGrid[x][y] == '.' &&
                    ((x - playerPos[0]) * (x - playerPos[0]) +
                     (y - playerPos[1]) * (y - playerPos[1])) >= 49));
        // 確 保 放 置 在 合 法 位 置 且 距 離 pacman 有 一 定 距 離
        ghostPos[i][0] = x;  // 設 置 鬼 的 行
        ghostPos[i][1] = y;  // 設 置 鬼 的 列
        ghostType[i] = 'a' + i;  // 分 配 鬼 的 類 型 a, b, c, d
    }

    for (/*question 10-1*/) {  // 設 定 點 數 網 格 的 初 始 狀 態
        for (/*question 10-2*/) {
            char c = initGrid[i][j];
            gridDisplay[i][j] = c;  // 初 始 化 顯 示 網 格
            if (c == '.' || c == '*') {
                pointsGrid[i][j] = c;  // 設 置 點 數 網 格
                pointsCnt++;  // 增 加 點 數 計 數
            }
        }
    }

    KeyManager keyM;  // 創 建 鍵 盤 管 理 器
    int curDir = playerDir;  // 當 前 方 向
    bool gameOver = false;  // 遊 戲 是 否 結 束

    cout << "\x1B[2J\x1B[H" << "\033[?25l";  // 初 始 化 螢 幕 ， 清 除 並 隱 藏 鼠 標
    update(initGrid, playerPos, ghostPos, ghostDir, ghostType, pointsGrid, pointsCnt, gridDisplay, score, scaredMode, scaredTime);
    gridPrint(gridDisplay, prevGridDisplay);
    cout << "\033[" << (HEIGHT + 1) << ";1H\033[K";
    cout << "press any key to start game (use arrows or WASD to move pacman)" << endl;
    keyM.anyKeyToContinue();
    for (int i = 3; i > 0; i--) {
        cout << "\033[" << (HEIGHT + 1) << ";1H\033[K";
        cout << "starting in " << i << "........." << endl;
        Sleep(1000);
    }
   
    while (!gameOver && gameRunning) {
        // 移 動 pacman
        movePacman(initGrid, playerPos, playerDir, keyM);  
        // 檢 查 是 否 與 鬼 碰 撞
        if (checkCollideGhost(ghostPos, playerPos)) {  
            if (scaredMode) 
                eatGhost(initGrid, playerPos, ghostPos, ghostDir, ghostType, score);  // 吃 掉 鬼
            else 
                gameOver = true;  // 如 果 不 是 驚 嚇 模 式 ， 遊 戲 結 束
        }
        // 移 動 鬼
        if (!(scaredMode && scaredTime % 2)) // 如 果 是 驚 嚇 模 式 只 有 一 半 的 時 間 會 移 動 鬼
            moveGhost(initGrid, ghostPos, ghostDir, ghostType, playerPos, playerDir, scaredMode);  
        // 再 次 檢 查 是 否 與 鬼 碰 撞
        if (checkCollideGhost(ghostPos, playerPos)) {  
            if (scaredMode) 
                eatGhost(initGrid, playerPos, ghostPos, ghostDir, ghostType, score);  // 吃 掉 鬼
            else 
                gameOver = true;  // 如 果 不 是 驚 嚇 模 式 ， 遊 戲 結 束
        }
        // 更 新 遊 戲 狀 態
        update(initGrid, playerPos, ghostPos, ghostDir, ghostType, pointsGrid, pointsCnt, gridDisplay, score, scaredMode, scaredTime);  
        // 輸 出 遊 戲 網 格
        gridPrint(gridDisplay, prevGridDisplay);  

        cout << "\033[" << (HEIGHT + 1) << ";1H\033[K";  // 移 動 鼠 標 到 顯 示 分 數 的 位 置
        cout << "Score: " << score << endl;  // 顯 示 分 數
        Sleep(500);  // 暫 停 0.5 秒
        if (pointsCnt == 0) gameOver = true;  // 如 果 所 有 點 數 都 被 吃 掉 ， 遊 戲 結 束
    }

    // 遊 戲 結 束
    cout << "\033[?25h" << "\033[" << (HEIGHT + 2) << ";1H";  // 恢 復 鼠 標 顯 示

    if (pointsCnt == 0) {
        cout << "You win! Final score: " << score << endl;  // 如 果 贏 了 ， 顯 示 勝 利 信 息
    } else {
        cout << "Game Over! Final score: " << score << endl;  // 如 果 遊 戲 結 束 ， 顯 示 結 束 信 息
    }
    return 0;  // 回 傳 0 ， 結 束 程 序
}
