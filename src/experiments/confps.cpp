#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <windows.h>
#include "sparrow_types.h"

#pragma comment(lib, "user32.lib")

using namespace std;

u32 ScreenWidth = 120;
u32 ScreenHeight = 30;

f32 PlayerX = 8.0f;
f32 PlayerY = 8.0f;
f32 PlayerA = 0.0f;

u32 MapHeight = 16;
u32 MapWidth = 16;

f32 FOV = 3.145927 / 4.0;
f32 Depth = 16;

int
main()
{
    // Create Screen Buffer
    u16* Buffer = new u16[ScreenWidth * ScreenHeight];
    HANDLE Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, 0,
                                               CONSOLE_TEXTMODE_BUFFER, 0);
    SetConsoleActiveScreenBuffer(Console);
    DWORD BytesWritten = 0;

    wstring map;
    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#......#########";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#.....##.......#";
    map += L"#.....##.......#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"######..####...#";
    map += L"#..........#...#";
    map += L"#..........#...#";
    map += L"#..........#...#";
    map += L"################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    u16 WallShade[] = {
        ' ',
        0x2591,
        0x2592,
        0x2593,
        0x2588};
    u16 FloorShade[] = {
        ' ',
        '.',
        '-',
        'x',
        '#',
    };

    // Game Loop
    while (1) {
        tp2 = chrono::system_clock::now();
        chrono::duration<f32> elapsed = tp2 - tp1;
        tp1 = tp2;
        f32 ElapsedTime = elapsed.count();

        // Controls
        f32 TurnSpeed = 1.2f;
        f32 MoveSpeed = 5.0f;

        if (GetAsyncKeyState((u16)'A') & 0x8000)
            PlayerA -= TurnSpeed * ElapsedTime;
        if (GetAsyncKeyState((u16)'D') & 0x8000)
            PlayerA += TurnSpeed * ElapsedTime;
        if (GetAsyncKeyState((u16)'W') & 0x8000) {
            PlayerX += sinf(PlayerA) * MoveSpeed * ElapsedTime;
            PlayerY += cosf(PlayerA) * MoveSpeed * ElapsedTime;
            // Check if new location is valid
            if (map[(u32)PlayerY * MapWidth + (u32)PlayerX] == '#') {
                PlayerX -= sinf(PlayerA) * MoveSpeed * ElapsedTime;
                PlayerY -= cosf(PlayerA) * MoveSpeed * ElapsedTime;
            }
        }
        if (GetAsyncKeyState((u16)'S') & 0x8000) {
            PlayerX -= sinf(PlayerA) * MoveSpeed * ElapsedTime;
            PlayerY -= cosf(PlayerA) * MoveSpeed * ElapsedTime;
            // Check if new location is valid
            if (map[(u32)PlayerY * MapWidth + (u32)PlayerX] == '#') {
                PlayerX += sinf(PlayerA) * MoveSpeed * ElapsedTime;
                PlayerY += cosf(PlayerA) * MoveSpeed * ElapsedTime;
            }
        }

        // Horizontal pass
        for (u32 x = 0;
             x < ScreenWidth;
             ++x) {

            // raytracing to determine wall distance
            f32 RayAngle = (PlayerA - FOV / 2.0f) + ((f32)x / (f32)ScreenWidth) * FOV;

            f32 DistanceToWall = 0;
            b32 WallHit = false;
            b32 WallBoundary = false;

            f32 EyeX = sinf(RayAngle); // Normalize ray to player space
            f32 EyeY = cosf(RayAngle);

            while (!WallHit && DistanceToWall < Depth) {
                DistanceToWall += 0.1f;

                s32 TestX = (s32)(PlayerX + EyeX * DistanceToWall);
                s32 TestY = (s32)(PlayerY + EyeY * DistanceToWall);

                if (TestX < 0 || TestX >= (s32)MapWidth ||
                    TestY < 0 || TestY >= (s32)MapHeight) {
                    // Ray is out of bounds, set distance to max depth
                    WallHit = true;
                    DistanceToWall = Depth;
                }
                else if (map[TestY * MapWidth + TestX] == '#') {
                    WallHit = true;

                    vector<pair<float, float>> p; // Vector Length & Dot Product
                    for (u32 tx = 0; tx < 2; ++tx)
                        for (u32 ty = 0; ty < 2; ++ty) {
                            f32 vx = (f32)(TestX + tx) - PlayerX;
                            f32 vy = (f32)(TestY + ty) - PlayerY;
                            f32 Length = sqrt(vx * vx + vy * vy);
                            f32 Dot = (EyeX * vx / Length) + (EyeY * vy / Length);
                            p.push_back(make_pair(Length, Dot));
                        }
                    // Sort Pairs from closest to farthest
                    sort(p.begin(), p.end(),
                         [](const pair<float, float>& left, const pair<float, float>& right) { return left.first < right.first; });
                    f32 Bound = 0.005f;
                    if (acos(p.at(0).second) < Bound) WallBoundary = true;
                    if (acos(p.at(1).second) < Bound) WallBoundary = true;
                }
            }

            // Calculate distance to ceiling and floor
            Assert(DistanceToWall != 0);
            s32 Ceiling = (u32)((f32)(ScreenHeight / 2.0f) - ScreenHeight / DistanceToWall);
            s32 Floor = ScreenHeight - Ceiling;

            u32 ShadeIndex = 0;

            // vertical pass
            for (s32 y = 0;
                 y < (s32)ScreenHeight; ++y) {
                u16* Pixel = Buffer + (y * ScreenWidth + x);
                if (y < Ceiling) {
                    *Pixel = WallShade[0];
                }
                else if (y > Ceiling && y <= Floor) {
                    if (DistanceToWall <= Depth / 4.0f)
                        ShadeIndex = 4;
                    else if (DistanceToWall <= Depth / 3.0f)
                        ShadeIndex = 3;
                    else if (DistanceToWall <= Depth / 2.0f)
                        ShadeIndex = 2;
                    else if (DistanceToWall <= Depth)
                        ShadeIndex = 1;
                    else
                        ShadeIndex = 0;

                    if (WallBoundary && ShadeIndex > 0) {
                        ShadeIndex--;
                    }

                    *Pixel = WallShade[ShadeIndex];
                }
                else // Floor
                {
                    f32 NormalDistanceFromCamera = 1.0f - ((f32)(y - ScreenHeight / 2.0f) /
                                                           (f32)(ScreenHeight / 2.0f));
                    if (NormalDistanceFromCamera < 0.25f)
                        ShadeIndex = 4;
                    else if (NormalDistanceFromCamera < 0.5f)
                        ShadeIndex = 3;
                    else if (NormalDistanceFromCamera < 0.75f)
                        ShadeIndex = 2;
                    else if (NormalDistanceFromCamera < 0.9f)
                        ShadeIndex = 1;
                    else
                        ShadeIndex = 0;

                    *Pixel = FloorShade[ShadeIndex];
                }
            }
        }

        // GUI
        swprintf_s((wchar_t*)Buffer, 50, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%3.2f", PlayerX, PlayerY, PlayerA, 1.0f / ElapsedTime);
        for (u32 MapX = 0; MapX < MapWidth; ++MapX)
            for (u32 MapY = 0; MapY < MapHeight; ++MapY) {
                u32 Pos = (MapY + 1) * ScreenWidth + MapX;
                u16 UIPixel = 'P';
                if ((((u32)PlayerY + 1) * ScreenWidth + (u32)PlayerX) != Pos) {
                    UIPixel = map[MapY * MapWidth + MapX];
                }

                Buffer[Pos] = UIPixel;
            }

        // Buffer output
        Buffer[ScreenWidth * ScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacterW(Console, (LPCWSTR)Buffer, ScreenWidth * ScreenHeight, {0, 0}, &BytesWritten);
    }

    return 0;
}