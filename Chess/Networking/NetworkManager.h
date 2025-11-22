#pragma once
#include <enet/enet.h>
#include <string>
#include <vector>

constexpr size_t CHANNELS = 2;
constexpr size_t MAX_CLIENTS = 2;
constexpr size_t DELAY_MS = 1000;
constexpr size_t DEFAULT_PING_INTERVAL_MS = 2000;

enum class MessageType : uint8_t
{
    MOVE,
    TURN_INFO,
    BOARD_STATE,
    ACKNOWLEDGEMENT,
    TEST_PACKET,
    PING
};


class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager();
    bool Initialize();
    void Deinitialize();
    std::vector<uint8_t> CreatePacketData(MessageType type, const std::vector<uint8_t>& payload); // Function to serialize packet data
    bool HostGame(unsigned short port);
    bool ConnectToGame(const std::string& address, unsigned short port);
    void Disconnect();
    void ServiceNetwork();
    bool IsConnected() const;
    ENetHost* GetClientHost() const { return _client; }
    ENetHost* GetServerHost() const { return _server; }
    ENetPeer* GetPeer() const { return _peer; }

    // Testing functions
    void sendTestPacket(ENetPeer* peer);

private:
    ENetHost* _client = nullptr;
    ENetHost* _server = nullptr;
    ENetPeer* _peer = nullptr;
    bool _isConnected = false;
};