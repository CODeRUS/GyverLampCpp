#pragma once

class WifiServer
{
public:
    static void Initialize(const char *firstSetupName, const char *onDemandName = nullptr, const char *onDemandPassword = nullptr);
    static void Process();

    static bool isConnected();
};

