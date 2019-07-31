#pragma once

class UpdateWebServer
{
public:
    static void Initialize(int port = 8080);
    static void Process();

    static bool IsUpdating();
};
