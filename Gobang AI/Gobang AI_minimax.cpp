#include <cstring>
#include <queue>
#include <fstream>
#include <iostream>
#include <thread>
#include <windows.h>
#define level_1 1000000000 // 5 stone
#define level_2 10000000   // alive-4 or more than one level_3
#define level_3 100000     // dead-4 or alive-3 or more than one level_4
#define level_4 1000       // dead-3 or alive-2 or more than one level_5
#define level_5 10         // dead-2 or alive-1
using namespace std;
bool first;
int mv, table[17][17];
const char stateName[] = "state_21.txt";
const char moveName[]  = "move_21.txt";
inline bool legal(int &x, int &y)
{
    if(x < 0 || y < 0 || x > 16 || y > 16 || y-x > 8 || x-y > 8) return false;
    return true;
}
class step
{
public:
    int x, y, score;
};
class plane
{
public:
    int point[17][17];
    vector <step> next;

    inline int conclude(int *cnt, bool *collide)
    {
        int score = 0;
        for(int i = 0 ; i < 3 ; i++)
        {
            if(cnt[i] + cnt[i+3] >= 4) return level_1;
            if(collide[i] && collide[i+3]) continue;
            if(collide[i] || collide[i+3])
            {
                if(cnt[i] + cnt[i+3] == 3) score += level_3;
                else if(cnt[i] + cnt[i+3] == 2) score += level_4;
                else if(cnt[i] || cnt[i+3]) score += level_5;
            }
            else
            {
                if(cnt[i] + cnt[i+3] == 3) score += level_2;
                else if(cnt[i] + cnt[i+3] == 2) score += level_3;
                else if(cnt[i] || cnt[i+3]) score += level_4;
                else score += level_5;
            }
        }
        if(score >= level_2) return level_2;
        if(score >= (level_3 << 1)) return level_2;
        if(score >= level_3) return level_3;
        if(score >= (level_4) << 1) return level_3;
        if(score >= level_4) return level_4;
        if(score >= (level_5) << 1) return level_4;
        if(score >= level_5) return level_5;
        return 0;
    }
    void cnt_score()
    {
        step tmp;
        next.clear();
        int cnt[2][6], x, y;
        bool collide[2][6];
        for(int i = 0 ; i < 17 ; i++)
            for(int j = 0 ; j < 17 ; j++)
            {
                if(!legal(i, j) || point[i][j]) continue;
                memset(cnt, 0, sizeof(cnt));
                memset(collide, 0, sizeof(collide));
                // count the six direction
                x = i; y = j;
                while(legal(--x, --y) && point[x][y])
                {
                    if((point[x][y]&1) == (point[i-1][j-1]&1))
                    {
                        cnt[point[x][y]&1][0]++;
                        collide[1-(point[x][y]&1)][0] = 1;
                    }
                    else {collide[point[x][y]&1][0] = 1; break;}
                }
                x = i; y = j;
                while(legal(--x, y) && point[x][y])
                {
                    if((point[x][y]&1) == (point[i-1][j]&1))
                    {
                        cnt[point[x][y]&1][1]++;
                        collide[1-(point[x][y]&1)][1] = 1;
                    }
                    else {collide[point[x][y]&1][1] = 1; break;}
                }
                x = i; y = j;
                while(legal(x, ++y) && point[x][y])
                {
                    if((point[x][y]&1) == (point[i][j+1]&1))
                    {
                        cnt[point[x][y]&1][2]++;
                        collide[1-(point[x][y]&1)][2] = 1;
                    }
                    else {collide[point[x][y]&1][2] = 1; break;}
                }
                x = i; y = j;
                while(legal(++x, ++y) && point[x][y])
                {
                    if((point[x][y]&1) == (point[i+1][j+1]&1))
                    {
                        cnt[point[x][y]&1][3]++;
                        collide[1-(point[x][y]&1)][3] = 1;
                    }
                    else {collide[point[x][y]&1][3] = 1; break;}
                }
                x = i; y = j;
                while(legal(++x, y) && point[x][y])
                {
                    if((point[x][y]&1) == (point[i+1][j]&1))
                    {
                        cnt[point[x][y]&1][4]++;
                        collide[1-(point[x][y]&1)][4] = 1;
                    }
                    else {collide[point[x][y]&1][4] = 1; break;}
                }
                x = i; y = j;
                while(legal(x, --y) && point[x][y])
                {
                    if((point[x][y]&1) == (point[i][j-1]&1))
                    {
                        cnt[point[x][y]&1][5]++;
                        collide[1-(point[x][y]&1)][5] = 1;
                    }
                    else {collide[point[x][y]&1][5] = 1; break;}
                }
                // conclude
                tmp.score = max(conclude(cnt[first], collide[first]), conclude(cnt[1-first], collide[1-first])-1);
                tmp.x = i; tmp.y = j;
                if(!next.size() || next[0].score == tmp.score) next.push_back(tmp);
                else if(tmp.score > next[0].score)
                {
                    next.clear();
                    next.push_back(tmp);
                }
            }
    }
}now;
step max_value(plane status, int a, int b, int d);
step min_value(plane status, int a, int b, int d);
step max_value(plane status, int a, int b, int d)
{
    status.cnt_score();
    if(d == 3)
        return status.next[0];
    if(status.next.size() > 1)
    {
        step ans, tmp;
        int x, y;
        ans.score = -1;
        for(int i = 0 ; i < status.next.size() ; i++)
        {
            x = status.next[i].x;
            y = status.next[i].y;
            status.point[x][y] = 2-first;
            tmp = min_value(status, a, b,d+1);
            tmp.x = x;
            tmp.y = y;
            if(tmp.score > b)
            {
                status.point[x][y] = 0;
                return tmp;
            }
            if(tmp.score > ans.score) ans = tmp;
            a = max(a, ans.score);
            status.point[x][y] = 0;
        }
        return ans;
    }
    else return status.next[0];
}
step min_value(plane status, int a, int b, int d)
{
    status.cnt_score();
    if(d == 3) return status.next[0];
    if(status.next.size() > 1)
    {
        step ans, tmp;
        int x, y;
        ans.score = 0x7FFFFFFF;
        for(int i = 0 ; i < status.next.size() ; i++)
        {
            x = status.next[i].x;
            y = status.next[i].y;
            status.point[x][y] = 1+first;
            tmp = max_value(status, a, b,d+1);
            tmp.x = x;
            tmp.y = y;
            if(tmp.score <= a) return tmp;
            if(tmp.score < ans.score) ans = tmp;
            b = min(b, ans.score);
        }
        return ans;
    }
    else return status.next[0];
}
int tryToGetState() {
    ifstream file(stateName);
    if (!file.is_open()) return 0;
    int move;
    if (!(file >> move)) return 0;
    if (move <= 0) return -1;
    if (move <= 2) first = (move&1) == 1;
    else if (move != mv + 2) return 0;
    for(int i = 0 ; i < 17 ; i++)
    {
        for(int j = 0 ; j < 17 ; j++)
        {
            if(legal(i, j))
            {
                file >> now.point[i][j];
                if(!file) return 0;
                //cout << now.point[i][j];
            }

        }
        //cout << "\n";
    }
    return move;
}
int main()
{
    int move = 0;
    step next;
    for(int i = 0 ; i < 17 ; i++) for(int j = 0 ; j < 17 ; j++) if(legal(i, j))table[i][j] = move++;
    while ((move = tryToGetState()) != -1) {
        if (move == 0) {
            // wait for opponent
            //this_thread::yield();
            continue;
        }
        if(move == 1)
        {
            next.x = 8;
            next.y = 8;
        }
        else next = max_value(now, 0, 0x7FFFFFFF, 0);
        ofstream f(moveName);
        //cout << next.x << " " << next.y << endl;
        f << move << ' ' << table[next.x][next.y] << std::endl;
        f.close();
        now.next.clear();
        mv = move;
    }
    system("PAUSE");
    return 0;
}
