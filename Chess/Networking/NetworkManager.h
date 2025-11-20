
#pragma once
#include <enet/enet.h>
#include <cstdint>
#include <vector>
#include <optional>
#include <string>
#include <atomic>
#include <chrono>

using std::string;

constexpr size_t CHANNELS = 2; // number of ENet channels to use
constexpr int POLL_INTERVAL_MS = 50;    // poll ENet every 50 ms
constexpr int DEFAULT_PING_INTERVAL_MS = 2000; // send a ping every 2s when idle

// Message kinds
enum class MessageType : uint8_t
{
    MOVE = 1,       // small binary move message
    FULL_FEN = 2,    // full-board sync using FEN string (text)
    TURN_INFO = 3,   // whose turn it is
    ACK = 4,
    PING = 5
};

// Small fixed-size move message (all single-byte fields => no hton/ntoh required)
struct MoveMessage
{
    uint8_t fromX;      // 0..7
    uint8_t fromY;      // 0..7
    uint8_t toX;        // 0..7
    uint8_t toY;        // 0..7
    uint8_t promotion;  // 0 = none, 1=Q,2=R,3=B,4=N
    uint8_t flags;      // bitflags (e.g. castling, en-passant marker)
};

class NetworkManager
{
public:
    NetworkManager() = default;
    ~NetworkManager();

    // Lifecycle
    bool InitializeHostAsServer(uint16_t port);
    bool InitializeHostAsClient(const char* host, uint16_t port);

    // Helpers for messages
    static std::vector<uint8_t> SerializeMove(const MoveMessage& m);
    static std::optional<MoveMessage> DeserializeMove(const uint8_t* data, size_t len);

    // Send helpers
    static bool SendMoveReliable(ENetPeer* peer, const MoveMessage& m);
    static bool SendTextReliable(ENetPeer* peer, const string& text, MessageType type = MessageType::FULL_FEN);

    // Event pump (call from your server thread / client thread)
    void ServiceLoop();          // runs enet_host_service loop (blocking/poll)
    void StopServiceLoop();

    // Simple accessors
    ENetHost* Host() const { return m_host; }
    ENetPeer* GetPeer() const { return m_peer; }     // last connected peer (useful for client)
    bool IsConnected() const { return m_connected.load(); }

private:
    ENetHost* m_host{ nullptr };
    ENetPeer* m_peer{ nullptr };   // store peer on connect event
    std::atomic<bool> m_running{ false };
    std::atomic<bool> m_connected{ false };

    // keep-alive
    std::chrono::steady_clock::time_point m_lastPing{};
    int m_pingIntervalMs{ DEFAULT_PING_INTERVAL_MS };
};