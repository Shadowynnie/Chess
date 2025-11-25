#pragma once
#include <enet/enet.h>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

constexpr size_t CHANNELS = 2;
constexpr size_t MAX_CLIENTS = 2;
constexpr size_t DELAY_MS = 1000;
constexpr size_t DEFAULT_PING_INTERVAL_MS = 2000;

struct MoveMessage
{
    uint8_t fromX;      // 0..7
    uint8_t fromY;      // 0..7
    uint8_t toX;        // 0..7
    uint8_t toY;        // 0..7
    uint8_t promotion;  // 0 = none, 1=Q,2=R,3=B,4=N
    uint8_t flags;      // bitflags (e.g. castling, en-passant marker)
};

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
    std::vector<uint8_t> CreatePacketData(const MoveMessage& mvMsg); // Function to serialize packet data
    std::optional<MoveMessage> ParsePacketData(const uint8_t* data, size_t length); // Function to deserialize packet data
    bool HostGame(unsigned short port);
    bool ConnectToGame(const std::string& address, unsigned short port);
    void Disconnect();
    void ServiceNetwork();
    void StopService() { _isServiceRunning = false; }
    bool IsConnected() const;
    bool IsServiceRunning() const { return _isServiceRunning; }
    ENetHost* GetClientHost() const { return _client; }
    ENetHost* GetServerHost() const { return _server; }
    ENetPeer* GetPeer() const { return _peer; }

    // Testing functions
    void SendTestPacket(ENetPeer* peer);
    void SendMovePacket(ENetPeer* peer, const MoveMessage& mvMsg);

private:
    ENetHost* _client = nullptr;
    ENetHost* _server = nullptr;
    ENetPeer* _peer = nullptr;
    bool _isConnected = false;
    bool _isServiceRunning = false;
    // keep-alive
    std::chrono::steady_clock::time_point _lastPing{};
    int _pingIntervalMs{ DEFAULT_PING_INTERVAL_MS };
};