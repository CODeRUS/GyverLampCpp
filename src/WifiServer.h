#pragma once

class WifiServer
{
public:
    static void Initialize(const char *firstSetupName);
    static void Process();
    static void StartPortal(const char *onDemandName, const char *onDemandPassword);

    static bool isConnected();
};

